/*
 * JSMNHook.c
 *
 *  Created on: Nov 27, 2014
 *      Author: mzapp
 */

#include "wavFile.h"
#include "wavProcesses.h"
#include "processParams.h"
#include "jsmn/jsmn.h"
#include "JSON.h"

//18

#define ERR_BADMEM -1
#define ERR_BADJSON -2
#define ERR_BADPROCESS -3


/**
 * returns the type of process specified by a provided "type" token
 */
proscessType_t getProcessType(jsonFile_t* jsonFile, unsigned int index)
{
	if (jsonStrcmp(jsonFile, "type", index))
	{
		// get the type of the processing
		index++;
		// gain, reverse, distortion, and stretch operations
		if (jsonStrcmp(jsonFile, "gain", index))
			return GAIN;
		else if (jsonStrcmp(jsonFile, "reverse", index))
			return REVERSE;
		else if (jsonStrcmp(jsonFile, "stretch", index))
			return STRETCH;
		else if (jsonStrcmp(jsonFile, "distort", index))
			return DISTORT;

		// delay and echo operations
		else if (jsonStrcmp(jsonFile, "delay", index))
			return DELAY;
		else if (jsonStrcmp(jsonFile, "echo", index))
			return ECHO;

		// standard modulation operations
		else if (jsonStrcmp(jsonFile, "tremolo", index))
			return TREMOLO;
		else if (jsonStrcmp(jsonFile, "ringmod", index))
			return RINGMOD;
		else if (jsonStrcmp(jsonFile, "vibrato", index))
			return VIBRATO;

		// advanced modulation operations
		if (jsonStrcmp(jsonFile, "chorus", index))
			return CHORUS;
		if (jsonStrcmp(jsonFile, "flange", index))
			return FLANGE;
		if (jsonStrcmp(jsonFile, "lowpass", index))
			return LOWPASS;
		else
			return ERR_BADPROCESS;

	}
	return ERR_BADJSON;
}


/**
 * parses parameters for each type of processor from a
 * location in a json file.  returns the distance the
 * global index needs to be advanced, and places
 * information into an array of parameters.
 * expects a params token to being, or will return 0.
 ***/
