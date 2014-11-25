/*
 * convolution.h
 *
 *  Created on: Nov 24, 2014
 *      Author: mzapp
 */

#ifndef CONVOLUTION_H_
#define CONVOLUTION_H_

#include <math.h>

#define INTERP_WINDOW 9

double sinc(double x);

double normSinc(double x);

float window(int i);

#endif /* CONVOLUTION_H_ */
