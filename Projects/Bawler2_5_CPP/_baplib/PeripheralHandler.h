/* 
* PeripheralHandler.h
*
* Created: 5/19/2014 4:18:48 PM
* Author: Matt
*/


#ifndef __PERIPHERALHANDLER_H__
#define __PERIPHERALHANDLER_H__

//Forward Declarations and Peripheral Handler Types

class PeripheralHandler;
class USART;
class MidiDevice;
class TWI_Master;
class HardwareTimer_8Bit;
class HardwareTimer_16Bit;

typedef void (PeripheralHandler::*PeripheralHandlerMemberFunction)( unsigned char data );

//Includes

#include "FlagHandler.h"
#include "USART.h"
#include "MidiDevice.h"
#include "TWI.h"
#include "HardwareTimer.h"

//Classes
class PeripheralHandler{
  //variables
	public:
	protected:
	private:

  //Peripherals		
		USART* mp_USART;
		MidiDevice* mp_MidiDevice;
		HardwareTimer_8Bit* mp_8BitTimer;
		HardwareTimer_16Bit* mp_16BitTimer;
		TWI_Master* mp_TWI_Master;
  //Soft Devices
		FlagHandler m_GPIOR0Flags;
		FlagHandler m_GPIOR1Flags;
  
  //Functions
	public:
		explicit PeripheralHandler();
		MidiDevice* GetNewMidiDevice();
		USART* GetNewUSART( unsigned long baudrate );
		HardwareTimer_8Bit* GetNew8BitTimer();
		HardwareTimer_8Bit* GetNew8BitTimer( prescaleSetting prescale );
		HardwareTimer_16Bit* GetNew16BitTimer();
		HardwareTimer_16Bit* GetNew16BitTimer( prescaleSetting prescale );
		TWI_Master* GetNewTWIMaster();
		
		void forward_MIDI( unsigned char data );

		void FireRXInterrupt( unsigned char USARTSpoofData );
		~PeripheralHandler();
	protected:
	private:
		
	  //Routing Functions and Function Pointers
		volatile PeripheralHandlerMemberFunction mp_PeripheralHandlerMemberFunction; // Used to change the effect of a client's ISR action
	  //Midi
		volatile MidiDeviceMemberFunction mp_MidiDeviceMemberFunction; //Arbitrary Member function pointer to the member Midi Device
		void executeMIDIMemberFunction();
		void setMIDIMemberFunction( MidiDeviceMemberFunction );

		//Copy/Move Operators
		PeripheralHandler( const PeripheralHandler &c );
		PeripheralHandler& operator=( const PeripheralHandler &c );

}; //PeripheralHandler

#endif //__PERIPHERALHANDLER_H__
