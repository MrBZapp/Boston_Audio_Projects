/*
 * BAP_SPIMemory.c
 *
 *  Created on: Apr 6, 2015
 *      Author: Matt
 */

#include "BAP_SPIMemory.h"

#define SPIMEM_ADDR(x) (x & 0x00FFFFFF)
#define SPIMEM_READ (0x03 << 24)
#define SPIMEM_FREAD (0x0B << 24)
#define SPI_INT16 16
#define SPI_INT8 8

void SPIMemcpy(uint32_t addr, uint8_t* target, uint32_t bytecount)
{
	uint16_t rx[bytecount];
	uint16_t commandFrame = SPIMEM_FREAD | SPIMEM_ADDR(addr);

	SPI_DATA_SETUP_T data = {
			&commandFrame,
			0,
			&rx,
			0,
			1,
			SPI_INT16
	};
	Chip_SPI_WriteFrames_Blocking(SPIMEM_SPI, &data);

	// Everything after this points is in bytes
	data.DataSize = SPI_INT8;
	data.Length = 1;

	//Write dummy clocks
	Chip_SPI_ReadFrames_Blocking(SPIMEM_SPI, &data);

	//Read all frames back
	data.Length = bytecount;
	Chip_SPI_ReadFrames_Blocking(SPIMEM_SPI, &data);

	for (int i = bytecount; i != 0; i--)
	{
		// write the byte into position
		*(target + i) = (uint8_t) rx[i];
	}
}
