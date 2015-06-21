/*
 * BAP_Midi.h
 *
 *  Created on: Nov 9, 2014
 *      Author: Matt
 */

#ifndef BAP_MIDI_H_
#define BAP_MIDI_H_

#include "chip.h"

typedef void(*MidiFuncPtr)(uint8_t D1, uint8_t D2);


// Midi Command Function Pointers
MidiFuncPtr MIDI_NoteOnFunc;
MidiFuncPtr MIDI_NoteOffFunc;
MidiFuncPtr MIDI_PCChFunc;
MidiFuncPtr MIDI_PitchFunc;

#define MIDI_MAX 0x7F

#define MIDI_ENABLERX 0x02
#define MIDI_ENABLETX 0x01

#define MIDI_STATBIT 0x80 // Mask to check for a status bit
#define MIDI_STATMSK 0x70 // Mask to extract status from status byte
#define MIDI_CHMSK 0x0F   // mask to extract channel from status byte
#define MIDI_DATAMSK 0x7F

// Statuses
#define MIDI_IDLE 0x00
#define MIDI_NOTEON 0x90
#define MIDI_NOTEOFF 0x80

/***
 * Midi initialize for USART (1 or 2) either receiving, transmitting, or both
 */
void MIDI_USARTInit(LPC_USART_T* USARTNumber, int RXTX_Enable);

/***
 * Enable the USART previously initialized to receive MIDI data
 */
void MIDI_Enable(LPC_USART_T* USARTNumber);

/***
 * Sets the address of the current receiver
 **/
void MIDI_SetAddress(uint8_t addr);

/***
 * Sets a selected function to the user-defined function
 */
void MIDI_SetFunction(uint8_t funcName, MidiFuncPtr func);

/***
 * Processes all received information.  Does nothing if no information was received.
 */
void MIDI_ProcessRXBuffer();


/***
 * Returns the status of the MIDI module
 */
uint8_t MIDI_GetStatus_0();

#endif /* BAP_MIDI_H_ */
