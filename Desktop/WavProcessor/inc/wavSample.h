/*
 * wavSample.h
 *
 *  Created on: Nov 18, 2014
 *      Author: mzapp
 *
 *  Sample type declarations and processes
 */

#include <math.h>
#include <limits.h>

#ifndef WAVSAMPLE_H_
#define WAVSAMPLE_H_

#define BITDEPTH_OUT 16
#define SAMP_MIN SHRT_MIN
#define SAMP_MAX SHRT_MAX

// 16-bit sample type
typedef struct wavSample_float{
	float left;
	float right;
}wavSample_float_t;

/**
 * changes the gain of a sample
 * returns the sample modified if needed for logic
 * prevents 16-bit over/underflow
 ***/
wavSample_float_t sampleMult(wavSample_float_t* sample, float factor);

/**
 * adds the values of two samples
 * prevents 16-bit over/underflow
 ***/
wavSample_float_t sampleSum(wavSample_float_t* sampA, wavSample_float_t* sampB);


/**
 * prevents 16-bit over/underflow
 ***/
wavSample_float_t sampleClip16Bit(wavSample_float_t* samp);

#endif /* WAVSAMPLE_H_ */
