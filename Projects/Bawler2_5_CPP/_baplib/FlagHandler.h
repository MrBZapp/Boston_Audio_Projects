/* 
* FlagHandler.h
*
* Created: 5/16/2014 12:53:06 PM
* Author: Matt
*/

#include "Uint8_Buffer.h"
#include "BAPTypedefs.h"

#ifndef __FLAGHANDLER_H__
#define __FLAGHANDLER_H__

class FlagHandler{
//variables
public:
protected:
private:
	volatile unsigned char* m_gpio_register;
	Buffer<unsigned char> m_flagsInUse;

//functions
public:
	FlagHandler( SFRaddress gpio_address );
	unsigned char GetNewFlag();
	bool GetFlagState( unsigned char flagNumber );
	void SetFlag( unsigned char flagNumber );
	void ClearFlag( unsigned char flagNumber );
	~FlagHandler();
protected:
private:
	//FlagHandler( const FlagHandler &c );
//	FlagHandler& operator=( const FlagHandler &c );

}; //FlagHandler

#endif //__FLAGHANDLER_H__
