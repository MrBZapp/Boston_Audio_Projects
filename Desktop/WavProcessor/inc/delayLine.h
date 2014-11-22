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
	int size;
	int ReadHead;
	int WriteHead;
}delayLine_t;

/**
 * Initializes a delay struct for use with delay functions
 * size is the sample maximum for the buffer and thus determines
 * the maximum delay length of the buffer.
 ***/
int initDelay(delayLine_t* delay, int size)
{
	// Allocate memory in the size of the delay buffer.
	delay->buffer = calloc(size, sizeof(float));
	delay->WriteHead = 0;
	delay->ReadHead = 0;
	delay->size = size;
}

/**
 * Sets the read head a fixed distance away from the write head
 ***/
void setDelay(delayLine_t* delay, int delSamp) {
	// move the read head to the delay, length away from the write head, wrapping around for the size of the buffer
	delay->ReadHead = (delay->WriteHead + delSamp) % delay->size;
}

/**
 * Writes the provided sample to the delay line.
 * Reads the sample underneath the read head and returns it.
 */
float delayWriteRead(delayLine_t* delay, float sample)
{
	// write the new sample to the write pointer
	delay->buffer[delay->WriteHead] = sample;
	float temp = delayStaticRead(delay);
	delay->WriteHead = delay->WriteHead++ % delay->size;
	delay->ReadHead = delay->ReadHead++ % delay->size;
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
