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
#include "distort.h"
#include "filter.h"

/**
 * REVERSE:
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
 * GAIN:
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
 * DISTORTION:
 * applies a Delinearizing function to every sample in a file
 ***/
void fileDist(wavFilePCM_t* file, distType_t type, float knee)
{
	for (int i = 0; i < wavGetSampCount(file); i++)
	{
		// get a sample
		wavSample_float_t temp = file->data[i];

		// process the sample
		switch(type)
		{
		case(SOFT):
				temp = sampleDeLinearize(temp, &distSoft, knee);
				break;
		case(HARD):
				temp = sampleDeLinearize(temp, &distHard, knee);
				break;
		default:
			break;
		}

		// place the sample back in the file
		file->data[i] = temp;
	}
}


/**
 * STRETCH:
 * stretches or shrinks a file
 * returns 1 on success, 0 upon failure (requires malloc)
 ***/
int filePitch(wavFilePCM_t* file, float shift)
{
	// calculate the new size of the file
	int oldSize = wavGetSampCount(file);
	int lenChange = ceil(oldSize * (1 / shift)) - oldSize;

	// Build the buffers used for pitch shifting
	delayLine_t left;
	delayLine_t right;
	delayInit(&left, oldSize);
	delayInit(&right,oldSize);

	// we don't want a delay, so we'll need to reset the write heads
    delaySetWriteHead(&left, 0);
    delaySetWriteHead(&right, 0);

	// Write the file out into the buffer
	for (int i = 0; i < oldSize; i++)
	{
		delayProgWrite(&left, file->data[i].left);
		delayProgWrite(&right, file->data[i].right);
	}

	// update the length of the file
	if (!wavChangeLength(file,lenChange))
	{
		free(left.buffer);
		free(right.buffer);
		return 0;
	}

	// Read the file back at speed
	for (int i = 0; i < wavGetSampCount(file); i++)
	{
		wavSample_float_t temp = {0,0};
		temp.left = delayProgRead(&left, shift);
		temp.right = delayProgRead(&right, shift);
		wavWriteAtPosition(file, i, &temp);
	}

	free(left.buffer);
	free(right.buffer);
	return 1;
}


/**
 * DELAY:
 * shifts a file in time by an arbitrary amount
 ***/
int fileDelay(wavFilePCM_t* file, float delay)
{
	// calculate the old size of the file
	int oldSize = wavGetSampCount(file);

	// update the length of the file with the added delay
	if (!wavChangeLength(file, ceil(delay)))
		return 0;

	// create a delay buffer
	delayLine_t left;
	delayLine_t right;
	delayInit(&left, ceil(delay));
	delayInit(&right, ceil(delay));

	//seed the buffers
	int fileReadHead = 0;
	for (int i = 0; i < HALF_WINDOW; i++, fileReadHead++)
	{
		delayProgWrite(&left, wavReadAtPosition(file, fileReadHead).left);
		delayProgWrite(&right, wavReadAtPosition(file, fileReadHead).right);
	}

	// find the fractional part of the delay and apply it to the read head
	// FF the read head to give all 0's in the interpolation window to start.
	float fracDel = delay - floor(delay);
	delaySetReadHead(&left, fracDel);
	delaySetReadHead(&right, fracDel);

	// read/write out the file
	for (int i = 0; i < oldSize; i++, fileReadHead++)
	{
		// read a sample out of the delay buffer.
		wavSample_float_t temp = {0, 0};

		temp.left = delayProgRead(&left, 1);
		temp.right = delayProgRead(&right, 1);

		// write a sample from the file to the delay buffer.
		delayProgWrite(&left, wavReadAtPosition(file, fileReadHead).left);
		delayProgWrite(&right, wavReadAtPosition(file, fileReadHead).right);

		// write the temp sample back to the file
		wavWriteAtPosition(file, i, &temp);
	}

	// flush the buffer
	for (int i = oldSize; i < wavGetSampCount(file); i++)
	{
		wavSample_float_t temp = {0, 0};
		temp.left = delayProgRead(&left, 1);
		temp.right = delayProgRead(&right, 1);

		wavWriteAtPosition(file, i, &temp);

		delayProgWrite(&left, 0);
		delayProgWrite(&right, 0);
	}

	// free the buffers
	free(left.buffer);
	free(right.buffer);
	return 1;
}


/**
 * ECHO:
 * uses a ring buffer delay line to repeat an input signal, gradually fading it out until all samples are 0
 ***/
