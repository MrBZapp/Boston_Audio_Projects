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

// Standard lib includes
#include "chip.h"
#include <cr_section_macros.h>
#include <stdbool.h>

// project global includes
#include "GlobalDEF.h"
#include "LocalMIDIFunc.h"
#include "KPS_Exciter.h"

// BAP_lib includes
#include "BAP_math.h"
#include "BAP_Clk.h"
#include "BAP_Midi.h"
#include "BAP_TLC_DAC.h"
#include "BAP_WaveGen.h"
#include "BAP_32Sel.h"

void SCT_IRQHandler(void);
/*****************************************************************************
 * IRQ-accessible Variables													 *
 ****************************************************************************/
volatile bool triggered = 0;

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
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_ACOMP);// ACOMP used to select between envelope variables

	// Ready To assign Pinouts
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_SWM);

    /* Pin Assign 8 bit Configuration */
    /* U0_RXD */
    LPC_SWM->PINASSIGN[0] = 0xffff01ffUL;
    /* SPI0_SCK */
    LPC_SWM->PINASSIGN[3] = 0x05ffffffUL;
    /* SPI0_MOSI */
    /* SPI0_SSEL */
    LPC_SWM->PINASSIGN[4] = 0xff03ff04UL;
    /* CTOUT_0 */
    LPC_SWM->PINASSIGN[6] = 0x02ffffffUL;

    /* Pin Assign 1 bit Configuration */
    /* ACMP_I1 */
    LPC_SWM->PINENABLE0 = 0xfffffffeUL;

	Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_SWM);
	/*END OF PIN ASSIGNMENTS*/

	// Configure the SPI to use TLC DAC settings
	TLC_Init();

	// Enable the mode selector
	Selector_Init(LPC_CMP);

	// Initialize the frequency generation timer
	WaveGen_Init(&Generator1, 200);
	WaveGen_Start(&Generator1);

	// Initialize the exciter envelope
	envNode_t ADSRarr[3] = {
			{ATTACK, ENVAMP_MAX, FALSE},
			{DECAY, SUSTAIN, TRUE},
			{RELEASE, 0, TRUE}
	};
	EnvInit(&GlobalEnv, ADSRarr, 3);

    // Configure the USART to Use MIDI protocol
	MIDI_USARTInit(LPC_USART0, MIDI_ENABLERX);
	MIDI_SetAddress(LOCAL_ADDRESS);

	// Assign note on and off functions
	MIDI_NoteOnFunc = &MIDI_NoteOn;
	MIDI_NoteOffFunc = &MIDI_NoteOff;

	MIDI_Enable(LPC_USART0);

	uint8_t prev_ValueA = BIAS;
	TLC_SetDACValue(PulseDAC, 1, &prev_ValueA);
	uint8_t ladderValue = 0;
	uint32_t lastTest = 0;
/////////////////////////////////////////////MAINLOOP.////////////////////////////////////////////////////
	while (1) {
		uint8_t cmpVal = ((LPC_CMP->LAD & ACMP_LADSEL_MASK) >> 1);
		if (!(LPC_CMP->CTRL & ACMP_COMPSTAT_BIT))
		{
			ADSRarr[1].length = i_lscale(0, 31, 0, 200, lastTest);
		}
		else
		{
			lastTest = cmpVal;
		}

		// Check if we've received any data
		MIDI_ProcessRXBuffer();

		// If the timer has requested a sample from the exciter...
		if (triggered != 0)
		{
			ladderValue++;
			ladderValue %= 31;
			Chip_ACMP_SetupVoltLadder(LPC_CMP, ladderValue, FALSE);
			if (ladderValue == 0)
			{

			}
			// Get a sample
			uint8_t DAC_Value = GenExciter(&GlobalEnv);

			// check if it's different from the previous value
			if (DAC_Value != prev_ValueA)
			{
				TLC_SetDACValue(PulseDAC, 1, &DAC_Value);
				prev_ValueA = DAC_Value;
			}

			// Clear the triggered variable
			triggered = 0;
		}// end exciter code
	}// end main loop
	return 0;
}// end MAIN


/********************************************************************************************************
 * 										Functions														*
 *******************************************************************************************************/
void SCT_IRQHandler(void)
{
	triggered = 1;
	// reset the interrupt flag
	Chip_SCT_ClearEventFlag(LPC_SCT, SCT_EVT_0);
}
