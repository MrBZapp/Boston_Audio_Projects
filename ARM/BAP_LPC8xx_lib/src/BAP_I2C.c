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

//// Allow the IRQ to access the I2C structure
static BAP_I2C_Bus_t* local_I2C;


//// Static functions and IRQ
/*
 * Begins the transmission of the message
 * by managing the current message's address
 * and initiating the start condition
 */
static void BAP_I2C_Master_AddressSlave()
{
	uint8_t frame = 0; // empty frame
	uint8_t strst = BAP_I2C_MSTSTART; // assume we're starting a new transmission

	if (local_I2C->msg.tenbit)
	{
		switch(local_I2C->state){
		// Entry Point 1
		default:
			// make sure the message accounts for the extra byte we need to send
			local_I2C->msg.txcnt++;

			// format the MSB correctly
			frame = BAP_I2C_MASTRCMD_TENBITADDR(local_I2C->msg.addr.byte[1]);

			// set state to indicate the next byte will be from the address
			local_I2C->state = BAP_I2C_ADDRESSING;
			break;

		// Entry Point 2
		case(BAP_I2C_ADDRESSING):
			// If we're Addressing, we've been here before.
			// we have no extra bytes to transmit, remove the extra byte from the count
			local_I2C->msg.txcnt--;

			// Load up the second byte
			frame = local_I2C->msg.addr.byte[0];

			// Set the state to indicate we need to re-start next time we come through here.
			local_I2C->state = BAP_I2C_TENBITADDRESSING;

			// Set the Master command type to "continue" instead of start to continue the address.
			strst = BAP_I2C_MSTCONTINUE;
			break;

		// Entry Point 3
		case(BAP_I2C_TENBITADDRESSING):
			// Place the two most significant bits in the data frame after the 10-Bit address Master command.
			frame = BAP_I2C_MASTRCMD_TENBITADDR(local_I2C->msg.addr.byte[1]);

			// set the state to indicate that the next byte will be from the message
			local_I2C->state = BAP_I2C_MSGINPROGRESS;
			break;
		}
	}
	else
	{
		// Otherwise, we need to send the most significant byte first.
		frame = ((local_I2C->msg.addr.sevenbit & SEVENBIT_MASK) << 1);

		// set the state to indicate that the next byte will be from the message
		local_I2C->state = BAP_I2C_MSGINPROGRESS;
	}

	// assume we're reading by setting the read-bit (bit0) high.
	frame++;

	// if we have more than the address to transmit we are writing. Set the r/w bit low.
	if (local_I2C->msg.txcnt !=0)
	{
		frame &= ~(1 << 0);
	}

	// Load the data register with the frame
	BAP_I2C->MSTDAT = frame;

	// enable the MSTPENDING interrupt flag
	BAP_I2C->INTENSET |= BAP_I2C_MSTPENDING;

	// start/continue/restart the transfer
	BAP_I2C->MSTCTL |= strst;

	// Increment the index and reduce the count
	local_I2C->master_tx_index++;
	local_I2C->msg.txcnt--;
}


static void BAP_I2C_Master_TX_Continue()
{
	// Load the data register with the value under the index.
	BAP_I2C->MSTDAT = local_I2C->master_rx_data[local_I2C->master_tx_index];

	// continue the transfer
	BAP_I2C->MSTCTL |= BAP_I2C_MSTCONTINUE;

	// increment the index and reduce the count
	local_I2C->master_tx_index++;
	local_I2C->msg.txcnt--;
}


static void BAP_I2C_Master_RX_Continue()
{
	// Copy the byte into the buffer
	local_I2C->master_rx_data[local_I2C->master_rx_index] = BAP_I2C->MSTDAT;

	// Continue with the reception
	BAP_I2C->MSTCTL |= BAP_I2C_MSTCONTINUE;

	// increment the index and reduce the count
	local_I2C->master_rx_index++;
	local_I2C->msg.rxcnt--;
}


