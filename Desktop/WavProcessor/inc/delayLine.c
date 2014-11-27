/*
 * delayLine.c
 *
 *  Created on: Nov 22, 2014
 *      Author: mzapp
 */

#ifndef DELAYLINE_C_
#define DELAYLINE_C_

#include <stdlib.h>
#include <math.h>
#include "delayLine.h"
#include "convolution.h"

/**
 * Initializes a delay struct for use with delay functions
 * size is the sample maximum for the buffer and thus determines
 * the maximum delay length of the buffer.
 *
 * SEEDING OF THE BUFFER MUST BE PERFORMED MANUALLY
 ***/
int delayInit(delayLine_t* delay, long size)
{
	// minimum delay line size is the interpolation window
	size += INTERP_WINDOW;

	// set the size for reference later.
	delay->size = size;

	// Allocate memory in the size of the delay buffer.
	delay->buffer = calloc(size, sizeof(float));
	if (delay->buffer == NULL)
	{
		return 0;
	}

	// initialize the read/write heads
	delaySetReadHead(delay,0);

	// set delay time to be the full length of the delay
	delaySetWriteHead(delay, size - INTERP_WINDOW);

	return 1;
}




/**
 * Sets the read head, takes into account proper interpolation range
 * and wraps properly with the delay line.
 */
void delaySetReadHead(delayLine_t* delay, float i)
{
	delay->ReadHead = HALF_WINDOW + i;
	delay->ReadHead = fmod(delay->ReadHead, delay->size);
}


/**
 * Sets the write head, takes into account proper interpolation range
 * and wraps properly with the delay line.
 ***/
void delaySetWriteHead(delayLine_t* delay, int i)
{
	delay->WriteHead = HALF_WINDOW + i;
	delay->WriteHead %= delay->size;
}


/**
 * Sets the read head a fixed distance away from the write head
 ***/
void delaySetDistance(delayLine_t* delay, long delSamp) {
	// move the read head to the delay, length away from the write head, wrapping around for the size of the buffer
	delay->ReadHead = (delay->WriteHead + delSamp) % delay->size;
}


/**
 * Linear Interpolating read
 ***/
float delayLinearRead(delayLine_t* delay, int nextSamp)
{
	// find the closest previous sample
	int intRHead0 = floor(delay->ReadHead);

	// find the next nearest sample in the buffer, wrap this around if at the end of a buffer
	int intRHead1 = intRHead0 + nextSamp;
	intRHead1 %= delay->size;

	// find the distance between the fractional and actual samples
	double a = delay->ReadHead - intRHead0;

	// linearly interpolate between the nearest int sample and the next-nearest int sample
	float temp = (a * delay->buffer[intRHead0]) + ((1-a) * delay->buffer[intRHead1]);

	//return the linearly interpolated value
	return temp;
}


/**
 *  Sinc interpolating read.
 * Reads a value from the read head using Windowed Sinc Interpolation
 * to find fractional read values.
 ***/
float delaySincRead(delayLine_t* delay)
{
	// find the closest previous sample
	int intRHead = floor(delay->ReadHead);

	// find the distance between the fractional and actual samples
	float fracDelay = delay->ReadHead - intRHead;

	// return value
	float y = 0;

	// convolve a set of samples against a sinc function
	for (int i = 0; i < INTERP_WINDOW; i++)
	{
		// calculate the window offset
		int offset = i - HALF_WINDOW;

		// select the correct position from the sinc function and window
		float x = offset + fracDelay;
		float sincVal = normSinc(x) * window(i);

		// select the appropriate sample
		float sampleVal = delayStaticRead(delay, offset );

		// sum the function.
		y += sincVal * sampleVal;
	}

	//return the interpolated value
	return y;
}





/**
 * reads whatever is currently under the read head plus an offset
 * adjusts the read head to be within bounds
 ***/
float delayStaticRead(delayLine_t* delay, float offset)
{

	// bound the read head to the buffer
	float tempHead = fmod(delay->ReadHead + offset, delay->size);

	// TODO: I am concerned this is a bit of a hack.
	// The delay buffer should never be 0, but if it were, this would hang.
	while (tempHead < 0)
	{
		tempHead += delay->size;
	}

	// find the nearest actual buffer position and return it
	int rhi = trunc(tempHead);
	return delay->buffer[rhi];
}


/**
 * Progressively writes data to a buffer
 ***/
void delayProgWrite(delayLine_t* delay, float data)
{
	delay->buffer[delay->WriteHead] = data;
    delay->WriteHead = (delay->WriteHead + 1) % delay->size;
}


/**
 * Progressively reads data from a buffer
 ***/
float delayProgRead(delayLine_t* delay, float progress)
{
	float temp = delaySincRead(delay);
	delay->ReadHead += progress;

	// wrap the value if greater than the size of the buffer
	delay->ReadHead = fmod(delay->ReadHead, delay->size);

	return temp;
}



#endif /* DELAYLINE_C_ */
