////
// Button Actions
//
// This library contains the actions to perform when the buttons and quad encoders are actuated.
////

#include "buttonActions.h"

// Other SiLabs headers
// Ignore a cast-align warning in some cmsis header and a sign conversion in
// sl_sleeptimer.h
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-align"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include "app_log.h"
#pragma GCC diagnostic pop

// Project Libraries
#include "PWMControl.h"
#include "buttons.h"
#include "debounce.h"
#include "gpio_HW.h"

// TODO: Maybe we can store errors in some variables, and then check for interrutp
// errors in the main loop?

void gpioCallbackButton(uint8_t intNo, void* ctx) {
    (void)intNo;
    // set debounce timer
    //startButtonTimer((button_t*)ctx, TIMER_DEBOUNCE);
    // set sampling timer
    startButtonTimer((button_t*)ctx, TIMER_SAMPLE);
}

void gpioCallbackQuad(uint8_t intNo, void* ctx) {
    (void)intNo;
    // Trivial, but makes it easier to read
    quad_encoder_t* quadPtr = (quad_encoder_t*)ctx;
    // The pin 0 has triggered a falling interrupt, read pin 1 to determine if we are CW or CCW
    if (readPin(quadPtr->pin1Port, quadPtr->pin1No) == 1) {
        quadPtr->clockWiseAction();
    } else {
        quadPtr->counterClockWiseAction();
    }
}

static int8_t currPercent = 0;

void button1Pressed(void) {
    currPercent += 10;
    if (currPercent > 100) {
        currPercent = 0;
    }
    app_log_info("Set PWM to %d\r\n", currPercent);
    //setDutyCycle(CC_CHANNEL_0, currPercent);
}

void button1Released(void) {
    // app_log_debug("Btn1 Released\r\n");
}

void quad1ClockWise(void) {
    if (currPercent < 100) {
        currPercent++;
        app_log_debug("%d\r\n", currPercent);
        //setDutyCycle(CC_CHANNEL_0, currPercent);
    }
}

void quad1CounterClockWise(void) {
    if (currPercent > 0) {
        currPercent--;
        app_log_debug("%d\r\n", currPercent);
        //setDutyCycle(CC_CHANNEL_0, currPercent);
    }
}