void I2C_IRQHandler(void)
{
	// Check what interrupt triggered the request
	// If it was the service becoming available, check on the status of the message
	if (BAP_I2C->INTSTAT & BAP_I2C_MSTPENDING)
	{
		switch(local_I2C->state)
		{
		// If we're addressing, just keep on doing that
		case(BAP_I2C_ADDRESSING):
		case(BAP_I2C_TENBITADDRESSING):
				BAP_I2C_Master_AddressSlave();
				break;

		// If we're in the message, figgure out whether we should transmit or receive
		case(BAP_I2C_MSGINPROGRESS):
			// Transmit comes before receive
			// If the transmit isn't over, transmit! This should clear the interrupt on its own?
			if(local_I2C->msg.txcnt != 0)
			{
				BAP_I2C_Master_TX_Continue();
			}

			// If the receive isn't over, receive!
			else if (local_I2C->msg.rxcnt != 0)
			{
				BAP_I2C_Master_RX_Continue();
			}

			// If everything is done
			else
			{
				// turn off the interrupt
				BAP_I2C->INTENCLR = BAP_I2C_MSTPENDING;

				// issue a stop or not
				BAP_I2C->MSTCTL |= BAP_I2C_MSTSTOP(local_I2C->stop_flag);

				// Set the state to idle
				local_I2C->state = BAP_I2C_IDLE;

				// fire the callback or not.
				if(local_I2C->msg.callback != NULL)
					local_I2C->msg.callback(LPC_OK, 0);
			}
			break;

		// Bad news if we get here.
		default:
			break;
		}
	}
}

//// Public Functions
/***
 * Initializes the hardware for the I2C
 * It is expected that the user code will have enabled the system clock
 * and routed the pins before calling this function
 **/
void BAP_I2C_Init(CHIP_PINx_T sda, CHIP_PINx_T scl)
{
	// Initialize the peripheral
	Chip_I2C_Init();

	// Allow for 400kHz+ bit rate
	Chip_IOCON_PinSetI2CMode(LPC_IOCON, sda, PIN_I2CMODE_FASTPLUS);
	Chip_IOCON_PinSetI2CMode(LPC_IOCON, scl, PIN_I2CMODE_FASTPLUS);

	// Enable the I2C IRQ
	NVIC_EnableIRQ(I2C_IRQn);
}


/***
 * Sets the current I2C channel hardware as master
 **/
void BAP_I2C_SetAsMaster(BAP_I2C_Bus_t* I2C)
{
	// Assign the local pointer
	local_I2C = I2C;

	// Enable the bus for use as a master
	BAP_I2C->CFG |= BAP_I2C_MSTEN(1);
}


ErrorCode_t BAP_I2C_SetBaud(uint32_t speed)
{
	//block div 0
	if (speed == 0)
	{
		return LPC_ERROR;
	}

	speed = speed * 4;

	BAP_I2C->DIV = (Chip_Clock_GetSystemClockRate() / speed) & 0x0000FFFF;
	return LPC_OK;

}


ErrorCode_t BAP_I2C_SetTimeout(uint32_t timeout)
{

	return LPC_OK; //TODO: Actually set the baud rate.
}

ErrorCode_t BAP_I2C_MasterProcessNewMessage(BAP_I2C_MsgParam_t* param, uint8_t* data)
{
	// check to make sure everything will fit
	if (param->rxcnt > local_I2C->mem_size || param->txcnt > local_I2C->mem_size)
	{
		return ERR_I2C_BUFFER_OVERFLOW;
	}

	// is the I2C bus finished processing the last message?
	if ((local_I2C->msg.txcnt == 0) && (local_I2C->msg.rxcnt == 0))
	{
		// Copy the message parameters
		local_I2C->msg = *param;

		// clear the indexes
		local_I2C->master_rx_index = 0;
		local_I2C->master_tx_index = 0;

		// Copy the transmitting data to the tx buffer
		memcpy(local_I2C->master_tx_data, data, param->txcnt);

		// Start the transmission
		BAP_I2C_Master_AddressSlave();
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
	uint8_t hispeed = BAP_I2C_MASTERCMD_HIGHSPEED;
	BAP_I2C_MsgParam_t param;

	param.txcnt = 1;
	param.rxcnt = 0;
	param.callback = NULL;

	 // No stop flags after going into high-speed mode!
	local_I2C->stop_flag = 0;

	// Start the new message, return the errors.
	return  BAP_I2C_MasterProcessNewMessage(&param, &hispeed);
}


void BAP_I2C_DisableHighSpeedMode(uint32_t newspeed)
{
	local_I2C->stop_flag = 1; // turn the stop-flag back on
	BAP_I2C_SetBaud(newspeed); // reset to new speed
}

