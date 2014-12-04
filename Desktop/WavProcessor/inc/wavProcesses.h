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



/**
 * Changes the gain of an entire file
 ***/
void fileGain(wavFilePCM_t* file, float factor);


/**
 * applies a Delinearizing function to every sample in a file
 ***/
void fileDist(wavFilePCM_t* file, distType_t type, float knee);


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


/**
 * Applies amplitude modulation and phase inversion over time for a given
 * frequency and depth
 ***/
void fileRing(wavFilePCM_t* file, lfoShape_t shape, int freq, float depth);


/**
 * Applies a variation in pitch over time given a frequency and depth
 ***/
int fileVibrato(wavFilePCM_t* file, lfoShape_t shape, int freq, float depth);


/**
 * FLANGE:
 * Applies variation in pitch, and feeds the output back into the input
 ***/
int fileFlange(wavFilePCM_t* file, lfoShape_t shape, int freq, float depth, float feedback);


/**
 * applies a Low pass filter at a specific cutoff and quality to a file
 ***/
void fileLPF(wavFilePCM_t* file, double cutoff, float q);

#endif /* WAVPROCESSES_H_ */
