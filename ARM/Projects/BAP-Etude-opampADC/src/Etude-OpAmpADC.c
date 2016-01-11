/*
===============================================================================
 Name        : Metaphase-MASK.c
 Author      : Matt Zapp
 Version     : 1.0
 Copyright   : June 12, 2015
 Description : A sandbox to experiment with the variables involved in converting
 	 	 	   a digital input pin into a Wilkinson ADC input, by using an
 	 	 	   extra digital output pin and a passive filter as a ramp generator
 	 	 	   feeding an op-amp comparator.

  	  	  	   Resetting the ramp every sample cycle and timing the interval to the
  	  	  	   falling edge of the comparator allows a conversion of a time into
  	  	  	   a voltage level.
===============================================================================
*/

#include <cr_section_macros.h>
#include "chip.h"
#include "global.h"
#include "BAP_32Sel.h"
#include "BAP_TLV_DAC.h"
#include "BAP_ADC121S0.h"
#include "BAP_Clk.h"



int main(void) {
	// Standard boot procedure
	CoreClockInit_30Hz();

	// Configure Pinout
    /* Enable SWM clock */
    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<7);

    /* Pin Assign 1 bit Configuration */
    LPC_SWM->PINENABLE0 = 0xffffffffUL;

    // Global Peripheral Enables:
    Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_GPIO);

    // Initialize and disable all Multi-rate timers
    Chip_MRT_Init();
	for (int i = 0; i < MRT_CHANNELS_NUM; i++) {
		Chip_MRT_SetDisabled(Chip_MRT_GetRegPtr(i));
	}

	// Configure ADC I/O
    OpAmpADCInit(ADC_MRT_CHAN, ADC_SAMPRATE, ADC_IN, ADC_OUT);


	while(1)
	{

	}
    return 0 ;
}
