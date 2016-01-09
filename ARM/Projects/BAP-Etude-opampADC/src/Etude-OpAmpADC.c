/*
===============================================================================
 Name        : Metaphase-MASK.c
 Author      : Matt Zapp
 Version     : 1.0
 Copyright   : June 12, 2015
 Description : main definition of Mask 2-input comparison module
===============================================================================
*/

#include <cr_section_macros.h>
#include "chip.h"
#include "global.h"
#include "BAP_32Sel.h"
#include "BAP_TLV_DAC.h"
#include "BAP_ADC121S0.h"
#include "BAP_Clk.h"

#define SAMPLE_WIDTH 0x0FFF
#define FILTER_SIZE 4

void OpAmpADCInit(uint8_t in, uint8_t out);

int main(void) {
	// Standard boot procedure
	CoreClockInit_30Hz();

	// Configure Pinout
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_SWM);

#ifdef LPC810
    /* Enable SWM clock */
    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<7);

    /* Pin Assign 1 bit Configuration */
    LPC_SWM->PINENABLE0 = 0xffffffffUL;

#endif

	// Global Peripheral Enables:
    Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_GPIO);

	// Configure ADC I/O
	// TODO: get rid of this
	LPC_GPIO_PORT->DIR[0] |= 1 << ADC_OUT;
	LPC_GPIO_PORT->DIR[0] |= 1 << ADC_IN;

	while(1)
	{

	}
    return 0 ;
}
