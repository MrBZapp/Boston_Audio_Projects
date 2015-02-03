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
#include "FrequencyMaps.h"
#include "BAP_Midi.h"
#include "BAP_WaveGen.h"
#include "BAP_TLV_DAC.h"
#include "BAP_Type.h"
#include "BAP_math.h"

// I/O setup
#define SERIAL_IN_LOCATION 0
#define CLOCK_OUT_LOCATION 2
#define TRIGGER_LOCATION  1
#define SPI_SCK_LOCATION 3
#define SPI_MOSI_LOCATION 4
#define SPI_SEL_LOCATION 5

// Define TLV DAC functions
#define FilterDAC TLV_DAC_1
#define FeedbackDAC TLV_DAC_2

// Define Address
#define LOCAL_ADDRESS 0x1

void genNoisePulse(WaveGen* Generator, uint32_t cycleCount);
void genNoiseService();
void genNoise();
void serviceNote(uint8_t noteValue);
void SCT_IRQHandler(void);

/*****************************************************************************
 * Variables																 *
 ****************************************************************************/


volatile VoidFuncPointer interruptFunc = 0;
volatile uint32_t remainingNoise = 0;

/********************************************************************************************************
 * 											MAIN														*
 *******************************************************************************************************/
int main(void)
{
	SystemCoreClockUpdate();

	// IO setup
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_SWM);

    /* U0_RXD */
    LPC_SWM->PINASSIGN[0] = 0xffff00ffUL;
    /* SPI0_SCK */
    LPC_SWM->PINASSIGN[3] = 0x03ffffffUL;
    /* SPI0_MOSI */
    /* SPI0_SSEL */
    LPC_SWM->PINASSIGN[4] = 0xff05ff04UL;
    /* CTOUT_0 */
    LPC_SWM->PINASSIGN[6] = 0x02ffffffUL;

    /* Pin Assign 1 bit Configuration */
    LPC_SWM->PINENABLE0 = 0xffffffffUL;

	// Setup trigger location
    LPC_GPIO_PORT->DIR[0] |= (1 << TRIGGER_LOCATION);

    /*END OF PIN ASSIGNMENTS*/
	Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_SWM);

	// Enable the SPI interface to the DAC
	TLV_Init();

	WaveGenInit(&Generator1, MIDIto12MhzReload[25]);

	/* Unhalt the counter to start */
	LPC_SCT->CTRL_U &= ~(1 << 2);


    // UART0 initialization
	Chip_Clock_SetUARTClockDiv(1);	/* divided by 1 */
	Chip_UART_Init(LPC_USART0);
	Chip_UART_ConfigData(LPC_USART0, UART_CFG_DATALEN_8 | UART_CFG_PARITY_NONE | UART_CFG_STOPLEN_1);
	Chip_UART_SetBaud(LPC_USART0, 115200);

	// UART variables
	uint8_t status = 48;
	uint8_t address = LOCAL_ADDRESS;
	uint8_t value = 0;

	// Enable the UART to start receiving messages
	LPC_USART0->CFG |= UART_CFG_ENABLE;


/////////////////////////////////////////////MAINLOOP.////////////////////////////////////////////////////

	while (1) {
		if (Chip_UART_Read(LPC_USART0, &value, 1))
		{
			if ( (value & MIDI_STATBIT) )
			{
				status = value & MIDI_STATMSK;
				address = value & MIDI_CHMSK;
			}
			else if (address == LOCAL_ADDRESS)
			{
				switch (status)
				{
				case(MIDI_NOTEON):
						// Set up the scaling for the DACs and PWM.
						serviceNote(value);

						// Set frequency generator's frequency.
						setReload(&Generator1, MIDIto12MhzReload[value % 128]);
						updateFreq(&Generator1);

						break;
				default:
					break;
				}
			}
		}
	}
	return 0;
}

/********************************************************************************************************
 * 											FUNCTIONS													*
 *******************************************************************************************************/

void SCT_IRQHandler(void)
{
	if (interruptFunc != 0)
	{
		interruptFunc();
	}
	Chip_SCT_ClearEventFlag(LPC_SCT, SCT_EVT_0);
}

void serviceNote(uint8_t note)
{
	uint16_t value = (uint16_t) i_lscale(0, 127, 0, DACSIZE - 1, note);
	TLV_SetDACValue(FilterDAC, 0, value);

	value = (uint16_t) i_lscale(0, 127, DACSIZE - 1, 0, note);
	TLV_SetDACValue(FeedbackDAC, 0, value);

	if (note >= 84)
	{
		value = 15;
	}
	else
	{
		value = 50;
	}
	setWidth(&Generator1, (value % 100));
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


void genNoisePulse(WaveGen* Generator, uint32_t cycleCount)
{
	remainingNoise = cycleCount;
	interruptFunc = &genNoiseService;
	//TODO: set this up so we can generate noise based on any timer's overflow.
	Chip_SCT_EnableEventInt(LPC_SCT, SCT_EVT_0);
	NVIC_EnableIRQ(SCT_IRQn);
}


void genNoiseService()
{

	//If we still need to generate noise, do so.
	if (remainingNoise != 0)
	{
		uint16_t randBit = LFSR() & 0x1;
		LPC_GPIO_PORT->PIN[0] = (randBit << TRIGGER_LOCATION);
		// Decrement the noise remaining
		remainingNoise--;
	}
	else
	{
	// else, turn off the interrupt, set the LED low
		LPC_GPIO_PORT->CLR[0] = 1 << TRIGGER_LOCATION;
		Chip_SCT_DisableEventInt(LPC_SCT, SCT_EVT_0);
		NVIC_DisableIRQ(SCT_IRQn);
	}
}

void genNoise()
{
	uint16_t randBit = LFSR() & 0x1;
	LPC_GPIO_PORT->PIN[0] = (randBit << TRIGGER_LOCATION);
}

