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
#include <stdbool.h>
#include "BAP_math.h"
#include "BAP_Envelope.h"
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
#define PULSE_LENGTH 300
#define TRANSIENT_LENGTH 0
#define SUSTAIN 50
#define RELEASE 10000
#define BIAS 127

// exciter generation forward declarations
void SCT_IRQHandler(void);
void MIDI_NoteOn(uint8_t note, uint8_t vel);
void MIDI_NoteOff(uint8_t note, uint8_t ignore);
uint16_t LFSR();

// exciter types

void GenPluckBow(envLinADSR_t* Envelope, uint32_t Position);
void GenPluckBow_NoteOff(envLinADSR_t* Envelope, int32_t Position);

/*****************************************************************************
 * IRQ-accessible Variables																 *
 ****************************************************************************/
volatile uint8_t triggered = 0;
volatile uint32_t remainingNoise = 0;

/*****************************************************************************
 * GLOBAL Variables
 ****************************************************************************/
uint8_t pluckStrength = 0;
bool ACMP_Status;
int8_t activeNote = -1;
int32_t EnvPosition = 0;
uint8_t ampTable[6] = {130, 130, 148, 169, 188, 211};


/********************************************************************************************************
 * 											MAIN														*
 *******************************************************************************************************/
int main(void)
{
	// Standard boot procedure
	CoreClockInit_30Hz();

	// Global Peripheral Enables:
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_SCT); // Clock used for freq generation and envelope timing
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_UART0); // UART0 used for MIDI
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_SPI0); // SPI0 used for DAC control
//	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_ACOMP);// ACOMP used to select between envelope variables


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

	// Configure ACOMP
//	Chip_ACMP_EnableVoltLadder(LPC_CMP);
//	Chip_ACMP_SetupVoltLadder(LPC_CMP, 0, FALSE);
//	ACMP_Status = Chip_ACMP_GetCompStatus(LPC_CMP);

	// Initialize the frequency generation timer
	WaveGenInit(&Generator1, MIDIto30MhzReload[25]);
	WaveGenStart(&Generator1);

	// Initialize the envelope
	envLinADSR_t envelope;
	SetEnvelopeTimes(&envelope, TRANSIENT_LENGTH, PULSE_LENGTH, SUSTAIN,0x0);

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
		if (triggered != 0)
		{
			//Hard Stop at Max.
			if (EnvPosition < 0)
			{
				EnvPosition = INT32_MAX;
			}

			// Generate the sound
			if (activeNote == -1)
			{
				GenPluckBow_NoteOff(&envelope, EnvPosition);
			}
			else
			{
				GenPluckBow(&envelope, EnvPosition);
			}

			// Clear the triggered variable
			triggered = 0;

			// Advance the Envelope Position
			EnvPosition++;
		}// end exciter code


	}// end main loop
	return 0;
}

/********************************************************************************************************
 * 											FUNCTIONS													*
 *******************************************************************************************************/


void MIDI_NoteOn(uint8_t num, uint8_t vel)
{
	// Load global variables
	activeNote = num;
	pluckStrength = vel;
	EnvPosition = 0;
	remainingNoise = PULSE_LENGTH;

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


/////// EXCITER TYPES


void GenPluckBow(envLinADSR_t* Envelope, uint32_t Position)
{
	if (Position < Envelope->dk || Envelope->sus != 0)
	{
		int32_t envVal = GenLinADS(Envelope, Position);
		int32_t noise = LFSR() & 0x7F;

		// calculate the strength of the pluck

		// Generate the attack
		//if (Position < Envelope->atk)
		//{
		//	noise = (uint8_t) envVal;
		//}

		// scale the amplitude
		noise = i_lscale(0, 127, 0, pluckStrength, noise);
		noise = i_lscale(0,127, 0, envVal, noise);

		// Set bias to be 1/2 available range
		noise = 127 + noise;
		uint8_t output = noise;
		TLC_SetDACValue(PulseDAC, 1, &output);
	}
}

void GenPluckBow_NoteOff(envLinADSR_t* Envelope, int32_t Position)
{
	if (Position <= RELEASE && Envelope->sus != 0)
	{
		int32_t envVal = GenLinRelease(Envelope, Position + Envelope->dk);
		int32_t noise = LFSR() & 0x7F;

		// scale the amplitude
		noise = i_lscale(0, 127, 0, pluckStrength, noise);
		noise = i_lscale(0,127, 0, envVal, noise);

		// Set bias to be 1/2 available range
		noise = 127 + noise;
		uint8_t output = noise;
		TLC_SetDACValue(PulseDAC, 1, &output);
	}
}

void SCT_IRQHandler(void)
{
	triggered = 1;
	// reset the interrupt flag
	Chip_SCT_ClearEventFlag(LPC_SCT, SCT_EVT_0);
}
