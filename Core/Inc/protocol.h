/*
 * conf.h
 *
 *  Created on: 29 mag 2020
 *      Author: GGLV
 */


#ifndef INC_PROTOCOL_H_
#define INC_PROTOCOL_H_

#include <stdint.h>
#include "main.h"

/* Print define ------------------------------------------------------------------ */
#define RESET_MEMORY_CONFIG ("\fDO YOU WANT RESET THE SYSTEM?\r\n")
#define SYSTEM_CONFIGURATION ("\r\nSYSTEM CONFIGURATION: \r\n")
#define INSERT_NEW_PIN ("INSERT NEW PIN: ")
#define NEW_CONFIGURATION ("EMPTY MEMORY - INSERT NEW CONFIGURATION\r\nINSERT NEW PIN: ")
#define OLD_CONFIGURATION ("A CONFIGURATION IS ALREADY PRESENT IN MEMORY\r\nINSERT THE PREVIOUS PIN TO CHANGE THIS CONFIGURATION OR ATTEND 30 SECONDS TO LOAD IT: \r\n")
#define TIME_ENDED ("TIMEOUT FOR CONFIGRATION: DEFAULT CONFIGURATION LOADED!\r\n")
#define ACTIVE_PIR (" Command accepted: PIR ACTIVE\r\n")
#define INACTIVE_PIR (" Command accepted: PIR INACTIVE\r\n")
#define ACTIVE_BARRIER (" Command accepted: BARRIER ACTIVE\r\n")
#define INACTIVE_BARRIER (" Command accepted: BARRIER INACTIVE\r\n")
#define ACTIVE_PIR_BARRIER (" Command accepted: PIR AND BARRIER ACTIVE\r\n")
#define INACTIVE_PIR_BARRIER (" Command accepted: PIR AND BARRIER INACTIVE\r\n")
#define SYSTEM_ACTIVE (" Command accepted: SYSTEM ACTIVE\r\n")
#define SYSTEM_INACTIVE (" Command accepted: SYSTEM INACTIVE\r\n")
#define SYSTEM_ALREADY_ACTIVE (" Command rejected: SYSTEM ALREADY ACTIVE\r\n")
#define PIN_INVALID (" Command rejected: INVALID PIN\r\n")
#define PIN_ACTIVE_SYSTEM_INVALID (" INVALID PIN: BEFORE ACTIVE THE SYSTEM\r\n")
#define COMMAND_REJECTED (" COMMAND REJECTED\r\n")
#define MEMORY_LOADING ("LOADING CONFIGURATION FROM MEMORY\r\n")
#define LOADING_COMPLETED ("LOADING COMPLETED\r\n")
#define AREA_DELAY ("INSERT AREA DELAY: ")
#define INSERT_ERROR_PIN ("INSERT ERROR - PIN HAS TO CONTAIN ONLY NUMBERS\r\n")
#define BARRIER_DELAY ("INSERT BARRIER DELAY: ")
#define INSERT_ERROR_AREA_DELAY ("INSERT ERROR - AREA DELAY MUST BE A NUMBER\r\n")
#define ALARM_DURATION ("INSERT ALLARM DURATION: ")
#define INSERT_ERROR_BARRIER_DELAY ("INSERT ERROR - BARRIER DELAY MUST BE A NUMBER\r\n")
#define INSERT_ERROR_ALARM_DURATION ("INSERT ERROR - ALARM DURATION MUST BE A NUMBER\r\n")
#define CHANGING_CONF ("CHANGING CONFIGURATION ABILITED\r\n")
#define CHANGING_CONF_ERROR (" PIN INVALID - IMPOSSIBLE TO CHANGE CONFIGURATION\r\nLOADING PREVIOUS CONFIGURATION\r\n")
#define AREA_DELAY_SETTED_MAX_30 ("AREA DELAY SETTED TO MAX: 30\r\n")
#define BARRIER_DELAY_SETTED_MAX_30 ("BARRIER DELAY SETTED TO MAX: 30\r\n")
#define DELAY_SETTED_MAX_60 ("ALARM DELAY SETTED TO MAX: 60\r\n")


/* Utility define ------------------------------------------------------------------ */
#define ARRAY_PIN (4)
#define PERIOD_ONE_SECOND 999
#define PIN_SIZE 7
#define BUFFER_SIZE_PIN 4
#define BUFFER_SIZE_DELAY 2



typedef enum{
	INITIALIZATION,
	TIME_CONFIGURATION_END,
	ACTIVATION_PIN,
	INACTIVE_PIN,
	AREA_DELAY_INSERTED,
	INSERT_BARRIER_DELAY,
	INSERT_ALARM_DURATION,
	BARRIER_DELAY_INSERTED,
	PIN_INSERTED,
	INSERT_AREA_DELAY,
	CONF_INSERTED,
	CHANGE_CONFIG,
	INSERT_PIN
} _protocol_state;


extern _protocol_state protocol_state;
extern uint8_t sistema;

void init_protocol(void);
void reset_count(void);
void log_transmit(void);
void reset_memory_config();
void check_sensor_status_activation(uint8_t cmd);
void check_sensor_status_deactivation(uint8_t cmd);

#endif /* INC_CONF_H_ */
