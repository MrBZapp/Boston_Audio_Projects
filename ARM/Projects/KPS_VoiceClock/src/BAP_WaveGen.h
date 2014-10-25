/*
 * BAP_WaveGen.h
 *
 *  Created on: Oct 24, 2014
 *      Author: mzapp
 */
 /**
  * @brief: Functions and structures for generating a square wave with variable pulse-width.
  *
  *
  */
#ifndef BAP_WAVEGEN_H_
#define BAP_WAVEGEN_H_

/*****************************************************************************
 * Type definitions
 ****************************************************************************/
typedef struct WaveGen_S{
	//ID
	uint8_t ID;
	//Freq
	float frequency;
	//Width
	char width;
} WaveGen;

/*****************************************************************************
 * Global Variables																 *
 ****************************************************************************/
extern WaveGen Generator1;

extern WaveGen Generator2;

/*****************************************************************************
 * Functions
 ****************************************************************************/
void WaveGenInit(WaveGen* Generator1, float freq);

void setFreq(WaveGen* Generator, float frequency);
void setWidth(WaveGen* Generator, uint8_t percentage);

uint32_t getRealFreq(WaveGen* Generator);
uint32_t getRealWidth(WaveGen* Generator);

#endif /* BAP_WAVEGEN_H_ */
