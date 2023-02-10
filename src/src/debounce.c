/*
 * debounce.c
 *
 * Adaptation of Kuhn's debounce code using an integrator.
 * An interrupt in a button will start a timer. While the timer is running, the
 * button will be polled frequently and it's value integrated. If the value of
 * the integrator "converges", the value of the button is updated and the timer
 * is stopped. If, on the other hand, if the integrator doesn't reach the expected
 * value, then the button press is rejected and the old state of the button is
 * preserved
 *
 *  Created on: 15 Nov 2022
 *      Author: joel.santos
 */

#include "debounce.h"

#include "inttypes.h"
#include "stddef.h"

#include "gpio_HW.h"

#include "app_log.h"

#include "sleepyTimers_HW.h"
/******************************************************************************
written by Kenneth A. Kuhn
version 1.00

This is an algorithm that debounces or removes random or spurious
transistions of a digital signal read as an input by a computer.  This is
particularly applicable when the input is from a mechanical contact.  An
integrator is used to perform a time hysterisis so that the signal must
persistantly be in a logical state (0 or 1) in order for the output to change
to that state.  Random transitions of the input will not affect the output
except in the rare case where statistical clustering is longer than the
specified integration time.

The following example illustrates how this algorithm works.  The sequence
labeled, real signal, represents the real intended signal with no noise.  The
sequence labeled, corrupted, has significant random transitions added to the
real signal.  The sequence labled, integrator, represents the algorithm
integrator which is constrained to be between 0 and 3.  The sequence labeled,
output, only makes a transition when the integrator reaches either 0 or 3.
Note that the output signal lags the input signal by the integration time but
is free of spurious transitions.

real signal 0000111111110000000111111100000000011111111110000000000111111100000
corrupted   0100111011011001000011011010001001011100101111000100010111011100010
integrator  0100123233233212100012123232101001012321212333210100010123233321010
output      0000001111111111100000001111100000000111111111110000000001111111000

I have been using this algorithm for years and I show it here as a code
fragment in C.  The algorithm has been around for many years but does not seem
to be widely known.  Once in a rare while it is published in a tech note.  It
is notable that the algorithm uses integration as opposed to edge logic
(differentiation).  It is the integration that makes this algorithm so robust
in the presence of noise.
******************************************************************************/
//     /* Step 1: Update the integrator based on the input signal.  Note that the
//   integrator follows the input, decreasing or increasing towards the limits as
//   determined by the input state (0 or 1). */
//     if (input == 0) {
//         if (integrator > 0)
//             integrator--;
//     } else  {
//         if (integrator < INTEGRATOR_TARGET)
//         integrator++;
//     }

//     /* Step 2: Update the output state based on the integrator.  Note that the
//     output will only change states if the integrator has reached a limit, either
//     0 or MAXIMUM. */

//     if (integrator == 0) {
//         output = 0;
//     } else if (integrator >= ) {
//         output     = 1;
//         integrator = MAXIMUM; /* defensive code if integrator got corrupted */
//     }

/* The following parameters tune the algorithm to fit the particular
application.  The example numbers are for a case where a computer samples a
mechanical contact 10 times a second and a half-second integration time is
used to remove bounce.  Note: DEBOUNCE_TIME is in seconds and SAMPLE_FREQUENCY
is in Hertz */

// DEBOUNCE_TIME_MS is the duration of the debounce event. During this event, the button is sampled frequently, and when
// it converges (integrator reaches INTEGRATOR_TARGET when the button is pressed or reaches 0 when it is released), we
// change the button state. If the integrator doesn't converge in this time, we can consider the press to be spurious
// and ignore it.
// DEBOUNCE_SAMPLING_PERIOD_MS is the period of the sampling events.
// INTEGRATOR_TARGET is the number of sampling events the pin has to remain in the new position to consider it a valid
// press. A button press won't be valid until is pressed at least for INTEGRATOR_TARGET * DEBOUNCE_SAMPLING_PERIOD_MS
const uint32_t DEBOUNCE_TIME_MS            = 500;  // Max time for the button to settle in the new state
const uint32_t DEBOUNCE_SAMPLING_PERIOD_MS = 10;   // read the button every this ms
const int32_t INTEGRATOR_TARGET            = 5;    // Button is stable after SAMPLING * TARGET ms

////
// forward declarations
////

static uint32_t stopButtonTimers(button_t* btnPtr);
static void sleeptimerDebounceCallback(timerHandle_t* handle, void* data);
static void sleeptimerSamplingCallback(timerHandle_t* handle, void* data);

////
// getters for the timer durations
////

// Returns the value of the duration of the debounce timer.
static uint32_t getDebounceTime(void) {
    return DEBOUNCE_TIME_MS;
}
// Returns the value of the debounce sampling period.
static uint32_t getDebounceSamplingPeriod(void) {
    return DEBOUNCE_SAMPLING_PERIOD_MS;
}

