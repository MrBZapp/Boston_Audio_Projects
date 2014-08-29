/*
 * WGM.h
 *
 * Created: 8/11/2014 8:15:05 PM
 *  Author: Matt Zapp
 */ 

#ifndef WGM_H_
#define WGM_H_



#ifdef ENNABLE_HIGH_FREQUENCY_ACCURACY
	#define FREQUENCY_TYPE float
#else
	#define FREQUENCY_TYPE unsigned int
#endif

#define OCA 0x00
#define OCB 0x01
#define OUT_COMPARE_MASK 0xF0

unsigned int waveGen1Freq;

void WaveGen1_Init( /*FREQUENCY_TYPE frequency,*/ unsigned char output );

void WaveGen1_SetOutput( unsigned char output);

void WaveGen1_SetFrequency( FREQUENCY_TYPE frequency );

void WaveGen1_DirectSetOCR( unsigned int ocrValue );

#endif /* WGM_H_ */