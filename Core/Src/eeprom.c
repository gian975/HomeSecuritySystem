/*
 * eeprom.c
 *
 *  Created on: Jun 1, 2020
 *      Author: GGLV
 */
#include <stdio.h>
#include "eeprom.h"

#define EEPROM_ADDRESS	0xA0
#define MEMORY_ADDRESS  0 // sempre 0 perchè la memoria sarà usato solo pe salvare la configurazione
#define MAX_ATTEMPTS 3
#define EEPROM_ERR -1
#define EEPROM_OK 0

/* Useful variables during communication */
uint8_t eeprom_data[CONFIGURATION_SIZE];
extern _configuration conf;


/**
 * @brief		: This function stores in memory a configuration, converted in hexadecimal values.
 * @param conf	: Pointer to a _configuration structure that contains the configuration informations
 */
void save_configuration(_configuration* conf){

	eeprom_data[0] = '[';
	eeprom_data[1] = dec2Bcd(conf->date_time.seconds);
	eeprom_data[2] = dec2Bcd(conf->date_time.minutes);
	eeprom_data[3] = dec2Bcd(conf->date_time.hours);
	eeprom_data[4] = dec2Bcd(conf->date_time.date);
	eeprom_data[5] = dec2Bcd(conf->date_time.month);
	eeprom_data[6] = dec2Bcd(conf->date_time.year);
	eeprom_data[7] = dec2Bcd(conf->user_pin[0]);
	eeprom_data[8] = dec2Bcd(conf->user_pin[1]);
	eeprom_data[9] = dec2Bcd(conf->user_pin[2]);
	eeprom_data[10] = dec2Bcd(conf->user_pin[3]);
	eeprom_data[11] = dec2Bcd(conf->barrier_alarm_delay);
	eeprom_data[12] = dec2Bcd(conf->area_alarm_delay);
	eeprom_data[13] = dec2Bcd(conf->alarm_duration);

	/* Write in EEPROM */
	if(HAL_I2C_Mem_Write_DMA(&hi2c1, (uint16_t)EEPROM_ADDRESS, MEMORY_ADDRESS, I2C_MEMADD_SIZE_16BIT, eeprom_data, 14)!= HAL_OK)
	{
		/* Writing process Error */
		Error_Handler();
	}

}


/**
 * @brief 		: This function erases a configuration from the eeprom memory
 */
void erase_configuration(){
	// buffer of void characters
	uint8_t data_erase[15]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
	if(HAL_I2C_Mem_Write(&hi2c1, (uint16_t)EEPROM_ADDRESS, MEMORY_ADDRESS, I2C_MEMADD_SIZE_16BIT, data_erase, 14,HAL_MAX_DELAY)!= HAL_OK)
		{
			/* Writing process Error */
			Error_Handler();
		}
}


/**
 * @brief: 		This function read a configuration from memory and stores it in eeprom_data buffer.
 */
void read_configuration(void){

	int returnvalue=0;
	returnvalue = HAL_I2C_Mem_Read_DMA(&hi2c1, (uint16_t)EEPROM_ADDRESS, MEMORY_ADDRESS, I2C_MEMADD_SIZE_16BIT, eeprom_data,14);
	if(returnvalue != HAL_OK)
	{
		/* Reading process Error */
		Error_Handler();
	}
}


/**
 * @brief	 	: This function compiles the fields of configuration structure with the values red from the eeprom memory
 * @param conf  : Pointer to a _buzzer_HandleTypeDef structure that has to contains the configuration information loaded from eeprom
 * @param data  : Pointer to the data red from eeprom
 */
void load_configuration_saved(_configuration* conf, uint8_t* data){
	conf->date_time.seconds = bcd2Dec(data[1]);
	conf->date_time.minutes = bcd2Dec(data[2]);
	conf->date_time.hours = bcd2Dec(data[3]);
	conf->date_time.date = bcd2Dec(data[4]);
	conf->date_time.month = bcd2Dec(data[5]);
	conf->date_time.year = bcd2Dec(data[6]);
	conf->user_pin[0] = bcd2Dec(data[7]);
	conf->user_pin[1] = bcd2Dec(data[8]);
	conf->user_pin[2] = bcd2Dec(data[9]);
	conf->user_pin[3]= bcd2Dec(data[10]);
	conf->barrier_alarm_delay= bcd2Dec(data[11]);
	conf->area_alarm_delay= bcd2Dec(data[12]);
	conf->alarm_duration = bcd2Dec(data[13]);
}


/**
 * @brief 		: This function initializes the eeprom memory device
 * @retval 		EEPROM_OK in case of device ready
 * @retval 		EEPROM_ERR in case of device not ready
 */
uint8_t EEPROM_Init()
{
	HAL_StatusTypeDef returnValue;

	returnValue = HAL_I2C_IsDeviceReady(&hi2c1, EEPROM_ADDRESS, MAX_ATTEMPTS, HAL_MAX_DELAY);

	if(returnValue != HAL_OK)
	{
		return EEPROM_ERR;
	}

	return EEPROM_OK;
}


