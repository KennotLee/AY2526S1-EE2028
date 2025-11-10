/*
 * DipSwitch.c
 *
 *  Created on: Oct 27, 2025
 *      Author: mushr
 */
#include "DipSwitch.h"

extern I2C_HandleTypeDef hi2c1;

uint8_t DS_Read(){
	uint8_t data = 0;

	if (HAL_I2C_Master_Receive(&hi2c1, DS_addr,&data, 1 , 100) == HAL_OK){
		return data &0x3F;
	}
	return 0;
}

uint8_t DS_GetPos(uint8_t idx){

	if (idx >= DS_count){
		return 0;
	}

	uint8_t value = DS_Read();
	return (value >> idx) & 0x01;
}
