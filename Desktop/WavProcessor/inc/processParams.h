/*
 * processParams.h
 *
 *  Created on: Nov 30, 2014
 *      Author: mzapp
 */

#ifndef PROCESSPARAMS_H_
#define PROCESSPARAMS_H_


// list of all available processes and their associated numbers
typedef enum processType{
	// gain, reverse, and stretch operations
	GAIN = 1,
	REVERSE = 2,
	STRETCH = 3,

	// delay and echo operations
	DELAY = 4,
	ECHO = 5,

	// standard modulation operations
	TREMOLO = 6,
	RINGMOD = 7,
	VIBRATO = 8,

	// advanced modulation operations
	CHORUS = 9,
	FLANGE = 10
}proscessType_t;

#endif /* PROCESSPARAMS_H_ */
