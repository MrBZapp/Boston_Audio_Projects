/*
 * KPS_Exciter.c
 *
 *  Created on: Apr 24, 2015
 *      Author: Matt
 */


#include "KPS_Exciter.h"
#include "BAP_math.h"

void GenPluckBow(envLinADSR_t* Envelope, uint32_t Position)
{
	if (Position < Envelope->dk || Envelope->sus != 0)
	{
		int32_t envVal = GenLinADS(Envelope, Position);
		int32_t noise = LFSR() & 0x7F;

		// calculate the strength of the pluck

		// Generate the attack
		//if (Position < Envelope->atk)
		//{
		//	noise = (uint8_t) envVal;
		//}

		// scale the amplitude
		noise = i_lscale(0, 127, 0, pluckStrength, noise);
		noise = i_lscale(0,127, 0, envVal, noise);

		// Set bias to be 1/2 available range
		noise = 127 + noise;
		uint8_t output = noise;
		TLC_SetDACValue(PulseDAC, 1, &output);
	}
}

void GenPluckBow_NoteOff(envLinADSR_t* Envelope, int32_t Position)
{
	if (Position <= (Envelope->rls - Envelope->dk) && Envelope->sus != 0)
	{
		int32_t envVal = GenLinRelease(Envelope, Position + Envelope->dk);
		int32_t noise = LFSR() & 0x7F;

		// scale the amplitude
		noise = i_lscale(0, 127, 0, pluckStrength, noise);
		noise = i_lscale(0,127, 0, envVal, noise);

		// Set bias to be 1/2 available range
		noise = 127 + noise;
		uint8_t output = noise;
		TLC_SetDACValue(PulseDAC, 1, &output);
	}
}
