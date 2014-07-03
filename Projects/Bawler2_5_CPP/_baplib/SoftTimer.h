/* 
* SoftTimer.h
*
* Created: 7/2/2014 8:30:37 AM
* Author: Matt
*/

#include "Scheduler.h"
#ifndef __SOFTTIMER_H__
#define __SOFTTIMER_H__


class SoftTimer
{
//variables
public:	
protected:
private:
	unsigned char m_eventNumber;
	unsigned char m_elapsedTime;
//functions
public:
	SoftTimer();
	void start();
	void stop();
	~SoftTimer();
protected:
private:
	SoftTimer( const SoftTimer &c );
	SoftTimer& operator=( const SoftTimer &c );

}; //SoftTimer

#endif //__SOFTTIMER_H__
