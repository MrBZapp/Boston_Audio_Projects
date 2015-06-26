/*
 * BAP_I2C.c
 *
 *  Created on: Jun 17, 2015
 *      Author: Matt
 */

#include "BAP_I2C.h"
#include <string.h>
#include "chip.h"


//// Static Local variables
static I2C_PARAM_T local_param;
static uint8_t* master_tx_data;
static uint32_t master_tx_index;
static uint32_t master_rx_index;
static uint32_t mem_size;

//// Static functions and IRQ
static void BAP_I2C_Master_TX_Start()
{
	// Load the data register with the value under the index.
	BAP_I2C->MSTDAT = local_param.buffer_ptr_send[master_tx_index++];

	// enable the MSTPENDING interrupt flag
	BAP_I2C->INTENSET |= BAP_I2C_MSTPENDING;

	// start the transfer
	BAP_I2C->MSTCTL |= BAP_I2C_MSTSTART;
}


static void BAP_I2C_Master_TX_Continue()
{
	// Load the data register with the value under the index.
	BAP_I2C->MSTDAT = local_param.buffer_ptr_send[master_tx_index++];

	// continue the transfer
	BAP_I2C->MSTCTL |= BAP_I2C_MSTCONTINUE;

	// decrement the quantity of things we have to transmit
	local_param.num_bytes_send--;
}


static void BAP_I2C_Master_RX_Continue()
{
	// Copy the byte into the buffer
	local_param.buffer_ptr_rec[master_rx_index++] = BAP_I2C->MSTDAT;

	// Continue with the reception
	BAP_I2C->MSTCTL |= BAP_I2C_MSTCONTINUE;

	// decrement the quantity of things we have to receive
	local_param.num_bytes_rec--;
}


void I2C_IRQHandler(void)
{
	// Check what interrupt triggered the request
	// If it was the service becoming available, check on the status of the message
	if (BAP_I2C->INTSTAT & BAP_I2C_MSTPENDING)
	{
		// If the transmit isn't over, transmit! This should clear the interrupt on its own?
		if(local_param.num_bytes_send != 0)
		{
			BAP_I2C_Master_TX_Continue();
		}

		// If the receive isn't over, receive!
		else if (local_param.num_bytes_rec != 0)
		{
			BAP_I2C_Master_RX_Continue();
		}

		// If everything is done, turn off the interrupt, issue a stop or not, and fire the callback or not.
		else
		{
			BAP_I2C->INTENCLR = BAP_I2C_MSTPENDING;
			BAP_I2C->MSTCTL |= BAP_I2C_MSTSTOP(local_param.stop_flag);

			if(local_param.func_pt != NULL)
				local_param.func_pt(LPC_OK, 0);
		}
	}
}

//// Public Functions
void BAP_I2C_MasterInit(uint8_t* start_of_tx_ram, uint32_t available_space)
{
	master_tx_data = start_of_tx_ram;
	mem_size = available_space;
}

ErrorCode_t BAP_I2C_SetBaud(uint32_t speed)
{
	return LPC_OK; //TODO: write this code instead of relying entirely on the API
}

ErrorCode_t BAP_I2C_MasterProcessNewMessage(I2C_PARAM_T* param)
{
	// check to make sure everything will fit
	if (param->num_bytes_rec > mem_size || param->num_bytes_send > mem_size)
	{
		return ERR_I2C_BUFFER_OVERFLOW;
	}

	// is the I2C bus finished processing the last message?
	if ((local_param.num_bytes_send == 0) && (local_param.num_bytes_rec == 0))
	{
		// Copy the relevant param items
		local_param.buffer_ptr_rec = param->buffer_ptr_rec;
		local_param.num_bytes_rec = param->num_bytes_rec;
		local_param.num_bytes_send = param->num_bytes_send;
		local_param.func_pt = param->func_pt;

		// clear the indexes
		master_rx_index = 0;
		master_tx_index = 0;

		// Copy the buffer
		memcpy(local_param.buffer_ptr_send, param->buffer_ptr_send, param->num_bytes_send);

		// Start the transmission
		BAP_I2C_Master_TX_Start();
	}
	else
	{
		return LPC_ERROR;
	}
	
	return LPC_OK;
}


/***
 * Remember to set the pins to operate in high-speed mode if using this function
 */
ErrorCode_t BAP_I2C_InitiateHighSpeedMode()
{
	uint8_t hispeed = I2C_MASTERCMD_HIGHSPEED;
	I2C_PARAM_T param;

	param.num_bytes_send = 1;
	param.buffer_ptr_send = &hispeed;
	param.num_bytes_rec = 0;
	local_param.stop_flag = 0; // No stop flags after going into high-speed mode!

	// Start the new message, return the errors.
	return  BAP_I2C_MasterProcessNewMessage(&param);
}

void BAP_I2C_DisableHighSpeedMode(uint32_t newspeed)
{
	local_param.stop_flag = 1; // turn the stop-flag back on
	BAP_I2C_SetBaud(newspeed); // reset to new speed
}

