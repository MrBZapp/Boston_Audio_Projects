/*
 * BAP_SPIMemory.h
 *
 *  Created on: Apr 6, 2015
 *      Author: Matt
 */

#ifndef BAP_SPIMEMORY_H_
#define BAP_SPIMEMORY_H_

#include "chip.h"
#define SPIMEM_SPI LPC_SPI0

void SPIMemcpy(uint32_t addr, uint8_t* target, uint32_t bytecount);

#endif /* BAP_SPIMEMORY_H_ */
