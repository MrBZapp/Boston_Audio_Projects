/*
 * LocalMIDIFunc.c
 *
 *  Created on: Apr 24, 2015
 *      Author: Matt
 */

#include "GlobalDEF.h"
#include "LocalMIDIFunc.h"
#include "BAP_TLC_DAC.h"
#include "BAP_WaveGen.h"
#include "FrequencyMaps.h"

void MIDI_NoteOn(uint8_t num, uint8_t vel)
{
	// Load global variables
	activeNote = num;
	pluckStrength = vel;
	EnvPosition = 0;

	// Set up the scaling for the DACs and PWM.
	uint8_t value = 127 - num;
	TLC_SetDACValue(FilterDAC, 1, &value);
	value = ampTable[((num % 67) / 12) % 6] + (num % 12);
	TLC_SetDACValue(AmpDAC, 0, &value);

	// Knee for PWM adjustment in higher registers
	//value = (num >= 84) ? 15 : 50;
	//setWidth(&Generator1, value);

	// Set frequency generator's frequency.
	setReload(&Generator1, MIDIto30MhzReload[num % 67]);
	updateFreq(&Generator1);

	// start the exciter
	Chip_SCT_EnableEventInt(LPC_SCT, SCT_EVT_0);
	NVIC_EnableIRQ(SCT_IRQn);
}

void MIDI_NoteOff(uint8_t note, uint8_t ignore)
{
	if (note == activeNote)
	{
		uint8_t value = 255 - activeNote;
		TLC_SetDACValue(FilterDAC, 1, &value);
		// set to Idle
		activeNote = -1;
		EnvPosition = 0;
	}
}
