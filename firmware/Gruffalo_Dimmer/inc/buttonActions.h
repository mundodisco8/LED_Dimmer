/**
 * @file buttonActions.h
 * @author Joel Santos (jsantosrico@gmail.com)
 * @brief Logic and handlers for user input events from buttons and quadrature encoders.
 * @defgroup BUTTON_ACTIONS Button Actions Module
 * @{
 *
 * ### Detailed Description
 * This module defines the high-level actions triggered by actions on
 * push-buttons and quadrature encoders. It manages the state of user interactions,
 * such as selecting active LED channels and adjusting effects parameters.
 *
 * ### Key Functionalities
 * * **Channel Selection:** Button 0 handles cycling through available PWM channels
 * (1, 2, 3, or All) to target adjustments.
 * * **Brightness Control:** Quadrature encoder 0 increases or decreases the
 * brightness of the selected channel(s) using a fixed delta (@ref BRIGHTNESS_DELTA).
 * * **Effect Management:** Button 1 cycles through available lighting animations
 * (e.g., Fixed, Breathe).
 * * **Parameter Tuning:** Quadrature encoder 1 dynamically adjusts effect-specific
 * variables, such as the period of the "Breathe" effect.
 *
 * ### Input Handling
 * This module provides callback functions designed to be registered with the GPIO
 * interrupt controller:
 * 1. @ref gpioCallbackButton: Manages debouncing and timing for push-buttons.
 * 2. @ref gpioCallbackQuad: Decodes rotation direction for quadrature encoders.
 *
 * @version 0.1
 * @date 2026-03-13
 *
 * @copyright Copyright (c) 2026
 * @date 2026-03-13
 */
#ifndef _BUTTON_ACTIONS_H_
#define _BUTTON_ACTIONS_H_

#include "buttons.h"
#include "inttypes.h"

/**
 * @brief callback to call on pushbutton interrup
 */
void gpioCallbackButton(uint8_t intNo, void* ctx);
/**
 * @brief callback to call on quad encoder interrup
 */
void gpioCallbackQuad(uint8_t intNo, void* ctx);

/**
 * @brief Action triggered on a short press of button 0
 */
void button0ShortPressed(void* ctx);
/**
 * @brief Action triggered on a long press of button 0
 */
void button0LongPressed(void* ctx);
/**
 * @brief Action triggered on release of of button 0
 */
void button0Released(void* ctx);
/**
 * @brief Action triggered on a CW rotation of quad 0
 */
void quad0ClockWise(void* ctx);
/**
 * @brief Action triggered on a CCW rotation of quad 0
 */
void quad0CounterClockWise(void* ctx);

/**
 * @brief Action triggered on a short press of button 1
 */
void button1ShortPressed(void* ctx);
/**
 * @brief Action triggered on a long press of button 1
 */
void button1LongPressed(void* ctx);
/**
 * @brief Action triggered on release of of button 1
 */
void button1Released(void* ctx);
/**
 * @brief Action triggered on a CW rotation of quad 1
 */
void quad1ClockWise(void* ctx);
/**
 * @brief Action triggered on a CCW rotation of quad 1
 */
void quad1CounterClockWise(void* ctx);

#endif  // _BUTTON_ACTIONS_H_
