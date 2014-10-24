/*
===============================================================================
 Name        : KPS_VoiceClock.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#include "chip.h"
#include <cr_section_macros.h>
#include "FrequencyMaps.h"

#define UART0RX_BUFFSIZE 10 //Receive 10 values before overflowing the buffer

#define LED_LOCATION    (2)
#define ERR_DIV0 1
/*****************************************************************************
 * Type definitions
 ****************************************************************************/
typedef struct WaveGen_S{
	//ID
	uint8_t ID;
	//Freq
	float frequency;
	//Width
	char width;
} WaveGen;

/*****************************************************************************
 * Functions
 ****************************************************************************/

void setFreq(WaveGen* Generator, float frequency);
void setWidth(WaveGen* Generator, uint8_t percentage);
void WaveGenInit(WaveGen* Generator1, float freq);
void setMasterClockFreq(uint32_t freq);

void SCT_IRQHandler(void);
/*****************************************************************************
 * Variables
 ****************************************************************************/

WaveGen Generator1 = {
		//ID
		0,
		//Freq
		20,
		//Width
		75
};

WaveGen Generator2 = {
		//ID
		0,
		//Freq
		20,
		//Width
		75
};
uint8_t uartRXBuffArr[UART0RX_BUFFSIZE];
RINGBUFF_T uartRXBuff1;

/* Saved tick count used for a PWM cycle */
static uint32_t cycleTicks;


volatile int noteNumber = 0;
volatile int countdir = 1;



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

	uint8_t note = 48;
/////////////////////////////////////////////MAINLOOP.////////////////////////////////////////////////////

	while (1) {
		if (Chip_UART_Read(LPC_USART0, &note, 1))
		{
			setFreq(&Generator1, MIDItoBBD[(note % 127)]);
			Chip_UART_SendByte(LPC_USART0, note);

		}
	}
	return 0;
}

/********************************************************************************************************
 * 											FUNCTIONS													*
 *******************************************************************************************************/


void setWidth(WaveGen* Generator, uint8_t percentage)
{
	uint32_t value;
	uint32_t reload = Chip_Clock_GetSystemClockRate() / Generator->frequency;
	int genID = Generator->ID;

	// TODO: actually write some good code here instead of dumb code.
	if ((genID < 0) || (genID > 3)) {
		return;
	}

	// If highest, set match as close to beginning as possible
	if (percentage >= 100) {
		value = 1;
	}
	// If Lowest, set as close to frequency as possible
	else if (percentage == 0) {
		value = reload + 1;
	}
	else {
		uint32_t newTicks;

		newTicks = (reload * percentage)/100;

		/* Approximate duty cycle rate */
		value = reload - newTicks;
	}

	LPC_SCT->MATCHREL[genID + 1].U = value;
	Generator->width = percentage;
}

void setFreq(WaveGen* Generator, float frequency)
{
	// block Div0!
	if (frequency <=0 )
	{
		return;
	}
	// update the generator's frequency
	Generator->frequency = frequency;

	// update the width to match the frequency
	setWidth(Generator, Generator->width);

	// Update the associated Match Reload register
	int reload =(int)(Chip_Clock_GetSystemClockRate() / frequency);
	LPC_SCT->MATCHREL[Generator->ID].U = reload;
}

void WaveGenInit(WaveGen* Generator1, float freq)
{
	/* Chip specific SCT setup - clocks and peripheral reset
	 There are a lot of registers in the SCT peripheral. Performing
	 the reset allows the default states of the SCT to be loaded, so
	 we don't need to set them all and rely on defaults when needed. */
	Chip_SCT_Init(LPC_SCT);

	/* Configure the SCT as a 32bit counter using the bus clock */
	LPC_SCT->CONFIG = SCT_CONFIG_32BIT_COUNTER | SCT_CONFIG_CLKMODE_BUSCLK;

	/* Initial CTOUT0 state is high */
	LPC_SCT->OUTPUT = (7 << 0);

	/* The PWM will use a cycle time of (PWMCYCLERATE)Hz based off the bus clock */
	cycleTicks = Chip_Clock_GetSystemClockRate() / freq;

	/* Setup for match mode */
	LPC_SCT->REGMODE_L = 0;

	/* Setup match counter 0 for the number of ticks in a PWM sweep, event 0
	 will be used with the match 0 count to reset the counter.  */
	LPC_SCT->MATCH[0].U = cycleTicks;
	LPC_SCT->MATCHREL[0].U = cycleTicks;
	LPC_SCT->EVENT[0].CTRL = 0x00001000;
	LPC_SCT->EVENT[0].STATE = 0xFFFFFFFF;
	LPC_SCT->LIMIT_L = (1 << 0);

	/* For CTOUT0 to CTOUT2, event 1 is used to clear the output */
	LPC_SCT->OUT[0].CLR = (1 << 0);
	LPC_SCT->OUT[1].CLR = (1 << 0);

	/* Setup event 1 to trigger on match 1 and set CTOUT0 high */
	LPC_SCT->EVENT[1].CTRL = (1 << 0) | (1 << 12);
	LPC_SCT->EVENT[1].STATE = 1;
	LPC_SCT->OUT[0].SET = (1 << 1);

	setWidth(&*Generator1, 50);

	/* Don't use states */
	LPC_SCT->STATE_L = 0;
}

void SCT_IRQHandler(void)
{
	Chip_SCT_ClearEventFlag(LPC_SCT, SCT_EVT_0);
}



void setMasterClockFreq(uint32_t freq)
{
	LPC_PMU->PCON;

}
