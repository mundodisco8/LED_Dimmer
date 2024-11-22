#ifndef _SLEEPYTIMERS_HW_
#define _SLEEPYTIMERS_HW_

#include <inttypes.h>
#include <stdbool.h>

#include <sleepyTimers_HW_types.h>

// TODO: Ok, rookie mistake. I created a struct, timerHandle, that had the same elements as sl_sleeptimer_timer_handle
// to avoid exporting it, as it's an SDK thingy. We moved to a newer SDK and boom, they change the definition, their's
// is bigger than mine, and I start to overwrite memory.
// What I should have done then, but did't knew, was to use an abstract type. I should have defined a pointer, and then
// this module should deal with the conversions. Because it's an incomplete type, it should not be deferenced outside
// of this module. The problem is that I can't provide memory for it outside of this module, either, as no other code
// knows how big it is. or what it contains.
// What I can do is get a buffer of timers (as I know how many I can have, it's a global) and then request "timers" from
// there. I think it can work. I return the pointers to sl_sleeptimer_timer_handle's as mypointer_t, work with the
// pointers, and then if I need to touch something from it, provide a getter from this module

/// @brief Resets the number of timers used to 0
/// NOTE: this doesn't get used in the code, as there's no need to dynamically change the
/// timer assignment, but it's needed in testing, or we would quickly run out of timers :D
void SLP_resetTimersUsed(void);

slpTimerStatus_t SLP_reserveTimer(timerHandlePtr_t* handlePtr);

slpTimerStatus_t SLP_startTimer(timerHandlePtr_t handlePtr, uint32_t timeoutMs, timerCallback_t callback, void* ctxPtr);
slpTimerStatus_t SLP_startPeriodicTimer(timerHandlePtr_t handlePtr, uint32_t timeoutMs, timerCallback_t callback, void* ctxPtr);
slpTimerStatus_t SLP_stopTimer(timerHandlePtr_t handlePtr);

bool SLP_isTimerRunning(timerHandlePtr_t handlePtr);
#endif // _SLEEPYTIMERS_HW_
