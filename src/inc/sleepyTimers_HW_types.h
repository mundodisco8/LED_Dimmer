#ifndef _SLEEPYTIMERS_HW_TYPES_H_
#define _SLEEPYTIMERS_HW_TYPES_H_

// timerHandle_t is an incomplete type, used to share pointers to sl_sleeptimer handlers without having to export
// the SDK files
struct timerHandle_t;
typedef struct timerHandle_t* timerHandlePtr_t;

// timerCallback_t is an equivalent type to sl_sleeptimer_timer_callback_t, so we can load our own functions as
// callbacks for out timers, without having to expose the SDK headers
typedef void (*timerCallback_t)(timerHandlePtr_t* handle, void* data);

typedef enum slpTimerStatus{
    SLPTIMER_OK = 0,
    SLPTIMER_NO_TIMERS_AVAILABLE = 1,
    SLPTIMER_ERROR,
} slpTimerStatus_t;

#endif // _SLEEPYTIMERS_HW_TYPES_H_
