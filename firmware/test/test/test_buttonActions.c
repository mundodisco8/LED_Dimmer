#include <unity.h>

// Module being tested
#include "buttonActions.h"

// Mocks and others
#include "app_log.h"
#include "mock_PWMControl.h"
#include "mock_buttons.h"
#include "mock_debounce.h"
#include "mock_effectControl.h"
#include "mock_gpio_HW.h"
#include "mock_sleepyTimers_HW.h"

/*
 * Unit tests for "buttoyActions.h"
 */

// externs to test some static variables in the module
extern uint8_t channelIdx;

// Provide values for some globals whose definitions are lost when using mocks
const uint32_t PWM_FREQUENCY = 1000UL;
const uint32_t BREATHE_MIN_PERIOD_MS = BREATHE_LUT_SIZE;

void setUp(void) {
    // reset globals
    channelIdx = 0;
}

void tearDown(void) {}

// gpioCallbackButton
// The press of a button starts the debounce and sample timers
void test_gpioCallBackButton(void) {
    button_t testButton = {0};
    uint8_t testIntNo = 4;
    // Set Expectations
    startButtonTimer_ExpectAndReturn(&testButton, TIMER_LONGPRESS, BTN_OK);
    startButtonTimer_ExpectAndReturn(&testButton, TIMER_SAMPLE, BTN_OK);

    gpioCallbackButton(testIntNo, &testButton);
}

// gpioCallbackQuad
// Quad input is simpler, as it has HW debouncing. we just check if we are rotating CW or CCW
// The quad encoder object is passed as context
// We will need two dummy callback functions to test them
uint32_t quadCallbackTest = 0;
void dummy_CWAction(void* ctx) {
    (void)ctx;
    quadCallbackTest = 1;
}

void dummy_CCWAction(void* ctx) {
    (void)ctx;
    quadCallbackTest = 2;
}

void test_gpioCallBackQuad(void) {
    uint32_t testIntNo = 9;
    pinPort_t testPort = portA;
    uint8_t testPinNo = 4;
    quad_encoder_t testEncoder = {.pin1No = testPinNo,
                                  .pin1Port = testPort,
                                  .clockWiseAction = dummy_CWAction,
                                  .counterClockWiseAction = dummy_CCWAction};

    // Set Expectations - CW rotation
    readPin_ExpectAndReturn(testEncoder.pin1Port, testEncoder.pin1No, 1);

    gpioCallbackQuad(testIntNo, &testEncoder);
    TEST_ASSERT_EQUAL(1, quadCallbackTest);

    // Test 2 - CCW rotation - Set Expectations
    readPin_ExpectAndReturn(testEncoder.pin1Port, testEncoder.pin1No, 0);

    gpioCallbackQuad(testIntNo, &testEncoder);
    TEST_ASSERT_EQUAL(2, quadCallbackTest);
}

// button0 changes the current PWM channel active. Third press resets channel to 0
void test_button0ReleasedAfterShortPress(void) {
    uint32_t expectedCh = 1;
    button_t testBtn = {.prevState = BUTTON_PRESSED};  // So all releases will be "short" releases

    // Set Expectations
    button0Released(&testBtn);
    TEST_ASSERT_EQUAL(expectedCh, channelIdx);

    // Second press gets channel 2
    expectedCh = 2;

    // Set Expectations
    button0Released(&testBtn);
    TEST_ASSERT_EQUAL(expectedCh, channelIdx);

    // Third press rests to 0;
    expectedCh = 0;

    // Set Expectations
    button0Released(&testBtn);
    TEST_ASSERT_EQUAL(expectedCh, channelIdx);
}

// button0 on long press. does nothing, for now
void test_button0ReleasedAfterLongPress(void) {
    button_t testBtn = {.prevState = BUTTON_LONGPRESSED};  // So the release will be a "long" release

    // No action on long release

    button0Released(&testBtn);
}

// Button 0 ShortPressed does nothing for now
void test_button0ShortPressed(void) { button0ShortPressed(NULL); }

// Button 0 LongPressed does nothing for now
void test_button0LongPressed(void) { button0LongPressed(NULL); }

// Test Quad actions
// Rotating CW increases channel's % by 5, and CCW decreases it

void test_quad0ClockWise(void) {
    uint32_t expectedInitialBrightness = 1000;
    uint32_t expectedFinalBrightness = 1500;
    LEDChannel_t expectedChannel = LED_CHANNEL_1;

    // Set Expectations
    getLEDBrightness_ExpectAndReturn(expectedChannel, expectedInitialBrightness);
    setLEDBrightness_ExpectAndReturn(expectedChannel, expectedFinalBrightness, EFF_OK);
    quad0ClockWise(NULL);

    // Turn twice more...
    expectedFinalBrightness = 2500;

    // Set Expectations
    getLEDBrightness_ExpectAndReturn(expectedChannel, 1500);
    setLEDBrightness_ExpectAndReturn(expectedChannel, 2000, EFF_OK);
    getLEDBrightness_ExpectAndReturn(expectedChannel, 2000);
    setLEDBrightness_ExpectAndReturn(expectedChannel, expectedFinalBrightness, EFF_OK);
    quad0ClockWise(NULL);
    quad0ClockWise(NULL);
}

void test_quad0ClockWise_MaxBrightness(void) {
    uint32_t expectedInitialBrightness = MAX_BRIGHTNESS;
    LEDChannel_t expectedChannel = LED_CHANNEL_1;

    // Set Expectations
    getLEDBrightness_ExpectAndReturn(expectedChannel, expectedInitialBrightness);

    quad0ClockWise(NULL);
}

