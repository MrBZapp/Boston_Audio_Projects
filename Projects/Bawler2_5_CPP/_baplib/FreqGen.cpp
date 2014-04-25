/*
 * FreqGen.cpp
 *
 * Created: 4/20/2014 8:42:31 PM
 *  Author: Matt
 */ 

#include <avr/io.h>
#include "FreqGen.h"
#include "timercpp.h"
#include "global.h"

// 
// unsigned char m_Frequency = 0xFF;
// Timer8 m_timer( TIMER_CLK_DIV1 );
 
Generator_8_Bit::Generator_8_Bit( unsigned char output, unsigned int frequency ){ //Constructor with initial frequency
	TCCR0A = TCCR0A | (1 << WGM01); //turn on Clear on Timer compare frequency generation mode.
	this->SetFrequency( frequency ); // set OCR0A and prescalar to achieve nearest possible frequency
	this->SetOutput( output ); // Sets TCCR0A and DDR to the appropriate output
}

void Generator_8_Bit::SetFrequency( unsigned long frequency ){ //sets output compare registers to match frequency
	unsigned long liCPU = 8000000;
	// Calculate the appropriate prescale
	if ( frequency > (F_CPU/0xFF) ){
		m_Timer.SetPrescale( TIMER_CLK_DIV1 );
	} 
	else if ( frequency > ((F_CPU/8)/0xFF) ){
		m_Timer.SetPrescale( TIMER_CLK_DIV8 );
		liCPU /= 8;
	} 
	else if ( frequency > ((F_CPU/64)/0xFF) ){ 
		m_Timer.SetPrescale( TIMER_CLK_DIV64 );
		liCPU /= 64;
	} 
	else if ( frequency > ((F_CPU/256)/0xFF) ){
		m_Timer.SetPrescale( TIMER_CLK_DIV256 );
		liCPU /= 256;
	} 
	else {
		m_Timer.SetPrescale( TIMER_CLK_DIV1024 ); 
		liCPU /= 1024;
	}	
	// Calculate the mask and output
	m_Frequency = (unsigned char) ( liCPU / frequency);
	OCR0A = m_Frequency;
	OCR0B = m_Frequency; //If you ever want to have A/B out of phase, add some math here.
}

unsigned char Generator_8_Bit::GetFrequency(){ return m_Frequency; }; //Gets the value of the output compare register and translates it to a frequency

void Generator_8_Bit::SetOutput( unsigned char output ){
	if ( output == OCA ){
		TCCR0A = ( (TCCR0A & ~OUT_COMPARE_MASK) | 0x10 );
	} else {
		TCCR0A = ( (TCCR0A & ~OUT_COMPARE_MASK) | 0x40 );
	}
}

//////////////////////////////////////////////////
Generator_16_Bit::Generator_16_Bit( unsigned char output, unsigned int frequency ){ //Constructor with initial frequency
	TCCR0A = TCCR0A | (1 << WGM01); //turn on Clear on Timer compare frequency generation mode.
	this->SetFrequency( frequency ); // set OCR0A and prescalar to achieve nearest possible frequency
	this->SetOutput( output ); // Sets TCCR0A and DDR to the appropriate output
}

void Generator_16_Bit::SetFrequency( unsigned long frequency ){ //sets output compare registers to match frequency
	unsigned long liCPU = 8000000;
	// Calculate the appropriate prescale
	if ( frequency > (F_CPU/0xFF) ){
		m_Timer.SetPrescale( TIMER_CLK_DIV1 );
	}
	else if ( frequency > ((F_CPU/8)/0xFF) ){
		m_Timer.SetPrescale( TIMER_CLK_DIV8 );
		liCPU /= 8;
	}
	else if ( frequency > ((F_CPU/64)/0xFF) ){
		m_Timer.SetPrescale( TIMER_CLK_DIV64 );
		liCPU /= 64;
	}
	else if ( frequency > ((F_CPU/256)/0xFF) ){
		m_Timer.SetPrescale( TIMER_CLK_DIV256 );
		liCPU /= 256;
	}
	else {
		m_Timer.SetPrescale( TIMER_CLK_DIV1024 );
		liCPU /= 1024;
	}
	// Calculate the mask and output
	m_Frequency = (unsigned char) ( liCPU / frequency);
	OCR0A = m_Frequency;
	OCR0B = m_Frequency; //If you ever want to have A/B out of phase, add some math here.
}