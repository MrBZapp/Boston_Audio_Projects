/*
 * global.h
 *
 *  Created on: Jun 20, 2015
 *      Author: Matt
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

#include "BAP_32Sel.h"
#include "BAP_math.h"

#define LPC810
#define LED_DEBUG (0)
#define ADC_IN (3)
#define ADC_OUT (4)
#define ADC_MRT_CHAN (0)
#define ADC_SAMPRATE (50000)


// Global ADC variables for interrupt access
volatile uint32_t last_conversion;
volatile uint32_t lc_buff;
// ADC functions
void OpAmpADCInit(uint32_t chan, uint32_t samp_rate, uint32_t in, uint32_t out)
{
	// Configure GPIO according to ins/outs
	LPC_GPIO_PORT->DIR[0] |= (1 << out) | (1 << LED_DEBUG);
	LPC_GPIO_PORT->DIR[0] |= (0 << in);

	// Pull conversion-start pin low (This is inverted)
	LPC_GPIO_PORT->SET[0] = (1 << out) | (1 << LED_DEBUG);


	/* CONFIGURE I/O */
	// Configure interrupt channel 0 for the GPIO pin in SysCon block
	LPC_SYSCTL->PINTSEL[0] = (uint32_t) in;

	// Configure interrupt as edge sensitive and falling edge interrupt
	Chip_PININT_SetPinModeEdge(LPC_PININT, PININTCH0);
	Chip_PININT_EnableIntHigh(LPC_PININT, PININTCH0);
	Chip_SYSCTL_SetPinInterrupt(0, ADC_IN);

	/* Enable interrupt in the NVIC */
	NVIC_EnableIRQ(PININT0_IRQn);

	/* ENABLE MAIN SAMPLING LOOP */
	// Get pointer to desired timer channel
	LPC_MRT_CH_T *pMRT = LPC_MRT_CH(chan);

	// Setup timer with sample rate.  Load immediate.
	Chip_MRT_SetInterval(pMRT, (Chip_Clock_GetSystemClockRate() / samp_rate) | MRT_INTVAL_LOAD);

	// Set to regularly sample
	Chip_MRT_SetMode(pMRT, MRT_MODE_REPEAT);

    // initialize the global ADC variables
    last_conversion = 0;

	// Clear any pending interrupts
	Chip_MRT_IntClear(pMRT);

	/* Enable the interrupt for the MRT */
	NVIC_EnableIRQ(MRT_IRQn);

	//enable timer to start sampling
	Chip_MRT_SetEnabled(pMRT);
}


void MRT_IRQHandler(void)
{
	/* reset the conversion trigger */
	// Clear the convert pin (Reminder: inverted)
	LPC_GPIO_PORT->SET[0] = 1 << ADC_OUT;
	// Clear the pin-interrupt flags, if any
	Chip_PININT_ClearIntStatus(LPC_PININT, PININTCH0);

	// Take the buffer, place it in the conversion position, and clear it (if time reaches 0, the answer is 0.)
	last_conversion = lc_buff;
	lc_buff = 0;

	// trigger the next conversion

	// Clear the interrupt
	uint32_t pending = Chip_MRT_GetIntPending();
	Chip_MRT_ClearIntPending(pending);
	LPC_GPIO_PORT->CLR[0] = 1 << ADC_OUT;
}


void PININT0_IRQHandler(void)
{
	/* Get the time of the MRT*/
	// Get pointer to desired timer channel
	LPC_MRT_CH_T *pMRT = LPC_MRT_CH(ADC_MRT_CHAN);

	// write the channel's time into the buffer
	lc_buff = pMRT->TIMER;

	// Pull the output low to wait for the release at the next sample (inverted)
	LPC_GPIO_PORT->SET[0] = 1 << ADC_OUT;
	LPC_GPIO_PORT->CLR[0] = 1 << LED_DEBUG;

	// Reset the interrupt
	Chip_PININT_ClearIntStatus(LPC_PININT, PININTCH0);
}
#endif
