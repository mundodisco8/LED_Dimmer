#ifndef _SLEEPYTIMERS_HW_
#define _SLEEPYTIMERS_HW_

#include <inttypes.h>

// NOTE: This is kind of dodgy... but it should be compliant.

// Forward declaration
typedef struct timerHandle timerHandle_t;

// A type for callback functions. It's compatible with sl_sleeptimer_timerCallback_t
// typedef void (*sl_sleeptimer_timerCallback_t)(sl_sleeptimer_timerHandle_t *handle, void *data);
//
// 1) Returns the same type: void
// 2) It has the same number of parameters, 2
// 3) sl_sleeptimer_timerHandle_t* and timerHandle_t* are compatible (timerHandle_t is really a renaming
// of sl_sleeptimer_timerHandle_t)
typedef void (*timerCallback_t)(timerHandle_t* handle, void* data);

// Similarly, redefining the sl_sleeptimer_timerHandle_t to be able to export it. Everything compatible as the only
// "non-standard" types, timerHandle_t and timerCallback_t, are renamings. Casting between timerHandle_t and
// sl_sleeptimer_timerHandle_t is compatible.
struct timerHandle {
  void *callback_data;                     ///< User data to pass to callback function.
  uint8_t priority;                        ///< Priority of timer.
  uint16_t option_flags;                   ///< Option flags.
  timerHandle_t *next;      ///< Pointer to next element in list.
  timerCallback_t callback; ///< Function to call when timer expires.
  uint32_t timeout_periodic;               ///< Periodic timeout.
  uint32_t delta;                          ///< Delay relative to previous element in list.
  uint32_t timeout_expected_tc;            ///< Expected tick count of the next timeout (only used for periodic timer).
};

typedef enum timerStatus {
    TIMER_OK = 0,
} timerStatus_t;

timerStatus_t SLP_startTimer(timerHandle_t* handlePtr, uint32_t timeoutMs, timerCallback_t callback, void* ctxPtr);
timerStatus_t SLP_startPeriodicTimer(timerHandle_t* handlePtr, uint32_t timeoutMs, timerCallback_t callback, void* ctxPtr);

#endif // _SLEEPYTIMERS_HW_