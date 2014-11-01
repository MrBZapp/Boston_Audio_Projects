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
#include "BAP_WaveGen.h"

typedef void (*VoidFuncPointer)();

#define UART0RX_BUFFSIZE 10 //Receive 10 values before overflowing the buffer
#define LED_LOCATION  2

void setMasterClockFreq(uint32_t freq);
void genNoisePulse(WaveGen* Generator, uint32_t cycleCount);
void genNoiseService();
void genNoise();
void SCT_IRQHandler(void);

/*****************************************************************************
 * Variables																 *
 ****************************************************************************/
uint8_t uartRXBuffArr[UART0RX_BUFFSIZE];
RINGBUFF_T uartRXBuff1;

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

	Chip_Clock_SetUARTClockDiv(1);	/* divided by 1 */

	Chip_SWM_DisableFixedPin(SWM_FIXED_ACMP_I1);
	Chip_SWM_DisableFixedPin(SWM_FIXED_SWDIO);

	// Assign CTOUT0 to pin 0.
	Chip_SWM_MovablePinAssign(SWM_CTOUT_0_O, 3);
	// Assign UART0 Receive to pin 4.
	Chip_SWM_MovablePinAssign(SWM_U0_RXD_I, 0);
	Chip_SWM_MovablePinAssign(SWM_U0_TXD_O, 4);

	Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_SWM);

    LPC_GPIO_PORT->DIR[0] |= (1 << LED_LOCATION);

    //UART0 initialization
	Chip_UART_Init(LPC_USART0);
	Chip_UART_ConfigData(LPC_USART0, UART_CFG_DATALEN_8 | UART_CFG_PARITY_NONE | UART_CFG_STOPLEN_1);
	Chip_UART_SetBaud(LPC_USART0, 115200);
	LPC_USART0->CFG |= UART_CFG_ENABLE;

	WaveGenInit(&Generator1, MIDItoFreq[0]);

	/* Unhalt the counter to start */
	LPC_SCT->CTRL_U &= ~(1 << 2);


	uint8_t status = 48;
	uint8_t value = 0;
/////////////////////////////////////////////MAINLOOP.////////////////////////////////////////////////////

	while (1) {
		if (Chip_UART_Read(LPC_USART0, &value, 1))
		{
			if ( value == 0xF1 || value == 0xF2 )
			{
				status = value & 0x0F;
			}
			else
			{
				switch (status){
				case(0x01):
						setFreq(&Generator1, MIDItoBBD[(value % 127)]);
						genNoisePulse(&Generator1, 256);
						break;
				case(0x02):
						setWidth(&Generator1, value);
						break;
				default:
					break;
				}
			}
		}
	//	genNoise();

	}
	return 0;
}

/********************************************************************************************************
 * 											FUNCTIONS													*
 *******************************************************************************************************/

void SCT_IRQHandler(void)
{

//	LPC_GPIO_PORT->NOT[0] = 1 << LED_LOCATION;
	if (interruptFunc != 0)
	{
		interruptFunc();
	}
	Chip_SCT_ClearEventFlag(LPC_SCT, SCT_EVT_0);
}


void setMasterClockFreq(uint32_t freq)
{
	LPC_PMU->PCON;
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
		LPC_GPIO_PORT->PIN[0] = (randBit << LED_LOCATION);
		// Decrement the noise remaining
		remainingNoise--;
	}
	else
	{
	// else, turn off the interrupt, set the LED low
		LPC_GPIO_PORT->CLR[0] = 1 << LED_LOCATION;
		Chip_SCT_DisableEventInt(LPC_SCT, SCT_EVT_0);
		NVIC_DisableIRQ(SCT_IRQn);
	}
}

void genNoise()
{
	uint16_t randBit = LFSR() & 0x1;
	LPC_GPIO_PORT->PIN[0] = (randBit << LED_LOCATION);
}

