/*
 * eeprom.h
 *
 *  Created on: Jun 1, 2020
 *      Author: GGLV
 */

#ifndef INC_EEPROM_H_
#define INC_EEPROM_H_
#define CONFIGURATION_SIZE 14

#include "configuration.h"
#include "i2c.h"
#include <stdio.h>

extern uint8_t eeprom_data[14];
uint8_t EEPROM_Init(void);
void save_configuration(_configuration*);
void read_configuration(void);
void load_configuration_saved(_configuration*, uint8_t*);
void erase_configuration();

#endif /* INC_EEPROM_H_ */