int jsonGetParams(jsonFile_t* jsonFile, int tokIndex, proscessType_t processType, float* params)
{
	// keep track of how man tokens we consume
	int counterAdv = 0;

	// are we at a params index?
	if (!jsonStrcmp(jsonFile,"params", tokIndex))
		return 0;

	// if we are indeed at the params, make sure there's an object there.
	tokIndex++;
	counterAdv++;

	// make sure we're not trying to read things that don't exist
	if(tokIndex >= jsonFile->tokenCount)
		return counterAdv;

	//if there isn't a params object, return and use the process defaults.
	if (jsonFile->tokens[tokIndex].type != JSMN_OBJECT)
	{
		return counterAdv;
	}

	// token index should now be inside the params object
	tokIndex++;
	counterAdv++;

	// make sure we're not trying to read things that don't exist
	if(tokIndex >= jsonFile->tokenCount)
		return counterAdv;

	// while we haven't hit another process, or the end of the available tokens:
	while(tokIndex < jsonFile->tokenCount && !jsonStrcmp(jsonFile, "process", tokIndex))
	{
		switch (processType)
		{
		case(GAIN):
				// find the parameter
				if (jsonStrcmp(jsonFile, "mult", tokIndex))
				{
					// get the value and place it in the appropriate params location
					params[0] = jsonTokToF(jsonFile, tokIndex + 1);
				}
				break;

		case(DISTORT):
				// find the parameter
				if (jsonStrcmp(jsonFile, "algorithm", tokIndex))
				{
					// get the value and place it in the appropriate params location
					params[0] = jsonTokToF(jsonFile, tokIndex + 1);
				}
				// find the parameter
				if (jsonStrcmp(jsonFile, "knee", tokIndex))
				{
					// get the value and place it in the appropriate params location
					params[1] = jsonTokToF(jsonFile, tokIndex + 1);
				}
				break;

		case(REVERSE):
				break;

		case(STRETCH):
				if (jsonStrcmp(jsonFile, "ratio", tokIndex))
				{
					params[0] = jsonTokToF(jsonFile, tokIndex + 1);
				}
				break;

		case(DELAY):
				if (jsonStrcmp(jsonFile, "samples", tokIndex))
				{
					params[0] = jsonTokToF(jsonFile, tokIndex + 1);
				}
				break;

		case(ECHO):
				if (jsonStrcmp(jsonFile, "samples", tokIndex))
				{
					params[0] = jsonTokToF(jsonFile, tokIndex + 1);
				}
				if (jsonStrcmp(jsonFile, "feedback", tokIndex))
				{
					params[1] = jsonTokToF(jsonFile, tokIndex + 1);
				}
				break;

		case(TREMOLO):
				if (jsonStrcmp(jsonFile, "shape", tokIndex))
				{
					params[0] = jsonTokToF(jsonFile, tokIndex + 1);
				}
				if (jsonStrcmp(jsonFile, "freq", tokIndex) ||
					jsonStrcmp(jsonFile, "frequency", tokIndex) ||
					jsonStrcmp(jsonFile, "rate", tokIndex))
				{
					params[1] = jsonTokToF(jsonFile, tokIndex + 1);
				}
				if (jsonStrcmp(jsonFile, "depth", tokIndex))
				{
					params[2] = jsonTokToF(jsonFile, tokIndex + 1);
				}
				break;

		case(RINGMOD):
				if (jsonStrcmp(jsonFile, "shape", tokIndex))
				{
					params[0] = jsonTokToF(jsonFile, tokIndex + 1);
				}
				if (jsonStrcmp(jsonFile, "freq", tokIndex) ||
					jsonStrcmp(jsonFile, "frequency", tokIndex) ||
					jsonStrcmp(jsonFile, "rate", tokIndex))
				{
					params[2] = jsonTokToF(jsonFile, tokIndex + 1);
				}
				if (jsonStrcmp(jsonFile, "depth", tokIndex))
				{
					params[3] = jsonTokToF(jsonFile, tokIndex + 1);
				}
				break;

		case(VIBRATO):
				if (jsonStrcmp(jsonFile, "shape", tokIndex))
				{
					params[0] = jsonTokToF(jsonFile, tokIndex + 1);
				}
				if (jsonStrcmp(jsonFile, "freq", tokIndex) ||
				jsonStrcmp(jsonFile, "frequency", tokIndex) ||
				jsonStrcmp(jsonFile, "rate", tokIndex))
				{
					params[1] = jsonTokToF(jsonFile, tokIndex + 1);
				}
				if (jsonStrcmp(jsonFile, "depth", tokIndex))
				{
					params[2] = jsonTokToF(jsonFile, tokIndex + 1);
				}
				break;

		case(CHORUS):
				if (jsonStrcmp(jsonFile, "shape", tokIndex))
				{
					params[0] = jsonTokToF(jsonFile, tokIndex + 1);
				}
				if (jsonStrcmp(jsonFile, "freq", tokIndex) || jsonStrcmp(jsonFile, "frequency", tokIndex) )
				{
					params[1] = jsonTokToF(jsonFile, tokIndex + 1);
				}
				if (jsonStrcmp(jsonFile, "depth", tokIndex))
				{
					params[2] = jsonTokToF(jsonFile, tokIndex + 1);
				}
				if (jsonStrcmp(jsonFile, "mix", tokIndex))
				{
					params[3] = jsonTokToF(jsonFile,tokIndex + 1);
				}
				break;

		case(FLANGE):
				break;

		case(LOWPASS):
				if (jsonStrcmp(jsonFile, "cutoff", tokIndex))
				{
					params[0] = jsonTokToF(jsonFile, tokIndex + 1);
				}
				if (jsonStrcmp(jsonFile, "q", tokIndex))
				{
					params[1] = jsonTokToF(jsonFile,tokIndex + 1);
				}
				break;
		default:
			// this is for catching errors.
			break;
		}

		tokIndex++;
		counterAdv++;
	}

	// the minus 1 here is to allow the main loop to account for the "process" token
	return counterAdv - 1;
}


/**
 * selects and applies processing from a json string and a set of token markers
 ***/
