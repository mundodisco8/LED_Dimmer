#ifndef _BUTTON_ACTIONS_H_
#define _BUTTON_ACTIONS_H_

#include "inttypes.h"

void gpioCallbackButton1(uint8_t intNo, void* ctx);
void gpioCallbackQuad1(uint8_t intNo, void* ctx);

void button1Pressed(void);
void button1Released(void);
void quad1ClockWise(void);
void quad1CounterClockWise(void);

uint32_t getRotary(void);
uint32_t getButton(void);

#endif // _BUTTON_ACTIONS_H_
