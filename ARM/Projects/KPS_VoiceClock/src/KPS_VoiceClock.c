/*
===============================================================================
 Name        : KPS_VoiceClock.c
 Author      : Matt Zapp
 Version     : 1
 Copyright   : 2014
 Description :	This is the software to control a single voice of the BBD-based
 	 	 	 	Karplus-Strong synthesizer.

 	 	 	 	I/O setup:
 	 	 	 		GPIO 0: Serial/MIDI input
 	 	 	 		GPIO 1: BBD CLK output
 	 	 	 		GPIO 2: Trigger
 	 	 	 		GPIO 3: SPI CLK
 	 	 	 		GPIO 4: SPI MOSI
 	 	 	 		GPIO 5: RESET

 	 	 	 	Program Steps:
 	 	 	 		-->	Get pitch data via serial wire
 	 	 	 			TODO: replace this with a true MIDI signal
					-->	translate pitch into BBD clock frequency, and translate
						that into an overflow count for the on-board counter.
						TODO: make some Master clock adjustments to allow the counter
							to more-accurately follow true pitch.
					--> TODO: Send key-scaling adjustment data to the out-board DACs
					--> Send a trigger pulse to the analog core to strike a note.
						TODO: set up this trigger to be adjusted with key-scaling
						TODO: tailor the trigger's timbre to be less abrasive.

===============================================================================
*/

#include "chip.h"
#include <cr_section_macros.h>
#include "BAP_Midi.h"
#include "FrequencyMaps.h"
#include "BAP_WaveGen.h"
#include "BAP_TLC_DAC.h"
#include "BAP_Type.h"
#include "BAP_math.h"
#include "BAP_Debug.h"


// I/O setup
#define TRIGGER_LOCATION  2
#define TLC_LDAC_LOCATION 3

// Define TLV DAC functions
#define FilterDAC TLC_DAC_1
#define FeedbackDAC TLC_DAC_2
#define PulseDAC TLC_DAC_4

// Define Address
#define LOCAL_ADDRESS 0x00

#define PULSE_LENGTH 384
#define TRANSIENT_LENGTH 128

void genPluck(uint32_t cycleCount, uint8_t strength);
void SCT_IRQHandler(void);
void MIDI_NoteOn(uint8_t note, uint8_t vel);
uint16_t LFSR();

/*****************************************************************************
 * Variables																 *
 ****************************************************************************/
volatile uint32_t remainingNoise = 0;
volatile uint8_t pluckStrength = 0;
volatile uint8_t triggered = 0;

/********************************************************************************************************
 * 											MAIN														*
 *******************************************************************************************************/
int main(void)
{
	MIDI_NoteOnFunc = &MIDI_NoteOn;
	SystemCoreClockUpdate();

	// IO setup
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_SWM);

    /* Pin Assign 8 bit Configuration */
    /* U0_RXD */
    LPC_SWM->PINASSIGN[0] = 0xffff00ffUL;
    /* SPI0_SCK */
    LPC_SWM->PINASSIGN[3] = 0x05ffffffUL;
    /* SPI0_MOSI */
    /* SPI0_SSEL */
    LPC_SWM->PINASSIGN[4] = 0xff03ff04UL;
    /* CTOUT_0 */
    LPC_SWM->PINASSIGN[6] = 0x02ffffffUL;

    /* Pin Assign 1 bit Configuration */
    LPC_SWM->PINENABLE0 = 0xffffffffUL;

    /*END OF PIN ASSIGNMENTS*/
	Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_SWM);

	// Enable the SPI interface to the DAC
	TLC_Init(TLC_LDAC_LOCATION);

	WaveGenInit(&Generator1, MIDIto12MhzReload[25]);

	/* Unhalt the counter to start */
	LPC_SCT->CTRL_U &= ~(1 << 2);

    // MIDI init
	MIDI_USARTInit(LPC_USART0, MIDI_ENABLERX);
	MIDI_SetAddress(LOCAL_ADDRESS);
	MIDI_Enable(LPC_USART0);

/////////////////////////////////////////////MAINLOOP.////////////////////////////////////////////////////
	while (1) {
		MIDI_ProcessRXBuffer();

		// If we need to generate any data from a pulse, do so.
		if (triggered)
		{
			//If we still need to generate noise, do so.
			if (remainingNoise != 0)
			{
				// get a random number
				int8_t noise = LFSR() & 0xFF;

				// calculate the strength of the pluck
				int32_t velocity = (pluckStrength * 1000) / 127;

				// depending on how much is left to the pluck, change the gain of the noise
				int32_t decay = (remainingNoise * 1000) / PULSE_LENGTH;

				if (remainingNoise > (PULSE_LENGTH - TRANSIENT_LENGTH))
				{
					noise = PULSE_LENGTH - remainingNoise;
				}

				noise = (noise * velocity)/1000;
				// update the noise variable accordingly
				noise = (noise * decay)/1000;

				noise = 127 + noise;

				uint8_t randBit = (uint8_t) noise;
				TLC_SetDACValue(PulseDAC, 1, &randBit);

				// Decrement the noise remaining
				remainingNoise--;
			}

			// else, turn off the interrupt, set the DAC to idle
			else
			{
				Chip_SCT_DisableEventInt(LPC_SCT, SCT_EVT_0);
				NVIC_DisableIRQ(SCT_IRQn);
			}
			// Clear the triggered variable
			triggered = 0;
		}// end pulse code

	}// end main loop

	return 0;
}

/********************************************************************************************************
 * 											FUNCTIONS													*
 *******************************************************************************************************/
void MIDI_NoteOn(uint8_t num, uint8_t vel)
{
	// Set up the scaling for the DACs and PWM.
	uint8_t value = num * 2;
	TLC_SetDACValue(0, 1, &value);

	// Knee for PWM adjustment in higher registers
	value = (num >= 84) ? 15 : 50;
	setWidth(&Generator1, value);

	// Set frequency generator's frequency.
	setReload(&Generator1, MIDIto12MhzReload[num % 128]);
	updateFreq(&Generator1);

	// Generate main pluck
	genPluck(PULSE_LENGTH, vel);
}

/**
 * Pluck Generation
 */
void genPluck(uint32_t cycleCount, uint8_t strength)
{
	pluckStrength = strength;
	remainingNoise = cycleCount;
	//TODO: set this up so we can generate noise based on any timer's overflow.
	Chip_SCT_EnableEventInt(LPC_SCT, SCT_EVT_0);
	NVIC_EnableIRQ(SCT_IRQn);
}

uint16_t LFSR()
{
	static uint16_t lfsr = 0xACE1u;
	uint8_t lsb = lfsr & 1;
	lfsr >>= 1;

	if (lsb == 1)
	{
		/* Only apply toggle mask if output bit is 1. */
		lfsr ^= 0xB400u;
	}
	return lfsr;
}

void SCT_IRQHandler(void)
{
	triggered = 1;
	// reset the interrupt flag
	Chip_SCT_ClearEventFlag(LPC_SCT, SCT_EVT_0);
}