static int processFromJSON(wavFilePCM_t* wavFile, jsonFile_t* jsonFile)
{
	// require we start with a JSMN object
	if (jsonFile->tokenCount < 1 || jsonFile->tokens[0].type != JSMN_OBJECT) {
		return ERR_BADJSON;
	}

	unsigned int processCount = 0;

	// iterate over all tokens
	for (int i = 1; i < jsonFile->tokenCount; i++)
	{
		// If we find a process token, dive into that object.
		if (jsonStrcmp(jsonFile, "process", i))
		{
			//if there isn't a process object, skip that process, it's bogus.
			if (jsonFile->tokens[i + 1].type != JSMN_OBJECT)
			{
				//return ERR_BADJSON;
				continue;
			}

			// i now points to Process Object
			i++;

			// get the process by submitting the type token
			// i now points to the Type Token
			i++;


			// make sure we're not trying to read things that don't exist
			if(i >= jsonFile->tokenCount)
				continue;

			int processType = getProcessType(jsonFile, i);
			// i now points to the process type token
			i++;


			// advance the index to indicate the process type has been acquired
			// i now points to the params token
			i++ ;

			// make sure we're not trying to read things that don't exist
			if(i >= jsonFile->tokenCount)
				continue;

			// using do loops to maintain scope of the
			// params array without wigging out the compiler
			switch (processType)
			{
			case(GAIN):
					do
					{
						// set default params
						float params[1] = {1};

						// get params from JSON file and advance the counter for each token consumed
						i += jsonGetParams(jsonFile, i, GAIN, params);

						// tell the user what they're doing
						printf("process: gain\n    params:\n        gain: %f\n", params[0]);

						// apply params
						fileGain(wavFile, params[0]);
					} while(0);
					break;

			case(DISTORT):
					do
					{
						// set default params
						float params[2] = {0, 0};

						// get params from JSON file and advance the counter for each token consumed
						i += jsonGetParams(jsonFile, i, DISTORT, params);

						// tell the user what they're doing
						printf("process: distort\n    params:\n        algorithm: %f\n        knee: %f\n", params[0], params[1]);

						// apply params
						fileDist(wavFile, params[0], params[1]);
					} while(0);
					break;

			case(REVERSE):
					// tell the user what they are doing
					printf("process: reverse\n");
					fileReverse(wavFile);
					break;

			case(STRETCH):
					do
					{
						// set default params
						float params[1] = {1};

						// get params from JSON file and advance the counter for each token consumed
						i += jsonGetParams(jsonFile, i, STRETCH, params);

						// tell the user what they're doing
						printf("process: stretch\n    params:\n        ratio: %f\n", params[0]);

						// apply params
						filePitch(wavFile, params[0]);
					} while(0);
					break;

			case(DELAY):
					do
					{
						// set default params
						float params[1] = {wavGetSampCount(wavFile)};

						// get params from JSON file and advance the counter for each token consumed
						i += jsonGetParams(jsonFile, i, DELAY, params);

						// tell the user what they're doing to their file.
						printf("process: delay\n    params:\n        samples: %f\n", params[0]);

						// apply params
						fileDelay(wavFile, params[0]);
					} while(0);
					break;

			case(ECHO):
					do
					{
						// set default params
						float params[2] = {wavFile->FormatChunk.SampleRate, 0};

						// get params from JSON file and advance the counter for each token consumed
						i += jsonGetParams(jsonFile, i, ECHO, params);

						// tell the user what they're doing to their file.
						printf("process: echo\n    params:\n        samples: %f\n        feedback: %f\n", params[0], params[1]);

						// apply params
						fileEcho(wavFile, params[0], params[1]);
					} while(0);
					break;

			case(TREMOLO):
					do
					{
						// set default params
						float params[3] = {SINE, 1, 0};

						// get params from JSON file and advance the counter for each token consumed
						i += jsonGetParams(jsonFile, i, TREMOLO, params);

						// tell the user what they are doing
						printf("process: tremolo\n    params:\n        shape: %f\n        rate: %f\n        depth: %f\n", params[0], params[1], params[2]);

						// apply params
						fileTremolo(wavFile,params[0], params[1], params[2]);
					} while(0);
					break;

			case(RINGMOD):
					do
					{
						// set default params
						float params[3] = {SINE, 1, 1};

						// get params from JSON file and advance the counter for each token consumed
						i += jsonGetParams(jsonFile, i, RINGMOD, params);

						// tell the user what they are doing
						printf("process: ringmod\n    params:\n        shape: %f\n        rate: %f\n        depth: %f\n", params[0], params[1], params[2]);

						// apply params
						fileRing(wavFile,params[0], params[1], params[2]);
					} while(0);
					break;

			case(VIBRATO):
					do
					{
						// set default params
						float params[3] = {SINE, 1, 0};

						// get params from JSON file and advance the counter for each token consumed
						i += jsonGetParams(jsonFile, i, VIBRATO, params);

						// tell the user what they are doing
						printf("process: vibrato\n    params:\n        shape: %f\n        rate: %f\n        depth: %f\n", params[0], params[1], params[2]);

						// apply params
						fileVibrato(wavFile,params[0], params[1], params[2]);
					} while(0);
					break;

			case(CHORUS):
					break;

			case(FLANGE):
					break;

			case(LOWPASS):
					do
					{
						// set default params
						float params[2] = {10000, 1};

						// get params from JSON file and advance the counter for each token consumed
						i += jsonGetParams(jsonFile, i, LOWPASS, params);

						// tell the user what they are doing
						printf("process: lowpass\n    params:\n        cutoff: %f\n        q: %f\n", params[0], params[1]);

						// apply params
						fileLPF(wavFile, params[0], params[1]);
					} while(0);
					break;
			default:
				// this is for catching errors.
				processCount--;
				break;
			}

			processCount++;
	}
	}

	printf("%i processes applied to file.\n", processCount);

	return 0;
}


/**
 * Takes a wave file and a file path as an argument, parses the JSON
 * and passes the commands on to processing. will return errors if
 * they occur during parsing or processing. returns an error code.
 ***/
int processWavFromJSONString(wavFilePCM_t* wavFile, const char* JSONPath)
{
	int err = 0;

	jsonFile_t* jsonFile = openJSONFile(JSONPath);

	if (jsonFile == NULL)
	{
		printf("Failed to read JSON file.\n");
		return -1;
	}

	// Process the audio!
	err = processFromJSON(wavFile, jsonFile);

	// we're done with the json file, close it down
	closeJSONFile(jsonFile);

	// Complete! return no errors!
	return 0;
}
