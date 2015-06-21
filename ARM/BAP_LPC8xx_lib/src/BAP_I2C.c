/*
 * BAP_I2C.c
 *
 *  Created on: Jun 17, 2015
 *      Author: Matt
 */

#include "BAP_I2C.h"

/***
 * Remember to set the pins to operate in high-speed mode if using this function
 */
ErrorCode_t I2C_InitiateHighSpeedMode(I2C_HANDLE_T I2CHandle)
{
	uint8_t HighSpeed = I2C_MASTERCMD_HIGHSPEED;
	I2C_PARAM_T param;
	I2C_RESULT_T result;

	param.num_bytes_send    = 1;
	param.buffer_ptr_send   = &HighSpeed;
	param.num_bytes_rec     = 0;
	param.stop_flag         = 0; // No stop flags after going into high-speed mode!

	LPC_I2CD_API->i2c_set_timeout(I2CHandle, 1000);

	// Write the message, return the errors.
	return  LPC_I2CD_API->i2c_master_transmit_poll(I2CHandle, &param, &result);
}
