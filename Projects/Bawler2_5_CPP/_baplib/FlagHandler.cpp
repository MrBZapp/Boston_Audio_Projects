/* 
* FlagHandler.cpp
*
* Created: 5/16/2014 12:53:05 PM
* Author: Matt
*/


#include "FlagHandler.h"

// default constructor
FlagHandler::FlagHandler( SFRaddress gpio_address )
:m_flagsInUse(8)
{
	unsigned char initializer = 0x00;
	do{
		m_flagsInUse.AddToEnd( initializer );
		initializer++;
	} while (m_flagsInUse.SpaceRemaining());
	
	m_gpio_register = gpio_address; //set the FlagHandler to the I/O register
	*m_gpio_register |= 0x00; //clear the I/O register to prepare for flag.
} //FlagHandler

unsigned char FlagHandler::GetNewFlag(){
	return m_flagsInUse.PopFromFront();
}

bool FlagHandler::GetFlagState( unsigned char flagNumber ){
	return (bool)(*m_gpio_register && (1<<flagNumber));
}

void FlagHandler::SetFlag( unsigned char flagNumber ){
	*m_gpio_register |= (1<<flagNumber);//(m_gpio_register && (1<<flagNumber));
}


void FlagHandler::ClearFlag( unsigned char flagNumber ){
	*m_gpio_register &= ~(1<<flagNumber);
}
// default destructor
FlagHandler::~FlagHandler()
{
} //~FlagHandler
