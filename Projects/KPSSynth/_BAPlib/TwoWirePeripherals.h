/*
 * TwoWirePeripherals.h
 *
 * Created: 8/15/2014 5:16:50 PM
 *  Author: Matt Zapp
 */ 


#ifndef TWOWIREPERIPHERALS_H_
#define TWOWIREPERIPHERALS_H_
#include <avr/io.h>
#include "../_AVRLib/timer.h"

#define DEFAULT_PRESCALER
//Standard peripheral use warning system:
#ifdef SPI_USE_TIMER0 //If global.h has designated that the SPI use timer 0 overflow as its count source
	#ifdef USING_TIMER0 // first check to see if anyone's using timer one
		#undef SPI_USE_TIMER0 // then un-define the designation so as not to include bad code
		#warning "SPI: Timer 0 is already in use, reverting to software clock"
	#else
		#define USING_TIMER0 // if no one's using it, designate Timer 0 as in use.
	#endif
#endif

#ifndef SPI_MAX_MESSAGE_SIZE_TYPE
	#define SPI_MAX_MESSAGE_SIZE_TYPE int //standard message size is 2 bytes
#endif

 //Attiny 2313 uses Port B
#define SCL 7 //Serial clock
#define SDA 5 //Serial data wire
#define DO 6 // Data out (three wire mode)
#define DI 5 // Data in (three wire mode)

//Master/Slave enum
enum masterSlave{ Master, Slave };
//list of SPI wire modes
enum wireModes{ Three_Wire, Two_Wire, Two_Wire_Hold };
	
//Gloabl Variables
unsigned SPI_MAX_MESSAGE_SIZE_TYPE spi_Message = 0;
unsigned char spi_Message_SentBits = 8; //USI data shift-register
unsigned char spi_Message_ByteCount = 1; // raw size of the message is the number of times we'll have to load the USIDR.
unsigned char spi_Message_BitCount = 8; // size of a standard message.  is set by spi_TransmitData.
unsigned char spi_TransmitInProgress = 0; // flag to indicate if a transmit is in progress.

//Forward Declarations for internal functions
void spi_ReadyByteForTransmission();

void inline spi_Idle(){ //Sets the data register high to idle the serial buss
	USIDR = 0xFF;
}

//! Initializes the internal SPI hardware to a default setting
///
void spi_Init( unsigned char masterSlaveSelect, unsigned char wireMode ){
	spi_Idle();
	//Clock Type
	#ifdef SPI_USE_TIMER0
		timer0Init();
		
		//Stop the clock and clear the count
		timer0SetPrescaler( TIMER_CLK_STOP );
		TCNT0 = 0;
		
		//set clock speed
		timer0SetOverflowPoint( 0x02 );
		
		//turn on shift on timer 0 overflow
		USICR |= (1<<USICS0); 
	#else
		//TODO:
		//put code here that would allow for software strobing of shift register
	#endif
	
	//Master/Slave selection
	if( masterSlaveSelect == 0 ){
		DDRB |= (1<<SCL) | (1<<SDA);// if you're transmitting, you're a master. Turn on the drivers
		PORTB |= (1<<SCL) | (1<<SDA);
	}else{//slave code goes
	}
	
	//wire mode selection
	switch( wireMode ){
		case( Three_Wire ):
			break;
		case( Two_Wire ):
			USICR |= (1<<USIWM1);
			break;
		case ( Two_Wire_Hold ):
			USICR |= (3<<USIWM0);
			break;
		default: //Error catch
		break;
	}
	
	//turn on the counter overflow
	USICR |= (1<<USIOIE); //enable the counter overflow.
};

//! Left aligns and prepares the flags necessary to ensure safe transmission of a message of any length from 0 to SPI_MAX_MESSAGE_SIZE_TYPE
///
void spi_ReadyMessageForTransmission(unsigned SPI_MAX_MESSAGE_SIZE_TYPE data, unsigned char local_Message_BitCount ){
	//prep the message statistics
	spi_Message_SentBits = 8; //USI data shift-register
	spi_Message_BitCount = local_Message_BitCount;
	spi_Message_ByteCount = 1;//No matter what, we'll send something at least once
	spi_TransmitInProgress = 1; //A transmission is beginning. This will not go 0 until the entire message is cleared
		
	while( local_Message_BitCount > 8){
		spi_Message_ByteCount++; // for ever eight bits increments the number of times we'll have to load the USIDR.
		local_Message_BitCount -= 8; //subtract a byte for ever 8 bits in the message's bit count.
	}
	
	//left-align the data if necessary
	if( spi_Message_BitCount < (sizeof(data) * 8) ){
		spi_Message = (data << ((sizeof(data) * 8)  - spi_Message_BitCount));// find the difference between the size of the message (in bits) and the bit count of the message
	}else{
		spi_Message = data;
	}
}

//!
void spi_ReadyByteForTransmission() {
	//Load a byte from the spi_Message variable into the data register
	USIDR = spi_Message >> ( 8 * (spi_Message_ByteCount - 1) );
	
	//update the remaining bits
	if (spi_Message_BitCount < 8 ){
		spi_Message_SentBits = spi_Message_BitCount;
	}
	
	// Load the counter:
	// count back from 16 to transmit bits if fewer than 8 bits
	// clear all flags
	if ( spi_Message_BitCount < 8){
		USISR |= ( 0x0F - (spi_Message_BitCount) );
		} else{
		USISR |= (0x0F - 8);
	}
}

void spi_transmitRawMessage( unsigned char data, unsigned char local_Message_BitCount ){ //(193cy)
	spi_ReadyMessageForTransmission( data, local_Message_BitCount );
	spi_ReadyByteForTransmission( data, local_Message_BitCount );
	#ifdef SPI_USE_TIMER0
		timer0SetPrescaler( TIMER_CLK_DIV8 );
	#else
		//BitBang Code
	#endif
}

void spiGenerateStartCondition(){
	//TODO: generate start condition
}

void spi_GenerateStopCondition(){
	//TODO: generate stop condition
}

void spi_shiftNBits( unsigned char ShiftBitCount ){
	//TODO: use software strobe to send data
}



ISR(USI_OVERFLOW_vect){
	timer0SetPrescaler( TIMER_CLK_STOP ); //Stop clock
	
	//update the bit count
	if(spi_Message_BitCount < 8){
		spi_Message_SentBits = spi_Message_BitCount;
	}

	spi_Message_BitCount -= spi_Message_SentBits; //update sent bits
	spi_Message_ByteCount --; //we've loaded one of our n bytes.	
	
	//Logic for handling multi-byte messages
	if(spi_Message_ByteCount !=0 ){
		spi_ReadyByteForTransmission();
		#ifdef SPI_USE_TIMER0

			TCNT0 = 0; //clear count
			timer0SetPrescaler( TIMER_CLK_DIV8 ); //Start clock
		#endif					
	}else{
		#ifdef SPI_USE_TIMER0
			TCNT0 = 0; //clear count
		#endif
		
		USISR &= 0xF0;//clear the status register of count and all flags
		spi_TransmitInProgress = 0; //message complete
	}	
}

#endif /* TWOWIREPERIPHERALS_H_ */
