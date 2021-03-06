/****
 * wavFile.h
 *
 *  Created on: Nov 14, 2014
 *      Author: mzapp
 *
 *  functions associated with encoding and decoding "cannonical"
 *  *.wav format files.  Cannonical wav format spec can be found
 *  at ccrma's website on the subject:
 *
 *  https://ccrma.stanford.edu/courses/422/projects/WaveFormat/
 ****/

#ifndef WAVFILE_H_
#define WAVFILE_H_

#include <stdio.h>
#include <stdlib.h>
//#include <libkern/OSByteOrder.h>
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
 * changes the length of a file, truncating if making shorter,
 * and reallocing/zeroing data if longer.
 ***/
int wavChangeLength(wavFilePCM_t* file, long change);


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
 * returns the sample of an theoretically infinite file.
 ***/
wavSample_float_t wavReadAtPosition(wavFilePCM_t* file, long sample);

/**
 * Writes a sample at a position in a file.  automatically bounds to the file's length.
 ***/
void wavWriteAtPosition(wavFilePCM_t* file, long position, wavSample_float_t* sample);

/**
 * Prints the error code's meaning to the terminal
 * and returns true if an error is found.
 ***/
int wavError(int error);

#endif /* WAVDECODE_H_ */
