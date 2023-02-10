#include "sleepyTimers_HW.h"

#include <stdbool.h>

#include "sl_sleeptimer.h"
#include "app_log.h"

////
// Forward declarations
////

// static bool isTimerRunning(timer_handle_t* handlePtr);

// Starts a one-shot timer
// Parameters: handlePtr, a pointer to a timerHandle_t object
//             timeoutMs, the duration of the timer in ms
//             callback, (optional) a callback funtion of type timerCallback_t to be called on timer expiry
//             ctxPtr, (optional) a context to pass to the callback function
// Returns: SL_STATUS_OK on success or
//          SL_STATUS_INVALID_PARAMETER if timeoutMs > 131071999mssion = .
//          SL_STATUS_NULL_POINTER if the timer handler is null
//          SL_STATUS_NOT_READY if the timer is already running (can be treated as a warning)
slpTimerStatus_t SLP_startTimer(timerHandle_t* handlePtr, uint32_t timeoutMs, timerCallback_t callback, void* ctxPtr) {
    // sl_sleeptimer_start_periodic_timer_ms() returns SL_STATUS_OK on success and
    // 1) 0x21 - SL_STATUS_INVALID_PARAMETER if time is out of bounds:
    //    max_millisecond_conversion = (uint32_t)(((uint64_t)UINT32_MAX * (uint64_t)1000u) / timer_frequency);
    //    where timer_frequency is 32768 -> max_millisecond_conversion = 131071999ms.
    // 2) 0x22 - SL_STATUS_NULL_POINTER if the timer handler is null
    // 3) 0x03 - SL_STATUS_NOT_READY if the timer is already running
    // Surprisingly, the callback can be NULL: it won't get called and it won't cause an error
    uint32_t retVal = sl_sleeptimer_start_timer_ms((sl_sleeptimer_timer_handle_t*) handlePtr, timeoutMs, (sl_sleeptimer_timer_callback_t)callback, ctxPtr, 0, 0);
    if (retVal == SL_STATUS_NOT_READY) {
        app_log_warning("Timer already running\r\n");
        // no need to return, the timer was already running, all is good
    } else if (retVal != SL_STATUS_OK) {
        // If not INVALID_STATE or OK, then it's error
        app_log_error("Error 0x%04X starting periodic timer\r\n", retVal);
        return SLPTIMER_ERROR;
    }
    return SLPTIMER_OK;
}

// Starts a periodic timer
// Parameters: handlePtr, a pointer to a timerHandle_t object
//             timeoutMs, the duration of the timer in ms
//             callback, (optional) a callback funtion of type timerCallback_t to be called on timer expiry
//             ctxPtr, (optional) a context to pass to the callback function
// Returns: SL_STATUS_OK on success or
//          SL_STATUS_INVALID_PARAMETER if timeoutMs > 131071999mssion = .
//          SL_STATUS_NULL_POINTER if the timer handler is null
//          SL_STATUS_INVALID_STATE id the timer is already running (can be treated as a warning)
// NOTE: that if the timer is running, periodic returns INVALID_STATE and one-shot returns NOT_READY!
slpTimerStatus_t SLP_startPeriodicTimer(timerHandle_t* handlePtr, uint32_t timeoutMs, timerCallback_t callback, void* ctxPtr){
    // sl_sleeptimer_start_periodic_timer_ms() returns SL_STATUS_OK on success and
    // 1) SL_STATUS_INVALID_PARAMETER if time is out of bounds:
    //    max_millisecond_conversion = (uint32_t)(((uint64_t)UINT32_MAX * (uint64_t)1000u) / timer_frequency);
    //    where timer_frequency is 32768 -> max_millisecond_conversion = 131071999ms.
    // 2) SL_STATUS_NULL_POINTER if the timer handler is null
    // 3) SL_STATUS_INVALID_STATE id the timer is already running
    // Surprisingly, the callback can be NULL: it won't get called and it won't cause an error
    uint32_t retVal = sl_sleeptimer_start_periodic_timer_ms((sl_sleeptimer_timer_handle_t*) handlePtr, timeoutMs, (sl_sleeptimer_timer_callback_t)callback, ctxPtr, 0, 0);
    app_log_debug("Starting periodic returns 0x%04\r\n", retVal);
    if (retVal == SL_STATUS_INVALID_STATE) {
        app_log_warning("Timer already running\r\n");
        // no need to return, the timer was already running, all is good
    } else if (retVal != SL_STATUS_OK) {
        // If not INVALID_STATE or OK, then it's error
        app_log_error("Error 0x%04X starting periodic timer\r\n", retVal);
        return SLPTIMER_ERROR;
    }
    return SLPTIMER_OK;
}

slpTimerStatus_t SLP_stopTimer(timerHandle_t* handlePtr) {
    uint32_t retVal = sl_sleeptimer_stop_timer((sl_sleeptimer_timer_handle_t*)handlePtr);
    if (retVal != SL_STATUS_OK) {
        // sl_sleeptimer_stop_timer() fails if the handle passed is null or if the list of timers is in the wrong state
        // we can't do much about the second.
        app_log_error("Error 0x%04X stopping timer\r\n", retVal);
        return SLPTIMER_ERROR;
    }
    return SLPTIMER_OK;
}

// Tells if a timer is currently running or not
// Parameters: handlePtr: pointer to the handle for the timer to check.
// Returns: true if the timer is running, and false if it isn't or there was a problem checking it
bool SLP_isTimerRunning(timerHandle_t* handlePtr) {
    bool timerRunning = false;
    // sl_sleeptimer_is_timer_running returns SL_STATUS_OK on success or SL_STATUS_NULL_POINTER if any of the
    // passed parameters are null pointers
    uint32_t retVal = sl_sleeptimer_is_timer_running((sl_sleeptimer_timer_handle_t*) handlePtr, &timerRunning);
    if (retVal != SL_STATUS_OK) {
        app_log_error("Error 0x%04X checking if timer running\r\n", retVal);
        return false;
    }
    // app_log_debug("%s\r\n", timerRunning?"timer is running":"timer is not running");
    return timerRunning;
}
