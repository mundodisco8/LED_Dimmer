/**
 * @file timer_HW_types.h
 * @author Joel Santos (jsantosrico@gmail.com)
 * @brief datatypes for the timer_HW types
 * @version 0.1
 * @date 2026-03-13
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef _TIMER_HW_TYPES_H_
#define _TIMER_HW_TYPES_H_

/**
 * @brief typedef for the CC Channels
 */
typedef enum CCChannel { CC_CHANNEL_0 = 0, CC_CHANNEL_1 = 1, CC_CHANNEL_2 = 2, CC_MAX_CHANNELS } CCChannel_t;

/**
 * @brief error codes for the timer_HW module
 */
typedef enum timerStatus {
    TIMER_OK = 0,
    TIMER_WRONG_CC_CHANNEL = 1,
    TIMER_DISBLED_BEFORE_WRITING_SYNC = 2,    // Tried to write to a SYNC register with the module disabled
    TIMER_ENABLED_BEFORE_WRITING_CONFIG = 3,  // tried to write to a CONFIG register with the module enabled
} timerStatus_t;

/**
 * @brief a typedef for the types of PWM wave generation
 */
typedef enum polarity {
    PWM_ACTIVE_HIGH = 0,
    PWM_ACTIVE_LOW = 1,
} polarity_t;

#endif  // _TIMER_HW_TYPES_H_
