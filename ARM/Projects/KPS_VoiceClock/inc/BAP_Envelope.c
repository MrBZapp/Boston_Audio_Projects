/*
 * BAP_Envelope.c
 *
 *  Created on: Apr 11, 2015
 *      Author: Matt
 */

#include "BAP_Envelope.h"
#include "BAP_math.h"

uint32_t GenLinADS(envLinADSR_t* env, uint32_t pos)
{

	// Attack
	if (pos < env->atk){
		return i_lscale(0, env->atk, 0, ENVAMP_MAX, pos);
	}

	// Decay
	else if ((pos <= env->dk) && (pos >= env->atk)){
		return i_lscale(env->atk, env->dk, ENVAMP_MAX, env->sus, pos);
	}

	// Sustain
	return env->sus;
}

uint32_t GenLinRelease (envLinADSR_t* env, uint32_t pos)
{
	return i_lscale(env->dk, env->rls, env->sus, ENVAMP_MIN, pos);
}

void SetEnvelopeTimes(envLinADSR_t* env, uint32_t atk, uint32_t dk, uint32_t sus, uint32_t rls)
{
	env->atk = atk;
	//shift all times to associate with a global time.
	env->dk = dk += atk;
	env->sus = sus;
	env->rls = rls += dk;
}
