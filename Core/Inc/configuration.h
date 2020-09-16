/*
 * protocol.h
 *
 *  Created on: May 26, 2020
 *      Author: GGLV
 */

#ifndef INC_CONFIGURATION_H_
#define INC_CONFIGURATION_H_

#include <stdint.h>
#include "realtimeclock.h"

typedef struct{
	 uint8_t user_pin[4];
	 uint8_t area_alarm_delay;
	 uint8_t barrier_alarm_delay;
	 uint8_t alarm_duration;
	 date_time_t date_time;
} _configuration;

void load_standard_configuration(_configuration*);
void set_configuration_pin(_configuration *, uint8_t[]);

extern _configuration conf;



#endif /* INC_CONFIGURATION_H_ */
