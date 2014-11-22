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
 ***/
void filePitch(wavFilePCM_t* file);

/**
 * copies a file and repeats it, gradually fading out the copy until all samples are 0
 * returns 1 upon success, 0 upon failure (requires realloc)
 ***/
int fileEcho(wavFilePCM_t* file, long sampDelay, float decay);

#endif /* WAVPROCESSES_H_ */
