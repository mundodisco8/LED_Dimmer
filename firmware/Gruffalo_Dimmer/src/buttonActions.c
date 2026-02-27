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
#include "effectControl.h"
#include "gpio_HW.h"
#include "sleepyTimers_HW.h"

// Trickery to allow testing of static elements. Better to mess a bit with the code than to overcomplicate tests
#ifdef TEST
#define STATIC
#else
#define STATIC static
#endif

////
// Defines and Consts
////

/**
 * @brief Increase in brightness triggered by button actions
 */
static const uint32_t BRIGHTNESS_DELTA = 500UL;  // in percent with two decimals -> 5%

static const uint32_t LONG_PRESS_DELTA =
    1000UL;  // Time delta in ms from button press to button release to consider it a long press

/**
 * @brief A silly way to keep track of which channel is selected via push-buttons
 */
static const LEDChannel_t currChannel[PWM_CHANNELS] = {LED_CHANNEL_1, LED_CHANNEL_2, LED_CHANNEL_3};
STATIC uint8_t channelIdx = 0UL;  // currently selected channel, used to know where to apply changes

// TODO: Maybe we can store errors in some variables, and then check for interrutp
// errors in the main loop?

void gpioCallbackButton(uint8_t intNo, void* ctx) {
    (void)intNo;
    // TODO: could get rid of this timer if I count ticks of the TIMER_SAMPLE instead
    // set debounce timer
    startButtonTimer((button_t*)ctx, TIMER_LONGPRESS);
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

void button0Pressed(void* ctx) {
    (void)ctx;
    // app_log_debug("Btn0 Pressed\r\n");
}
bool sleepFlag = false;
void button0Released(void* ctx) {
    // app_log_debug("Btn0 Released\r\n");
    // Cast the context as a button_t pointer
    button_t* btnPtr = (button_t*)ctx;
    uint64_t currTime = SLP_getSystemTickInMs();
    if ((currTime - btnPtr->lastPressMs) > LONG_PRESS_DELTA) {
        // Long Press
        app_log_debug("IT WAS A LONGPRESS\r\n");
        sleepFlag = true;
    } else {
        // Short Press
        channelIdx++;
        if (channelIdx >= (uint8_t)CC_MAX_CHANNELS) {
            channelIdx = 0;
        }
        app_log_info("Set Ch%d\r\n", channelIdx);
    }
}

void quad0ClockWise(void* ctx) {
    (void)ctx;
    const uint32_t currBrightness = getLEDBrightness(currChannel[channelIdx]);
    if (currBrightness < MAX_BRIGHTNESS) {
        // Only trigger an action if brightness is less than 100%
        const uint32_t newBrightness = currBrightness + BRIGHTNESS_DELTA;
        // No need to check return
        uint32_t setValue = setLEDBrightness(channelIdx, newBrightness);
        app_log_debug("Set Ch%d PWM to %" PRIu32 "\r\n", channelIdx, setValue);
    }
}

void quad0CounterClockWise(void* ctx) {
    (void)ctx;

    const uint32_t currBrightness = getLEDBrightness(currChannel[channelIdx]);
    if (currBrightness > MIN_BRIGHTNESS) {
        const uint32_t newBrightness = currBrightness - BRIGHTNESS_DELTA;
        // No need to check return
        uint32_t setValue = setLEDBrightness(channelIdx, newBrightness);

        app_log_debug("Set Ch%d PWM to %" PRIu32 "\r\n", channelIdx, setValue);
    }
}

void button1Pressed(void* ctx) {
    (void)ctx;
    // app_log_debug("Btn1 Pressed\r\n");
}

void button1Released(void* ctx) {
    // app_log_debug("Btn1 Released\r\n");
    // Cast the context as a button_t pointer
    button_t* btnPtr = (button_t*)ctx;
    uint64_t currTime = SLP_getSystemTickInMs();
    if ((currTime - btnPtr->lastPressMs) > LONG_PRESS_DELTA) {
        // Long Press
        app_log_debug("IT WAS A LONGPRESS\r\n");
    } else {
        // Short Press
        LED_t* LED_ptr = getLEDStruct(currChannel[channelIdx]);
        // "increase" the current animation
        LED_ptr->currAnimation++;
        if (LED_ptr->currAnimation == ANIM_MAX_EFFECTS) {
            // reset on overflow
            LED_ptr->currAnimation = ANIM_FIXED;
        }
        app_log_info("Ch %d set to Mode %s\r\n", channelIdx, getEffectName(LED_ptr->currAnimation));
    }
}

void quad1ClockWise(void* ctx) {
    (void)ctx;
    // The breathe period increases / decreases by sizeOfTheBreatheLUT * PWMPeriod, so each tick changes the number of
    // times each sample is played by one
    const uint32_t BREATHE_PERIOD_MS_DELTA = BREATHE_LUT_SIZE * 1000UL / PWM_FREQUENCY;  // can't be a global :S

    uint32_t currentPeriod = getBreathePeriod(currChannel[channelIdx]);
    setBreathePeriod(currChannel[channelIdx], currentPeriod + BREATHE_PERIOD_MS_DELTA);
    app_log_debug("Set Ch%d Breathe Period to %" PRIu32 "ms\r\n", channelIdx, currentPeriod + BREATHE_PERIOD_MS_DELTA);
}

void quad1CounterClockWise(void* ctx) {
    (void)ctx;
    // The breathe period increases / decreases by sizeOfTheBreatheLUT * PWMPeriod, so each tick changes the number of
    // times each sample is played by one
    const uint32_t BREATHE_PERIOD_MS_DELTA = BREATHE_LUT_SIZE * 1000UL / PWM_FREQUENCY;  // can't be a global :S

    uint32_t currentPeriod = getBreathePeriod(currChannel[channelIdx]);
    uint32_t newPeriod = currentPeriod - BREATHE_PERIOD_MS_DELTA;
    if (newPeriod < BREATHE_MIN_PERIOD_MS) {
        newPeriod = BREATHE_MIN_PERIOD_MS;
    }
    setBreathePeriod(currChannel[channelIdx], newPeriod);
    app_log_debug("Set Ch%d Breathe Period to %" PRIu32 "ms\r\n", channelIdx, newPeriod);
}