int fileEcho(wavFilePCM_t* file, long sampDelay, float feedback)
{
	// protect against hangs:
	if (feedback >= 1)
	{
		feedback = 0.9;
	}

	// create a delay line
	delayLine_t left;
	delayLine_t right;

	delayInit(&left, sampDelay);
	delayInit(&right, sampDelay);

	// get the size of the file
	int newSize = wavGetSampCount(file);

	// for every sample in the file, read/write into the buffer
	for (int i = 0; i < newSize; i++)
	{
		// select the sample to delay
		wavSample_float_t tempSamp = file->data[i];

		// read a sample out of the buffer one sample at a time
		float tempLeft = delayProgRead(&left, 1);
		float tempRight = delayProgRead(&right, 1);

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
		delayProgWrite(&left, tempLeft);
		delayProgWrite(&right, tempRight);
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
		newSize = buffStart + left.size;
		
		// Reallocate the memory
		wavSample_float_t* tmp = realloc(file->data, (sizeof(wavSample_float_t) * newSize));
		if (tmp == NULL)
		{
			// if realloc fails.
			// free the delay buffer's memory.
			free(left.buffer);
			free(right.buffer);
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
			float tempLeft = delayProgRead(&left, 1);
			float tempRight = delayProgRead(&right, 1);

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
			delayProgWrite(&left, tempLeft);
			delayProgWrite(&right, tempRight);
		}

	}while (echoing == 1);

	// free the delay buffer's memory.
	free(left.buffer);
	free(right.buffer);
	return 1;
}


/**
 * TREMOLO:
 * Applies a variation in volume over time given an LFO shape, frequency, and depth
 ***/
void fileTremolo(wavFilePCM_t* file, lfoShape_t shape, int freq, float depth)
{
	for(int i = 0; i < wavGetSampCount(file); i++)
	{
		// Get the 0 to 1 LFO value
		float lfoValue = lfoGetValueUni(shape, freq, file->FormatChunk.SampleRate, i);

		// if the depth is greater less than 1, adjust the offset accordingly, else it is 0
		float offset = 0;
		if ( depth < 1)
		{
			offset = 1 - depth;
		}

		float multValue = (lfoValue * depth) / 2 + offset;

		sampleMult(&file->data[i], multValue);
	}
}



/**
 * RINGMOD:
 * Applies amplitude modulation and phase inversion over time for a given
 * frequency and depth.
 ***/
void fileRing(wavFilePCM_t* file, lfoShape_t shape, int freq, float depth)
{
	for(int i = 0; i < wavGetSampCount(file); i++)
	{
		// Get the 0 to 1 LFO value
		float lfoValue = lfoGetValue(shape, freq, file->FormatChunk.SampleRate, i);

		float multValue = (lfoValue * depth) / 2;

		sampleMult(&file->data[i], multValue);
	}
}


/**
 * VIBRATO:
 * Applies a variation in pitch over time given a frequency and depth
 ***/
int fileVibrato(wavFilePCM_t* file, lfoShape_t shape, int freq, float depth)
{
	/*
	 * Calculating the new length of the file:
	 * The length of the file will only change if the LFO does not end
	 * on a 0 crossing.  Otherwise, no change will be needed.
	 */
	int oldSize = wavGetSampCount(file);
	int newSize = (lfoGetValueUni(shape, freq, file->FormatChunk.SampleRate, oldSize) * depth * oldSize) + oldSize;

	// allocate delay lines
	delayLine_t left;
	delayLine_t right;
	delayInit(&left, newSize);
	delayInit(&right, newSize);

	// update the length of the file
	int lenChange = newSize - oldSize;
	if (!wavChangeLength(file,lenChange))
	{
		free(left.buffer);
		free(right.buffer);
		return 0;
	}

	// we don't want a delay here, so set the write heads back to 0 to fill the buffers
	delaySetWriteHead(&left, 0);
	delaySetWriteHead(&left, 0);

	// read the file out into the buffer
	// Write the file out into the buffer
	for (int i = 0; i < oldSize; i++)
	{
		delayProgWrite(&left, wavReadAtPosition(file, i).left);
		delayProgWrite(&right, wavReadAtPosition(file, i).right);
	}


	// Read the file back at a variable speed
	for (int i = 0; i < wavGetSampCount(file); i++)
	{
		wavSample_float_t temp = {0,0};

		// Get the 0 to 1 LFO value
		float lfoValue = lfoGetValueUni(shape, freq, file->FormatChunk.SampleRate, i);

		// if the depth is greater less than 1, adjust the offset accordingly, else it is 0
		float offset = 0;
		if ( depth < 1)
		{
			offset = 1 - depth;
		}

		float progress = (lfoValue * depth) / 2 + offset;

		temp.left = delayProgRead(&left, progress);
		temp.right = delayProgRead(&right, progress);
		wavWriteAtPosition(file, i, &temp);
	}
	// free the delay buffers
	free(left.buffer);
	free(right.buffer);
	return 1;
}


