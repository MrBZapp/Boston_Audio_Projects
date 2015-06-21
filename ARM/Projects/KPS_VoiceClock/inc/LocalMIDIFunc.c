/*
 * LocalMIDIFunc.c
 *
 *  Created on: Apr 24, 2015
 *      Author: Matt
 */

#include "GlobalDEF.h"
#include "LocalMIDIFunc.h"
#include "BAP_math.h"
#include "BAP_32Sel.h"
#include "BAP_TLC_DAC.h"
#include "BAP_WaveGen.h"
#include "FrequencyMaps.h"

void SetMode(){
	//Check what mode we're in
	static 	uint8_t prev_ValueB = 1;
	uint8_t cmpVal = i_lscale(0, 32, 0, 127, SelectorGetValue(LPC_CMP));
	if (prev_ValueB != cmpVal)
	{
		TLC_SetDACValue(TLC_DAC_3, 1, &cmpVal);
		prev_ValueB = cmpVal;
	}
}

void MIDI_NoteOn(uint8_t num, uint8_t vel)
{
	// Block Note Offs disguised as note-ons.
	if (vel == 0)
	{
		MIDI_NoteOff(num, vel);
		return;
	}


	// Load global variables
	activeNote = num;
	pluckStrength = vel;

	// Reset the Envelope
	EnvReset(&GlobalEnv);

	// Set up the scaling for the DACs and PWM.
	uint8_t value = 127 - num;
	TLC_SetDACValue(FilterDAC, 1, &value);
	value = ampTable[((num % 67) / 12) % 6] + (num % 12);
	TLC_SetDACValue(AmpDAC, 0, &value);

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

		// reset envelope to release
		EnvRelease(&GlobalEnv);
	}
}
