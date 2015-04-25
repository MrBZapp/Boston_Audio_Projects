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


void EnvInit(env_t* env, envNode_t* nodes, uint16_t size)
{
	env->node = nodes;
	env->index = 0;
	env->position = 0;
	env->size = size;
	env->value = 0;
}


int32_t GenEnv(env_t* env)
{
	// get node
	envNode_t node = env->node[env->index];

	// get the length of the current node
	uint32_t len = node.length;

	// if we're holding... well... hold.
	if (env->position > len && node.hold)
	{
		env->position = len;
		return node.level;
	}

	// if the position falls outside the node
	if ((env->position > len))
	{
		// shorten the length
		env->position -= len;

		// advance the node
		env->index++;

		// and try again
		return GenEnv(env);
	}


	// scale between the current value and the desired result
	int32_t value = i_lscale(0, len, env->value, node.level, env->position);

	// advance the position
	env->position++;

	// return the value
	return value;
}