// Callback used by the sampling timer to update the integrator value.
// Reads the state of the pin passed as parameter and updates the integrator value. If the value reaches one of the two
// targets (0 or INTEGRATOR_TARGET) it updates the state of the button.
//
// Parameters: btnPtr: pointer to a button_t object
//
// Returns: nothing
void samplingTimerCallback(button_t* btnPtr) {
    /* Step 1: Update the integrator based on the input signal.  Note that the
       integrator follows the input, decreasing or increasing towards the limits as
       determined by the input state (0 or 1). */

    if (readPin(btnPtr->btnPort, btnPtr->pinNo) == 1) {
        if (btnPtr->integrator < INTEGRATOR_TARGET) {
            btnPtr->integrator++;
        }
    } else {  // readPin returns low
        if (btnPtr->integrator > 0) {
            btnPtr->integrator--;
        }
    }

    /* Step 2: Update the output state based on the integrator.  Note that the
       output will only change states if the integrator has reached a limit, either
       0 or MAXIMUM. */

    // corruption guard
    if (btnPtr->integrator > INTEGRATOR_TARGET) {
        btnPtr->integrator = INTEGRATOR_TARGET;
    } else if (btnPtr->integrator < 0) {
        btnPtr->integrator = 0;
    }  // else 0 < integrator < INTEGRATOR_TARGET, do nothing

    if ((btnPtr->state == BUTTON_RELEASED) && (btnPtr->integrator == INTEGRATOR_TARGET)) {
        btnPtr->state = BUTTON_PRESSED;
        stopButtonTimers(btnPtr);
        if (btnPtr->pressedAction != NULL) {
            btnPtr->pressedAction();
        }  // else, action was NULL, assert, I guess?
    } else if ((btnPtr->state == BUTTON_PRESSED) && (btnPtr->integrator == 0)) {
        btnPtr->state = BUTTON_RELEASED;
        stopButtonTimers(btnPtr);
        if (btnPtr->releasedAction != NULL) {
            btnPtr->releasedAction();
        }  // else, action was NULL, assert, I guess?
    }  // else, or integrator is 0 but button is released or integrator is INTEGRATOR_TARGET but button is pressed, do
       // nothing
}

////
// Starting and stopping the sampling and callback timers
////

// Start the timers of a button. To be called on a button's GPIO interrupt
uint32_t startButtonTimer(button_t* btnPtr, timerType_t timerType) {
    uint32_t retVal = SLPTIMER_OK;
    // TODO: check for null pointers? initButton should protect against it.
    if (timerType == TIMER_SAMPLE) {
        if (!SLP_isTimerRunning(btnPtr->samplingTimerPtr)) {  // Don't restart it if it's already running, or noise
                                                              // would extend the debounce time
            retVal = SLP_startPeriodicTimer(btnPtr->samplingTimerPtr, getDebounceSamplingPeriod(),
                                            sleeptimerSamplingCallback, btnPtr);
        }
    } else {  // Only two types of timers, TIMER_SAMPLE and TIMER_DEBOUNCE
        if (!SLP_isTimerRunning(btnPtr->debounceTimerPtr)) {
            retVal = SLP_startTimer(btnPtr->debounceTimerPtr, getDebounceTime(), sleeptimerDebounceCallback, btnPtr);
        }
    }
    if (retVal != SLPTIMER_OK) {
        app_log_error("Error 0x%04X starting %s timer\r\n", retVal,
                      (timerType == TIMER_SAMPLE ? "sampling" : "debouncing"));
    }
    return retVal;
}

// Stops the timers associated to a button
// Parameters: btnPtr, a pointer to a button_t object whose timers are going to be stopped
static uint32_t stopButtonTimers(button_t* btnPtr) {
    uint32_t retVal = SLPTIMER_OK;

    if (SLP_isTimerRunning(btnPtr->samplingTimerPtr)) {
        retVal = SLP_stopTimer(btnPtr->samplingTimerPtr);
        if (retVal != SLPTIMER_OK) {
            app_log_error("Error 0x%04X stopping sampling timer\r\n", retVal);
            return retVal;
        }
    }
    if (SLP_isTimerRunning(btnPtr->debounceTimerPtr)) {
        retVal = SLP_stopTimer(btnPtr->debounceTimerPtr);
        if (retVal != SLPTIMER_OK) {
            app_log_error("Error 0x%04X stopping debounce timer\r\n", retVal);
        return retVal;
        }
    }
    return SLPTIMER_OK;
}

////
// Timer callbacks
////

// Action to run when a debounce timer has timed out
static void sleeptimerDebounceCallback(timerHandle_t* handle, void* data) {
    // If the debounce timer callback is executed, it means the integrator hasn't converged in the debouncing time
    // given. Stop the sampling timer, because it's periodic, and reset the integrator value to the default value
    // for the current state
    (void)handle;
    app_log_warning("Debouncer Callback\r\n");
    stopButtonTimers((button_t*)data);
}

// Action to run when a sampling timer has timed out
static void sleeptimerSamplingCallback(timerHandle_t* handle, void* data) {
    (void)handle;
    // When a sampling sleeptimer runs out, calls samplingTimerCallback from debounce.h, passing a button_t object as
    // context data.
    samplingTimerCallback((button_t*)data);
}