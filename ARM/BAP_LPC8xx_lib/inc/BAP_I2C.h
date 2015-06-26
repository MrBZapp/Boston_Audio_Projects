/*
 * BAP_I2C.h
 *
 *  Created on: Jun 17, 2015
 *      Author: Matt
 */

#ifndef BAP_I2C_H_
#define BAP_I2C_H_

#include "chip.h"

#ifndef CORE_M0PLUS
#error CORE_M0PLUS is not defined for the LPC8xx architecture
#error CORE_M0PLUS should be defined as part of your compiler define list
#endif

#ifndef CHIP_LPC8XX
#error The LPC8XX Chip include path is used for this build, but
#error CHIP_LPC8XX is not defined!
#endif

/**
 * @brief LPC8XX I2C register block structure
 */
typedef struct {
	__IO uint32_t CFG; // Configuration for shared functions
	__IO uint32_t STAT; // Status register for master, slave, and monitor functions
	__IO uint32_t INTENSET; // Interrupt enable set and read register.
	__O  uint32_t INTENCLR; // Interrupt enable Clear register
	__IO uint32_t TIMEOUT; // Time-out value register
	__IO uint32_t DIV; // Clock pre-divider for the entire I2C block.
	__I  uint32_t INTSTAT; // Interrupt Status register for Master, Slave, and Monitor functions.
	__IO uint32_t MSTCTL; // Master Control register
	__IO uint32_t MSTTIME; // Master timing configuration
	__IO uint32_t MSTDAT; // Combined master receiver and transmitter
	__IO uint32_t SLVCTL; // Slave Control register
	__IO uint32_t SLVTIME; // Slave timing configuration
	__IO uint32_t SLVDAT; // Combined slave receiver and transmitter
	__IO uint32_t SLVADR[4]; // Slave addresses
	__IO uint32_t SLVQUAL0; // Slave Qualification for address 0.
	__O  uint32_t MONRXDAT; // Monitor receiver data register (read-only!)
} BAP_I2C_T;

// Definition of the location of the I2C registers in the LPC8XX
#define BAP_I2C_BASE (0x40050000UL)
#define BAP_I2C      ((BAP_I2C_T*) BAP_I2C_BASE)

// Configuration bits and offsets
#define BAP_I2C_MSTEN(x) (x << 0)
#define BAP_I2C_SLVEN(x) (x << 1)
#define BAP_I2C_TIMEOUTEN(x) (x << 3)
#define BAP_I2C_MONCLKSTR(x) (x << 4)

// Status masks
#define BAP_I2C_MSTPENDING (1 << 0)
#define BAP_I2C_MSTSTATE (7 << 1)
#define BAP_I2C_MSTARBLOSS (1 << 4)
#define BAP_I2C_MSTSTSTPERR (1 << 6)
#define BAP_I2C_SLVPENDING (1 << 8)
#define BAP_I2C_SLVSTATE (3 << 9)
#define BAP_I2C_SLVNOSTR (1 << 11)
#define BAP_I2C_SLVIDX (3 << 12)
#define BAP_I2C_SLVSEL (1 << 14)
#define BAP_I2C_SLVDESEL (1 << 15)
#define BAP_I2C_MONRDY (1 << 16)
#define BAP_I2C_MONNOV (1 << 17)
#define BAP_I2C_MONACTIVE (1 << 18)
#define BAP_I2C_MONIDLE (1 << 19)
#define BAP_I2C_EVENTTIMEOUT (1 << 24)
#define BAP_I2C_SCLTIMEOUT (1 << 25)

// Master function state codes (MSSTATE)
#define BAP_I2C_STATE_IDLE 0
#define BAP_I2C_STATE_SLVNACK_ADDR 3
#define BAP_I2C_STATE_SLVNACK_DATA 4

// Slave function state codes (SLVSTATE)
#define BAP_I2C_STATE_ADRRWRX 0

// General function state codes (MSSTATE/SLVSTATE)
#define BAP_I2C_STATE_RXRDY 1
#define BAP_I2C_STATE_TXRDY 2

// Interrupt flags
#define BAP_I2C_INTMSTPENDING (1 << 0)
#define BAP_I2C_INTMSTARBLOSS (1 << 4)
#define BAP_I2C_INTMSTSTSTPERR (1 << 6)
#define BAP_I2C_INTSLVPENDING (1 << 8)
#define BAP_I2C_INTSLVNOTSTR (1 << 11)
#define BAP_I2C_INTSLVDESEL (1 << 15)
#define BAP_I2C_INTMONRDY (1 << 16)
#define BAP_I2C_INTMONOV (1 << 17)
#define BAP_I2C_INTMONIDLE (1 << 19)
#define BAP_I2C_INTEVENTTIMEOUT (1 << 24)
#define BAP_I2C_INTSCLTIMEOUT (1 << 25)



// Time-out register
#define BAP_I2C_TOMIN(x) ((0xf & x) << 0)
#define BAP_I2C_TO(x) ((0xFFF & x) << 4)

// I2C clock divider register
#define BAP_I2C_DIVVAL(x) ((0xFFFF & x) << 0)

// Master Control Register
#define BAP_I2C_MSTCONTINUE (1 << 0)
#define BAP_I2C_MSTSTART (1 << 1)
#define BAP_I2C_MSTSTOP(x) ((0x1 & x) << 2)

// Slave Control Register
#define BAP_I2C_SLVCONTINUE (1 << 0)
#define BAP_I2C_SLVNACK (1 << 1)

// Master Time register
#define BAP_I2C_MSTSCLLOW(x) ((0x7 & x) << 0)
#define BAP_I2C_MSTSCLHIGH(x) ((0x7 & x) << 4)

// General Data register
#define BAP_I2C_DATA (0xF << 0)

// Slave Address register
#define BAP_I2C_SADISABLE(x) ((0x1 & x) << 0)
#define BAP_I2C_SLVADR(x) ((0xFF & x) << 1)

// Slave address qualifier 0 register
#define BAP_I2C_QUALMODE0(x) ((0x1 & x) << 0)
#define BAP_I2C_SLVQUAL0(x) ((0xFF & x) << 1)

// Monitor Data register
#define BAP_I2C_MONRESTART (1 << 0)
#define BAP_I2C_MONNACK (1 << 0)

#define I2C_MASTERCMD_HIGHSPEED 0b00001111

void BAP_I2C_MasterInit(uint8_t * start_of_tx_ram, uint32_t available_space);
ErrorCode_t BAP_I2C_MasterProcessNewMessage(I2C_PARAM_T* param);
ErrorCode_t BAP_I2C_InitiateHighSpeedMode();

#endif /* BAP_I2C_H_ */
