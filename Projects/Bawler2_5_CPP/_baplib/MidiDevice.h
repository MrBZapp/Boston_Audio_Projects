/*
 * MidiDevice.h
 *
 * Created: 5/18/2014 8:34:24 PM
 *  Author: Matt
 */ 


#ifndef MIDIDEVICE_H_
#define MIDIDEVICE_H_

#include <avr/pgmspace.h>

#define MIDI_STATUS_MASK 0x80
#define MIDI_STATUS_NIB_MASK 0xF0

class MidiDevice;
class PeripheralHandler;

enum Midi_Status{
	 MIDI_Idle = 0x00
	,MIDI_NoteOff = 0x80
	,MIDI_NoteOn = 0x90
	};
	
const float MidiToBBDChart[56] PROGMEM = {
	 8.3712
	,8.86784
	,9.3952
	,9.9584
	,10.5472
	,11.17696
	,11.83744
	,12.544
	,13.29152
	,14.08
	,14.91968
	,15.80544
	,16.7424
	,17.7408
	,18.79552
	,19.91168
	,21.0944
	,22.3488
	,23.68
	,25.088
	,26.57792
	,28.16
	,29.83424
	,31.61088
	,33.48992
	,35.4816
	,37.59104
	,39.82336
	,42.19392
	,44.70272
	,47.36
	,50.176
	,53.16096
	,56.32
	,59.66848
	,63.21664
	,66.97472
	,70.95808
	,75.17696
	,79.64672
	,84.38272
	,89.40032
	,94.72
	,100.352
	,106.3168
	,112.64
	,119.33696
	,126.43328
	,133.95456
	,141.91616
	,150.35392
	,159.29856
	,168.77056
	,178.80576
	,189.43
};

typedef void (*MidiFunctionPtr)( unsigned char byte ); //Function Pointer for Midi commands
typedef void (MidiDevice::*MidiDeviceMemberFunction)( unsigned char data );//Function pointer for MidiDevice Member functions

class MidiDevice {
  //Variables
	public:
	protected:
	private:
		Midi_Status m_MidiStatus;
		unsigned char byteCount;
  //Functions
	public:
		MidiDevice();
		//MidiDevice( PeripheralHandler* perHandeler);
		void Router( unsigned char byte );
		void Trigger_NoteOn( unsigned char note, unsigned char vel );
		void AssignFunction_MIDI_NoteOn( MidiFunctionPtr pFunc );
		void AssignFunction_MIDI_NoteOff( MidiFunctionPtr pFunc );
	protected:	
		volatile MidiFunctionPtr NoteOnFunction;
		volatile MidiFunctionPtr NoteOffFunction;
	private:
		bool IsStatus( unsigned char byte );
	};

#endif /* MIDIPARSE_H_ */