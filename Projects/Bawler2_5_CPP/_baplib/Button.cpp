/* 
* Button.cpp
*
* Created: 5/27/2014 10:20:28 PM
* Author: Matt
*/


#include "Button.h"

// default constructor
Button::Button( SFRaddress port, unsigned char pin, PeripheralHandler* pPeripheralHandler )
{
	m_pPeripheralHandler = pPeripheralHandler;
	io_Port = port;
	io_Pin = pin;
} //Button

// default destructor
Button::~Button()
{
} //~Button
