/*
 * TLV5620.h
 *
 * Created: 8/15/2014 5:50:39 PM
 *  Author: Matt Zapp
 */ 


#ifndef TLV5620_H_
#define TLV5620_H_
#include <stdbool.h>
#include "TwoWirePeripherals.h"

#define TLV5620_HIGH_RANGE 1
#define TLV5620_LOW_RANGE 0
#define  TLV5620_Init() spi_Init( Master, Two_Wire_Hold );

bool TLV5620_SendMessage( unsigned char local_dacValue, unsigned char dacNumber, unsigned char range ){
	//bond the message together
	unsigned int message = ( ((dacNumber -1) << 9 ) | //Shift the DAC numer into the message
							 (range << 8 ) |		//Select the range of the message
							 ( local_dacValue ));	//Set the data value
	
	//if the transmitter is ready, give it a message to send
	if( ! spi_TransmitInProgress ){
		spi_transmitRawMessage( message, 11 ); //Send the message.
		return true;
	} else{
		return false;
	}
}


#endif /* TLV5620_H_ */