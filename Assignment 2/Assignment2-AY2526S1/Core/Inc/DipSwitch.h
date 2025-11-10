/*
 * DipSwitch.h
 *
 *  Created on: Oct 27, 2025
 *      Author: mushr
 */
#include <stdint.h>
#include "../../Core/Inc/main.h"
#include "stm32l4xx_hal.h"

#ifndef DIPSWITCH_H_
#define DIPSWITCH_H_

#define DS_addr 0x03
#define DS_count 6


uint8_t DS_Read(void);
uint8_t DS_GetPos(uint8_t idx);

#endif /* DIPSWITCH_H_ */
