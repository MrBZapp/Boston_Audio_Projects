/*
 * BAP_ADC121S0.c
 *
 *  Created on: Jul 7, 2015
 *      Author: Matt
 */


#include "chip.h"
#include "ring_buffer.h"
#include "BAP_ADC121S0.h"


/***
 * Initializes the ring buffer and other variables
 */

void ADC121S0_AttatchToSPI(LPC_SPI_T* spi)
{
	// Configure the SPI
	Chip_SPI_ConfigureSPI(spi, SPI_CFG_CPOL_LO | // Resting state is high
			                   SPI_CFG_CPHA_FIRST | // Data is clocked in when SCL returns to a resting state
			                   SPI_CFG_MSB_FIRST_EN | // Data is transferred Most-significant-bit first
			                   SPI_CFG_MASTER_EN | // Enabled as a master
			                   SPI_CFG_SPI_EN); // Enable the spi
}



/***
 * Requests a new sample from the ADC attached to the ssel_pin,
 * and places said data in the location pointed to by the buffer
 */
ADC121S0_Error_t ADC121S0_RequestSample(LPC_SPI_T* pSPI)
{
	// If the SPI is ready and the buffer is empty.
    if ((Chip_SPI_GetStatus(pSPI) & SPI_STAT_TXRDY))
    {
		// Send a dummy frame
		Chip_SPI_SendLastFrame(pSPI, 0xFFFF, 16);
		return ADC121S0_OK;
    }
    return ERR_ADC121S0_BUSY;
}

ADC121S0_Error_t ADC121S0_GetRequestResult(LPC_SPI_T* pSPI, uint16_t* sample)
{
	// wait for the receive to be ready
	if(!(Chip_SPI_GetStatus(pSPI) & SPI_STAT_RXRDY))
		return ERR_ADC121S0_BUSY;

	// place the sample in its rightful place
	*sample = (uint16_t) Chip_SPI_ReadRXData(pSPI);
	return ADC121S0_OK;
}

