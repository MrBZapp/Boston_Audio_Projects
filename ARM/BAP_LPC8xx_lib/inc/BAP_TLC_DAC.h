/*
 * BAP_TLC_DAC.h
 *
 *  Created on: Apr 1, 2015
 *      Author: Matt
 */

#ifndef BAP_TLC_DAC_H_
#define BAP_TLC_DAC_H_

#define TLC_Datamask(x) ((uint16_t) x & 0x00FF )
#define TLC_DAC(x) (x << 9)
#define TLC_RNG(x) (x << 8)
#define DACSIZE 256


typedef enum{
	TLC_DAC_1,
	TLC_DAC_2,
	TLC_DAC_3,
	TLC_DAC_4
}TLC_DACNumber_8;

typedef enum{
	TLC_DAC_00,
	TLC_DAC_01
}TLC_DACNumber_16;


typedef enum{
	TLC_Range_Normal = 0,
	TLC_Range_High = 1
}TLC_Range;


void TLC_Init();
void TLC_SetDACValue(TLC_DACNumber_8 DAC, TLC_Range range, uint8_t* value);
void TLC_SetDACValue_int16(TLC_DACNumber_16 DAC, TLC_Range range, uint16_t* value);
#endif /* BAP_TLC_DAC_H_ */
