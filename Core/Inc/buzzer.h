/*
 * buzzer.h
 *
 *  Created on: May 25, 2020
 *      Author: vince
 */

#ifndef SRC_BUZZER_H_
#define SRC_BUZZER_H_

#include "main.h"

#define AREA_BARRIER_FACTOR 1
#define AREA_FACTOR 4
#define BARRIER_FACTOR 2
#define PERIOD_ONE_SECOND 999


extern _buzzer_HandleTypeDef buzzer;


void BUZZER_Init(void);
void BUZZER_sound(_buzzer_HandleTypeDef*);
void BUZZER_Handler_State(_buzzer_HandleTypeDef*);
void BUZZER_Handler_Sound(_buzzer_HandleTypeDef*);
void BUZZER_Bip(_buzzer_HandleTypeDef*);


#endif /* SRC_BUZZER_H_ */
