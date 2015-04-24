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

void SetEnvelopeTimes(envLinADSR_t* env, uint32_t atk, uint32_t dk, uint32_t sus, uint32_t rls);

uint32_t GenLinADS(envLinADSR_t* env, uint32_t pos);

uint32_t GenLinRelease (envLinADSR_t* env, uint32_t pos);

#endif /* BAP_ENVELOPE_H_ */
