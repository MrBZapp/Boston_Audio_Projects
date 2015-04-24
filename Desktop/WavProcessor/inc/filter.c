/*
 * filter.c
 *
 *  Created on: Dec 3, 2014
 *      Author: mzapp
 *
 *  Most filters are based on Robert Bristo-Johnson's designs
 *  You can find documentation on these filters at:
 *  http://www.musicdsp.org/files/Audio-EQ-Cookbook.txt
 */
#define _USE_MATH_DEFINES
#include <math.h>
#include "filter.h"



/**
 * Allows for consistent writing into a filter stream.
 * Must be called to add a sample to a filter stream before
 * calling the type of filter in order to do any processing.
 ***/
void filtWriteXStream(filtStream_t* stream, float samp)
{
	// advance X
	stream->x[0] = stream->x[1];
	stream->x[1] = stream->x[2];

	// write data to X
	stream->x[2] = samp;
}


/**
 * Called by biquad to update the filter stream
 */
static inline void filtWriteYStream(filtStream_t* stream, float samp)
{
	// advance Y
	stream->y[0] = stream->y[1];
	stream->y[1] = stream->y[2];

	// write data to X
	stream->y[2] = samp;
}


/**
 * This is the core of every filter.
 * really cannot be called directly without some setup,
 * the helper functions for each type of filter should
 * take care of that.
 ***/
static double biquad(filter_t* f, filtStream_t* stream)
{
	double y = (f->b0 / f->a0) * stream->x[2] + (f->b1 / f->a0) * stream->x[1] + (f->b2 / f->a0) * stream->x[0]
	                                          - (f->a1 / f->a0) * stream->y[1] - (f->a2 / f->a0) * stream->y[0];
	filtWriteYStream(stream, y);
	return y;
}


/**
 * Given a sample rate, a buffer of 3 samples, the cutoff frequency, and the quality
 * of the filter, will return a single sample
 ***/
float filtLowPass(float sampRate, filtStream_t* stream, double cutoff, float q )
{
	double w0 = 2 * M_PI * (cutoff / sampRate);

	double sinw0 = sin(w0);
	double cosw0 = cos(w0);

	double alpha = sinw0 / (2 * q);

	filter_t LPF;

	LPF.b0 =  (1 - cosw0)/2;
	LPF.b1 =   1 - cosw0;
	LPF.b2 =  (1 - cosw0)/2;
	LPF.a0 =   1 + alpha;
	LPF.a1 =  -2 * cosw0;
	LPF.a2 =   1 - alpha;

	double y = biquad(&LPF, stream);
	return y;
}


