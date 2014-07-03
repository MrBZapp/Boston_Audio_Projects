/* 
* USART.cpp
*
* Created: 5/15/2014 3:21:48 PM
* Author: Matt
*/

#include <avr/io.h>
#include "USART.h"
USART* USART::mp_InterruptHandler = 0;

USART::USART(unsigned long baudrate, FlagHandler* flags)
	:m_TXCallback(0)
	,m_RXCallback(0)
	,mp_FlagHandler(flags)
{
	this->SetBaudRate( baudrate );
	mp_InterruptHandler = this;
	mf_TransmitReady = mp_FlagHandler->GetNewFlag();
	mp_FlagHandler->SetFlag( mf_TransmitReady );//Set ready to transmit
	//UCSRB = (1<<RXCIE)|(1<<RXEN)|(1<<TXEN)|(1<<TXCIE); //enable interrupts
} //USART

USART::USART(unsigned long baudrate, FlagHandler* flags, PeripheralHandler* peripherals)
	:m_TXCallback(0)
	,m_RXCallback(0)
	,mp_FlagHandler(flags)
{
	this->SetBaudRate( baudrate );
	mp_PeripheralHandler = peripherals;
	mp_InterruptHandler = this;
	mf_TransmitReady = mp_FlagHandler->GetNewFlag();
	mp_FlagHandler->SetFlag( mf_TransmitReady );//Set ready to transmit
	//UCSRB = (1<<RXCIE)|(1<<RXEN)|(1<<TXEN)|(1<<TXCIE); //enable interrupts
} //USART

void USART::SetBaudRate( unsigned long baudrate){
	const long baud = ((F_CPU/(baudrate*16))-1);
	UBRRL = baud;
  #ifdef UBRRH
	UBRRH = baud>>8;
  #endif
}

void USART::EnableRX( unsigned char buffersize ){
	mp_RXBuffer = new Buffer<unsigned char>( buffersize );
	UCSRB |= (1<<RXCIE)|(1<<RXEN);
}


/*STATUS FUNCTIONS*/
bool USART::HasDataInRX(){
	return (bool) mp_RXBuffer->SpaceUsed();
}
unsigned char USART::GetByteFromRXBuffer(){
	return 0;
}

/* INTERRUPT ROUTING */

void USART::AssignCallback_TX( const CBFunctor0 DoThis ){
	m_TXCallback = DoThis;
}

void USART::AssignCallback_RX( const CBFunctor1<unsigned char> DoThis ){
	m_RXCallback = DoThis;
}

void USART::AssignHandlerFunction_USART_RX( PeripheralHandlerMemberFunction newFunction ){
	mp_RX_Interrupt_Handler = newFunction;
}

void USART::AssignFunction_USART_RX( USART_RXFunctionPtr fptr ){
	mp_RX_Interrupt_Func = fptr;
}
void USART::AssignFunction_USART_TX( USART_TXFunctionPtr fptr ){
	mp_TX_Interrupt_Func = fptr;
}

void USART_RX_vect(){
	const unsigned char data = UDR;
	if ( !USART::mp_InterruptHandler->mp_RXBuffer->AddToEnd( data ) ){
		USART::mp_InterruptHandler->m_RXOverFlow++;
	}
}

void USART::FireRXInterrupt( unsigned char USARTSpoofData){//DEBUGGING ROUTINE FOR TESTING INTERUPT HANDLING
	(mp_PeripheralHandler->*mp_RX_Interrupt_Handler)(USARTSpoofData);
}
USART::~USART() // default destructor
{
} //~USART
