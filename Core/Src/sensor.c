/*
 * sensor.c
 *
 *  Created on: Jun 1, 2020
 *      Author: GGLV
 */


/* Includes ------------------------------------------------------------------*/
#include "tim.h"
#include "adc.h"

/* Private function ----------------------------------------------------------*/
uint16_t calibrate_threshold(void);


_sensor_HandleTypeDef area;
_sensor_HandleTypeDef barrier;


/**
 * @brief This function initialize AREA sensor
 */
void AREA_Init(void){

	area.type=AREA;
	area.current_state=INACTIVE;
	area.delay = 0;
}

/**
 * @brief This function initialize BARRIER sensor
 */
void BARRIER_Init(void) {

	barrier.type = BARRIER;
	barrier.current_state = INACTIVE;
	barrier.delay = 0;
	barrier.threshold = calibrate_threshold();

	if (barrier.current_state == INACTIVE) {
		HAL_GPIO_WritePin(GPIOB, Activation_LASER_Pin, GPIO_PIN_RESET);
	}

}

/**
 * @brief This function handles the state of the sensor passed thought the input.
 * @param sensor : Pointer to a _sensor_HandleTypeDef structure that contains the configuration information for the specific sensor
 */
void sensor_Handler_Alarm(_sensor_HandleTypeDef *sensor) {
	uint16_t runtime = 0;
	if (sensor->current_state != INACTIVE) { //if the sensor is inactive avoid the handle
		if (sensor->current_state == ACTIVE) {
			if (sensor->type == BARRIER) {
				//handle BARRIER
				runtime = HAL_ADC_GetValue(&hadc1); //Read the value from ADC
				if (runtime > sensor->threshold + 1) { //laser not detected on the LDR
					if (sensor->delay == 0) {
						// remain in delay for a little time (the sensor is not delayed)
						__HAL_TIM_SET_AUTORELOAD(&htim10,
								(PERIOD_ONE_SECOND + 1) / 100);
					}
					__HAL_TIM_SET_AUTORELOAD(&htim10,
							(PERIOD_ONE_SECOND + 1) * (sensor->delay));
					HAL_TIM_Base_Start_IT(&htim10);
					sensor->current_state = DELAYED;
				}
			} else {
				//handle AREA
				if (sensor->delay == 0) {
					// remain in delay for a little time (the sensor is not delayed)
					__HAL_TIM_SET_AUTORELOAD(&htim10,
							(PERIOD_ONE_SECOND + 1) / 100);
				}
				__HAL_TIM_SET_AUTORELOAD(&htim11,
						(PERIOD_ONE_SECOND + 1) * (sensor->delay));
				HAL_TIM_Base_Start_IT(&htim11);
				sensor->current_state = DELAYED;
			}
		} else if (sensor->current_state == DELAYED) {
			//if the sensor is delayed and (capture the light on the LDR/ PIR interrupt Falling) restore the state in ACTIVE
			if (sensor->type == BARRIER) {
				//handle BARRIER
				runtime = HAL_ADC_GetValue(&hadc1); //Read the value from ADC
				if (runtime < sensor->threshold) { //laser on the LDR
					sensor->current_state = ACTIVE;
					HAL_TIM_Base_Stop_IT(&htim10);
					__HAL_TIM_SET_COUNTER(&htim10, 0);
				}
			} else {
				//handle AREA
				sensor->current_state = ACTIVE;
				HAL_TIM_Base_Stop_IT(&htim11);
				__HAL_TIM_SET_COUNTER(&htim11, 0);
			}
		}
	}
}

/**
 * @brief This function handles the Timer used for the sensor (The timer is used for count the delay and after is used for count alarm duration)
 * @param sensor : Pointer to a _sensor_HandleTypeDef structure that contains the configuration information for the specific sensor

 */
void sensor_Handler_Timer(_sensor_HandleTypeDef *sensor) {
	if (sensor->current_state != INACTIVE) { //if the sensor is inactive avoid the handle
		if (sensor->current_state == ALARMED) {
			sensor->current_state = ACTIVE;
		} else if (sensor->current_state == DELAYED) { //when the the delay status is over, set in alarmed and sound the buzzer
			sensor->current_state = ALARMED;
		}
	}
}

/**
 * @brief This function change the state of the sensor passed and on the base of the state passed manage the component used
 * @param sensor : Pointer to a _sensor_HandleTypeDef structure that contains the configuration information for the specific sensor .
 * @param state : specific state of the sensor to set.
 */
void sensor_change_state(_sensor_HandleTypeDef *sensor, _sensor_state state) {
	if (sensor->type == BARRIER) {
		//handle BARRIER
		switch (state) {
		case ACTIVE:
			HAL_GPIO_WritePin(GPIOB, Activation_LASER_Pin, GPIO_PIN_SET);
			HAL_ADC_Start_IT(&hadc1);
			break;
		case INACTIVE:
			HAL_GPIO_WritePin(GPIOB, Activation_LASER_Pin, GPIO_PIN_RESET);
			HAL_ADC_Stop_IT(&hadc1);
			break;
		default:
			break;
		}
	} else {
		//handle AREA
	}
	sensor->current_state = state;
}

/**
 * @brief This function calibrate the threshold of the barrier in two phase:
 * 			- reading the value of the Light dependent photo-resistor when the Laser is off
 * 			- reading the value of the Light dependent photo-resistor when the Laser is off
 * @return a threshold calibrated based on the light of the room.
 */
uint16_t calibrate_threshold(void) {
	uint16_t low_barrier;
	uint16_t high_barrier;

	HAL_ADC_Start(&hadc1);
	while (1) {
		if (HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY) == HAL_OK) {
			low_barrier = HAL_ADC_GetValue(&hadc1);
			break;
		}
	}
	HAL_GPIO_WritePin(GPIOB, Activation_LASER_Pin, GPIO_PIN_SET);
	HAL_Delay(1000);
	while (1) {
		if (HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY) == HAL_OK) {
			high_barrier = HAL_ADC_GetValue(&hadc1);
			HAL_ADC_Stop(&hadc1);
			break;
		}
	}
	return (uint16_t) (low_barrier - high_barrier) / 3;
}
