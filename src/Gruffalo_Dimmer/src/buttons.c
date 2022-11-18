////
// Buttons
//
// A library to handle the buttons on this project.
// Provides a button_t object that is debounced via software using Kenneth A. Kuhn's integrator algo (see integrator.c)
//
////

/*
 * buttons.c
 *
 *  Created on: 11 Nov 2022
 *      Author: joel.santos
 */

#include "buttons.h"

// Other SiLabs headers
#include "pin_config.h"

// Project's Libraries
#include "timers_HW.h"

button_t button0 = {0};
button_t button1 = {0};

// Initialises the button_t struct with the default values and associates the pressed and released actions.
// Parameters: btnPtr: a pointer to a button_t object which will be initialised
//             pinPort: the port of the GPIO Pin of the button
//             pinNo: the pin number of the button
//             pressedAction: a buttonCallback_t pointer to the callback to call when the button is pressed
//             releasedAction: a buttonCallback_t pointer to the callback to call when the button is released
// Returns: nothing
void initButton(button_t* btnPtr, pin_port_t pinPort, uint32_t pinNo, buttonCallback_t pressedAction, buttonCallback_t releasedAction) {
    btnPtr->btnPort = pinPort;
    btnPtr->pinNo = pinNo;
    btnPtr->integrator = 0;
    btnPtr->state = BUTTON_RELEASED; //buttons are released by default
    btnPtr->pressedAction = pressedAction;
    btnPtr->releasedAction = releasedAction;
}

// Configures the interrupt for the button and sets its callback
// Parameters: btnPtr: a pointer to the button_t object to be set
//             callback: a callbacCtxPtr_t pointer to the function to be called when the interrupt is triggered.
// Returns: the interrupt number set, or 0xFF if no interrupt was available.
uint32_t configureButtonInterrupts(button_t* btnPtr, callbackCtxPtr_t callback) {
    // GPIO clock and pin direction already set by GPIO Init module
    uint32_t intPin = setInterruptCallbackWCtx(btnPtr->pinNo, callback, (void*)btnPtr);
    // We call configurePinInterrupt with whatever pin numnber setInterruptCallbackWCtx retunrs, not with
    // pinNo, as one would expect, just in case they are not the same number
    if (intPin != 0xFF) { // We were given a valid interrupt, proceed to configure it.
        configurePinInterrupt(btnPtr->btnPort, btnPtr->pinNo, intPin, true, true, true);
        enablePinInterrupts(1 << intPin);
    }
    return intPin;
}

// Configures the interrupt for a quadrature encoder.
// Only falling edge interrupts are selected because of the HW config of the board. For the same reason, there's no need
// to debounce the inputs, as the board has HW filters.
// Parameters: callback: a pointer to a callBackPtr_t function to be called when the interrupt triggers.
void configureQuadratureInterrupts(callbackPtr_t callback) {
    // TODO: This is just for quad1, will need to do it generic, like configureButton avobe
     // The rotary encoder is a PEC11R-4115F-S0018 -> 18 detents and 18 pulses. Each click generates a full pulse, so
    // there's only two codes 00 and 01 to care about, as the encoder will always return to 11 (00 can only achieved if
    // you fiddle and try to hold the shaft between detents).
    // quad1_0 pin interrupt on falling edge only, as we are not interested on the rising edge.
    setInterruptCallback(quad1_0_PIN, callback);
    configurePinInterrupt(quad1_0_PORT, quad1_0_PIN, quad1_0_PIN, false, true, true);
    enablePinInterrupts(1 << quad1_0_PIN);
}

// DELETE? test functions, to be removed
button_t* getButton0(void) {
    return &button0;
}

button_t* getButton1(void) {
    return &button1;
}
