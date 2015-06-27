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

static MAX1164x_t* local_ADC;

////////////////////// Static Functions /////////////////////////////////
static void MAX1164x_WriteCmd(uint8_t cmd)
{
	BAP_I2C_MsgParam_t param;
	param.addr.sevenbit = MAX1164x_I2C_ADDR_7BIT;
	param.tenbit = FALSE;

	// transmitting the command and nothing else.
	param.txcnt = 1;
	param.rxcnt = 0;

	// Don't do anything after the transfer
	param.callback = NULL;

	BAP_I2C_SetTimeout(10000);
	BAP_I2C_MasterProcessNewMessage(&param, &cmd);
}


// The callback function for the MAX1164x
void MAX1164x_CBFunc(uint32_t err, uint32_t n)
{
	memcpy(local_ADC->sample_buffer.buffer, local_ADC->rx_buffer, MAX1164x_SAMPBYTECNT);
}


////////////////////////// Public Functions ///////////////////////////////////
void MAX1164x_UpdateConfig(MAX1164x_t* MAX1164x)
{
	MAX1164x_WriteCmd(MAX1164x->config.Register);
}


void MAX1164x_UpdateSetup(MAX1164x_t* MAX1164x)
{
	MAX1164x_WriteCmd(MAX1164x->setup.Register);
}


void MAX1164x_SetChannel(MAX1164x_t* MAX1164x, bool chnl)
{
	MAX1164x->config.BitField.chan_sel = chnl;
	MAX1164x_UpdateConfig(MAX1164x);
}


/***
 * Initialize the MAX1164x interface
 * Params: Speed, SDA pin, SCL pin
 */
void MAX1164x_Init(MAX1164x_t* MAX1164x)
{
	// Initialize register models
	MAX1164x->setup.Register = MAX1164x_SETUP_DEFAULT;
	MAX1164x->config.Register = MAX1164x_CONFIG_DEFAULT;

	BAP_I2C_SetBaud(MAX1164x_FAST_BITRATE);

	// Setup for use with external clock for max speed
	MAX1164x->setup.BitField.clk = MAX1164x_EXT_CLK;

	// Set scan bits to scan up from in 0
	MAX1164x->config.BitField.scan = MAX1164x_SCANFROMIN0;
	// Set scan to catch both inputs
	MAX1164x->config.BitField.chan_sel = MAX1164x_CHAN_B;

	MAX1164x_UpdateSetup(MAX1164x);
	MAX1164x_UpdateConfig(MAX1164x);
}


//Attaches the ADC to the I2C bus to allow for read/write
void MAX1164x_AttachToI2C(MAX1164x_t* ADC, BAP_I2C_Bus_t* I2C)
{
	// Set the I2C bus to master mode
	BAP_I2C_SetAsMaster(I2C);

	// Attach the buffers to the I2C pointers and set the max buffer size
	I2C->master_rx_data = ADC->rx_buffer;
	I2C->master_tx_data = ADC->tx_buffer;
	I2C->mem_size = MAX1164x_MEMSIZE;
}


void MAX1164x_SetSpeed(bool spd)
{
	// Switch the clock to external if high-speed is requested
	// And speed up the bit i2C bit rate
	if (spd == MAX1164x_SPD_HI)
	{
		BAP_I2C_InitiateHighSpeedMode();
		LPC_GPIO_PORT->CLR[0] = 1 << 14;
		// Set I2C bitrate to "High Speed" mode
		BAP_I2C_SetBaud(MAX1164x_HIGHSPEED_BITRATE);
	}
	else
	{
		BAP_I2C_DisableHighSpeedMode();
		BAP_I2C_SetBaud(MAX1164x_FAST_BITRATE);
	}
}



ErrorCode_t MAX1164x_RequestNewSample(MAX1164x_t* MAX1164x)
{
	BAP_I2C_MsgParam_t param;

	// fill out the parameter form
	param.addr.sevenbit = MAX1164x_I2C_ADDR_7BIT;
	param.txcnt = 0;
	param.rxcnt = MAX1164x_SAMPBYTECNT;
	param.callback = MAX1164x_CBFunc;

	/* Set timeout (much) greater than the transfer length */
	BAP_I2C_SetTimeout(10000);

	/* Do master read transfer */
	int error =  BAP_I2C_MasterProcessNewMessage(&param, NULL);
	return error;
}

