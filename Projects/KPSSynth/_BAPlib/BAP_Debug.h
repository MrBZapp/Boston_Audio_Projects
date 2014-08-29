/*
 * BAP_Debug.h
 *
 * Created: 8/19/2014 4:08:35 PM
 *  Author: Matt Zapp
 */ 


#ifndef BAP_DEBUG_H_
#define BAP_DEBUG_H_
#include <avr/io.h>

#ifdef DIAGNOSTICS
//Diagnostic compiled code
#define DIAGNOSTIC_TOGGLE(port, pin) port ^= (1<<pin)

//Diagnostic linker messages
#ifdef USE_SMALL_BUFFER_STRUCTURE
	#pragma message "Using small buffers."
#endif

#ifdef ENABLE_UART0_RX
	#pragma message "Using UART0 RX"
#endif

#ifdef SPI_USE_TIMER0
	#pragma  message "SPI clock driven from TIMER0"
#endif

#else
#define DIAGNOSTIC_TOGGLE

#endif

#endif /* BAP_DEBUG_H_ */