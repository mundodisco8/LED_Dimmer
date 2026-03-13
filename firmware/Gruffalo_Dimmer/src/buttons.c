/**
 * @file buttons.c
 * @author Joel Santos (jsantosrico@gmail.com)
 * @brief Implementation of debounced button logic and quadrature encoder state machines.
 * @version 0.1
 * @date 2026-03-13
 * * @copyright Copyright (c) 2026
 * * @details
 * Implements the core logic for the Button and Encoder hardware abstraction.
 * Key implementation details include:
 * * **Long-Press Threshold:** Set to **3000ms** by default (@ref LONGPRESS_TIME_MS).
 * * **Encoder Logic:** Decodes rotation by sampling the B-signal phase upon a falling
 * edge interrupt on the A-signal.
 * * **Safety:** Uses hardware-level asserts to ensure valid pointers and
 * timer availability during runtime.
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
 * @brief Time required to consider a press a longpress
 */
const uint32_t LONGPRESS_TIME_MS = 3000U;

////
// forward declarations
////

STATIC void samplingTimerCallback(timerHandlePtr_t handlePtr, void* data);
STATIC void longPressTimerCallback(timerHandlePtr_t handlePtr, void* data);

/**
 * @brief Initialises the button_t struct with the default values and associates the pressed and released actions.
 * NOTE: Each button uses two sleeptimers, so make sure you have enough!
 * @param btnPtra pointer to a button_t object which will be initialised
 * @param pinPortthe port of the GPIO Pin of the button
 * @param pinNothe pin number of the button
 * @param shortPressActiona buttonCallback_t pointer to the callback to call when the button is short pressed
 * @param longPressActiona buttonCallback_t pointer to the callback to call when the button is long pressed
 * @param releasedAction buttonCallback_t pointer to the callback to call when the button is released
 * @return Doesn't return anything, but Asserts if the quad_encoder_t pointer is NULL
 */
void initButton(button_t* btnPtr, const pinPort_t pinPort, const uint8_t pinNo, const actionCallback_t shortPressAction,
                const actionCallback_t longPressAction, const actionCallback_t releasedAction) {
    app_assert(btnPtr != NULL, "Passed NULL Button pointer to initButton");

    setPinMode(pinPort, pinNo, MODE_INPUT, false);
    btnPtr->btnPort = pinPort;
    btnPtr->pinNo = pinNo;
    btnPtr->integrator = 0;
    btnPtr->state = BUTTON_RELEASED;  // buttons are released by default
    btnPtr->pressedAction = shortPressAction;
    btnPtr->longPressedAction = longPressAction;
    btnPtr->releasedAction = releasedAction;

    slpTimerStatus_t retVal = SLP_reserveTimer(&(btnPtr->longPressTimerPtr));
    //TODO: Test Asserts on error
    app_assert((retVal == SLPTIMER_OK), "No timer available. Increase the number of timers\r\n");
    retVal = SLP_reserveTimer(&(btnPtr->samplingTimerPtr));
    app_assert((retVal == SLPTIMER_OK), "No timer available. Increase the number of timers\r\n");
}

/**
 * @brief Initialises the quad_encoder_t struct with the default values and associates the CW and CCW actions.
 * @param quadPtr a pointer to a quad_encoder_t object which will be initialised
 * @param pin0Port the port of the GPIO Pin of the A signal of the quaa
 * @param pin0No the pin number of the A signal of the quaa
 * @param pin1Port the port of the GPIO Pin of the B signal of the quaa
 * @param pin1No the pin number of the B signal of the quaa
 * @param CWAction a buttonCallback_t pointer to the callback to call when the button is pressed
 * @param CCWAction a buttonCallback_t pointer to the callback to call when the button is released
 * @return Doesn't return anything, but Asserts if the quad_encoder_t pointer is NULL
 */
void initQuadEncoder(quad_encoder_t* quadPtr, const pinPort_t pin0Port, const uint8_t pin0No, const pinPort_t pin1Port,
                     const uint8_t pin1No, const actionCallback_t CWAction, const actionCallback_t CCWAction) {
    app_assert(quadPtr != NULL, "Passed NULL Quad Encoder pointer to initQuadEncoder");

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
}

/**
 * @brief Configures the interrupt for the button and sets its callback.
 * This callback is the function to call when the GPIO interrupt triggers, but is not the action to take when the button
 * is pressed or released. This callback is the function that will determine if the button is pressed or released and
 * act accordingly
 *
 * @param btnPtr a pointer to the button_t object to be set
 * @param callback a callbacCtxPtr_t pointer to the function to be called when the interrupt is triggered.
 * @param intNoPtr a pointer to a unit32_t that will store the pin interrupt number assigned, or 0xFF if no interrupt
 * was available.
 * @return returns nothing, but Asserts if there are no interrupts available for the button, or if btnPtr or intNoPtr
 * was NULL
 */
void configureButtonInterrupts(button_t* btnPtr, const callbackCtxPtr_t callback, uint32_t* intNoPtr) {
    // We need btnPtr and intNoPtr to not be null for this to work

    // Null pointer guards
    app_assert(btnPtr != NULL, "Passed NULL Button pointer to configureButtonInterrupts");
    app_assert(intNoPtr != NULL, "Passed NULL Int No parameter pointer to configureButtonInterrupts");
    // GPIO clock and pin direction already set by GPIO Init module
    *intNoPtr = setInterruptCallbackWCtx(btnPtr->pinNo, callback, (void*)btnPtr);
    app_assert(*intNoPtr != 0xFF, "No Interrupts available for the button!");
    // We call configurePinInterrupt with whatever pin numnber setInterruptCallbackWCtx returns, not with
    // pinNo, as one would expect, just in case they are not the same number
    configurePinInterrupt(btnPtr->btnPort, btnPtr->pinNo, *intNoPtr, true, true, true);
    enablePinInterrupts(1 << (*intNoPtr));
}

