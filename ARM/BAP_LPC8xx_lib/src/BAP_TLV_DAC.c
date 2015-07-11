/*
 * BAP_TLV_DAC.c
 *
 *  Created on: Nov 1, 2014
 *      Author: Matt
 */

#include "chip.h"
#include "ring_buffer.h"
#include "BAP_TLV_DAC.h"

#define TLV_BUFFER_SIZE 10

/* Tx buffer */
volatile uint16_t TLV_TxBufData[TLV_BUFFER_SIZE];
RINGBUFF_T TLV_TxBuffer;


void TLV_noblockInit()
{
	// Prep the Transmit frame buffer
	RingBuffer_Init(&TLV_TxBuffer, &TLV_TxBufData, sizeof(uint16_t), TLV_BUFFER_SIZE);
	Chip_SYSCTL_PeriphReset(RESET_SPI0);

	// Disable all interrupts
	Chip_SPI_Int_Cmd(LPC_SPI0, SPI_INTENCLR_TXDYEN | SPI_INTENCLR_RXDYEN | SPI_INTENCLR_RXOVEN | SPI_INTENCLR_TXUREN, DISABLE);

	// Enable the SPI
 	NVIC_EnableIRQ(SPI0_IRQn);
}

void TLV_AttatchToSPI(LPC_SPI_T* pSPI)
{
	// Configure the SPI
	Chip_SPI_ConfigureSPI(pSPI, SPI_CFG_CPOL_HI | // Resting state is high
			                   SPI_CFG_CPHA_FIRST | // Data is clocked in when SCL returns to a resting state
			                   SPI_CFG_MSB_FIRST_EN | // Data is transferred Most-significant-bit first
			                   SPI_CFG_MASTER_EN | // Enabled as a master
			                   SPI_CFG_SPI_EN); // Enable the spi
}

static uint16_t TLV_SetupFrame(TLV_DACNumber DAC, TLV_Speed speed, uint16_t value)
{
	uint16_t frame = 0;

	// Code the frame with the right dac and speed
	switch(DAC)
	{
	case(TLV_DAC_1):
			frame = TLV_R1(1) | TLV_SPD(speed) | TLV_R2(0) | TLV_Datamask(value);
			break;
	case(TLV_DAC_2):
			frame = TLV_R1(0) | TLV_SPD(speed) | TLV_R2(0) | TLV_Datamask(value);
			break;
	default:
		break;
	}
	return frame;
}


/***
 * attempts to set the dac value.  returns TLV_OK or ERR_TLV_BUSY errors depending on SPI state
 */
TLV_Error_t TLV_SetDACValue(LPC_SPI_T* pSPI, TLV_DACNumber DAC, TLV_Speed speed, uint16_t value)
{
	if ((Chip_SPI_GetStatus(pSPI) & SPI_STAT_TXRDY))
	{
    	/* Clear status */
//		Chip_SPI_ClearStatus(LPC_SPI0, SPI_STAT_CLR_RXOV | SPI_STAT_CLR_TXUR | SPI_STAT_CLR_SSA | SPI_STAT_CLR_SSD);

		uint16_t frame = TLV_SetupFrame(DAC, speed, value);
		Chip_SPI_SendLastFrame(pSPI, frame, 16);
		return TLV_OK;
	}
	return ERR_TLV_BUSY;
}

TLV_Error_t TLV_SetDACValue_noread(LPC_SPI_T* pSPI, TLV_DACNumber DAC, TLV_Speed speed, uint16_t value)
{
	if ((Chip_SPI_GetStatus(pSPI) & SPI_STAT_TXRDY))
	{
		uint16_t frame = TLV_SetupFrame(DAC, speed, value);
		Chip_SPI_SendLastFrame_RxIgnore(pSPI, frame, 16);
		return TLV_OK;
	}
	return ERR_TLV_BUSY;
}
/***
 * Sets the value for the selected DAC.  Returns ERR_BUSY on error, TLV_OK on success
 */
TLV_Error_t TLV_SetDACValue_noblock(TLV_DACNumber DAC, TLV_Speed speed, uint16_t value)
{
	// Format the frame
	uint16_t frame = TLV_SetupFrame(DAC, speed, value);

	// If the SPI is ready and the buffer is empty.
    if ((Chip_SPI_GetStatus(LPC_SPI0) & SPI_STAT_TXRDY) && RingBuffer_IsEmpty(&TLV_TxBuffer))
    {
    	// Send a frame.
    	// Since there is no need to read a response,
    	// and the full message fits with a frame,
    	// we assume this will be the final frame.
    	Chip_SPI_SendLastFrame_RxIgnore(LPC_SPI0, frame, 16);

		// enable the interrupt
		Chip_SPI_Int_Cmd(LPC_SPI0, SPI_INTENSET_TXDYEN, ENABLE);
    }

	// Otherwise insert the new frame onto the buffer
    else if (!RingBuffer_Insert(&TLV_TxBuffer, &frame))
	{
    	return ERR_TLV_BUSY;
	}
    return TLV_OK;
}


void SPI0_IRQHandler(void)
{
	// Disable all interrupts
	Chip_SPI_Int_Cmd(LPC_SPI0, SPI_INTENCLR_TXDYEN, DISABLE);

	// If the buffer isn't empty, load up the next frame
	if (!RingBuffer_IsEmpty(&TLV_TxBuffer))
	{
		// Get the next value
		uint16_t frame = 0;
		RingBuffer_Pop(&TLV_TxBuffer, &frame);

    	// Send a frame.
    	// Since there is no need to read a response,
    	// and the full message fits with a frame,
    	// we assume this will be the final frame.
    	Chip_SPI_SendLastFrame_RxIgnore(LPC_SPI0, frame, 16);

    	// Enable the interrupt
		Chip_SPI_Int_Cmd(LPC_SPI0, SPI_INTENSET_TXDYEN , ENABLE);
	}
}



