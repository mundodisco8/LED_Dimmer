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
// Ignore a cast-align warning in some cmsis header and a sign conversion
#include "app_log.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-align"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include "app_assert.h"
#pragma GCC diagnostic pop

// Project's Libraries
#include "debounce.h"
#include "gpio_HW.h"
#include "interrupt_HW.h"
#include "sleepyTimers_HW.h"

// Trickery to allow testing of static elements. Better to mess a bit with the code than to overcomplicate tests
#ifdef TEST
#define STATIC
#else
#define STATIC static
#endif

/**
 * @brief Debounce control variables
 * DEBOUNCE_TIME_MS is the duration of the debounce event. During this event, the button is sampled frequently, and when
 * it converges (integrator reaches INTEGRATOR_TARGET when the button is pressed or reaches 0 when it is released), we
 * change the button state. If the integrator doesn't converge in this time, we can consider the press to be spurious
 * and ignore it.
 * DEBOUNCE_SAMPLING_PERIOD_MS is the period of the sampling events.
 * INTEGRATOR_TARGET is the number of sampling events the pin has to remain in the new position to consider it a valid
 * press. A button press won't be valid until is pressed at least for INTEGRATOR_TARGET * DEBOUNCE_SAMPLING_PERIOD_MS
 */
const uint32_t DEBOUNCE_TIME_MS = 75U;            // Max time for the button to settle in the new state
const uint32_t DEBOUNCE_SAMPLING_PERIOD_MS = 5U;  // read the button every this ms
const int32_t INTEGRATOR_TARGET = 5U;             // Button is stable after SAMPLING * TARGET ms

/**
 * @brief Time required to consider a press a longpress
 */
const uint32_t LONGPRESS_TIME_MS = 3000U;

////
// forward declarations
////

STATIC void samplingTimerCallback(timerHandlePtr_t handlePtr, void* data);
STATIC void longPressTimerCallback(timerHandlePtr_t handlePtr, void* data);

/**
 * @brief This is in debounce.h, but this way we don't need to include it, breaking a circular dependency
 */
void debounceButton(button_t* btnPtr);

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
    slpTimerStatus_t retVal = SLP_reserveTimer(&(btnPtr->longPressTimerPtr));
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

////
// Timer functions - Starting and stopping the sampling and callback timers
////

/**
 * @brief Silly function to get the type of timer as a string
 * @param timerType a timer type
 * @return char* a string with "Sampling timer" if the timer was a TIMER_SAMPLE, "Debouncing Timer" otherwise
 */
static char* getTimerTypeString(const btnTimerType_t timerType) {
    return (TIMER_SAMPLE ? "Sampling Timer" : "Debouncing Timer");
}

/**
 * @brief Start the timers of a button.
 * @param btnPtr a pointer to the button whose timers we want to start
 * @param timerType which one of the timers of the button to start
 * @return BTN_OK on success, BTN_ERROR otherwise. Asserts if wrong type of timer is passed in timerType
 */
btnError_t startButtonTimer(button_t* btnPtr, const btnTimerType_t timerType) {
    btnError_t retVal = BTN_OK;
    // TODO: check for null pointers? initButton should protect against it.
    uint32_t time = 0;
    expBehaviour_t behaviour = TIMER_ONE_SHOT;
    timerHandlePtr_t timerPtr = NULL;
    timerCallback_t callbackPtr = NULL;

    switch (timerType) {
        case TIMER_SAMPLE: {
            time = DEBOUNCE_SAMPLING_PERIOD_MS;
            behaviour = TIMER_ONE_SHOT;
            timerPtr = btnPtr->samplingTimerPtr;
            callbackPtr = samplingTimerCallback;
            break;
        }
        case TIMER_LONGPRESS: {
            time = LONGPRESS_TIME_MS;
            behaviour = TIMER_ONE_SHOT;
            timerPtr = btnPtr->longPressTimerPtr;
            callbackPtr = longPressTimerCallback;
            break;
        }
        default: {
            // We fell through the cracks!
            app_assert(false, "Wrong type of timer passed -> 0x%" PRIX32 "\r\n", (uint32_t)timerType);
            break;
        }
    }

    if (!SLP_isTimerRunning(timerPtr)) {
        // Don't restart it if it's already running, or noise would extend the debounce time
        if (SLPTIMER_OK != SLP_startTimer(timerPtr, time, behaviour, callbackPtr, btnPtr)) {
            app_log_error("Error starting %s timer\r\n", getTimerTypeString(timerType));
            return BTN_ERROR;
        }
    }

    return retVal;
}

//

/**
 * @brief Action to run when a sampling timer has timed out
 * @param handlePtr (unused) a handle to the pointer that triggered the timeout
 * @param data metadata passed to the callback on trigger. In our case, it's a pointer to the button whose timer
 *  triggered the callback
 */
STATIC void samplingTimerCallback(timerHandlePtr_t handlePtr, void* data) {
    (void)handlePtr;
    // When a sampling sleeptimer runs out, calls debounceButton from debounce.h, passing a button_t object as
    // context data.
    debounceButton((button_t*)data);
}

// Action to run when a sampling timer has timed out

/**
 * @brief Action to run when a button has been held pressed for long enough to trigger a long-press
 * @param handlePtr (unused) a handle to the pointer that triggered the timeout
 * @param data metadata passed to the callback on trigger. In our case, it's a pointer to the button whose timer
 *  triggered the callback
 */
STATIC void longPressTimerCallback(timerHandlePtr_t handlePtr, void* data) {
    (void)handlePtr;
    // Change button state
    ((button_t*)data)->state = BUTTON_LONGPRESSED;
    // call longpress action if any
}
