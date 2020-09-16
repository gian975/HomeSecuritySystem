/*
 * keypad.h
 *
 *  Created on: May 27, 2020
 *      Author: GGLV
 */



#include "gpio.h"
#include "usart.h"



#ifndef INC_KEYPAD_H_
#define INC_KEYPAD_H_


/*Colums are oututs*/
/*Column 1 default */
#define C1_PORT GPIOB
#define C1_PIN GPIO_PIN_1

/*Column 2 default */
#define C2_PORT GPIOB
#define C2_PIN GPIO_PIN_12

/*Column 3 default */
#define C3_PORT GPIOB
#define C3_PIN GPIO_PIN_13

/*Column 4 default */
#define C4_PORT GPIOB
#define C4_PIN GPIO_PIN_14


/*Rows are inputs*/
/*Row 1 default */
#define R1_PORT GPIOA
#define R1_PIN GPIO_PIN_6

/*Row 2 default */
#define R2_PORT GPIOA
#define R2_PIN GPIO_PIN_4

/*Row 3 default */
#define R3_PORT GPIOA
#define R3_PIN GPIO_PIN_8

/*Row 4 default */
#define R4_PORT GPIOA
#define R4_PIN GPIO_PIN_9


/*Number of milliseconds between 2 reads*/
#define KEYPAD_READ_INTERVAL 100


#define KEYPAD_NO_PRESSED (uint8_t)0xFF


/**
 * @brief:	Keypad button enumeration
 */
typedef enum{
    KEYPAD_button_0='0',
    KEYPAD_button_1='1',
    KEYPAD_button_2='2',
    KEYPAD_button_3='3',
    KEYPAD_button_4='4',
    KEYPAD_button_5='5',
    KEYPAD_button_6='6',
    KEYPAD_button_7='7',
    KEYPAD_button_8='8',
    KEYPAD_button_9='9',
    KEYPAD_button_STAR='*',
    KEYPAD_button_HASH='#',
    KEYPAD_button_A='A',
    KEYPAD_button_B='B',
    KEYPAD_button_C='C',
    KEYPAD_button_D='D',
    KEYPAD_button_NOPRESSED = KEYPAD_NO_PRESSED
} KEYPAD_button_t;



void KEYPDAD_Init(void);
KEYPAD_button_t KEYPAD_read(void);
KEYPAD_button_t KEYPAD_Read_status(void);
void KEYPAD_Handler_Press(void);
void KEYPAD_Update(void);
uint16_t check_millis(void);





#endif /* INC_KEYPAD_H_ */
