/*
 * convolution.h
 *
 *  Created on: Nov 24, 2014
 *      Author: mzapp
 */

#ifndef CONVOLUTION_H_
#define CONVOLUTION_H_

#include <math.h>


double sinc(double x);

float normSinc(double x);

float precisionf(double x, long places);

float window(int i);

#endif /* CONVOLUTION_H_ */
