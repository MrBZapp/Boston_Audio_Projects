/*
 * BAP_Debug.h
 *
 *  Created on: Mar 29, 2015
 *      Author: Matt
 */

#ifndef BAP_DEBUG_H_
#define BAP_DEBUG_H_

#include "chip.h"

#define DEBUG_SEND(x) if ((LPC_USART0->STAT & UART_STAT_TXRDY)){LPC_USART0->TXDATA = (uint32_t) x;}
#endif /* BAP_DEBUG_H_ */
