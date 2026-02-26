#include <unity.h>

#include "debounce.h"
#include "mock_buttons.h"
#include "mock_gpio_HW.h"
#include "mock_sleepyTimers_HW.h"

/*
 * Unit tests for "debounce.h
"
 */

// STATIC'ed objects that need now to be extern'ed
extern const uint32_t DEBOUNCE_TIME_MS;
extern const uint32_t DEBOUNCE_SAMPLING_PERIOD_MS;
extern const int32_t INTEGRATOR_TARGET;
extern void sleeptimerSamplingCallback(timerHandlePtr_t* handle, void* data);

// Some empty action callbacks for the buttons
void fakePressAction(void* ctx) { (void)ctx; }
void fakeReleaseAction(void* ctx) { (void)ctx; }

// Some timer handlers for the buttons
timerHandlePtr_t debounceTimerPtr;
timerHandlePtr_t samplingTimerPtr;

void setUp(void) {}

void tearDown(void) {}

////
// samplingTimerCallback
////

// List of Tests for samplingTimerCallback
// * Integrator increases
// * Integrator decreases
// * Integrator doens't decrease below 0
// * Integrator doesn't increase above INTEGRATOR_TARGET
// * Integrator increases when called multiple times while pin is HIGH
// * Integrator decreases when called multiple times while pin is LOW
// * When a released button is pressed for long enough, button state changes to pressed
// * When a pressed button is released for long enough, button state changes to released
// * Simulate a noisy press in a released button and a noisy release in a pressed button.
// * Corruption guards work
// * State is only changed if the integrator converges and the button is in the right state

// We will call startButtonTimer for timerSample lots of times! Packing its expects in a function
void startButtonTimerForSampleTimer_Expectations(button_t* buttonPtr) {
    SLP_isTimerRunning_ExpectAndReturn(buttonPtr->samplingTimerPtr, false);
    SLP_startTimer_ExpectAndReturn(buttonPtr->samplingTimerPtr, DEBOUNCE_SAMPLING_PERIOD_MS, TIMER_ONE_SHOT,
                                   sleeptimerSamplingCallback, buttonPtr, SLPTIMER_OK);
}

void test_IntegratorIncreases(void) {
    pinPort_t testPort = portA;
    uint8_t testPin = 1U;
    button_state_t testState = BUTTON_RELEASED;
    int32_t testIntegrator = 0UL;
    uint32_t testCycles = 0UL;

    button_t testBtn = {.btnPort = testPort,
                        .pinNo = testPin,
                        .state = testState,
                        .integrator = testIntegrator,
                        .debounceCycles = testCycles};

    // Expectations
    uint32_t expectedDebounceCyles = testCycles + 1UL;
    int32_t expectedIntegratorValue = 1UL;
    uint32_t expectedPinState = 1UL;
    readPin_ExpectAndReturn(testPort, testPin, expectedPinState);
    startButtonTimerForSampleTimer_Expectations(&testBtn);

    samplingTimerCallback(&testBtn);
    TEST_ASSERT_EQUAL_INT32(expectedIntegratorValue, testBtn.integrator);
    TEST_ASSERT_EQUAL_UINT32(expectedDebounceCyles, testBtn.debounceCycles);
}

void test_IntegratorDecreases(void) {
    pinPort_t testPort = portA;
    uint8_t testPin = 1U;
    button_state_t testState = BUTTON_RELEASED;
    int32_t testIntegrator = 2UL;
    uint32_t testCycles = 0UL;

    button_t testBtn = {.btnPort = testPort,
                        .pinNo = testPin,
                        .state = testState,
                        .integrator = testIntegrator,
                        .debounceCycles = testCycles};

    // Expectations
    uint32_t expectedDebounceCyles = testCycles + 1UL;
    int32_t expectedIntegratorValue = 1UL;
    uint32_t expectedPinState = 0UL;
    readPin_ExpectAndReturn(testPort, testPin, expectedPinState);
    startButtonTimerForSampleTimer_Expectations(&testBtn);

    samplingTimerCallback(&testBtn);
    TEST_ASSERT_EQUAL_INT32(expectedIntegratorValue, testBtn.integrator);
    TEST_ASSERT_EQUAL_UINT32(expectedDebounceCyles, testBtn.debounceCycles);
}

