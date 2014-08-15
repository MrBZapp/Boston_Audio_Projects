/* 
* Scheduler.cpp
*
* Created: 5/29/2014 10:55:30 PM
* Author: Matt
*/


//BUILD A TASK SCHEDULER
// Blocking:
// time in sub interval.  start 1st timer. start second timer by taking the immediate time and storing it as time 1.*
// set time to 0x00 and start to time subinterval. subinterval must return to the timer a time which is added onto
// when finishing. time immediately resumes upon subinterval ceased.
//
//
//Not Blocking:
// *if the first thread might end while measuring the second interval, don't reset, store times in a FIFO buffer and trigger that timer's interrupt, but first trigger an intermediate interrupt
// that manages which times and interrupts should come next.  fast forwarding the timer the second interval will cause the next timer's interrupt to fire when required.
// make sure fast-forwarding to 0 is acceptable, interrupts will trigger in scheduled order.


#include "Scheduler.h"

// default constructor
Scheduler::Scheduler( HardwareTimer_8Bit* htimer )
	: m_EventSchedule_blocking( EVENT_MAXIMUM )
	, m_CoreTimer8( htimer )
{
	//m_CoreTimer8->setOverageCallback( makeFunctor( (CBFunctor0*)0, *this, &Scheduler::interruptHandler ) );
}

unsigned char Scheduler::ScheduleAnEvent( ScheduledEvent newEvent ){
	if ( m_EventSchedule_blocking.SpaceUsed() == 0 ){
		m_CoreTimer8->start();
	}
	newEvent.EventCount = (m_EventSchedule_blocking.SpaceUsed() + 1);
	m_EventSchedule_blocking.AddToEnd(newEvent);
	return m_EventSchedule_blocking.SpaceUsed();
}

void Scheduler::ProcessTime( time newTime ){
	ScheduledEvent* testEvent;
	for(signed char i = m_EventSchedule_blocking.SpaceUsed(); i >= 0; i--){//iterate through each event
		testEvent = m_EventSchedule_blocking.ReadAddressOfIndex(i);//pull the event into temp space
			if ( testEvent->OverageDistance != 0 ){ // ignore non-events. BUG WARNING: the buffer may contain junk data, especially after an event cycle is finished
				testEvent->CurrentDistance -= ( m_CoreTimer8->getPrescale() *256 ); //update the distance
				if ( testEvent->CurrentDistance <= 0 ) //if the timer has moved far enough to trigger the event, do a bunch of crap.
				{
					testEvent->EventFunction();	//fire the event
					testEvent->CurrentDistance = testEvent->OverageDistance; //reset the distance
					testEvent->EventCount--;
					if (testEvent->EventCount == 0 ){//if the count is done, get rid of the event from the buffer
						m_EventSchedule_blocking.PopFromIndex( i );
					}
				} 
			}
	}
}

/*
void SortOverages(){ 
	for(signed char i = 1; i >= 0; i--){
		if (Timers[i].CurrentDistance - (CoreTimer.getPrescale() * 256) <= 0 ) {
			Timers[i].CurrentDistance = Timers[i].OverageDistance;
			Timers[i].OverageCallback();
		} else {
			Timers[i].CurrentDistance -= (CoreTimer.getPrescale() * 256);
		}
	}
 }
*/

/*
void Scheduler::updateTimerOrder(){
	time tmpHardTime = m_CoreTimer8->pause();
	const int distanceElapsed = tmpHardTime.count
							  + (tmpHardTime.overs * m_CoreTimer8->getPrescale() * 256);	
	for ( unsigned char i = 0; i < m_Schedule.SpaceUsed(); i++ ){ // Update the interrupt distances of all timers
		SoftTimer* tmpTimer = m_Schedule.ReadIndex(i);
		tmpTimer->updateInterruptDistance( distanceElapsed );
	}
	
	//Insert Sort! i is the value being moved, it starts at the second value of the array and moves to the final timer in the array
	for ( unsigned char i = 1; i < m_Schedule.SpaceUsed(); i++){
		
		//locate the first interrupt distance that is larger than or equal to the value being moved.
		//break when j meets i or when the value is found. j is the value to be compared against.  
		//It starts at 0 and moves towards i in order to find the first sorted value larger than it.

		unsigned char j = i; 
		signed int MovingTimerInterruptDistance = m_Schedule.ReadIndex(i)->m_InterruptDistance;
		while ( j > 0 && m_Schedule.ReadIndex(j-1)->m_InterruptDistance > MovingTimerInterruptDistance ){
			j--;
		}
	
		// if the sorted timer isn't being compared against itself, pull timer to be moved out of the buffer
		// and move it to the location of the timer that is larger than it.
	
		if ( j != i){
			m_Schedule.PushToIndex( m_Schedule.PopFromIndex(i), j); 
		}//Endif
	
		j = 0;
	}//endfor
	
	m_CoreTimer8->start();//restart the core timer after sort is complete.
}//end updateTimerOrder
*/

// default destructor
Scheduler::~Scheduler()
{
} //~Scheduler
