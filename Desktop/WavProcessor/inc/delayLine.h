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
int delayInit(delayLine_t* delay, long size);


/**
 * Sets the read head a fixed distance away from the write head
 ***/
void delaySetDistance(delayLine_t* delay, long delSamp);


/**
 * Interpolating read
 ***/
float delayInterRead(delayLine_t* delay, int nextSamp);

/**
 * Static int read
 ***/
float delayStaticRead(delayLine_t* delay);


/**
 * Progressively writes data to a buffer
 ***/
void delayProgWrite(delayLine_t* delay, float data);


/**
 * Progressively reads data from a buffer
 ***/
float delayProgRead(delayLine_t* delay, float progress);

#endif /* DELAYLINE_H_ */
