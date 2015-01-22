; KPSVoice_Clock.asm
;	Created: 4/3/2013 4:11:20 PM
;   Author: Matt
;	==============================
;			VOICE HANDLER
;	==============================
;	The following program continually updates a timer to send clocking data to the BBD analog memory.
;	Eventually I'd also like to implement some form of vibrato into the clocking logic.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.INCLUDE "tn13def.inc" ;ATtiny13A header file
;	==============================
; 			PIN DESCRIPTION			; OPPOSITE I/O DESIGNATION FROM PIC.  KEEP THAT IN MIND. 0 = input 1= output
;	==============================
;	PORTB I/O: (00 1110)
	.EQU	DataIn		=	PINB0		;INPUT	1.) Data input from Voice_Handler
	.EQU	Trigger		=	PINB1		;OUTPUT	2.) Trigger out to the noise generator
	.EQU	BBDClock	=	PINB2		;OUTPUT	3.)	Clock output to the delay line
	.EQU	VCDATAOUT	=	PINB3		;OUTPUT	4.) Handshake Out to Voice_Handler
	.EQU	KBMUX		=	PINB4		;INPUT	5.)	Handshake input from Voice_Handler
	.EQU	VIBRATO		=	PINB5		;INPUT	6.) Analog input for vibrato calculation.

;	========================================================
;			PROGRAM MEMORY VARIABLE DEFINITION
;	========================================================
	.DEF	WorkingReg1	=	R16			;General purpose working register
	.DEF	WorkingReg2	=	R17			;General purpose working register
;	========================================================
;					SRAM VARIABLE DEFINITION
;	========================================================
;	========================================================
;						PROGRAM START
;	========================================================
.CSEG
.ORG	0x0000
		RJMP MCU_INIT					; Reset Handler
		RETI	;rjmp EXT_INT0 ; IRQ0 Handler
		RETI	;rjmp PCINT0 ; PCINT0 Handler
		RETI	;rjmp TIM0_OVF ; Timer0 Overflow Handler
		RETI	;rjmp EE_RDY ; EEPROM Ready Handler
		RETI	;rjmp ANA_COMP ; Analog Comparator Handler
		RETI	;rjmp TIM0_COMPA ; Timer0 CompareA Handler
		RETI	;rjmp TIM0_COMPB ; Timer0 CompareB Handler
		RETI	;rjmp WATCHDOG ; Watchdog Interrupt Handler
		RETI	; rjmp ADC ; ADC Conversion Handler

	RJMP	MCU_INIT											;Ready to initialize
MCU_INIT:
			CLI													; Turn off global interrupt 
			WDR													; Reset Watchdog Timer
			IN		WorkingReg1,		MCUSR
			ANDI	WorkingReg1,		(0xff - (0<<WDRF))
			OUT		MCUSR,				WorkingReg1				;Clear WD Reset Flag in MCUSR
			
			IN		WorkingReg1,		WDTCR										
			ORI		WorkingReg1,		(1<<WDCE) | (1<<WDE)
			OUT		WDTCR,				WorkingReg1				;Write logical one to WDCE and WDE Keep old prescaler setting to prevent unintentional time-out
			LDI		WorkingReg1,		(0<<WDE)
			OUT		WDTCR,				WorkingReg1				;Turn off WDT

			LDI		WorkingReg1,		LOW(RAMEND)
			OUT		SPL,				WorkingReg1				;Set the Stack Pointer

			LDI		WorkingReg1,		0b10000000
			LDI		WorkingReg2,		0b00001110						
			OUT		MCUCR,				WorkingReg1				;Pull-up resistor disable,Disable interrupts, disable sleep
			OUT		DDRB,				WorkingReg2				;Set I/O for PortD as listed above, PortB is automatically configured to all Inputs.
START:		
