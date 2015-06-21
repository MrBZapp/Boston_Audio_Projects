/*
 * BAP_Envelope.h
 *
 *  Created on: Apr 11, 2015
 *      Author: Matt
 */

#ifndef BAP_ENVELOPE_H_
#define BAP_ENVELOPE_H_

#include "chip.h"

#define ENVAMP_MAX INT16_MAX
#define ENVAMP_MIN INT16_MIN


typedef struct{
	uint32_t length;
	uint32_t level;
	bool hold;
}envNode_t;

typedef struct{
	envNode_t* node;
	int32_t value;
	uint32_t position;
	uint16_t index;
	uint16_t size;
}env_t;

void EnvInit(env_t* env, envNode_t* nodes, uint16_t size);

void EnvReset(env_t* env);

int32_t EnvGenSample(env_t* env);

void EnvNextNode(env_t* env);

void EnvRelease(env_t* env);


#endif
