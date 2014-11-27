/*
 * convolution.c
 *
 *  Created on: Nov 24, 2014
 *      Author: mzapp
 */

#include "convolution.h"
#include <math.h>
#include <stdio.h>

double sinc(double x)
{
	if (x == 0)
	{
		return 1;
	}

	double sine = sin(x);
	double csine = sine/x;
	return csine;
}

float normSinc(double x)
{
	if (x == 0)
	{
		return 1;
	}
	// get a normalized sinc function rounded to
	// ten decimal places of precision.
	// trolololol.
	float nsync = precisionf( sinc(M_PI * x), 10);
	return nsync;
}


float precisionf(double x, long places)
{
	// find the multipule for places
	places = pow(10, places);
	long i = x * places;
	float y = ((float)i) / places;
	return y;
}


float window(int i)
{
	float y = 1;
	return y;
}
