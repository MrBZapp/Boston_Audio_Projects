/*
 * BAP_Midi.c
 *
 *  Created on: Jan 22, 2015
 *      Author: Matt
 */

#include "BAP_Midi.h"

//#include "BAP_Debug.h"

#define MIDI_RX_ENABLE

#ifdef MIDI_RX_ENABLE
	// Midi Input buffer
	#define MIDI_RXBUFFERSIZE 10
	static uint8_t MIDI_RxData[MIDI_RXBUFFERSIZE];
	static RINGBUFF_T MIDI_RxBuffer;
#endif

#ifdef MIDI_TX_ENABLE
	// Midi Output buffer
	#define MIDI_TXBUFFERSIZE 10
	static uint8_t MIDI_TxData[MIDI_TXBUFFERSIZE];
	static RINGBUFF_T MIDI_TxBuffer;
#endif


typedef struct MIDI_Status{
	uint8_t Status;
	uint8_t Addressed;
	uint8_t DataCount;
	uint8_t DB1;
}MIDI_Status_t;

// Static Global Variables
uint8_t MIDI_Address = 0;
static MIDI_Status_t MIDI_Status0 = {MIDI_IDLE, 0, 0, 0};

// Midi Command Function Pointers
MidiFuncPtr MIDI_NoteOnFunc = 0;
MidiFuncPtr MIDI_NoteOffFunc = 0;
MidiFuncPtr MIDI_PCChFunc = 0;
MidiFuncPtr MIDI_PitchFunc = 0;

// Midi Sync function pointers
VoidFuncPointer MIDI_Sync_StartFunc = 0;
VoidFuncPointer MIDI_Sync_StopFunc = 0;
VoidFuncPointer MIDI_Sync_ClkFunc = 0;
VoidFuncPointer MIDI_Sync_ContFunc = 0;

static void MIDI_ClearStatus0()
{
	// Doing it this way, we'll get a build warning if we ever need to expand MIDI_Status_t
	MIDI_Status_t temp = {MIDI_IDLE, 0, 0, 0};
	MIDI_Status0 = temp;
}



///////////////////////////////////////////////////////////////////////////////////////////

/***
 * Midi initialize for USART (1 or 2)
 */
void MIDI_USARTInit(LPC_USART_T* USARTNumber, int RXTX_Enable)
{
	/***
	 * Global UART initializations
	 **/
	// Clocking directly from the master clock
	Chip_Clock_SetUARTClockDiv(1);

	// configure the frame format
	Chip_UART_ConfigData(USARTNumber, UART_CFG_DATALEN_8 | UART_CFG_PARITY_NONE | UART_CFG_STOPLEN_1);

	// Set the baud rate to 31250
	Chip_UART_SetBaud(USARTNumber, 31250);

	/***
	 *  Initialize the RX and TX specific functions
	 **/
#ifdef MIDI_RX_ENABLE
		// Initialize the Receive buffer
		RingBuffer_Init(&MIDI_RxBuffer, &MIDI_RxData, sizeof(uint8_t), MIDI_RXBUFFERSIZE);
		// Enable the receive interrupt
		Chip_UART_IntEnable(USARTNumber, UART_INTEN_RXRDY);
#endif
#ifdef MIDI_TX_ENABLE
		// Initialize the Transmit buffer
		RingBuffer_Init(&MIDI_TxBuffer, &MIDI_TxData, sizeof(uint8_t), MIDI_TXBUFFERSIZE);
		// Enable the transmit interrupt
		Chip_UART_IntEnable(USARTNumber, UART_INTEN_TXRDY);
#endif

	MIDI_ClearStatus0();
	NVIC_EnableIRQ(UART0_IRQn);
}


/***
 * Sets the address of the current receiver
 **/
void inline MIDI_SetAddress(uint8_t addr)
{
	MIDI_Address = addr;
}

/***
 * Enable the USART previously initialized to receive MIDI data
 */

void inline MIDI_Enable(LPC_USART_T* USARTNumber)
{
	// Enable the UART to start receiving messages
	USARTNumber->CFG |= UART_CFG_ENABLE;
}


/***
 * counts sync messages handled, fires appropriate high-priority functions
 */
int MIDI_SyncHandler(uint8_t msg)
{
	int handled = 0;
	if (msg == MIDI_SYNC_CLK) {
		if (MIDI_Sync_ClkFunc != 0)
		{
			MIDI_Sync_ClkFunc();
		}
		handled = 1;
	}

	else if (msg == MIDI_SYNC_START) {
		if (MIDI_Sync_StartFunc != 0)
		{
			MIDI_Sync_StartFunc();
		}

		handled = 1;
	}

	else if (msg == MIDI_SYNC_STOP) {
		if (MIDI_Sync_StopFunc != 0)
		{
			MIDI_Sync_StopFunc();
		}

		handled = 1;
	}

	else if (msg == MIDI_SYNC_CONT) {
		if (MIDI_Sync_ContFunc != 0)
		{
			MIDI_Sync_ContFunc();
		}
		handled = 1;
	}
	return handled;
}


static void MIDI_ByteProcessor(uint8_t byte)
{
	// If it's the first byte, store it.
	if ((MIDI_Status0.DataCount % 2))
		MIDI_Status0.DB1 = byte;
	// else, send it to a function
	else
	{
		switch(MIDI_Status0.Status)
		{
		case MIDI_NOTEON:
			if ( MIDI_NoteOnFunc != 0)
			{
				MIDI_NoteOnFunc(MIDI_Status0.DB1, byte);
			}
			break;
		case MIDI_NOTEOFF:
			if ( MIDI_NoteOffFunc != 0)
			{
				MIDI_NoteOffFunc(MIDI_Status0.DB1, byte);
			}
			break;
		default:
			break;
		}
	}
}


void MIDI_ProcessRXBuffer()
{
	uint8_t byte = 0;
	// Pop a value off the buffer

	if(Chip_UART_ReadRB(LPC_USART0, &MIDI_RxBuffer, &byte, 1) != 0)
	{
		// Is it a status bit
		if((byte & MIDI_STATBIT))
		{
			// Start of new message, clear the status
			MIDI_ClearStatus0();
			MIDI_Status0.Status = (uint8_t) byte;

			// Are we being addressed?
			if(((byte & MIDI_CHMSK) == MIDI_Address) || MIDI_Address == 0x00)
			{
				MIDI_Status0.Addressed = 1;
			}
		}
		else if (MIDI_Status0.Addressed)
		{
			MIDI_Status0.DataCount++;
			MIDI_ByteProcessor(byte);
		}

		// Process the next byte
		MIDI_ProcessRXBuffer();
	}
}


void UART0_IRQHandler(void)
{

	// Insert value into RB
	while ((Chip_UART_GetStatus(LPC_USART0) & UART_STAT_RXRDY) != 0) {
		uint8_t ch = Chip_UART_ReadByte(LPC_USART0);
		// Filter Sync messages, these are high-priority
		if (!MIDI_SyncHandler(ch)) {
			RingBuffer_Insert(&MIDI_RxBuffer, &ch);
		}
	}
}


uint8_t MIDI_GetStatus_0()
{
	return MIDI_Status0.Status;
}
