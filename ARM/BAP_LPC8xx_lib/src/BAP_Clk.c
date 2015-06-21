/*
 * BAP_Clk.c
 *
 *  Created on: Apr 10, 2015
 *      Author: Matt
 *
 *  Clock Initialization code modified from Jasper Duba's original
 *  code to use the LPCOpen Libraries.
 */

#include "BAP_Clk.h"

#define PLL_60MHZ (0x24)

void CoreClockInit_30Hz() {

	 /* Enable the PLL Clock for a maximum 30MHZ SystemCoreClock */
	LPC_SYSCTL->SYSPLLCTRL = PLL_60MHZ; /* 60 MHZ pll output clock*/
	LPC_SYSCTL->PDRUNCFG &= ~(0x1 << 7); /* Power-up SYSPLL          */

	while (!(LPC_SYSCTL->SYSPLLSTAT & 0x01)){/* Wait Until PLL Locked*/}

	LPC_SYSCTL->MAINCLKSEL = 3; /* Select PLL Clock Output  */
	LPC_SYSCTL->MAINCLKUEN = 0x01; /* Update MCLK Clock Source */

	while (!(LPC_SYSCTL->MAINCLKUEN & 0x01)){/* Wait Until Updated*/}

	LPC_SYSCTL->SYSAHBCLKDIV  =  2; /* Divide by 2 to get the maximum 30MHZ SystemCoreClock */
	SystemCoreClockUpdate();
}


