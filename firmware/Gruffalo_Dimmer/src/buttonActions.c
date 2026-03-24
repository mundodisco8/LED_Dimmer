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
#include "LEDIndicator.h"
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
STATIC const uint32_t BRIGHTNESS_DELTA = 500UL;  // in percent with two decimals -> 5%

/**
 * @brief A silly way to keep track of which channel is selected via push-buttons
 */
static const LEDChannel_t LEDChannelArray[PWM_CHANNELS + 1] = {LED_CHANNEL_1, LED_CHANNEL_2, LED_CHANNEL_3,
                                                               LED_CHANNELS_ALL};
STATIC uint8_t channelIdx = 0UL;  // currently selected channel, used to know where to apply changes

// TODO: Maybe we can store errors in some variables, and then check for interrutp
// errors in the main loop?

/**
 * @brief callback to call on pushbutton interrup
 *
 * @param intNo the number of the interrupt, used to get the button that triggered the interrupt
 * @param ctx a button_t pointer to the button that triggered the interrupt
 */
void gpioCallbackButton(uint8_t intNo, void* ctx) {
    (void)intNo;
    // TODO: could get rid of this timer if I count ticks of the TIMER_SAMPLE instead
    // set debounce timer
    startButtonTimer((button_t*)ctx, TIMER_LONGPRESS);
    // set sampling timer
    startButtonTimer((button_t*)ctx, TIMER_SAMPLE);
}

/**
 * @brief callback to call on quad encoder interrup
 *
 * @param intNo the number of the interrupt, used to get the button that triggered the interrupt
 * @param ctx a button_t pointer to the quad encoder that triggered the interrupt
 */
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

/**
 * @brief Action triggered on a short press of button 0
 *
 * @param ctx a button_t pointer to the button that triggered the interrupt
 */
void button0ShortPressed(void* ctx) {
    (void)ctx;
    // app_log_debug("Btn0 Pressed\r\n");
}

/**
 * @brief Action triggered on a long press of button 0
 *
 * @param ctx a button_t pointer to the button that triggered the interrupt
 */
void button0LongPressed(void* ctx) {
    (void)ctx;
    // app_log_debug("Btn0 LONG Pressed\r\n");
}

/**
 * @brief Action triggered on release of of button 0
 *
 * @param ctx a button_t pointer to the button that triggered the interrupt
 */
void button0Released(void* ctx) {
    // app_log_debug("Btn0 Released\r\n");
    // Cast the context as a button_t pointer
    const button_t* btnPtr = (button_t*)ctx;

    if (BUTTON_LONGPRESSED == (btnPtr->prevState)) {
        // Long Press
        // app_log_debug("Released 0 after LONG press\r\n");
    } else {
        // Short Press
        channelIdx++;
        if (channelIdx >= (sizeof(LEDChannelArray) / (sizeof(LEDChannelArray[0])))) {
            channelIdx = 0;
        }
        if (LEDChannelArray[channelIdx] != LED_CHANNELS_ALL) {
            app_log_info("Selected Ch%d\r\n", channelIdx);
        } else {
            app_log_info("Selected all channels\r\n");
        }
        // turn the correct channel select indicator LEDs
        if (LED_CHANNEL_1 == LEDChannelArray[channelIdx]) {
            LEDIndicator_setChannel(CHANNEL_1);
        } else if (LED_CHANNEL_2 == LEDChannelArray[channelIdx]) {
            LEDIndicator_setChannel(CHANNEL_2);
        } else if (LED_CHANNEL_3 == LEDChannelArray[channelIdx]) {
            LEDIndicator_setChannel(CHANNEL_3);
        } else {
            // Else, ALL Channels
            LEDIndicator_setChannel(CHANNEL_ALL);
        }
    }
}

/**
 * @brief Increases the brightness of an LED channel by a delta (or decreases if the delta is negative!)
 * Extracted as a function to remove code duplication, as it's used in quad0Clockwise and quad0CounterClockWise
 * @param channel the LEDChannel to increase / decrease
 * @param delta the amount to increase/decrease, in percentage as int with two decimal places (100 -> 1%)
 */
