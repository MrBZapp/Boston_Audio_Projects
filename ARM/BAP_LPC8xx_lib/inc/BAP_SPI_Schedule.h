/*
 * BAP_SPI_Schedule.h
 *
 *  Created on: Jul 7, 2015
 *      Author: Matt
 */

#ifndef BAP_SPI_SCHEDULE_H_
#define BAP_SPI_SCHEDULE_H_

#include "chip.h"

typedef struct BAP_SPI_ScheduleRequest{
	SPIM_XFER_T xfer;
	uint32_t config_reg;
	uint8_t dummy;
}BAP_SPI_ScheduleRequest_t;

ErrorCode_t BAP_SPI_Schedule_AddRequest(BAP_SPI_ScheduleRequest_t* request);

#endif /* BAP_SPI_SCHEDULE_H_ */
