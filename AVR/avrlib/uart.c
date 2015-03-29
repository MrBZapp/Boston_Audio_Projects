/*! \file uart.c \brief UART driver with buffer support. */
// *****************************************************************************
//
// File Name	: 'uart.c'
// Title		: UART driver with buffer support
// Author		: Pascal Stang - Copyright (C) 2000-2002
// Created		: 11/22/2000
// Revised		: 06/09/2003
// Version		: 1.3
// Target MCU	: ATMEL AVR Series
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
// *****************************************************************************

#include <avr/io.h>
#include <avr/interrupt.h>
#include "buffer.h"
#include "uart.h"

// UART global variables
// flag variables
#ifdef ENABLE_UART0_TX
volatile bool  uartReadyTx;			///< uartReadyTx flag
volatile bool  uartBufferedTx;		///< uartBufferedTx flag 
#endif

// receive and transmit buffers
#ifdef ENABLE_UART0_RX
cBuffer uartRxBuffer;				///< uart receive buffer
#endif
#ifdef ENABLE_UART0_TX
cBuffer uartTxBuffer;				///< uart transmit buffer
#endif

unsigned short uartRxOverflow;		///< receive overflow counter

#ifndef UART_BUFFERS_EXTERNAL_RAM
	// using internal ram,
	// automatically allocate space in ram for each buffer
	#ifdef ENABLE_UART0_RX
		static unsigned char uartRxData[UART_RX_BUFFER_SIZE];
	#endif
	#ifdef ENABLE_UART0_TX
		static unsigned char uartTxData[UART_TX_BUFFER_SIZE];
	#endif
#endif

typedef void (*voidFuncPtru08)(unsigned char);
typedef bool (*boolFuncPtru08)(unsigned char);

#ifdef ENABLE_UART0_RX
volatile static voidFuncPtru08 UartRxFunc;
#endif

// enable and initialize the uart
void uartInit(void)
{
	// initialize the buffers
	uartInitBuffers();
	// initialize user receive handler
	#ifdef ENABLE_UART0_RX
		UartRxFunc = 0;
	#endif
	// set frame size, stop bits 
		UCSRC = ((0<<USBS) | (3<<UCSZ0));
	// enable RxD/TxD and interrupts if function is enabled	
	#ifdef ENABLE_UART0_TX
		UCSRB = ((1<<TXCIE)|(1<<TXEN));
	#endif

	#ifdef ENABLE_UART0_RX
		UCSRB = ((1<<RXCIE)|(1<<RXEN));
	#endif

	// set default baud rate
	uartSetBaudRate(UART_DEFAULT_BAUD_RATE);  
	
	#ifdef ENABLE_UART0_TX
	// initialize transmit states
	uartReadyTx = true;
	uartBufferedTx = true;
	#endif
	
	#ifdef ENABLE_UART0_RX
	// clear overflow count
	uartRxOverflow = 0;
	#endif
	// enable interrupts
	//sei();	
}

void uartInitBuffers(void){ // create and initialize the uart transmit and receive buffers
	#ifndef UART_BUFFERS_EXTERNAL_RAM
		// initialize the UART receive buffer
		#ifdef ENABLE_UART0_RX
			buffer_Init(&uartRxBuffer, uartRxData, UART_RX_BUFFER_SIZE);
		#endif
		#ifdef ENABLE_UART0_TX
		// initialize the UART transmit buffer
			buffer_Init(&uartTxBuffer, uartTxData, UART_TX_BUFFER_SIZE);
		#endif
	#else
		// initialize the UART receive buffer if enabled
		#ifdef ENABLE_UART0_RX
			buffer_Init(&uartRxBuffer, (unsigned char*) UART_RX_BUFFER_ADDR, UART_RX_BUFFER_SIZE);
		#endif
		
		// initialize the UART transmit buffer if enabled
		#ifdef ENABLE_UART0_TX
			buffer_Init(&uartTxBuffer, (unsigned char*) UART_TX_BUFFER_ADDR, UART_TX_BUFFER_SIZE);
		#endif
	#endif
}

#ifdef ENABLE_UART0_RX
// redirects received data to a user function
void uartSetRxHandler(void (*rx_func)(unsigned char c)){
	// set the receive interrupt to run the supplied user function
 	UartRxFunc = rx_func;
}
#endif

// set the uart baud rate
void uartSetBaudRate(unsigned long baudrate){
	// calculate division factor for requested baud rate, and set it
	unsigned int bauddiv = ((F_CPU+(baudrate*8L))/(baudrate*16L)-1);
	UBRRL = bauddiv;
	#ifdef UBRRH
		UBRRH = (bauddiv>>8);
	#endif
}

