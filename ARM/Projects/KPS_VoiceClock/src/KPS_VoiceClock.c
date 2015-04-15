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
#include "BAP_Clk.h"
#include "BAP_Midi.h"
#include "FrequencyMaps.h"
#include "BAP_WaveGen.h"
#include "BAP_TLC_DAC.h"
#include "BAP_Debug.h"

// Define TLV DAC functions
#define FilterDAC TLC_DAC_1
#define AmpDAC TLC_DAC_2
#define PulseDAC TLC_DAC_4

// Define Address
#define LOCAL_ADDRESS 0x00

// Define Pulse characteristics
#define PULSE_LENGTH 384
#define TRANSIENT_LENGTH 128
#define RELEASE_LENGTH 1024
#define BIAS 127

// exciter generation forward declarations
void SCT_IRQHandler(void);
void MIDI_NoteOn(uint8_t note, uint8_t vel);
void MIDI_NoteOff(uint8_t note, uint8_t ignore);
uint16_t LFSR();

// exciter types
void GenPulse();
void GenRelease();


/*****************************************************************************
 * IRQ-accessible Variables																 *
 ****************************************************************************/
volatile uint8_t triggered = 0;
volatile uint32_t remainingNoise = 0;

/*****************************************************************************
 * GLOBAL Variables
 ****************************************************************************/
uint8_t pluckStrength = 0;
int8_t activeNote = -1;

uint8_t ampTable[6] = {130, 130, 148, 169, 188, 211};

/********************************************************************************************************
 * 											MAIN														*
 *******************************************************************************************************/
int main(void)
{
	// Standard boot procedure
	CoreClockInit_30Hz();

	// Global Inits:
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_SCT); // Clock used for freq generation and envelope timing
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_UART0); // UART0 used for MIDI
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_SPI0); // SPI0 used for DAC control

	// Ready To assign Pinouts
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

	Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_SWM);
	/*END OF PIN ASSIGNMENTS*/

	// Configure the SPI to use TLC DAC settings
	TLC_Init();

	// Initialize the frequency generation timer
	WaveGenInit(&Generator1, MIDIto30MhzReload[25]);
	WaveGenStart(&Generator1);

    // Configure the USART to Use MIDI protocol
	MIDI_USARTInit(LPC_USART0, MIDI_ENABLERX);
	MIDI_SetAddress(LOCAL_ADDRESS);

	// Assign note on and off functions
	MIDI_NoteOnFunc = &MIDI_NoteOn;
	MIDI_NoteOffFunc = &MIDI_NoteOff;

	MIDI_Enable(LPC_USART0);

/////////////////////////////////////////////MAINLOOP.////////////////////////////////////////////////////
	while (1) {
		// Check if we've received any data
		MIDI_ProcessRXBuffer();

		// If the timer has requested a sample, we need to generate any data from a pulse, do so.
		if (triggered)
		{

			GenPulse();
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
	activeNote = num;
	uint8_t value = 127 - num;

	// Set up the scaling for the DACs and PWM.
	TLC_SetDACValue(FilterDAC, 1, &value);
	value = ampTable[((num % 66) / 12) % 6] + (num % 12);
	TLC_SetDACValue(AmpDAC, 0, &value);

	// Knee for PWM adjustment in higher registers
	value = (num >= 84) ? 15 : 50;
	setWidth(&Generator1, value);

	// Load exciter variables
	pluckStrength = vel;
	remainingNoise = PULSE_LENGTH;

	// start the pluck
	Chip_SCT_EnableEventInt(LPC_SCT, SCT_EVT_0);
	NVIC_EnableIRQ(SCT_IRQn);

	// Set frequency generator's frequency.
	setReload(&Generator1, MIDIto30MhzReload[num % 66]);
	updateFreq(&Generator1);
}

void MIDI_NoteOff(uint8_t note, uint8_t ignore)
{
	if (note == activeNote)
	{
		uint8_t value = 255 - activeNote;
		TLC_SetDACValue(FilterDAC, 1, &value);
		// set to Idle
		activeNote = -1;
	}
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
	//If we still need to generate a signal, do so.
	if (remainingNoise != 0)
	{
		// Enable the trigger
		triggered = 1;

		// Decrement the noise remaining
		remainingNoise--;
	}

	else
	{

		Chip_SCT_DisableEventInt(LPC_SCT, SCT_EVT_0);
		NVIC_DisableIRQ(SCT_IRQn);
	}

	// reset the interrupt flag
	Chip_SCT_ClearEventFlag(LPC_SCT, SCT_EVT_0);
}


/////// EXCITER TYPES


void GenPulse()
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

	// update the noise variable accordingly
	noise = (noise * velocity)/1000;
	noise = (noise * decay)/1000;

	// Set bias to be 1/2 available range
	noise = BIAS + noise;

	uint8_t randBit = (uint8_t) noise;
	TLC_SetDACValue(PulseDAC, 1, &randBit);
}