void test_IntegratorNotBelow0(void) {
    pinPort_t testPort = portA;
    uint8_t testPin = 1U;
    button_state_t testState = BUTTON_RELEASED;  // as to not trigger the button state change part of the code
    int32_t testIntegrator = 0UL;
    uint32_t testCycles = 0UL;

    button_t testBtn = {.btnPort = testPort,
                        .pinNo = testPin,
                        .state = testState,
                        .integrator = testIntegrator,
                        .debounceCycles = testCycles};

    // Expectations
    uint32_t expectedDebounceCyles = testCycles + 1UL;
    int32_t expectedIntegratorValue = 0UL;
    uint32_t expectedPinState = 0UL;
    readPin_ExpectAndReturn(testPort, testPin, expectedPinState);
    startButtonTimerForSampleTimer_Expectations(&testBtn);

    samplingTimerCallback(&testBtn);
    TEST_ASSERT_EQUAL_INT32(expectedIntegratorValue, testBtn.integrator);
    TEST_ASSERT_EQUAL_UINT32(expectedDebounceCyles, testBtn.debounceCycles);
}

void test_IntegratorNotAboveMax(void) {
    pinPort_t testPort = portA;
    uint8_t testPin = 1U;
    button_state_t testState = BUTTON_PRESSED;  // as to not trigger the button state change part of the code
    int32_t testIntegrator = INTEGRATOR_TARGET;
    uint32_t testCycles = 0UL;

    button_t testBtn = {.btnPort = testPort,
                        .pinNo = testPin,
                        .state = testState,
                        .integrator = testIntegrator,
                        .debounceCycles = testCycles};

    // Expectations
    uint32_t expectedDebounceCyles = testCycles + 1UL;
    int32_t expectedIntegratorValue = INTEGRATOR_TARGET;
    uint32_t expectedPinState = 1UL;

    readPin_ExpectAndReturn(testPort, testPin, expectedPinState);
    startButtonTimerForSampleTimer_Expectations(&testBtn);

    samplingTimerCallback(&testBtn);
    TEST_ASSERT_EQUAL_INT32(expectedIntegratorValue, testBtn.integrator);
    TEST_ASSERT_EQUAL_UINT32(expectedDebounceCyles, testBtn.debounceCycles);
}

// In the previous test, I realised that if I leave the button actions pointing to NULL, we get an exception when
// that "action" is called. Add some code to fix it
void test_ButtonActionIsNull(void) {
    button_t testBtn = {.btnPort = portA,
                        .pinNo = 1,
                        .state = BUTTON_RELEASED,
                        .integrator = INTEGRATOR_TARGET,
                        .pressedAction = NULL,
                        .releasedAction = NULL};

    readPin_ExpectAndReturn(portA, 1, 1);
    // The system tick of the current press is registered in the button_t struct
    uint64_t timeAtPress = 1000;
    SLP_getSystemTickInMs_ExpectAndReturn(timeAtPress);
    // Expectations for stopButtonTimers. Ignore parameters as we are not testing it here.
    // Retunr false so we exit the execution faster. It doesn't change the outcome
    SLP_isTimerRunning_IgnoreAndReturn(false);
    samplingTimerCallback(&testBtn);

    // Same thing with the release action
    testBtn.integrator = 0;
    testBtn.state = BUTTON_PRESSED;
    readPin_ExpectAndReturn(portA, 1, 0);
    // Expectations for stopButtonTimers. Ignore parameters as we are not testing it here.
    // Retunr false so we exit the execution faster. It doesn't change the outcome
    SLP_isTimerRunning_IgnoreAndReturn(false);
    samplingTimerCallback(&testBtn);
    // TODO: Don't know how to pass this test. If fails, it throws an exception
    // So if the tests don't complain about bad memory, it means it worked?
}

void test_IntegratorDecreasesMultiple(void) {
    pinPort_t testPort = portA;
    uint8_t testPin = 1U;
    button_state_t testState = BUTTON_RELEASED;
    int32_t testIntegrator = 4UL;
    uint32_t testCycles = 0UL;

    button_t testBtn = {.btnPort = testPort,
                        .pinNo = testPin,
                        .state = testState,
                        .integrator = testIntegrator,
                        .debounceCycles = testCycles};

    // Expectations
    uint32_t expectedDebounceCyles = testCycles + 3UL;  // we cycle three times with the button released
    int32_t expectedIntegratorValue = testIntegrator - 3UL;
    uint32_t expectedPinState = 0UL;
    readPin_ExpectAndReturn(testPort, testPin, expectedPinState);
    startButtonTimerForSampleTimer_Expectations(&testBtn);

    samplingTimerCallback(&testBtn);

    readPin_ExpectAndReturn(testPort, testPin, expectedPinState);
    startButtonTimerForSampleTimer_Expectations(&testBtn);

    samplingTimerCallback(&testBtn);

    readPin_ExpectAndReturn(testPort, testPin, expectedPinState);
    startButtonTimerForSampleTimer_Expectations(&testBtn);

    samplingTimerCallback(&testBtn);

    TEST_ASSERT_EQUAL_INT32(expectedIntegratorValue, testBtn.integrator);
    TEST_ASSERT_EQUAL_UINT32(expectedDebounceCyles, testBtn.debounceCycles);
}

