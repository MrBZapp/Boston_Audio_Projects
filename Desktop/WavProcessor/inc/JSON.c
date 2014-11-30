/*
 * JSON.C
 *
 *  Created on: Nov 30, 2014
 *      Author: mzapp
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "JSON.h"
#include "jsmn/jsmn.h"

/**
 * reads a file path into a JSON structure and returns
 * a pointer to that structure
 ***/
jsonFile_t* openJSONFile(const char* JSONPath)
{
    // open input file
	FILE* infile = fopen(JSONPath, "r");
	if(infile == NULL)
	    return NULL;

	// create a blank JSON file.
	jsonFile_t* jsonFile  = calloc(1, sizeof(jsonFile_t));

	// find out how big the file is and reset
	fseek(infile, 0L, SEEK_END);
	long bytecount = ftell(infile);
	fseek(infile, 0L, SEEK_SET);

	// allocate a (hopefully not) epic amount of memory for the text.
	jsonFile->text = calloc(bytecount, sizeof(char));
	if(jsonFile->text == NULL)
	{
		free(jsonFile);
		return NULL;
	}

	// if we've got a pointer, turn that mother out and close the file.
	fread(jsonFile->text, sizeof(char), bytecount, infile);
	fclose(infile);

	// create and initialize the jsmn parser
	jsmn_parser parser;
	jsmn_init(&parser);

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

	jsonFile->tokenCount = 0;

	while (err == JSMN_ERROR_NOMEM)
	{
		//expand the token count
		jsonFile->tokenCount++;
		jsmntok_t* tmp = realloc(jsonFile->tokens, sizeof(jsmntok_t) * jsonFile->tokenCount);

		// make sure the pointer is valid before assignment
		if (tmp == NULL)
		{
			// if it isn't, make sure the tokens pointer is free'd before returning
			if (jsonFile->tokens != NULL)
			{
				free(jsonFile->tokens);
			}

			// we're done with the string, free that memory.
			free(jsonFile->text);
			return NULL;
		}

		// reassign the pointer.
		jsonFile->tokens = tmp;

		// re-initialize the parser
		jsmn_init(&parser);

		// try to read the file
		err = jsmn_parse(&parser, jsonFile->text, strlen(jsonFile->text), jsonFile->tokens, jsonFile->tokenCount );
	}

	// if there were any other errors, do not return the file.
	if (err != 0)
	{
		free(jsonFile->text);
		free(jsonFile);
		return NULL;
	}

	// return the file
	return jsonFile;
}


/**
 * comprises all the steps required to close a JSON file down.
 ***/
void closeJSONFile(jsonFile_t* file)
{
}


/**
 * compares a token against a string.  returns 1 if match, 0 if no match
 */
int jsonStrcmp(jsonFile_t* jsonFile, const char* s, unsigned int location)
{
	// make sure we're not trying to read things that don't exist
	if(location >= jsonFile->tokenCount)
		return 0;

	// get the length of the token, the length of the desired match string, and the differences between them
	int tokLength = jsonFile->tokens[location].end - jsonFile->tokens[location].start;
	int strLength = strlen(s);
	int strDiff = strncmp(jsonFile->text + jsonFile->tokens[location].start, s, tokLength);

	// if the token is a string AND the string is the right length AND the string is the right string...
	// return 1.
	if (jsonFile->tokens[location].type == JSMN_STRING
			&& strLength == tokLength
			&& strDiff == 0)

	{
		return 1;
	}
	return 0;
}


float jsonTokToF(jsonFile_t* jsonFile, int tokenIndex)
{
	jsmntok_t token = jsonFile->tokens[tokenIndex];
	int fLen = token.end - token.size;
	char value[fLen + 1];
	strncpy(value, jsonFile->text + token.start, fLen);
	return atof(value);
}