static void updateLEDBrightness(LEDChannel_t channel, int32_t delta) {
    const uint32_t currBrightness = getLEDBrightness(channel);
    if (((delta > 0) && (currBrightness < MAX_BRIGHTNESS)) || ((delta < 0) && (currBrightness > MIN_BRIGHTNESS))) {
        // Only trigger an action if brightness is less than 100% with positive deltas, or more than 0% with negative
        // deltas
        const uint32_t newBrightness = currBrightness + delta;
        // No need to check return
        uint32_t setValue = setLEDBrightness(channel, newBrightness);
        app_log_debug("Set Ch%" PRIu32 " PWM to %" PRIu32 "\r\n", (uint32_t)channel, setValue);
    }
}

/**
 * @brief Action triggered on a CW rotation of quad 0
 *
 * @param ctx a quad_t pointer to the quad encoder that triggered the interrupt
 */
void quad0ClockWise(void* ctx) {
    (void)ctx;
    if (LED_CHANNELS_ALL != LEDChannelArray[channelIdx]) {
        // Increase only this channel
        updateLEDBrightness(LEDChannelArray[channelIdx], BRIGHTNESS_DELTA);
    } else {
        // increase all channels
        updateLEDBrightness(LED_CHANNEL_1, BRIGHTNESS_DELTA);
        updateLEDBrightness(LED_CHANNEL_2, BRIGHTNESS_DELTA);
        updateLEDBrightness(LED_CHANNEL_3, BRIGHTNESS_DELTA);
    }
}

/**
 * @brief Action triggered on a CCW rotation of quad 0
 *
 * @param ctx a quad_t pointer to the quad encoder that triggered the interrupt
 */
void quad0CounterClockWise(void* ctx) {
    (void)ctx;
    if (LED_CHANNELS_ALL != LEDChannelArray[channelIdx]) {
        // Increase only this channel
        updateLEDBrightness(LEDChannelArray[channelIdx], -BRIGHTNESS_DELTA);
    } else {
        // increase all channels
        updateLEDBrightness(LED_CHANNEL_1, -BRIGHTNESS_DELTA);
        updateLEDBrightness(LED_CHANNEL_2, -BRIGHTNESS_DELTA);
        updateLEDBrightness(LED_CHANNEL_3, -BRIGHTNESS_DELTA);
    }
}

/**
 * @brief Action triggered on a short press of button 1
 *
 * @param ctx a button_t pointer to the button that triggered the interrupt
 */
void button1ShortPressed(void* ctx) {
    (void)ctx;
    // app_log_debug("Btn1 Pressed\r\n");
}

/**
 * @brief Action triggered on a long press of button 1
 *
 * @param ctx a button_t pointer to the button that triggered the interrupt
 */
void button1LongPressed(void* ctx) {
    (void)ctx;
    // app_log_debug("Btn1 LONG Pressed\r\n");
}

/**
 * @brief Action triggered on release of of button 1
 *
 * @param ctx a button_t pointer to the button that triggered the interrupt
 */
void button1Released(void* ctx) {
    // app_log_debug("Btn1 Released\r\n");
    // Cast the context as a button_t pointer
    const button_t* btnPtr = (button_t*)ctx;
    if (BUTTON_LONGPRESSED == (btnPtr->prevState)) {
        // Long Press
        // app_log_debug("Released 1 after LONG press\r\n");
    } else {
        // Short Press
        anim_t currAnimation = ANIM_FIXED;
        if (LED_CHANNELS_ALL != LEDChannelArray[channelIdx]) {
            // If not ALL channels selected, get the animation of the channel
            currAnimation = getAnimation(LEDChannelArray[channelIdx]);
        } else {
            // If all channels selected, get the animation of LED Channel 1
            currAnimation = getAnimation(LEDChannelArray[0]);
        }

        // "increase" the current animation
        anim_t nextAnimation = (anim_t)((uint32_t)currAnimation + 1);
        if (nextAnimation == ANIM_MAX_EFFECTS) {
            // reset on overflow
            nextAnimation = ANIM_FIXED;
        }
        if (LED_CHANNELS_ALL != LEDChannelArray[channelIdx]) {
            // If single channel selected, set the animation of the channel
            setAnimation(LEDChannelArray[channelIdx], nextAnimation);
            app_log_info("Ch %d set to Mode %s\r\n", channelIdx, getEffectName(nextAnimation));
        } else {
            // If all channels selected, set the animation for all channels
            setAnimation(LED_CHANNEL_1, nextAnimation);
            setAnimation(LED_CHANNEL_2, nextAnimation);
            setAnimation(LED_CHANNEL_3, nextAnimation);
            app_log_info("All channels set to Mode %s\r\n", getEffectName(nextAnimation));
        }
    }
}