void test_IntegratorIncreasesMultiple(void) {
    pinPort_t testPort = portA;
    uint8_t testPin = 1U;
    button_state_t testState = BUTTON_RELEASED;
    int32_t testIntegrator = 1UL;
    uint32_t testCycles = 0UL;

    button_t testBtn = {.btnPort = testPort,
                        .pinNo = testPin,
                        .state = testState,
                        .integrator = testIntegrator,
                        .debounceCycles = testCycles};

    // Expectations
    uint32_t expectedDebounceCyles = testCycles + 3UL;  // we cycle three times with the button released
    int32_t expectedIntegratorValue = testIntegrator + 3UL;
    uint32_t expectedPinState = 1UL;
    readPin_ExpectAndReturn(testPort, testPin, expectedPinState);
    startButtonTimerForSampleTimer_Expectations(&testBtn);

    samplingTimerCallback(&testBtn);

    readPin_ExpectAndReturn(testPort, testPin, expectedPinState);
    startButtonTimerForSampleTimer_Expectations(&testBtn);

    samplingTimerCallback(&testBtn);

    readPin_ExpectAndReturn(testPort, testPin, expectedPinState);
    startButtonTimerForSampleTimer_Expectations(&testBtn);

    samplingTimerCallback(&testBtn);

    TEST_ASSERT_EQUAL_INT32(expectedIntegratorValue, testBtn.integrator);
    TEST_ASSERT_EQUAL_UINT32(expectedDebounceCyles, testBtn.debounceCycles);
}

void test_fromReleasedToPressed(void) {
    pinPort_t testPort = portA;
    uint8_t testPin = 1U;
    button_state_t testState = BUTTON_RELEASED;
    int32_t testIntegrator = 0UL;
    uint32_t testCycles = 0UL;

    button_t testBtn = {.btnPort = testPort,
                        .pinNo = testPin,
                        .state = testState,
                        .integrator = testIntegrator,
                        .debounceCycles = testCycles,
                        .pressedAction = fakePressAction};

    // Expectations
    uint32_t expectedDebounceCyles = 0UL;  // we reset it on button state change
    int32_t expectedIntegratorValue = INTEGRATOR_TARGET;
    uint32_t expectedPinState = 1UL;
    uint64_t timeAtPress = 1000;

    for (int32_t i = 0; i < INTEGRATOR_TARGET - 1; i++) {
        readPin_ExpectAndReturn(testPort, testPin, expectedPinState);
        startButtonTimerForSampleTimer_Expectations(&testBtn);

        // Press the button for (INTEGRATOR_TARGET -1) times
        samplingTimerCallback(&testBtn);
        // button must remain RELEASED
        TEST_ASSERT_EQUAL_UINT32(BUTTON_RELEASED, testBtn.state);
        TEST_ASSERT_EQUAL_UINT32(i + 1, testBtn.integrator);
    }
    // Press the button an additional time, the button state must switch
    readPin_ExpectAndReturn(testPort, testPin, expectedPinState);
    // If the button was pressed, record when it happened
    SLP_getSystemTickInMs_ExpectAndReturn(timeAtPress);

    samplingTimerCallback(&testBtn);

    TEST_ASSERT_EQUAL_UINT32(BUTTON_PRESSED, testBtn.state);
    TEST_ASSERT_LESS_OR_EQUAL_INT64(timeAtPress, testBtn.lastPressMs);
    TEST_ASSERT_EQUAL_UINT32(expectedIntegratorValue, testBtn.integrator);
    TEST_ASSERT_EQUAL_UINT32(expectedDebounceCyles, testBtn.debounceCycles);
}

