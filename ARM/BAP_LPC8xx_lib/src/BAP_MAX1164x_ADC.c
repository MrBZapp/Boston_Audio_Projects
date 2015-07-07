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
#define MAX1164x_I2C_ADDR_7BIT		(0x36)
#define MAX1164x_CLK_DIVIDER         (40)

static MAX1164x_t* local_ADC;

// The callback function for the MAX1164x
void MAX1164x_CBFunc(uint32_t err, uint32_t n)
{
	for (uint8_t i = 0; i < MAX1164x_SAMPBYTECNT; i++)
	{
		local_ADC->sample_buffer.buffer[i] = local_ADC->rx_buffer[i];
	}
}


////////////////////////// Public Functions ///////////////////////////////////
void MAX1164x_SetChannel(MAX1164x_t* MAX1164x, bool chnl)
{
	MAX1164x->config.BitField.chan_sel = chnl;
	MAX1164x_UpdateConfig(MAX1164x);
}


/***
 * Initialize the MAX1164x model only. Attach to the I2C line to initilize the interface
 */
void MAX1164x_InitChip(MAX1164x_t* MAX1164x)
{
	// Initialize register models
	MAX1164x->setup.Register = MAX1164x_SETUP_DEFAULT;
	MAX1164x->config.Register = MAX1164x_CONFIG_DEFAULT;

	// Setup for use with external clock for max speed
	MAX1164x->setup.BitField.clk = MAX1164x_EXT_CLK;

	// Set scan bits to scan up from in 0
	MAX1164x->config.BitField.scan = MAX1164x_SCANFROMIN0;
	// Set scan to catch both inputs
	MAX1164x->config.BitField.chan_sel = MAX1164x_CHAN_B;

	// use the stop flag
	MAX1164x->stop = 1;

	// initialize the local dac
	local_ADC = MAX1164x;

	// Write the setup and config registers
	BAP_I2C_Frame_t frame;

	// Transmit address as a write-bit
	frame.txdata = (MAX1164x_I2C_ADDR_7BIT << 1);
	frame.rxcnt = 0;
	frame.callback = 0;
	frame.msgst = 1;
	frame.start = 1;
	frame.stop = 0;
	frame.ack_nack_ = 1;
	BAP_I2C_AddFrame(&frame);

	//we're no longer starting
	frame.start = 0;

	// first tx byte is the setup register
	frame.txdata = MAX1164x->setup.Register;
	BAP_I2C_AddFrame(&frame);

	// second tx byte is the configuration register. issue a stop after this.
	frame.txdata = MAX1164x->config.Register;
	frame.stop = 1;
	BAP_I2C_AddFrame(&frame);
}


static void MAX1164x_speedCB()
{
	// Set I2C bitrate to "High Speed" mode
	Chip_I2CM_SetBusSpeed(LPC_I2C, MAX1164x_HIGHSPEED_BITRATE);
}

void MAX1164x_SetSpeed(bool spd)
{
	// Switch the clock to external if high-speed is requested
	// And speed up the bit i2C bit rate
	if (spd == MAX1164x_SPD_HI)
	{
		if (BAP_I2C_InitiateHighSpeedMode(MAX1164x_speedCB) != LPC_OK)
			LPC_GPIO_PORT->CLR[0] = 1 << 14;
		local_ADC->stop = 0;
	}
	else
	{
		Chip_I2CM_SetBusSpeed(LPC_I2C, MAX1164x_FAST_BITRATE);
	}
}



ErrorCode_t MAX1164x_RequestNewSample(MAX1164x_t* MAX1164x)
{
	BAP_I2C_Frame_t frame;
	frame.start = 1;
	frame.txdata = (MAX1164x_I2C_ADDR_7BIT << 1) + 1;
	frame.rxcnt = 4;
	frame.rxpoint = MAX1164x->rx_buffer;
	frame.ack_nack_ = 1;
	// data gets dumped into the temp buffer
	// If we're making the last frame, decide whether to use the stop flag
	// and set the callback function to fire
	frame.stop = MAX1164x->stop;
	frame.callback = MAX1164x_CBFunc;

	if(BAP_I2C_AddFrame(&frame))
		return LPC_OK;
	return LPC_ERROR;
}

