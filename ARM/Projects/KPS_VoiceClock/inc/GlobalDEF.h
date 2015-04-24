/*
 * GlobalDEF.h
 *
 *  Created on: Apr 24, 2015
 *      Author: Matt
 */

#ifndef GLOBALDEF_H_
#define GLOBALDEF_H_

#include "chip.h"
#include "BAP_TLC_DAC.h"

// Define TLV DAC functions
#define FilterDAC TLC_DAC_1
#define AmpDAC TLC_DAC_2
#define PulseDAC TLC_DAC_4

// Define Address
#define LOCAL_ADDRESS 0x00

// Global Variables
uint8_t ampTable[6];
int8_t activeNote;
uint8_t pluckStrength;
int32_t EnvPosition;

#endif /* GLOBALDEF_H_ */
