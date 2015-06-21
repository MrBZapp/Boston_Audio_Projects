/*
 * BAP_math.c
 *
 *  Created on: Apr 23, 2015
 *      Author: Matt
 */

#include "BAP_math.h"

int32_t i_lim(int32_t bottom, int32_t top, int32_t x)
{
	x = (x < bottom) ? bottom : x;
	x = (x > top) ? top : x;
	return x;
}

int32_t i_lscale(int32_t minX, int32_t maxX, int32_t minY, int32_t maxY, int32_t x)
{
	// Initialize the return value
	int32_t y = 0;

	// calculate the change in both axis
	int32_t DeltaX = maxX - minX;
	int32_t DeltaY = maxY - minY;
	int32_t X = x - minX;

	// Block Div0 and Mult0
	if( DeltaX == 0)
	{
		return y;
	}

	// calculate scaled position
	y = minY + ((DeltaY *X) /DeltaX);

	// if negative, return the position in range
	//if (DeltaX < 0 || DeltaY < 0)
	//{
	//	y += (maxY > minY ? maxY : minY);
	//}

	return y;
}

int i_interpolate(int32_t x, int32_t y, float weight)
{
	return (x * weight) + (y * (1.0 - weight));
}

uint32_t LFSR()
{
	static uint32_t lfsr = 0xACE1u;
	uint8_t lsb = lfsr & 1;
	lfsr >>= 1;

	if (lsb == 1)
	{
		/* Only apply toggle mask if output bit is 1. */
		lfsr ^= 0xB400u;
	}
	return lfsr;
}

