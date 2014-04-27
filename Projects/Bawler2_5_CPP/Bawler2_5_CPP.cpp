/*
 * Bawler2_5_CPP.cpp
 *
 * Created: 4/6/2014 8:57:56 PM
 *  Author: Matt
 */ 


#include <avr/io.h>
#include <stdint.h>
#include <avr/wdt.h>
#include "_cplusplus.h"
#include "_baplib/FreqGen.h"

//#include "timercpp.h"

int main(void)
{
	wdt_disable();
	//Generator_8_Bit DelayLine( OCA , 4000000 );
	Timer8 test;
    while(1) //Main Loop
    {
      // DelayLine.SetFrequency( 4 );// bbd1._GetFrequency();//TODO:: Please write your application code
	  PORTB = test.getTime_NoClear().overs;
    }
	return 0;
}