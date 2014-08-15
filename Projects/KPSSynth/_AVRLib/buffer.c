/*! \file buffer.c \brief Multipurpose byte buffer structure and methods. */
//*****************************************************************************
//
// File Name	: 'buffer.c'
// Title		: Multipurpose byte buffer structure and methods
// Author		: Pascal Stang - Copyright (C) 2001-2002
// Created		: 9/23/2001
// Revised		: 9/23/2001
// Version		: 1.0
// Target MCU	: any
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#include "buffer.h"

// global variables

// initialization

#ifdef USE_SMALL_BUFFER_STRUCTURE
	#define SHORTVAR char
#else
	#define SHORTVAR short
#endif
void bufferInit(cBuffer* buffer, unsigned char *start, unsigned SHORTVAR size)
{
	// begin critical section
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		// set start pointer of the buffer
		buffer->dataptr = start;
		buffer->size = size;
		// initialize index and length
		buffer->dataindex = 0;
		buffer->datalength = 0;
		// end critical section
	}
}

// access routines
unsigned char  bufferGetFromFront(cBuffer* buffer)
{
	unsigned char data = 0;
	// begin critical section
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		// check to see if there's data in the buffer
		if(buffer->datalength)
		{
			// get the first character from buffer
			data = buffer->dataptr[buffer->dataindex];
			// move index down and decrement length
			buffer->dataindex++;
			if(buffer->dataindex >= buffer->size)
			{
				buffer->dataindex -= buffer->size;
			}
			buffer->datalength--;
		}
	}//Atomic End
	// return
	return data;
}

void bufferDumpFromFront(cBuffer* buffer, unsigned SHORTVAR numbytes)
{
	// begin critical section
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		// dump numbytes from the front of the buffer
		// are we dumping less than the entire buffer?
		if(numbytes < buffer->datalength){
			// move index down by numbytes and decrement length by numbytes
			buffer->dataindex += numbytes;
			if(buffer->dataindex >= buffer->size)
			{
				buffer->dataindex -= buffer->size;
			}
			buffer->datalength -= numbytes;
		}
		else
		{
			// flush the whole buffer
			buffer->datalength = 0;
		}
	}//Atomic End
}

unsigned char bufferGetAtIndex(cBuffer* buffer, unsigned SHORTVAR index){
	// begin critical section
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		// return character at index in buffer
		unsigned char data = buffer->dataptr[(buffer->dataindex+index)%(buffer->size)];
		// end critical section
	return data;
	}
	return 0; //This is to clear a warning associated with Atomic access. If code reaches here, it's a problem.
}

bool bufferAddToEnd(cBuffer* buffer, unsigned char data)
{
	// begin critical section
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		// make sure the buffer has room
		if(buffer->datalength < buffer->size)
		{
			// save data byte at end of buffer
			buffer->dataptr[(buffer->dataindex + buffer->datalength) % buffer->size] = data;
			// increment the length
			buffer->datalength++;
			// return success
			return true;
		}
		// return failure
		return false;
	}
	return false; //This is to clear a warning associated with Atomic access. If code reaches here, it's a problem.
}

unsigned SHORTVAR bufferIsNotFull(cBuffer* buffer){
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){	
		// check to see if the buffer has room
		// return true if there is room
		unsigned SHORTVAR bytesleft = (buffer->size - buffer->datalength);
		// end critical section
		return bytesleft;
	}
	return 0; //This is to clear a warning associated with Atomic access. If code reaches here, it's a problem.
}

void inline bufferFlush(cBuffer* buffer)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		// reset the buffer
		buffer->datalength = 0;
	}
}

#undef SHORTVAR
