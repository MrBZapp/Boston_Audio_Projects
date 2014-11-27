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
#include "../inc/wavProcesses.h"
#include "../inc/lfo.h"
#include "../inc/convolution.h"




int main(int argc, char* argv[]) {

	// HACK WARNING
    char* infile = argv[1];

    // make an empty PCM wave file
    wavFilePCM_t wavFile;

    // open input file
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    int error = wavDecode_PCM(&wavFile, inptr);

    // any error at this point should fail the entire process.
    if (wavError(error)){
    	fclose(inptr);
    	return 3;
    }

    // we don't need the input file anymore, so we can close it.
    fclose(inptr);

    FILE* outptr = fopen("outfile.wav", "w");
    if (outptr == NULL)
    {
        printf("Could not open %s.\n", "outfile.wav");
        fclose(inptr);
        fclose(outptr);
        free(wavFile.data);
        return 2;
    }

//    fileReverse(&wavFile);
//    fileTremolo(&wavFile, RAMP_DOWN, 4 , .5);
//    fileRing(&wavFile, TRI, 2000, 1);
//    fileEcho(&wavFile, 40000, 0.5);
//    filePitch(&wavFile, .5);
    fileVibrato(&wavFile, TRI, 1, 0.5);
//    fileDelay(&wavFile, 100);
//    fileGain(&wavFile, .2);


//    // Print data to wav
//    for (int i = 0; i < wavGetSampCount(&wavFile); i ++)
//    {

//    	float freq = (i * wavFile.FormatChunk.SampleRate) / wavGetSampCount(&wavFile);
//    	float phase = i;
//    	wavFile.data[i].left = wavFile.data[i].right = 4000 * sinc(M_PI *.5);//((lfoGetValue(SINE, freq , wavFile.FormatChunk.SampleRate , phase)));
//    }

	wavEncode_PCM(&wavFile, outptr);

    free(wavFile.data);
    fclose(outptr);

    return 0;
}
