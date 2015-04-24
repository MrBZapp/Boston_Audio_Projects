/*
 * wavFile.c
 *
 *  Created on: Nov 14, 2014
 *      Author: mzapp
 */

#include <stdio.h>
#include <stdlib.h>
//#include <libkern/OSByteOrder.h>
#include <math.h>
#include "wavSample.h"
#include "wavFile.h"

/**
 * Decodes a file as a wav file, verifying its validity and populating a wavFilePCM_t variable,
 * and swapping the endianness of the file's data to match the host system's.
 ***/
int wavDecode_PCM(wavFilePCM_t* wavFile, FILE* file){

	// read the file header
	fread(&wavFile->fileHeader, sizeof(wavHeader_t), 1, file);

	// swap the bytes if necessary according to the endianness in the wav spec
	wavFile->fileHeader.riff = OSSwapBigToHostInt(wavFile->fileHeader.riff);
	wavFile->fileHeader.wave = OSSwapBigToHostInt(wavFile->fileHeader.wave);
	wavFile->fileHeader.chunkSize = OSSwapLittleToHostInt(wavFile->fileHeader.chunkSize);

	// check if the file is a wav file.
	if (wavFile->fileHeader.riff != 'RIFF' || wavFile->fileHeader.wave != 'WAVE'){
		// if not, return error 1.
		return 1;
	}

	// read the format chunk
	fread(&wavFile->FormatChunk, sizeof(wavFmtChunk_t), 1, file);

	// swap the bytes in the format chunk to match wav spec
	wavFile->FormatChunk.Subchunk1ID = OSSwapBigToHostInt(wavFile->FormatChunk.Subchunk1ID);
	wavFile->FormatChunk.Subchunk1Size = OSSwapLittleToHostInt(wavFile->FormatChunk.Subchunk1Size);
	wavFile->FormatChunk.audioFormat = OSSwapLittleToHostInt(wavFile->FormatChunk.audioFormat);
	wavFile->FormatChunk.NumChannels = OSSwapLittleToHostInt(wavFile->FormatChunk.NumChannels);
	wavFile->FormatChunk.SampleRate = OSSwapLittleToHostInt(wavFile->FormatChunk.SampleRate);
	wavFile->FormatChunk.ByteRate = OSSwapLittleToHostInt(wavFile->FormatChunk.ByteRate);
	wavFile->FormatChunk.BlockAlign = OSSwapLittleToHostInt(wavFile->FormatChunk.BlockAlign);
	wavFile->FormatChunk.BitsPerSample = OSSwapLittleToHostInt(wavFile->FormatChunk.BitsPerSample);

	// check if the file's format chunk is valid
	if (wavFile->FormatChunk.Subchunk1ID != 'fmt '){
		return 2;
	}

	// check if the file is in PCM format
	if (wavFile->FormatChunk.Subchunk1Size != 16 || wavFile->FormatChunk.audioFormat != 1){
		return 3;
	}

	// read the data header
	fread(&wavFile->dataHeader, sizeof(wavDataHead_t), 1, file);

	// swap the bytes in the data chunk to match the wav spec
	wavFile->dataHeader.data = OSSwapBigToHostInt(wavFile->dataHeader.data);
	wavFile->dataHeader.subchunk2Size = OSSwapLittleToHostInt(wavFile->dataHeader.subchunk2Size);


	// check to see if the data header is valid
	if (wavFile->dataHeader.data != 'data'){
		return 4;
	}

	// calculate the bytes per sample
	int byteCount = wavGetByteCount(wavFile);

	// calculate sample count: subchunk2Size = NumSamples * NumChannels * BitsPerSample/8.
	long sampleCount = wavGetSampCount(wavFile);

	// allocate enough memory for each of the file's samples
	wavFile->data = calloc(sampleCount, sizeof(wavSample_float_t));
	if (wavFile->data == NULL)
	{
		return 6;
	}

	// read the entire file out into memory
	// mono
	if (wavFile->FormatChunk.NumChannels == 1)
	{
		for (int i = 0; i < sampleCount; i++)
		{
			// temp data for 16-bit ints. Trololol.
			short temper = 0;

			// mono files, only use the left channel of the sample
			fread(&temper, byteCount, 1, file);

			// check for unexpected EOF
			if( feof(file)){
				free(wavFile->data);
				return 7;
			}

			// ensure endianness is correct
			temper = OSSwapLittleToHostInt(temper);

			// Write the data to memory as a float
			wavFile->data[i].left = temper;
		}
	}
	// stereo
	else if (wavFile->FormatChunk.NumChannels == 2)
	{
		for (int i = 0; i < sampleCount; i++)
		{
			// temp data for 16-bit ints.
			short temper = 0;

			// Left channel
			fread(&temper, byteCount, 1, file);

			// check for unexpected EOF
			if( feof(file)){
				free(wavFile->data);
				return 7;
			}

			// ensure endianness is correct
			temper = OSSwapLittleToHostInt(temper);

			// Write the data to memory as a float
			wavFile->data[i].left = temper;

			// Right channel
			fread(&temper, byteCount, 1, file);

			// check for unexpected EOF
			if( feof(file)){
				free(wavFile->data);
				return 7;
			}

			// ensure endianness is correct
			temper = OSSwapLittleToHostInt(temper);

			// Write the data to memory
			wavFile->data[i].right = temper;
		}
	}
	else
	{
		return 5;
	}

	// all clear!
	return 0;
}


