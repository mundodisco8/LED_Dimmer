/**
 * @file debounce.c
 * @author Joel Santos (jsantosrico@gmail.com)
 * @brief Implementation of the button debounce algorithm
 * Adaptation of Kuhn's debounce code using an integrator.
 * An interrupt in a button will start a timer. While the timer is running, the
 * button will be polled frequently and it's value integrated. If the value of
 * the integrator "converges", the value of the button is updated and the timer
 * is stopped. If, on the other hand, if the integrator doesn't reach the expected
 * value, then the button press is rejected and the old state of the button is
 * preserved
 * @version 0.1
 * @date 2022-11-15
 *
 * @copyright Copyright (c) 2026
 *
 */

#include "debounce.h"

// StdLib headers
#include <inttypes.h>
#include <stddef.h>

// Silabs SDK headers
// Ignore a cast-align warning in some cmsis header and a sign conversion in
// sl_sleeptimer.h
#include "app_assert.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-align"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include "app_log.h"
#pragma GCC diagnostic pop

// Project headers
#include "gpio_HW.h"

// Our headers
#include "buttons.h"

// Trickery to allow testing of static elements. Better to mess a bit with the code than to overcomplicate tests
#ifdef TEST
#define STATIC
#else
#define STATIC static
#endif

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
const uint32_t DEBOUNCE_SAMPLING_PERIOD_MS = 5UL;            // read the button every this ms
STATIC const uint32_t DEBOUNCE_TIME_MS = 75UL;               // Max time for the button to settle in the new state
STATIC const uint32_t DEBOUNCE_STABLE_INPUT_TIME_MS = 25UL;  // Min time of input stability required to change the state
// Value the integrator must hit to change states. There's a +1 because of how we increment/decrement. Don't worry about
// it
STATIC const int32_t INTEGRATOR_TARGET = (DEBOUNCE_STABLE_INPUT_TIME_MS / DEBOUNCE_SAMPLING_PERIOD_MS) + 1UL;

/**
 * @brief Callback used by the sampling timer to update the integrator value.
 * Reads the state of the pin passed as parameter and updates the integrator value. If the value reaches one of the two
 * targets (0 or INTEGRATOR_TARGET) it updates the state of the button.
 * @param btnPtr pointer to a button_t object
 */
void debounceButton(button_t* btnPtr) {
    if (btnPtr->debounceCycles < (DEBOUNCE_TIME_MS / DEBOUNCE_SAMPLING_PERIOD_MS)) {
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
        // app_log_debug("%"PRIu32"\r\n", btnPtr->integrator);

        /* Step 2: Update the output state based on the integrator.  Note that the
       output will only change states if the integrator has reached a limit, either
       0 or MAXIMUM. */
        if ((btnPtr->state == BUTTON_RELEASED) && (btnPtr->integrator == INTEGRATOR_TARGET)) {
            buttonSetState(btnPtr, BUTTON_PRESSED);
            // Start the longpress timer, if it kicks in, then it will be a long press
            startButtonTimer(btnPtr, TIMER_LONGPRESS);
            btnPtr->debounceCycles = 0UL;  // reset debounceCycles so it's ready for next button press/release
            if (btnPtr->pressedAction != NULL) {
                btnPtr->pressedAction(btnPtr);
            }  // else, no action on pressed
        } else if (((btnPtr->state == BUTTON_PRESSED) || (btnPtr->state == BUTTON_LONGPRESSED)) &&
                   (btnPtr->integrator == 0)) {
            // Change button state and stop sampling
            buttonSetState(btnPtr, BUTTON_RELEASED);
            // Stop the LongPress timer, in case it was running (i.e. we released after a short press)
            stopButtonTimer(btnPtr, TIMER_LONGPRESS);
            btnPtr->debounceCycles = 0UL;  // reset debounceCycles so it's ready for next button press/release
            if (btnPtr->releasedAction != NULL) {
                btnPtr->releasedAction(btnPtr);
            }  // else, no action on released
        } else {
            // else,
            //      or button is released and integrator is < INTEGRATOR_TARGET
            //      or button is pressed and integrator is > 0
            // Carry on sampling
            startButtonTimer(btnPtr, TIMER_SAMPLE);
            btnPtr->debounceCycles++;
        }
    } else {
        // else, we have reached the debounce max time and button is still in undefined state, leave things as they
        // are and stop sampling. Reset the button state based on whatever it's state is
        if (btnPtr->state == BUTTON_RELEASED) {
            btnPtr->integrator = 0UL;
            btnPtr->debounceCycles = 0UL;
        } else {  // Button is PRESSED or LONG_PRESSED
            btnPtr->integrator = INTEGRATOR_TARGET;
            btnPtr->debounceCycles = 0UL;
        }
    }
}
