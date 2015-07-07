/*
 * BAP_I2C.c
 *
 *  Created on: Jun 17, 2015
 *      Author: Matt
 */

#include "BAP_I2C.h"
#include <string.h>
#include "chip.h"

#define SEVENBIT_MASK 0x7F
#define BAP_WRITE_BIT (1 << 0)

#define BAP_I2C_BUFFER_SIZE 16

/* Tx buffer */
static BAP_I2C_Frame_t BAP_I2C_FrameData[BAP_I2C_BUFFER_SIZE];
static RINGBUFF_T BAP_I2C_Frames;


//// Allow the IRQ to access the last frame sent by the I2C bus
static BAP_I2C_Frame_t local_frame;
static volatile uint8_t rxtx_message_flag;

//// Static functions and IRQ
static void BAP_I2C_Reset()
{
	rxtx_message_flag = 0;

	local_frame.callback = 0;
	local_frame.txdata = 0;
	local_frame.rxcnt = 0;
	local_frame.rxpoint = 0;
	local_frame.ack_nack_ = 0;
	local_frame.start = 0;
	local_frame.stop = 0;

	RingBuffer_Flush(&BAP_I2C_Frames);

	// Clear the arbitration loss and errors.
	Chip_I2CM_ClearStatus(LPC_I2C, I2C_STAT_MSTRARBLOSS | I2C_STAT_MSTSTSTPERR);

	Chip_I2C_ClearInt(LPC_I2C, I2C_INTENSET_MSTPENDING | I2C_INTENSET_MSTRARBLOSS | I2C_INTENSET_MSTSTSTPERR);

}

static ErrorCode_t BAP_I2C_NextFrame()
{
	// If there is a frame, process it
	if (RingBuffer_Pop(&BAP_I2C_Frames, &local_frame))
	{
		// The I2C Bus is now active.
		rxtx_message_flag = 1;

		// write address or whatever other data is there to be written
		LPC_I2C->MSTDAT = (uint32_t) local_frame.txdata;

		// if the frame is a start condition, send the start condition, otherwise, send a continue.
		if (local_frame.start == 1)
		{
			LPC_I2C->MSTCTL = I2C_MSTCTL_MSTSTART;
		}

		// if it isn't, we're continuing a transmission
		else
		{
			LPC_I2C->MSTCTL = I2C_MSTCTL_MSTCONTINUE;
		}

		// Enable the interrupts
		Chip_I2C_EnableInt(LPC_I2C, I2C_INTENSET_MSTPENDING);
	}

	// If there are no frames, turn off the interrupts and tell everyone.
	else
	{
		// Reset messaging, I2C Bus is now idle.
		BAP_I2C_Reset();
	}
	return LPC_OK;
}

void I2C_IRQHandler(void)
{
	uint32_t status = LPC_I2C->STAT & ~I2C_STAT_RESERVED;
	/* Master is Pending */
	if (status & I2C_STAT_MSTPENDING)
	{
		/* Branch based on Master State Code */
		switch (Chip_I2CM_GetMasterState(LPC_I2C))
		{
		// If there is receive data available and the receive frame has a pointer
		case I2C_STAT_MSTCODE_RXREADY:
			// If we're still receiving more data, break early to do that
			if (local_frame.rxcnt > 0)
			{
				local_frame.rxcnt--;
				// read the data into wherever the index and icrement it
				local_frame.rxpoint[local_frame.rxcnt] = LPC_I2C->MSTDAT;
				Chip_I2CM_MasterContinue(LPC_I2C);
				return;
			}
			break;

			// If master is ready to send or just chillin
		case I2C_STAT_MSTCODE_IDLE:
		case I2C_STAT_MSTCODE_TXREADY:
			break;

		// If the slave was disobedient
		case I2C_STAT_MSTCODE_NACKADR:
		case I2C_STAT_MSTCODE_NACKDAT:
			// Stop
			break;

		// If there is ... things that happen? Maybe? Hopefully not?
		default:
			//Bad news Bears.
			break;
		}
	}

	// POST-FRAME ACTION!!
	// if the frame is supposed to have a stop condition, do so and release the I2C bus
	if (local_frame.stop == 1)
	{
		Chip_I2CM_SendStop(LPC_I2C);
	}

	// Save the callback momentarily, we can run it after we ask for the next frame
	VoidFuncPointer CB = local_frame.callback;

	// get the next message frame.
	BAP_I2C_NextFrame();

	// if the old frame had a callback, fire it.
	if (CB != 0)
	{
		CB();
	}
LPC_GPIO_PORT->NOT[0] |= 1<<14;
}



//// Public Functions
/***
 * Initializes the hardware for the I2C
 * It is expected that the user code will have enabled the system clock
 * and routed the pins before calling this function
 **/
void BAP_I2C_Init()
{
	// Initialize the peripheral
	Chip_I2C_Init(LPC_I2C);

	/* Setup clock rate for I2C */
	Chip_I2C_SetClockDiv(LPC_I2C, 1);

	/* Setup I2CM transfer rate */
	Chip_I2CM_SetBusSpeed(LPC_I2C, 400000);

	// Initialize the buffer
	RingBuffer_Init(&BAP_I2C_Frames, &BAP_I2C_FrameData, sizeof(BAP_I2C_Frame_t), BAP_I2C_BUFFER_SIZE);

	// Initialize the buffer and message
	BAP_I2C_Reset();
	NVIC_EnableIRQ(I2C_IRQn);
}


int BAP_I2C_GetBufferSpace()
{
	return RingBuffer_GetFree(&BAP_I2C_Frames);
}


ErrorCode_t BAP_I2C_AddFrame(BAP_I2C_Frame_t* frame)
{
	if (RingBuffer_Insert(&BAP_I2C_Frames, frame))
	{
		// If the status is idle, send the new frame(s)!
		if (rxtx_message_flag == 0)
		{
			BAP_I2C_NextFrame();
		}
		return LPC_OK;
	}
	else
	{

	}
	return LPC_ERROR;
}



/***
 * Remember to set the pins to operate in high-speed mode if using this function
 */
ErrorCode_t BAP_I2C_InitiateHighSpeedMode(VoidFuncPointer new_speed_CB)
{
	BAP_I2C_Frame_t frame;
	frame.msgst = 1;
	frame.start = 1;
	frame.rxcnt = 0;
	frame.stop = 0;
	// Nack from slaves
	frame.ack_nack_ = 0;
	frame.callback = new_speed_CB;
	frame.txdata = BAP_I2C_MASTERCMD_HIGHSPEED;

	// Start the new message, return the errors.
	return  BAP_I2C_AddFrame(&frame);
}