/**
 * Encodes a wav file from a wav file type to a file stream out.
 *
 * HACK WARNING: the output is ONLY available in 16-bit audio at the moment.
 * as a result, a few assumptions are being made that will need to be addressed
 * if a more full-featured implementation is to be built.
 *
 * TODO: Don't hard-code the bits-per sample to 16
 ***/
int wavEncode_PCM(wavFilePCM_t* wavFile, FILE* file)
{
	wavFilePCM_t tempSamp;

	// Encode the file header
	// swap the bytes if necessary according to the endianness in the wav spec
	tempSamp.fileHeader.riff = OSSwapHostToBigInt(wavFile->fileHeader.riff);
	tempSamp.fileHeader.wave = OSSwapHostToBigInt(wavFile->fileHeader.wave);
	tempSamp.fileHeader.chunkSize = OSSwapHostToLittleInt(wavFile->fileHeader.chunkSize);

	// write the file header
	fwrite(&tempSamp.fileHeader, sizeof(tempSamp.fileHeader), 1, file);

	// Encode the format chunk
	// swap the bytes in the format chunk to match wav spec
	tempSamp.FormatChunk.Subchunk1ID = OSSwapHostToBigInt(wavFile->FormatChunk.Subchunk1ID);
	tempSamp.FormatChunk.Subchunk1Size = OSSwapHostToLittleInt(wavFile->FormatChunk.Subchunk1Size);
	tempSamp.FormatChunk.audioFormat = OSSwapHostToLittleInt(wavFile->FormatChunk.audioFormat);
	tempSamp.FormatChunk.NumChannels = OSSwapHostToLittleInt(wavFile->FormatChunk.NumChannels);
	tempSamp.FormatChunk.SampleRate = OSSwapHostToLittleInt(wavFile->FormatChunk.SampleRate);
	tempSamp.FormatChunk.ByteRate = OSSwapHostToLittleInt(wavFile->FormatChunk.ByteRate);
	tempSamp.FormatChunk.BlockAlign = OSSwapHostToLittleInt(wavFile->FormatChunk.BlockAlign);
	tempSamp.FormatChunk.BitsPerSample = OSSwapHostToLittleInt(BITDEPTH_OUT);

	// write format chunk
	fwrite(&tempSamp.FormatChunk, sizeof(tempSamp.FormatChunk), 1, file);

	// Encode the data header
	tempSamp.dataHeader.data = OSSwapHostToBigInt(wavFile->dataHeader.data);
	tempSamp.dataHeader.subchunk2Size = OSSwapHostToLittleInt(wavFile->dataHeader.subchunk2Size);

	// write write data header
	fwrite(&tempSamp.dataHeader, sizeof(tempSamp.dataHeader), 1, file);

	// Encode the data
	// calculate the bytes per sample per channel.
	int byteCount = wavGetByteCount(wavFile);

	// calculate sample count: subchunk2Size = NumSamples * NumChannels * BitsPerSample/8.
	int sampleCount = wavGetSampCount(wavFile);

	// write the data out to the file stream
	// mono files, only use the left channel of the sample
	if (wavFile->FormatChunk.NumChannels == 1)
	{
		for (int i = 0; i < sampleCount; i++)
		{
			// clamp the sample to the maximum and minimum available in a short
			// Clip the top and bottom
			sampleClip16Bit(&wavFile->data[i]);

			// write float to short
			short temper = wavFile->data[i].left;

			// ensure proper endianness
			temper = OSSwapHostToLittleInt(temper);
			fwrite(&temper, byteCount, 1, file);
		}
	}
	// stereo
	else if (wavFile->FormatChunk.NumChannels == 2)
	{
		for (int i = 0; i < sampleCount; i++)
		{
			// Clip the top and bottom
			sampleClip16Bit(&wavFile->data[i]);

			// write float to short LEFT
			short temper = wavFile->data[i].left;

			// ensure proper endianness
			temper = OSSwapHostToLittleInt(temper);
			fwrite(&temper, byteCount, 1, file);

			// write float to short RIGHT
			temper = wavFile->data[i].right;

			// ensure proper endianness
			temper = OSSwapHostToLittleInt(temper);
			fwrite(&temper, byteCount, 1, file);
		}
	}
	else
	{
		return 5;
	}
	return 0;
}


