/*
 * convolution.h
 *
 *  Created on: Nov 24, 2014
 *      Author: mzapp
 */

#ifndef CONVOLUTION_H_
#define CONVOLUTION_H_

#define _USE_MATH_DEFINES
#include <math.h>



// Window types.
typedef enum windowType {
	RECTANGLE,
	HAMMING
} windowType_t;

double sinc(double x);

double normSinc(double x);

float precisionf(double x, long places);

double window(windowType_t winType, int winSize, float x);

#endif /* CONVOLUTION_H_ */
