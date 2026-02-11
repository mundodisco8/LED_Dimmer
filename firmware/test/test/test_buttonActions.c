#include <unity.h>

#include "buttonActions.h"
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
    startButtonTimer_ExpectAndReturn(&testButton, TIMER_DEBOUNCE, SLPTIMER_OK);
    startButtonTimer_ExpectAndReturn(&testButton, TIMER_SAMPLE, SLPTIMER_OK);

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
void test_button0Released_ShortPress(void) {
    uint32_t expectedCh = 1;
    button_t testBtn = {.lastPressMs = 1000};
    uint64_t timeOfRelease = 1100;  // mimicks a short press

    // Set Expectations
    SLP_getSystemTickInMs_ExpectAndReturn(timeOfRelease);
    button0Released(&testBtn);
    TEST_ASSERT_EQUAL(expectedCh, channelIdx);

    // Second press gets channel 2
    expectedCh = 2;

    // Set Expectations
    SLP_getSystemTickInMs_ExpectAndReturn(timeOfRelease);
    button0Released(&testBtn);
    TEST_ASSERT_EQUAL(expectedCh, channelIdx);

    // Third press rests to 0;
    expectedCh = 0;

    // Set Expectations
    SLP_getSystemTickInMs_ExpectAndReturn(timeOfRelease);
    button0Released(&testBtn);
    TEST_ASSERT_EQUAL(expectedCh, channelIdx);
}

// button0 on long press. does nothing, for now
void test_button0Released_LongPress(void) {
    button_t testBtn = {.lastPressMs = 1000};
    uint64_t timeOfRelease = 3000;  // mimicks a short press

    // Set Expectations
    SLP_getSystemTickInMs_ExpectAndReturn(timeOfRelease);
    button0Released(&testBtn);
}

// Button 0 Pressed does nothing for now
void test_button0Pressed(void) { button0Pressed(NULL); }

// Test Quad actions
// Rotating CW increases channel's % by 5, and CCW decreases it

void test_quad0ClockWise(void) {
    uint32_t expectedInitialBrightness = 1000;
    uint32_t expectedFinalBrightness = 1500;
    CCChannel_t expectedChannel = LED_CHANNEL_1;

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
    CCChannel_t expectedChannel = LED_CHANNEL_1;

    // Set Expectations
    getLEDBrightness_ExpectAndReturn(expectedChannel, expectedInitialBrightness);

    quad0ClockWise(NULL);
}

void test_quad0CounterClockWise(void) {
    uint32_t expectedInitialBrightness = 9000;
    uint32_t expectedFinalBrightness = 8500;
    CCChannel_t expectedChannel = LED_CHANNEL_1;

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
    CCChannel_t expectedChannel = LED_CHANNEL_1;

    // Set Expectations
    getLEDBrightness_ExpectAndReturn(expectedChannel, expectedInitialBrightness);

    quad0CounterClockWise(NULL);
}
