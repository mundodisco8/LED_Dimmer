/*
 * buttons.h
 *
 *  Created on: 11 Nov 2022
 *      Author: joel.santos
 */

#ifndef INC_BUTTONS_H_
#define INC_BUTTONS_H_

#include <inttypes.h>
#include <stddef.h>

#include "gpio_HW_types.h"
#include "interrupt_HW_types.h"

typedef enum btnError {
    BTN_OK = 0,
    BTN_NO_INTS_AVAILABLE = 0x1,
    BTN_NULL_POINTER_PASSED = 0x2,
} btnError_t;

// An enum to define the state of a button
typedef enum button_state {
    BUTTON_PRESSED = 0,
    BUTTON_RELEASED = 1
} button_state_t;

typedef void (*actionCallback_t)(void);

// A struct to contain the context of a button
typedef struct {
    pinPort_t btnPort;
    uint32_t pinNo;
    button_state_t state; // The state of the button, pressed or released
    int32_t integrator;  // The value of the integrator for this button
    actionCallback_t pressedAction; // action to perform when the state changes to BUTTON_PRESSED
    actionCallback_t releasedAction;// action to perform when the state changes to BUTTON_RELEASED
} button_t;

typedef struct {
    pinPort_t pin0Port;
    uint32_t pin0No;
    pinPort_t pin1Port;
    uint32_t pin1No;
    actionCallback_t clockWiseAction;
    actionCallback_t counterClockWiseAction;
}quad_encoder_t;

// Initialises the button_t struct with the default values and associates the pressed and released actions.
btnError_t initButton(button_t* btnPtr, pinPort_t pinPort, uint32_t pinNo, actionCallback_t pressedAction, actionCallback_t releasedAction);
btnError_t initQuadEncoder(quad_encoder_t* quadPtr, pinPort_t pin0Port, uint32_t pin0No, pinPort_t pin1Port, uint32_t pin1No, actionCallback_t CWAction, actionCallback_t CCWAction);

// Configures the interrupt for the button and sets its callback
btnError_t configureButtonInterrupts(button_t* btnPtr, callbackCtxPtr_t callback, uint32_t* intNoPtr);
// Configures the interrupt for a quadrature encoder.
btnError_t configureQuadratureInterrupts(quad_encoder_t* quadPtr, callbackCtxPtr_t callback, uint32_t* intNoPtr);

// button_t* getButton0(void);
// button_t* getButton1(void);

#endif /* INC_BUTTONS_H_ */
