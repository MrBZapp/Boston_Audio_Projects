/**
 * wavProcesses.c
 *
 *  Created on: Nov 16, 2014
 *      Author: mzapp
 ***/

#include <string.h>

#include "wavSample.h"
#include "wavFile.h"
#include "wavProcesses.h"
#include "delayLine.h"

/**
 * Reverses a file
 ***/
void fileReverse(wavFilePCM_t* file)
{
	// get the sample count
	long sampleCount = wavGetSampCount(file);

	for (int i = 0; i < sampleCount; i++)
	{
		sampleCount--;
		wavSample_float_t temp = file->data[i];
		file->data[i] = file->data[sampleCount];
		file->data[sampleCount] = temp;
	}
}

/**
 * Changes the gain of an entire file
 ***/
void fileGain(wavFilePCM_t* file, float factor)
{
	long sampleCount = wavGetSampCount(file);

	for (int i = 0; i < sampleCount; i++)
	{
		sampleMult(&file->data[i], factor);
	}
}


/**
 * copies a file and repeats it, gradually fading out the copy until all samples are 0
 ***/
int fileEcho(wavFilePCM_t* file, long sampDelay, float feedback)
{
	// create a static delay line
	delayLine_t delayBufferL;
	delayLine_t delayBufferR;

	delayInit(&delayBufferL, sampDelay * 2);
	delayInit(&delayBufferR, sampDelay * 2);

	delaySetDistance(&delayBufferL, sampDelay);
	delaySetDistance(&delayBufferR, sampDelay);

	// get the size of the file
	int newSize = wavGetSampCount(file);

	// for every sample in the file, read/write into the buffer
	for (int i = 0; i < newSize; i++)
	{
		// select the sample to delay
		wavSample_float_t tempSamp = file->data[i];

		// read a sample out of the buffer
		float tempLeft = delayProgRead(&delayBufferL);
		float tempRight = delayProgRead(&delayBufferR);

		// mix that data back to the file
		file->data[i].left += tempLeft;
		file->data[i].right += tempRight;

		// Calculate how much of that sample will be fed back into the buffer
		tempLeft *= feedback;
		tempRight *= feedback;

		// add the buffer and the file's samples together
		tempLeft += tempSamp.left;
		tempRight += tempSamp.right;

		// write them to the buffer
		delayProgWrite(&delayBufferL, tempLeft);
		delayProgWrite(&delayBufferR, tempRight);
	}

	// Flush the buffer until the echo has decayed completely
	// echoing flag.
	int echoing = 0;

	do
	{
		// Assume we've fully flushed the buffer
		echoing = 0;

		// calculate where to start the buffer
		int buffStart = wavGetSampCount(file);

		// determine the new file size
		newSize = buffStart + delayBufferL.size;
		
		// Reallocate the memory
		wavSample_float_t* tmp = realloc(file->data, (sizeof(wavSample_float_t) * newSize));
		if (tmp == NULL)
		{
			// if realloc fails.
			// free the delay buffer's memory.
			free(delayBufferL.buffer);
			free(delayBufferR.buffer);
			return 0;
		}

		// if realloc succeeds, re-assign the pointer.

		file->data = tmp;
		// update the file size
		wavSetSampleCount(file, newSize);

		// flush the buffer
		for (int i = buffStart; i < newSize; i++)
		{
			// read a sample out of the buffer
			float tempLeft = delayProgRead(&delayBufferL);
			float tempRight = delayProgRead(&delayBufferR);

			// add that data back to the file
			file->data[i].left = tempLeft;
			file->data[i].right = tempRight;

			// Calculate how much of that sample will be fed back into the buffer
			tempLeft *= feedback;
			tempRight *= feedback;

			// If either sample has data in it, keep echoing.
			if (((short) tempLeft) != 0 || ((short) tempRight) != 0)
			{
				echoing = 1;
			}

			// write them to the buffer
			delayProgWrite(&delayBufferL, tempLeft);
			delayProgWrite(&delayBufferR, tempRight);
		}

	}while (echoing == 1);

	// free the delay buffer's memory.
	free(delayBufferL.buffer);
	free(delayBufferR.buffer);
	return 1;
}
