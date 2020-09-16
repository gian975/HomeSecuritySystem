/*
 * protocol.c
 *
 *  Created on: Jun 1, 2020
 *      Author: GGLV
 */


/* Includes ------------------------------------------------------------------*/
#include "protocol.h"
#include "usart.h"
#include "tim.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern uint8_t eeprom_data[CONFIGURATION_SIZE];
extern uint8_t rtc_data[DATE_SIZE];
extern _configuration conf;

uint8_t sistema = 0;

char area_state[4][20] = { "Area Active", "Area Inactive", "Area Alarmed","Area Delayed" };
char barrier_state[4][20] = { "Barrier Active", "Barrier Inactive","Barrier Alarmed", "Barrier Delayed" };
char logging[60];

/* Buffer for USART communication ----------------------------------------------------------*/
uint8_t read_buffer_reset[1];
uint8_t read_buffer[PIN_SIZE];
uint8_t buffer_pin[BUFFER_SIZE_PIN];
uint8_t buffer_new_conf[BUFFER_SIZE_PIN];
uint8_t buffer_barrier_delay[BUFFER_SIZE_DELAY];
uint8_t buffer_area_delay[BUFFER_SIZE_DELAY];
uint8_t buffer_alarm_duration[BUFFER_SIZE_DELAY];

_protocol_state protocol_state;

/* Private function ----------------------------------------------------------*/
uint8_t check_pin(uint8_t[], uint8_t[], uint8_t);
uint8_t evaluate_buffer(uint8_t []);
uint8_t check_buffer_value(uint8_t [], uint8_t);
void set_configuration_devices(_configuration *, uint8_t [], uint8_t [], uint8_t []);
void set_device(_configuration *, _sensor_HandleTypeDef *, _sensor_HandleTypeDef*, _buzzer_HandleTypeDef *);

/**
 * @brief: This function initializes the protocol and transmits on the serial port.
 */
void init_protocol(void) {
	protocol_state = INITIALIZATION;
	HAL_UART_Transmit_IT(&huart2, (uint8_t*) SYSTEM_CONFIGURATION,
			strlen(SYSTEM_CONFIGURATION));
}

/**
 * @brief	 	: This function checks which of the buttons A, B, C, D has been pressed on the keypad and, depending on the button pressed, performs an activation operation.
 * @param cmd	: specifies the button pressed.
 */
//check in activation state which sensor must be activate
void check_sensor_status_activation(uint8_t cmd) {
	switch (cmd) {

	case 'A':
		HAL_UART_Transmit(&huart2, (uint8_t*) ACTIVE_PIR, strlen(ACTIVE_PIR),
		HAL_MAX_DELAY);
		sensor_change_state(&area, ACTIVE);
		sensor_change_state(&barrier, INACTIVE);
		log_transmit();
		BUZZER_Bip(&buzzer);
		break;

	case 'B':
		HAL_UART_Transmit(&huart2, (uint8_t*) ACTIVE_BARRIER,
				strlen(ACTIVE_BARRIER), HAL_MAX_DELAY);
		sensor_change_state(&area, INACTIVE);
		sensor_change_state(&barrier, ACTIVE);
		log_transmit();
		BUZZER_Bip(&buzzer);
		break;

	case 'C':
		HAL_UART_Transmit(&huart2, (uint8_t*) ACTIVE_PIR_BARRIER,
				strlen(ACTIVE_PIR_BARRIER), HAL_MAX_DELAY);
		sensor_change_state(&area, ACTIVE);
		sensor_change_state(&barrier, ACTIVE);
		log_transmit();
		BUZZER_Bip(&buzzer);
		break;

	case 'D':
		HAL_UART_Transmit(&huart2, (uint8_t*) SYSTEM_ALREADY_ACTIVE,
				strlen(SYSTEM_ALREADY_ACTIVE), HAL_MAX_DELAY);
		log_transmit();
		BUZZER_Bip(&buzzer);
		break;

	default:
		HAL_UART_Transmit(&huart2, (uint8_t*) COMMAND_REJECTED,
				strlen(COMMAND_REJECTED), HAL_MAX_DELAY);
		break;
	}


}
/**
 * @brief		: This function checks which of the buttons A, B, C, D has been pressed on the keypad and, depending on the button pressed,
 *                performs an deactivation operation.
 * @param cmd	: specifies the button pressed.
 */

