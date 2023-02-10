#ifndef _SLEEPYTIMERS_HW_
#define _SLEEPYTIMERS_HW_

#include <inttypes.h>
#include <stdbool.h>

#include <sleepyTimers_HW_types.h>

slpTimerStatus_t SLP_startTimer(timerHandle_t* handlePtr, uint32_t timeoutMs, timerCallback_t callback, void* ctxPtr);
slpTimerStatus_t SLP_startPeriodicTimer(timerHandle_t* handlePtr, uint32_t timeoutMs, timerCallback_t callback, void* ctxPtr);
slpTimerStatus_t SLP_stopTimer(timerHandle_t* handlePtr);

bool SLP_isTimerRunning(timerHandle_t* handlePtr);
#endif // _SLEEPYTIMERS_HW_
