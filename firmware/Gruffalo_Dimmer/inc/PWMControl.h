#ifndef _PWMCONTROL_H_
#define _PWMCONTROL_H_

#include <inttypes.h>

#include "gpio_HW_types.h"
#include "timer_HW_types.h"

extern volatile uint32_t dutyCycle0;
extern volatile uint32_t dutyCycle1;
extern volatile uint32_t dutyCycle2;

// Start TIMER0's HW
void initTimer0PWM(uint32_t PWMFreqHz);
// Starts the CC module of a channel of TIMER0 as PWM mode.
void initTimer0CCChannel(CCChannel_t channel, pinPort_t port, uint8_t pinNo, polarity_t polarity);

// Sets the PWM signal frequency by adjusting the TOP value of the counter
void configureTimerPWMFrequency(uint32_t frequencyHz);

// Sets the Duty Cycle of the PWM signal on one of TIMER0's channels
void setDutyCycle(CCChannel_t channel, int8_t percent);
// Gets the COUNT value for the requested channel
uint32_t getDutyCycle(CCChannel_t channel);
// Sets the brightness level for one of TIMER0's channels. The brighness is adjusted using gamma correction
void setBrightness(CCChannel_t channel, int8_t percent);

#endif  // _PWMCONTROL_H_
