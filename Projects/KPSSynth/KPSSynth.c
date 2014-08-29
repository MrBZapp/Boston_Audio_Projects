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
#include "BAPlib/BAP_Debug.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/sleep.h>

#include "AVRlib/uart.h"
#include "BAPlib/WGM.h"
#include "BAPlib/MidiDevice.h"
#include "BAPlib/MIDI_TranslationCharts.h"
#include "BAPlib/TLV5620.h"

void updateBBD( unsigned char note ){
	unsigned char index = ( note % 60); //Bound the note number to the size of the lookup table
	unsigned int BBDOverFlowValue = pgm_read_word(&MIDItoBBDClockInstructionCountChart_int[index]);//read the data out of PGM memory space.
	WaveGen1_SetFrequency( BBDOverFlowValue ); //place the value of the lookup table into OCR1 A and B (849)
}

 
 /*******
 * MAIN *
 ********/
 
int main(void){
	wdt_disable(); //Turn off the watch-dog timer
	set_sleep_mode( SLEEP_MODE_IDLE ); //enable sleep in idle mode
	MIDI_DeviceInit(); //set up the Midi Device
	TLV5620_Init(); //turn on the USI to contact the peripheral DAC
	WaveGen1_Init( OCA ); //Turn on the 16-bit waveform generator
	uartInit(); //ready the UART to receive data
	uartSetRxHandler( &MIDI_Router ); //direct the received data to the MIDI router
	sei(); //Enable interrupts!

	while(1) // main Loop
    {
		MIDI_NoteOnMessage newNote = MIDI_GetNoteOn();
		if( !( newNote.NoteValue < 0 ) ){ // If there's a new note, process it.
			TLV5620_SendMessage( newNote.NoteValue, 1, 0 );
			TLV5620_SendMessage( newNote.NoteValue, 2, 0 );
			updateBBD( newNote.NoteValue );
		}else{
			sleep_mode(); // you're done, chill out for a while.
		}
    }
}