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
void WaveGen_Init(WaveGen* Generator1, float freq);
void WaveGen_Start(WaveGen* Generator);

void WaveGen_SetFreqf(WaveGen* Generator, float frequency);
void WaveGen_SetWidth(WaveGen* Generator, uint8_t percentage);
void WaveGen_SetReload(WaveGen* Generator, int reload);

void WaveGen_UpdateFreq(WaveGen* Generator);
uint32_t getRealFreq(WaveGen* Generator);
uint32_t getRealWidth(WaveGen* Generator);

#endif /* BAP_WAVEGEN_H_ */
