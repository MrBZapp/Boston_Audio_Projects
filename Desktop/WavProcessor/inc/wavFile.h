/*
 * wavFile.h
 *
 *  Created on: Nov 14, 2014
 *      Author: mzapp
 */

#ifndef WAVFILE_H_
#define WAVFILE_H_

#include <stdio.h>
#include <stdlib.h>
#include <libkern/OSByteOrder.h>
#include <limits.h>

#include "wavSample.h"

// Wave Header Chunk structure and type
typedef struct wavHeader{
	int riff;
	int chunkSize;
	int wave;
} wavHeader_t;

// Wave Format chunk structure and type
typedef struct wavFmtChunk{
	int Subchunk1ID;
	int Subchunk1Size;
	short audioFormat;
	short NumChannels;
	int SampleRate;
	int ByteRate;
	short BlockAlign;
	short BitsPerSample;
} wavFmtChunk_t;

// Wave Data Head Chunk structure and type
typedef struct wavDataHead{
	int data;
	int subchunk2Size;
} wavDataHead_t;

// PCM wave file structure and type
typedef struct waveFilePCM{
	wavHeader_t fileHeader;
	wavFmtChunk_t FormatChunk;
	wavDataHead_t dataHeader;
	wavSample_float_t* data;
}wavFilePCM_t;


/**
 * Decodes a wav file, verifying its validity and populating a wavFilePCM_t variable
 ***/
int wavDecode_PCM(wavFilePCM_t* wavFile, FILE* file);

/**
 * Encodes a wav file from a wav file type to a file stream out.
 ***/
int wavEncode_PCM(wavFilePCM_t* wavFile, FILE* file);

/**
 * returns the sample count of a file
 ***/
int wavGetSampCount(wavFilePCM_t* file);

/**
 * update the sample count of a file
 */
void wavSetSampleCount(wavFilePCM_t* file, int sampleCount);

/**
 * returns the sample count of a file
 ***/
int wavGetByteCount(wavFilePCM_t* file);

/**
 * Prints the error code's meaning to the terminal
 * and returns true if an error is found.
 ***/
int wavError(int error);

#endif /* WAVDECODE_H_ */
