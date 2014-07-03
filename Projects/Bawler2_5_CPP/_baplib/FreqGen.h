/*
 * FreqGen.h
 *
 * Created: 4/20/2014 8:42:16 PM
 *  Author: Matt
 */ 


#ifndef FREQGEN_H_
#define FREQGEN_H_
#include <avr/io.h>
#include <math.h>
#include "HardwareTimer.h"

#define OCA 0x00
#define OCB 0x01
#define OUT_COMPARE_MASK 0xF0

class Generator_8_Bit {
	public:
		//8_Bit_Generator( uint8_t* _outputRegister ); //Constructor without initial frequency
		Generator_8_Bit( unsigned char output, float frequency ); //Constructor with initial frequency
		
		//sets output compare registers to match frequency
		void SetFrequency( float frequency ); 
		
		//Gets the value of the output compare register and translates it to a frequency.
		unsigned char GetFrequency(); 
		
		// selects between timer 0 output compares.  NOTE: changing the output will not reset DDR.  This must be done manually.
		void SetOutput( unsigned char );
		void SetOCBPhase( unsigned char );
		
	protected:
	private:
		unsigned char m_Frequency;
		HardwareTimer_8Bit m_Timer;
};

class Generator_16_Bit {
	public:
		//8_Bit_Generator( uint8_t* _outputRegister ); //Constructor without initial frequency
		Generator_16_Bit( unsigned char _output, float _frequency ); //Constructor with initial frequency
	
		//sets output compare registers to match frequency
		void SetFrequency( float _frequency );
	
		//Gets the value of the output compare register and translates it to a frequency.
		unsigned char GetFrequency();
	
		// selects between timer 0 output compares.  NOTE: changing the output will not reset B/D port Direction Registers.  This must be done manually.
		void SetOutput( unsigned char );
	
		void SetOCBPhase( unsigned char );
	protected:
	private:
		unsigned long m_Frequency;
		HardwareTimer_16Bit m_Timer;
};

#endif /* FREQGEN_H_ */