void test_fromPressedToReleased(void) {
    pinPort_t testPort = portA;
    uint8_t testPin = 1U;
    button_state_t testState = BUTTON_PRESSED;
    int32_t testIntegrator = 5UL;
    uint32_t testCycles = 0UL;

    button_t testBtn = {.btnPort = testPort,
                        .pinNo = testPin,
                        .state = testState,
                        .integrator = testIntegrator,
                        .debounceCycles = testCycles,
                        .releasedAction = fakeReleaseAction};

    // Expectations
    uint32_t expectedDebounceCyles = 0UL;  // we reset on button state change
    int32_t expectedIntegratorValue = 0UL;
    uint32_t expectedPinState = 0UL;
    uint64_t timeAtPress = 1000;

    for (int32_t i = 0; i < INTEGRATOR_TARGET - 1; i++) {
        readPin_ExpectAndReturn(testPort, testPin, expectedPinState);
        startButtonTimerForSampleTimer_Expectations(&testBtn);

        // Press the button for (INTEGRATOR_TARGET -1) times
        samplingTimerCallback(&testBtn);
        // button must remain RELEASED
        TEST_ASSERT_EQUAL_UINT32(BUTTON_PRESSED, testBtn.state);
        TEST_ASSERT_EQUAL_UINT32(INTEGRATOR_TARGET - (i + 1), testBtn.integrator);
    }
    // Press the button an additional time, the button state must switch
    readPin_ExpectAndReturn(testPort, testPin, expectedPinState);
    // If the button was released, we DON'T record when it happened

    samplingTimerCallback(&testBtn);

    TEST_ASSERT_EQUAL_UINT32(BUTTON_RELEASED, testBtn.state);
    TEST_ASSERT_LESS_OR_EQUAL_INT64(timeAtPress, testBtn.lastPressMs);
    TEST_ASSERT_EQUAL_UINT32(expectedIntegratorValue, testBtn.integrator);
    TEST_ASSERT_EQUAL_UINT32(expectedDebounceCyles, testBtn.debounceCycles);
}

// Testing that an unstable input doesn't converge and the button state doesn't change
// Also, after the total debounce time, we stop rearming the sampling clock
void test_someNoisyInputs_ButtonReleased(void) {
    pinPort_t testPort = portA;
    uint8_t testPin = 1U;
    button_state_t testState = BUTTON_RELEASED;
    int32_t testIntegrator = 0UL;
    uint32_t testCycles = 0UL;

    button_t testBtn = {
        .btnPort = testPort,
        .pinNo = testPin,
        .state = testState,
        .integrator = testIntegrator,
        .debounceCycles = testCycles,
    };

    // Test a noisy press
    uint32_t buttonExpectedPress[] = {1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0};
    // integrator values =             1  2  3  2  1  2  3  4  3  2  3  2  3  2  3  2
    // this is used to check the expected state, based on the input
    uint32_t buttonExpectedState = BUTTON_RELEASED;

    uint32_t integrator = 0;
    for (uint32_t i = 0; i < (sizeof(buttonExpectedPress) / sizeof(buttonExpectedPress[0])) - 1; i++) {
        readPin_ExpectAndReturn(testPort, testPin, buttonExpectedPress[i]);
        buttonExpectedPress[i] == 0 ? integrator-- : integrator++;
        startButtonTimerForSampleTimer_Expectations(&testBtn);

        samplingTimerCallback(&testBtn);
        // button must remain RELEASED
        TEST_ASSERT_EQUAL_UINT32(buttonExpectedState, testBtn.state);
        TEST_ASSERT_EQUAL_UINT32(integrator, testBtn.integrator);
        TEST_ASSERT_EQUAL_UINT32(i + 1, testBtn.debounceCycles);
    }

    samplingTimerCallback(&testBtn);
    // button must remain RELEASED, and the integrator and debounce cycles must be reset too
    TEST_ASSERT_EQUAL_UINT32(buttonExpectedState, testBtn.state);
    TEST_ASSERT_EQUAL_UINT32(0UL, testBtn.debounceCycles);
    TEST_ASSERT_EQUAL_UINT32(0UL, testBtn.integrator);
}

void test_someNoisyInputs_ButtonPressed(void) {
    pinPort_t testPort = portA;
    uint8_t testPin = 1U;
    button_state_t testState = BUTTON_PRESSED;
    int32_t testIntegrator = INTEGRATOR_TARGET;
    uint32_t testCycles = 0UL;

    button_t testBtn = {
        .btnPort = testPort,
        .pinNo = testPin,
        .state = testState,
        .integrator = testIntegrator,
        .debounceCycles = testCycles,
    };

    // Test a noisy press
    uint32_t buttonExpectedPress[] = {0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0};
    // integrator values =            4  3  2  3  4  3  2  1  2  1  2  1  2  1  2  1

    // this is used to check the expected state, based on the input
    uint32_t buttonExpectedState = BUTTON_PRESSED;

    uint32_t integrator = INTEGRATOR_TARGET;
    for (uint32_t i = 0; i < (sizeof(buttonExpectedPress) / sizeof(buttonExpectedPress[0])) - 1; i++) {
        readPin_ExpectAndReturn(testPort, testPin, buttonExpectedPress[i]);
        buttonExpectedPress[i] == 0 ? integrator-- : integrator++;
        startButtonTimerForSampleTimer_Expectations(&testBtn);

        samplingTimerCallback(&testBtn);
        // button must remain PRESSED
        TEST_ASSERT_EQUAL_UINT32(buttonExpectedState, testBtn.state);
        TEST_ASSERT_EQUAL_UINT32(integrator, testBtn.integrator);
        TEST_ASSERT_EQUAL_UINT32(i + 1, testBtn.debounceCycles);
    }

    samplingTimerCallback(&testBtn);
    // button must remain RELEASED, and the integrator and debounce cycles must be reset too
    TEST_ASSERT_EQUAL_UINT32(buttonExpectedState, testBtn.state);
    TEST_ASSERT_EQUAL_UINT32(0UL, testBtn.debounceCycles);
    TEST_ASSERT_EQUAL_UINT32(INTEGRATOR_TARGET, testBtn.integrator);
}

