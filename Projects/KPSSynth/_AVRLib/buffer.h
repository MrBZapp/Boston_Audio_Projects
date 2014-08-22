/*! \file buffer.h \brief Multipurpose byte buffer structure and methods. */
//*****************************************************************************
//
// File Name	: 'buffer.h'
// Title		: Multipurpose byte buffer structure and methods
// Author		: Pascal Stang - Copyright (C) 2001-2002
// Created		: 9/23/2001
// Revised		: 11/16/2002
// Version		: 1.1
// Target MCU	: any
// Editor Tabs	: 4
//
///	\ingroup general
/// \defgroup buffer Circular Byte-Buffer Structure and Function Library (buffer.c)
/// \code #include "buffer.h" \endcode
/// \par Overview
///		This byte-buffer structure provides an easy and efficient way to store
///		and process a stream of bytes.  You can create as many buffers as you
///		like (within memory limits), and then use this common set of functions to
///		access each buffer.  The buffers are designed for FIFO operation (first
///		in, first out).  This means that the first byte you put in the buffer
///		will be the first one you get when you read out the buffer.  Supported
///		functions include buffer initialize, get byte from front of buffer, add
///		byte to end of buffer, check if buffer is full, and flush buffer.  The
///		buffer uses a circular design so no copying of data is ever necessary.
///		This buffer is not dynamically allocated, it has a user-defined fixed
///		maximum size.  This buffer is used in many places in the avrlib code.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************
//@{

#ifndef BUFFER_H
#define BUFFER_H
#include <avr/io.h>
#include <stdbool.h>
#include <util/atomic.h>
#include "global.h"
// structure/typdefs

//! cBuffer structure
#ifdef USE_SMALL_BUFFER_STRUCTURE
	#define SHORTVAR char
	typedef struct struct_cBuffer
	{
		unsigned char *dataptr;			///< the physical memory address where the buffer is stored
		unsigned char size;			///< the allocated size of the buffer
		unsigned char datalength;		///< the length of the data currently in the buffer
		unsigned char dataindex;		///< the index into the buffer where the data starts
	} cBuffer;
#else
	#define SHORTVAR short
	typedef struct struct_cBuffer
	{
		unsigned char *dataptr;			///< the physical memory address where the buffer is stored
		unsigned SHORTVAR size;			///< the allocated size of the buffer
		unsigned SHORTVAR datalength;		///< the length of the data currently in the buffer
		unsigned SHORTVAR dataindex;		///< the index into the buffer where the data starts
	} cBuffer;
#endif

// function prototypes

//! initialize a buffer to start at a given address and have given size
void	buffer_Init(cBuffer* buffer, unsigned char *start, unsigned SHORTVAR size);

//! get the first byte from the front of the buffer
unsigned char	buffer_GetFromFront(cBuffer* buffer);

//! dump (discard) the first numbytes from the front of the buffer
void buffer_DumpFromFront(cBuffer* buffer, unsigned SHORTVAR numbytes);

//! get a byte at the specified index in the buffer (kind of like array access)
// ** note: this does not remove the byte that was read from the buffer
unsigned char	buffer_GetAtIndex(cBuffer* buffer, unsigned SHORTVAR index);

//! add a byte to the end of the buffer
bool			buffer_AddToEnd(cBuffer* buffer, unsigned char data);

//! check if the buffer is full/not full (returns zero value if full)
unsigned SHORTVAR	buffer_BytesLeft(cBuffer* buffer);

//! flush (clear) the contents of the buffer
void			buffer_Flush(cBuffer* buffer);

#endif
//@}
