/*
 * GlobalDEF.c
 *
 *  Created on: Apr 24, 2015
 *      Author: Matt
 */

#include "GlobalDEF.h"

uint8_t ampTable[6] = {130, 130, 148, 169, 188, 211};
int8_t activeNote = -1;
uint8_t pluckStrength = 0;
int32_t EnvPosition = 0;
