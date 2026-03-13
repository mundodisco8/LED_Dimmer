/**
 * @file sleepyTimers_HW_types.h
 * @author Joel Santos (jsantosrico@gmail.com)
 * @brief datatypes for the sleepytimers_HW module
 * @version 0.1
 * @date 2026-03-13
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef _SLEEPYTIMERS_HW_TYPES_H_
#define _SLEEPYTIMERS_HW_TYPES_H_

/**
 * @brief timerHandle_t is an incomplete type, used to share pointers to sl_sleeptimer handlers without having to export
 * the SDK files
 * WARNING! Incomplete type, don't dereference it!
 */
struct timerHandle_t;
typedef struct timerHandle_t* timerHandlePtr_t;

/**
 * @brief timerCallback_t is an equivalent type to sl_sleeptimer_timer_callback_t, so we can load our own functions as
 * callbacks for out timers, without having to expose the SDK headers
 */
typedef void (*timerCallback_t)(timerHandlePtr_t handlePtr, void* data);

/**
 * @brief Error codes for the module
 */
typedef enum slpTimerStatus {
    SLPTIMER_OK = 0,
    SLPTIMER_NO_TIMERS_AVAILABLE = 1,  // No more timers available, allocate more
    SLPTIMER_ERROR,                    // Generic error
} slpTimerStatus_t;

#endif  // _SLEEPYTIMERS_HW_TYPES_H_
