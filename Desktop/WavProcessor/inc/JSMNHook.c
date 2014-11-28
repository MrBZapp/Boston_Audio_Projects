/*
 * JSMNHook.c
 *
 *  Created on: Nov 27, 2014
 *      Author: mzapp
 */

#include "stdio.h"
#include "string.h"
#include "wavFile.h"
#include "wavProcesses.h"
#include "jsmn/jsmn.h"
//18

#define ERR_BADPROCESS 0
#define ERR_BADMEM 1
#define ERR_BADJSON 2

typedef struct jsonFile
{
	char* text;
	jsmntok_t* tokens;
}jsonFile_t;

/**
 * reads a file into a string and returns a pointer to that string.
 ***/
static char* readJSONFile(const char* JSONPath)
{
    // open input file
	FILE* infile = fopen(JSONPath, "r");
	if(infile == NULL)
	    return NULL;

	// find out how big the file is and reset
	fseek(infile, 0L, SEEK_END);
	long bytecount = ftell(infile);
	fseek(infile, 0L, SEEK_SET);

	// allocate a (hopefully not) epic amount of memory.
	char* json = (char*)calloc(bytecount, sizeof(char));
	if(json == NULL)
	{
		return NULL;
	}

	// if we've got a pointer, turn that mother out and close the file.
	fread(json, sizeof(char), bytecount, infile);
	fclose(infile);

	// return the string
	return json;
}


/**
 * compares a token against a string.  returns 1 if match, 0 if no match
 */
static int jsonStrcmp(jsonFile_t* jsonFile, const char* s, unsigned int location) {
	// get the length of the token
	int tokLength = jsonFile->tokens[location].end - jsonFile->tokens[location].start;

	// if the token is a string AND the string is the right length AND the string is the right string...
	// return 1.
	if (jsonFile->tokens[location].type == JSMN_STRING
			&& (int) strlen(s) == tokLength
			&& strncmp(jsonFile->text + jsonFile->tokens[location].start, s, tokLength) == 0)
	{
		return 1;
	}
	return 0;
}

int getProcessType(jsonFile_t* jsonFile, unsigned int index)
{
	if (jsonStrcmp(jsonFile, "type", index))
	{
		// get the type of the processing
		index++;
		// gain, reverse, and stretch operations
		if (jsonStrcmp(jsonFile, "gain", index))
			return GAIN;
		else if (jsonStrcmp(jsonFile, "reverse", index))
			return REVERSE;
		else if (jsonStrcmp(jsonFile, "stretch", index))
			return STRETCH;

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
		else
			return ERR_BADPROCESS;

	}
	return ERR_BADJSON;
}

/**
 * selects and applies processing from a json string and a set of token markers
 ***/
static int processFromTokens(wavFilePCM_t* file, jsonFile_t* jsonFile, int tokenCount)
{
	// require we start with a JSMN object
	if (tokenCount < 1 || jsonFile->tokens[0].type != JSMN_OBJECT) {
		return ERR_BADJSON;
	}

	unsigned int processCount = 0;

	// iterate over all tokens
	for (int i = 0; i <= tokenCount; i++)
	{
		// If we find a process token, add it to the list
		if (jsonStrcmp(jsonFile, "Process", i))
		{
			//get the next token
			i++;
			// if it isn't an object, throw it back.
			if (jsonFile->tokens[i].type != JSMN_OBJECT)
			{
				return ERR_BADJSON;
			}

			//get the next token
			i++;
			int processType = getProcessType(jsonFile, i);

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
int processWavFromJSON(wavFilePCM_t* wavFile, const char* JSONPath)
{
	// create a blank JSON file.
	jsonFile_t jsonFile;

	// try to read out the file
	jsonFile.text = readJSONFile(JSONPath);
	jsonFile.tokens = NULL;

	if (jsonFile.text == NULL)
		return ERR_BADMEM;

	// create and initialize the jsmn parser
	jsmn_parser parser;
	jsmn_init(&parser);

	// prepare to read an arbitrary number of tokens
	unsigned int tokenCount = 0;

	/* We assume we're not giving the parser enough memory to work with,
	 * and gradually increase it so as to not limit the amount of processing
	 * one can do to a file.
	 *
	 * if realloc ever fails this should abort, but this has the potential
	 * to hang if there are any other memory errors within jsmn. There shouldn't
	 * be; NOMEM only returns when the user doesn't give jsmn enough memory,
	 * and they don't alloc anything so gradually increasing the memory provided
	 * should be sufficient to avoid hangs.
	 */
	jsmnerr_t err = JSMN_ERROR_NOMEM;
	printf("Parsing JSON\n");
	while (err == JSMN_ERROR_NOMEM)
	{
		//expand the token count
		tokenCount++;
		jsmntok_t* tmp = realloc(jsonFile.tokens, sizeof(jsmntok_t) * tokenCount);

		// make sure the pointer is valid before assignment
		if (tmp == NULL)
		{
			// if it isn't, make sure the tokens pointer is free'd before returning
			if (jsonFile.tokens != NULL)
			{
				free(jsonFile.tokens);
			}

			// we're done with the string, free that memory.
			free(jsonFile.text);
			return ERR_BADMEM;
		}

		// reassign the pointer.
		jsonFile.tokens = tmp;

		// re-initialize the parser
		jsmn_init(&parser);

		// try to read the file
		err = jsmn_parse(&parser, jsonFile.text,  strlen(jsonFile.text),jsonFile.tokens , tokenCount );
	}

	// Process the audio!
	err = processFromTokens(wavFile, &jsonFile, tokenCount);

	// we're done with the string and tokens, free that memory.
	free(jsonFile.text);
	free(jsonFile.tokens);
	// Complete! return no errors!
	return 0;
}
