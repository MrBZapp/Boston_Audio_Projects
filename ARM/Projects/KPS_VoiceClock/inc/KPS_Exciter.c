/*
 * KPS_Exciter.c
 *
 *  Created on: Apr 24, 2015
 *      Author: Matt
 */
#include "KPS_Exciter.h"
#include "BAP_math.h"

uint8_t GenExciter(env_t* env)
{
	// Gen the envelope value
	int32_t envVal = EnvGenSample(env);

	// Get a bit of noise
	int16_t noise = LFSR();

	// Generate the attack
	if (env->index == 0)
	{
		noise = envVal;
	}

	// scale the amplitude
	noise = i_lscale(0, ENVAMP_MAX, 0, envVal, noise);
	noise = i_lscale(0, ENVAMP_MAX, 0, pluckStrength, noise);

	// Set bias to be 1/2 available range
	noise = 127 + noise;
	return (uint8_t) noise;
}
