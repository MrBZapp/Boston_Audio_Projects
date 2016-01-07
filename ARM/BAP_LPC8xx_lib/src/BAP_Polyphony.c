/*
 * BAP_Polyphony.c
 *
 *  Created on: Nov 17, 2015
 *      Author: Matt
 */

#include "BAP_Polyphony.h"

uint8_t PolyVoice_Assign(VoiceHandler_t vh, uint8_t note)
{
	// search for a free voice
	for (int i = (vh.index + 1); i != vh.index; i++)
	{
		// bound the loop to just available voices
		i %= vh.voice_count;

		// if the voice is free, assign it, and return it, otherwise, keep searching
		if (vh.voices[i] == -1)
		{
			vh.index = i;
			vh.voices[i] = note;

			// Designate the note as assigned to a voice
			vh.notes[note] = vh.index;
			return vh.index;
		}
	}

	// if there isn't a free voice, steal the one already under the index
	// Designate the note as assigned to a voice
	vh.notes[note] = vh.index;
	return vh.index;

}

void PolyVoice_Release(VoiceHandler_t vh, uint8_t note)
{

}
