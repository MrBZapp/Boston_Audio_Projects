/*
 * KPSSynth.c
 *
 * Created: 8/4/2014 3:14:34 PM
 *  Author: Matt Zapp
 
 Some things of which to be aware (potential bugs)
 
 timings:
 1.) a midi message takes 2560us to send.  the USART interrupt can react to a message before the message even completes (as quickly as 2304us and as slowly as the full 2560us)
 2.) the USART code can react as quickly as 61cy before resetting the global interrupt flag. (this was measured with the MIDI router in its current incarnation.
 3.) a raw 11-bit SPI message can take 731cy from function start to interrupt end. 8-bits requires 474cy, 16 bits requires 818cy.
 
 at 1MHz clock speed (1:1) this results in about 1760cy to play with before the next USART message comes ins
 */ 

#define DIAGNOSTICS
#include "_BAPlib/BAP_Debug.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include "_AVRLib/global.h"
#include "_AVRLib/uart.h"
#include "_BAPlib/WGM.h"
#include "_BAPlib/MidiDevice.h"
#include "_BAPlib/MIDI_TranslationCharts.h"
#include "_BAPlib/TwoWirePeripherals.h"


void SetKeyScaling( unsigned char noteNumber ){
		unsigned char dacNumber = 4;
		unsigned char messageBitCount = 11;
				
		//bond the message together 
		unsigned int message = ( ( (dacNumber -1) << 9 ) |
								 ( noteNumber ));
								 
		//if the transmitter is ready, give it a message to send						 
		if( ! spi_TransmitInProgress ){ 		
			spi_transmitRawMessage( message, messageBitCount ); //Send the message.
		}
}

void updateBBD( MIDI_NoteOnMessage note ){
	unsigned char index = ( note.NoteValue % 60); //Bound the note number to the size of the lookup table
	unsigned int BBDOverFlowValue = pgm_read_word(&MIDItoBBDClockInstructionCountChart_int[index]);//read the data out of PGM memory space.
	WaveGen1_SetFrequency( BBDOverFlowValue ); //place the value of the lookup table into OCR1 A and B (849)
	SetKeyScaling( note.NoteValue ); //USART interrupt returns from here
}
 
void newVoice(){
	
}
 
 /*******
 * MAIN *
 ********/
 
int main(void){
	wdt_disable();
	MIDI_DeviceInit();
	spi_Init( Master, Two_Wire_Hold );
	WaveGen1_Init( 500, OCA );
	uartInit();
	uartSetRxHandler( &MIDI_Router );
	sei();

	while(1) // Idle Loop
    {
		MIDI_NoteOnMessage newNote = MIDI_GetNoteOn();
		if( !( newNote.NoteValue < 0 ) ){
			updateBBD(newNote);
		}
    }
}