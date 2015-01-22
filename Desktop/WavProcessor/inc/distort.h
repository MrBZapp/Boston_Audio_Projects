/*
 * distort.h
 *
 *  Created on: Dec 3, 2014
 *      Author: mzapp
 */

#ifndef DISTORT_H_
#define DISTORT_H_

typedef enum distType
{
	SOFT,
	HARD
}distType_t;

typedef double (*distortion_pt)(float, float);

double distSoft(float samp, float knee);

double distHard(float samp, float knee);

#endif /* DISTORT_H_ */
