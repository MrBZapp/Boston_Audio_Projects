/*
 * LocalMIDIFunc.h
 *
 *  Created on: Apr 24, 2015
 *      Author: Matt
 */

#ifndef LOCALMIDIFUNC_H_
#define LOCALMIDIFUNC_H_

#include "chip.h"
#include "GlobalDEF.h"


void SetMode();
void MIDI_NoteOff(uint8_t note, uint8_t ignore);
void MIDI_NoteOn(uint8_t num, uint8_t vel);

#endif /* LOCALMIDIFUNC_H_ */
