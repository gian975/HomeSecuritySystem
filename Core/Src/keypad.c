/*
 * keypad.h
 *
 *  Created on: May 27, 2020
 *      Author: GGLV
 */

#include "keypad.h"
#include "gpio.h"
#include "usart.h"
#include "protocol.h"

#define BUFFER_SIZE (7)


uint8_t read_buffer[BUFFER_SIZE];
uint8_t count = 0;
KEYPAD_button_t key;
static uint16_t millis = 0;

extern _protocol_state protocol_state;
static KEYPAD_button_t KeypadStatus = KEYPAD_button_NOPRESSED;



/*Private Functions*/
uint16_t check_millis(void);
void reset_count(void);
void KEYPAD_select_colums(uint8_t column);
uint8_t KEYPAD_check_row(uint8_t column);
uint8_t KEYPAD_read(void);


uint8_t KEYPAD_INT_Buttons[4][4] = {
		{ '1', '2', '3', 'A' },
		{ '4', '5', '6', 'B' },
		{ '7', '8', '9', 'C' },
		{ '*', '0', '#', 'D' }, };



/**
 * @brief	: This function return the value of milliseconds spent from the last reading
 * @return the value
 */
uint16_t check_millis(void) {
	return millis;
}

void reset_count(void) {
	count = 0;
}


/**
 * @brief 		: This function initializes the device. Set the outputs pin(colums) to high and inputs pin (rows) to low.
 */
void KEYPDAD_Init(void) {
	HAL_GPIO_WritePin(C1_PORT, C1_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(C2_PORT, C2_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(C3_PORT, C3_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(C4_PORT, C4_PIN, GPIO_PIN_SET);

	KEYPAD_select_colums(0);
}

/**
 * @brief 		 : This function sets the column specified to low.
 * @param column : specifies the column to set low.
 */
void KEYPAD_select_colums(uint8_t column) {
	/*Set colums high*/
	HAL_GPIO_WritePin(C1_PORT, C1_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(C2_PORT, C2_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(C3_PORT, C3_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(C4_PORT, C4_PIN, GPIO_PIN_SET);

	/* Set column low */
	if (column == 1) {
		HAL_GPIO_WritePin(C1_PORT, C1_PIN, GPIO_PIN_RESET);
	}
	if (column == 2) {
		HAL_GPIO_WritePin(C2_PORT, C2_PIN, GPIO_PIN_RESET);
	}
	if (column == 3) {
		HAL_GPIO_WritePin(C3_PORT, C3_PIN, GPIO_PIN_RESET);
	}
	if (column == 4) {
		HAL_GPIO_WritePin(C4_PORT, C4_PIN, GPIO_PIN_RESET);
	}
}

/**
 * @brief 		 : This function returns the key pressed based on the row read based on the colomn low.
 * @param column : Specifies the colums low .
 * @return The key pressed that is an element of the matrix KEYPAD_INT_Buttons.
 */
uint8_t KEYPAD_check_row(uint8_t column) {
	/* Read rows */

	/* Scan row 1 */
	if (HAL_GPIO_ReadPin(R1_PORT, R1_PIN) == 0) {
		return KEYPAD_INT_Buttons[0][column - 1];
	}
	/* Scan row 2 */
	if (HAL_GPIO_ReadPin(R2_PORT, R2_PIN) == 0) {
		return KEYPAD_INT_Buttons[1][column - 1];
	}
	/* Scan row 3 */
	if (!HAL_GPIO_ReadPin(R3_PORT, R3_PIN)) {
		return KEYPAD_INT_Buttons[2][column - 1];
	}
	/* Scan row 4 */
	if (!HAL_GPIO_ReadPin(R4_PORT, R4_PIN)) {
		return KEYPAD_INT_Buttons[3][column - 1];
	}

	/* Not pressed */
	return KEYPAD_NO_PRESSED ;
}

/**
 * @brief 		: This function pulls all four columns and
 * 				then reads the input states of each row to define the key pressed  or the KEY_PAD_NO_PRESSED.
 * @return The button key pressed.
 */
uint8_t KEYPAD_read(void) {
	uint8_t check;
	/* Set column 1 to LOW */
	KEYPAD_select_colums(1);
	/* Check row */
	check = KEYPAD_check_row(1);
	if (check != KEYPAD_NO_PRESSED) {
		return check;
	}

	/* Set column 2 to LOW */
	KEYPAD_select_colums(2);
	/* Check row */
	check = KEYPAD_check_row(2);
	if (check != KEYPAD_NO_PRESSED) {
		return check;
	}

	/* Set column 3 to LOW */
	KEYPAD_select_colums(3);
	/* Check row */
	check = KEYPAD_check_row(3);
	if (check != KEYPAD_NO_PRESSED) {
		return check;
	}
	/* Set column 4 to LOW */

	KEYPAD_select_colums(4);
	/* Check rows */
	check = KEYPAD_check_row(4);
	if (check != KEYPAD_NO_PRESSED) {
		return check;

	}

	/* Not pressed */
	return KEYPAD_NO_PRESSED ;
}

/**
 * @brief 		: This function reads the status of keypad that is a value of key_button_t enumeration.
 * @return
 */
KEYPAD_button_t KEYPAD_Read_status(void) {
	KEYPAD_button_t temp;

	/* Get keypad status */
	temp = KeypadStatus;

	/* Reset keypad status */
	KeypadStatus = KEYPAD_button_NOPRESSED;

	return temp;
}

/**
 * @brief 		: This function updates the status of the keypad.
 */
void KEYPAD_Update(void) {

	/* Every KEYPAD_READ_INTERVAL ms read */
	if (++millis >= KEYPAD_READ_INTERVAL
			&& KeypadStatus == KEYPAD_button_NOPRESSED) {

		/* Reset */
		millis = 0;

		/* Read keyboard */
		KeypadStatus = (KEYPAD_button_t) KEYPAD_read();

	}
}

/**
 * @brief 		: This function reads from the keypad and puts the key pressed in a buffer.
 */
void KEYPAD_Handler_Press(void) {

	if (count < BUFFER_SIZE) {
		key = KEYPAD_Read_status();
		if (key != KEYPAD_button_NOPRESSED) {
			read_buffer[count] = (uint8_t) key;
			count++;
		}

		if (count == BUFFER_SIZE) {
			if (protocol_state == INACTIVE_PIN) {
				protocol_state = ACTIVATION_PIN;
				HAL_UART_Transmit_IT(&huart2, read_buffer, BUFFER_SIZE);
			}
		}
	}
}

