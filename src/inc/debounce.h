/*
 * debounce.h
 *
 *  Created on: 15 Nov 2022
 *      Author: joel.santos
 */

#ifndef SRC_DEBOUNCE_H_
#define SRC_DEBOUNCE_H_

#include "buttons.h"

typedef enum timerType {
    TIMER_SAMPLE,
    TIMER_DEBOUNCE
} timerType_t;

// Callback used by the sampling timer to update the integrator value.
void samplingTimerCallback(button_t* btnPtr);

uint32_t startButtonTimer(button_t* btnPtr, timerType_t timerType);

#endif /* SRC_DEBOUNCE_H_ */
