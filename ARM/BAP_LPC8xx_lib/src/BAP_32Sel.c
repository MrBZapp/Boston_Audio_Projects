/*
 * BAP_32Sel.c
 *
 *  Created on: Apr 25, 2015
 *      Author: Matt
 */

#include "BAP_32Sel.h"

void Selector_Init(LPC_CMP_T* ACMP){
	// Turn on the analog comparator
	Chip_SYSCTL_PowerUp(SYSCTL_SLPWAKE_ACMP_PD);

	// Enable the voltage ladder
	Chip_ACMP_EnableVoltLadder(LPC_CMP);

	// Set up comparator:
	// set COMPEDGE on both edges, use the ACMP_in1 as the positive input, the ladder as the negative, with no hysteresis.
	Chip_ACMP_SetupAMCPRefs(LPC_CMP, ACMP_EDGESEL_BOTH, ACMP_POSIN_ACMP_I2, ACMP_NEGIN_VLO, ACMP_HYS_NONE);
}



uint32_t Selector_GetValue()
{
	static uint32_t lastValue = 0;
	uint32_t lasttest = 0;
	uint32_t matches = 0;
	uint32_t loopcnt = 0;
	if (loopcnt < SEL_LOOPMAX)
	{
		uint32_t test = (LPC_CMP->CTRL & ACMP_COMPSTAT_BIT);
		if (test == lasttest)
			matches++;

		if (matches > SEL_MATCHCNT)
		{
			lastValue = LPC_CMP->LAD & ACMP_LADREF_MASK;
		}

		lasttest = test;
	}
	return (lastValue >> 1);
}


uint32_t SelectorGetValue(LPC_CMP_T* ACMP){
	for (int i = 32; i != 0; i--)
	{
		Chip_ACMP_SetupVoltLadder(LPC_CMP, i , FALSE);
		if ((ACMP->CTRL & ACMP_COMPSTAT_BIT))
		{
			return i;
		}
	}
	// if we reach here it's a problem.
	return 1;
}
