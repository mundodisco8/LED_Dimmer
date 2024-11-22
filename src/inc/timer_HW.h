#ifndef _TIMER_HW_H
#define _TIMER_HW_H

#include <inttypes.h>
#include <stdbool.h>

#include "gpio_HW_types.h"
#include "timer_HW_types.h"

extern volatile float dutyCycle0;
extern volatile float dutyCycle1;
extern volatile float dutyCycle2;

// Initalises the HW clock of TIMER0
void TIMHW_initTimer0Clock(void);
// Initialises TIMER0
void TIMHW_initTimer0(bool startTimerOnInit);

// Sets a pin as output and associates a GPIO pin with one of the Compare/Capture channels of TIMER0
timerStatus_t TIMHW_setCCChannelPin(pinPort_t pinPort, uint8_t pinNo, CCChannel_t channel);
// Configures the Capture/Compare module in PWM mode for one of the CC channels of TIMER0
void TIMHW_configCCChannelPWM(CCChannel_t channel, polarity_t polarity);
// Enables the Compare/Capture Module for the selected channel on TIMER0
void TIMHW_enableChannelCompCapUnit(CCChannel_t channel);
// Disables the Compare/Capture Module for the selected channel on TIMER0
void TIMHW_disableChannelCompCapUnit(CCChannel_t channel);

// Starts TIMER0
void TIMHW_startTimer0(void);
// Stops TIMER0
void TIMHW_stopTimer0(void);

// Sets the value of the Compare Register (unbuffered)
timerStatus_t TIMHW_setT0ChannelOutputCompare(CCChannel_t channel, uint32_t compareValue);
// Sets the value of the buffered Compare Register (buffered). The change will be effective on the
timerStatus_t TIMHW_setT0ChannelBufferedOutputCompare(CCChannel_t channel, uint32_t compareValue);
// Set the value of the TOP register on TIMER0
timerStatus_t TIMHW_setTimer0TopValue(uint32_t top);
// Gets the value of the TOP register on TIMER0
uint32_t TIMHW_getTimer0TopValue(void);
// Returns the clock frequency of TIMER0
uint32_t TIMHW_getTimer0Frequency(void);
#endif  // _TIMER_HW_H
