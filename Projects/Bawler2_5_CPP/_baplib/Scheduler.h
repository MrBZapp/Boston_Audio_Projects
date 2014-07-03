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

Event Number:	Don't bother setting to anything but 0, this value will be set by the 
				scheduler and returned when adding an event to the schedule
Priority:		Used by the scheduler to decide the order of triggered events.  
				Priorities 0xF0 and above are considered "blocking" events and will be executed 
				in priority order as fast as possible from time of interrupt.
Repetitions:	How many times should this particular function be triggered? To run and event until
				manually removed from the schedule, use 0.
RepCount:		Used by the scheduler to indicate how many times a particular event has been triggered.
OverageDistance: How long to wait.  negative sign indicates run this event is to run only once.
CurrentDistance: How far the event is from triggering.
EventFunction:	Void function pointer of what to do once the appropriate quantity of time has elapsed.  
				The body of an event must be written as a C function without any variables and return no data.
*/


struct ScheduledEvent{//8bytes
	unsigned char EventNumber;
	unsigned char Priority;
	unsigned char Repetitions;
	unsigned char RepCount;
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
		void DeScheduleAnEvent( unsigned char eventNumber );	
		void ClearSchedule();
		void ProcessTime( time newTime );
		~Scheduler();
	protected:
	private:
		Buffer<ScheduledEvent> m_EventSchedule_noblock;
		Buffer<ScheduledEvent> m_EventSchedule_blocking;
		
		//HardwareTimer* m_CoreTimer;
		HardwareTimer_8Bit* m_CoreTimer8;
	
		Scheduler( const Scheduler &c );
		Scheduler& operator=( const Scheduler &c );

}; //Scheduler



#endif //__SCHEDULER_H__
