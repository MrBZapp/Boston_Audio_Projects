/*
 * BAP_TLV_DAC.h
 *
 *  Created on: Nov 1, 2014
 *      Author: Matt
 *      Details: Texas Instruments TLV_5616 DAC driver
 */

#ifndef BAP_TLV_DAC_H_
#define BAP_TLV_DAC_H_

#include "chip.h"

#define TLV_Datamask(x)		((uint16_t) x & 0x0FFF )
#define TLV_R1(x) (x << 15)
#define TLV_SPD(x) (x << 14)
#define TLV_PWR(x) (x << 13)
#define TLV_R2(x) (x << 12)
#define DACSIZE 4096

typedef enum{
	TLV_OK = 1,
	ERR_TLV_BUSY = 0
}TLV_Error_t;

typedef enum{
	TLV_DAC_1,
	TLV_DAC_2
}TLV_DACNumber;

typedef enum{
	TLV_SPD_Normal = 0,
	TLV_SPD_Fast = 1
}TLV_Speed;

// Initialization functions
void TLV_noblockInit();
void TLV_AttatchToSPI(LPC_SPI_T* pSPI);
TLV_Error_t TLV_SetDACValue_noread(LPC_SPI_T* pSPI, TLV_DACNumber DAC, TLV_Speed speed, uint16_t value);

/***
 * attempts to set the dac value.  returns LPC_OK or ERR_SPI_BUSY errors depending on SPI state
 */
TLV_Error_t TLV_SetDACValue(LPC_SPI_T* pSPI, TLV_DACNumber DAC, TLV_Speed speed, uint16_t value);

/***
 * Sets the value for the selected DAC.  Returns ERR_BUSY on error, LPC_OK on success
 */
TLV_Error_t TLV_SetDACValue_noblock(TLV_DACNumber DAC, TLV_Speed speed, uint16_t value);

#endif /* BAP_TLV_DAC_H_ */
