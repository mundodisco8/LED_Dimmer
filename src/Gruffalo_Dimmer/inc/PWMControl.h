#ifndef _PWMCONTROL_H_
#define _PWMCONTROL_H_

#include "inttypes.h"

#include "gpio_HW_types.h"
#include "timer_HW_types.h"

// Start TIMER0's HW
void initTimer0HW(void);
void runTimer0(void);
// Starts the CC module of a channel of TIMER0 as PWM mode.
void initTimer0CCChanel(CCChannel_t channel, pinPort_t port, uint32_t pinNo, uint32_t PWMFreqHz, polarity_t polarity);

// Sets the PWM signal frequency by adjusting the TOP value of the counter
void configureTimerPWMFrequency(uint32_t frequencyHz);

// Sets the Duty Cycle of the PWM signal on one of TIMER0's channels
void setDutyCycle(CCChannel_t channel, uint8_t percent);
// Sets the brightness level for one of TIMER0's channels. The brighness is adjusted using gamma correction
void setBrightness(CCChannel_t channel, uint8_t percent);

#endif // _PWMCONTROL_H_