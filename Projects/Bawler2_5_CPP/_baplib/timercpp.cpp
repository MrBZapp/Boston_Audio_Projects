/*
 * timer.cpp
 *
 * Created: 4/7/2014 9:26:22 PM
 *  Author: Matt
 */ 

#include <avr/io.h>
#include "timercpp.h"

/**********************************
**							**
**	8-Bit Timer Class Methods	**
**							**
**********************************/

Timer8::Timer8(){
	TCCR0B = ( (TCCR0B & ~TIMER_PRESCALE_MASK) | TIMER_CLK_DIV1 );
	TCNT0 = 0x00;
}

Timer8::Timer8( uint8_t prescale ){
	// initialize timer 0
	TCCR0B = ( (TCCR0B & ~TIMER_PRESCALE_MASK) | prescale );// set prescaler
	TCNT0 = 0x00;	// reset TCNT0
	//TIMSK = TOIE0;	// enable TCNT0 overflow interrupt
	// 	timer0ClearOverflowCount();
};

void Timer8::SetPrescale( unsigned char prescale ){
	TCCR0B = ( (TCCR0B & ~TIMER_PRESCALE_MASK) | prescale );
	};	
	
unsigned char Timer8::GetPrescale(){
	return (TCCR0B & ~TIMER_PRESCALE_MASK);
	};


/**********************************
**							**
**	16-Bit Timer Class Methods	**
**							**
**********************************/

Timer16::Timer16(){
	TCCR1B = ( (TCCR1B & ~TIMER_PRESCALE_MASK) | TIMER_CLK_DIV1 );
	TCNT1L = 0x00;
	TCNT1H = 0x00;
}
Timer16::Timer16( uint8_t prescale ){
	// initialize timer 0
	TCCR1B = ( (TCCR1B & ~TIMER_PRESCALE_MASK) | prescale );// set prescaler
	TCNT1L = 0x00;
	TCNT1H = 0x00;
	//TIMSK = TOIE0;	// enable TCNT0 overflow interrupt
	// 	timer0ClearOverflowCount();
};

void Timer16::SetPrescale( unsigned char prescale ){
	TCCR0B = ( (TCCR0B & ~TIMER_PRESCALE_MASK) | prescale );
};

unsigned char Timer16::GetPrescale(){
	return (TCCR0B & ~TIMER_PRESCALE_MASK);
};
