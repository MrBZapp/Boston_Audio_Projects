/*
===============================================================================
 Name        : KPS_VoiceClock.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#include "chip.h"
#include <cr_section_macros.h>
#include "FrequencyMaps.h"
#include "BAP_WaveGen.h"

#define UART0RX_BUFFSIZE 10 //Receive 10 values before overflowing the buffer

#define LED_LOCATION    (2)
void setMasterClockFreq(uint32_t freq);

void SCT_IRQHandler(void);
/*****************************************************************************
 * Variables																 *
 ****************************************************************************/
uint8_t uartRXBuffArr[UART0RX_BUFFSIZE];
RINGBUFF_T uartRXBuff1;

volatile int noteNumber = 0;
volatile int countdir = 1;

/********************************************************************************************************
 * 											MAIN														*
 *******************************************************************************************************/
int main(void)
{

	SystemCoreClockUpdate();

	// IO setup
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_SWM);

	Chip_Clock_SetUARTClockDiv(1);	/* divided by 1 */

	Chip_SWM_DisableFixedPin(SWM_FIXED_ACMP_I1);
	Chip_SWM_DisableFixedPin(SWM_FIXED_SWDIO);

	// Assign CTOUT0 to pin 0.
	Chip_SWM_MovablePinAssign(SWM_CTOUT_0_O, 3);
	// Assign UART0 Receive to pin 4.
	Chip_SWM_MovablePinAssign(SWM_U0_RXD_I, 0);
	Chip_SWM_MovablePinAssign(SWM_U0_TXD_O, 4);

	Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_SWM);

    LPC_GPIO_PORT->DIR[0] |= (1 << LED_LOCATION);

    //UART0 initialization
	Chip_UART_Init(LPC_USART0);
	Chip_UART_ConfigData(LPC_USART0, UART_CFG_DATALEN_8 | UART_CFG_PARITY_NONE | UART_CFG_STOPLEN_1);
	Chip_UART_SetBaud(LPC_USART0, 115200);
	LPC_USART0->CFG |= UART_CFG_ENABLE;

	WaveGenInit(&Generator1, MIDItoFreq[0]);

	/* Unhalt the counter to start */
	LPC_SCT->CTRL_U &= ~(1 << 2);


	uint8_t status = 48;
	uint8_t value = 0;
/////////////////////////////////////////////MAINLOOP.////////////////////////////////////////////////////

	while (1) {
		if (Chip_UART_Read(LPC_USART0, &value, 1))
		{
			if ( value == 0xF1 || value == 0xF2 )
			{
				LPC_GPIO_PORT->NOT[0] = LED_LOCATION;
				status = value & 0x0F;
			}
			else
			{
				switch (status){
				case(0x01):
						setFreq(&Generator1, MIDItoBBD[(value % 127)]);
						LPC_GPIO_PORT->CLR[0] = LED_LOCATION;
						break;
				case(0x02):
						LPC_GPIO_PORT->SET[0] = LED_LOCATION;
						setWidth(&Generator1, value);
						break;
				default:
					LPC_GPIO_PORT->NOT[0] = LED_LOCATION;
					break;
				}
			}
			Chip_UART_SendByte(LPC_USART0, status);
			Chip_UART_SendByte(LPC_USART0, value);
		}
	}
	return 0;
}

/********************************************************************************************************
 * 											FUNCTIONS													*
 *******************************************************************************************************/

void SCT_IRQHandler(void)
{
	Chip_SCT_ClearEventFlag(LPC_SCT, SCT_EVT_0);
}



void setMasterClockFreq(uint32_t freq)
{
	LPC_PMU->PCON;

}
