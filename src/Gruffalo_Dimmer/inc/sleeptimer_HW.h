#ifndef _SLEEPTIMER_HW_H
#define _SLEEPTIMER_HW_H

#include "inttypes.h"

#include "buttons.h"

// This would be part of a real sleeptimer_HW were we only wrap the sleeptimer functions
// TODO: test this as a way to export timer handles without sharing
typedef struct button_timer_handle button_timer_handle_t;
// Add functions to "create/initialise" a timer
// Because we are using static allocation, the only way to do it is to have an array of sl_timers and grab them
// as you need them, and error out if too many are requested?
// Add function to assign a button_timer to a button / assign a sleeptimer to the button_timer member of the button_t struct?

// This would be part of a new "buttonTimers.h" or something like that.

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

#endif // _SLEEPTIMER_HW_H