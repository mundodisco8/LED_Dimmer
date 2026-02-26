#include "sleepyTimers_HW.h"

#include <stddef.h>

// Silabs SDK headers
// Ignore a cast-align warning in some cmsis header and a sign conversion in
// sl_sleeptimer.h
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-align"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include "sl_sleeptimer.h"
#pragma GCC diagnostic pop
#include "app_log.h"
#include "sl_bluetooth_config.h"  // This is only needed to get the max number of timers

// timerHandlePtr_t will be exposed as an incomplete type, so we can't de-reference it and
// access the members of sl_sleeptimer_timer_handle_t through it
typedef sl_sleeptimer_timer_handle_t timerHandle_t;

// static bool isTimerRunning(timer_handle_t* handlePtr);

static sl_sleeptimer_timer_handle_t timerArray[SL_BT_CONFIG_MAX_SOFTWARE_TIMERS] = {0};
static uint8_t timersUsed = 0;

size_t a = sizeof(timerArray);

/// @brief Resets the number of timers used to 0
/// NOTE: this doesn't get used in the code, as there's no need to dynamically change the
/// timer assignment, but it's needed in testing, or we would quickly run out of timers :D
void SLP_resetTimersUsed(void) { timersUsed = 0; }

slpTimerStatus_t SLP_reserveTimer(timerHandlePtr_t* handlePtr) {
    // Check current number of given timers
    if (timersUsed >= SL_BT_CONFIG_MAX_SOFTWARE_TIMERS) {
        *handlePtr = NULL;
        // TODO: assert?
        return SLPTIMER_NO_TIMERS_AVAILABLE;
    }
    // Else, we can still give a timer. Make handlePtr point to the next available timer.
    *handlePtr = (timerHandlePtr_t) & (timerArray[timersUsed]);
    timersUsed++;
    return SLPTIMER_OK;
}

/**
 * @brief Starts a one-shot timer, the timer won't restart after expiring
 * @param handlePtr a pointer to a timer object
 * @param timeoutMs the duration of the timer in ms
 * @param expireBehaviour if the timer will be one shot or rearm on expire
 * @param callback (optional) a callback funtion of type timerCallback_t to be called on timer expiry
 * @param ctxPtr (optional) a context to pass to the callback function
 * @return SLPTIMER_OK on success or SLPTIMER_ERROR on error
 */
slpTimerStatus_t SLP_startTimer(timerHandlePtr_t handlePtr, uint32_t timeoutMs, expBehaviour_t expireBehaviour,
                                timerCallback_t callback, void* ctxPtr) {
    // sl_sleeptimer_start_*_timer_ms() returns SL_STATUS_OK on success and
    // 1) 0x21 - SL_STATUS_INVALID_PARAMETER if time is out of bounds:
    //    max_millisecond_conversion = (uint32_t)(((uint64_t)UINT32_MAX * (uint64_t)1000u) / timer_frequency);
    //    where timer_frequency is 32768 -> max_millisecond_conversion = 131071999ms.
    // 2) 0x22 - SL_STATUS_NULL_POINTER if the timer handler is null
    // 3) 0x03 - SL_STATUS_NOT_READY if the timer is already running
    // Surprisingly, the callback can be NULL: it won't get called and it won't cause an error
    uint32_t retVal = SL_STATUS_FAIL;
    if (expireBehaviour == TIMER_ONE_SHOT) {
        retVal = sl_sleeptimer_start_timer_ms((sl_sleeptimer_timer_handle_t*)handlePtr, timeoutMs,
                                              (sl_sleeptimer_timer_callback_t)callback, ctxPtr, 0, 0);
    } else {
        retVal = sl_sleeptimer_start_periodic_timer_ms((sl_sleeptimer_timer_handle_t*)handlePtr, timeoutMs,
                                                       (sl_sleeptimer_timer_callback_t)callback, ctxPtr, 0, 0);
    }
    if ((retVal == SL_STATUS_INVALID_STATE) || (retVal == SL_STATUS_NOT_READY)) {
        // start_timer retunrs STATUS_NOT_READY when the timer is already running, but periodic_timer returns INVALID_STATE...
        app_log_warning("Timer already running\r\n");
        // no need to return, the timer was already running, all is good
    } else if (retVal != SL_STATUS_OK) {
        // If not INVALID_STATE or OK, then it's error
        app_log_error("Error 0x%04" PRIX32 " starting timer\r\n", retVal);
        return SLPTIMER_ERROR;
    }
    return SLPTIMER_OK;
}

slpTimerStatus_t SLP_stopTimer(timerHandlePtr_t handlePtr) {
    uint32_t retVal = sl_sleeptimer_stop_timer((sl_sleeptimer_timer_handle_t*)handlePtr);
    if (retVal != SL_STATUS_OK) {
        // sl_sleeptimer_stop_timer() fails if the handle passed is null or if the list of timers is in the wrong state
        // we can't do much about the second.
        app_log_error("Error 0x%04" PRIX32 " stopping timer\r\n", retVal);
        return SLPTIMER_ERROR;
    }
    return SLPTIMER_OK;
}

// Tells if a timer is currently running or not
// Parameters: handlePtr, a pointer to the timer to check
// Returns: true if the timer is running, and false if it isn't or there was a problem checking it
bool SLP_isTimerRunning(timerHandlePtr_t handlePtr) {
    bool timerRunning = false;
    // TODO: Check null pointer
    // sl_sleeptimer_is_timer_running returns SL_STATUS_OK on success or SL_STATUS_NULL_POINTER if any of the
    // passed parameters are null pointers
    uint32_t retVal = sl_sleeptimer_is_timer_running((sl_sleeptimer_timer_handle_t*)handlePtr, &timerRunning);
    if (retVal != SL_STATUS_OK) {
        app_log_error("Error 0x%04" PRIX32 " checking if timer running\r\n", retVal);
        return false;
    }
    // app_log_debug("%s\r\n", timerRunning?"timer is running":"timer is not running");
    return timerRunning;
}

// Returns the current system tick in ms
uint64_t SLP_getSystemTickInMs(void) {
    uint64_t timeInMs = 0;
    if (sl_sleeptimer_tick64_to_ms(sl_sleeptimer_get_tick_count64(), &timeInMs) == SL_STATUS_OK) {
        return timeInMs;
    }
    return 0;
}