/*
 * lfo.c
 *
 *  Created on: Nov 22, 2014
 *      Author: mzapp
 */
#include <math.h>
#include "lfo.h"

/**
 * Returns the value of the LFO of a specified frequency and shape
 * requires the file sample rate and current global sample position
 * to properly calculate the return value.  Returns -1 to 1.
 *
 * CHANGING PHASE: if you wish to change the phase of the oscillator,
 * simply add a constant to the sample Position.
 *
 * TODO: track the phase so that different frequencies and wavs can
 * be stitched together on the fly.
 ***/
float lfoGetValue(lfoShape_t shape, float lfoFreq, int sampleRate, float samplePosition)
{
	// calculate the quantity of samples in an LFO cycle based upon the provided sample rate and frequency.
	float lfoSamps = (1/lfoFreq) * sampleRate;

	// calculate the LFO phase 0 to 2pi
	float floatPhase = (fmod(samplePosition, lfoSamps) / lfoSamps) * 2 * M_PI;

	float retVal = floatPhase;

	// select the right algorithm
	switch(shape)
	{
	case(SINE):
			retVal = sinf(floatPhase) * M_PI;
			break;

	// Calculate the return of a triangle wave given the phase
	case(TRI):
			retVal = fabs(floatPhase - M_PI) * 2 - M_PI;
			break;

	// Is low for the first half of a cyle, high for the second half
	case(SQUARE):
			if (floatPhase < M_PI)
			{
				return -1;
			}
			else
			{
				return 1;
			}
			break;

	// Is, simply, the LFO phase.
	case(RAMP_UP):
			retVal = floatPhase - M_PI;
			break;

	// Is the inverse of the LFO phase.
	case(RAMP_DOWN):
			retVal = M_PI - floatPhase;
			break;

	case(NOISE):
			break;

	case(SAH):
			break;

	default:
		break;
	}
	return retVal / M_PI;
}


/**
 * this is functionally equivalent to lfoGetValue() but is unipolar, returning 0 - 1.
 * Use this for control signals unless processing requires a bi-polar signal.
 ***/
float lfoGetValueUni(lfoShape_t shape, float lfoFreq, int sampleRate, float samplePosition)
{
	float uniPolarValue = (lfoGetValue(shape, lfoFreq, sampleRate, samplePosition) / LFO_DEPTH) + (LFO_OFFSET / LFO_DEPTH);
	return uniPolarValue;
}
