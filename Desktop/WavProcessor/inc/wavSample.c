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
