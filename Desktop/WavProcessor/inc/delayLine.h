/*
 * delayLine.h
 *
 *  Created on: Nov 22, 2014
 *      Author: mzapp
 *  structures and methods for creating and manipulating a delay line
 *  heavily informed by Stanford CCRMA's design found at:
 *  https://ccrma.stanford.edu/~jos/doppler/Variable_Delay_Line_Software.html
 *
 *  Delay lines are sample-agnostic. You must have a delay line for each channel
 *  when working with multi-channel audio.
 */

#ifndef DELAYLINE_H_
#define DELAYLINE_H_

typedef struct delayLine
{
	float* buffer;
	long size;
	float ReadHead;
	int WriteHead;
}delayLine_t;


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
float delayInterRead(delayLine_t* delay)
{
	// find the closest previous sample
	int intRHead0 = floor(delay->ReadHead);

	// find the next nearest sample in the buffer, wrap this around if at the end of a buffer
	int intRHead1 = intRHead0 + 1;
	intRHead1 %= delay->size;

	// find the distance between the fractional and actual samples
	double a = delay->ReadHead - intRHead0;


	// linearly interpolate between the nearest int sample and the next-nearest int sample
	float temp = (a * delay->buffer[intRHead0]) + ((1-a) * delay->buffer[intRHead1]);

	//return the linearly interpolated value
	return temp;
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
	float temp = delayInterRead(delay);
	delay->ReadHead += progress;
	if (floor(delay->ReadHead) >= delay->size)
	{
		delay->ReadHead -= delay->size;
	}
	return temp;
}

#endif /* DELAYLINE_H_ */
