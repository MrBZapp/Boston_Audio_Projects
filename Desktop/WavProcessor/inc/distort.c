/*
 * distort.c
 *
 *  Created on: Dec 3, 2014
 *      Author: mzapp
 */

#include <math.h>

/**
 * Soft clip lowers the gain of samples
 * above the knee, but does not remove them entirely
 ***/
double distSoft(float samp, float knee)
{
	if (samp > knee)
	{
		samp = (samp + knee) / 2;
	}
	else if (samp < -knee)
	{
		samp = (samp - knee) / 2 ;
	}
	return samp;
}


/**
 * Hard clip completely removes data above the knee
 ***/
double distHard(float samp, float knee)
{
	if (samp > knee)
	{
		samp -= knee - samp;
	}
	else if (samp < -knee)
	{
		samp -= -knee - samp;
	}
	return samp;
}
