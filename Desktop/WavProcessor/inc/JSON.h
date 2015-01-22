/*
 * JSON.h
 *
 *  Created on: Nov 30, 2014
 *      Author: mzapp
 */

#ifndef JSON_H_
#define JSON_H_

#include "jsmn/jsmn.h"

// Structure of a completed JSON file
typedef struct jsonFile
{
	char* text;
	jsmntok_t* tokens;
	int tokenCount;
}jsonFile_t;

/**
 * reads a file path into a JSON structure and returns
 * a pointer to that structure
 ***/
jsonFile_t* openJSONFile(const char* JSONPath);

/**
 * comprises all the steps required to close a JSON file down.
 ***/
void closeJSONFile(jsonFile_t* file);



float jsonTokToF(jsonFile_t* jsonFile, int tokenIndex);



/**
 * compares a token against a string.  returns 1 if match, 0 if no match
 */
int jsonStrcmp(jsonFile_t* jsonFile, const char* s, unsigned int location);

#endif /* JSON_H_ */
