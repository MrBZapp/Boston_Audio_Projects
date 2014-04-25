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
	Generator_8_Bit bbd1( OCA ,34 );
	wdt_disable();
    while(1) //Main Loop
    {
       // bbd1._GetFrequency();//TODO:: Please write your application code 
    }
}