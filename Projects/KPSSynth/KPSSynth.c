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

unsigned char testArr[50] = {0};

void updateBBD( unsigned char test ){
	WaveGen1_SetFrequency( testArr[test] );
}

int main(void)
{
	wdt_disable();
	uartInit();
	WaveGen1_Init( 500, OCA );
	
	MIDI_AssignFunction_MIDI_NoteOn( &updateBBD );
    while(1)
    {
		while(!uartReceiveBufferIsEmpty()){
			MIDI_Router(uartGetByte());
		}
    }
}