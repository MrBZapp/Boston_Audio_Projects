/*
 * KPS_Exciter.h
 *
 *  Created on: Apr 24, 2015
 *      Author: Matt
 */

#ifndef KPS_EXCITER_H_
#define KPS_EXCITER_H_

#include "chip.h"
#include "GlobalDEF.h"
#include "BAP_Envelope.h"

void GenPluckBow(envLinADSR_t* Envelope, uint32_t Position);

void GenPluckBow_NoteOff(envLinADSR_t* Envelope, int32_t Position);

#endif /* KPS_EXCITER_H_ */
