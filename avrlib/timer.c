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
// time registers
volatile unsigned long TimerPauseReg;
#ifdef TIMER0OVERFLOW_INT
	#pragma message "Timer0 Interrupt enabled"
	volatile unsigned long Timer0Reg0;
#endif
#ifdef TIMER2OVERFLOW_INT
volatile unsigned long Timer2Reg0;
#endif

typedef void (*voidFuncPtr)(void);
volatile static voidFuncPtr TimerIntFunc[TIMER_NUM_INTERRUPTS];

// delay for a minimum of <us> microseconds 
// the time resolution is dependent on the time the loop takes 
// e.g. with 4Mhz and 5 cycles per loop, the resolution is 1.25 us 
void delay_us(unsigned short time_us) 
{
	unsigned short delay_loops;
	register unsigned short i;

	delay_loops = (time_us+3)/5*CYCLES_PER_US; // +3 for rounding up (dirty) 

	// one loop takes 5 cpu cycles 
	for (i=0; i < delay_loops; i++) {};
}
/*
void delay_ms(unsigned char time_ms)
{
	unsigned short delay_count = F_CPU / 4000;

	unsigned short cnt;
	asm volatile ("\n"
                  "L_dl1%=:\n\t"
                  "mov %A0, %A2\n\t"
                  "mov %B0, %B2\n"
                  "L_dl2%=:\n\t"
                  "sbiw %A0, 1\n\t"
                  "brne L_dl2%=\n\t"
                  "dec %1\n\t" "brne L_dl1%=\n\t":"=&w" (cnt)
                  :"r"(time_ms), "r"((unsigned short) (delay_count))
	);
}
*/
void timerInit(void)
{
	unsigned char intNum;
	// detach all user functions from interrupts
	for(intNum=0; intNum<TIMER_NUM_INTERRUPTS; intNum++)
		timerDetach(intNum);

	// initialize all timers
	timer0Init();
	timer1Init();
	#ifdef TCNT2	// support timer2 only if it exists
	timer2Init();
	#endif
	// enable interrupts
	sei();
}


#ifdef TCCR0B
void timer0SetPrescaler( unsigned char prescale ){
	TCCR0B = ((TCCR0B & ~TIMER_PRESCALE_MASK) | prescale); // set prescaler on timer 0
}

