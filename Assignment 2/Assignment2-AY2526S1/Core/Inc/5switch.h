/*
 * 5switch.h
 *
 *  Created on: Oct 27, 2025
 *      Author: mushr
 */

#ifndef SWITCH_5SWITCH_H_
#define SWITCH_5SWITCH_H_

#include <stdint.h>
#include "stm32l4xx_hal.h"

#define Switch_addr 0x03 << 1

typedef enum{
	TOP,
	LEFT,
	RIGHT,
	BOTTOM,
	CENTER,
	NONE
} S_Direction;

S_Direction Switch_GetDir(void);


#endif /* SWITCH_5SWITCH_H_ */
