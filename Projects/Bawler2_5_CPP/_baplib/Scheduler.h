/* 
* Scheduler.h
*
* Created: 5/29/2014 10:55:30 PM
* Author: Matt
*/


#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__
#include "Uint8_Buffer.h"
#include "CPPTimer.h"
#include "HardwareTimer.h"

#define EVENT_MAXIMUM 5

class SoftTimer;
class HardwareTimer;

/* The Scheduler is capable of firing one or more events after a certain quantity
of clock cycles.  The response time for each function to start running is not as fast
as a direct link into the ISR.  Events must be presented to the Scheduler as follows:

Event Count:	How many times should this event be fired?
OverageDistance: How long to wait.  negative sign indicates run this event is to run only once.
CurrentDistance: How far the event is from triggering.
EventFunction:	Void function pointer of what to do once the appropriate quantity of time has elapsed.  
				The body of an event must be written as a C function without any variables and return no data.
*/


struct ScheduledEvent{//8bytes
	unsigned char EventCount;
	signed int OverageDistance;
	signed int CurrentDistance;
	VoidFuncPtr EventFunction; 
};


class Scheduler{
	//variables
	public:
	protected:
	private:
	//functions
	public:
		Scheduler( HardwareTimer_8Bit* htimer );
		unsigned char ScheduleAnEvent( ScheduledEvent newEvent ); //Returns the event number if properly scheduled, 0 if not.
		void ClearSchedule();
		void ProcessTime( time newTime );
		~Scheduler();
	protected:
	private:
		Buffer<ScheduledEvent> m_EventSchedule_blocking;
		
		//HardwareTimer* m_CoreTimer;
		HardwareTimer_8Bit* m_CoreTimer8;
	
		Scheduler( const Scheduler &c );
		Scheduler& operator=( const Scheduler &c );

}; //Scheduler



#endif //__SCHEDULER_H__
