/*
 * BAP_math.h
 *
 *  Created on: Nov 8, 2014
 *      Author: Matt
 */

#ifndef BAP_MATH_H_
#define BAP_MATH_H_

#include "chip.h"
typedef struct signedIntScaleStruct{
	int32_t minX;
	int32_t minY;
	int32_t maxX;
	int32_t maxY;
} iscalar_t;

int i_interpolate(int32_t x, int32_t y, float weight);
int32_t i_lscale(int32_t minX, int32_t maxX, int32_t minY, int32_t maxY, int32_t x);
int32_t i_lim(int32_t bottom, int32_t top, int32_t x);
uint32_t LFSR();

#endif /* BAP_MATH_H_ */