/**
 * changes the length of a file, truncating if making shorter,
 * and reallocing/zeroing data if longer.
 ***/
int wavChangeLength(wavFilePCM_t* file, long change)
{
	// we don't need to do a ton of work moving data around if we're not changing anything
	if (change == 0)
		return 1;

	// get the old and new lengths
	long oldSize = wavGetSampCount(file);
	long newSize = oldSize + change;

	// if we've been asked to make a file with a nonsense length, fail.
	if (newSize <= 0 )
		return 0;


	// realloc to the new size
	wavSample_float_t* tmp = realloc(file->data, sizeof(wavSample_float_t) * newSize);
	if (tmp == NULL)
	{
		return 0;
	}

	// reset the data to the new size.
	file->data = tmp;

	// zero any new data
	for (int i = oldSize; i < newSize; i++)
	{
		file->data[i].left = 0;
		file->data[i].right = 0;
	}

	// Update the sample count
	wavSetSampleCount(file, newSize);

	return 1;
}


/**
 * returns the sample count of a file
 ***/
int wavGetByteCount(wavFilePCM_t* file)
{
	// calculate the bytes per sample
	return (file->FormatChunk.BitsPerSample / 8);
}


/**
 * returns the sample count of a file
 ***/
int wavGetSampCount(wavFilePCM_t* file)
{
	// calculate the bytes per sample
	int byteCount = wavGetByteCount(file);

	// calculate sample count: subchunk2Size = NumSamples * NumChannels * BitsPerSample/8.
	return file->dataHeader.subchunk2Size / (file->FormatChunk.NumChannels * byteCount);
}


/**
 *
 ***/
wavSample_float_t wavReadAtPosition(wavFilePCM_t* file, long sample)
{
	wavSample_float_t x = {0, 0};
	if (sample < 0 || sample > wavGetSampCount(file))
		return x;
	x = file->data[sample];
	return x;
}


/**
 *
 ***/
void wavWriteAtPosition(wavFilePCM_t* file, long position, wavSample_float_t* sample)
{
	if (position < 0 || position > wavGetSampCount(file))
		return;
	file->data[position] = *sample;
	return;
}


/**
 * update the sample count of a file
 ***/
void wavSetSampleCount(wavFilePCM_t* file, int sampleCount)
{
	// update the subchunk size
	file->dataHeader.subchunk2Size = sampleCount * file->FormatChunk.NumChannels * (file->FormatChunk.BitsPerSample / 8);
}


/**
 * Prints the error code's meaning to the terminal
 * and returns true if an error is found.
 ***/
int wavError(int error){
	switch(error){
	case(0):
			return 0;
			break;
	case(1):
			printf("No valid *.wav header found.\n");
			return 1;
			break;
	case(2):
			printf("File's format chunk is corrupted.\n");
			return 1;
			break;
	case(3):
			printf("File is not in PCM format.\n");
			return 1;
			break;
	case(4):
			printf("File's data header is corrupted.\n");
			return 1;
			break;
	case(5):
			printf("File has must be mono or stereo only.");
			return 1;
			break;
	case(6):
			printf("Not enough available memory!");
			return 1;
			break;
	case(7):
			printf("Unexpected end of file!");
			return 1;
			break;
	default:
		printf("An unknown error has occurred.\n");
		return 1;
		break;
	}
	return 0;
}
