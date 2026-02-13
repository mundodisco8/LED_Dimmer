#ifndef _PWMCONTROL_H_
#define _PWMCONTROL_H_

#include <inttypes.h>
#include <stdbool.h>

#include "gpio_HW_types.h"
#include "timer_HW_types.h"

// TODO: THis is probably bad
extern volatile uint32_t dutyCycle0;
extern volatile uint32_t dutyCycle1;
extern volatile uint32_t dutyCycle2;

////
// Defines and Consts
////

#define PWM_CHANNELS 3UL

/**
 * @brief Max and min brightness  (in the case of overflow)
 */
#define MAX_BRIGHTNESS 10000UL
#define MIN_BRIGHTNESS 0UL

/**
 * @brief
 *
 */
extern const uint32_t PWM_FREQUENCY;

/**
 * @brief Start TIMER0's HW in PWM mode
 */
void initTimer0PWM(const uint32_t PWMFreqHz);

/*
 * @brief Starts the CC module of a channel of TIMER0 as PWM mode.
 */
void initTimer0CCChannel(const CCChannel_t channel, const pinPort_t port, const uint8_t pinNo,
                         const polarity_t polarity);

/**
 * @brief Gets the freq of the PWM signal, based on the current clock freq and the TOP value
 */
uint32_t getPWMFrequency(void);

/**
 * @brief Sets the Duty Cycle of the PWM signal on one of TIMER0's channels. Brightness is gamma-corrected
 * NOTE: expects TIMER- to be enabled when called
 */
void setDutyCycle(const CCChannel_t channel, uint32_t percent, bool setBuffered);
#endif  // _PWMCONTROL_H_
