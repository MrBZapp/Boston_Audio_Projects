/********************
 * delayLine.h
 *
 *  Created on: Nov 22, 2014
 *      Author: mzapp
 *  structures and methods for creating and manipulating a delay line
 *  informed by Stanford CCRMA's design found at:
 *  https://ccrma.stanford.edu/~jos/doppler/Variable_Delay_Line_Software.html
 *
 *  Delay lines are sample-agnostic to allow for separate processing of multi-channel
 *  audio.  As a result, even if processing is identical on both samples, two buffers
 *  will need to be used and addressed independently.
 *
 *  Care needs to be taken when using these delay lines to never "cross the streams."
 *  if the read and write heads do cross, there is a very good chance of creating
 *  discontinuity, especially if the buffer is used for an audio stream.
 ********************/

#ifndef DELAYLINE_H_
#define DELAYLINE_H_


#define INTERP_WINDOW 0
#define HALF_WINDOW (INTERP_WINDOW / 2)

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
 * Sets the read head a fixed distance away from the write head.
 * Assumes the write-head is fixed.  allows for varying the read
 * head's distance from the write head without calling a
 * progressive read function.
 *
 * This function will wrap the read head if a goofy value is
 * provided.
 ***/
void delaySetDistance(delayLine_t* delay, float delSamp);


/**
 * returns the distance of the write head to the read head
 ***/
float delayGetDistance(delayLine_t* delay);

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
 * reads whatever is currently under the read head plus an offset
 * offsets outside the range of the delay wrap around
 ***/
float delayModRead(delayLine_t* delay, float offset);

/**
 * Progressively writes data to a buffer
 ***/
void delayProgWrite(delayLine_t* delay, float data);


/**
 * Progressively reads data from a buffer
 ***/
float delayProgRead(delayLine_t* delay, float progress);

#endif /* DELAYLINE_H_ */
