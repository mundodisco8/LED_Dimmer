
#ifndef SL_SLEEPTIMER_H
#define SL_SLEEPTIMER_H

/*****************************************************************************************
 *
 * FAKE sl_sleeptimer.H USED IN TESTS. CONTAINS THE MINIMUM AMOUNT OF IT TO RUN
 *
 ****************************************************************************************/
#include <inttypes.h>
#include <stdbool.h>

#include "sl_status.h"

// Forward declaration
typedef struct sl_sleeptimer_timer_handle sl_sleeptimer_timer_handle_t;

/***************************************************************************/ /**
                                                                               * Typedef for the user supplied callback function which is called when
                                                                               * a timer expires.
                                                                               *
                                                                               * @param handle The timer handle.
                                                                               *
                                                                               * @param data An extra parameter for the user application.
                                                                               ******************************************************************************/

typedef void (*sl_sleeptimer_timer_callback_t)(sl_sleeptimer_timer_handle_t* handle, void* data);

/// @brief Timer structure for sleeptimer
struct sl_sleeptimer_timer_handle {
    void* callback_data;                      ///< User data to pass to callback function.
    uint8_t priority;                         ///< Priority of timer.
    uint16_t option_flags;                    ///< Option flags.
    sl_sleeptimer_timer_handle_t* next;       ///< Pointer to next element in list.
    sl_sleeptimer_timer_callback_t callback;  ///< Function to call when timer expires.
    uint32_t timeout_periodic;                ///< Periodic timeout.
    uint32_t delta;                           ///< Delay relative to previous element in list.
    uint32_t timeout_expected_tc;             ///< Expected tick count of the next timeout (only used for periodic timer).
    uint16_t conversion_error;                ///< The error when converting ms to ticks (thousandths of ticks)
    uint16_t accumulated_error;               ///< Accumulated conversion error (thousandths of ticks)
};

sl_status_t sl_sleeptimer_start_timer_ms(sl_sleeptimer_timer_handle_t* handle, uint32_t timeout_ms, sl_sleeptimer_timer_callback_t callback, void* callback_data, uint8_t priority,
                                         uint16_t option_flags);
sl_status_t sl_sleeptimer_ms32_to_tick(uint32_t time_ms, uint32_t* tick);
sl_status_t sl_sleeptimer_start_timer_ms(sl_sleeptimer_timer_handle_t* handle, uint32_t timeout_ms, sl_sleeptimer_timer_callback_t callback, void* callback_data, uint8_t priority,
                                         uint16_t option_flags);
sl_status_t sl_sleeptimer_start_periodic_timer_ms(sl_sleeptimer_timer_handle_t* handle, uint32_t timeout_ms, sl_sleeptimer_timer_callback_t callback, void* callback_data, uint8_t priority,
                                                  uint16_t option_flags);
sl_status_t sl_sleeptimer_stop_timer(sl_sleeptimer_timer_handle_t* handle);
sl_status_t sl_sleeptimer_is_timer_running(sl_sleeptimer_timer_handle_t* handle, bool* running);
uint64_t sl_sleeptimer_get_tick_count64(void);
sl_status_t sl_sleeptimer_tick64_to_ms(uint64_t tick, uint64_t* ms);

#endif  // SL_SLEEPTIMER_H