
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "i2c.h"
#include "realtimeclock.h"
#include "usart.h"

uint8_t rtc_data[DATE_SIZE];
date_time_t date_time;

uint8_t months[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

/*Private functions*/
uint8_t bcd2Dec(uint8_t);
uint8_t dec2Bcd(uint8_t);
int give_me_month(void);

/* Conversion from BCD to decimal */
uint8_t bcd2Dec(uint8_t val) {
	uint8_t res = ((val / 16 * 10) + (val % 16));
	return res;
}

/* Conversion from decimal to BCD */
uint8_t dec2Bcd(uint8_t val) {
	uint8_t res = ((val / 10 * 16) + (val % 10));
	return res;
}

/**
 * This function handles months.
 * @return The value of the corresponding month.
 */
int give_me_month(void) {

	if (__DATE__[0] == 'J' && __DATE__[1] == 'a' && __DATE__[2] == 'n')
		return 1;
	else if (__DATE__[0] == 'F' && __DATE__[1] == 'e' && __DATE__[2] == 'b')
		return 2;
	else if (__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'r')
		return 3;
	else if (__DATE__[0] == 'A' && __DATE__[1] == 'p' && __DATE__[2] == 'r')
		return 4;
	else if (__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'y')
		return 5;
	else if (__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'n')
		return 6;
	else if (__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'l')
		return 7;
	else if (__DATE__[0] == 'A' && __DATE__[1] == 'u' && __DATE__[2] == 'g')
		return 8;
	else if (__DATE__[0] == 'S' && __DATE__[1] == 'e' && __DATE__[2] == 'p')
		return 9;
	else if (__DATE__[0] == 'O' && __DATE__[1] == 'c' && __DATE__[2] == 't')
		return 10;
	else if (__DATE__[0] == 'N' && __DATE__[1] == 'o' && __DATE__[2] == 'v')
		return 11;
	else if (__DATE__[0] == 'D' && __DATE__[1] == 'e' && __DATE__[2] == 'c')
		return 12;
	else
		return 0;
}

/**
 * @brief: This function sets the current date.
 * @param datatime : Pointer to a date_time_t structure that contains the information of date and hour.
 */
void set_current_date(date_time_t *datatime) {

	datatime->seconds = BUILD_CURRENT_SECONDS;
	datatime->minutes = BUILD_CURRENT_MINUTES;
	datatime->hours = BUILD_CURRENT_HOURS;
	datatime->date = BUILD_CURRENT_DATE;
	datatime->month = give_me_month();
	datatime->year = BUILD_CURRENT_YEAR;

}

/**
 * @brief: This function initializes the device.
 * @retval DS1307_OK if the device is ready
 * @retval DS1307_ERR if the device is not ready
 */
uint8_t RTC_Init() {
	HAL_StatusTypeDef returnValue;

	returnValue = HAL_I2C_IsDeviceReady(&hi2c1, DS1307_ADDRESS, MAX_RETRY,
			HAL_MAX_DELAY);

	if (returnValue != HAL_OK) {
		return DS1307_ERR;
	}

	return DS1307_OK;
}

/**
 * @brief: This function reads the date from the registers of the DS1307_RTC.
 * @param datetime : Pointer to a date_time_t structure that contains the information of date and hour.
 * @retval DS1307_OK if the device is ready and the reading was successfull.
 * @retval DS1307_ERR if the device is not ready
 */
int8_t rtc_get_date_time(date_time_t *datetime) {
	HAL_StatusTypeDef returnValue;

	returnValue = HAL_I2C_Mem_Read_DMA(&hi2c1, DS1307_ADDRESS, DS1307_SECONDS,
			ADDRESS_SIZE, (rtc_data), DATE_SIZE);

	if (returnValue != HAL_OK) {
		return DS1307_IC2_ERR;
	}
	return DS1307_OK;

}

/**
 * @brief: This function writes the date in the registers of the DS1307_RTC.
 * @param datetime : Pointer to a date_time_t structure that contains the information of date and hour.
 * @retval DS1307_OK if the device is ready and the writing was successfull.
 * @retval DS1307_ERR if the device is not ready
 */
int8_t rtc_set_date_time(date_time_t *datetime) {
	HAL_StatusTypeDef returnValue;

	rtc_data[0] = dec2Bcd(datetime->seconds);
	rtc_data[1] = dec2Bcd(datetime->minutes);
	rtc_data[2] = dec2Bcd(datetime->hours);
	rtc_data[4] = dec2Bcd(datetime->date);
	rtc_data[5] = dec2Bcd(datetime->month);
	rtc_data[6] = dec2Bcd(datetime->year);

	returnValue = HAL_I2C_Mem_Write(&hi2c1, DS1307_ADDRESS, DS1307_SECONDS,
			ADDRESS_SIZE, (rtc_data), DATE_SIZE, HAL_MAX_DELAY);
	if (returnValue != HAL_OK) {
		return DS1307_IC2_ERR;
	}

	return DS1307_OK;

}

/**
 * @brief: This function sets a date of default.
 * @param datetime: Pointer to a date_time_t structure that contains the information of date and hour.
 */

void rtc_set_date_time_default(date_time_t *datetime) {

	datetime->seconds = 0;
	datetime->minutes = 00;
	datetime->hours = 00;
	datetime->date = 01;
	datetime->month = 1;
	datetime->year = 01;

}
