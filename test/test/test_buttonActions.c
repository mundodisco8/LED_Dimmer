#include "buttonActions.h"

#include <unity.h>

#include "mock_buttons.h"
#include "mock_PWMControl.h"
#include "mock_debounce.h"
#include "mock_gpio_HW.h"
#include "mock_sleepyTimers_HW.h"

/*
 * Unit tests for "buttoyActions.h"
 */

// externs to test some static variables in the module
extern int8_t currPercent[3];
extern uint8_t channelIdx;

void setUp(void) {
    // reset globals
    channelIdx = 0;
    currPercent[0] = 0;
    currPercent[1] = 0;
    currPercent[2] = 0;
}

void tearDown(void) {
}

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
void dummy_CWAction(void) {
    quadCallbackTest = 1;
}

void dummy_CCWAction(void) {
    quadCallbackTest = 2;
}

void test_gpioCallBackQuad(void) {
    uint32_t testIntNo = 9;
    pinPort_t testPort = portA;
    uint8_t testPinNo = 4;
    quad_encoder_t testEncoder = {.pin1No = testPinNo, .pin1Port = testPort, .clockWiseAction = dummy_CWAction, .counterClockWiseAction = dummy_CCWAction};

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
void test_button0Pressed(void) {
    uint32_t expectedCh = 1;

    button0Pressed();
    TEST_ASSERT_EQUAL(expectedCh, channelIdx);

    // Second press gets channel 2
    expectedCh = 2;

    button0Pressed();
    TEST_ASSERT_EQUAL(expectedCh, channelIdx);

    // Third press rests to 0;
    expectedCh = 0;

    button0Pressed();
    TEST_ASSERT_EQUAL(expectedCh, channelIdx);
}

// Button 0 Released does nothing for now
void test_button0Released(void) {
    button0Released();
}

// Test Quad actions
// Rotating CW increases channel's % by 5, and CCW decreases it
// Check that it doesn't go over 100 or below 0
void test_quad0ClockWise(void) {
    uint32_t expectedPercent = 5;
    CCChannel_t expectedChannel = CC_CHANNEL_0;

    // Set Expectations
    setDutyCycle_Expect(expectedChannel, expectedPercent);
    quad0ClockWise();
    TEST_ASSERT_EQUAL(expectedPercent, currPercent[channelIdx]);

    // Turn twice more...
    expectedPercent = 15;

    // Set Expectations
    setDutyCycle_Expect(expectedChannel, 10);
    setDutyCycle_Expect(expectedChannel, expectedPercent);
    quad0ClockWise();
    quad0ClockWise();
    TEST_ASSERT_EQUAL(expectedPercent, currPercent[channelIdx]);

    // Test the limit
    currPercent[channelIdx] = 97;
    expectedPercent = 100;
    setDutyCycle_Expect(expectedChannel, expectedPercent);

    quad0ClockWise();
    TEST_ASSERT_EQUAL(expectedPercent, currPercent[channelIdx]);
}

void test_quad0CounterClockWise(void) {
     currPercent[channelIdx] = 83;
    uint32_t expectedPercent = 78;
    CCChannel_t expectedChannel = CC_CHANNEL_0;

    // Set Expectations
    setDutyCycle_Expect(expectedChannel, expectedPercent);
    quad0CounterClockWise();
    TEST_ASSERT_EQUAL(expectedPercent, currPercent[channelIdx]);

    // Turn twice more...
    expectedPercent = 68;

    // Set Expectations
    setDutyCycle_Expect(expectedChannel, 73);
    setDutyCycle_Expect(expectedChannel, expectedPercent);
    quad0CounterClockWise();
    quad0CounterClockWise();
    TEST_ASSERT_EQUAL(expectedPercent, currPercent[channelIdx]);

    // Test the limit
    currPercent[channelIdx] = 3;
    expectedPercent = 0;
    setDutyCycle_Expect(expectedChannel, expectedPercent);

    quad0CounterClockWise();
    TEST_ASSERT_EQUAL(expectedPercent, currPercent[channelIdx]);
}