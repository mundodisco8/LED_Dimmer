/*
 * buttons.h
 *
 *  Created on: 11 Nov 2022
 *      Author: joel.santos
 */

#ifndef INC_BUTTONS_H_
#define INC_BUTTONS_H_

#include "inttypes.h"

void configureGPIO(void);

uint32_t getRotary(void);
uint32_t getButton(void);

#endif /* INC_BUTTONS_H_ */
