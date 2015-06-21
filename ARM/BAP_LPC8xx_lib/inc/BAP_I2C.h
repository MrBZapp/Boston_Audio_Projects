/*
 * BAP_I2C.h
 *
 *  Created on: Jun 17, 2015
 *      Author: Matt
 */

#ifndef BAP_I2C_H_
#define BAP_I2C_H_

#include "chip.h"

#define I2C_MASTERCMD_HIGHSPEED 0b00001111

ErrorCode_t I2C_InitiateHighSpeedMode(I2C_HANDLE_T I2CHandle);

#endif /* BAP_I2C_H_ */
