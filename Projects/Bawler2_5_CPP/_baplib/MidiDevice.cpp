/*
 * MidiDevice.cpp
 *
 * Created: 5/19/2014 9:10:40 AM
 *  Author: Matt
 */ 

#include "MidiDevice.h"

MidiDevice::MidiDevice(){
	//Default Constructor
	m_MidiStatus = MIDI_Idle;
	NoteOnFunction = 0;
}

void MidiDevice::Trigger_NoteOn( unsigned char note, unsigned char vel ){
	this->Router( 0x80 );
	this->Router( note );
	this->Router( vel );
}

bool MidiDevice::IsStatus( unsigned char byte ){
	return (bool) (byte & MIDI_STATUS_MASK);
}

void MidiDevice::Router( unsigned char byte ){
	//unsigned char u8_Value = Midi_IN.GetByteFromRXBuffer();
	if ( this->IsStatus( byte )){//Set the status if receiving a status byte
		byteCount = 0;
		byte &= MIDI_STATUS_NIB_MASK;
		switch ( byte ){
			case ( MIDI_NoteOn ):
				m_MidiStatus = MIDI_NoteOn;
				break;
			default:
				m_MidiStatus = MIDI_Idle;
		}
	}
	else{ //route the data to the appropriate functions
		byteCount++;
		switch ( (unsigned char) m_MidiStatus ){
			case ( MIDI_Idle ):
				break;
			case ( MIDI_NoteOn ):
				byteCount %= 2;
				(*NoteOnFunction)( byte );
			break;
		}
	}
}


void MidiDevice::AssignFunction_MIDI_NoteOn( MidiFunctionPtr fptr ){
	NoteOnFunction =  fptr;
}