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
	// block some weirdnesses that could hang the app.
	if (delay->size <= 0)
	{
		i = 0;
	}
	while ( i  < 0)
	{
		i += delay->size;
	}
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
	// find the distance between the fractional and actual samples
	double frac = delay->ReadHead - floor(delay->ReadHead);

	// linearly interpolate between the nearest int sample and the next-nearest int sample
	float interp = ((1 - frac) * delayStaticRead(delay, 0)) + (frac * delayStaticRead(delay, nextSamp));

	//return the linearly interpolated value
	return interp;
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
	double y = 0;

	// convolve a set of samples against a sinc function
	for (int i = 0; i < INTERP_WINDOW; i++)
	{
		// calculate the window offset
		int offset = i - HALF_WINDOW;

		// select the correct position from the sinc function and window
		double x = offset + fracDelay;
		double sincVal = normSinc(x) * window(RECTANGLE, HALF_WINDOW, i);

		// select the appropriate sample
		double sampleVal = delayModRead(delay, offset);

		// sum the function.
		y += sincVal * sampleVal;
	}

	//return the interpolated value
	return precisionf(y, 10);
}


/**
 * reads whatever is currently under the read head plus an offset
 * offsets outside the range of the delay line return 0
 ***/
float delayStaticRead(delayLine_t* delay, float offset)
{
	float tempHead = delay->ReadHead + offset;

	// return 0 if out of range
	if (tempHead < 0 || tempHead > delay->size)
	{
		return 0;
	}

	// find the nearest actual buffer position and return it
	int rhi = trunc(tempHead);
	return delay->buffer[rhi];
}


/**
 * reads whatever is currently under the read head plus an offset
 * offsets outside the range of the delay wrap around
 ***/
float delayModRead(delayLine_t* delay, float offset)
{
	float tempHead = delay->ReadHead + offset;

	// block hangs
	if (delay->size <= 0)
		return 0;

	// if the the readhead is too small, make it bigger.
	while (tempHead < 0)
	{
		tempHead = tempHead + delay->size;
	}

	// if it's larger, wrap to the beginning.
	if (tempHead > delay->size)
	{
		tempHead = fmod(tempHead, delay->size);
	}

	// find the closest preceding actual buffer position and return it
	int rhi = floor(tempHead);
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
	float temp = delayLinearRead(delay, 1);
	delay->ReadHead += progress;

	// wrap the value if greater than the size of the buffer
	delay->ReadHead = fmod(delay->ReadHead, delay->size);

	return temp;
}


#endif /* DELAYLINE_C_ */
