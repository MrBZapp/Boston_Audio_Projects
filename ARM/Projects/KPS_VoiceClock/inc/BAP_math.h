/*
 * BAP_math.h
 *
 *  Created on: Nov 8, 2014
 *      Author: Matt
 */

#ifndef BAP_MATH_H_
#define BAP_MATH_H_

typedef struct signedIntScaleStruct{
	int32_t minX;
	int32_t minY;
	int32_t maxX;
	int32_t maxY;
} iscalar_t;

int32_t i_lscale(int32_t minX, int32_t maxX, int32_t minY, int32_t maxY, int32_t x)
{
	// Initialize the return value
	int32_t y = 0;

	// calculate the change in both axis
	int32_t DeltaX = maxX - minX;
	int32_t DeltaY = maxY - minY;

	// Block Div0 and Mult0
	if( DeltaX == 0)
	{
		return 0;
	}

	// calculate scaled position
	y = (DeltaY * x) / (DeltaX);

	// if negative, return the position in range
	if (DeltaX < 0 || DeltaY < 0)
	{
		y += (maxY > minY ? maxY : minY);
	}

	return y;
}

#endif /* BAP_MATH_H_ */
