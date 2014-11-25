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

	return sin(x)/x;
}

double normSinc(double x)
{
	return sinc(M_PI * x);
}


float window(int i)
{
	float y = 1;
	return y;
}
