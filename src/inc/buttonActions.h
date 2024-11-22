#ifndef _BUTTON_ACTIONS_H_
#define _BUTTON_ACTIONS_H_

#include "inttypes.h"

void gpioCallbackButton(uint8_t intNo, void* ctx);
void gpioCallbackQuad(uint8_t intNo, void* ctx);

void button0Pressed(void);
void button0Released(void);
void quad0ClockWise(void);
void quad0CounterClockWise(void);

uint32_t getRotary(void);
uint32_t getButton(void);

#endif // _BUTTON_ACTIONS_H_
