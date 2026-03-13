/**
 * @file PWMControl.h
 * @version 0.1
 * @brief Pulse Width Modulation (PWM) Controller for LED Brightness.
 * @defgroup PWM_CONTROL PWM Control Module
 * @{
 *
 * ### Detailed Description
 * This module provides an interface to configure and control **TIMER0** in PWM mode
 * specifically for LED intensity management. It supports up to three channels
 * (defined by @ref PWM_CHANNELS) and implements hardware-level timing control.
 *
 * ### Key Features
 * * **Gamma Correction:** Uses a pre-computed Look-Up Table (LUT) to map percentage
 * brightness to hardware compare values, ensuring linear visual perception.
 * * **Frequency Constraints:** Operates within a safe range of 250Hz to 1000Hz to
 * maintain high resolution (minimum 12-bit) and avoid flicker.
 * * **Hardware Abstraction:** Interfaces with `timer_HW` and `gpio_HW` to manage
 * registers and pin multiplexing.
 *
 * ### Initialization Sequence
 * 1. Initialize the timer hardware and set frequency: @ref initTimer0PWM.
 * 2. Configure specific Capture/Compare (CC) channels and pins: @ref initTimer0CCChannel.
 * 3. Set the desired duty cycle (0-10000): @ref setDutyCycle.
 * * @note Changing @ref PWM_FREQUENCY requires a rebuild of the Gamma LUT to maintain
 * accuracy.
 *
 * @author Joel Santos (jsantosrico@gmail.com)
 * @date 2026-03-13
 */

#ifndef _PWMCONTROL_H_
#define _PWMCONTROL_H_

#include <inttypes.h>
#include <stdbool.h>

#include "gpio_HW_types.h"
#include "timer_HW_types.h"

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
 * @brief The current PWM Frequency
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
