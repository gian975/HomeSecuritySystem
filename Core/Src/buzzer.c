/*
 * buzzer.c
 *
 *  Created on: May 25, 2020
 *      Author: GGLV
 */

#include "buzzer.h"
#include "security_system.h"

/* Useful Variable for managing the sound */
uint8_t nino_area=0;
uint8_t nino_barrier=0;

_buzzer_HandleTypeDef buzzer;


/**
 * @brief			: this function initializes the buzzer device with state CONF e duration 5
 */
void BUZZER_Init(void){

	buzzer.state = OFF;
	buzzer.duration = 5;

}


/**
 * @brief 			: This function set the duty cycle for the PWM in according to the buzzer state
 * @param buz 		: Pointer to a _buzzer_HandleTypeDef structure that contains the configuration information for the buzzer
 */
void BUZZER_sound(_buzzer_HandleTypeDef* buz){
	switch(buz->state){
	case CONF:
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, PERIOD_ONE_SECOND);
		break;
	case BARRIER_ALARM:
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, PERIOD_ONE_SECOND/BARRIER_FACTOR);
		break;
	case  AREA_ALARM:
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, PERIOD_ONE_SECOND/AREA_FACTOR);
		break;
	case  BARRIER_AREA_ALARM:
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, PERIOD_ONE_SECOND/AREA_BARRIER_FACTOR);
		break;
	default:
		break;
	}
	HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_1);
}


/**
 * @brief			: This function handles the sound of the buzzer, in according to the state
 * @param buzzer	: Pointer to a _buzzer_HandleTypeDef structure that contains the configuration information for the buzzer
 */
void BUZZER_Handler_Sound(_buzzer_HandleTypeDef* buzzer){
	switch(buzzer->state){
		case AREA_ALARM:
			if(nino_area==1){
				__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, (PERIOD_ONE_SECOND/AREA_FACTOR)/2);
				nino_area=0;
			}else{
				__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, PERIOD_ONE_SECOND/AREA_FACTOR);
				nino_area=1;
			}
			break;
		case BARRIER_ALARM:
			if(nino_barrier==1){
				__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, (PERIOD_ONE_SECOND/BARRIER_FACTOR)/2);
				nino_barrier=0;
			}else{
				__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, PERIOD_ONE_SECOND/BARRIER_FACTOR);
				nino_barrier=1;
			}
			break;
		case CONF:
			// in case of configuration the buzzer has to emit only one sound, so it switch in OFF
			buzzer->state=OFF;
			HAL_TIM_PWM_Stop_IT(&htim2, TIM_CHANNEL_1);
			break;
		case OFF:
			// If the buzzer is setted on OFF, it has to stop to emit sound
			HAL_TIM_PWM_Stop_IT(&htim2, TIM_CHANNEL_1);
			break;
		case BARRIER_AREA_ALARM:
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1,PERIOD_ONE_SECOND/AREA_BARRIER_FACTOR);
			break;
	}

}


/**
 * @brief			: This function allows to sound a single 'bip'
 * @param buzzer 	: Pointer to a _buzzer_HandleTypeDef structure that contains the configuration information for the buzzer
 */
void BUZZER_Bip(_buzzer_HandleTypeDef*buzzer){

	buzzer->state = CONF;
	BUZZER_sound(buzzer);

}

