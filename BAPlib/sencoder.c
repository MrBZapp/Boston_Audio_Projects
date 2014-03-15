/*
 * sencoder.c
 * Software Rotary Encoder Routines
 * Created: 3/2/2014 2:03:54 PM
 *  Author: Matt Zapp
 *
 */ 

#include "sencoder.h"

 void sEncoderInit( sEncoder *e ){
	 //the -0x20 in each function here is to align the registers properly.  careful when using this with other projects.
	 _SFR_IO8(e->port - 0x21) = 0<<e->pin1 | 0<<e->pin2; // Set the data direction register
	 _SFR_IO8(e->port - 0x20) = 0<<e->pin1 | 0<<e->pin2; // Set the port
	sEncoderReadDirection(e); //set "then" for the encoder
 }

 signed char sEncoderReadDirection ( sEncoder *e ){
	 unsigned char returnDirection = WAITING;
	 volatile unsigned char tmpRead = _SFR_IO8(e->port - 0x22); //select PIN IO register
	 tmpRead = (tmpRead & (1<<e->pin1|1<<e->pin2)); //mask the port 

	 if ( (tmpRead != e->then) && (tmpRead != ( (1<<e->pin1 | 1<< e->pin2)^e->then) ) ){ // throw out equalities
		 if ( (tmpRead & (1<<e->pin1)) != (e->then & (1<<e->pin1)) ){ // did pin 1 change?
			 if ( (e->then & (1<<e->pin2)) == 0){ // if so, take the position of pin 2
				 if ( (e->then & (1<<e->pin1)) < (tmpRead & (1<<e->pin1)) ){ // Case: pin 2 is low, pin 1 goes high: rotate right
					 returnDirection = RIGHT;
					 } else { // case: pin 2 is low, pin 1 goes low: rotate left.
					 returnDirection = LEFT;
				 }
				 } else { // if pin 2 is not low, it must be high
				 if ( (e->then & (1<<e->pin1)) > (tmpRead & (1<<e->pin1)) ){ // Case: pin 2 is high, pin 1 goes low: rotate right
					 returnDirection = RIGHT;
					 } else { // case: pin 2 is high, pin 1 goes high: rotate left.
					 returnDirection = LEFT;
				 }
			 }
			 
			 } else { // pin 1 didn't change, pin 2 must have
			 
			 if ( (e->then & (1<<e->pin1)) == 0 ){
				 if ( (e->then & (1<<e->pin2)) < (tmpRead & (1<<e->pin2)) ){ //case: pin 1 is low, pin 2 goes high: rotate left
					 returnDirection = LEFT;
					 } else {
					 returnDirection = RIGHT;
				 }
				 } else { // if pin 1 is not low, it must be high
				 if ( (e->then & (1<<e->pin2)) > (tmpRead & (1<<e->pin2)) ){ //case: pin 1 is high, pin 2 goes low: rotate left
					 returnDirection = LEFT;
					 } else {
					 returnDirection = RIGHT;
				 }
			 }
		 }
	 }
	 e->then = tmpRead; //update reading for next status update
	 return returnDirection; //return that nothing has been updated.
 }