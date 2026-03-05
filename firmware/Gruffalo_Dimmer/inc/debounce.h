/*
 * debounce.h
 *
 *  Created on: 15 Nov 2022
 *      Author: joel.santos
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
