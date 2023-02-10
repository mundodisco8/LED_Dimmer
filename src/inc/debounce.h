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

// TO DELETE?
// Callback used by the sampling timer to update the integrator value.
void samplingTimerCallback(button_t* btnPtr);

// TO DELETE?
//// Returns the value of the duration of the debounce timer.
//uint32_t getDebounceTime(void);
//// Returns the value of the debounce sampling period.
//uint32_t getDebounceSamplingPeriod(void);

uint32_t startButtonTimer(button_t* btnPtr, timerType_t timerType);

#endif /* SRC_DEBOUNCE_H_ */
