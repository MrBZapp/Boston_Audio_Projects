/*
 * BAP_SPI_Schedule.c
 *
 *  Created on: Jul 7, 2015
 *      Author: Matt
 */

#include "BAP_SPI_Schedule.h"

/***
 * The idea of this scheduler is to organize and handle requests to the SPI peripheral.
 * A request to use the peripheral should include:
 * 1.) the SPI format in completion.
 * 2.) a pin on which the SPI Slave Select signal should be asserted/de-asserted.
 * 3.) a transmission form with rx,tx, and callback functions.
 */



void SPI_IRQHandler()
{
	// Disable all interrupts
	Chip_SPI_Int_Cmd(LPC_SPI0, SPI_INTENCLR_TXDYEN, DISABLE);

	// If the buffer isn't empty, load up the next frame
//	if (!RingBuffer_IsEmpty(&ADC121S0_RxBuffer))
//	{
//		// Get the next value
//		uint16_t frame = 0;
//		RingBuffer_Pop(&ADC121S0_RxBuffer, &frame);
//
//    	// Send a frame.
//    	// Since there is no need to read a response,
//    	// and the full message fits with a frame,
//    	// we assume this will be the final frame.
//    	Chip_SPI_SendLastFrame_RxIgnore(LPC_SPI0, frame, 16);
//
//    	// Enable the interrupt
//		Chip_SPI_Int_Cmd(LPC_SPI0, SPI_INTENSET_TXDYEN , ENABLE);
//	}
}
