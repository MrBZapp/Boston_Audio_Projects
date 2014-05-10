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
#include "_baplib/timercpp.h"
#include "_baplib/TWI.h"

// program variables
//bbd1 tempo
//bbd2 tempo
// 

int main(void)
{
	wdt_disable();
	//Generator_8_Bit DelayLine( OCA , 4000000 );
	
	TWI_Master test2;
//	Timer8 test;
	/* open USI channels to slaves 
	 * set pc interrupts for:
		*relays
		* tap tempo
	*/
    while(1) //Main Loop
    {
		unsigned char Foo[2] = {0x0F, 0xF0};
		//unsigned char* Bar = &Foo;
		test2.sendDataBytes( Foo, 2 );
      //check activation buttons
	  //set relays
	  
    }
	return 0;
}