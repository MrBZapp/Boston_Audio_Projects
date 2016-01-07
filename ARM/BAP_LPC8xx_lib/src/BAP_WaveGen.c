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
void WaveGen_Init(WaveGen* Generator, float freq)
{
	//TODO: Get the hard-coded crap out of here.

	/* Configure the SCT as a 32bit counter using the bus clock */
	LPC_SCT->CONFIG = SCT_CONFIG_32BIT_COUNTER | SCT_CONFIG_CLKMODE_BUSCLK;

	/* Initial CTOUT0 state is high */
	LPC_SCT->OUTPUT = (7 << 0);

	// Block Div0!
	if (freq == 0)
	{
		freq = 1;
	}

	/* Setup for match mode */
	LPC_SCT->REGMODE_L = 0;

	// what does this do?
	//LPC_SCT->EVENT[0].CTRL = 0x00001000;

	// By writing all ones, all events are unmasked and allowed to happen.
	//LPC_SCT->EVENT[0].STATE = 0xFFFFFFFF;

	// Event 0 is used to limit the counter
	LPC_SCT->LIMIT_L = (1 << 0);

	/* For CTOUT0, event 1 is used to clear the output */

	LPC_SCT->OUT[0].CLR = (1 << 0);
	LPC_SCT->OUT[1].CLR = (1 << 0);

	/* Setup event 1 to trigger on match 1 and set CTOUT0 high */
	//LPC_SCT->EVENT[1].CTRL = (1 << 0) | (1 << 12);
	// What does this do?
	//LPC_SCT->EVENT[1].STATE = 1;
	LPC_SCT->OUT[0].SET = (1 << 1);

	WaveGen_SetWidth(Generator, 50);

	/* Don't use states */
	// Do we need this?  resets to 0.
	LPC_SCT->STATE_L = 0;
}


void WaveGen_Start(WaveGen* Generator)
{
	switch(Generator->ID)
	{
	case 0:
		LPC_SCT->CTRL_U &= ~(1 << 2);
		break;
	case 1:
		break;
	}
}


void WaveGen_SetWidth(WaveGen* Generator, uint8_t percentage)
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

void WaveGen_SetWidthf(WaveGen* Generator, float percentage)
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

void WaveGen_SetFreqf(WaveGen* Generator, float frequency)
{
	// block Div0!
	if (frequency <=0 )
	{
		return;
	}
	// update the generator's frequency
	Generator->frequency = frequency;

	// update the width to match the frequency
	WaveGen_SetWidth(Generator, Generator->width);

	// Update the associated Match Reload register
	int reload =(int)(Chip_Clock_GetSystemClockRate() / frequency);
	LPC_SCT->MATCHREL[Generator->ID].U = reload;
}


void WaveGen_SetReload(WaveGen* Generator, int reload)
{
	LPC_SCT->MATCHREL[Generator->ID].U = reload;
}


void WaveGen_UpdateFreq(WaveGen* Generator)
{
	Generator->frequency = Chip_Clock_GetSystemClockRate() / LPC_SCT->MATCHREL[Generator->ID].U;
	WaveGen_SetWidth(Generator, Generator->width);
}
