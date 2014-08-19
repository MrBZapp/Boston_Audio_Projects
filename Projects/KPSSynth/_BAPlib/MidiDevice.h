/*
 * MidiDevice.h
 *
 * Created: 8/9/2014 12:19:18 PM
 *  Author: Matt Zapp
 */ 

#ifndef MIDIDEVICE_H_
#define MIDIDEVICE_H_
#include <stdbool.h>

#define MIDI_STATUS_MASK 0x80
#define MIDI_STATUS_NIB_MASK 0xF0


enum Midi_Status{
	MIDI_Idle = 0x00
	,MIDI_NoteOff = 0x80
	,MIDI_NoteOn = 0x90
};

typedef void (*MidiFunctionPtr)( unsigned char byte ); //Function Pointer for Midi commands

//! Logs and routes a series of bytes as MIDI data
/// 
void MIDI_Router( unsigned char byte );

//! Assigns a function to Note On
///
void MIDI_AssignFunction_NoteOn( MidiFunctionPtr pFunc );

//! Assigns a function to Note Off
///
void MIDI_AssignFunction_NoteOff( MidiFunctionPtr pFunc );

volatile MidiFunctionPtr NoteOnFunction;//need a way to enable these things by if they exist in a system or not.
volatile MidiFunctionPtr NoteOffFunction;

bool MIDI_IsStatus( unsigned char byte );


#endif /* MIDIDEVICE_H_ */