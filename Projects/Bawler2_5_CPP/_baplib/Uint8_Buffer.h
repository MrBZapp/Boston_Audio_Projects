/*
 * Uint8_Buffer.h, Created: 5/10/2014 8:21:18 PM,  Author: Matt Zapp
 *  
 *  Based on Pascal Stang's AVRlib buffer, this buffer includes all the same functionality, but 
 *  is expanded to allow dynamic allocation of a buffer, and eliminates the need for the ugly 
 *  C macros that required a developer know what buffers they would need and how large 
 *  ahead of time.
 *  
 *  This particular buffer implementation is templated and should work with any type of data,
 *  although it is recommended that types be kept small as to prevent the buffer from balooning
 *  to preposterous sizes.
 *  
 *  From Pascal's original documentation:
 *		This byte-buffer structure provides an easy and efficient way to store
 *		and process a stream of bytes.  You can create as many buffers as you 
 *		like (within memory limits), and then use this common set of functions to
 *		access each buffer.  The buffers are designed for FIFO operation (first
 *		in, first out). This means that the first byte you put in the buffer
 *		will be the first one you get when you read out the buffer.  Supported
 *		functions include buffer initialize, get byte from front of buffer, add
 *		byte to end of buffer, check if buffer is full, and flush buffer.  The
 *		buffer uses a circular design so no copying of data is ever necessary.
 *	
 *	ADDITIONAL NOTES:
 *		-->	All functions pause interrupts during their operation and any interrupts that 
 *			occur during a R/W operation to the buffer will be delayed until after the function 
 *			has completed and before it has returned.  This is to ensure that you never read 
 *			out-of-date information if you have a buffer that is accessed as part of an interrupt. 
 *		-->	The buffer is, at its core, an array, so indexing works from a 0 basis.
 *		-->	This particular buffer implementation only works with bytes.  Larger structures 
 *			need to be written to the buffer byte-by-byte.
 *		-->	current maximum size is 256 bytes. This could be expanded in the future by 
 *			expanding the char data types to shorts, but consideration must be given for 
 *			AVR's data memory size.
 *		-->	If you dynamically allocate a buffer that cannot fit in static ram, the buffer 
 *			creation will fail silently.  I leave it to you the developer to be wise about memory 
 *			management how, and be aware that if your buffers are not acting in a way 
 *			you expect that you first consider your allocations.
 *			
 */ 


		/*Buffer Unit Test
		static unsigned char someNumber = 2;
		 while ( data.SpaceRemaining() != 0 ) 
		{
		data.AddToEnd( someNumber );
		someNumber++;
		};
		data.AddToEnd( PORTD );//what happens when I overwrite the array?
		PORTB = data.ReadIndex( 5 );
		PORTB = data.FindFirstInstance( 6 );
		GPIOR0 = data.PopFromIndex( 4 );
		data.OverwriteAtIndex( 0x30 , 9 );
		data.PushToIndex( someNumber , 4 );
		PORTD = data.ReadIndex( 4 );*/
		

#ifndef UINT_8_BUFFER_H_
#define UINT_8_BUFFER_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

/*
#ifndef CRITICAL_SECTION_START
#define CRITICAL_SECTION_START	unsigned char _sreg = SREG; cli()
#define CRITICAL_SECTION_END	SREG = _sreg
#endif
*/

template <typename T>
struct TemplateBuffer{
	T* data;			///< the physical memory address where the buffer is stored
	unsigned char  size;		// the allocated size of the buffer
	unsigned char  length;		// amount of the data currently in the buffer.  when used as a pointer, points to the next free space.
};

template <class T>	
class Buffer{
	public:
		Buffer( unsigned char specSize )
			: m_buffer (new TemplateBuffer<T>)
		{
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
				m_buffer->data = new T[specSize]; // create the internal array
				m_buffer->size = specSize; // set the size of the buffer to the specified
				m_buffer->length = 0;
			}
		}; // create a buffer of "size" length

