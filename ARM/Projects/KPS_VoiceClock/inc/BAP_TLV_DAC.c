/*
 * BAP_TLV_DAC.c
 *
 *  Created on: Nov 1, 2014
 *      Author: Matt
 */

#include "chip.h"
#include "ring_buffer.h"
#include "BAP_TLV_DAC.h"

#define BUFFER_SIZE         5

/* Tx buffer */
static uint16_t TxBufData[BUFFER_SIZE];
RINGBUFF_T TxBuffer;



void TLV_Init()
{
	// Prep the Transmit frame buffer
	RingBuffer_Init(&TxBuffer, &TxBufData, sizeof(uint16_t), BUFFER_SIZE);

	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_SPI0);
	Chip_SYSCTL_PeriphReset(RESET_SPI0);

	// SPI Configure
	LPC_SPI0->CFG = ((uint32_t) SPI_MODE_MASTER) | ((uint32_t) SPI_DATA_MSB_FIRST) | ((uint32_t) SPI_CLOCK_MODE2) | ((uint32_t) SPI_SSEL_ACTIVE_LO);

	// Rate Divider setting
	LPC_SPI0->DIV = SPI_DIV_VAL(Chip_SPI_CalClkRateDivider(LPC_SPI0, 10000));

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

	// Insert the new frame onto the buffer
	if (!RingBuffer_IsFull(&TxBuffer))
	{
		// If the buffer isn't full, add the value
		RingBuffer_Insert(&TxBuffer, &frame);

		LPC_GPIO_PORT->PIN[0] = (1 << 1);

		// and enable the interrupt
		Chip_SPI_Int_Cmd(LPC_SPI0, SPI_INTENSET_TXDYEN, ENABLE);
	}
}


void SPI0_IRQHandler(void)
{

	// Disable all interrupts
	Chip_SPI_Int_Cmd(LPC_SPI0, SPI_INTENCLR_TXDYEN, DISABLE);

	LPC_GPIO_PORT->NOT[0] = (1 << 1);

	// If the buffer isn't empty, load up the next frame
	if (!RingBuffer_IsEmpty(&TxBuffer))
	{

		// Get the next value
		uint16_t frame = 0;
		RingBuffer_Pop(&TxBuffer, &frame);

		// Place the data in the transmit buffer
		if (RingBuffer_GetCount(&TxBuffer) == 1)
		{
			Chip_SPI_SendLastFrame_RxIgnore(LPC_SPI0, frame, 16);
		}
		else
		{
			Chip_SPI_SendMidFrame(LPC_SPI0, frame);
		}

		// Enable the interrupt
		Chip_SPI_Int_Cmd(LPC_SPI0, SPI_INTENSET_TXDYEN , ENABLE);
	}
}



