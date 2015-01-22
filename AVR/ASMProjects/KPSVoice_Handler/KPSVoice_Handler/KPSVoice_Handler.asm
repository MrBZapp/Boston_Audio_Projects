; KPSVoice_Handler.asm
;	Created: 4/3/2013 4:11:20 PM
;   Author: Matt
;	==============================
;	VOICE HANDLER
;	==============================
;	The following program is responsible for reading the data from the keyboard, performing an "oldest" voice 
;	stealing function, and delivering note on/off data to the various clocks.
;
;	WIP:
;	Not-insignificant refactoring of of the Keyboard polling scheme will need to happen because Ports D and B are not as flexible as I once thought. 
;	2 lines need to be dedicated in Port B for the Multi-core voice clock system, and Port A needs to be dedicated entirely to managing the external 20MHz clock.
;	2 lines need to be dedicated in Port D for eventual USART expansion for MIDI capabilities.
;	Now that the polling capabilites are separated between the two ports, optimizing for polling speed will be a bit of a challenge.
;	Consider using more memory to poll faster, and be mindful of how quickly the PSS-470 is actually polling.
;
;	Certain variables may not be all that useful. For example, the use of the working registers in subroutines could potentially eliminated
;	the need to use the Frame define, and potentially the GeneriCount define.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.INCLUDE "tn2313Adef.inc" ;ATtiny2313A header file
;	==============================
; 	PIN DESCRIPTION	; OPPOSITE I/O DESIGNATION FROM PIC.  KEEP THAT IN MIND. 0 = input 1= output
;	==============================
;	PORTB I/O: (1010 0000)
	.EQU	SCL			=	PINB7	;OUTPUT	7.) Multi-core Serial Clock
	.EQU	SDA			=	PINB5	;OUTPUT	5.)	Multi-core Serial Data
;	Inputs 0,1,2,3 for keyboard polling lines