/**
 * @brief Configures the interrupt for the quad encoder and sets its callback.
 * This callback is the function to call when the GPIO interrupt triggers, but is not the action to take when the button
 * is pressed or released. This callback is the function that will determine if the button is pressed or released and
 * act accordingly
 * * @param quadPtra pointer to the quad_encoder_t object to be set
 * @param callback a callbacCtxPtr_t pointer to the function to be called when the interrupt is triggered.
 * @param intNoPtr a pointer to a unit32_t that will store the pin interrupt number assigned, or 0xFF if no interrupt
 * @return returns nothing, but Asserts if there are no interrupts available for the button, or if quadPtr or intNoPtr
 * was NULL
 */
void configureQuadratureInterrupts(quad_encoder_t* quadPtr, const callbackCtxPtr_t callback, uint32_t* intNoPtr) {
    // The rotary encoder is a PEC11R-4115F-S0018 -> 18 detents and 18 pulses. Each click generates a full pulse, so
    // there's only two codes 00 and 01 to care about, as the encoder will always return to 11 (00 can only achieved if
    // you fiddle and try to hold the shaft between detents).

    // Null pointer guards
    app_assert(quadPtr != NULL, "Passed NULL Quad Encoder pointer to configureQuadratureInterrupts");
    app_assert(intNoPtr != NULL, "Passed NULL Int No parameter pointer to configureQuadratureInterrupts");

    // quad1_0 pin interrupt on falling edge only, as we are not interested on the rising edge.
    // GPIO clock and pin direction already set by GPIO Init module
    *intNoPtr = setInterruptCallbackWCtx(quadPtr->pin0No, callback, (void*)quadPtr);
    app_assert(*intNoPtr != 0xFF, "No Interrupts available for the quad encoder!");

    configurePinInterrupt(quadPtr->pin0Port, quadPtr->pin0No, *intNoPtr, false, true, true);
    enablePinInterrupts(1 << *intNoPtr);
}

/**
 * @brief Updates the button state, making sure we also
 * @param btnPtr a pointer to the button whose state has changed
 * @param newState the new state of the button
 */
void buttonSetState(button_t* btnPtr, const buttonState_t newState) {
    app_assert(btnPtr != NULL, "Passed NULL pointer to buttonSetState");

    // Keep track of the previous state, as we use that on the action logic
    btnPtr->prevState = btnPtr->state;
    btnPtr->state = newState;
}

/**
 * @brief Returns the state of the button
 * @param btnPtr a pointer to the button whose state we want to get
 * @return the state of the button
 */
buttonState_t buttonGetState(const button_t* btnPtr) { return btnPtr->state; }

////
// Timer functions - Starting and stopping the sampling and callback timers
////

/**
 * @brief Silly function to get the type of timer as a string
 * @param timerType a timer type
 * @return char* a string with "Sampling timer" if the timer was a TIMER_SAMPLE, "Debouncing Timer" otherwise
 */
static char* getTimerTypeString(const btnTimerType_t timerType) {
    return ((timerType == TIMER_SAMPLE) ? "Sampling Timer" : "Debouncing Timer");
}

/**
 * @brief Start the timers of a button.
 * @param btnPtr a pointer to the button whose timers we want to start
 * @param timerType which one of the timers of the button to start
 * @return BTN_OK on success, BTN_ERROR otherwise. Asserts if wrong type of timer is passed in timerType or if btnPtr
 * is NULL
 */
btnError_t startButtonTimer(button_t* btnPtr, const btnTimerType_t timerType) {
    app_assert(btnPtr != NULL, "Passed NULL Button pointer to startButtonTimer");

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

    return BTN_OK;
}

/**
 * @brief Stops a timer on a button.
 * @param btnPtr a pointer to the button whose timers we want to stop
 * @param timerType which one of the timers of the button to stop
 * @return BTN_OK on success, BTN_ERROR otherwise. Asserts if wrong type of timer is passed in timerType or if btnPtr
 * is NULL
 */
btnError_t stopButtonTimer(const button_t* btnPtr, const btnTimerType_t timerType) {
    app_assert(btnPtr != NULL, "Passed NULL Button pointer to stoptButtonTimer");

    timerHandlePtr_t timerPtr = NULL;

    switch (timerType) {
        case TIMER_SAMPLE: {
            timerPtr = btnPtr->samplingTimerPtr;
            break;
        }
        case TIMER_LONGPRESS: {
            timerPtr = btnPtr->longPressTimerPtr;
            break;
        }
        default: {
            // We fell through the cracks!
            app_assert(false, "Wrong type of timer passed -> 0x%" PRIX32 "\r\n", (uint32_t)timerType);
            break;
        }
    }
    if (SLP_isTimerRunning(timerPtr)) {
        if (SLPTIMER_OK != SLP_stopTimer(timerPtr)) {
            app_log_error("Error stopping %s\r\n", getTimerTypeString(timerType));
            return BTN_ERROR;
        }
    }
    return BTN_OK;
}

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
    button_t* btnPtr = (button_t*)data;
    // Change button state
    buttonSetState(btnPtr, BUTTON_LONGPRESSED);
    // call longpress action if any
    if (NULL != btnPtr->longPressedAction) {
        btnPtr->longPressedAction(btnPtr);
    }
}
