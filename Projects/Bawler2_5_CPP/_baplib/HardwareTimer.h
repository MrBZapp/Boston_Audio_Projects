/*
 * timer.h
 *
 * Created: 4/7/2014 9:26:41 PM
 *  Author: Matt
 */ 


#ifndef HARDWARETIMER_H_
#define HARDWARETIMER_H_
#include "BAPTypedefs.h"
#include "CPPTimer.h"


 //ABSTRACT CLASS
/*class HardwareTimer: public CPPTimer {
	public:
		//static HardwareTimer* accessTOIE0;
		
};*/

extern "C" void  TIMER0_OVF_vect(void) __attribute__ ((signal)); //required for interrupt handling in C++

class HardwareTimer_8Bit : public CPPTimer {
	public:
	//operational methods
		HardwareTimer_8Bit();
		explicit HardwareTimer_8Bit( prescaleSetting prescale );
	//	HardwareTimer_8Bit ( prescaleSetting prescale, CBFunctor0 callback);  //Comment out this line please, these callbacks are slow and take up a lot of space
		static HardwareTimer_8Bit* accessTOIE0;
		time pause();
		void start();
		void stop();
		void setPrescale( prescaleSetting prescale );
		prescaleSetting getPrescale();
		
	//access methods
		time getTime();
		time getTime_NoClear();
		time getTime_NoUpdates();
		//void setCallback( CBFunctor0 newCallback ); //Comment out this line please, these callbacks are slow and take up a lot of space
		
	//ISR
		friend void TIMER0_OVF_vect();// Interrupt Service Routine for Timer overflow
	
	protected:
		inline void updateTime();
		
	private:
		inline unsigned char translatePrescale( prescaleSetting toTranslate );
		CBFunctor0 m_Callback;
	};

class HardwareTimer_16Bit : public CPPTimer {
	public:
		HardwareTimer_16Bit();
		explicit HardwareTimer_16Bit( unsigned char prescale );
		void enableInterrupt();
		time pause();
		void start();
		void stop();
		void setPrescale( unsigned char prescale );
		prescaleSetting getPrescale();
		
		//access methods
		time getTime();
		time getTime_NoClear();
		time getTime_NoUpdates();
		
	protected:
	private:
	void updateTime();
};




#endif /* HARDWARETIMER_H_ */