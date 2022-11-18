/*
 * buttons.h
 *
 *  Created on: 11 Nov 2022
 *      Author: joel.santos
 */

#ifndef INC_BUTTONS_H_
#define INC_BUTTONS_H_

#include "inttypes.h"

#include "gpio_HW.h"
#include "interrupt_HW.h"

// An enum to define the state of a button
typedef enum button_state {
    BUTTON_PRESSED = 0,
    BUTTON_RELEASED = 1
} button_state_t;

typedef void (*buttonCallback_t)(void);

// A struct to contain the context of a button
typedef struct {
    pin_port_t btnPort;
    uint32_t pinNo;
    button_state_t state; // The state of the button, pressed or released
    int32_t integrator;  // The value of the integrator for this button
    buttonCallback_t pressedAction; // action to perform when the state changes to BUTTON_PRESSED
    buttonCallback_t releasedAction;// action to perform when the state changes to BUTTON_RELEASED
} button_t;

// Initialises the button_t struct with the default values and associates the pressed and released actions.
void initButton(button_t* btnPtr, pin_port_t pinPort, uint32_t pinNo, buttonCallback_t pressedAction, buttonCallback_t releasedAction);

// Configures the interrupt for the button and sets its callback
uint32_t configureButtonInterrupts(button_t* btnPtr, callbackCtxPtr_t callback);
// Configures the interrupt for a quadrature encoder.
void configureQuadratureInterrupts(callbackPtr_t callback);

button_t* getButton0(void);
button_t* getButton1(void);

#endif /* INC_BUTTONS_H_ */
