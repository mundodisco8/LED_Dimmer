
////
// sleeptimer_HW
////

// Wrapper Library for the sleeptimer component, to provide a seam for unit testing
//
// This library is a bit trickier to desing, because to set a timer you need a timer handle, which is a type defined
// by the `sl_sleeptimer.h` library. Options:
// 1) You could "hide" the handle, but then you can't reuse the same function to set different timers
// 2) We prepare an array of timer handles, and refer to them via pointers. The drawback is that you can
//    only have a number of timers.
// 3) Just set functions for each timer that you will need. It's the least flexible, but allows hiding all the
//    'sl_sleeptimer.h` junk under the hood
// I'll try 3 and see how it goes. It's a bit ugly and not as flexible as I wanted, but this is just a hobby project

#include "sleeptimer_HW.h"

#include "em_gpio.h"

#include "pin_config.h"
#include "sl_sleeptimer.h"
#include "app_log.h"

#include "debounce.h"

struct button_timer_handle {
  sl_sleeptimer_timer_handle_t* joelTimerPtr;
};

// TODO: can we include these timers in the button_t object?
// Handles for the two timers for each of the two buttons we have.
sl_sleeptimer_timer_handle_t debounceTimerBtn0;
sl_sleeptimer_timer_handle_t debounceTimerBtn1;

sl_sleeptimer_timer_handle_t samplingTimerBtn0;
sl_sleeptimer_timer_handle_t samplingTimerBtn1;

// Tells if a timer is currently running or not
// Parameters: handlePtr: pointer to the handle for the timer to check.
// Returns: true if the timer is running, and false if it isn't or there was a problem checking it
static bool isTimerRunning(sl_sleeptimer_timer_handle_t* handlePtr) {
    bool timerRunning = false;
    uint32_t retVal = sl_sleeptimer_is_timer_running(handlePtr, &timerRunning);
    if (retVal != SL_STATUS_OK) {
        app_log_error("Error 0x%04X checking if timer running\r\n", retVal);
        return false;
    }
    return timerRunning;
}

////
// Timer Callbacks
////

// Action to run when a debounce timer has timed out
static void sleeptimerDebounceCallback(sl_sleeptimer_timer_handle_t *handle, void *data) {
    // If the debounce timer callback is executed, it means the integrator hasn't converged in the debouncing time
    // given. Stop the sampling timer, because it's periodic, and reset the integrator value to the default value
    // for the current state
    (void) handle;
    app_log_warning("Debouncer Callback\r\n");
    stopButtonTimers((button_t*)data);
}

// Action to run when a sampling timer has timed out
static void sleeptimerSamplingCallback(sl_sleeptimer_timer_handle_t *handle, void *data) {
    (void) handle;
    // When a sampling sleeptimer runs out, calls samplingTimerCallback from debounce.h, passing a button_t object as
    // context data.
    samplingTimerCallback((button_t*) data);
}

////
// Set Timer functions
////

// Sets the debounce timer for btn 0
// Parameters: btnPtr, a pointer to a button_t object to be passed as a context
uint32_t setDebounceTimerBnt0(button_t* btnPtr) {
    // return sl_sleeptimer_start_periodic_timer_ms(debounceTimerBtn0, getDebounceTime(), , callbackDataPtr, 0, 0);
  (void) btnPtr;
    return 0;
}

// Sets the debounce timer for btn 1
// Parameters: btnPtr, a pointer to a button_t object to be passed as a context
uint32_t setDebounceTimerBnt1(button_t* btnPtr){
  (void) btnPtr;
        uint32_t retVal = SL_STATUS_OK;
    if (!isTimerRunning(&debounceTimerBtn1)) {
        retVal = sl_sleeptimer_start_timer_ms(&debounceTimerBtn1, getDebounceTime(), sleeptimerDebounceCallback, btnPtr, 0, 0);
        if (retVal != SL_STATUS_OK) {
            app_log_error("Error 0x%04X starting debounce timer btn1 running\r\n", retVal);
            return retVal;
        }
    }
    return retVal;
}

// Sets the sampling timer for btn 0
// Parameters: btnPtr, a pointer to a button_t object to be passed as a context
uint32_t setSamplingTimerBnt0(button_t* btnPtr){
    // return sl_sleeptimer_start_periodic_timer_ms(debounceTimerBtn0, getDebounceTime(), sleeptimerSamplingCallback, btnPtr, 0, 0);
  (void) btnPtr;
    return 0;
}

// Sets the sampling timer for btn 1
// Parameters: btnPtr, a pointer to a button_t object to be passed as a context
uint32_t setSamplingTimerBtn1(button_t* btnPtr){
    uint32_t retVal = SL_STATUS_OK;
    if (!isTimerRunning(&samplingTimerBtn1)) {
        retVal = sl_sleeptimer_start_periodic_timer_ms(&samplingTimerBtn1, getDebounceSamplingPeriod(), sleeptimerSamplingCallback, btnPtr, 0, 0);
        if (retVal != SL_STATUS_OK) {
            app_log_error("Error 0x%04X starting sampling timer btn1\r\n", retVal);
            return retVal;
        }
        // app_log_debug("Sampling Timer1 Set\r\n");
    }
    return retVal;
}

// Stops the timers associated to a button
// Parameters: btnPtr, a pointer to a button_t object whose timers are going to be stopped
uint32_t stopButtonTimers(button_t* btnPtr){
    uint32_t retVal = SL_STATUS_OK;
    if ((btnPtr->btnPort = (pinPort_t)btn1_PORT) && (btnPtr->pinNo == btn1_PIN)) {
        if (isTimerRunning(&debounceTimerBtn1)) {
            retVal = sl_sleeptimer_stop_timer(&debounceTimerBtn1);
            if (retVal != SL_STATUS_OK) {
                app_log_error("Error 0x%04X stopping debounce timer Btn1\r\n", retVal);
                return retVal;
            }
        }
        if (isTimerRunning(&samplingTimerBtn1)) {
            retVal = sl_sleeptimer_stop_timer(&samplingTimerBtn1);
            if (retVal != SL_STATUS_OK) {
                app_log_error("Error 0x%04X stopping sampling timer Btn1\r\n", retVal);
            return retVal;
            }
        }
    // } else ((btnPtr->btnPort = btn0_PORT) && (btnPtr->pinNo == btn0_PIN)) {
    //    sl_sleeptimer_stop_timer(&debounceTimerBtn0);
    //   sl_sleeptimer_stop_timer(&samplingTimerBtn0);
    // }
    } else {
        //wrong pin config
        app_log_error("Wrong button context!\r\n");
        retVal = SL_STATUS_INVALID_PARAMETER;
    }
    return retVal;
}

////
// Hardware Timer
////
