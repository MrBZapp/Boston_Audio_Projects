/*
 * BAP_32Sel.h
 *
 *  Created on: Apr 25, 2015
 *      Author: Matt
 */

#ifndef BAP_32SEL_H_
#define BAP_32SEL_H_

#include "chip.h"

#define SEL_MATCHCNT 20
#define SEL_LOOPMAX 1000

#define SEL_SetupVoltLadder(x) LPC_CMP->LAD = (x << 1) & (ACMP_LADREF_MASK)

uint32_t Selector_GetValue();
void Selector_Init(LPC_CMP_T* ACMP);
uint32_t SelectorGetValue(LPC_CMP_T* ACMP);
#endif