/**
 * FLANGE:
 * Applies variation in pitch, and feeds the output back into the input
 ***/
int fileFlange(wavFilePCM_t* file, lfoShape_t shape, int freq, float depth, float feedback)
{
	// protect against hangs:
	if (feedback >= 1)
	{
		feedback = 0.9;
	}

	// create a delay line of 20 milliseconds
	delayLine_t left;
	delayLine_t right;
	delayInit(&left, mstosamps(file->FormatChunk.SampleRate, 20) + 1);
	delayInit(&right,mstosamps(file->FormatChunk.SampleRate, 20) + 1);

	// set the read head 10 ms away from the write head.
	delaySetDistance(&left, mstosamps(file->FormatChunk.SampleRate, 10));
	delaySetDistance(&right, mstosamps(file->FormatChunk.SampleRate, 10));

	// get the size of the file
	int newSize = wavGetSampCount(file);

	// for every sample in the file, read/write into the buffer
	for (int i = 0; i < newSize; i++)
	{
		// Get the 0 to 20ms LFO value
		float lfoValue = (lfoGetValue(shape, freq, file->FormatChunk.SampleRate, i) * 20) + 20;

		// apply the depth
		lfoValue *= depth;

		// calculate and apply the offset
		float offset = 10 - (10 * depth);
		lfoValue += offset;

		printf("%f\n", delayGetDistance(&left));

		// convert lfo to samps
		lfoValue = mstosamps(file->FormatChunk.SampleRate, lfoValue);

		/** READ **/
		// select the sample to delay
		wavSample_float_t tempSamp = file->data[i];

		// read a sample out of the buffer one sample at a time
		float tempLeft = delayStaticRead(&left, 0);
		float tempRight = delayStaticRead(&right, 0);

		/** MODIFY **/
		// mix that data back to the file
		file->data[i].left += tempLeft;
		file->data[i].right += tempRight;

		// Calculate how much of that sample will be fed back into the buffer
		tempLeft *= feedback;
		tempRight *= feedback;

		// add the buffer and the file's samples together
		tempLeft += tempSamp.left;
		tempRight += tempSamp.right;

		/** WRITE **/
		// write them to the buffer
		delayProgWrite(&left, tempLeft);
		delayProgWrite(&right, tempRight);

		// update the delay distance
		delaySetDistance(&left, lfoValue);
		delaySetDistance(&right, lfoValue);
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
		newSize = buffStart + left.size;

		// Reallocate the memory
		wavSample_float_t* tmp = realloc(file->data, (sizeof(wavSample_float_t) * newSize));
		if (tmp == NULL)
		{
			// if realloc fails.
			// free the delay buffer's memory.
			free(left.buffer);
			free(right.buffer);
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
			float tempLeft = delayProgRead(&left, 1);
			float tempRight = delayProgRead(&right, 1);

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
			delayProgWrite(&left, tempLeft);
			delayProgWrite(&right, tempRight);
		}

	}while (echoing == 1);

	// free the delay buffer's memory.
	free(left.buffer);
	free(right.buffer);
	return 1;
}


/**
 * LOWPASS:
 * applies a Low pass filter at a specific cutoff and quality to a file
 ***/
void fileLPF(wavFilePCM_t* file, double cutoff, float q)
{
	// create two filter streams, one for each channel
	filtStream_t left = {
			{0,0,0},
			{0,0,0}
	};
	filtStream_t right = {
			{0,0,0},
			{0,0,0}
	};

	int sampleRate = file->FormatChunk.SampleRate;

	// run the file through the filter sample by sample
	for (int i = 0; i < wavGetSampCount(file); i++)
	{
		filtWriteXStream(&left, file->data[i].left);
		file->data[i].left = filtLowPass(sampleRate, &left, cutoff, q);

		filtWriteXStream(&right, file->data[i].right);
		file->data[i].right = filtLowPass(sampleRate, &right, cutoff, q);
	}
}
