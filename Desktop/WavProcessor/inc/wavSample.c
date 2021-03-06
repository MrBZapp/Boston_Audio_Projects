/*
 * wavSample.c
 *
 *  Created on: Nov 18, 2014
 *      Author: mzapp
 */


#include <limits.h>
#include <math.h>
#include "wavSample.h"

/**
 * changes the gain of a sample
 * returns the sample modified if needed for logic
 ***/
wavSample_float_t sampleMult(wavSample_float_t* sample, float factor)
{
	// create a temp sample to return
	wavSample_float_t temp;

	// multiply the sample channels
	temp.left = sample->left = (sample->left * factor);
	temp.right = sample->right = (sample->right * factor);

	return temp;
}


/**
 * adds the values of two samples
 * returns the sample sum
 ***/
wavSample_float_t sampleSum(wavSample_float_t* sampA, wavSample_float_t* sampB )
{
	wavSample_float_t tempSamp;
	tempSamp.left = sampA->left + sampB->left;
	tempSamp.right = sampA->right + sampB->right;

	return tempSamp;
}


/**
 * Changes samples' value based on a distortion function
 * and a knee value.
 ***/
wavSample_float_t sampleDeLinearize(wavSample_float_t samp, distortion_pt func, float knee)
{
	samp.left = func(samp.left, knee);
	samp.right = func(samp.right, knee);
	return samp;
}


/**
 * prevents 16-bit over/underflow
 ***/
wavSample_float_t sampleClip16Bit(wavSample_float_t* sample)
{
	wavSample_float_t tempSamp;

	tempSamp.left = sample->left = fminf(sample->left, SAMP_MAX);
	tempSamp.left = sample->left = fmaxf(sample->left, SAMP_MIN);

	tempSamp.right = sample->right = fminf(sample->right, SAMP_MAX);
	tempSamp.right = sample->right = fmaxf(sample->right, SAMP_MIN);

	return tempSamp;
}



/**
 * converts a sample count to a millisecond value for
 * a given sample rate
 ***/
float sampstoms(float sampleRate, float sampleCount)
{
	float ms = (sampleRate / 1000) * sampleCount;
	return ms;
}


/**
 * converts a millisecond count to a sample count for
 * a given sample rate
 ***/
float mstosamps(float sampleRate, float msCount)
{
	float samps = (sampleRate * msCount) / 1000;
	return samps;
}
