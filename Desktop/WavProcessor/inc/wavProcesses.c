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
int fileEcho(wavFilePCM_t* file, long sampDelay, float decay)
{
	// create a copy of the file
	int sampleCount = wavGetSampCount(file);
	int echoSampCount = sampleCount;
	wavSample_float_t* echo = calloc(sizeof(wavSample_float_t), sampleCount);
	memcpy(echo, file->data, sampleCount * sizeof(wavSample_float_t));

	// echoing flag
	int echoing = 0;

	// used to keep track of the write head for the echo
	long writeHead = sampDelay;

	do
	{
		// assume we've run out of data to copy
		echoing = 0;

		// modify the gain of the echo.
		for ( int i = 0; i < echoSampCount; i++)
		{
			wavSample_float_t test = sampleMult((echo + i), decay);

			// if any sample doesn't return 0, we're still echoing.
			if (((short) test.left) != 0 || ((short)test.right) != 0)
			{
				echoing = 1;
			}
		}

		// only do this if we're actually echoing
		if(echoing == 1)
		{
			// calculate the number up updated samples
			sampleCount += sampDelay;

			// Reallocate the memory
			wavSample_float_t* tmp = realloc(file->data, (sizeof(wavSample_float_t) * sampleCount));
			if (tmp == NULL)
			{
				// exit if realloc fails.
				return 0;
			}

			// If successful, update the data pointer
			file->data = tmp;

			// update the file header
			wavSetSampleCount(file, sampleCount);

			// zero the new memory
			for ( int i = sampleCount - sampDelay; i < sampleCount; i++ )
			{
				file->data[i].left = 0;
				file->data[i].right = 0;
			}

			// add the echo
			for (int i = 0; i < echoSampCount; i++)
			{
				file->data[i + writeHead].left += echo[i].left;
				file->data[i + writeHead].right += echo[i].right;
			}

			// update the write head with the new position of the echo
			writeHead += sampDelay;
		}

	}while (echoing == 1);

	return 1;
}
