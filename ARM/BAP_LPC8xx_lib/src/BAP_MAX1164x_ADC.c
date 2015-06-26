/*
 * BAP_MAX1164x_ADC.c
 *
 *  Created on: Jun 13, 2015
 *      Author: Matt
 */
#include <String.h>
#include "chip.h"
#include "BAP_I2C.h"
#include "BAP_MAX1164x_ADC.h"

/** 7-bit I2C address */
#define MAX1164x_I2C_ADDR_7BIT		0x36

///////////////////// Global Variables /////////////////////////


/////////////////// Static Global Variables //////////////////////////////
static I2C_HANDLE_T* localHandle;
////////////////////// Static Functions /////////////////////////////////

static void MAX1164x_WriteCmd(MAX1164x_t* MAX1164x, uint8_t cmd)
{
	// Maximum CMD is a two-byte write cycle following the 1 byte address & R/W indicator
	uint8_t SendData[2];
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
	param.stop_flag         = MAX1164x->UseStopFlag;

	// Result for sake of keeping the API happy.
	I2C_RESULT_T result;

	LPC_I2CD_API->i2c_set_timeout(MAX1164x->i2cHandleMaster, 10000);
	LPC_I2CD_API->i2c_master_transmit_poll(MAX1164x->i2cHandleMaster, &param, &result);
}

void MAX1164x_CBFunc(uint32_t err, uint32_t n)
{
	sample_count++;
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
	MAX1164x->ConfigReg.BitField.chan_sel = chnl;
	MAX1164x_UpdateConfig(MAX1164x);
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

	// Initialize the use stop flag bool
	MAX1164x->UseStopFlag = TRUE;

	// Setup the I2C API handle
	localHandle = MAX1164x->i2cHandleMaster = LPC_I2CD_API->i2c_setup(LPC_I2C_BASE, MAX1164x->i2cMasterHandleMEM);

	sample_count = 0;

	if (MAX1164x->i2cHandleMaster == NULL)
	{
		// If we get here, problems.  We should probably address this
		while(1){};
	}

	// Set I2C bitrate to normal "fast" mode
	if (LPC_I2CD_API->i2c_set_bitrate(localHandle, Chip_Clock_GetSystemClockRate(), MAX1164x_FAST_BITRATE) != LPC_OK)
	{
		// If we get here, problems.  We should probably address this
		while(1){};
	}

	// Setup for use with external clock for max speed
	MAX1164x->SetupReg.BitField.clk = MAX1164x_EXT_CLK;

	// Set scan bits to scan up from in 0
	MAX1164x->ConfigReg.BitField.scan = MAX1164x_SCANFROMIN0;
	// Set scan to catch both inputs
	MAX1164x->ConfigReg.BitField.chan_sel = MAX1164x_CHAN_B;

	MAX1164x_UpdateSetup(MAX1164x);
	MAX1164x_UpdateConfig(MAX1164x);

	interruptErrorFlag = LPC_OK;

	NVIC_EnableIRQ(I2C_IRQn);
}


void MAX1164x_SetSpeed(MAX1164x_t* MAX1164x, bool spd)
{
	// Switch the clock to external if high-speed is requested
	// And speed up the bit i2C bit rate
	if (spd == MAX1164x_SPD_HI)
	{
		// Set the UseStopFlag bool
		MAX1164x->UseStopFlag = FALSE;

		BAP_I2C_InitiateHighSpeedMode(MAX1164x->i2cHandleMaster);

		// Set I2C bitrate to normal "High Speed" mode
		if (LPC_I2CD_API->i2c_set_bitrate( localHandle,
										   Chip_Clock_GetSystemClockRate(),
										   MAX1164x_HIGHSPEED_BITRATE) != LPC_OK )
		{
			// If we get here, it means the bus can't move that quickly. Back it off a bit.
			LPC_I2CD_API->i2c_set_bitrate(localHandle, Chip_Clock_GetSystemClockRate(), MAX1164x_FAST_BITRATE);
		}

	}
	else
	{
		// If we get here, it means the bus can't move that quickly. Back it off a bit.
		LPC_I2CD_API->i2c_set_bitrate(localHandle, Chip_Clock_GetSystemClockRate(), MAX1164x_FAST_BITRATE);
		MAX1164x->UseStopFlag = TRUE;
	}
}



ErrorCode_t MAX1164x_RequestNewSample(MAX1164x_t* MAX1164x)
{
	I2C_PARAM_T param;
	I2C_RESULT_T result;

	MAX1164x->sample_buffer.buffer[0] = (MAX1164x_I2C_ADDR_7BIT << 1);

	param.num_bytes_send    = 0;
	param.num_bytes_rec     = MAX1164x_SAMPBYTECNT;
	param.buffer_ptr_rec = &MAX1164x->sample_buffer.buffer[0];
	param.stop_flag = MAX1164x->UseStopFlag;
	param.func_pt = MAX1164x_CBFunc;

	/* Set timeout (much) greater than the transfer length */
	LPC_I2CD_API->i2c_set_timeout(localHandle, 100000);

	interruptErrorFlag = -1;

	/* Do master read transfer */
	int error =  LPC_I2CD_API->i2c_master_receive_intr(localHandle, &param, &result);

	/* Sleep until transfer is complete, but allow IRQ to wake system
	   to handle I2C IRQ */
///	while (interruptErrorFlag == -1) {
///		__WFI();
///	}

	return error;
}

