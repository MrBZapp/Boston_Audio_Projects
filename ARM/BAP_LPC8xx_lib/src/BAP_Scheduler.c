/*
 * BAP_Scheduler.c
 *
 *  Created on: Apr 5, 2015
 *      Author: Matt
 */

#include "BAP_Scheduler.h"
#include "chip.h"
#include "BAP_Type.h"

// Function pointers for each used event
VoidFuncPointer Event_func[SCHEDULER_EVENT_COUNT];

// Function to convert event numbers to index numbers
static int EventToIndex(CHIP_SCT_EVENT_T event)
{
	int index = -1;
	while (event != 0)
	{
		event /= 2;
		index++;
	}
	return index;
}

int IndexToEvent(uint8_t i)
{
	return 2 ^ i;
}

void AttatchEventFunction(CHIP_SCT_EVENT_T event, VoidFuncPointer func)
{
	if (EventToIndex(event) >= 0)
	{
		Event_func[EventToIndex(event)] = func;
	}
}

void ScheduleEvent(CHIP_SCT_EVENT_T event, uint32_t overflow)
{
	Chip_SCT_EnableEventInt(LPC_SCT, event);
	NVIC_EnableIRQ(SCT_IRQn);
}


void DeScheduleFunction(CHIP_SCT_EVENT_T event)
{
	Chip_SCT_DisableEventInt(LPC_SCT, event);

	// If there aren't any more event interrupts enabled, turn off the SCT interrupt entirely.
	if (LPC_SCT->EVEN == 0)
		NVIC_DisableIRQ(SCT_IRQn);
}


/***
 * Interrupt placeholders
 */
void SCT_IRQHandler(void)
{
	// Start checking from Event 0
	int EventNumber = 0;

	// For every enabled Scheduler Event, check if that's the reason we've hit the interrupt
	do
	{
		// Convert the event number to event flag
		int EventFlag = 2 ^ EventNumber;

		// Check if that flag is enabled
		if ((LPC_SCT->EVFLAG & EventFlag))
		{
			Event_func[EventNumber]();
			Chip_SCT_ClearEventFlag(LPC_SCT, EventFlag);
		}

		// Get ready to check the next event
		EventNumber++;
	}  while (LPC_SCT->EVFLAG);
}
