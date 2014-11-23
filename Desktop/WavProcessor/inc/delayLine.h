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
	int ReadHead;
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
float delayProgRead(delayLine_t* delay)
{
	float temp = delay->buffer[delay->ReadHead];
	delay->ReadHead++;
	delay->ReadHead %= delay->size;
	return temp;
}

/**
 * Interpolating read
 ***/
float delayInterRead(delayLine_t* delay)
{
	long rpi = (long)floor(delay->ReadHead);
	double a = delay->ReadHead - (double)rpi;
	float temp = a * delay->buffer[rpi] + (1-a) * delay->buffer[rpi+1];
	return temp;
}

float delayStaticRead(delayLine_t* delay)
{
	return delay->buffer[delay->ReadHead];
}
#endif /* DELAYLINE_H_ */
