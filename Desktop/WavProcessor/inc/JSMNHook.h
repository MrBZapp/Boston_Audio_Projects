/**
 * JSMNHook.h
 *
 *  Created on: Nov 27, 2014
 *      Author: mzapp
 *  This file exists solely to keep the sausage guts that are
 *  JSMN and JSON parsing in general out of main().
 ***/

#ifndef JSMNHOOK_H_
#define JSMNHOOK_H_

/**
 * Takes a wave file and a file path as an argument, parses the JSON
 * and passes the commands on to processing. will return errors if
 * they occur during parsing or processing.
 ***/
int processWavFromJSONString(wavFilePCM_t* wavFile, const char* JSONPath);

#endif /* JSMNHOOK_H_ */
