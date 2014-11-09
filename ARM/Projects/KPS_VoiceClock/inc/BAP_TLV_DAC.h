/*
 * BAP_TLV_DAC.h
 *
 *  Created on: Nov 1, 2014
 *      Author: Matt
 */

#ifndef BAP_TLV_DAC_H_
#define BAP_TLV_DAC_H_

#define TLV_Datamask(x)		((uint16_t) x & 0x0FFF )
#define TLV_R1(x) (x << 15)
#define TLV_SPD(x) (x << 14)
#define TLV_PWR(x) (x << 13)
#define TLV_R2(x) (x << 12)
#define DACSIZE 4096

typedef enum{
	TLV_DAC_1,
	TLV_DAC_2
}TLV_DACNumber;

typedef enum{
	TLV_SPD_Normal = 0,
	TLV_SPD_Fast = 1
}TLV_Speed;


void TLV_Init();
void TLV_SetDACValue(TLV_DACNumber DAC, TLV_Speed speed, uint16_t value);


#endif /* BAP_TLV_DAC_H_ */
