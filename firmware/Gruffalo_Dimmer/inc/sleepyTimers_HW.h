/**
 * @file sleepyTimers_HW.h
 * @author Joel Santos (jsantosrico@gmail.com)
 * @brief Public API for the low-power SleepyTimer Hardware Abstraction Layer.
 * @version 0.1
 * @date 2026-03-13
 * * @copyright Copyright (c) 2026
 * * @defgroup SLEEPY_TIMERS Sleepy Timers Module
 * @{
 *
 * ### Detailed Description
 * This module provides a safe wrapper around the Silicon Labs `sl_sleeptimer` SDK.
 * It manages a pool of hardware-backed software timers used for debouncing,
 * long-press detection, and animation timing.
 * * ### Important Architectural Note
 * To avoid memory corruption caused by SDK version changes, this module uses
 * **Incomplete Types** for timer handles. By exposing `timerHandlePtr_t` as an
 * abstract pointer, we ensure that the rest of the application remains agnostic
 * to the internal size and structure of the SDK's `sl_sleeptimer_timer_handle_t`.
 *
 * ### Key Features
 * * **Timer Pooling:** Manages a fixed array of timers based on @ref SL_BT_CONFIG_MAX_SOFTWARE_TIMERS.
 * * **Flexible Expiration:** Supports both Periodic (Restart) and One-Shot behaviors.
 * * **Context Support:** Allows passing `void*` context pointers to callbacks for stateful handling.
 *
 * ### Usage Flow
 * 1. **Reserve:** Obtain a pointer to an available timer using @ref SLP_reserveTimer.
 * 2. **Start:** Configure the period and callback via @ref SLP_startTimer.
 * 3. **Stop:** Manually halt a running timer using @ref SLP_stopTimer.
 */
#ifndef _SLEEPYTIMERS_HW_
#define _SLEEPYTIMERS_HW_

#include <inttypes.h>
#include <sleepyTimers_HW_types.h>
#include <stdbool.h>

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

/**
 * @brief The possible restart behaviours of a timer
 */
typedef enum restartBehaviour {
    TIMER_RESTART = 0,  // Timer will restart after expiring
    TIMER_ONE_SHOT = 1  // Timer will not restart after expiring
} expBehaviour_t;

/**
 * @brief Resets the number of timers used to 0
 * NOTE: this doesn't get used in the code, as there's no need to dynamically change the
 * timer assignment, but it's needed in testing, or we would quickly run out of timers :D
 */
void SLP_resetTimersUsed(void);

/**
 * @brief Assigns one of the available sleeptimers
 */
slpTimerStatus_t SLP_reserveTimer(timerHandlePtr_t* handlePtr);

/**
 * @brief Starts a one-shot timer, the timer won't restart after expiring
 */
slpTimerStatus_t SLP_startTimer(timerHandlePtr_t handlePtr, uint32_t timeoutMs, expBehaviour_t expireBehaviour,
                                timerCallback_t callback, void* ctxPtr);

/**
 * @brief Stops a timer.
 */
slpTimerStatus_t SLP_stopTimer(timerHandlePtr_t handlePtr);

/**
 * @brief Tells if a timer is currently running or not
 */
bool SLP_isTimerRunning(timerHandlePtr_t handlePtr);

/**
 * @brief Returns the current system tick in ms
 */
uint64_t SLP_getSystemTickInMs(void);
#endif  // _SLEEPYTIMERS_HW_
