/*
 * MidiDevice.c
 *
 * Created: 8/9/2014 12:19:42 PM
 *  Author: Matt Zapp
 */ 

#include "MidiDevice.h"
#include "../_AVRLib/global.h"

unsigned char MidiStatus = MIDI_Idle;

#define MIDI_NOTEBUFFER_SIZE 12 //must be dividable by 2.
cBuffer MIDI_NoteBuffer;
unsigned char MIDI_NoteBufferData[ MIDI_NOTEBUFFER_SIZE ] = { [0 ... (MIDI_NOTEBUFFER_SIZE - 1)] 0x00 };


void MIDI_DeviceInit(){
	buffer_Init( &MIDI_NoteBuffer, MIDI_NoteBufferData, MIDI_NOTEBUFFER_SIZE );
}


bool inline MIDI_IsStatus( unsigned char byte ){
	return (bool) (byte & MIDI_STATUS_MASK);
}


void MIDI_Router( unsigned char byte ){
	//unsigned char u8_Value = Midi_IN.GetByteFromRXBuffer();
	if ( MIDI_IsStatus( byte )){//Set the status if receiving a status byte
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

//!Re-sets all buffers and counts.
///
void MIDI_Panic(){
	
}

#ifdef MIDI_BUFFERED_NOTES
//!Retrieves a Note on message from the internal buffer.  will return -1 if no data is available
///
MIDI_NoteOnMessage MIDI_GetNoteOn(){
	MIDI_NoteOnMessage NoteOn = {-1, -1};// assume everything is wrong.
	if ( buffer_BytesLeft( &MIDI_NoteBuffer ) != MIDI_NOTEBUFFER_SIZE ){ //check to see if there's any information in the buffer
		if( !(buffer_BytesLeft( &MIDI_NoteBuffer ) % 2) ){ // check to see if there's enough data to be useful. every note on message must be 2 bytes
			NoteOn.NoteValue = buffer_GetFromFront( &MIDI_NoteBuffer ); //Note value will be first
			NoteOn.Velocity = buffer_GetFromFront( &MIDI_NoteBuffer ); //Followed by its velocity value
		}
	}
	return NoteOn;
}
#endif

#ifndef MIDI_BUFFERED_NOTES
void MIDI_AssignFunction_NoteOn( MidiFunctionPtr fptr ){
	NoteOnFunction =  fptr;
}
#endif