/**
 * lfo.h
 *
 *  Created on: Nov 22, 2014
 *      Author: mzapp
 *
 *  This is an interface for generating and calculating
 *  oscillating values along a string of samples. While
 *  it is named LFO (Low-Frequency Oscillator) due to its
 *  general use-case, it is technically capable of
 *  oscillating at any given frequency and is not limited
 *  to sub 20Hz values.  It is not, however, capable of
 *  generating smooth transitions to new frequencies (yet,) so
 *	please only use static values.
 ***/

#ifndef LFO_H_
#define LFO_H_

#include <math.h>

#define LFO_OFFSET 1
#define LFO_DEPTH 1

typedef enum
{
	SINE,
	TRI,
	SQUARE,
	RAMP_UP,
	RAMP_DOWN,
	// the following shapes have not been implemented.
	// Selecting this will result in a ramp-up shape.
	NOISE,
	SAH
}lfoShape_t;


/**
 * Returns the value of the LFO of a specified frequency and shape
 * requires the file sample rate and current global sample position
 * to properly calculate the return value.  Returns -1 to 1.
 *
 * CHANGING PHASE: if you wish to change the phase of the oscillator,
 * simply add a constant to the sample Position.
 ***/
float lfoGetValue(lfoShape_t shape, float lfoFreq, int sampleRate, float samplePosition);



/**
 * this is functionally equivalent to lfoGetValue() but is unipolar, returning 0 - 1.
 * Use this for control signals unless processing requires a bi-polar signal.
 ***/
float lfoGetValueUni(lfoShape_t shape, float lfoFreq, int sampleRate, float samplePosition);

#endif /* LFO_H_ */