/**
 * @brief Increases the breathe effect period of an LED channel by a delta (or decreases if the delta is negative!)
 * Extracted as a function to remove code duplication, as it's used in quad1Clockwise and quad1CounterClockWise
 * @param channel the LEDChannel to increase / decrease
 * @param delta the amount to increase/decrease, in ms.
 */
static void updateBreathePeriod(LEDChannel_t channel, int32_t delta) {
    uint32_t currentPeriod = getBreathePeriod(channel);
    uint32_t newPeriod = currentPeriod + delta;
    if (newPeriod < BREATHE_MIN_PERIOD_MS) {
        newPeriod = BREATHE_MIN_PERIOD_MS;
    }
    setBreathePeriod(channel, newPeriod);
    app_log_debug("Set Ch%" PRIu32 " Breathe Period to %" PRIu32 "ms\r\n", (uint32_t)channel, newPeriod);
}

/**
 * @brief Action triggered on a CW rotation of quad 1
 *
 * @param ctx a quad_t pointer to the quad encoder that triggered the interrupt
 */
void quad1ClockWise(void* ctx) {
    (void)ctx;
    // The breathe period increases / decreases by sizeOfTheBreatheLUT * PWMPeriod, so each tick changes the number of
    // times each sample is played by one
    const uint32_t BREATHE_PERIOD_MS_DELTA = BREATHE_LUT_SIZE * 1000UL / PWM_FREQUENCY;  // can't be a global :S

    if (LED_CHANNELS_ALL != LEDChannelArray[channelIdx]) {
        // Increase only this channel
        updateBreathePeriod(LEDChannelArray[channelIdx], BREATHE_PERIOD_MS_DELTA);
    } else {
        // increase all channels
        updateBreathePeriod(LED_CHANNEL_1, BREATHE_PERIOD_MS_DELTA);
        updateBreathePeriod(LED_CHANNEL_2, BREATHE_PERIOD_MS_DELTA);
        updateBreathePeriod(LED_CHANNEL_3, BREATHE_PERIOD_MS_DELTA);
    }
}

/**
 * @brief Action triggered on a CCW rotation of quad 1
 *
 * @param ctx a quad_t pointer to the quad encoder that triggered the interrupt
 */
void quad1CounterClockWise(void* ctx) {
    (void)
        ctx;  // The breathe period increases / decreases by sizeOfTheBreatheLUT * PWMPeriod, so each tick changes the number of
    // times each sample is played by one
    const uint32_t BREATHE_PERIOD_MS_DELTA = -(BREATHE_LUT_SIZE * 1000UL / PWM_FREQUENCY);  // can't be a global :S
    if (LED_CHANNELS_ALL != LEDChannelArray[channelIdx]) {
        // Increase only this channel
        updateBreathePeriod(LEDChannelArray[channelIdx], BREATHE_PERIOD_MS_DELTA);
    } else {
        // increase all channels
        updateBreathePeriod(LED_CHANNEL_1, BREATHE_PERIOD_MS_DELTA);
        updateBreathePeriod(LED_CHANNEL_2, BREATHE_PERIOD_MS_DELTA);
        updateBreathePeriod(LED_CHANNEL_3, BREATHE_PERIOD_MS_DELTA);
    }
}