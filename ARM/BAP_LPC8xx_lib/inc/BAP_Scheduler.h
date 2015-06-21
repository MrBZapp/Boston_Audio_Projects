/*
 * BAP_Scheduler.h
 *
 *  Created on: Apr 5, 2015
 *      Author: Matt
 */

#ifndef BAP_SCHEDULER_H_
#define BAP_SCHEDULER_H_

#include "chip.h"
#include "BAP_Type.h"

#define SCHEDULER_EVENT_COUNT 5

void ScheduleFunction(CHIP_SCT_EVENT_T event, VoidFuncPointer func);
void DeScheduleFunction(CHIP_SCT_EVENT_T event);
void SetEventInterval(CHIP_SCT_EVENT_T event, uint32_t cycles);

#endif /* BAP_SCHEDULER_H_ */
