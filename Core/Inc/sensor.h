/*
 * sensor.h
 *
 *  Created on: 1 giu 2020
 *      Author: GGLV
 */

#ifndef INC_SENSOR_H_
#define INC_SENSOR_H_

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"


extern _sensor_HandleTypeDef area;
extern _sensor_HandleTypeDef barrier;

void AREA_Init(void);
void BARRIER_Init(void);

void sensor_Handler_Alarm(_sensor_HandleTypeDef *);
void sensor_Handler_Timer(_sensor_HandleTypeDef *);
void sensor_change_state(_sensor_HandleTypeDef*,_sensor_state);

#endif /* INC_SENSOR_H_ */
