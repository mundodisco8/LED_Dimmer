/**
 * @file buttons.h
 * @author Joel Santos (jsantosrico@gmail.com)
 * @brief Public API for button handling and quadrature encoder decoding.
 * @version 0.1
 * @date 2026-03-13
 * * @copyright Copyright (c) 2026
 * * @defgroup BUTTON_HW Button and Encoder Hardware Module
 * @{
 *
 * ### Detailed Description
 * This module provides an interface to initialize and manage physical user inputs.
 * It supports push-buttons with multi-state logic and quadrature encoders for
 * rotational input.
 *
 * ### Key Features
 * * **Software Debouncing:** Uses an integrator algorithm to filter mechanical noise.
 * * **Multi-Action Buttons:** Supports distinct callbacks for short-press, long-press,
 * and release events.
 * * **Quadrature Decoding:** Optimized for PEC11R-style encoders with 18 detents.
 *
 * ### Resource Requirements
 * * **Timers:** Each button instance requires **two reserved sleep timers** (one for
 * sampling/debouncing and one for long-press timing).
 * * **Interrupts:** Requires one GPIO interrupt per button and one per encoder A-signal.
 *
 * ### Initialization Sequence
 * 1. Initialize the structure and pins: @ref initButton or @ref initQuadEncoder.
 * 2. Configure hardware interrupts: @ref configureButtonInterrupts or @ref configureQuadratureInterrupts.
 */

#ifndef _BUTTONS_H_
#define _BUTTONS_H_

#include <inttypes.h>
#include <stddef.h>

#include "interrupt_HW_types.h"

// Some types, extracted to their own header so they can break circular dependencies when included elsewhere.
#include "buttons_types.h"

/**
 * @brief Time required to consider a press a longpress
 */
extern const uint32_t LONGPRESS_TIME_MS;

/**
 * @brief Initialises the button_t struct with the default values and associates the pressed and released actions.
 * NOTE: Each button uses two sleeptimers, so make sure you have enough!
 */
void initButton(button_t* btnPtr, const pinPort_t pinPort, const uint8_t pinNo, const actionCallback_t shortPressAction,
                const actionCallback_t longPressAction, const actionCallback_t releasedAction);
/**
 * @brief Initialises the quad_encoder_t struct with the default values and associates the CW and CCW actions.
 */
void initQuadEncoder(quad_encoder_t* quadPtr, const pinPort_t pin0Port, const uint8_t pin0No, const pinPort_t pin1Port,
                     const uint8_t pin1No, const actionCallback_t CWAction, const actionCallback_t CCWAction);

/**
 * @brief Configures the interrupt for the button and sets its callback.
 * This callback is the function to call when the GPIO interrupt triggers, but is not the action to take when the button
 * is pressed or released. This callback is the function that will determine if the button is pressed or released and
 * act accordingly
 */
void configureButtonInterrupts(button_t* btnPtr, const callbackCtxPtr_t callback, uint32_t* intNoPtr);
/**
 * @brief Configures the interrupt for the quad encoder and sets its callback.
 * This callback is the function to call when the GPIO interrupt triggers, but is not the action to take when the button
 * is pressed or released. This callback is the function that will determine if the button is pressed or released and
 * act accordingly
 */
void configureQuadratureInterrupts(quad_encoder_t* quadPtr, const callbackCtxPtr_t callback, uint32_t* intNoPtr);
/**
 * @brief Updates the button state, making sure we also
 */
void buttonSetState(button_t* btnPtr, const buttonState_t newState);

/**
 * @brief Returns the state of the button
 */
buttonState_t buttonGetState(const button_t* btnPtr);

/**
 * @brief Starts a timer on a button.
 */
btnError_t startButtonTimer(button_t* btnPtr, const btnTimerType_t timerType);
/**
 * @brief Stops a timer on a button.
 */
btnError_t stopButtonTimer(const button_t* btnPtr, const btnTimerType_t timerType);
#endif /* _BUTTONS_H_ */
