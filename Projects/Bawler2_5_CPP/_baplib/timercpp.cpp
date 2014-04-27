/*
 * timer.cpp
 *
 * Created: 4/7/2014 9:26:22 PM
 *  Author: Matt
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "timercpp.h"


/**********************************
**							**
**	8-Bit Timer Class Methods	**
**							**
**********************************/
//static Timer8 interruptAccess;


Timer8* Timer8::accessTOIE0 = 0;

Timer8::Timer8(){
	TCCR0B = ( (TCCR0B & ~TIMER_PRESCALE_MASK) | TIMER_CLK_DIV1 );
	TCNT0 = 0x00;
	accessTOIE0 = this;
	TIMSK = (1<<TOIE0);
	sei();
}

Timer8::Timer8( uint8_t prescale ){
	// initialize timer 0
	TCCR0B = ( (TCCR0B & ~TIMER_PRESCALE_MASK) | prescale );// set prescaler
	TCNT0 = 0x00; //reset TCNT0
	accessTOIE0	= this; //Set ISR access
	//TIMSK = TOIE0;	// enable TCNT0 overflow interrupt
	// 	timer0ClearOverflowCount();
};

void Timer8::setPrescale( unsigned char prescale ){
	TCCR0B = ( (TCCR0B & ~TIMER_PRESCALE_MASK) | prescale );
	};	
	
unsigned char Timer8::getPrescale(){
	return (TCCR0B & TIMER_PRESCALE_MASK);
	};

 void Timer8::updateTime(){
	m_Time.count = TCNT0;	 
 }
 
time Timer8::getTime(){
	ATOMIC_BLOCK(ATOMIC_FORCEON){ // memory wall to force read the time before updating the time
	this->updateTime();
	}
	time tempTime = m_Time;
	m_Time.overs = 0;
	return tempTime;
}

time Timer8::getTime_NoUpdates(){
	return m_Time;
}

time Timer8::getTime_NoClear(){
	this->updateTime();
	return m_Time;
}
void TIMER0_OVF_vect(){
	Timer8::accessTOIE0->m_Time.overs++;
	TIMSK = (1<< TOIE0);
	sei();
}

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
	TCNT1 = 0x0000;
	//TIMSK = TOIE0;	// enable TCNT0 overflow interrupt
	// 	timer0ClearOverflowCount();
};

void Timer16::SetPrescale( unsigned char prescale ){
	TCCR1B = ( (TCCR1B & ~TIMER_PRESCALE_MASK) | prescale );
};

unsigned char Timer16::GetPrescale(){
	return (TCCR0B & ~TIMER_PRESCALE_MASK);
};
