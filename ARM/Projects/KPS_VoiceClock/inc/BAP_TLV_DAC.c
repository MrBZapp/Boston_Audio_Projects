/*
 * BAP_TLV_DAC.c
 *
 *  Created on: Nov 1, 2014
 *      Author: Matt
 */

#include "chip.h"
#include "ring_buffer.h"
#include "BAP_TLV_DAC.h"

#define BUFFER_SIZE 0

/* Tx buffer */
volatile uint16_t TxBufData[BUFFER_SIZE];
RINGBUFF_T TxBuffer;



void TLV_Init()
{
	// Prep the Transmit frame buffer
	RingBuffer_Init(&TxBuffer, &TxBufData, sizeof(uint16_t), BUFFER_SIZE);

	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_SPI0);
	Chip_SYSCTL_PeriphReset(RESET_SPI0);

	// SPI Configure
	LPC_SPI0->CFG = (((uint32_t) SPI_MODE_MASTER) | ((uint32_t) SPI_DATA_MSB_FIRST) | ((uint32_t) SPI_CLOCK_MODE2) | ((uint32_t) SPI_SSEL_ACTIVE_LO)) & ~((uint32_t) SPI_CFG_SPI_EN);

	// Set the rate to 20MHz
	LPC_SPI0->DIV = SPI_DIV_VAL(Chip_SPI_CalClkRateDivider(LPC_SPI0, 20000000));

	// Clear status flags
	Chip_SPI_ClearStatus(LPC_SPI0, SPI_STAT_CLR_RXOV | SPI_STAT_CLR_TXUR | SPI_STAT_CLR_SSA | SPI_STAT_CLR_SSD);

	// Disable all interrupts
	Chip_SPI_Int_Cmd(LPC_SPI0, SPI_INTENCLR_TXDYEN | SPI_INTENCLR_RXDYEN | SPI_INTENCLR_RXOVEN | SPI_INTENCLR_TXUREN, DISABLE);

	// Enable the SPI
	Chip_SPI_Enable(LPC_SPI0);
 	NVIC_EnableIRQ(SPI0_IRQn);
}


void TLV_SetDACValue(TLV_DACNumber DAC, TLV_Speed speed, uint16_t value)
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
		return;
		break;
	}

	// If the SPI is ready and the buffer is empty.
    if ((Chip_SPI_GetStatus(LPC_SPI0) & SPI_STAT_TXRDY) && RingBuffer_IsEmpty(&TxBuffer))
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
    else if (RingBuffer_Insert(&TxBuffer, &frame))
	{
	}
}


void SPI0_IRQHandler(void)
{
	// Disable all interrupts
	Chip_SPI_Int_Cmd(LPC_SPI0, SPI_INTENCLR_TXDYEN, DISABLE);

	// If the buffer isn't empty, load up the next frame
	if (!RingBuffer_IsEmpty(&TxBuffer))
	{
		// Get the next value
		uint16_t frame = 0;
		RingBuffer_Pop(&TxBuffer, &frame);

    	// Send a frame.
    	// Since there is no need to read a response,
    	// and the full message fits with a frame,
    	// we assume this will be the final frame.
    	Chip_SPI_SendLastFrame_RxIgnore(LPC_SPI0, frame, 16);

    	// Enable the interrupt
		Chip_SPI_Int_Cmd(LPC_SPI0, SPI_INTENSET_TXDYEN , ENABLE);
	}
}



