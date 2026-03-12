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
