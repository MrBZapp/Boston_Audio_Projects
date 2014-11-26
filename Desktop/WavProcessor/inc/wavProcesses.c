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
#include "lfo.h"
#include "convolution.h"

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
 * stretches or shrinks a file
 * returns 1 on success, 0 upon failure (requires malloc)
 ***/
int filePitch(wavFilePCM_t* file, float shift)
{
	// calculate the new size of the file
	int oldSize = wavGetSampCount(file);
	int newSize = ceil(oldSize * (1 / shift));

	// Build the buffers used for pitch shifting
	delayLine_t left;
	delayLine_t right;
	delayInit(&left, oldSize);
	delayInit(&right,oldSize);

	// Write the file out into the buffer
	for (int i = 0; i < oldSize; i++)
	{
		delayProgWrite(&left, file->data[i].left);
		delayProgWrite(&right, file->data[i].right);
	}

	// Try to realloc to the new file size
	wavSample_float_t* tmp = realloc(file->data, sizeof(wavSample_float_t) * newSize);
	if (tmp == NULL)
	{
		free(tmp);
		free(left.buffer);
		free(right.buffer);
		return 0;
	}

	// assign the new pointer
	file->data = tmp;

	// update the file size
	wavSetSampleCount(file, newSize);

	// reset the write head. In order to properly interpolate certain values, the buffer must be overwritten
	left.WriteHead = 0;
	right.WriteHead = 0;


	// Read the file back, overwritting previous samples previously written
	for (int i = 0; i < wavGetSampCount(file); i++)
	{
		left.WriteHead = floor(left.ReadHead);
		/*left.buffer[left.WriteHead] =*/ file->data[i].left = delayProgRead(&left, shift);
		right.WriteHead = floor(right.ReadHead);
		/*right.buffer[right.WriteHead] =*/ file->data[i].right = delayProgRead(&right, shift);

	}

	free(left.buffer);
	free(right.buffer);
	return 1;
}


/**
 * shifts a file in time by an arbitrary amount
 ***/
int fileDelay(wavFilePCM_t* file, float delay)
{
	// calculate the old size of the file
	int oldSize = wavGetSampCount(file);

	// calculate the added length of the file
	int newSize = oldSize + ceil(delay);

	// realloc to the new size
	wavSample_float_t* tmp = realloc(file->data, sizeof(wavSample_float_t) * newSize);
	if (tmp == NULL)
	{
		return 0;
	}

	// reset the data to the new size.
	file->data = tmp;
	wavSetSampleCount(file, newSize);

	// create a delay buffer
	delayLine_t left;
	delayLine_t right;

	/*
	 * if you're doing any kind of fractional delay you'll
	 * need to be reaching back in time to get samples.
	 * as a result, you'll need at least and interpolation
	 * window's quantity of samples before the read head and
	 * write heads to account for the first few 0 samples.
	 */
	delayInit(&left, ceil(delay));
	delayInit(&right, ceil(delay));

	// find the fractional part of the delay and apply it to the read head
	// FF the read head to give all 0's in the interpolation window to start.
	float fracDel = delay - floor(delay);
	delaySetReadHead(&left, fracDel);
	delaySetReadHead(&right, fracDel);


	// read/write out the file
	for (int i = 0; i < oldSize; i++)
	{
		// read a sample out of the delay buffer.
		float tempLeft = delayProgRead(&left, 1);
		float tempRight = delayProgRead(&right, 1);

		// write a sample from the file to the delay buffer.
		delayProgWrite(&left, file->data[i].left);
		delayProgWrite(&right, file->data[i].right);

		// write the temp sample back to the file
		file->data[i].left = tempLeft;
		file->data[i].right = tempRight;
	}

	// flush the buffer
	for (int i = oldSize; i < newSize; i++)
	{
		file->data[i].left = delayProgRead(&left, 1);
		file->data[i].right = delayProgRead(&right, 1);
		delayProgWrite(&left, 0);
		delayProgWrite(&right, 0);
	}

	return 1;
}


/**
 * uses a ring buffer delay line to repeat an input signal, gradually fading it out until all samples are 0
 ***/
int fileEcho(wavFilePCM_t* file, long sampDelay, float feedback)
{
	// create a static delay line
	delayLine_t delayBufferL;
	delayLine_t delayBufferR;

	delayInit(&delayBufferL, sampDelay);
	delayInit(&delayBufferR, sampDelay);

	// get the size of the file
	int newSize = wavGetSampCount(file);

	// for every sample in the file, read/write into the buffer
	for (int i = 0; i < newSize; i++)
	{
		// select the sample to delay
		wavSample_float_t tempSamp = file->data[i];

		// read a sample out of the buffer one sample at a time
		float tempLeft = delayProgRead(&delayBufferL, 1);
		float tempRight = delayProgRead(&delayBufferR, 1);

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
			float tempLeft = delayProgRead(&delayBufferL, 1);
			float tempRight = delayProgRead(&delayBufferR, 1);

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


/**
 * Applies a variation in volume over time given an LFO shape, frequency, and depth
 ***/
void fileTremolo(wavFilePCM_t* file, lfoShape_t shape, int freq, float depth)
{
	for(int i = 0; i < wavGetSampCount(file); i++)
	{
		// Get the 0 to 1 LFO value
		float lfoValue = (lfoGetValue(shape, freq, file->FormatChunk.SampleRate, i) / LFO_DEPTH) + (LFO_OFFSET / LFO_DEPTH);
		float offset = 1 - depth;
		float multValue = (lfoValue * depth) / 2 + offset;

		sampleMult(&file->data[i], multValue);
	}
}