/////////Write Methods/////////
		bool AddToEnd( T newData ){
			if(m_buffer->length < m_buffer->size){ // make sure the buffer has room
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
					m_buffer->data[m_buffer->length % m_buffer->size] = newData; //Add the data to the address at which length points.
				}
				m_buffer->length++; // Update the length by one
				return true;
			} else {
				return false;//return failure
			}
		}; // add a byte to the end of the buffer
		
		void OverwriteAtIndex( T newData, unsigned char index ){
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
				m_buffer->data[ (m_buffer->DataEnd+index)%(m_buffer->size) ] = newData;
			}
		};//Write data at a specific index
		
		
	/* Places data at the specified index and pushes back all other data.  returns overflowed value.
	 * WARNING, WIP: right now there are no protections for an out-of-bounds read.  writing to 
	 * beyond the edge of the buffer will overwrite internal data and return unexpected results.*/
		T PushToIndex( T newData, unsigned char index){
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
				for( unsigned char i = index; i < (m_buffer->length-1); i++ ){
					T tempData = m_buffer->data[ (i%m_buffer->size) ];
					this->OverwriteAtIndex( newData, i );
					newData = tempData;
				}
			}
			return newData;
		};
		
/////////Read Methods/////////
		T PopFromFront(){// get the first byte from the front of the buffer //THIS IS SUPER BROKEN
			T data = 0;
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
				if(m_buffer->length){ // check to see if there's data in the buffer
					data = m_buffer->data[0]; // get the 0 value from buffer
					m_buffer->length--;//decrement the length
				}
				return data;
			}// end critical section
		};	
		
		// get a byte at the specified index in the buffer (kind of like array access) ** note: this does not remove the byte that was read from the buffer
		T ReadIndex( unsigned char index ){
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
				/*the return is the buffer's data array member*/
				T data = m_buffer->data[(index % (m_buffer->size))];
				return data;
			}
		};
		
		// get the address of the object stored at the location of the index
		T* ReadAddressOfIndex( unsigned char index ){
			return &m_buffer->data[index % (m_buffer->size)];
		};
		
		/* removes and returns the item at the specified index.  Back-fills with 0s.
		 * WARNING, WIP: right now there are no protections for an out-of-bounds read.  Reading from
		 * beyond the edge of the buffer will return data internal to the buffer and clear the final entry
		 * which could result in data loss.*/
		T PopFromIndex( unsigned char index ){
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
				T returnData = this->ReadIndex( index );
				for( unsigned char i = index; i < (m_buffer->length-1) ; i++ ){
					T tempData = this->ReadIndex( i + 1 );
					this->OverwriteAtIndex( tempData, i );
				}
				this->OverwriteAtIndex( 0, m_buffer->length-1 );//clear final list item
				return returnData;
			}
		};
		
/////////Query Methods/////////
		unsigned char FindFirstInstance( T searchValue ){ // Locate the first time a value is found in a reverse search of the buffer.
			T tempValue = 0;
			for ( unsigned char i = m_buffer->length; i > 0; i--){
				tempValue = this->ReadIndex(i);
				if ( tempValue == searchValue ){
					return i;
				}
			}
			return 0;
		};
		unsigned char SpaceRemaining(){// returns the size of the unused space in the buffer.  Can cast to bool to return false (0) when full.
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
				unsigned char bytesleft = m_buffer->size - m_buffer->length;
				return bytesleft;
			}
		}
		
		inline unsigned char SpaceUsed(){//returns the size of the used space in the buffer.  Can cast to bool to return false (0) when empty.
			return m_buffer->length;
		}
	  
/////////Management Methods/////////
		void DiscardFromFront(unsigned short nEntries){ // Discard the first N bytes from the front of the buffer.
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
				if(nEntries < m_buffer->length){ // are we dumping less than the entire buffer?
					m_buffer->DataEnd += nEntries; // move index down by n and decrement length by n
					if(m_buffer->DataEnd >= m_buffer->size){
						m_buffer->DataEnd -= m_buffer->size;
					}
					m_buffer->length -= nEntries;
					}else{
					this->Flush();
				}
			}
		};
		
		
		void Flush(){ //! flush (clear) the contents of the buffer WIP
			m_buffer->length = 0;
		};
		
		~Buffer(){ //destructor
			delete[] m_buffer->data;
			delete m_buffer;
		};
	private:
		TemplateBuffer<T>* m_buffer;
	};//END OF BUFFER CLASS



#endif /* UINT_8_BUFFER_H_ */
