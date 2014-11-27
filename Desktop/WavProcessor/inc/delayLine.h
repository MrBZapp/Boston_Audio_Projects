/*
 * delayLine.h
 *
 *  Created on: Nov 22, 2014
 *      Author: mzapp
 *  structures and methods for creating and manipulating a delay line
 *  heavily informed by Stanford CCRMA's design found at:
 *  https://ccrma.stanford.edu/~jos/doppler/Variable_Delay_Line_Software.html
 *
 *  Delay lines are sample-agnostic to allow for separate processing of multi-channel
 *  audio.  As a result, even if processing is identical on both samples, two buffers
 *  will need to be used and addressed independently.
 */

#ifndef DELAYLINE_H_
#define DELAYLINE_H_


#define INTERP_WINDOW 9
#define HALF_WINDOW truncf(INTERP_WINDOW / 2)

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
 *
 * THIS DOES NOT SEED THE BUFFER, THAT MUST BE DONE SEPARATELY.
 ***/
int delayInit(delayLine_t* delay, long size);



/**
 * Sets the read head, takes into account proper interpolation range
 ***/
void delaySetReadHead(delayLine_t* delay, float i);



/**
 * Sets the write head, takes into account proper interpolation range
 * and wraps properly with the delay line.
 ***/
void delaySetWriteHead(delayLine_t* delay, int i);


/**
 * Sets the read head a fixed distance away from the write head
 ***/
void delaySetDistance(delayLine_t* delay, long delSamp);


/**
 * Linear Interpolating read
 ***/
float delayLinearRead(delayLine_t* delay, int nextSamp);


/**
 * Sinc interpolating read.
 * Reads a value from the read head using Sinc interpolation
 * to find fractional read values.
 ***/
float delaySincRead(delayLine_t* delay);


/**
 * reads whatever is currently under the read head plus an offset
 * adjusts the read head to be within bounds
 ***/
float delayStaticRead(delayLine_t* delay, float offset);


/**
 * Progressively writes data to a buffer
 ***/
void delayProgWrite(delayLine_t* delay, float data);


/**
 * Progressively reads data from a buffer
 ***/
float delayProgRead(delayLine_t* delay, float progress);

#endif /* DELAYLINE_H_ */
