/*
 * MidiDevice.c
 *
 * Created: 8/9/2014 12:19:42 PM
 *  Author: Matt Zapp
 */ 

#include <stdbool.h>
#include "MidiDevice.h"

unsigned char byteCount = 0;
unsigned char MidiStatus = MIDI_Idle;

bool MIDI_IsStatus( unsigned char byte ){
	return (bool) (byte & MIDI_STATUS_MASK);
}

void MIDI_Router( unsigned char byte ){
	//unsigned char u8_Value = Midi_IN.GetByteFromRXBuffer();
	if ( MIDI_IsStatus( byte )){//Set the status if receiving a status byte
		byteCount = 0;
		byte &= MIDI_STATUS_NIB_MASK;
		switch ( byte ){
			case ( MIDI_NoteOn ):
				MidiStatus = MIDI_NoteOn;
				break;
			default:
				MidiStatus = MIDI_Idle;
				break;
		}
	}
	else{ //route the data to the appropriate functions
		byteCount++;
		switch ( (unsigned char) MidiStatus ){
			case ( MIDI_Idle ):
			break;
			case ( MIDI_NoteOn ):
			byteCount %= 2;
			(*NoteOnFunction)( byte );
			break;
		}
	}
}


void MIDI_AssignFunction_MIDI_NoteOn( MidiFunctionPtr fptr ){
	NoteOnFunction =  fptr;
}