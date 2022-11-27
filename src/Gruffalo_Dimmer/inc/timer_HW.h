#ifndef _TIMER_HW_H
#define _TIMER_HW_H

#include "inttypes.h"

#include "gpio_HW_types.h"
#include "timer_HW_types.h"

// Initalises the HW clock of TIMER0
void initTimer0Clock(void);
// Sets a pin as output and associates a GPIO pin with one of the Compare/Capture channels of TIMER0
timerStatus_t setCCChannelPin(pinPort_t pinPort, uint32_t pinNo, CCChannel_t channel);
// Configures the Capture/Compare module in PWM mode for one of the CC channels of TIMER0
void configCCChannelPWM(CCChannel_t channel, polarity_t polarity);
void startTimer0(void);
void startChannelPWM(CCChannel_t channel);
void stopChannelPWM(CCChannel_t channel);

void timerModuleEnable(void);
void timerModuleDisable(void);

// Sets the value of the Compare Register (unbuffered)
timerStatus_t setChannelOutputCompare(CCChannel_t channel, uint32_t compareValue);
// Sets the value of the buffered Compare Register (buffered). The change will be effective on the
timerStatus_t setChannelBufferedOutputCompare(CCChannel_t channel, uint32_t compareValue);
// Set the value of the TOP register on TIMER0
timerStatus_t setTimer0TopValue(uint32_t top);
// Gets the value of the TOP register on TIMER0
uint32_t getTimer0TopValue(void);
// Returns the clock frequency of TIMER0
uint32_t getTimerFrequency(void);
#endif  // _TIMER_HW_H
