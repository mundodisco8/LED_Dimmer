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

#ifndef SRC_DEBOUNCE_H_
#define SRC_DEBOUNCE_H_

#include "buttons_types.h"

////
// Globals
////
extern const uint32_t DEBOUNCE_SAMPLING_PERIOD_MS;  // read the button every this ms

/**
 * @brief Callback used by the sampling timer to update the integrator value.
 * Reads the state of the pin passed as parameter and updates the integrator value. If the value reaches one of the two
 * targets (0 or INTEGRATOR_TARGET) it updates the state of the button.
 */
void debounceButton(button_t* btnPtr);

#endif /* SRC_DEBOUNCE_H_ */
