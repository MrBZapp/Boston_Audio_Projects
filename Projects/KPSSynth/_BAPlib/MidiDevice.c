/*
 * MidiDevice.c
 *
 * Created: 8/9/2014 12:19:42 PM
 *  Author: Matt Zapp
 */ 

#include "MidiDevice.h"
#include "../_AVRLib/global.h"

unsigned char MIDI_byteCount = 0;
unsigned char MidiStatus = MIDI_Idle;

#define MIDI_NOTEBUFFER_SIZE 12
cBuffer MIDI_NoteBuffer;
unsigned char MIDI_NoteBufferData[ MIDI_NOTEBUFFER_SIZE ];


void MIDI_DeviceInit(){
	buffer_Init( &MIDI_NoteBuffer, MIDI_NoteBufferData, MIDI_NOTEBUFFER_SIZE );
}


bool inline MIDI_IsStatus( unsigned char byte ){
	return (bool) (byte & MIDI_STATUS_MASK);
}


void MIDI_Router( unsigned char byte ){
	//unsigned char u8_Value = Midi_IN.GetByteFromRXBuffer();
	if ( MIDI_IsStatus( byte )){//Set the status if receiving a status byte
		MIDI_byteCount = 0;
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
		MIDI_byteCount++;
		switch ( MidiStatus ){
			case ( MIDI_Idle ):
				break;
			case ( MIDI_NoteOn ):
				#ifdef MIDI_BUFFERED_NOTES
					buffer_AddToEnd( &MIDI_NoteBuffer, byte );
				#else
					(*NoteOnFunction)( byte );
				#endif
				break;
		}
	}
}

#ifdef MIDI_BUFFERED_NOTES
MIDI_NoteOnMessage MIDI_GetNoteOn(){
	MIDI_NoteOnMessage NoteOn;
	NoteOn.NoteValue = buffer_GetFromFront( &MIDI_NoteBuffer );//
	NoteOn.Velocity = buffer_GetFromFront( &MIDI_NoteBuffer );
	return NoteOn;
}
#endif

#ifndef MIDI_BUFFERED_NOTES
void MIDI_AssignFunction_NoteOn( MidiFunctionPtr fptr ){
	NoteOnFunction =  fptr;
}
#endif