/*
 * KPSSynth.c
 *
 * Created: 8/4/2014 3:14:34 PM
 *  Author: Matt Zapp
 */ 


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
	for( unsigned char i = 4; i !=0; i--){
		unsigned char dacNumber = i;
		unsigned char messageBitCount = 11;
				
		//bond the message together 
		unsigned int message = ( ( dacNumber -1) << 9 ) |
								( noteNumber );
		while( spi_TransmitInProgress ){}// wait for transmitter to be ready 		
		spi_transmitRawMessage( message, messageBitCount ); //Send the message.
	}
}

void updateBBD( unsigned char noteNumber ){
	unsigned char offset = 4;
	noteNumber -= offset;
	unsigned char index = ( noteNumber % sizeof(MIDItoBBDClockInstructionCountChart_int)); //Bound the note number to the size of the lookup table
	WaveGen1_SetFrequency( MIDItoBBDClockInstructionCountChart_int[ index ]); //place the value of the lookup table into OCR1 A and B
	SetKeyScaling( noteNumber );
}
 
 
 /*******
 * MAIN *
 ********/
 
int main(void)
{
	wdt_disable();
	spi_Init( Master, Two_Wire );
	WaveGen1_Init( 500, OCA );
	MIDI_AssignFunction_NoteOn( &updateBBD );
	uartInit();
	uartSetRxHandler( &MIDI_Router );

	while(1) // Idle Loop
    {
    }
}