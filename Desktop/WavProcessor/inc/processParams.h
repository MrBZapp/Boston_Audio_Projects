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
	GAIN,
	REVERSE,
	DISTORT,
	STRETCH,

	// delay and echo operations
	DELAY,
	ECHO,

	// standard modulation operations
	TREMOLO,
	RINGMOD,
	VIBRATO,

	// advanced modulation operations
	CHORUS,
	FLANGE,

	// Filters
	LOWPASS
}proscessType_t;

#endif /* PROCESSPARAMS_H_ */
