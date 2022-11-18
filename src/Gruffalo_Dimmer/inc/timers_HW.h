#ifndef TIMERS_HW_H
#define TIMERS_HW_H

#include "inttypes.h"

#include "buttons.h"

// Sets the debounce timer for btn 0
uint32_t setDebounceTimerBnt0(button_t* btnPtr);
// Sets the debounce timer for btn 1
uint32_t setDebounceTimerBnt1(button_t* btnPtr);

// Sets the sampling timer for btn 0
uint32_t setSamplingTimerBnt0(button_t* btnPtr);
// Sets the sampling timer for btn 1
uint32_t setSamplingTimerBtn1(button_t* btnPtr);
// Stops the timers associated to a button
uint32_t stopButtonTimers(button_t* btnPtr);

#endif // TIMERS_HW_H