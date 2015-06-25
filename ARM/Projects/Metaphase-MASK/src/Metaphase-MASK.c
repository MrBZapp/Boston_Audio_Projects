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
#include "Mask.h"
#include "BAP_32Sel.h"
#include "BAP_TLV_DAC.h"
#include "BAP_Clk.h"
#include "BAP_MAX1164x_ADC.h"

#define LED_LOCATION    (14)
#define LPC812

int main(void) {
	// Standard boot procedure
	CoreClockInit_30Hz();

	// Global Peripheral Enables:
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_GPIO); // TODO: we don't need this for anything but debugging.
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_SPI0); // SPI0 used for DAC control
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_ACOMP);// ACOMP used to select between envelope variables
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_I2C); // I2C used for ADC control

	// Configure Pinout
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_SWM);
#ifdef LPC812
	/* Pin Assign 8 bit Configuration */
	/* SPI0_SCK */
	LPC_SWM->PINASSIGN[3] = 0x06ffffffUL;
	/* SPI0_MOSI */
	/* SPI0_SSEL */
	LPC_SWM->PINASSIGN[4] = 0xff11ff07UL;
	/* I2C0_SDA */
	LPC_SWM->PINASSIGN[7] = 0x0bffffffUL;
	/* I2C0_SCL */
	LPC_SWM->PINASSIGN[8] = 0xffffff0aUL;

	/* Pin Assign 1 bit Configuration */
	/* ACMP_I2 */
	/* SWCLK */
	/* SWDIO */
	/* RESET */
	LPC_SWM->PINENABLE0 = 0xffffffb1UL;
#endif
#ifdef LPC810
    /* Pin Assign 8 bit Configuration */
    /* SPI0_SCK */
    LPC_SWM->PINASSIGN[3] = 0x04ffffffUL;
    /* SPI0_MOSI */
    /* SPI0_SSEL */
    LPC_SWM->PINASSIGN[4] = 0xff05ff03UL;
    /* I2C0_SDA */
    LPC_SWM->PINASSIGN[7] = 0x02ffffffUL;
    /* I2C0_SCL */
    LPC_SWM->PINASSIGN[8] = 0xffffff01UL;

    /* Pin Assign 1 bit Configuration */
    /* ACMP_I1 */
    LPC_SWM->PINENABLE0 = 0xffffffffUL;
#endif
	Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_SWM);

	// Configure debug LED
	// TODO: get rid of this
	LPC_GPIO_PORT->DIR[0] |= (1 << LED_LOCATION);
	LPC_GPIO_PORT->SET[0] = 1 << LED_LOCATION;

	// Initialize the ADC
	TLV_Init();

	//LPC_GPIO_PORT->CLR[0] = 1 << LED_LOCATION;
	// Initialize the Analog-to-Digital converter
	MAX1164x_t ADC;
	MAX1164x_Init(&ADC, IOCON_PIO11, IOCON_PIO10);
	MAX1164x_SetSpeed(&ADC, MAX1164x_SPD_HI);
	LPC_GPIO_PORT->SET[0] = 1 << LED_LOCATION;

	// Initialize the 32-position selector
	Selector_Init(LPC_CMP);
	uint16_t last_count = 0;
	while(1)
	{
		MAX1164x_RequestNewSample(&ADC);
		if (sample_count != last_count)
		{
			TLV_SetDACValue(TLV_DAC_1, TLV_SPD_Fast, ADC.sample_buffer.channel[0]);
			TLV_SetDACValue(TLV_DAC_2, TLV_SPD_Fast, ADC.sample_buffer.channel[1]);
		}
	}
    return 0 ;
}
