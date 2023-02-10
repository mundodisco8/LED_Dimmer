#ifndef _SLEEPYTIMERS_HW_TYPES_H_
#define _SLEEPYTIMERS_HW_TYPES_H_

typedef enum {
    SLPTIMER_OK = 0,
    SLPTIMER_ERROR = 1,
} slpTimerStatus_t;

// Forward declaration
typedef struct timerHandle timerHandle_t;

typedef void (*timerCallback_t)(timerHandle_t *handle, void *data);

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

#endif // _SLEEPYTIMERS_HW_TYPES_H_