/*
 * BAP_TLC_DAC.C
 *
 *  Created on: Apr 1, 2015
 *      Author: Matt
 */

#include "chip.h"
#include "BAP_TLC_DAC.h"

void TLC_Init()
{
	// SPI Configure
	LPC_SPI0->CFG = (((uint32_t) SPI_MODE_MASTER) | ((uint32_t) SPI_DATA_MSB_FIRST) | ((uint32_t) SPI_CLOCK_MODE2) | ((uint32_t) SPI_SSEL_ACTIVE_HI)) & ~((uint32_t) SPI_CFG_SPI_EN);

	// Set the rate to 1MHz
	LPC_SPI0->DIV = SPI_DIV_VAL(Chip_SPI_CalClkRateDivider(LPC_SPI0, 1000000));

	// Clear status flags
	Chip_SPI_ClearStatus(LPC_SPI0, SPI_STAT_CLR_RXOV | SPI_STAT_CLR_TXUR | SPI_STAT_CLR_SSA | SPI_STAT_CLR_SSD);

	// Disable all interrupts
	Chip_SPI_Int_Cmd(LPC_SPI0, SPI_INTENCLR_TXDYEN | SPI_INTENCLR_RXDYEN | SPI_INTENCLR_RXOVEN | SPI_INTENCLR_TXUREN, DISABLE);

	// Enable the SPI
	Chip_SPI_Enable(LPC_SPI0);
 	NVIC_EnableIRQ(SPI0_IRQn);
}


void TLC_SetDACValue(TLC_DACNumber_8 DAC, TLC_Range range, uint8_t* value)
{
	// A frame is 11 bits: 2 bit address, 1 bit range, 8 bits data.
	uint16_t frame = (uint16_t)(TLC_DAC(DAC) | TLC_RNG(range) | TLC_Datamask(*value));

	// Wait for the SPI to be ready
	// This is a blocking method
	while (!(Chip_SPI_GetStatus(LPC_SPI0) & SPI_STAT_TXRDY)){/*do nothing*/}

	// Send a frame.
	// Since there is no need to read a response,
	// and the full message fits with a frame,
	// we assume this will be the final frame.
	Chip_SPI_SendLastFrame_RxIgnore(LPC_SPI0, frame, 11);
}
