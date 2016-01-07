/*
 * Mask.h
 *
 *  Created on: Jun 20, 2015
 *      Author: Matt
 */

#ifndef MASK_H_
#define MASK_H_

#include "chip.h"
#include "BAP_WaveGen.h"
#include "FrequencyMaps.h"
#include "BAP_math.h"


#define SCT_PWM          LPC_SCT
#define SCT_PWM_PIN_1    0		/* COUT0 */
#define SCT_PWM_PIN_2    1		/* COUT1 */
#define SCT_PWM_PIN_3    2		/* COUT2 */
#define SCT_PWM_PIN_4    3		/* COUT3 */

#define SCT_PWM_1        1		/* Index of PWM 1 */
#define VOCT1 SCT_PWM_1
#define SCT_PWM_2        2		/* Index of PWM 2 */
#define VOCT2 SCT_PWM_2
#define SCT_PWM_3        3		/* Index of PWM 3 */
#define TRIG SCT_PWM_3
#define SCT_PWM_4        4		/* Index of PWM 4 */
#define SYNC SCT_PWM_4

#define LOCAL_ADDRESS 0x00
#define LED_LOCATION (4)
#define GATE_LENGTH 5
#define TICKRATE_HZ     1000		/* 1 ms Tick rate */
#define SYNC_TICKS 2 // Sync to an 8th note

static uint8_t voicenum = 0;

uint8_t gatems = 0;

void SysTick_Handler(void)
{
	if (gatems != 0)
		gatems--;
}

void MIDI_SyncCounter()
{
	static uint8_t ticks = 0;
	static uint8_t value = 127;
	if (ticks == 0)
	{
		Chip_SCTPWM_SetDutyCycle(SCT_PWM, SYNC , value);
		value = value ^ 127;
	}

	ticks++;
	ticks %= SYNC_TICKS;


}

void MIDI_NoteOff(uint8_t num, uint8_t vel)
{
	voicenum--;
	voicenum %= 2;
}

void MIDI_NoteOn(uint8_t num, uint8_t vel)
{
	// Block Note Offs disguised as note-ons.
	if (vel == 0)
	{
		return;
	}

	// Assign note to a voice
	Chip_SCTPWM_SetDutyCycle(SCT_PWM, (voicenum + 1) , num);

	// Advance voice
	voicenum++;
	voicenum %= 2;

	// Gate on
	Chip_SCTPWM_SetDutyCycle(SCT_PWM, TRIG,  127);
	gatems = GATE_LENGTH;
}

#endif /* MASK_H_ */