//check in deactivation state which sensor must be activate
void check_sensor_status_deactivation(uint8_t cmd) {
	switch (cmd) {

	case 'A':
		HAL_UART_Transmit(&huart2, (uint8_t*) INACTIVE_PIR,
				strlen(INACTIVE_PIR), HAL_MAX_DELAY);
		sensor_change_state(&area, INACTIVE);
		log_transmit();
		BUZZER_Bip(&buzzer);
		break;

	case 'B':
		HAL_UART_Transmit(&huart2, (uint8_t*) INACTIVE_BARRIER,
				strlen(INACTIVE_BARRIER), HAL_MAX_DELAY);
		sensor_change_state(&barrier, INACTIVE);
		log_transmit();
		BUZZER_Bip(&buzzer);
		break;

	case 'C':
		HAL_UART_Transmit(&huart2, (uint8_t*) INACTIVE_PIR_BARRIER,
				strlen(INACTIVE_PIR_BARRIER), HAL_MAX_DELAY);
		sensor_change_state(&area, INACTIVE);
		sensor_change_state(&barrier, INACTIVE);
		log_transmit();
		BUZZER_Bip(&buzzer);
		break;

	case 'D':
		sistema = 0;
		sensor_change_state(&area, INACTIVE);
		sensor_change_state(&barrier, INACTIVE);
		HAL_UART_Transmit(&huart2, (uint8_t*) SYSTEM_INACTIVE,
				strlen(SYSTEM_INACTIVE), HAL_MAX_DELAY);
		HAL_GPIO_WritePin(Status_LED_GPIO_Port, Status_LED_Pin, GPIO_PIN_RESET);
		log_transmit();
		BUZZER_Bip(&buzzer);
		break;

	default:

		HAL_UART_Transmit(&huart2, (uint8_t*) COMMAND_REJECTED,
				strlen(COMMAND_REJECTED), HAL_MAX_DELAY);
		break;
	}

}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {

	switch (protocol_state) {

	case INITIALIZATION:
		__HAL_TIM_SET_AUTORELOAD(&htim10, (PERIOD_ONE_SECOND + 1) * (30));
		HAL_TIM_Base_Start_IT(&htim10);
		read_configuration();
		break;

	case PIN_INSERTED:
		protocol_state = INSERT_AREA_DELAY;
		HAL_UART_Receive_IT(&huart2, buffer_area_delay, BUFFER_SIZE_DELAY);
		break;

	case AREA_DELAY_INSERTED:
		protocol_state = INSERT_BARRIER_DELAY;
		HAL_UART_Receive_IT(&huart2,buffer_barrier_delay, BUFFER_SIZE_DELAY);
		break;

	case BARRIER_DELAY_INSERTED:
		protocol_state = INSERT_ALARM_DURATION;
		HAL_UART_Receive_IT(&huart2, buffer_alarm_duration, BUFFER_SIZE_DELAY);
		break;

	case TIME_CONFIGURATION_END:
		read_configuration();
		break;

	case CONF_INSERTED:
		log_transmit();
		protocol_state = INACTIVE_PIN;
		HAL_TIM_Base_Start_IT(&htim3);
		BUZZER_Bip(&buzzer);
		set_device(&conf, &area, &barrier, &buzzer);
		break;

	case ACTIVATION_PIN:
		reset_count();
		if (!sistema) {
			if (check_pin(read_buffer, conf.user_pin, 1)) {
				if (read_buffer[0] == '#' && read_buffer[6] == '#') {
					if (read_buffer[5] == 'D') {
						sistema = 1;
						protocol_state = INACTIVE_PIN;
						HAL_UART_Transmit(&huart2, (uint8_t*) SYSTEM_ACTIVE,
								strlen(SYSTEM_ACTIVE), HAL_MAX_DELAY);
						HAL_GPIO_WritePin(Status_LED_GPIO_Port, Status_LED_Pin, GPIO_PIN_SET);
						log_transmit();
						BUZZER_Bip(&buzzer);
					} else {
						protocol_state = INACTIVE_PIN;
						HAL_UART_Transmit(&huart2,
								(uint8_t*) PIN_ACTIVE_SYSTEM_INVALID,
								strlen(PIN_ACTIVE_SYSTEM_INVALID),
								HAL_MAX_DELAY);
						log_transmit();
					}
				} else {
					protocol_state = INACTIVE_PIN;
					HAL_UART_Transmit(&huart2, (uint8_t*) COMMAND_REJECTED,
							strlen(COMMAND_REJECTED), HAL_MAX_DELAY);
					log_transmit();
				}
			} else {
				protocol_state = INACTIVE_PIN;
				HAL_UART_Transmit(&huart2, (uint8_t*) PIN_INVALID,
						strlen(PIN_INVALID), HAL_MAX_DELAY);
				log_transmit();
			}
			break;
		}

		if (sistema) {
			if (check_pin(read_buffer, conf.user_pin, 1)) {
				if (read_buffer[0] == '#' && read_buffer[6] == '#') {
					protocol_state = INACTIVE_PIN;
					check_sensor_status_activation(read_buffer[5]);
				} else if (read_buffer[0] == '#' && read_buffer[6] == '*') {
					protocol_state = INACTIVE_PIN;
					check_sensor_status_deactivation(read_buffer[5]);
				} else {
					protocol_state = INACTIVE_PIN;
					HAL_UART_Transmit(&huart2, (uint8_t*) COMMAND_REJECTED,
							strlen(COMMAND_REJECTED), HAL_MAX_DELAY);
					log_transmit();
				}
			} else {
				protocol_state = INACTIVE_PIN;
				HAL_UART_Transmit(&huart2, (uint8_t*) PIN_INVALID,
						strlen(PIN_INVALID), HAL_MAX_DELAY);
				log_transmit();
			}
		}
		break;

	default:
		break;
	}
}

