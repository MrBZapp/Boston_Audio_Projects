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
#define DIAGNOSTIC_TOGGLE(port, pin) port ^= (1<<pin)

#else
#define DIAGNOSTIC_TOGGLE

#endif

#endif /* BAP_DEBUG_H_ */