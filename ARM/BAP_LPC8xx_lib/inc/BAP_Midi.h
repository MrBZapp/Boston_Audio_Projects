/*
 * BAP_Midi.h
 *
 *  Created on: Nov 9, 2014
 *      Author: Matt
 */

#ifndef BAP_MIDI_H_
#define BAP_MIDI_H_

#include "chip.h"
#include "BAP_Type.h"

typedef void(*MidiFuncPtr)(uint8_t D1, uint8_t D2);

// Midi Command Function Pointers
MidiFuncPtr MIDI_NoteOnFunc;
MidiFuncPtr MIDI_NoteOffFunc;
VoidFuncPointer MIDI_Sync_StartFunc;
VoidFuncPointer MIDI_Sync_StopFunc;
VoidFuncPointer MIDI_Sync_ClkFunc;
VoidFuncPointer MIDI_Sync_ContFunc;
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
#define MIDI_SYNC_CLK 0xF8
#define MIDI_SYNC_START 0xFA
#define MIDI_SYNC_CONT 0xFB
#define MIDI_SYNC_STOP 0xFC
#define MIDI_NOTEOFF 0x80
#define MIDI_NOTEON 0x90

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
 * Processes all received information.  Does nothing if no information was received.
 */
void MIDI_ProcessRXBuffer();

/***
 * counts sync messages handled, fires appropriate high-priority functions.
 * Returns 1 if the message was handled, 0 otherwise.
 */
int MIDI_SyncHandler(uint8_t msg);


/***
 * Returns the status of the MIDI module
 */
uint8_t MIDI_GetStatus_0();

#endif /* BAP_MIDI_H_ */
