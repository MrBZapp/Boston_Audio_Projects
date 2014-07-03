/*
 * Bawler2_5_CPP.cpp
 *
 * Created: 4/6/2014 8:57:56 PM
 *  Author: Matt
 */ 

#include <avr/io.h>
#include <avr/wdt.h>
#include "_baplib/BAPTypedefs.h"
//#include "../AVRcpplib/delegate.h"
#include "_cplusplus.h"
#include "_baplib/Scheduler.h"
#define  TAPS_REQUIRED 4 //Sequence starts at 1

bool debounce ( SFRaddress port, unsigned char pin );
unsigned int TapTempo();

void testFunc1(){ PORTD = 0x01;}
void testFunc2(){ PORTD = 0x04;}
void testFunc3(){ PORTD = 0x02;}
	
void SortOverages();
HardwareTimer_8Bit CoreTimer( TIMER_CLK_DIV8 );
Scheduler EventSchedule( &CoreTimer );

//const CppDelegate::FastDelegate0<void> TestDelegate( &testFunc1 );
//const CBFunctor0 doofuss( makeFunctor( (CBFunctor0*)0,testFunc2 ) );

int main(void){
	wdt_disable();
	cli();
	CoreTimer.setOverageFunction( &SortOverages );
	ScheduledEvent EventOne = { 0, 0x01, 0, 0, 255, 255, &testFunc1 };
	ScheduledEvent EventTwo = { 0, 0x01, 0, 0, 254, 254, &testFunc2 };
	ScheduledEvent EventThree = { 0, 0xF0, 0, 0, 256, 256, &testFunc3 };
	//CoreTimer.setOverageCallback(makeFunctor((CBFunctor0*)0, testFunc1));
	
	EventSchedule.ScheduleAnEvent(EventOne);
	EventSchedule.ScheduleAnEvent(EventTwo);
	EventSchedule.ScheduleAnEvent(EventThree);
	PORTB ^= 0xFF;
	
	//CoreTimer.FireOverflowInterrupt();  
	//Timer2.start();
	//Timer3.start();
	
    while(1) //Main Loop
    {	

	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
bool debounce( SFRaddress port, unsigned char pin){
	/*TODO: write an actual debounce algorithm*/
	
	return (bool) (*port && (1<<pin));//Returns whether pin is up or down
}

unsigned int TapTempo(){
	static unsigned char tapTimes[ TAPS_REQUIRED ];
	static bool oldButtonState = 1;
	static unsigned char tapCount = 0;
	unsigned char tapAverage = 0;
	bool tempButtonState = debounce( (SFRaddress) &PORTB, PINB0 );//Poll button state
	if( tempButtonState != oldButtonState ){//Wait for the button to change state 
		if( !tempButtonState ){// if it's down, we can detect a tap
			switch ( tapCount ){
				case ( 0 )://First tap. Start the timer.
					//timer.start();
					tapCount++;
					break;
				case (TAPS_REQUIRED)://last tap.  Get the last time, stop the timer, reset tap count, return average.
					//tapTimes[ (tapCount-1) ] = timer.getTime().overs;
					//timer.stop();
					tapCount = 0;
					for( unsigned char i = 0; i < TAPS_REQUIRED; i++ ){//Average the tapTimes array
						tapAverage += tapTimes[i];
					}
					return tapAverage/TAPS_REQUIRED; //((tapTimes[0] + tapTimes[1] + tapTimes[2] + tapTimes[3])/4);
					break;
				default://intermediate tap.  Update the tap time and tap count
					//tapTimes[ (tapCount - 1) ] = timer.getTime().overs;
					tapCount++;
					break;
			}//End Tap routing
		}//End Tap Detect
		oldButtonState = tempButtonState;
	}//End Change-state Detect
	return 0;
}//End Tap Tempo

void SortOverages(){ 
	time newTime = { 0, 1 };
	EventSchedule.ProcessTime( newTime );
 }