void test_quad0CounterClockWise(void) {
    uint32_t expectedInitialBrightness = 9000;
    uint32_t expectedFinalBrightness = 8500;
    LEDChannel_t expectedChannel = LED_CHANNEL_1;

    // Set Expectations
    getLEDBrightness_ExpectAndReturn(expectedChannel, expectedInitialBrightness);
    setLEDBrightness_ExpectAndReturn(expectedChannel, expectedFinalBrightness, EFF_OK);
    quad0CounterClockWise(NULL);

    // Turn twice more...
    expectedFinalBrightness = 7500;

    // Set Expectations
    getLEDBrightness_ExpectAndReturn(expectedChannel, 8500);
    setLEDBrightness_ExpectAndReturn(expectedChannel, 8000, EFF_OK);
    getLEDBrightness_ExpectAndReturn(expectedChannel, 8000);
    setLEDBrightness_ExpectAndReturn(expectedChannel, expectedFinalBrightness, EFF_OK);
    quad0CounterClockWise(NULL);
    quad0CounterClockWise(NULL);
}

void test_quad0CounterClockWise_MinBrightness(void) {
    uint32_t expectedInitialBrightness = MIN_BRIGHTNESS;
    LEDChannel_t expectedChannel = LED_CHANNEL_1;

    // Set Expectations
    getLEDBrightness_ExpectAndReturn(expectedChannel, expectedInitialBrightness);

    quad0CounterClockWise(NULL);
}

/**
 * button1Released
 * - Shortpress changes from fixed to breathe
 * - Shortpress changes from breathe to fixed
 */

void test_button1Released_FixedToBreathe(void) {
    // Shortpress, current lastPressMs is 0
    button_t testBtn = {
        .prevState = BUTTON_PRESSED  // so release is "short" release
    };
    LEDChannel_t testChannel = LED_CHANNEL_1;
    anim_t testCurrAnimation = ANIM_FIXED;
    anim_t expectedNewAnimation = ANIM_BREATHE;

    // Set Expectations
    getAnimation_ExpectAndReturn(testChannel, testCurrAnimation);
    setAnimation_ExpectAndReturn(testChannel, expectedNewAnimation, EFF_OK);
    getEffectName_ExpectAndReturn(expectedNewAnimation, "Breathe Effect");

    button1Released(&testBtn);
}

void test_button1Released_BreatheToFixed(void) {
    // Shortpress, current lastPressMs is 0
    button_t testBtn = {
        .prevState = BUTTON_PRESSED  // so release is "short" release

    };
    LEDChannel_t testChannel = LED_CHANNEL_1;
    anim_t testCurrAnimation = ANIM_BREATHE;
    anim_t expectedNewAnimation = ANIM_FIXED;

    // Set Expectations
    getAnimation_ExpectAndReturn(testChannel, testCurrAnimation);
    setAnimation_ExpectAndReturn(testChannel, expectedNewAnimation, EFF_OK);
    getEffectName_ExpectAndReturn(expectedNewAnimation, "Fixed Brightness");

    button1Released(&testBtn);
}

// button0 on long press. does nothing, for now
void test_button1Released_LongPress(void) {
    button_t testBtn = {
        .prevState = BUTTON_LONGPRESSED  // so release is "short" release
    };

    // No action on long release

    button1Released(&testBtn);
}

// Button 1 Pressed does nothing for now
void test_button1Pressed(void) { button1ShortPressed(NULL); }

// Button 1 LongPressed does nothing for now
void test_button1LongPressed(void) { button1LongPressed(NULL); }

/**
 * quad1Clockwise
 *
 */

void test_quad1Clockwise_IncreaseBreathePeriod(void) {
    uint32_t currentPeriod = 1000;
    LEDChannel_t expectedChannel = LED_CHANNEL_1;

    uint32_t expectedPeriod = currentPeriod + (BREATHE_LUT_SIZE * 1000UL / PWM_FREQUENCY);

    // Set expecations
    getBreathePeriod_ExpectAndReturn(expectedChannel, currentPeriod);
    setBreathePeriod_ExpectAndReturn(expectedChannel, expectedPeriod, EFF_OK);

    quad1ClockWise(NULL);
}

/**
 * quad1CounterClockwise
 *
 */

void test_quad1CounterClockwise_DecreaseBreathePeriod(void) {
    uint32_t currentPeriod = 1000;
    LEDChannel_t expectedChannel = LED_CHANNEL_1;

    uint32_t expectedPeriod = currentPeriod - (BREATHE_LUT_SIZE * 1000UL / PWM_FREQUENCY);

    // Set expecations
    getBreathePeriod_ExpectAndReturn(expectedChannel, currentPeriod);
    setBreathePeriod_ExpectAndReturn(expectedChannel, expectedPeriod, EFF_OK);

    quad1CounterClockWise(NULL);
}

void test_quad1CounterClockwise_DontGoBelowMinimum(void) {
    uint32_t currentPeriod = BREATHE_MIN_PERIOD_MS;
    LEDChannel_t expectedChannel = LED_CHANNEL_1;

    uint32_t expectedPeriod = BREATHE_MIN_PERIOD_MS;

    // Set expecations
    getBreathePeriod_ExpectAndReturn(expectedChannel, currentPeriod);
    setBreathePeriod_ExpectAndReturn(expectedChannel, expectedPeriod, EFF_OK);

    quad1CounterClockWise(NULL);
}