void timer0Init()
{
	// initialize timer 0
	timer0SetPrescaler( TIMER0PRESCALE );	// set prescaler
	TCNT0 = 0;							// reset TCNT0
	#ifdef TIMER0OVERFLOW_INT
		sbi(TIMSK, TOIE0);						// enable TCNT0 overflow interrupt
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
	#ifdef TIMER1OVERFLOW_INT
		TIMSK = (1<<TOIE1);						// enable TCNT1 overflow
	#endif
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
	#ifdef TIMER2OVERFLOW_INT
		sbi(TIMSK, TOIE2);						// enable TCNT2 overflow
		timer2ClearOverflowCount();				// initialize time registers
	#endif
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


void timerAttach(unsigned char interruptNum, void (*userFunc)(void) ){
	// make sure the interrupt number is within bounds
	if(interruptNum < TIMER_NUM_INTERRUPTS)
	{
		// set the interrupt function to run
		// the supplied user's function
		TimerIntFunc[interruptNum] = userFunc;
	}
}


void timerDetach(unsigned char interruptNum){
	// make sure the interrupt number is within bounds
	if(interruptNum < TIMER_NUM_INTERRUPTS)
	{
		// set the interrupt function to run nothing
		TimerIntFunc[interruptNum] = 0;
	}
}


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


#ifdef TIMER0OVERFLOW_INT
void timer0ClearOverflowCount(void)
{
	// clear the timer overflow counter registers
	Timer0Reg0 = 0;	// initialize time registers
}

long timer0GetOverflowCount(void)
{
	// return the current timer overflow count
	// (this is since the last timer0ClearOverflowCount() command was called)
	return Timer0Reg0;
}
#endif

void timer0ClearOverflowPoint(){
	TCCR0A &= (0<<WGM01);//clear the flag that sets reset on match with OCR0A. 
	// BUG ALERT:this needs a bit more TLC.
}

#ifdef TCNT2	// support timer2 only if it exists
#ifdef TIMER2OVERFLOW_INT
void timer2ClearOverflowCount(void)
{
	// clear the timer overflow counter registers
	Timer2Reg0 = 0;	// initialize time registers
}

long timer2GetOverflowCount(void)
{
	// return the current timer overflow count
	// (this is since the last timer2ClearOverflowCount() command was called)
	return Timer2Reg0;
}
#endif
#endif


#ifdef WGM10
// include support for arbitrary top-count PWM
// on new AVR processors that support it
void timer1PWMInitICR(unsigned int topcount)
{
	// set PWM mode with ICR top-count
	TCCR1A &= ~(1<<WGM10);
	TCCR1A |= (1<<WGM11);
	TCCR1B |= (1<<WGM12);
	TCCR1B |= (1<<WGM13);
	
	// set top count value
	ICR1 = topcount;
	
	// clear output compare value A
	OCR1A = 0;
	// clear output compare value B
	OCR1B = 0;

}
#endif


#ifdef TIMER0OVERFLOW_INT
//! Interrupt handler for tcnt0 overflow interrupt
TIMER_INTERRUPT_HANDLER(TIMER0_OVF_vect)
{
	Timer0Reg0++;			// increment low-order counter

	// increment pause counter
	TimerPauseReg++;

	// if a user function is defined, execute it too
	if(TimerIntFunc[TIMER0OVERFLOW_INT])
		TimerIntFunc[TIMER0OVERFLOW_INT]();
}
#endif


#ifdef TIMER1OVERFLOW_INT
//! Interrupt handler for tcnt1 overflow interrupt
TIMER_INTERRUPT_HANDLER(TIMER1_OVF_vect)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER1OVERFLOW_INT])
		TimerIntFunc[TIMER1OVERFLOW_INT]();
}
#endif

#ifdef TCNT2	// support timer2 only if it exists
#ifdef TIMER2OVERFLOW_INT
//! Interrupt handler for tcnt2 overflow interrupt
TIMER_INTERRUPT_HANDLER(TIMER2_OVF_vect)
{
	Timer2Reg0++;			// increment low-order counter

	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER2OVERFLOW_INT])
		TimerIntFunc[TIMER2OVERFLOW_INT]();
}
#endif
#endif

#ifdef OCR0
// include support for Output Compare 0 for new AVR processors that support it
//! Interrupt handler for OutputCompare0 match (OC0) interrupt
TIMER_INTERRUPT_HANDLER(SIG_OUTPUT_COMPARE0)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER0OUTCOMPARE_INT])
		TimerIntFunc[TIMER0OUTCOMPARE_INT]();
}
#endif

#ifdef TIMER1OUTCOMPAREA_INT
//! Interrupt handler for CutputCompare1A match (OC1A) interrupt
TIMER_INTERRUPT_HANDLER(TIMER1_COMPA_vect)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER1OUTCOMPAREA_INT])
		TimerIntFunc[TIMER1OUTCOMPAREA_INT]();
}
#endif

#ifdef TIMER1OUTCOMPAREB_INT
//! Interrupt handler for OutputCompare1B match (OC1B) interrupt
TIMER_INTERRUPT_HANDLER(TIMER1_COMPB_vect)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER1OUTCOMPAREB_INT])
		TimerIntFunc[TIMER1OUTCOMPAREB_INT]();
}
#endif

//! Interrupt handler for InputCapture1 (IC1) interrupt
TIMER_INTERRUPT_HANDLER(TIMER1_CAPT_vect)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER1INPUTCAPTURE_INT])
		TimerIntFunc[TIMER1INPUTCAPTURE_INT]();
}

#ifdef TCNT2
//! Interrupt handler for OutputCompare2 match (OC2) interrupt
TIMER_INTERRUPT_HANDLER(SIG_OUTPUT_COMPARE2)
{
	// if a user function is defined, execute it
	if(TimerIntFunc[TIMER2OUTCOMPARE_INT])
		TimerIntFunc[TIMER2OUTCOMPARE_INT]();
}
#endif