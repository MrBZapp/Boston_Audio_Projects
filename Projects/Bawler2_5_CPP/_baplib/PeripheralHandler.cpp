/* 
* PeripheralHandler.cpp
*
* Created: 5/19/2014 4:18:48 PM
* Author: Matt
*/


#include "PeripheralHandler.h"

// default constructor
PeripheralHandler::PeripheralHandler()
	:m_GPIOR0Flags( (SFRaddress) &GPIOR0 )//Initialize flag handlers
	,m_GPIOR1Flags( (SFRaddress) &GPIOR1 )		
{
} //PeripheralHandler

/*prepares and returns a pointer to a MIDI device.*/
MidiDevice* PeripheralHandler::GetNewMidiDevice(){
	mp_MidiDevice = new MidiDevice( ); // Create Midi Device
	mp_PeripheralHandlerMemberFunction = &PeripheralHandler::forward_MIDI;//Set the forwarding function to the MIDI device
	mp_MidiDeviceMemberFunction = &MidiDevice::Router; //Set the Midi signal router as the main receive function for the Peripheral forwarding function
	mp_USART = GetNewUSART( 31250 ); //Create USART running at MIDI clock speed
	mp_USART->AssignHandlerFunction_USART_RX( mp_PeripheralHandlerMemberFunction );//Assign the peripheral handler to route USART recieve interrupts
	//TO-DO:  Connect TX interrupts if necessary
	return mp_MidiDevice;
}

/*prepares and returns a pointer to a USART.*/
USART* PeripheralHandler::GetNewUSART( unsigned long baudrate ){
	mp_USART = new USART( baudrate, &m_GPIOR0Flags, this );
	return mp_USART;
}

/*Prepares and returns a pointer to an 8-bit Timer*/
HardwareTimer_8Bit* PeripheralHandler::GetNew8BitTimer(){
	mp_8BitTimer = new HardwareTimer_8Bit();
	return mp_8BitTimer;
}

HardwareTimer_8Bit* PeripheralHandler::GetNew8BitTimer( prescaleSetting prescale ){
	mp_8BitTimer = new HardwareTimer_8Bit( prescale );
	return mp_8BitTimer;
}


TWI_Master* PeripheralHandler::GetNewTWIMaster(){
	mp_TWI_Master = new TWI_Master();
	return mp_TWI_Master;
}

void PeripheralHandler::forward_MIDI( unsigned char data ) {
	(mp_MidiDevice->*mp_MidiDeviceMemberFunction)( data );
}

void PeripheralHandler::FireRXInterrupt( unsigned char USARTSpoofData ){
	mp_USART->FireRXInterrupt( USARTSpoofData );
}

// default destructor
PeripheralHandler::~PeripheralHandler(){} //~PeripheralHandler
