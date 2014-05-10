/*
 * USI.h
 *
 * Created: 4/27/2014 12:22:16 AM Author: Matt Zapp, based on the Atmel USI_TWI App note
 */ 


#ifndef TWI_H_
#define TWI_H_

#include <stdbool.h>

/****************************************************************************
  Bit and byte definitions
****************************************************************************/
#define TWI_READ_BIT  0       // Bit position for R/W bit in "address byte".
#define TWI_ADR_BITS  1       // Bit position for LSB of the slave address bits in the init byte.
#define TWI_NACK_BIT  0       // Bit position for (N)ACK bit.

#define NO_ACK_ON_ADDRESS		0x01  // The slave did not acknowledge  the address
#define NO_ACK_ON_DATA			0x02  // The slave did not acknowledge  all data
#define MISSING_START_CON		0x03  // Generated Start Condition not detected on bus
#define MISSING_STOP_CON		0x04  // Generated Stop Condition not detected on bus
#define UE_DATA_COL				0x05  // Unexpected Data Collision (arbitration)
#define UNEXPECTED_STOP_CON		0x06  // Unexpected Stop Condition
#define UNEXPECTED_START_CON	0x07  // Unexpected Start Condition
#define NO_DATA					0x08  // Transmission buffer is empty
#define DATA_OUT_OF_BOUND		0x09  // Transmission buffer is outside SRAM space


// For use with _delay_us()
#define T2_TWI    5 		// >4,7us
#define T4_TWI    4 		// >4,0us

// de-comment to enable special error checking for unexpected conditions and data collisions.  Normally not necessary with a single-master.
//#define ENABLE_GUT_CHECKS 0 

class TWI_Master{
	public:
		TWI_Master();
		bool sendDataBytes( unsigned char* messageStart, unsigned char messageLength);
	protected:
	private:
		union state{
			unsigned char errors;
			struct{
				bool addressMode : 1;
				bool masterWriteDataMode : 1;
				bool memReadMode : 1;
			};
		}state_twi;
	
		void usiStartup(); //Initializes USI
		void usiShutdown(); //Turns off all USI-related options
		unsigned char startTransfer( unsigned char temp); // shifts 
		bool sendStartCond();
		bool sendStopCond();
};


#endif /* USI_H_ */