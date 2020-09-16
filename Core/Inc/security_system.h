/*
 * security_system.h
 *
 *  Created on: 29 mag 2020
 *      Author: Gianluca
 */

#ifndef INC_SECURITY_SYSTEM_H_
#define INC_SECURITY_SYSTEM_H_

#include <stdio.h>

typedef enum{
	CHECK,
	OK
}_system_observer_state;


typedef enum
{
  ACTIVE=0,
  INACTIVE,
  ALARMED,
  DELAYED,
}_sensor_state;

typedef enum
{
  AREA = 0,
  BARRIER,
} _sensor_TypeDef;


typedef struct{
	_sensor_TypeDef type;
	_sensor_state current_state;
	uint8_t delay;
	uint16_t threshold;
}_sensor_HandleTypeDef;


typedef enum
{
	OFF,
	CONF,
	BARRIER_ALARM,
	AREA_ALARM,
	BARRIER_AREA_ALARM

} _buzzer_state;


typedef struct {
	_buzzer_state state;
	uint8_t duration;
}_buzzer_HandleTypeDef;

void SYSTEM_Handler_State(_buzzer_HandleTypeDef*,_sensor_HandleTypeDef*);

#endif /* INC_SECURITY_SYSTEM_H_ */
