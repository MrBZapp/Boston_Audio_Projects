/* 
* Button.h
*
* Created: 5/27/2014 10:20:28 PM
* Author: Matt
*/


#ifndef __BUTTON_H__
#define __BUTTON_H__

#include "PeripheralHandler.h"
class Button
{
//variables
public:
protected:
private:
	SFRaddress io_Port;
	unsigned char io_Pin;
	PeripheralHandler* m_pPeripheralHandler;
	unsigned char pin;

//functions
public:
	Button ( SFRaddress port, unsigned char pin, PeripheralHandler* pPeripheralHandler );
	bool GetState();
	bool GetStateDebounced();
	~Button();
protected:
private:
	Button( const Button &c );
	Button& operator=( const Button &c );

}; //Button

#endif //__BUTTON_H__