;	PORTD I/O:  (000 0110)
	.EQU	RXD			=	PIND0	;INPUT	0.)	USART Recieve
	.EQU	TXD			=	PIND1	;OUTPUT	1.)	USART Transmit
	.EQU	KBMUX		=	PIND2	;OUTPUT	2.)	KB polling MUX (Low = read Bank#, High = read Bank)
;	Inputs 3,4,5,6 for keyboard polling lines

;	========================================================
;	PROGRAM MEMORY VARIABLE DEFINITION
;	========================================================
	.DEF	ZPNTL		=	R0	;Reserve for the Z pointer return (LOW)
	.DEF	ZPNTH		=	R1	;Reserve for the Z pointer return (HIGH)
	.DEF	LI0			=	R2	;Reserve for List items
	.DEF	LI1			=	R3
	.DEF	LI2			=	R4
	.DEF	LI3			=	R5
	.DEF	LI4			=	R6
	.DEF	LI5			=	R7
	.DEF	LI6			=	R8
	.DEF	LI7			=	R9
	.DEF	NoteBank	=	R10	;A Variable for storing the active bank of notes as an integer
	.DEF	TestNotes	=	R11	;B Variable for storing the active notes in a bank
	.DEF	ActiveKeys	=	R12	;C Variable for storing the ActiveKeys list entry for comparison
	.DEF	BondedValue	=	R13	;D Variable for storing the active note and bank as an pair of nybles
	.DEF	VoiceNumber =	R14	;E Variable for storing the chosen Voice Numeber
	.DEF	BitAdvance	=	R15	;Used to kick the USI into sending/reciving a bit
	.DEF	WorkingReg1	=	R16	;10 General purpose working register
	.DEF	WorkingReg2	=	R17	;11 General purpose working register
	.DEF	WorkingReg3	=	R18
	.DEF	GeneriCount	=	R17	;12 General purpose counter
	.DEF	Pointer		=	R18	;13 Pointer for various test applications
	.DEF	ChangeLog	=	R19	;14	Stores the changes to be made to the Active Keys Register from the new notes register
	.DEF	Frame		=	R20 ;Used to store the frame 
	//Last voice definitions//
	.DEF	LastVoice	=	R7	;set your voice stealing issues (eg R9 = LI7 = 8 voices)
	.EQU	LstVceNum	=	0x05;

;	========================================================
;	SRAM VARIABLE DEFINITION
;	========================================================
	.EQU	ActiveKeyRegister	=	SRAM_START				;Reserve the first 6 registers for storing the active notes
	.EQU	ActiveNotesList		=	ActiveKeyRegister	+ 8	;Reserve the next 8 registers for a list of active voices
	.EQU	ActiveVoiceList		=	ActiveNotesList		+ 8	;Reserve the next 8 registers for marking voices as active
	.EQU	AvailableVoices		=	ActiveVoiceList		+ 8	;Reserve the next 8 registers for finding an available voice

;	========================================================
;	PROGRAM START
;	========================================================
.CSEG
.ORG	0x0000
		RJMP MCU_INIT ; Reset Handler
		RETI ; rjmp INT0 ; External Interrupt0 Handler
		RETI ; rjmp INT1 ; External Interrupt1 Handler
		RETI ; rjmp TIM1_CAPT ; Timer1 Capture Handler
		RETI ; rjmp TIM1_COMPA ; Timer1 CompareA Handler
		RETI ; rjmp TIM1_OVF ; Timer1 Overflow Handler
		RETI ; rjmp TIM0_OVF ; Timer0 Overflow Handler
		RETI ; rjmp USART0_RXC ; USART0 RX Complete Handler
		RETI ; rjmp USART0_DRE ; USART0,UDR Empty Handler
		RETI ; rjmp USART0_TXC ; USART0 TX Complete Handler
		RETI ; rjmp ANA_COMP ; Analog Comparator Handler
		RETI ; rjmp PCINT ; Pin Change Interrupt
		RETI ; rjmp TIMER1_COMPB ; Timer1 Compare B Handler
		RETI ; rjmp TIMER0_COMPA ; Timer0 Compare A Handler
		RETI ; rjmp TIMER0_COMPB ; Timer0 Compare B Handler
		RETI ; rjmp USI_START ; USI Start Handler
		RETI ; rjmp USI_OVERFLOW ; USI Overflow Handler
		RETI ; rjmp EE_READY ; EEPROM Ready Handler
		RETI ; RJMP WDT_OVERFLOW ; Watchdog Overflow Handler

;	========================================================
;						MCU INITIALIZATION
;	========================================================
MCU_INIT:	//Watchdog Timer Disable//
			CLI												; Turn off global interrupt 
			WDR												; Reset Watchdog Timer
			IN		WorkingReg1,	MCUSR
			ANDI	WorkingReg1,	(0xff & (0<<WDRF))
			OUT		MCUSR,			WorkingReg1				;Clear WD Reset Flag in MCUSR
			IN		WorkingReg1,	WDTCSR	
			ORI		WorkingReg1,	(1<<WDCE) | (1<<WDE)
			OUT		WDTCSR,			WorkingReg1				;Write logical one to WDCE and WDE Keep old prescaler setting to prevent unintentional time-out
			LDI		WorkingReg1,	(0<<WDE)
			OUT		WDTCSR,			WorkingReg1				;Turn off WDT
	
			//SETUP I/O//
			LDI		WorkingReg1,	0b10000000
			OUT		MCUCR,			WorkingReg1				;Pull-up resistor disable,Disable interrupts, disable sleep	
			LDI		WorkingReg1,	0b00000110
			OUT		DDRD,			WorkingReg1				;Set I/O for PortD as listed above.
			LDI		WorkingReg1,	0b10100000
			OUT		DDRB,			WorkingReg1				;Set I/O for PortB as listed above.

			//SETUP USI//
			LDI		WorkingReg1,		0b00101010			;(0<<USISIE)|(0<<USIOIE)|(1<<USIWM1)|(0<<USIWM0)|(1<<USICS1)|(0<<USICS0)|(1<<USICLK)
			MOV		BitAdvance,			WorkingReg1
			LDI		WorkingReg2,		0xFF
			OUT		USIDR,				WorkingReg2			;Set output to high
			OUT		USICR,				WorkingReg1			;Set Up the Universal Serial Interface. Disable Interrupts. Two-wire mode.  Software stobe as counter clock source.
			LDI		WorkingReg1,		0xF0
			OUT		USISR,				WorkingReg1			;Clear Flags, Reset Counter
			INC		BitAdvance								;Turn on the Toggle clock bit 
			
			//INITIALIZE SRAM//
			LDI		WorkingReg1,	0x01					;Initialize The Available Voices List
			STS		AvailableVoices,	WorkingReg1
			LDI		WorkingReg1,	0x02
			STS		AvailableVoices + 1,	WorkingReg1
			LDI		WorkingReg1,	0x03
			STS		AvailableVoices + 2 ,	WorkingReg1
			LDI		WorkingReg1,	0x04
			STS		AvailableVoices + 3,	WorkingReg1
			LDI		WorkingReg1,	0x05
			STS		AvailableVoices + 4,	WorkingReg1
			LDI		WorkingReg1,	0x06
			STS		AvailableVoices + 5,	WorkingReg1
/*			LDI		WorkingReg1,	0x07
			STS		AvailableVoices +6,	WorkingReg1
			LDI		WorkingReg1,	0x08
			STS		AvailableVoices +7,	WorkingReg1*/

			LDI		WorkingReg1,	LOW(RAMEND)				;Set the stack pointer to the end of the SRAM
			LDI		WorkingReg2,	HIGH(RAMEND)
			OUT		SPL,			WorkingReg1
			;OUT	SPH,			WorkingReg2			

;	========================================================
;					CORE EXECUTION START
;	========================================================
START:		LDI		Pointer,		0x01					;Give the generic pointer a value (1) to work with
POLLKB:		SBI		PORTD,			KBMUX					;Throw high the KBMUX pin to read which bank is active
				IN		WorkingReg1,	PORTB					
				IN		WorkingReg2,	PORTD					;Read the bank
				LSL		WorkingReg2								;Allign PORTB 3,4,5,6 to 4,5,6,7
				LDI		WorkingReg3,	0b00001111
				AND		WorkingReg1,	WorkingReg3
				SWAP	WorkingReg3
				AND		WorkingReg2,	WorkingReg3
				OR		WorkingReg1,	WorkingReg2				;Complete the 8-bit word of information
			CBI		PORTD,			KBMUX					;Throw the KBMUX pin low to prepair for reading the currently operational notes
			MOV		NoteBank,		WorkingReg1				;Move the previously read value into note bank register for comparison
				IN		WorkingReg1,	PORTB					
				IN		WorkingReg2,	PORTD					;Read the bank
				LSL		WorkingReg2								;Allign PORTB 3,4,5,6 to 4,5,6,7
				AND		WorkingReg2,	WorkingReg3
				SWAP	WorkingReg3
				AND		WorkingReg1,	WorkingReg3
				OR		WorkingReg1,	WorkingReg2				;Complete the 8-bit word of information
				MOV		TestNotes,		WorkingReg1
			SBI		PORTD,			KBMUX					;Throw high the KBMUX pin to make sure the bank hasn't changed
				IN		WorkingReg1,	PORTB					
				IN		WorkingReg2,	PORTD					;Read the bank
				LSL		WorkingReg2								;Allign PORTB 3,4,5,6 to 4,5,6,7
				AND		WorkingReg1,	WorkingReg3
				SWAP	WorkingReg3
				AND		WorkingReg2,	WorkingReg3
				OR		WorkingReg1,	WorkingReg2				;Complete the 8-bit word of information
					CPI	WorkingReg1,	0x00
					BREQ	POLLKB									;ERROR PROTECTION FOR IF THERE ARE NO BANKS ACTIVE
			SUB		NoteBank,		WorkingReg1				;subtract the two to make sure they're equal
			BREQ	CONV2DEC								;if they are, you can feel free to proceed with the program, 
															;the note bank number will stay in the Working register, this should also clear the Note Bank register for filling later
			RJMP	POLLKB									;if they aren't, give it another try

CONV2DEC:	MOV		WorkingReg2,	Pointer					;Move the pointer into the working register
			AND		WorkingReg2,	WorkingReg1	
			BRNE	PULACTVN								;if the result of the AND is a (1) this indicates a match. Branch to pull the active notes.
			LSL		Pointer									;move the pointer to the next bit position
			INC		NoteBank								;Add one to the Note bank number
			RJMP	CONV2DEC								;try again to find a match

PULACTVN:	MOV		WorkingReg1,	NoteBank				;Move the notebank into the working register
			LDI		WorkingReg2,	LOW(ActiveKeyRegister)
			ADD		WorkingReg1,	WorkingReg2				;Prepair the pointer
			LDI		XH,				HIGH(ActiveKeyRegister)
			MOV		XL,				WorkingReg1				;Load the value of the ActiveKeyRegister entry that needs to be pulled
			LD		ActiveKeys,		X						;Move that value into the ActiveKeys register
			MOV		WorkingReg1,	ActiveKeys				
				SUB		WorkingReg1,	TestNotes				;Subtract the two banks to see if they're different.  No need to waste time testing if they're the same
				BREQ	START									;Can go pull things again from the keyboard if nothing's changed
			CLR		BondedValue
			ADD		BondedValue,	NoteBank				;Add the integer to the Higher nybble of the Bonded Value.  This is saved for later
			SWAP	BondedValue
			LDI		Pointer,		0x01					;Make sure the Pointer is ready to do its jam
			LDI		GeneriCount,	0x08					;Load 8 into the counter to make sure it's ready to do its jam too
			MOV		ChangeLog,	ActiveKeys
			EOR		ChangeLog,	TestNotes

POLNOTES:	INC		BondedValue								;Keep track of what note you're testing
			MOV		WorkingReg2,	Pointer
			AND		WorkingReg2,	ChangeLog
			BREQ	NOCHANGE
			MOV		WorkingReg1,	Pointer					;Pointer (13) 
			AND		WorkingReg1,	ActiveKeys				;Mask position on active keys
			EOR		WorkingReg1,	Pointer					;Toggle that position.  0 equals turn it off
			BREQ	OLDNOTES								;This denotes turning the bit off
			RJMP	NEWNOTES
NOCHANGE:	LSL		Pointer
			DEC		GeneriCount
			BREQ	DONEPOL									;Go back to polling the Keyboard if there's nothing happening
			RJMP	POLNOTES
		
OLDNOTES:	RCALL	REMOVEVCE								;Call the "remove voice" subroutine
			MOV		Frame,			WorkingReg1				;Move the voice address into the frame	

			//Contact the voice//	
			RCALL	STARTCOND								;Call to generate a start condition			
			RCALL	SENDBYTE
			RCALL	ACKBIT
			CLR		Frame									;Send all 0s to the voice to shut down the voice. //Remember to update this in the clock//		
			RCALL	SENDBYTE
			RCALL	ACKBIT
			RCALL	STOPCOND

			//Done removing the voice.
			LSL		Pointer
			DEC		GeneriCount
			BREQ	DONEPOL
			RJMP	POLNOTES

NEWNOTES:	RCALL	ADDVOICE

			//Contact the voice//	
			MOV		Frame,			WorkingReg1				;ADDVOICE returns voice called into Working Register 1.  Place this in the frame.
			RCALL	STARTCOND										
			RCALL	SENDBYTE
			RCALL	ACKBIT
			
			MOV		Frame,			BondedValue				;Send the bonded value to the voice		
			RCALL	SENDBYTE
			RCALL	ACKBIT
			RCALL	STOPCOND

			LSL		Pointer
			DEC		GeneriCount
			BREQ	DONEPOL
			RJMP	POLNOTES

DONEPOL:	EOR		ActiveKeys,		ChangeLog
			MOV		WorkingReg1,	NoteBank				;Move the notebank into the working register
			LDI		WorkingReg2,	LOW(ActiveKeyRegister)
			ADD		WorkingReg1,	WorkingReg2				;Prepair the pointer
			LDI		XH,				HIGH(ActiveKeyRegister)
			MOV		XL,				WorkingReg1				;Load the value of the ActiveKeyRegister entry that needs to be pulled
			ST		X,				ActiveKeys				;Move that value into the ActiveKeys register
			RJMP	START
;	========================================================
;						SUBROUTINES
;	========================================================
/*STARTCOND:
Generate a start condition on the USI*/
STARTCOND:	IN		WorkingReg1,		PORTB
			LDI		WorkingReg2,		0b10100000
			OR		WorkingReg1,		WorkingReg2
			OUT		PORTB,				WorkingReg2				;Reset Port B's outputs
			RCALL	DELAY_16
			LDI		WorkingReg1,		0x80					
			OUT		PORTB,				WorkingReg1				;Pull SDA low, hold SCL high			
			RCALL	DELAY_16
			OUT		USICR,				BitAdvance				;Pull SCL low
			SBIS	USISR,				USISIF					
			RJMP	STARTCOND									;Try again if the start condition fails.  Full program should jump to something that resets everything
			SBI		USISR,				USISIF					;Clear the interrupt flag.
			LDI		WorkingReg1,		0x20
			OUT		PORTB,				WorkingReg1				;Start Condition complete
			RET

/*SENDBYTE:
Sends whatever is in the Frame register out on the serial data line.  Respects Fmax.
EXPECTS TO BE IN LOW CLOCK STATE, RETURNS A LOW CLOCK STATE.*/
			//Initialize//
SENDBYTE:	LDI		WorkingReg1,		0x08					;Bit count
			MOV		GeneriCount,		WorkingReg1		
			OUT		USIDR,				Frame					;Put the Frame into the Data Register
			LDI		WorkingReg1,		0xF0
			OUT		USISR,				WorkingReg1				;Clear Flags and counter

			//Transmit//
CYCLESTART:	OUT		USICR,				BitAdvance				;Generate a positive clock edge
WAIT1:		RCALL	DELAY_16
			SBIS	PINB,				SCL
			RJMP	WAIT1										;Wait for the clock to stabilize HIGH
			OUT		USICR,				BitAdvance				;Generate a negative clock edge
WAIT2:		RCALL	DELAY_16
			SBIC	PINB,				SCL
			RJMP	WAIT2										;Wait for the clock to stabilize LOW
			DEC		GeneriCount
			BRNE	CYCLESTART									;Loop for 8 bits
			;Need to clear overflow flags here
			RET

/*ACKBIT:
This subroutine runs a check for the acknowledge bit from any addressed slave device
THIS ROUTINE EXPECTS TO BE ENTERED WITH THE CLOCK PULLED LOW.*/
ACKBIT:		CBI		DDRB,				SDA						;Prepare the data line for input
			LDI		WorkingReg1,		0xFE
			OUT		USISR,				WorkingReg1				;Load the counter for one bit. Clear flags.
			LDI		WorkingReg2,		0x00
			OUT		USIDR,				WorkingReg2				;Clearing the data register (Is this necessary?)
			OUT		USICR,				BitAdvance				;Generate a positive clock edge
WAIT3:		RCALL	DELAY_16
			SBIS	PINB,				SCL						;Check for a high on the clock line
			RJMP	WAIT3
WAIT4:		OUT		USICR,				BitAdvance				;Generate a low clock signal
			RCALL	DELAY_16
			SBIC	PINB,				SCL						;Check for a low on the clock line
			RJMP	WAIT4
			SBI		USISR,				USIOIF					;Clear overflow flag
			;SBIC	USIDR,				1						;Make sure the Acknowledge bit was cleared.
			;SBI	PINB,				ERROR					;Flag as an error if no Acknowledge was recieved.
			SBI		DDRB,				SDA						;Reset as output
			RET													;Acknowledge Complete

/*STOPCOND
Generates a stop condition on the USI.*/			
STOPCOND:	IN		WorkingReg1,		PORTB
			LDI		WorkingReg2,		0b10000000
			OR		WorkingReg1,		WorkingReg2
			OUT		PORTB,				WorkingReg2				;Reset Port B's outputs, hold data line low
			RCALL	DELAY_16
			LDI		WorkingReg2,		0b10100000
			OR		WorkingReg1,		WorkingReg2
			OUT		PORTB,				WorkingReg2				;Release dataline
			LDI		WorkingReg1,		0xFF
			OUT		USIDR,				WorkingReg1				;Back-Fill Data Regitser to fully release data line
			RCALL	DELAY_16									;Stop Condition complete
			RET

/*DELAY_16:
This subroutine implements a holding pattern for 16 clock cycles.  
Needed to keep USI clock at (Fmax=SysClock/16)*/
DELAY_16:	NOP
			NOP
			NOP
			NOP
			NOP
			NOP
			NOP
			NOP
			NOP
			NOP
			NOP
			NOP
			NOP
			NOP
			NOP
			NOP
			RET

REMOVEVCE:	LDI		XL,				LOW(ActiveNotesList)
			LDI		XH,				HIGH(ActiveNotesList)
			RCALL	LOADLIST								;Call the Loadlist Subroutine for the ActiveNotesList
			MOV		WorkingReg1,	BondedValue				
			RCALL	SRCHLIST								;Search the Active Notes list for the note
			LDI		WorkingReg1,	0x00	/*OPTIMIZE THIS PLEASE*/
			SUB		WorkingReg1,	WorkingReg2				;Test to see if the return was 0.  If the return was zero, the note is already not playing
			BREQ	REMOVED
			RCALL	REMOVELI								;Call the "Remove List item" subroutine
			RCALL	STORLIST								;Store the updated notes list
			LDI		XL,				LOW(ActiveVoiceList)
			LDI		XH,				HIGH(ActiveVoiceList)
			RCALL	LOADLIST								;Pull out the active voices list
			RCALL	REMOVELI								;Remove the related voice from the voice list. This will pull that value into WorkingReg1
			RCALL	STORLIST								;Put the list back
			LDI		XL,				LOW(AvailableVoices)
			LDI		XH,				HIGH(AvailableVoices)
			RCALL	LOADLIST								;Pull out the available voices list
			RCALL	SHIFTIN									;Add the value from WorkingReg1 to the top of the list
			RCALL	STORLIST								;Put the list back
			;RCALL	KILLNOTE								;Turn the voice off
REMOVED:	RET

ADDVOICE:	LDI		XL,				LOW(ActiveNotesList)
			LDI		XH,				HIGH(ActiveNotesList)
			RCALL	LOADLIST								;Call the Loadlist Subroutine for the ActiveNotesList
			MOV		WorkingReg1,	BondedValue				
			RCALL	SHIFTIN									;Move the Bonded value into the Active Notes register
			RCALL	STORLIST
			LDI		XL,				LOW(AvailableVoices)	
			LDI		XH,				HIGH(AvailableVoices)
			RCALL	LOADLIST
			LDI		WorkingReg2,	0x00
			RCALL	SHIFTOUT								;Pull the top available voice (back fill w/0)
			RCALL	STORLIST								;Put the list away
			LDI		XL,				LOW(ActiveVoiceList)	
			LDI		XH,				HIGH(ActiveVoiceList)
			RCALL	LOADLIST								;Load the active voices list either for stealing for for adding
			MOV		VoiceNumber,	WorkingReg1
			ADD		WorkingReg1,	WorkingReg2				;Test for 0
			BREQ	VOICESTEAL								;if it's 0, go to the Voice Stealing routine
			MOV		WorkingReg1,	VoiceNumber
			RCALL	SHIFTIN
			RCALL	STORLIST
			RET

VOICESTEAL:	LDI		WorkingReg2,	LstVceNum
			RCALL	REMOVELI
			RCALL	SHIFTIN
			RCALL	STORLIST
			;RCALL	TRIGERNOTE
			RET


/*SHIFTIN:
This routine moves whatever is in the working register into the FIFO, 8 Register list.  
RETURN: WorkingReg1: maintains original value  WorkingReg2: Overflow*/
SHIFTIN:	MOV		WorkingReg2,	LI7						;Shift the last item into overflow
			CPI		WorkingReg2,	0x00
			MOV		LI7, LI6
			MOV		LI6, LI5
			MOV		LI5, LI4
			MOV		LI4, LI3
			MOV		LI3, LI2
			MOV		LI2, LI1
			MOV		LI1, LI0
			MOV		LI0,			WorkingReg1						;Shift the first register into the shift register
			RET

/*SHIFTOUT:
This routine moves whatever is at the top of the FIFO, 8 Register list into the first working register.  
Underflow is moved into the list from whatever value is in the second working register.
RETURN: WorkingReg1: Shift out value,  WorkingReg2: Maintains fil value*/
SHIFTOUT:	MOV		WorkingReg1,	LI0						;Shift the first item out
			MOV		LI0, LI1
			MOV		LI1, LI2
			MOV		LI2, LI3
			MOV		LI3, LI4
			MOV		LI4, LI5
			MOV		LI5, LI6
			MOV		LI6, LI7
			MOV		LI7,			WorkingReg2						;Shift the fill value into the shift register
			RET

/*LOADLIST:
This routine loads the 8 List Item registers from SRAM and resets the SRAM pointer 
to the start of the list's position in SRAM.
RETURN: Void*/
LOADLIST:	PUSH	XL
			PUSH	XH
			LD		LI0,	X+								;Load the 8 items of the list
			LD		LI1,	X+
			LD		LI2,	X+
			LD		LI3,	X+
			LD		LI4,	X+
			LD		LI5,	X+
			LD		LI6,	X+
			LD		LI7,	X+
			POP		XH
			POP		XL										;Reset the List pointer
			RET

/*STORLIST:
This routine Stores the 8 List Item registers to SRAM and resets the SRAM pointer 
to the start of the list's position in SRAM.
RETURN: Void*/
STORLIST:	PUSH	XL
			PUSH	XH
			ST		X+,		LI0								;Store the 8 items of the list
			ST		X+,		LI1
			ST		X+,		LI2
			ST		X+,		LI3
			ST		X+,		LI4
			ST		X+,		LI5
			ST		X+,		LI6
			ST		X+,		LI7
			POP		XH
			POP		XL										;Reset the List pointer
			RET

;SRCHLIST:
;This routine searches through the List Item registers for the value provided in Working Register 1
;The position is returned in WorkingRegister2, if the item isn't found the return is 0
SRCHLIST:	PUSH	GeneriCount								;Store the old counter value
			LDI		GeneriCount,	0x08					;Load the counter with the number of values in the list (8)
			MOV		WorkingReg2,	LI7						;Begin searching the values from the bottom up
			SUB		WorkingReg2,	WorkingReg1
			BREQ	RETLOCAT
			DEC		GeneriCount

			MOV		WorkingReg2,	LI6
			SUB		WorkingReg2,	WorkingReg1
			BREQ	RETLOCAT
			DEC		GeneriCount
			
			MOV		WorkingReg2,	LI5
			SUB		WorkingReg2,	WorkingReg1
			BREQ	RETLOCAT
			DEC		GeneriCount
			
			MOV		WorkingReg2,	LI4
			SUB		WorkingReg2,	WorkingReg1
			BREQ	RETLOCAT
			DEC		GeneriCount
			
			MOV		WorkingReg2,	LI3
			SUB		WorkingReg2,	WorkingReg1
			BREQ	RETLOCAT
			DEC		GeneriCount
			
			MOV		WorkingReg2,	LI2
			SUB		WorkingReg2,	WorkingReg1
			BREQ	RETLOCAT
			DEC		GeneriCount
			
			MOV		WorkingReg2,	LI1
			SUB		WorkingReg2,	WorkingReg1
			BREQ	RETLOCAT
			DEC		GeneriCount
			
			MOV		WorkingReg2,	LI0
			SUB		WorkingReg2,	WorkingReg1
			BREQ	RETLOCAT
			DEC		GeneriCount

RETLOCAT:	MOV		WorkingReg2,	GeneriCount
			POP		GeneriCount								;Restore the old countervalue
			RET

/*REMOVELI:
This subroutine removes the N item of a list and returns it into the first working register
N is defined by the second working register
RETURN: WorkingReg1 - List item N
		WorkingReg2 - N				*/
REMOVELI:	PUSH	WorkingReg2								;Store the test value so we can manipulate it without fucking things up.
			DEC		WorkingReg2								;Subtract one from the working register to line up w 0-7 registers
			LSL		WorkingReg2								;Multiply by two to pick the right register
			LDI		ZL,				LOW(LISTSTART)
			LDI		ZH,				HIGH(LISTSTART)
			ADD		ZL,				WorkingReg2
			BRCC	NOOVERFLOW
			INC		ZH
NOOVERFLOW:	IJMP	
LISTSTART:	MOV		WorkingReg1,	LI0
			RJMP	SHIFTALL								;Shift the wholedamn thing
			MOV		WorkingReg1,	LI1
			RJMP	SHIFT1
			MOV		WorkingReg1,	LI2
			RJMP	SHIFT2
			MOV		WorkingReg1,	LI3
			RJMP	SHIFT3
			MOV		WorkingReg1,	LI4
			RJMP	SHIFT4
			MOV		WorkingReg1,	LI5
			RJMP	SHIFT5
			MOV		WorkingReg1,	LI6
			RJMP	SHIFT6
			MOV		WorkingReg1,	LI7
			RJMP	SHIFT7
SHIFTALL:	MOV		LI0, LI1
SHIFT1:		MOV		LI1, LI2
SHIFT2:		MOV		LI2, LI3
SHIFT3:		MOV		LI3, LI4
SHIFT4:		MOV		LI4, LI5
SHIFT5:		MOV		LI5, LI6
SHIFT6:		MOV		LI6, LI7
SHIFT7:		CLR		LI7								;Backfill with a 0
			POP		WorkingReg2						;Return the test value back to the Working Register
			RET