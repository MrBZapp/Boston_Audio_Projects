/*
 * wavProcesses.h
 *
 *  Created on: Nov 16, 2014
 *      Author: mzapp
 */

#ifndef WAVPROCESSES_H_
#define WAVPROCESSES_H_

#include "wavSample.h"
#include "wavFile.h"
#include "lfo.h"


// list of all available processes and their associated numbers

		// gain, reverse, and stretch operations
#define GAIN 1
#define REVERSE 2
#define STRETCH 3

		// delay and echo operations
#define DELAY 4
#define ECHO 5

		// standard modulation operations
#define TREMOLO 6
#define RINGMOD 7
#define VIBRATO 8

		// advanced modulation operations
#define CHORUS 9
#define FLANGE 10


////////////////////////// functions for processing //////////////////////////

/**
 * Changes the gain of an entire file
 ***/
void fileGain(wavFilePCM_t* file, float factor);


/**
 * Reverses a file
 ***/
void fileReverse(wavFilePCM_t* file);


/**
 * stretches or shrinks a file
 * returns 1 on success, 0 upon failure (requires malloc)
 ***/
int filePitch(wavFilePCM_t* file, float pitch);


/**
 * shifts a file in time by an arbitrary amount
 ***/
int fileDelay(wavFilePCM_t* file, float delay);


/**
 * copies a file and repeats it, gradually fading out the copy until all samples are 0
 * returns 1 upon success, 0 upon failure (requires realloc)
 ***/
int fileEcho(wavFilePCM_t* file, long sampDelay, float decay);


/**
 * Applies a variation in volume over time given a frequency and depth
 ***/
void fileTremolo(wavFilePCM_t* file, lfoShape_t shape, int freq, float depth);


void fileRing(wavFilePCM_t* file, lfoShape_t shape, int freq, float depth);

/**
 * Applies a variation in pitch over time given a frequency and depth
 ***/
int fileVibrato(wavFilePCM_t* file, lfoShape_t shape, int freq, float depth);

#endif /* WAVPROCESSES_H_ */
