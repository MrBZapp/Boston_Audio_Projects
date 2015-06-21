/*
 * BAP_MAX1164x_ADC.c
 *
 *  Created on: Jun 13, 2015
 *      Author: Matt
 */

#include "chip.h"
#include "BAP_I2C.h"
#include "BAP_MAX1164x_ADC.h"

/** 7-bit I2C address */
#define MAX1164x_I2C_ADDR_7BIT		0x36

////////////////////// Static Functions /////////////////////////////////

static void MAX1164x_WriteCmd(MAX1164x_t* MAX1164x, uint8_t cmd)
{
	// Maximum CMD is a two-byte write cycle following the 1 byte address & R/W indicator
	uint8_t SendData[3];
	int i = 0;

	SendData[i++] = (uint8_t) (MAX1164x_I2C_ADDR_7BIT << 1);
	SendData[i++] = cmd;

	/* Setup I2C parameters:
	   Start - address7 & W bit - ack
	   Setup Byte - ack
	   Config Byte - ack - stop */

	I2C_PARAM_T param;

	param.num_bytes_send    = i;
	param.buffer_ptr_send   = &SendData[0];
	param.num_bytes_rec     = 0;
	param.stop_flag         = MAX1164x->HiSpeed;

	// Result for sake of keeping the API happy.
	I2C_RESULT_T result;

	LPC_I2CD_API->i2c_set_timeout(MAX1164x->i2cHandleMaster, 10000);
	LPC_I2CD_API->i2c_master_transmit_poll(MAX1164x->i2cHandleMaster, &param, &result);
}


////////////////////////// Public Functions ///////////////////////////////////

void MAX1164x_UpdateConfig(MAX1164x_t* MAX1164x)
{
	MAX1164x_WriteCmd(MAX1164x, MAX1164x->ConfigReg.Register);
}


void MAX1164x_UpdateSetup(MAX1164x_t* MAX1164x)
{
	MAX1164x_WriteCmd(MAX1164x, MAX1164x->SetupReg.Register);
}

void MAX1164x_SetChannel(MAX1164x_t* MAX1164x, bool chnl)
{
	MAX1164x->SetupReg.BitField.clk = chnl;
	MAX1164x_UpdateSetup(MAX1164x);
}

/***
 * Initialize the MAX1164x interface
 * Params: Speed, SDA pin, SCL pin
 */
void MAX1164x_Init(MAX1164x_t* MAX1164x, CHIP_PINx_T sda, CHIP_PINx_T scl)
{
	// Initialize the peripheral
	Chip_I2C_Init();

	// Allow for 400kHz+ bit rate
	Chip_IOCON_PinSetI2CMode(LPC_IOCON, sda, PIN_I2CMODE_FASTPLUS);
	Chip_IOCON_PinSetI2CMode(LPC_IOCON, scl, PIN_I2CMODE_FASTPLUS);

	// Initialize register models
	MAX1164x->SetupReg.Register = MAX1164x_SETUP_DEFAULT;
	MAX1164x->ConfigReg.Register = MAX1164x_CONFIG_DEFAULT;

	// Initialize Speed bool to false
	MAX1164x->HiSpeed = FALSE;

	// Setup the I2C API handle
	MAX1164x->i2cHandleMaster = LPC_I2CD_API->i2c_setup(LPC_I2C_BASE, MAX1164x->i2cMasterHandleMEM);

	if (MAX1164x->i2cHandleMaster == NULL)
	{
		// If we get here, problems.  We should probably address this
		while(1){};
	}

	// Set I2C bitrate to normal "fast" mode
	if (LPC_I2CD_API->i2c_set_bitrate( MAX1164x->i2cHandleMaster, Chip_Clock_GetSystemClockRate(), MAX1164x_FAST_BITRATE) != LPC_OK)
	{
		// If we get here, problems.  We should probably address this
		while(1){};
	}

	// Setup for use with external clock
	MAX1164x->SetupReg.BitField.clk = MAX1164x_EXT_CLK;
	MAX1164x_UpdateSetup(MAX1164x);
}


void MAX1164x_SetSpeed(MAX1164x_t* MAX1164x, bool spd)
{
	// Set the bool
	MAX1164x->HiSpeed = spd;

	// Switch the clock to external if high-speed is requested
	// And speed up the bit i2C bit rate
	if (MAX1164x->HiSpeed == MAX1164x_SPD_HI)
	{
		I2C_InitiateHighSpeedMode(MAX1164x->i2cHandleMaster);

		// Set I2C bitrate to normal "High Speed" mode
		if (LPC_I2CD_API->i2c_set_bitrate( MAX1164x->i2cHandleMaster,
										   Chip_Clock_GetSystemClockRate(),
										   MAX1164x_HIGHSPEED_BITRATE) != LPC_OK )
		{
			// If we get here, it means the bus can't move that quickly. Back it off a bit.
			LPC_I2CD_API->i2c_set_bitrate(MAX1164x->i2cHandleMaster, Chip_Clock_GetSystemClockRate(), MAX1164x_FAST_BITRATE);
		}

	}
	else
	{
		// If we get here, it means the bus can't move that quickly. Back it off a bit.
		LPC_I2CD_API->i2c_set_bitrate(MAX1164x->i2cHandleMaster, Chip_Clock_GetSystemClockRate(), MAX1164x_FAST_BITRATE);
	}
}



uint16_t MAX1164x_Get_Sample(MAX1164x_t* MAX1164x)
{
	uint8_t recvData[4];
	I2C_PARAM_T param;
	I2C_RESULT_T result;
	int index = 0;

	recvData[index++] = (uint8_t) (MAX1164x_I2C_ADDR_7BIT << 1);

	param.num_bytes_send    = 0;
	param.num_bytes_rec     = 3;
	param.buffer_ptr_rec    = &recvData[0];
	param.stop_flag = MAX1164x->HiSpeed;

	/* Set timeout (much) greater than the transfer length */
	LPC_I2CD_API->i2c_set_timeout(MAX1164x->i2cHandleMaster, 100000);

	/* Do master read transfer */
	LPC_I2CD_API->i2c_master_receive_poll(MAX1164x->i2cHandleMaster, &param, &result);

	// Format the data for uint16_t
	return (recvData[1] << 8) | recvData[2];
}
