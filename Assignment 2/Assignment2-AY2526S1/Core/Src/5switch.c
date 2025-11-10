/*
 * 5switch.c
 *
 *  Created on: Oct 27, 2025
 *      Author: mushr
 */
#include "5switch.h"
#include "main.h"
#include <stdio.h>

extern I2C_HandleTypeDef hi2c1;
S_Direction current_dir = NONE;

S_Direction Switch_GetDir(void){
	uint8_t buffer[9];

	HAL_StatusTypeDef result = HAL_I2C_Mem_Read(&hi2c1, Switch_addr, 0x01, I2C_MEMADD_SIZE_8BIT, buffer, 9, HAL_MAX_DELAY);

	if (result != HAL_OK){
		return NONE;
	}

	if ((buffer[4] & 0x01) == 0) return TOP;
	if ((buffer[5] & 0x01) == 0) return LEFT;
	if ((buffer[6] & 0x01) == 0) return BOTTOM;
	if ((buffer[7] & 0x01) == 0) return RIGHT;
	if ((buffer[8] & 0x01) == 0) return CENTER;

	return NONE;
}
