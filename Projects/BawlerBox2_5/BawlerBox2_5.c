/*
 * BawlerBox2_5.c
 *
 * Created: 4/6/2014 7:56:46 PM
 *  Author: Matt Zapp
 * 
 * The Bawler 2.5 needs 
 * analog control of 2 BBD chips (2 pins)
 * access to 2 encoders to amend the delay times (preferably 16 bit) (4 pins)
 * access to 2 buttons for control of the two effects available (2 pins)
 * access to 2 buttons to control tap tempo (2 pins)
 * access to 2 bi-colour LEDs to feed back current tempo to user (4 pins)
 * control of 2 relays  (4 pins)
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <avr/wdt.h>
#include "global.h"
#include "encoder.h"
#include "timer.h"

int main(void)
{
    while(1)
    {
        //TODO:: Please write your application code 
    }
}