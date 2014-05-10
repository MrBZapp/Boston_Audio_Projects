/*
 * USI.cpp
 *
 * Created: 4/27/2014 12:37:39 AM
 *  Author: Matt
 */ 

#include "global.h"
#include <avr/io.h>
#include <util/delay.h>
#include "TWI.h"

TWI_Master::TWI_Master(){
	this->usiStartup();
}

bool TWI_Master::sendDataBytes( unsigned char* messageStart, unsigned char messageSize ){
	//Local Variables for shifting bytes or (N)ACK bytes
	unsigned char const tempUSISR_8bit = (1<<USISIF)|(1<<USIOIF)|(1<<USIPF)|(1<<USIDC)|	(0x0<<USICNT0);	// Prepare register value to clear flags, and set USI to shift 8 bits i.e. count 16 clock edges.
	unsigned char const tempUSISR_1bit = (1<<USISIF)|(1<<USIOIF)|(1<<USIPF)|(1<<USIDC)|	(0xE<<USICNT0);	// Prepare register value to clear flags, and set USI to shift 1 bit i.e. count 2 clock edges.
	state_twi.addressMode = true; // Always send address first
	
	//Sanity Check
	if( (messageStart + messageSize) > (unsigned char*) RAMEND){ // Test if any of the message is outside SRAM space.
		state_twi.errors = DATA_OUT_OF_BOUND;
		return false;
	}
	
	//Gut Checks
#ifdef ENABLE_GUT_CHECKS
	if( messageSize <= 1 ){ //Test to see if the message contains one address byte and one data byte minimum
		state_twi.errors = NO_DATA;
		return false;
	}
	
	if( USISR & (1<<USISIF) ){ // Make sure nothing else accidentally triggered a start condition
		state_twi.errors = UNEXPECTED_START_CON;
		return false;
	}
	
	if( USISR & (1<<USIPF) ){ // Make sure nothing else accidentally triggered a stop condition
		state_twi.errors = UNEXPECTED_STOP_CON;
		return false;
	}
	if( USISR & (1<<USIDC) ){ // Detect Data Collisions
		state_twi.errors = UNEXPECTED_DATA_COL;
		return false;
	}
#endif

	if ( !this->sendStartCond() ){ //send a start condition and continue only if the condition is successfully sent
		return false;
	}
	
	// Write the message
	do{
		SCL_PORT &= ~(1<<SCL_BIT);              // Pull SCL LOW.
		USIDR = *(messageStart++);              // load data, prepare next byte in the message
		this->startTransfer( tempUSISR_8bit );  // Send 8 bits on bus.
	      
		//ACK from slave
		DI_DDR  &= ~(1<<DI_BIT);                // Enable SDA as input.
		if( !this->startTransfer( tempUSISR_1bit ) & (1<<TWI_NACK_BIT) ) { //check feedback and throw errors based on current send mode
			if ( state_twi.addressMode ){
				state_twi.errors = NO_ACK_ON_ADDRESS;
			} else {
				state_twi.errors = NO_ACK_ON_DATA;
				return false;
			}
		}
		state_twi.addressMode = false; //after the first message turn off addresss mode
	} while ( messageSize--);

	if ( !this->sendStopCond() ){
		return false; 
	}
	
	return true;
}

void TWI_Master::usiStartup(){
	DI_PORT |= (1<<DI_BIT);				// Enable pull-up on SDA, to set high as released state_twi.
	SCL_PORT |= (1<<SCL_BIT);           // Enable pull-up on SCL, to set high as released state_twi.
		
	SCL_DDR  |= (1<<SCL_BIT);           // Enable SCL as output.
	DI_DDR  |= (1<<DI_BIT);				// Enable SDA as output.
		
	USIDR = 0xFF;                       // Release SDA by setting USIDR "high."
	USICR = (0<<USISIE)|(0<<USIOIE)|    // Disable Interrupts.
	(1<<USIWM1)|(0<<USIWM0)|                            // Set USI in Two-wire mode.
	(1<<USICS1)|(0<<USICS0)|(1<<USICLK)|                // Software stobe as counter clock source
	(0<<USITC);
	USISR = (1<<USISIF)|(1<<USIOIF)|(1<<USIPF)|(1<<USIDC)|      // Clear flags,
	(0x0<<USICNT0);								                // and reset counter.
}

bool TWI_Master::sendStartCond(){
	SCL_PORT |= (1<<SCL_BIT);                     // Release SCL.
	while( !(SCL_PORT & (1<<SCL_BIT)) );          // Verify that SCL becomes high.
	_delay_us(T2_TWI);

	/* Generate Start Condition */
	DI_PORT &= ~(1<<DI_BIT);		// Force SDA LOW.
	_delay_us(T4_TWI);
	SCL_PORT &= ~(1<<SCL_BIT);		// Pull SCL LOW.
	DI_PORT |= (1<<DI_BIT);			// Release SDA.

	if( !(USISR & (1<<USISIF)) ){
		return false;
	}
	return true;
}

unsigned char TWI_Master::startTransfer( unsigned char temp ){
	USISR = temp; // Set USISR according to temp.
	// Prepare clocking.
	temp =  (0<<USISIE)|(0<<USIOIE)|				// Interrupts disabled
			(1<<USIWM1)|(0<<USIWM0)|                // Set USI in Two-wire mode.
			(1<<USICS1)|(0<<USICS0)|(1<<USICLK)|    // Software clock strobe as source.
			(1<<USITC);                             // Toggle Clock Port.
	
	do{
		_delay_us(T2_TWI);
		USICR = temp;                       // Generate positive SCL edge.
		while( !(SCL_PORT & (1<<SCL_BIT)) );// Wait for SCL to go high.
		_delay_us(T4_TWI);
		USICR = temp;                       // Generate negative SCL edge.
	} while( !(USISR & (1<<USIOIF)) );       // Check for transfer complete.
	
	_delay_us(T2_TWI);
	temp  = USIDR;                          // Read out data.
	USIDR = 0xFF;                           // Release SDA.
	DI_DDR |= (1<<DI_BIT);					// Enable SDA as output.

	return temp;                            // Return the data from the USIDR
}

bool TWI_Master::sendStopCond(){
	DI_PORT &= ~(1<<DI_BIT);           // Pull SDA low.
	SCL_PORT |= (1<<SCL_BIT);            // Release SCL.
	while( !(SCL_PORT & (1<<SCL_BIT)) );  // Wait for SCL to go high.
	_delay_us(T4_TWI);
	DI_PORT |= (1<<DI_BIT);            // Release SDA.
	_delay_us(T2_TWI);
	
	if( !(USISR & (1<<USIPF)) ){
		state_twi.errors = MISSING_STOP_CON;
		return false;
	}
	return true;
}
