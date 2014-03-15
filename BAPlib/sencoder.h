/*
 * sencoder.h
 * Software Rotary Encoder Routines
 * Created: 3/2/2014 1:52:58 PM
 *  Author: Matt Zapp
 *	this set of functions is intended to give access to rotary encoders attached
 *	to any IO pin on a micro controller without using dedicated interrupt pins.
 *	the one limitation is that an encoder cannot use more than one port, each encoder
 *	has to live on a single IO register.
 *
 *	An encoder has 4 parts, a port, 2 pins, and a status.  
 *	Initializing an encoder sets the IO pins to inputs and takes a baseline value.
 *
 *	Every read of direction of an encoder reads the value, compares it against the 
 *	previously stored value, and returns an "right" (increment,) "left" (decrement,) or 
 *	"waiting" (0) state. and updates the status for the next time the encoder is read.
 *
 *	WIP: AUTO READ.  DO NOT USE THESE YET
 *	The automatic read function uses an interrupt-based method to count the number of times and what direction each encoder is moving
 *	if using more than one encoder, a timer is used to automatically rotate and log what 
 */ 


#ifndef SENCODER_H_
#define SENCODER_H_

//includes 
#include <avr/io.h>
#include <inttypes.h>
#include "encoderconfig.h"
//defines
#define 	WAITING 0
#define 	LEFT -1
#define 	RIGHT 1

//typedefs
typedef struct sEncoder {
	unsigned char port;	//port the encoder is located within.
	unsigned char pin1;	//location of the encoder's on the hardware pins
	unsigned char pin2;
	unsigned char then;		//previous value
} sEncoder;

//variables

//prototypes

void sEncoderInit( sEncoder *e );

signed char sEncoderReadDirection ( sEncoder *e );

#endif /* SENCODER_H_ */