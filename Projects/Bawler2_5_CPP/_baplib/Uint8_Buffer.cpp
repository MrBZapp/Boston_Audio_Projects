/*
 * Buffer.cpp
 *
 * Created: 5/10/2014 8:20:59 PM
 *  Author: Matt
 */ 

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
/*
#include "Uint8_buffer.h"
#include "global.h"
#include "avr/io.h"

// global variables

// constructor
template <class T>
Buffer<T>::Buffer( unsigned char size )
	: m_buffer (new TemplateBuffer<T>) //create the buffer object
	{
		CRITICAL_SECTION_START;
		m_buffer->data = new T[size]; // create the internal array
		m_buffer->size = size;
		m_buffer->index = 0;
		m_buffer->length = 0;
		CRITICAL_SECTION_END;
	
	};

// access routines

template <class T>
T Buffer<T>::GetFromFront(){
	unsigned char data = 0;
	CRITICAL_SECTION_START;
	if(m_buffer->length){ // check to see if there's data in the buffer
		data = m_buffer->data[m_buffer->index]; // get the first character from buffer
		m_buffer->index++; // move index down and decrement length
		if(m_buffer->index >= m_buffer->size){
			m_buffer->index -= m_buffer->size;
		}
		m_buffer->length--;
	}
	// end critical section
	CRITICAL_SECTION_END;
	// return
	return data;
}


template <class T>
void Buffer<T>::DiscardFromFront(unsigned short numbytes){
	CRITICAL_SECTION_START;
	if(numbytes < m_buffer->length){ // are we dumping less than the entire buffer?
		m_buffer->index += numbytes; // move index down by numbytes and decrement length by numbytes
		if(m_buffer->index >= m_buffer->size){
			m_buffer->index -= m_buffer->size;
		}
		m_buffer->length -= numbytes;
	}else{
		this->Flush();
	}
	CRITICAL_SECTION_END;
}

template <class T>
T Buffer<T>::GetAtIndex(unsigned char index){	// return character at index in buffer
	CRITICAL_SECTION_START;
	unsigned char data = m_buffer->data[(m_buffer->index+index)%(m_buffer->size)];
	CRITICAL_SECTION_END;
	return data;
}


template <class T>
bool Buffer<T>::AddToEnd(unsigned char newData){
	CRITICAL_SECTION_START;
	if(m_buffer->length < m_buffer->size){ // make sure the buffer has room
		m_buffer->data[(m_buffer->index + m_buffer->length) % m_buffer->size] = newData; //Add the data
		m_buffer->length++; // Update the length
		CRITICAL_SECTION_END;
		return true;
	}
	// end critical section
	CRITICAL_SECTION_END;
	// return failure
	return false;
}

template <class T>
unsigned char Buffer<T>::SpaceRemaining(){
	CRITICAL_SECTION_START;
	unsigned char bytesleft = m_buffer->size - m_buffer->length;
	CRITICAL_SECTION_END;
	return bytesleft;
}

template <class T>
unsigned char Buffer<T>::SpaceUsed(){
	return m_buffer->length;
}


template <class T>
void Buffer<T>::Flush(){
	CRITICAL_SECTION_START;
	m_buffer->length = 0;
	CRITICAL_SECTION_END;
}


template <class T>
Buffer<T>::~Buffer(){
	delete[] m_buffer->data;
	delete m_buffer;
}*/