/*
 * BAP_WaveGen.c
 *
 *  Created on: Oct 24, 2014
 *      Author: mzapp
 */

#include "chip.h"
#include "BAP_WaveGen.h"

/*****************************************************************************
 * Global Variables															 *
 ****************************************************************************/
WaveGen Generator1 = {
		//ID
		0,
		//Freq
		20,
		//Width
		50
};

WaveGen Generator2 = {
		//ID
		1,
		//Freq
		20,
		//Width
		50
};


/********************************************************************************************************
 * 											FUNCTIONS													*
 *******************************************************************************************************/
void WaveGenInit(WaveGen* Generator, float freq)
{
	//TODO: Get the hard-coded crap out of here.

	/* Chip specific SCT setup - clocks and peripheral reset
	 There are a lot of registers in the SCT peripheral. Performing
	 the reset allows the default states of the SCT to be loaded, so
	 we don't need to set them all and rely on defaults when needed. */
	Chip_SCT_Init(LPC_SCT);

	/* Configure the SCT as a 32bit counter using the bus clock */
	LPC_SCT->CONFIG = SCT_CONFIG_32BIT_COUNTER | SCT_CONFIG_CLKMODE_BUSCLK;

	/* Initial CTOUT0 state is high */
	LPC_SCT->OUTPUT = (7 << 0);

	// Block Div0!
	if (freq == 0)
	{
		freq = 1;
	}

	/* The PWM will use a cycle time of (PWMCYCLERATE)Hz based off the bus clock */
	uint32_t cycleTicks = Chip_Clock_GetSystemClockRate() / freq;

	/* Setup for match mode */
	LPC_SCT->REGMODE_L = 0;

	/* Setup match counter 0 for the number of ticks in a PWM sweep, event 0
	 will be used with the match 0 count to reset the counter.  */
	LPC_SCT->MATCH[0].U = cycleTicks;
	LPC_SCT->MATCHREL[0].U = cycleTicks;
	LPC_SCT->EVENT[0].CTRL = 0x00001000;
	LPC_SCT->EVENT[0].STATE = 0xFFFFFFFF;
	LPC_SCT->LIMIT_L = (1 << 0);

	/* For CTOUT0, event 1 is used to clear the output */
	LPC_SCT->OUT[0].CLR = (1 << 0);
	LPC_SCT->OUT[1].CLR = (1 << 0);

	/* Setup event 1 to trigger on match 1 and set CTOUT0 high */
	LPC_SCT->EVENT[1].CTRL = (1 << 0) | (1 << 12);
	LPC_SCT->EVENT[1].STATE = 1;
	LPC_SCT->OUT[0].SET = (1 << 1);

	setWidth(Generator, 50);

	/* Don't use states */
	LPC_SCT->STATE_L = 0;
}


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

int calcReload(float freq)
{
	return 0;
}

void setReload(WaveGen* Generator, int reload)
{
	LPC_SCT->MATCHREL[Generator->ID].U = reload;
}

void updateFreq(WaveGen* Generator)
{
	Generator->frequency = Chip_Clock_GetSystemClockRate() / LPC_SCT->MATCHREL[Generator->ID].U;
	setWidth(Generator, Generator->width);
}
