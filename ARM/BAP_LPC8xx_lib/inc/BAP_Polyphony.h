/*
 * BAP_Polyphony.h
 *
 *  Created on: Nov 17, 2015
 *      Author: Matt
 */

#ifndef BAP_POLYPHONY_H_
#define BAP_POLYPHONY_H_

#include "chip.h"

typedef struct VoiceHandler {
	uint8_t notes[127];
	int8_t* voices;
	uint8_t voice_count;
	uint8_t index;
}VoiceHandler_t;

uint8_t PolyVoice_Init(VoiceHandler_t vh, uint8_t* voices);

uint8_t PolyVoice_Assign(VoiceHandler_t vh, uint8_t note);
void PolyVoice_Release(VoiceHandler_t vh, uint8_t note);

#endif /* BAP_POLYPHONY_H_ */
