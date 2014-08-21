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

const unsigned int MIDItoBBDClockInstructionCountChart_int[60] PROGMEM = {
	 0x03BC
	,0x0386
	,0x0353
	,0x0323
	,0x02F6
	,0x02CC
	,0x02A4
	,0x027E
	,0x025A
	,0x0238
	,0x0218
	,0x01FA
	,0x01DE
	,0x01C3
	,0x01AA
	,0x0192
	,0x017B
	,0x0166
	,0x0152
	,0x013F
	,0x012D
	,0x011C
	,0x010C
	,0x00FD
	,0x00EF
	,0x00E1
	,0x00D5
	,0x00C9
	,0x00BE
	,0x00B3
	,0x00A9
	,0x009F
	,0x0096
	,0x008E
	,0x0086
	,0x007F
	,0x0077
	,0x0071
	,0x006A
	,0x0064
	,0x005F
	,0x0059
	,0x0054
	,0x0050
	,0x004B
	,0x0047
	,0x0043
	,0x003F
	,0x003C
	,0x0038
	,0x0035
	,0x0032
	,0x002F
	,0x002D
	,0x002A
	,0x0028
	,0x0026
	,0x0024
	,0x0022
};

#endif /* MIDI_TRANSLATIONCHARTS_H_ */