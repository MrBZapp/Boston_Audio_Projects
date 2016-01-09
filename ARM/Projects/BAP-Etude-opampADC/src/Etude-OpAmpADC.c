/*
===============================================================================
 Name        : Metaphase-MASK.c
 Author      : Matt Zapp
 Version     : 1.0
 Copyright   : June 12, 2015
 Description : A sandbox to experiment with the variables involved in converting
 	 	 	   a digital input pin into an ADC ramp-compare input, by using an
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

#define SAMPLE_WIDTH 0x0FFF
#define FILTER_SIZE 4

void OpAmpADCInit(uint8_t chan, uint32_t samp_rate, uint8_t in, uint8_t out)

int main(void) {
	// Standard boot procedure
	CoreClockInit_30Hz();

	// Configure Pinout
    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<7);
    LPC_SWM->PINENABLE0 = 0xffffffffUL;


	// Global Peripheral Enables:
    Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_GPIO);
    Chip_MRT_Init();

	// Configure ADC I/O
    OpAmpADCInit(ADC_CHAN, ADC_SAMPRATE, ADC_IN, ADC_OUT);

	while(1)
	{

	}
    return 0 ;
}

void OpAmpADCInit(uint8_t chan, uint32_t samp_rate, uint8_t in, uint8_t out)
{
	/* ENABLE MAIN SAMPLING LOOP */
	// Get pointer to desired timer channel
	LPC_MRT_CH_T *pMRT = LPC_MRT_CH(chan);

	// Setup timer with sample rate.  Load immediate.
	Chip_MRT_SetInterval(pMRT, (Chip_Clock_GetSystemClockRate() / samp_rate) | MRT_INTVAL_LOAD);

	// Set to regularly sample
	Chip_MRT_SetMode(pMRT, MRT_MODE_REPEAT);

	// Configure GPIO according to ins/outs
	LPC_GPIO_PORT->DIR[0] |= (1 << out);
	LPC_GPIO_PORT->DIR[0] |= (0 << in);

	// Pull transfer start pin low
	LPC_GPIO_PORT->CLR = 1 << out;

	// Set up pin-change interrupts for falling-edge
	// Clear pending pin-change interrupt and enable the interrupt


	// Clear pending interrupt and enable timer to start sampling
	Chip_MRT_IntClear(pMRT);
	Chip_MRT_SetEnabled(pMRT);
}
