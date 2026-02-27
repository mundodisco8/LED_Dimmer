/*
 * buttons.h
 *
 *  Created on: 11 Nov 2022
 *      Author: joel.santos
 */

#ifndef _BUTTONS_H_
#define _BUTTONS_H_

#include <inttypes.h>
#include <stddef.h>

#include "interrupt_HW_types.h"

// Some types, extracted to their own header so they can break circular dependencies when included elsewhere.
#include "buttons_types.h"

/**
 * Globals
 */

/**
 * @brief Debounce control globals
 */
extern const uint32_t DEBOUNCE_TIME_MS;             // Max time for the button to settle in the new state
extern const uint32_t DEBOUNCE_SAMPLING_PERIOD_MS;  // read the button every this ms
extern const int32_t INTEGRATOR_TARGET;             // Button is stable after SAMPLING * TARGET ms

/**
 * @brief Time required to consider a press a longpress
 */
extern const uint32_t LONGPRESS_TIME_MS;

// Initialises the button_t struct with the default values and associates the pressed and released actions.
btnError_t initButton(button_t* btnPtr, pinPort_t pinPort, uint8_t pinNo, actionCallback_t pressedAction,
                      actionCallback_t releasedAction);
btnError_t initQuadEncoder(quad_encoder_t* quadPtr, pinPort_t pin0Port, uint8_t pin0No, pinPort_t pin1Port,
                           uint8_t pin1No, actionCallback_t CWAction, actionCallback_t CCWAction);

// Configures the interrupt for the button and sets its callback
btnError_t configureButtonInterrupts(button_t* btnPtr, callbackCtxPtr_t callback, uint32_t* intNoPtr);
// Configures the interrupt for a quadrature encoder.
btnError_t configureQuadratureInterrupts(quad_encoder_t* quadPtr, callbackCtxPtr_t callback, uint32_t* intNoPtr);
/**
 * @brief Start the timers of a button.
 */
btnError_t startButtonTimer(button_t* btnPtr, const btnTimerType_t timerType);
#endif /* _BUTTONS_H_ */