////
// StartButtonTimer()
//
// Also tests getDebounceTime() and getDebounceSamplingPeriod() (statics called by it)
////

// 1) Start a sampling timer that is not already running
//    a) Also test that getDebounceSamplingPeriod() returns the right value
// 2) Start a sampling timer that IS already running
// 3) Checks that we hit the logging function when an error is returned by the SLP* functions

void test_startButtonTimer_SamplingTimerNotRunning(void) {
    button_t testButton = {.samplingTimerPtr = samplingTimerPtr};

    SLP_isTimerRunning_ExpectAndReturn(testButton.samplingTimerPtr, false);
    SLP_startTimer_ExpectAndReturn(testButton.samplingTimerPtr, DEBOUNCE_SAMPLING_PERIOD_MS, TIMER_ONE_SHOT, NULL,
                                   &testButton, SLPTIMER_OK);
    // Sadly, the callback is a static function, so I can't test that startPeriodicTimer will be called with it as a parameter
    SLP_startTimer_IgnoreArg_callback();

    uint32_t retVal = startButtonTimer(&testButton, TIMER_SAMPLE);
    TEST_ASSERT_EQUAL_UINT32(SLPTIMER_OK, retVal);
}

void test_startButtonTimer_SamplingTimerISRunning(void) {
    button_t testButton = {.samplingTimerPtr = samplingTimerPtr};

    SLP_isTimerRunning_ExpectAndReturn(testButton.samplingTimerPtr, true);
    // startPeriodictimer is not called in this case

    uint32_t retVal = startButtonTimer(&testButton, TIMER_SAMPLE);
    TEST_ASSERT_EQUAL_UINT32(SLPTIMER_OK, retVal);
}

void test_startButtonTimer_DebounceTimer(void) {
    button_t testButton = {.debounceTimerPtr = debounceTimerPtr};

    uint32_t retVal = startButtonTimer(&testButton, TIMER_DEBOUNCE);
}

// This test that the log lines are hit. As in tests we disable logging, this is more something you would spot on the
// coverage report.
void test_startButtonTimer_ErrorOnSLP_startTimer(void) {
    button_t testButton = {.samplingTimerPtr = samplingTimerPtr};

    SLP_isTimerRunning_ExpectAndReturn(testButton.samplingTimerPtr, false);
    SLP_startTimer_ExpectAndReturn(testButton.samplingTimerPtr, DEBOUNCE_SAMPLING_PERIOD_MS, TIMER_ONE_SHOT, NULL,
                                   &testButton, SLPTIMER_ERROR);
    // Sadly, the callback is a static function, so I can't test that startPeriodicTimer will be called with it as a parameter
    SLP_startTimer_IgnoreArg_callback();

    uint32_t retVal = startButtonTimer(&testButton, TIMER_SAMPLE);
    TEST_ASSERT_EQUAL_UINT32(SLPTIMER_ERROR, retVal);
}

// The sampling timer times out, so we run the samplingTimerCallback.
// Let's assume that the button is as default (released, integrator on 0, GPIO returns pressed)
void test_sleeptimerSamplingCallback(void) {
    // Let's make things easy and set the button in a way that samplingTimerCallback() does the bare minimum
    button_t testBtn = {.debounceCycles = (DEBOUNCE_TIME_MS / DEBOUNCE_SAMPLING_PERIOD_MS), .state = BUTTON_RELEASED};

    // Don't expect anything

    // The timerHandle is not used, we can pass NULL
    sleeptimerSamplingCallback(NULL, &testBtn);
    // Check that the callback was called, as it should have reset debounceCycles
    TEST_ASSERT_EQUAL_UINT32(0UL, testBtn.debounceCycles);
}