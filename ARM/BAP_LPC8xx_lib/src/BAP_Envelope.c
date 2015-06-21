/*
 * BAP_Envelope.c
 *
 *  Created on: Apr 11, 2015
 *      Author: Matt
 */

#include "BAP_Envelope.h"
#include "BAP_math.h"

void EnvInit(env_t* env, envNode_t* nodes, uint16_t size)
{
	env->node = nodes;
	env->size = size;
	EnvReset(env);
}


int32_t EnvGenSample(env_t* env)
{
	// get node
	envNode_t node = env->node[env->index];

	// if we're holding... well... hold.
	if (env->position > node.length && node.hold)
	{
		env->position = node.length;
		env->value = node.level;
		return node.level;
	}

	// if the position falls outside the node
	if ((env->position > node.length))
	{
		// shorten the length
		env->position -= node.length;

		// Remember the value we should be at
		env->value = node.level;

		// advance the node
		env->index++;

		// and try again
		return EnvGenSample(env);
	}

	// scale between the current value and the desired result
	int32_t value = i_lscale(0, node.length, env->value, node.level, env->position);

	// advance the position
	env->position++;

	// return the value
	return value;
}


void EnvNextNode(env_t* env)
{
	// Advance the index
	env->index++;

	// Keep the index within bounds
	env->index %= env->size;

	// reset the read position
	env->position = 0;
}


void EnvRelease(env_t* env)
{
	// Calculate where we're from what value we will be releasing
	env->value = EnvGenSample(env);
	// Jump to the last node
	env->index = env->size - 1;
	env->position = 0;
}


void EnvReset(env_t* env)
{
	env->index = 0;
	env->position = 0;
	env->value = 0;
}

