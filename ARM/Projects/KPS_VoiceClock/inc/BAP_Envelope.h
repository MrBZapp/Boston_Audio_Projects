/*
 * BAP_Envelope.h
 *
 *  Created on: Apr 11, 2015
 *      Author: Matt
 */

#ifndef BAP_ENVELOPE_H_
#define BAP_ENVELOPE_H_

#include "chip.h"

#define ENVAMP_MAX 127
#define ENVAMP_MIN 0

typedef struct envShape_LinADSR{
	uint32_t atk;
	uint32_t dk;
	uint32_t sus;
	uint32_t rls;
}envLinADSR_t;

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

int32_t GenEnv(env_t* env);

void SetEnvelopeTimes(envLinADSR_t* env, uint32_t atk, uint32_t dk, uint32_t sus, uint32_t rls);

uint32_t GenLinADS(envLinADSR_t* env, uint32_t pos);

uint32_t GenLinRelease (envLinADSR_t* env, uint32_t pos);

#endif /* BAP_ENVELOPE_H_ */
