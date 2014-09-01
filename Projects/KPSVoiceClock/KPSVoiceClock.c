/*
 * KPSVoiceClock.c
 *
 * Created: 8/29/2014 5:35:13 PM
 *  Author: Matt Zapp
 */ 

//#define DIAGNOSTICS

#include <avr/io.h>
#include <avr/wdt.h>

#include "BAPlib/BAP_Debug.h"
#include "AVRlib/timer.h"
#include "BAPlib/WGM.h"
#include "BAPlib/TwoWirePeripherals.h"
#include "BAPlib/MIDI_TranslationCharts.h"
#include "BAPlib/TLV5620.h"

int main(void)
{
	wdt_disable();
	WaveGen0_Init( OCB );
	/*TODO:
	* build out TWI to support slave operation
	* fix midi translation chart
	* add logic to translate the translation to 8-bit timers w/ prescale adjustments
	* build out TLV interface to use bit-bang control.
	*/ 
    while(1)
    {
    }
}