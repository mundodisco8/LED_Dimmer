/**
 * @file buttons_types.h
 * @author Joel Santos (jsantosrico@gmail.com)
 * @brief Data types from buttons.h
 * @version 0.1
 * @date 2026-03-13
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef _BUTTONS_TYPES_H_
#define _BUTTONS_TYPES_H_

#include <inttypes.h>

#include "gpio_HW_types.h"
#include "sleepyTimers_HW_types.h"

typedef enum btnError {
    BTN_OK = 0,
    BTN_NO_INTS_AVAILABLE = 0x1,
    BTN_NULL_POINTER_PASSED = 0x2,
    BTN_ERROR,
} btnError_t;

// An enum to define the state of a button
typedef enum button_state { BUTTON_PRESSED = 0, BUTTON_LONGPRESSED = 1, BUTTON_RELEASED = 2 } buttonState_t;

typedef enum btnTimerType { TIMER_SAMPLE, TIMER_LONGPRESS } btnTimerType_t;

// Action callback prototype. We provide the option to pass a context to the function
typedef void (*actionCallback_t)(void* ctx);

// A struct to contain the context of a button
typedef struct button {
    pinPort_t btnPort;
    uint8_t pinNo;
    buttonState_t state;                 // The state of the button, pressed (long, short) or released
    buttonState_t prevState;             // The previous state of the button
    int32_t integrator;                  // The value of the integrator for this button
    actionCallback_t pressedAction;      // action to perform when the state changes to BUTTON_PRESSED
    actionCallback_t longPressedAction;  // action to perform when the state changes to BUTTON_LONGPRESSED
    actionCallback_t releasedAction;     // action to perform when the state changes to BUTTON_RELEASED
    uint32_t debounceCycles;             // Number of times we have sampled during debounce so far
    timerHandlePtr_t samplingTimerPtr;   // a hdlr to the timer that checks the button state sampling during debouncing
    timerHandlePtr_t longPressTimerPtr;  // a handler to the timer that checks the for long presses
} button_t;

typedef struct quad_encoder {
    pinPort_t pin0Port;
    uint8_t pin0No;
    pinPort_t pin1Port;
    uint8_t pin1No;
    actionCallback_t clockWiseAction;
    actionCallback_t counterClockWiseAction;
} quad_encoder_t;

#endif  // _BUTTONS_TYPES_H_