/**
 * @brief	: This function calls the function to read the rtc.
 */
void log_transmit() {
	rtc_get_date_time(&(conf.date_time));
}

void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c) {

	if (hi2c->Instance == I2C1 && protocol_state != INITIALIZATION && protocol_state!=TIME_CONFIGURATION_END) {

		conf.date_time.seconds=bcd2Dec(rtc_data[0]);
		conf.date_time.minutes=bcd2Dec(rtc_data[1]);
		conf.date_time.hours=bcd2Dec(rtc_data[2]);
		conf.date_time.date=bcd2Dec(rtc_data[4]);
		conf.date_time.month=bcd2Dec(rtc_data[5]);
		conf.date_time.year=bcd2Dec(rtc_data[6]);

		sprintf(logging, "[ %hu-%hu-%hu , %hu:%hu:%hu  %s----%s ]\r\n",
				(conf.date_time).date, (conf.date_time).month,
				(conf.date_time).year, (conf.date_time).hours,
				(conf.date_time).minutes, (conf.date_time).seconds,
				area_state[area.current_state],
				barrier_state[barrier.current_state]);
		HAL_UART_Transmit_IT(&huart2, (uint8_t*) logging, strlen(logging));
	}

	if(hi2c->Instance == I2C1 && protocol_state==TIME_CONFIGURATION_END){
		protocol_state=CONF_INSERTED;
		if (eeprom_data[0] == '[') {
			load_configuration_saved(&conf, eeprom_data);
		} else {
			load_standard_configuration(&conf);
		}
		rtc_set_date_time(&(conf.date_time));
		HAL_UART_Transmit_IT(&huart2, (uint8_t*) MEMORY_LOADING ,strlen(MEMORY_LOADING ));
	}

	if(hi2c->Instance == I2C1 && protocol_state==INITIALIZATION){
		if (eeprom_data[0] == '[') {
			load_configuration_saved(&conf, eeprom_data);
			protocol_state=CHANGE_CONFIG;
			HAL_UART_Transmit(&huart2, (uint8_t *)OLD_CONFIGURATION, strlen(OLD_CONFIGURATION), HAL_MAX_DELAY);
			HAL_UART_Receive_IT(&huart2,buffer_new_conf, BUFFER_SIZE_PIN);
		} else {
			protocol_state=INSERT_PIN;
			HAL_UART_Transmit(&huart2, (uint8_t*) NEW_CONFIGURATION,
					strlen(NEW_CONFIGURATION),HAL_MAX_DELAY);
			HAL_UART_Receive_IT(&huart2, (uint8_t*) buffer_pin, BUFFER_SIZE_PIN);
		}
	}
}

