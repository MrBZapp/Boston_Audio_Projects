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

#include "GlobalDEF.h"
#include "LocalMIDIFunc.h"
#include "BAP_Debug.h"

#include "BAP_Clk.h"
#include "BAP_Midi.h"
#include "BAP_TLC_DAC.h"
#include "BAP_WaveGen.h"

#include "KPS_Exciter.h"

// Define Pulse characteristics
#define PULSE_LENGTH 300
#define TRANSIENT_LENGTH 0
#define SUSTAIN 50
#define RELEASE 10000
#define BIAS 127

// exciter generation forward declarations
void SCT_IRQHandler(void);

/*****************************************************************************
 * IRQ-accessible Variables																 *
 ****************************************************************************/
volatile uint8_t triggered = 0;

/*****************************************************************************
 * GLOBAL Variables
 ****************************************************************************/

bool ACMP_Status;


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
	WaveGenInit(&Generator1, 200);
	WaveGenStart(&Generator1);

	// Initialize the envelope
	envLinADSR_t envelope;
	SetEnvelopeTimes(&envelope, TRANSIENT_LENGTH, PULSE_LENGTH, SUSTAIN, RELEASE);

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
 * 										Functions														*
 *******************************************************************************************************/
void SCT_IRQHandler(void)
{
	triggered = 1;
	// reset the interrupt flag
	Chip_SCT_ClearEventFlag(LPC_SCT, SCT_EVT_0);
}
