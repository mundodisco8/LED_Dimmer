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
#include "sleepyTimers_HW.h"

// Trickery to allow testing of static elements. Better to mess a bit with the code than to overcomplicate tests
#ifdef TEST
#define STATIC
#else
#define STATIC static
#endif

static const uint32_t LONG_PRESS_DELTA =
    1000;  // Time delta in ms from button press to button release to consider it a long press

// TODO: Maybe we can store errors in some variables, and then check for interrutp
// errors in the main loop?

void gpioCallbackButton(uint8_t intNo, void* ctx) {
    (void)intNo;
    // set debounce timer
    startButtonTimer((button_t*)ctx, TIMER_DEBOUNCE);
    // set sampling timer
    startButtonTimer((button_t*)ctx, TIMER_SAMPLE);
}

void gpioCallbackQuad(uint8_t intNo, void* ctx) {
    (void)intNo;
    // Trivial, but makes it easier to read
    quad_encoder_t* quadPtr = (quad_encoder_t*)ctx;
    // The pin 0 has triggered a falling interrupt, read pin 1 to determine if we are CW or CCW
    if (readPin(quadPtr->pin1Port, quadPtr->pin1No) == 1) {
        quadPtr->clockWiseAction(NULL);
    } else {
        quadPtr->counterClockWiseAction(NULL);
    }
}

STATIC int8_t currPercent[3] = {0, 0, 0};
STATIC CCChannel_t currChannel[3] = {CC_CHANNEL_0, CC_CHANNEL_1, CC_CHANNEL_2};
STATIC uint8_t channelIdx = 0;

void button0Pressed(void* ctx) {
    (void)ctx;
    // app_log_debug("Btn0 Pressed\r\n");
}

void button0Released(void* ctx) {
    // app_log_debug("Btn0 Released\r\n");
    // Cast the context as a button_t pointer
    button_t* btnPtr = (button_t*)ctx;
    uint64_t currTime = SLP_getSystemTickInMs();
    if ((currTime - btnPtr->lastPressMs) > LONG_PRESS_DELTA) {
        // Long Press
        app_log_debug("IT WAS A LONGPRESS\r\n");
    } else {
        // Short Press
        channelIdx++;
        if (channelIdx > 2) {
            channelIdx = 0;
        }
        app_log_info("Set Ch%d\r\n", channelIdx);
    }
}

void quad0ClockWise(void* ctx) {
    (void)ctx;
    currPercent[channelIdx] += 5;
    if (currPercent[channelIdx] > 100) {
        currPercent[channelIdx] = 100;
    }
    app_log_info("Set Ch%d PWM to %d\r\n", channelIdx, currPercent[channelIdx]);
    setDutyCycle(currChannel[channelIdx], currPercent[channelIdx]);
}

void quad0CounterClockWise(void* ctx) {
    (void)ctx;
    currPercent[channelIdx] -= 5;
    if (currPercent[channelIdx] < 0) {
        currPercent[channelIdx] = 0;
    }
    app_log_info("Set Ch%d PWM to %d\r\n", channelIdx, currPercent[channelIdx]);
    setDutyCycle(currChannel[channelIdx], currPercent[channelIdx]);
}
