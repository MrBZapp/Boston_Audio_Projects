/*
 * HardwareTimer.cpp
 *
 * Created: 4/7/2014 9:26:22 PM
 *  Author: Matt
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "HardwareTimer.h"

#define TIMER_CLK_T_FALL		0x06	///< Timer clocked at T falling edge
#define TIMER_CLK_T_RISE		0x07	///< Timer clocked at T rising edge
#define TIMER_PRESCALE_MASK		0x07	///< Timer Prescaler Bit-Mask

/*********************************
**								**
**	8-Bit Timer Class Methods	**
**								**
**********************************/


/////////////Operational Methods /////////////

HardwareTimer_8Bit* HardwareTimer_8Bit::accessTOIE0 = 0;
HardwareTimer_8Bit::HardwareTimer_8Bit()  
	:CPPTimer( TIMER_CLK_DIV1 )
{
	TCNT0 = 0x00;
	accessTOIE0 = this;
	TIMSK = (1<<TOIE0);
}

HardwareTimer_8Bit::HardwareTimer_8Bit( prescaleSetting prescale ) 
	:CPPTimer( prescale )
{
	accessTOIE0	= this; //Set ISR access
	TIMSK = (1<<TOIE0);	// enable TCNT0 overflow interrupt
	sei();
}

time HardwareTimer_8Bit::pause(){
	TCCR0B =  ( (TCCR0B & ~TIMER_PRESCALE_MASK ) | TIMER_CLK_STOP );
	m_Time.count = TCNT0;
	return m_Time;
}

void HardwareTimer_8Bit::stop(){
	TCCR0B = ( (TCCR0B & ~TIMER_PRESCALE_MASK ) | TIMER_CLK_STOP );
	// Clear count and time.
	TCNT0 = 0x00;
	m_Time.count = 0x00;
	m_Time.overs = 0x00;
	// disable interrupt
	TIMSK = (0 << TOIE0 );
}

void HardwareTimer_8Bit::setPrescale( prescaleSetting prescale ){
	m_Prescale = prescale;
	if ( (TCCR0B & ~TIMER_PRESCALE_MASK) != 0 ){//if the timer is already running, update the prescale mask.  otherwise, just store it.
		TCCR0B = ( (TCCR0B & ~TIMER_PRESCALE_MASK) | translatePrescale(prescale) );
	}
	
}
	
prescaleSetting HardwareTimer_8Bit::getPrescale(){
	return m_Prescale;
}

void HardwareTimer_8Bit::start(){
	TCNT0 = m_Time.count;	
	TCCR0B = translatePrescale( m_Prescale );
	sei();
}

//////////// Access Methods /////////////

 inline void HardwareTimer_8Bit::updateTime(){
	m_Time.count = TCNT0;	 
 }
 
time HardwareTimer_8Bit::getTime(){
	ATOMIC_BLOCK(ATOMIC_FORCEON){ // memory wall to force read the time before updating the time
		this->updateTime();
	}
	time tempTime = m_Time;
	m_Time.overs = 0;
	return tempTime;
}

time HardwareTimer_8Bit::getTime_NoUpdates(){
	return m_Time;
}

time HardwareTimer_8Bit::getTime_NoClear(){
	this->updateTime();
	return m_Time;
}

inline unsigned char HardwareTimer_8Bit::translatePrescale( prescaleSetting toTranslate ){
	switch ( toTranslate ){
		case TIMER_CLK_DIV1 :
			return 1;
			break;
		case TIMER_CLK_DIV8 :
			return 2;
			break;
		case TIMER_CLK_DIV64 :
			return 3;
			break;
		case TIMER_CLK_DIV256 :
			return 4;
			break;	
		case TIMER_CLK_DIV1024 :
			return 5;
			break;
		default:
			return 0;
			break;	
	}
}

/////////////Interrupt///////////////

void TIMER0_OVF_vect(){
	HardwareTimer_8Bit::accessTOIE0->m_Time.overs++;
	HardwareTimer_8Bit::accessTOIE0->m_overageFunction();//FireOverflowInterrupt();
	TIMSK = (1<< TOIE0);
}

/**********************************
**								**
**	16-Bit Timer Class Methods	**
**								**
**********************************/

HardwareTimer_16Bit::HardwareTimer_16Bit(){
	TCCR1B = ( (TCCR1B & ~TIMER_PRESCALE_MASK) | TIMER_CLK_DIV1 );
	TCNT1L = 0x00;
	TCNT1H = 0x00;
}
HardwareTimer_16Bit::HardwareTimer_16Bit( uint8_t prescale ){
	// initialize timer 0
	TCCR1B = ( (TCCR1B & ~TIMER_PRESCALE_MASK) | prescale );// set prescaler
	TCNT1 = 0x0000;
	//TIMSK = TOIE0;	// enable TCNT0 overflow interrupt
	// 	timer0ClearOverflowCount();
};

void HardwareTimer_16Bit::setPrescale( unsigned char prescale ){
	TCCR1B = ( (TCCR1B & ~TIMER_PRESCALE_MASK) | prescale );
};

prescaleSetting HardwareTimer_16Bit::getPrescale(){
	return (prescaleSetting)(TCCR0B & ~TIMER_PRESCALE_MASK);
};
