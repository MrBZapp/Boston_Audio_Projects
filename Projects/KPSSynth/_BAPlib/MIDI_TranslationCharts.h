/*
 * MIDI_TranslationCharts.h
 *
 * Created: 8/14/2014 10:11:12 PM
 *  Author: Matt Zapp
 */ 


#ifndef MIDI_TRANSLATIONCHARTS_H_
#define MIDI_TRANSLATIONCHARTS_H_

#include <avr/pgmspace.h>
#include <avr/eeprom.h>

const float MidiToBBDClockFreqChart_float[56] PROGMEM = {
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

const int MIDItoBBDClockInstructionCountChart_int[56] PROGMEM = {
	0xF6
	,0x2CC
	,0x2A4
	,0x27E
	,0x25A
	,0x238
	,0x218
	,0x1FA
	,0x1DE
	,0x1C3
	,0x1AA
	,0x192
	,0x17B
	,0x166
	,0x152
	,0x13F
	,0x12D
	,0x11C
	,0x10C
	,0xFD
	,0xEF
	,0xE1
	,0xD5
	,0xC9
	,0xBE
	,0xB3
	,0xA9
	,0x9F
	,0x96
	,0x8E
	,0x86
	,0x7F
	,0x77
	,0x71
	,0x6A
	,0x64
	,0x5F
	,0x59
	,0x54
	,0x50
	,0x4B
	,0x47
	,0x43
	,0x3F
	,0x3C
	,0x38
	,0x35
	,0x32
	,0x2F
	,0x2D
	,0x2A
};

#endif /* MIDI_TRANSLATIONCHARTS_H_ */