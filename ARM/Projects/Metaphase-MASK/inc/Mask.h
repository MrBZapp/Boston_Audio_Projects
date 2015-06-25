/*
 * Mask.h
 *
 *  Created on: Jun 20, 2015
 *      Author: Matt
 */

#ifndef MASK_H_
#define MASK_H_

#include "BAP_32Sel.h"
#include "BAP_math.h"

#define LOGIC_TYPES 12
#define INV(x) (x ^ 0xFFFF)

typedef enum {
	MSK_BYPASS,
	MSK_NONE_A,
	MSK_NONE_B,
	MSK_NOT_A,
	MSK_NOT_B,
	MSK_AND,
	MSK_OR,
	MSK_XOR,
	MSK_AND_INVA,
	MSK_OR_INVA,
	MSK_XOR_INVA,
	MSK_AND_INVB,
	MSK_OR_INVB,
	MSK_XOR_INVB,
} MSK_LOGIC_TYPES;


uint16_t Mask_Values(uint16_t A, uint16_t B)
{
	uint32_t mask_type = SelectorGetValue(LPC_CMP);
	uint32_t out = 0;
	mask_type = i_lscale(0, 31, 0, LOGIC_TYPES, mask_type);

	switch (mask_type){
	case(MSK_NONE_A):
			out = A;
	break;

	case(MSK_NONE_B):
			out = B;
	break;

	case(MSK_NOT_A):
			out = INV(A);
	break;

	case(MSK_NOT_B):
			out = INV(B);
	break;

	case(MSK_AND):
			out = A & B;
	break;

	case(MSK_OR):
			out = A | B;
	break;

	case(MSK_XOR):
			out = A ^ B;
	break;

	case(MSK_AND_INVA):
			out = INV(A) & B;
	break;

	case(MSK_OR_INVA):
			out = INV(A) | B;
	break;

	case(MSK_XOR_INVA):
			out = INV(A) ^ B;
	break;

	case(MSK_AND_INVB):
			out = A & INV(B);
	break;

	case(MSK_OR_INVB):
			out = A | INV(B);
	break;

	case(MSK_XOR_INVB):
			out = A ^ INV(B);
	break;
	}

	return out;
}


#endif /* MASK_H_ */