#ifdef ENABLE_UART0_RX
// returns the receive buffer structure 
cBuffer* uartGetRxBuffer(void){
	// return rx buffer pointer
	return &uartRxBuffer;
}
#endif

#ifdef ENABLE_UART0_TX
// returns the transmit buffer structure 
cBuffer* uartGetTxBuffer(void){
	// return tx buffer pointer
	return &uartTxBuffer;
}
#endif

#ifdef ENABLE_UART0_TX
// transmits a byte over the uart
void uartSendByte(unsigned char txData){
	// wait for the transmitter to be ready
	while(!uartReadyTx){};
	// send byte
	outb(UDR, txData);
	// set ready state to false
	uartReadyTx = false;
}
#endif

// gets a single byte from the uart receive buffer (getchar-style)
int uartGetByte(void){
	unsigned char c;
	if(uartReceiveByte(&c)){
		return c;
	}else{
		return -1;
	}
}

#ifdef ENABLE_UART0_RX
// gets a byte (if available) from the uart receive buffer
bool uartReceiveByte(unsigned char* rxData){
	// make sure we have a receive buffer
	if(uartRxBuffer.size){
		// make sure we have data
		if(uartRxBuffer.datalength){
			// get byte from beginning of buffer
			*rxData = buffer_GetFromFront(&uartRxBuffer);
			return true;
		}else{
			// no data
			return false;
		}
	}else{
		// no buffer
		return false;
	}
}

// flush all data out of the receive buffer
void uartFlushReceiveBuffer(void)
{
	// flush all data from receive buffer
	//bufferFlush(&uartRxBuffer);
	// same effect as above
	uartRxBuffer.datalength = 0;
}

// return true if uart receive buffer is empty
bool uartReceiveBufferIsEmpty(void){
	if(uartRxBuffer.datalength == 0){
		return true;
	}else{
		return false;
	}
}
#endif

#ifdef ENABLE_UART0_TX
// add byte to end of uart Tx buffer
bool uartAddToTxBuffer(unsigned char data){
	// add data byte to the end of the tx buffer
	return buffer_AddToEnd(&uartTxBuffer, data);
}

// start transmission of the current uart Tx buffer contents
void uartSendTxBuffer(void){
	// turn on buffered transmit
	uartBufferedTx = true;
	// send the first byte to get things going by interrupts
	uartSendByte(buffer_GetFromFront(&uartTxBuffer));
}
/*
// transmit nBytes from buffer out the uart
unsigned char uartSendBuffer(char *buffer, unsigned int nBytes)
{
	register unsigned char first;
	register unsigned int i;

	// check if there's space (and that we have any bytes to send at all)
	if((uartTxBuffer.datalength + nBytes < uartTxBuffer.size) && nBytes)
	{
		// grab first character
		first = *buffer++;
		// copy user buffer to uart transmit buffer
		for(i = 0; i < nBytes-1; i++)
		{
			// put data bytes at end of buffer
			bufferAddToEnd(&uartTxBuffer, *buffer++);
		}

		// send the first byte to get things going by interrupts
		uartBufferedTx = true;
		uartSendByte(first);
		// return success
		return true;
	}
	else
	{
		// return failure
		return false;
	}
}
*/


// UART Transmit Complete Interrupt Handler
ISR(USART0_TX_vect){
	// check if buffered tx is enabled
	if(uartBufferedTx){
		// check if there's data left in the buffer
		if(uartTxBuffer.datalength){
			// send byte from top of buffer
			outb(UDR, buffer_GetFromFront(&uartTxBuffer));
		}else{
			// no data left
			uartBufferedTx = false;
			// return to ready state
			uartReadyTx = true;
		}
	}else{
		// we're using single-byte tx mode
		// indicate transmit complete, back to ready
		uartReadyTx = true;
	}
}
#endif

#ifdef ENABLE_UART0_RX
// UART Receive Complete Interrupt Handler
ISR(USART0_RX_vect)
{
	unsigned char charRX;
	
	// get received char
	charRX = UDR;

	// if there's a user function to handle this receive event
	if(UartRxFunc){
		UartRxFunc(charRX);// call it and pass the received data.  if it fails, put the data into the buffer
	}else{
		// otherwise do default processing
		// put received char in buffer
		// check if there's space
		if( !buffer_AddToEnd(&uartRxBuffer, charRX) ){
			// no space in buffer
			// count overflow
			uartRxOverflow++;
		}
	}
}
#endif