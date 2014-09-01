/*! \file timer.c \brief System Timer function library. */
//*****************************************************************************
//
// File Name	: 'timer.c'
// Title		: System Timer function library
// Author		: Pascal Stang - Copyright (C) 2000-2002
// Created		: 11/22/2000
// Revised		: 07/09/2003
// Version		: 1.1
// Target MCU	: Atmel AVR Series
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>

#include "timer.h"


// Program ROM constants
// the prescale division values stored in order of timer control register index
// STOP, CLK, CLK/8, CLK/64, CLK/256, CLK/1024
const unsigned short __attribute__ ((progmem)) TimerPrescaleFactor[] = {0,1,8,64,256,1024};
// the prescale division values stored in order of timer control register index
// STOP, CLK, CLK/8, CLK/32, CLK/64, CLK/128, CLK/256, CLK/1024
const unsigned short __attribute__ ((progmem)) TimerRTCPrescaleFactor[] = {0,1,8,32,64,128,256,1024};

// Global variables
volatile unsigned long TimerPauseReg;
// time registers


void timers_Init(void){
	// initialize all timers
	timer0_Init();
	timer1Init();
	#ifdef TCNT2	// support timer2 only if it exists
	timer2Init();
	#endif
	// enable interrupts
	sei();
}


#ifdef TCCR0B
void timer0_SetPrescaler( unsigned char prescale ){
	TCCR0B = ((TCCR0B & ~TIMER_PRESCALE_MASK) | prescale); // set prescaler on timer 0
}

void timer0_Init()
{
	// initialize timer 0
	timer0_SetPrescaler( TIMER0PRESCALE );	// set prescaler
	TCNT0 = 0;							// reset TCNT0
	#ifdef TIMER0OVERFLOW_INT
		TIMSK |= TOIE0;						// enable TCNT0 overflow interrupt
		timer0ClearOverflowCount();				// initialize time registers
	#endif 
}
#endif

#ifdef TCCR1B
void timer1SetPrescaler(unsigned char prescale){
	// set prescaler on timer 1
	TCCR1B = ((TCCR1B & ~TIMER_PRESCALE_MASK) | prescale);
}


unsigned int timer1GetPrescaler(void){
	// get the current prescaler setting
	return (pgm_read_word(TimerPrescaleFactor+(TCCR1B & TIMER_PRESCALE_MASK)));
}

void timer1Init(void)
{
	// initialize timer 1
	timer1SetPrescaler( TIMER1PRESCALE );	// set prescaler
	TCNT1H = 0;								// reset TCNT1
	TCNT1L = 0;
}

void timer1SetOverflowPoint( unsigned int overflowPoint ){
	TCCR1B = ( TCCR1B & (1<<WGM12));// enable CTC for Timer1 (16-bit timer)
	OCR1A = overflowPoint;
	TCNT1 = 0x0000;
}

#endif

#ifdef TCNT2	// support timer2 only if it exists
void timer2Init(void)
{
	// initialize timer 2
	timer2SetPrescaler( TIMER2PRESCALE );	// set prescaler
	TCNT2 = 0;							// reset TCNT2
}
#endif



#ifdef TCNT2	// support timer2 only if it exists
void timer2SetPrescaler(unsigned char prescale){
	// set prescaler on timer 2
	TCCR2 = (TCCR2 & ~TIMER_PRESCALE_MASK) | prescale);
}
#endif

#ifdef TCCR0
unsigned int timer0GetPrescaler(void){
	// get the current prescaler setting
	return (pgm_read_word(TimerPrescaleFactor+(TCCR0 & TIMER_PRESCALE_MASK)));
}
#endif


#ifdef TCNT2	// support timer2 only if it exists
unsigned int timer2GetPrescaler(void){
	//TODO: can we assume for all 3-timer AVR processors,
	// that timer2 is the RTC timer?

	// get the current prescaler setting
	return (pgm_read_word(TimerRTCPrescaleFactor+(inb(TCCR2) & TIMER_PRESCALE_MASK)));
}
#endif


void timerPause(unsigned short pause_ms)
{
	// pauses for exactly <pause_ms> number of milliseconds
	unsigned char timerThres;
	unsigned long ticRateHz;
	unsigned long pause;

	// capture current pause timer value
	timerThres = TCNT0;
	// reset pause timer overflow count
	TimerPauseReg = 0;
	// calculate delay for [pause_ms] milliseconds
	// prescaler division = 1<<(pgm_read_byte(TimerPrescaleFactor+inb(TCCR0)))
	ticRateHz = F_CPU/timer0GetPrescaler();
	// precision management
	// prevent overflow and precision underflow
	//	-could add more conditions to improve accuracy
	if( ((ticRateHz < 429497) && (pause_ms <= 10000)) )
		pause = (pause_ms*ticRateHz)/1000;
	else
		pause = pause_ms*(ticRateHz/1000);

	// loop until time expires
	while( ((TimerPauseReg<<8) | TCNT0) < (pause+timerThres) )
	{
		if( TimerPauseReg < (pause>>8));
		{
			// save power by idling the processor
			set_sleep_mode(SLEEP_MODE_IDLE);
			sleep_mode();
		}
	}

	/* old inaccurate code, for reference
	
	// calculate delay for [pause_ms] milliseconds
	unsigned int prescaleDiv = 1<<(pgm_read_byte(TimerPrescaleFactor+inb(TCCR0)));
	unsigned long pause = (pause_ms*(F_CPU/(prescaleDiv*256)))/1000;
	
	TimerPauseReg = 0;
	while(TimerPauseReg < pause);

	*/
}


void timer0SetOverflowPoint( unsigned char overflowPoint ){
	TCCR0A |= (1<<WGM01); //Set timer to reset on match with OCR0A
	OCR0A = overflowPoint; //set max overflow point
	TCNT0 = 0x00; // clear timer count
}


void timer0ClearOverflowPoint(){
	TCCR0A &= ~(1<<WGM01);//clear the flag that sets reset on match with OCR0A. 
	// BUG ALERT:this needs a bit more TLC.
}
