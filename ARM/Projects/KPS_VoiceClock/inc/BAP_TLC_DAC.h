/*
 * BAP_TLC_DAC.h
 *
 *  Created on: Apr 1, 2015
 *      Author: Matt
 */

#ifndef BAP_TLC_DAC_H_
#define BAP_TLC_DAC_H_

#define DACSIZE 256


typedef enum{
	TLC_DAC_1,
	TLC_DAC_2,
	TLC_DAC_3,
	TLC_DAC_4
}TLC_DACNumber;

typedef enum{
	TLC_Range_Normal = 0,
	TLC_Range_High = 1
}TLC_Range;


void TLC_Init();
void TLC_SetDACValue(TLC_DACNumber DAC, TLC_Range range, uint16_t* value);

#endif /* BAP_TLC_DAC_H_ */
