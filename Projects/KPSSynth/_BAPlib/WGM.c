/*
 * WGM.c
 *
 * Created: 8/12/2014 6:49:59 PM
 *  Author: Matt Zapp
 */ 

#include "WGM.h"

void WaveGen1_Init( FREQUENCY_TYPE frequency, unsigned char output ){
	timer1Init();
	timer1SetOverflowPoint( 0xFFFF ); // enable CTC for Timer1 (16-bit timer)
	WaveGen1_SetFrequency( frequency );
	WaveGen1_SetOutput( output );
}


void WaveGen1_SetOutput( unsigned char output ){
	if ( output == OCA ){
		TCCR1A = ( (TCCR1A & ~OUT_COMPARE_MASK) | 0x10 );
		} else {
		TCCR1A = ( (TCCR1A & ~OUT_COMPARE_MASK) | 0x40 );
	}
}


void WaveGen1_SetFrequency( FREQUENCY_TYPE frequency ){ //sets output compare registers to match frequency
	// Calculate the appropriate prescale
	if ( frequency > (F_CPU/0xFF) ){
		timer1SetPrescaler( TIMER_CLK_DIV1 );
	}
	else if ( frequency > ((F_CPU/8)/0xFF) ){
		timer1SetPrescaler( TIMER_CLK_DIV8 );
	}
	else if ( frequency > ((F_CPU/64)/0xFF) ){
		timer1SetPrescaler( TIMER_CLK_DIV64 );
	}
	else if ( frequency > ((F_CPU/256)/0xFF) ){
		timer1SetPrescaler( TIMER_CLK_DIV256 );
	}
	else {
		timer1SetPrescaler( TIMER_CLK_DIV1024 );
	}
	// Calculate the mask and output
	waveGen1Freq = (unsigned char) ( F_CPU / ( (frequency * 2 * timer1GetPrescaler()) ) - 1 );

	WaveGen1_DirectSetOCR( waveGen1Freq );
}

void WaveGen1_DirectSetOCR( unsigned int ocrValue ){
	OCR1A = ocrValue;
	OCR1B = ocrValue;
	TCNT1 = 0x0000;
}