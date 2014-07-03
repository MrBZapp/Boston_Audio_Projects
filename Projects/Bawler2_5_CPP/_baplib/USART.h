/* 
* USART.h
*
* Created: 5/15/2014 3:21:48 PM
* Author: Matt
*/


#ifndef __USART_H__
#define __USART_H__

#include <avr/io.h>
#include "callback.h"
#include "PeripheralHandler.h"
#include "Uint8_Buffer.h"
#include "FlagHandler.h"
#include "global.h"

class PeripheralHandler;
class USART;

enum usartHalf { RX_Only, TX_Only, RX_and_TX };

extern "C" void USART_RX_vect(void) __attribute__ ((signal)); //required for interrupt handling in C++

typedef void (*USART_RXFunctionPtr)( unsigned char byte );
typedef void (*USART_TXFunctionPtr)();

class USART {
	//variables
	public:
		static USART* mp_InterruptHandler; //To allow C ISR functions to access an instance of the class.
	protected:
	private:
		unsigned char m_RXOverFlow;
		unsigned char mf_TransmitReady;
		Buffer<unsigned char>* mp_RXBuffer;
		Buffer<unsigned char>* mp_TXBuffer;
		volatile USART_RXFunctionPtr mp_RX_Interrupt_Func;
		PeripheralHandlerMemberFunction mp_RX_Interrupt_Handler;
		volatile USART_TXFunctionPtr mp_TX_Interrupt_Func;

	//functions
	public:
		USART(unsigned long baudrate, FlagHandler* flags);
		USART(unsigned long baudrate, FlagHandler* flags, PeripheralHandler* peripherals);
	  
	  //Initialization Routines
		void SetBaudRate( unsigned long baudrate );
		void EnableRX( unsigned char buffersize );
		void EnableTX( unsigned char buffersize );
		
	  
	  //Interrupt Routing
		void AssignCallback_TX( const CBFunctor0 DoThis );
		void AssignCallback_RX( const CBFunctor1<unsigned char> DoThis );
		void AssignHandlerFunction_USART_TX( PeripheralHandlerMemberFunction newFunction );
		void AssignHandlerFunction_USART_RX( PeripheralHandlerMemberFunction newFunction );
		void AssignFunction_USART_RX( USART_RXFunctionPtr fptr );
		void AssignFunction_USART_TX( USART_TXFunctionPtr fptr );
	
		bool HasDataInRX();
		unsigned char GetByteFromRXBuffer();
		void FireRXInterrupt( unsigned char USARTSpoofBuffer );
	
		friend void USART_RX_vect(); //Attach point for the USART_RX Interrupt vector
		~USART();
	protected:
	private:
		CBFunctor0 m_TXCallback;
		CBFunctor1<unsigned char> m_RXCallback;	
		PeripheralHandler* mp_PeripheralHandler;
		FlagHandler* mp_FlagHandler;
		USART( const USART &c );
		USART& operator=( const USART &c );
}; //USART

#endif //__USART_H__
