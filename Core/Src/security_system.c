/*
 * check_state.c
 *
 *  Created on: 9 giu 2020
 *      Author: GGLV
 */
#include "security_system.h"
#include "tim.h"

void check_ALARMED_state(_buzzer_HandleTypeDef *,_sensor_HandleTypeDef *);
void check_ACTIVE_state(_buzzer_HandleTypeDef *,_sensor_HandleTypeDef *);
void check_INACTIVE_state(_buzzer_HandleTypeDef *,_sensor_HandleTypeDef *);
/**
 * @brief 			: This function manages the state of system based on sensor state
 * @param buz  		: Pointer to a _buzzer_HandleTypeDef structure that contains the configuration information for the buzzer
 * @param sensor 	: Pointer to a _sensor_HandleTypeDef structure that contains the configuration information for the sensor
 */
void SYSTEM_Handler_State(_buzzer_HandleTypeDef *buz,_sensor_HandleTypeDef *sensor) {

	switch (sensor->current_state) {
	case ALARMED:
		check_ALARMED_state(buz,sensor);
		break;
	case ACTIVE:
		check_ACTIVE_state(buz,sensor);
		break;
	case INACTIVE:
		check_INACTIVE_state(buz,sensor);
		break;
	default:
		break;
	}
	BUZZER_sound(buz);
}

void check_ALARMED_state(_buzzer_HandleTypeDef *buz,_sensor_HandleTypeDef *sensor){
	switch (buz->state) {
	case OFF:
		if (sensor->type == BARRIER) {
			buz->state = BARRIER_ALARM;
		} else {
			buz->state = AREA_ALARM;
		}
		break;
	default:
		buz->state = BARRIER_AREA_ALARM;
		break;
	}
	if (sensor->type == BARRIER) {
		__HAL_TIM_SET_AUTORELOAD(&htim10,(PERIOD_ONE_SECOND + 1) * (buz->duration));
		HAL_TIM_Base_Start_IT(&htim10);
	} else {
		__HAL_TIM_SET_AUTORELOAD(&htim11,(PERIOD_ONE_SECOND + 1) * (buz->duration));
		HAL_TIM_Base_Start_IT(&htim11);
	}
}

void check_ACTIVE_state(_buzzer_HandleTypeDef *buz,_sensor_HandleTypeDef *sensor){
	//Based on type,handles the right timer
	if (sensor->type == BARRIER) {
		//handle BARRIER
		HAL_TIM_Base_Stop_IT(&htim10);
		__HAL_TIM_CLEAR_IT(&htim10, TIM1_UP_TIM10_IRQn);
	} else {
		//handle AREA
		HAL_TIM_Base_Stop_IT(&htim11);
		__HAL_TIM_CLEAR_IT(&htim11, TIM1_UP_TIM10_IRQn);
	}
	check_INACTIVE_state(buz,sensor);
}

void check_INACTIVE_state(_buzzer_HandleTypeDef *buz,_sensor_HandleTypeDef *sensor){
	//if the buzzer is sounding disable the right sound
	if (buz->state == BARRIER_AREA_ALARM) {
		if (sensor->type == BARRIER)
			buz->state = AREA_ALARM;
		else
			buz->state = BARRIER_ALARM;
	} else {
		buz->state = OFF;
	}
}
