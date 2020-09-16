/*
 * configuration.c
 *
 *  Created on: Jun 1, 2020
 *      Author: GGLV
 */


#include "configuration.h"

extern date_time_t date_time;
_configuration conf;


/**
 * @brief 			: This function loads a standard configuration for the system
 * @param conf		: Pointer to a _configuration structure in which stores the standard configuration
 */
void load_standard_configuration(_configuration* conf){
	conf->user_pin[0] = '0';
	conf->user_pin[1] = '0';
	conf->user_pin[2] = '0';
	conf->user_pin[3] = '0';
	conf->area_alarm_delay =  1;
	conf->barrier_alarm_delay = 1;
	conf->alarm_duration =  5;
	rtc_set_date_time_default(&(conf->date_time));
}


/**
 * @brief			: This function sets a new configuration pin
 * @param conf		: Pointer to a _configuration structure in which stores the standard configuration
 * @param pin		: Array that contains the pin that has to be setted
 */
void set_configuration_pin(_configuration *conf, uint8_t pin[4]){
	conf->user_pin[0] = pin[0];
	conf->user_pin[1] = pin[1];
	conf->user_pin[2] = pin[2];
	conf->user_pin[3] = pin[3];
}