void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c) {
	if (hi2c->Instance == I2C1){
		HAL_UART_Transmit_IT(&huart2, (uint8_t*) LOADING_COMPLETED,strlen(LOADING_COMPLETED));
		BUZZER_Bip(&buzzer);
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

	switch (protocol_state) {

	case INSERT_PIN:

		HAL_UART_Transmit(&huart2, (uint8_t*)"****\r\n",strlen("****\r\n"),HAL_MAX_DELAY);

		if (check_buffer_value(buffer_pin, BUFFER_SIZE_PIN)) {
			protocol_state = PIN_INSERTED;
			set_configuration_pin(&conf, buffer_pin);
			HAL_UART_Transmit_IT(&huart2, (uint8_t*) AREA_DELAY,
					strlen(AREA_DELAY));
		} else {
			protocol_state = TIME_CONFIGURATION_END;
			HAL_UART_Transmit_IT(&huart2, (uint8_t*) INSERT_ERROR_PIN,
					strlen(INSERT_ERROR_PIN));
		}
		break;

	case INSERT_AREA_DELAY:

		if (check_buffer_value(buffer_area_delay, BUFFER_SIZE_DELAY)) {
			protocol_state = AREA_DELAY_INSERTED;
			HAL_UART_Transmit(&huart2, buffer_area_delay, BUFFER_SIZE_DELAY, HAL_MAX_DELAY);
			HAL_UART_Transmit(&huart2, (uint8_t *) "\r\n", strlen("\r\n"), HAL_MAX_DELAY);
			HAL_UART_Transmit_IT(&huart2, (uint8_t*) BARRIER_DELAY,
					strlen(BARRIER_DELAY));
		} else {
			protocol_state = TIME_CONFIGURATION_END;
			HAL_UART_Transmit_IT(&huart2, (uint8_t*) INSERT_ERROR_AREA_DELAY,
					strlen(INSERT_ERROR_AREA_DELAY));
		}
		break;

	case INSERT_BARRIER_DELAY:

		if (check_buffer_value(buffer_barrier_delay, BUFFER_SIZE_DELAY)) {
			protocol_state = BARRIER_DELAY_INSERTED;
			HAL_UART_Transmit(&huart2, buffer_barrier_delay, BUFFER_SIZE_DELAY, HAL_MAX_DELAY);
			HAL_UART_Transmit(&huart2, (uint8_t *) "\r\n", strlen("\r\n"), HAL_MAX_DELAY);
			HAL_UART_Transmit_IT(&huart2, (uint8_t*) ALARM_DURATION,
					strlen(ALARM_DURATION));
		} else {
			protocol_state = TIME_CONFIGURATION_END;
			HAL_UART_Transmit_IT(&huart2, (uint8_t*) INSERT_ERROR_BARRIER_DELAY,
					strlen(INSERT_ERROR_BARRIER_DELAY));
		}
		break;

	case INSERT_ALARM_DURATION:

		if (check_buffer_value(buffer_alarm_duration, BUFFER_SIZE_DELAY)) {
			protocol_state = CONF_INSERTED;
			HAL_UART_Transmit(&huart2, buffer_alarm_duration, BUFFER_SIZE_DELAY, HAL_MAX_DELAY);
			HAL_UART_Transmit(&huart2, (uint8_t *) "\r\n", strlen("\r\n"), HAL_MAX_DELAY);

			set_current_date(&(conf.date_time));
			rtc_set_date_time(&(conf.date_time));
			set_configuration_devices(&conf,buffer_area_delay, buffer_barrier_delay, buffer_alarm_duration);
			save_configuration(&conf);
		} else {
			protocol_state = TIME_CONFIGURATION_END;
			HAL_UART_Transmit_IT(&huart2, (uint8_t*) INSERT_ERROR_ALARM_DURATION,
					strlen(INSERT_ERROR_ALARM_DURATION));
		}
		break;

	case CHANGE_CONFIG:
		if (check_pin(buffer_new_conf, conf.user_pin, 0)) {
			HAL_UART_Transmit(&huart2, (uint8_t*) CHANGING_CONF,
								strlen(CHANGING_CONF),
			HAL_MAX_DELAY);
			protocol_state=INSERT_PIN;

			HAL_UART_Transmit(&huart2,(uint8_t *)INSERT_NEW_PIN,strlen(INSERT_NEW_PIN),HAL_MAX_DELAY);
			HAL_UART_Receive_IT(&huart2, (uint8_t*) buffer_pin, 4);
		} else {
			protocol_state = TIME_CONFIGURATION_END;
			HAL_UART_Transmit_IT(&huart2, (uint8_t*) CHANGING_CONF_ERROR,
							strlen(CHANGING_CONF_ERROR));
		}

	default:
		break;
	}

}


/**
 * @brief 			: this function, based on the selection parameter, checks whether the pin inserted by the user is correct and then enable it to change
 * 					  the configuration or insert the commands to manage the system.
 * @param buff		: represents the set of values entered by the user.
 * @param pin		: represents the current right pin of the configuration.
 * @param selection : this parameter can be 0 to change configuration or 1 to insert the commands.
 * @return			: this function return 1 if the check was successful, otherwise return 0.
 */
uint8_t check_pin(uint8_t buff[7], uint8_t pin[4], uint8_t selection){
	if (selection)
		return  (buff[1] == pin[0]
					&& buff[2] == pin[1]
					&& buff[3] == pin[2]
					&& buff[4] == pin[3]);
	else{
	return  (buff[0] == pin[0]
					&& buff[1] == pin[1]
					&& buff[2] == pin[2]
					&& buff[3] == pin[3]);
	}
}

/**
 * @brief	: this function evaluate the buffer for giving a right value for the insertions.
 * @param b : represents the buffer to be evaluated.
 * @return  : the correct value that can be used.
 */
uint8_t evaluate_buffer(uint8_t b[2]){
	if ( b[0] == '0') {
		return  (uint8_t)(buffer_area_delay[1] - '0');
	} else {
		return  (uint8_t)((b[0] - '0') * 10 + b[1] - '0');
	}
}

/**
 * @brief 						: this function sets the correct values inserted by the user in the configuration.
 * @param conf					: represent the current configuration.
 * @param buffer_area_delay		: represent the area delay inserted by the user.
 * @param buffer_barrier_delay	: represent the barrier delay inserted by the user.
 * @param buffer_alarm_duration	: represent the alarm duration inserted by the user.
 */
void set_configuration_devices(_configuration *conf, uint8_t buffer_area_delay[2], uint8_t buffer_barrier_delay[2], uint8_t buffer_alarm_duration[2]) {


	conf->area_alarm_delay = evaluate_buffer(buffer_area_delay);
	if (conf->area_alarm_delay > 30) {
		HAL_UART_Transmit(&huart2, (uint8_t*)AREA_DELAY_SETTED_MAX_30 , strlen(AREA_DELAY_SETTED_MAX_30), HAL_MAX_DELAY);
		conf->area_alarm_delay = 30;
	}

	conf->barrier_alarm_delay = evaluate_buffer(buffer_barrier_delay);
	if (conf->barrier_alarm_delay > 30) {
		HAL_UART_Transmit(&huart2, (uint8_t*)(BARRIER_DELAY_SETTED_MAX_30) , strlen(BARRIER_DELAY_SETTED_MAX_30), HAL_MAX_DELAY);
		conf->barrier_alarm_delay = 30;
	}

	conf->alarm_duration = evaluate_buffer(buffer_alarm_duration);
	if (conf->alarm_duration > 60) {
		HAL_UART_Transmit(&huart2,( uint8_t*)(DELAY_SETTED_MAX_60) , strlen(DELAY_SETTED_MAX_60), HAL_MAX_DELAY);
		conf->alarm_duration = 60;
	}

}
/**
 * @brief 			: this function verifies that the pin values inserted by the user contain only numeric values.
 * @param buffer	: represents the pin values inserted by the user.
 * @param len		: represents the number of values inserted.
 * @return			: return 1 if the pin values inserted is all only numeric values, otherwise return 0.
 */
uint8_t check_buffer_value(uint8_t buffer[4], uint8_t len) {

	for (int i = 0; i < len; i++) {
		if (buffer[i] < 48 || buffer[i] > 57) {
			return 0;
		}
	}
	return 1;
}

/**
 * @brief			: this function sets the values present in the current configuration to the sensors.
 * @param conf		: represents the current configuration.
 * @param area		: represents the area sensor.
 * @param barrier	: represents the barrier sensor.
 * @param buzzer	: represents the buzzer.
 */
void set_device(_configuration *conf, _sensor_HandleTypeDef *area, _sensor_HandleTypeDef*barrier, _buzzer_HandleTypeDef *buzzer){
	area->delay = conf->area_alarm_delay;
	barrier->delay = conf->barrier_alarm_delay;
	buzzer->duration = conf->alarm_duration;
}
/**
 * @brief 	: this function waits for the deletion command to be entered and checks whether to delete the memory or not.
 */
void reset_memory_config(){
	HAL_UART_Transmit(&huart2, (uint8_t*) RESET_MEMORY_CONFIG,
			strlen(RESET_MEMORY_CONFIG), HAL_MAX_DELAY);
	HAL_UART_Receive(&huart2, read_buffer_reset, 1, HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, read_buffer_reset, 1, HAL_MAX_DELAY);
	if (read_buffer_reset[0] == 's') {
		erase_configuration();
	}
}



