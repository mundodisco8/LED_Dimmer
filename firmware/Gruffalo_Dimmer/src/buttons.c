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

// Stdlib
#include <stdbool.h>
#include <stddef.h>

// SiLabs
// Ignore a cast-align warning in some cmsis header and a sign conversion in
// sl_sleeptimer.h
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-align"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include "app_assert.h"
#pragma GCC diagnostic pop

// Project's Libraries
#include "gpio_HW.h"
#include "interrupt_HW.h"
#include "sleepyTimers_HW.h"

// Initialises the button_t struct with the default values and associates the pressed and released actions.
// NOTE: Each button uses two sleeptimers, so make sure you have enough!
// Parameters: btnPtr: a pointer to a button_t object which will be initialised
//             pinPort: the port of the GPIO Pin of the button
//             pinNo: the pin number of the button
//             pressedAction: a buttonCallback_t pointer to the callback to call when the button is pressed
//             releasedAction: a buttonCallback_t pointer to the callback to call when the button is released
// Returns: true on sucess, false if the button_t pointer passed is null
btnError_t initButton(button_t* btnPtr, pinPort_t pinPort, uint8_t pinNo, actionCallback_t pressedAction,
                      actionCallback_t releasedAction) {
    if (btnPtr == NULL) {
        return BTN_NULL_POINTER_PASSED;
    }
    setPinMode(pinPort, pinNo, MODE_INPUT, false);
    btnPtr->btnPort = pinPort;
    btnPtr->pinNo = pinNo;
    btnPtr->integrator = 0;
    btnPtr->state = BUTTON_RELEASED;  // buttons are released by default
    btnPtr->pressedAction = pressedAction;
    btnPtr->releasedAction = releasedAction;
    slpTimerStatus_t retVal = SLP_reserveTimer(&(btnPtr->debounceTimerPtr));
    //TODO: Test Asserts on error
    app_assert((retVal == SLPTIMER_OK), "No timer available. Increase the number of timers\r\n");
    retVal = SLP_reserveTimer(&(btnPtr->samplingTimerPtr));
    app_assert((retVal == SLPTIMER_OK), "No timer available. Increase the number of timers\r\n");

    return BTN_OK;
}

// Initialises the quad_encoder_t struct with the default values and associates the CW and CCW actions.
// Parameters: quadPtr: a pointer to a quad_encoder_t object which will be initialised
//             pin0Port: the port of the GPIO Pin of the A signal of the quaa
//             pin0No: the pin number of the A signal of the quaa
//             pin1Port: the port of the GPIO Pin of the B signal of the quaa
//             pin1No: the pin number of the B signal of the quaa
//             CWAction: a buttonCallback_t pointer to the callback to call when the button is pressed
//             CCWAction: a buttonCallback_t pointer to the callback to call when the button is released
// Returns: true on sucess, false if the button_t pointer passed is null
btnError_t initQuadEncoder(quad_encoder_t* quadPtr, pinPort_t pin0Port, uint8_t pin0No, pinPort_t pin1Port,
                           uint8_t pin1No, actionCallback_t CWAction, actionCallback_t CCWAction) {
    if (quadPtr == NULL) {
        // Not much we can do it the quad encoder pointer is NULL
        return BTN_NULL_POINTER_PASSED;
    }
    // TODO: If I set the pins as input with no pulls, the encoder signal doesn't go low enough to trigger the interrupt
    // This is really odd, as it measures 1.something volts, as if the pin had an internal pull-up!.
    // I only managed to get it working by actively pulling it dowm, but it should not be neccessary. Do more reasearch!
    setPinMode(pin0Port, pin0No, MODE_INPUT_PULL, false);
    setPinMode(pin1Port, pin1No, MODE_INPUT_PULL, false);
    quadPtr->pin0No = pin0No;
    quadPtr->pin0Port = pin0Port;
    quadPtr->pin1No = pin1No;
    quadPtr->pin1Port = pin1Port;
    quadPtr->clockWiseAction = CWAction;
    quadPtr->counterClockWiseAction = CCWAction;
    return BTN_OK;
}

// Configures the interrupt for the button and sets its callback.
// This callback is the function to call when the GPIO interrupt triggers, but is not the action to take when the button
// is pressed or released. This callback is the function that will determine if the button is pressed or released and
// act accordingly
// Parameters: btnPtr: a pointer to the button_t object to be set
//             callback: a callbacCtxPtr_t pointer to the function to be called when the interrupt is triggered.
//             intNoPtr: a pointer to a unit32_t that will store the pin interrupt number assigned.
// Returns: the interrupt number set, or 0xFF if no interrupt was available.
//          The interrupt number set in the uint32_t pointed by intNoPtr
btnError_t configureButtonInterrupts(button_t* btnPtr, callbackCtxPtr_t callback, uint32_t* intNoPtr) {
    // We need btnPtr and intNoPtr to not be null for this to work
    if ((btnPtr == NULL) || (intNoPtr == NULL)) {
        return BTN_NULL_POINTER_PASSED;
    }
    // GPIO clock and pin direction already set by GPIO Init module
    *intNoPtr = setInterruptCallbackWCtx(btnPtr->pinNo, callback, (void*)btnPtr);
    if (*intNoPtr == 0xFF) {
        // No ints available
        return BTN_NO_INTS_AVAILABLE;
    }  // Else, an int was correctly configured, and returned in intPin
    // We call configurePinInterrupt with whatever pin numnber setInterruptCallbackWCtx returns, not with
    // pinNo, as one would expect, just in case they are not the same number
    configurePinInterrupt(btnPtr->btnPort, btnPtr->pinNo, *intNoPtr, true, true, true);
    enablePinInterrupts(1 << *intNoPtr);
    return BTN_OK;
}

// Configures the interrupt for a quad encoer and sets its callback.
// This callback is the function to call when the GPIO interrupt triggers, but is not the action to take when its
// rotated CW or CCW. This callback is the function that determines the rotation direction and acts accordingly
// Parameters: quadPtr: a pointer to the quad_encoder_t object to be set
//             callback: a callbacCtxPtr_t pointer to the function to be called when the interrupt is triggered.
//             intNoPtr: a pointer to a unit32_t that will store the pin interrupt number assigned.
// Returns: the interrupt number set, or 0xFF if no interrupt was available.
//          The interrupt number set in the uint32_t pointed by intNoPtr
btnError_t configureQuadratureInterrupts(quad_encoder_t* quadPtr, callbackCtxPtr_t callback, uint32_t* intNoPtr) {
    // The rotary encoder is a PEC11R-4115F-S0018 -> 18 detents and 18 pulses. Each click generates a full pulse, so
    // there's only two codes 00 and 01 to care about, as the encoder will always return to 11 (00 can only achieved if
    // you fiddle and try to hold the shaft between detents).

    // Null pointer guards
    if ((quadPtr == NULL) || (intNoPtr == NULL)) {
        return BTN_NULL_POINTER_PASSED;
    }
    // quad1_0 pin interrupt on falling edge only, as we are not interested on the rising edge.
    // GPIO clock and pin direction already set by GPIO Init module
    *intNoPtr = setInterruptCallbackWCtx(quadPtr->pin0No, callback, (void*)quadPtr);
    if (*intNoPtr == 0xFF) {
        return BTN_NO_INTS_AVAILABLE;
    }
    configurePinInterrupt(quadPtr->pin0Port, quadPtr->pin0No, *intNoPtr, false, true, true);
    enablePinInterrupts(1 << *intNoPtr);
    return BTN_OK;
}
