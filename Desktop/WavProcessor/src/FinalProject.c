/*
 ============================================================================
 Name        : FinalProject.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../inc/wavFile.h"
#include "../inc/lfo.h"
#include "../inc/JSMNHook.h"

//    fileReverse(&wavFile);
//    fileTremolo(&wavFile, TRI, 4 , 1);
//    fileRing(&wavFile, TRI, 2000, 1);
//    filePitch(&wavFile, .5);
//    fileVibrato(&wavFile, TRI, 1, 0.5);
//    fileDelay(&wavFile, 100);
//    fileGain(&wavFile, .2);



int main(int argc, char* argv[]) {

	// block gooberish behaviour.
	if (argc != 4)
	{
		printf("Usage: ./WavProcessor <file name input> <file name output> <process list JSON> ");
		return 1;
	}

	// grab the arguments
	const char* infile = argv[1];
    const char* outfile = argv[2];
    const char* JSONPath = argv[3];

    // open input file
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    // open the output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        printf("Could not open %s.\n", "outfile.wav");
        fclose(inptr);
        return 2;
    }

    // make an empty PCM wave file
    wavFilePCM_t wavFile;

    // decode the input file into memory
    int error = wavDecode_PCM(&wavFile, inptr);

    // we don't need the input file anymore, so we can close it.
    fclose(inptr);

    // any error at this point should fail the entire process.
    if (wavError(error)){
    	return 3;
    }

    // parse the json file and process the audio
    error = processWavFromJSONString(&wavFile, JSONPath);

    // encode the wav file to the output file.
	error = wavEncode_PCM(&wavFile, outptr);

	// clean up the final open pointers and return success!
    printf("Processing completed successfully!\n");
    free(wavFile.data);
    fclose(outptr);
    return 0;
}
