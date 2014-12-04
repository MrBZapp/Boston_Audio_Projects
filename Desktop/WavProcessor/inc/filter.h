/*
 * filter.h
 *
 *  Created on: Dec 3, 2014
 *      Author: mzapp
 */

#ifndef FILTER_H_
#define FILTER_H_

#include "delayLine.h"

// To simplify the passing of coefficients to the bi-quadratic filter
typedef struct filter
{
	// coefficients for the filter
	double a0;
	double a1;
	double a2;
	double b0;
	double b1;
	double b2;
} filter_t;

// allows for static access of previous samples from a biquad function
typedef struct filtStream
{
	// Buffers for samples
	float x[3];
	float y[3];
}filtStream_t;



/**
 * Allows for consistent writing into a filter stream.
 * Must be called to add a sample to a filter stream before
 * calling the type of filter in order to do any processing.
 ***/
void filtWriteXStream(filtStream_t* stream, float samp);

float filtLowPass(int sampRate, filtStream_t* stream, double cutoff, float q );

#endif /* FILTER_H_ */
