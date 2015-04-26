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
#include "BAP_Envelope.h"

// Define TLV DAC functions
#define FilterDAC TLC_DAC_1
#define AmpDAC TLC_DAC_2
#define PulseDAC TLC_DAC_4

// Define Address
#define LOCAL_ADDRESS 0x00

// Define Pulse characteristics
#define ATTACK 0
#define DECAY 300
#define SUSTAIN 50
#define RELEASE 10000
#define BIAS 127

// Global Variables
uint8_t ampTable[6];
int8_t activeNote;
uint8_t pluckStrength;

// Global Access to envelope
env_t GlobalEnv;

#endif /* GLOBALDEF_H_ */
