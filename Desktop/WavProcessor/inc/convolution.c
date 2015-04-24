/*
 * convolution.c
 *
 *  Created on: Nov 24, 2014
 *      Author: mzapp
 */

#include "convolution.h"

#define _USE_MATH_DEFINES
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

double normSinc(double x)
{
	if (x == 0)
	{
		return 1;
	}
	// get a normalized sinc function rounded to
	// ten decimal places of precision.
	// trolololol.
	double nsync = sinc(M_PI * x);
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



double window(windowType_t winType, int winSize, float x)
{
	double y = 1;
	switch(winType)
	{
	case(HAMMING):
			y = 0.54 - 0.46 * cos(M_PI * x / winSize);
			break;

	default:
		break;
	}

	return y;
}



//	static inline double
//	calc_output_single (SINC_FILTER *filter, increment_t increment, increment_t start_filter_index)
//	{	double		fraction, left, right, icoeff ;
//		increment_t	filter_index, max_filter_index ;
//		int			data_index, coeff_count, indx ;
//
//		/* Convert input parameters into fixed point. */
//		max_filter_index = int_to_fp (filter->coeff_half_len) ;
//
//		/* First apply the left half of the filter. */
//		filter_index = start_filter_index ;
//		coeff_count = (max_filter_index - filter_index) / increment ;
//		filter_index = filter_index + coeff_count * increment ;
//		data_index = filter->b_current - coeff_count ;
//
//		left = 0.0 ;
//		do
//		{	fraction = fp_to_double (filter_index) ;
//			indx = fp_to_int (filter_index) ;
//
//			icoeff = filter->coeffs [indx] + fraction * (filter->coeffs [indx + 1] - filter->coeffs [indx]) ;
//
//			left += icoeff * filter->buffer [data_index] ;
//
//			filter_index -= increment ;
//			data_index = data_index + 1 ;
//			}
//		while (filter_index >= MAKE_INCREMENT_T (0)) ;
//
//		/* Now apply the right half of the filter. */
//		filter_index = increment - start_filter_index ;
//		coeff_count = (max_filter_index - filter_index) / increment ;
//		filter_index = filter_index + coeff_count * increment ;
//		data_index = filter->b_current + 1 + coeff_count ;
//
//		right = 0.0 ;
//		do
//		{	fraction = fp_to_double (filter_index) ;
//			indx = fp_to_int (filter_index) ;
//
//			icoeff = filter->coeffs [indx] + fraction * (filter->coeffs [indx + 1] - filter->coeffs [indx]) ;
//
//			right += icoeff * filter->buffer [data_index] ;
//
//			filter_index -= increment ;
//			data_index = data_index - 1 ;
//			}
//		while (filter_index > MAKE_INCREMENT_T (0)) ;
//
//		return (left + right) ;
//	}
