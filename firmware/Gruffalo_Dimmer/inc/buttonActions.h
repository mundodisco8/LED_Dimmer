#ifndef _BUTTON_ACTIONS_H_
#define _BUTTON_ACTIONS_H_

#include "buttons.h"
#include "inttypes.h"

void gpioCallbackButton(uint8_t intNo, void* ctx);
void gpioCallbackQuad(uint8_t intNo, void* ctx);

void button0Pressed(void* ctx);
void button0Released(void* ctx);
void quad0ClockWise(void* ctx);
void quad0CounterClockWise(void* ctx);

uint32_t getRotary(void);
uint32_t getButton(void);

#endif  // _BUTTON_ACTIONS_H_
