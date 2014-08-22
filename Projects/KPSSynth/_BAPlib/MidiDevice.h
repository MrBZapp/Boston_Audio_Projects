/*
 * MidiDevice.h
 *
 * Created: 8/9/2014 12:19:18 PM
 *  Author: Matt Zapp
 */ 

#ifndef MIDIDEVICE_H_
#define MIDIDEVICE_H_
#include <stdbool.h>
#include "../_AVRLib/buffer.h"

#define MIDI_STATUS_MASK 0x80
#define MIDI_STATUS_NIB_MASK 0xF0

enum Midi_Status{
	MIDI_Idle = 0x00
	,MIDI_NoteOff = 0x80
	,MIDI_NoteOn = 0x90
};

typedef struct struct_MIDI_NoteOnMessage{
	signed char NoteValue; //These values are signed so that function calls can recognize invalid values. MIDI spec does not use Bit 7.
	signed char Velocity;
	} MIDI_NoteOnMessage;

typedef void (*MidiFunctionPtr)( unsigned char byte ); //Function Pointer for Midi commands
typedef bool (*MidiBufferPtr)( cBuffer buffer, unsigned char data);

//! Logs and routes a series of bytes as MIDI data
/// 
void MIDI_Router( unsigned char byte );

//! Assigns a function to Note On
///
#ifdef MIDI_BUFFERED_NOTES
void MIDI_AssignFunction_NoteOn( MidiFunctionPtr pFunc );
#else

#endif


//! Assigns a function to Note Off
///
void MIDI_AssignFunction_NoteOff( MidiFunctionPtr pFunc );

#ifdef MIDI_BUFFERED_NOTES
volatile MidiBufferPtr NoteOnBuffer;//need a way to enable these things by if they exist in a system or not.
#else
volatile MidiFunctionPtr NoteOnFunction;//need a way to enable these things by if they exist in a system or not.
#endif
volatile MidiFunctionPtr NoteOffFunction;


bool MIDI_IsStatus( unsigned char byte );


#endif /* MIDIDEVICE_H_ */