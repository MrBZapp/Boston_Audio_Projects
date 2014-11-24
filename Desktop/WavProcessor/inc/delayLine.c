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

/**
 * Initializes a delay struct for use with delay functions
 * size is the sample maximum for the buffer and thus determines
 * the maximum delay length of the buffer.
 ***/
int delayInit(delayLine_t* delay, long size)
{
	// Allocate memory in the size of the delay buffer.
	delay->buffer = calloc(size, sizeof(float));
	if (delay->buffer == NULL)
	{
		return 0;
	}
	delay->WriteHead = 0;
	delay->ReadHead = 0;
	delay->size = size;
	return 1;
}


/**
 * Sets the read head a fixed distance away from the write head
 ***/
void delaySetDistance(delayLine_t* delay, long delSamp) {
	// move the read head to the delay, length away from the write head, wrapping around for the size of the buffer
	delay->ReadHead = (delay->WriteHead + delSamp) % delay->size;
}

/**
 * Interpolating read
 ***/
float delayInterRead(delayLine_t* delay, int nextSamp)
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

float delayStaticRead(delayLine_t* delay)
{
	int rhi = floor(delay->ReadHead);
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
	float temp = delayInterRead(delay, 1);
	delay->ReadHead += progress;
	if (floor(delay->ReadHead) >= delay->size)
	{
		delay->ReadHead -= delay->size;
	}
	return temp;
}



#endif /* DELAYLINE_C_ */
