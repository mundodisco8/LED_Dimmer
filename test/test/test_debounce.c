#include "debounce.h"

// Ugly, but debounce has two static functions that are not testable otherwise, as they are only called as
// callbacks of a timer running out, and we can't trigger that timer in the test.
#include "debounce.c"

#include <unity.h>

#include "mock_buttons.h"
#include "mock_gpio_HW.h"
#include "mock_sleepyTimers_HW.h"

/*
 * Unit tests for "debounce.h
"
 */

// Some empty action callbacks for the buttons
void fakePressAction(void) {}
void fakeReleaseAction(void) {}

// Some timer handlers for the buttons
timerHandle_t debounceTimer = {0};
timerHandle_t samplingTimer = {0};

void setUp(void) {
}

void tearDown(void) {
}

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

void test_IntegratorIncreases(void) {
    button_t testBtn = {
        .btnPort = portA,
        .pinNo = 1,
        .state = BUTTON_RELEASED,
        .integrator = 0};

    readPin_ExpectAndReturn(portA, 1, 1);
    samplingTimerCallback(&testBtn);
    TEST_ASSERT_EQUAL_INT32(1, testBtn.integrator);
}

void test_IntegratorDecreases(void) {
    button_t testBtn = {
        .btnPort = portA,
        .pinNo = 1,
        .state = BUTTON_RELEASED,
        .integrator = 2};

    readPin_ExpectAndReturn(portA, 1, 0);
    samplingTimerCallback(&testBtn);
    TEST_ASSERT_EQUAL_INT32(1, testBtn.integrator);
}

void test_IntegratorNotBelow0(void) {
    button_t testBtn = {
        .btnPort = portA,
        .pinNo = 1,
        .state = BUTTON_RELEASED,
        .integrator = 0};

    readPin_ExpectAndReturn(portA, 1, 0);
    // This shouldn't trigger a call to stopButtonTimers
    samplingTimerCallback(&testBtn);
    TEST_ASSERT_EQUAL_INT32(0, testBtn.integrator);
}

void test_IntegratorNotAboveMax(void) {
    button_t testBtn = {
        .btnPort = portA,
        .pinNo = 1,
        .state = BUTTON_RELEASED,
        .integrator = INTEGRATOR_TARGET,
        .pressedAction = fakePressAction,
        .releasedAction = fakeReleaseAction};

    readPin_ExpectAndReturn(portA, 1, 1);
    // Expectations for stopButtonTimers. Ignore parameters as we are not testing it here.
    // Retunr false so we exit the execution faster. It doesn't change the outcome
    SLP_isTimerRunning_IgnoreAndReturn(false);

    samplingTimerCallback(&testBtn);
    TEST_ASSERT_EQUAL_INT32(INTEGRATOR_TARGET, testBtn.integrator);
}

// In the previous test, I realised that if I leave the button actions pointing to NULL, we get an exception when
// that "action" is called. Add some code to fix it
void test_ButtonActionIsNull(void) {
    button_t testBtn = {
        .btnPort = portA,
        .pinNo = 1,
        .state = BUTTON_RELEASED,
        .integrator = INTEGRATOR_TARGET,
        .pressedAction = NULL,
        .releasedAction = NULL};

    readPin_ExpectAndReturn(portA, 1, 1);
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
    button_t testBtn = {
        .btnPort = portA,
        .pinNo = 1,
        .state = BUTTON_RELEASED,
        .integrator = 4};

    readPin_ExpectAndReturn(portA, 1, 0);
    samplingTimerCallback(&testBtn);
    readPin_ExpectAndReturn(portA, 1, 0);
    samplingTimerCallback(&testBtn);
    readPin_ExpectAndReturn(portA, 1, 0);
    samplingTimerCallback(&testBtn);
    TEST_ASSERT_EQUAL_INT32(1, testBtn.integrator);
}

void test_IntegratorIncreasesMultiple(void) {
    button_t testBtn = {
        .btnPort = portA,
        .pinNo = 1,
        .state = BUTTON_RELEASED,
        .integrator = 1};

    readPin_ExpectAndReturn(portA, 1, 1);
    samplingTimerCallback(&testBtn);
    readPin_ExpectAndReturn(portA, 1, 1);
    samplingTimerCallback(&testBtn);
    readPin_ExpectAndReturn(portA, 1, 1);
    samplingTimerCallback(&testBtn);
    TEST_ASSERT_EQUAL_INT32(4, testBtn.integrator);
}

void test_fromReleasedToPressed(void) {
    button_t testBtn = {
        .btnPort = portA,
        .pinNo = 1,
        .state = BUTTON_RELEASED,
        .integrator = 0};

    for (uint32_t i = 0; i < INTEGRATOR_TARGET - 1; i++) {
        // Press the button for (INTEGRATOR_TARGET -1) times
        readPin_ExpectAndReturn(portA, 1, 1);
        samplingTimerCallback(&testBtn);
        // button must remain RELEASED
        TEST_ASSERT_EQUAL_UINT32(BUTTON_RELEASED, testBtn.state);
    }
    // Press the button an additional time, the button state must switch
    readPin_ExpectAndReturn(portA, 1, 1);
    // Expectations for stopButtonTimers. Ignore parameters as we are not testing it here.
    // Retunr false so we exit the execution faster. It doesn't change the outcome
    SLP_isTimerRunning_IgnoreAndReturn(false);
    samplingTimerCallback(&testBtn);

    TEST_ASSERT_EQUAL_UINT32(BUTTON_PRESSED, testBtn.state);
}

void test_fromPressedToReleased(void) {
    button_t testBtn = {
        .btnPort = portA,
        .pinNo = 1,
        .state = BUTTON_PRESSED,
        .integrator = INTEGRATOR_TARGET,
        .pressedAction = fakePressAction,
        .releasedAction = fakeReleaseAction};

    for (uint32_t i = 0; i < INTEGRATOR_TARGET - 1; i++) {
        // Press the button for (INTEGRATOR_TARGET -1) times
        readPin_ExpectAndReturn(portA, 1, 0);
        samplingTimerCallback(&testBtn);
        // button must remain RELEASED
        TEST_ASSERT_EQUAL_UINT32(BUTTON_PRESSED, testBtn.state);
    }
    // Press the button an additional time, the button state must switch
    readPin_ExpectAndReturn(portA, 1, 0);
    // Expectations for stopButtonTimers. Ignore parameters as we are not testing it here.
    // Retunr false so we exit the execution faster. It doesn't change the outcome
    SLP_isTimerRunning_IgnoreAndReturn(false);
    samplingTimerCallback(&testBtn);

    TEST_ASSERT_EQUAL_UINT32(BUTTON_RELEASED, testBtn.state);
}

void test_someNoisyInputs(void) {
    button_t testBtn = {
        .btnPort = portA,
        .pinNo = 1,
        .state = BUTTON_RELEASED,
        .integrator = 0,
        .pressedAction = fakePressAction,
        .releasedAction = fakeReleaseAction};

    // Test a noisy press
    uint32_t buttonExpectedPress1[] = {1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1};
    // integrator values = 1  2  3  2  1  2  3  4  5  4  5
    // this is used to check the expected state, based on the input
    uint32_t buttonExpectedState1[] = {BUTTON_RELEASED, BUTTON_RELEASED, BUTTON_RELEASED, BUTTON_RELEASED, BUTTON_RELEASED, BUTTON_RELEASED, BUTTON_RELEASED, BUTTON_RELEASED, BUTTON_PRESSED, BUTTON_PRESSED, BUTTON_PRESSED};
    // Expectations for stopButtonTimers. Ignore parameters as we are not testing it here.
    // Retunr false so we exit the execution faster. It doesn't change the outcome
    SLP_isTimerRunning_IgnoreAndReturn(false);

    for (uint32_t i = 0; i < (sizeof(buttonExpectedPress1) / sizeof(buttonExpectedPress1[0])); i++) {
        readPin_ExpectAndReturn(portA, 1, buttonExpectedPress1[i]);
        samplingTimerCallback(&testBtn);
        // button must remain RELEASED
        TEST_ASSERT_EQUAL_UINT32(buttonExpectedState1[i], testBtn.state);
    }

    // Test a noisy release
    testBtn.state = BUTTON_PRESSED;
    testBtn.integrator = INTEGRATOR_TARGET;
    uint32_t buttonExpectedPress2[] = {0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0};
    // integrator values = 5  4  3  2  3  4  3  2  1  0  1, 0
    // this is used to check the expected state, based on the input
    uint32_t buttonExpectedState2[] = {BUTTON_PRESSED, BUTTON_PRESSED, BUTTON_PRESSED, BUTTON_PRESSED, BUTTON_PRESSED, BUTTON_PRESSED, BUTTON_PRESSED, BUTTON_PRESSED, BUTTON_RELEASED, BUTTON_RELEASED, BUTTON_RELEASED, BUTTON_RELEASED};

    for (uint32_t i = 0; i < (sizeof(buttonExpectedPress2) / sizeof(buttonExpectedPress2[0])); i++) {
        readPin_ExpectAndReturn(portA, 1, buttonExpectedPress2[i]);
        samplingTimerCallback(&testBtn);
        // button must remain RELEASED
        TEST_ASSERT_EQUAL_UINT32(buttonExpectedState2[i], testBtn.state);
    }
}

void test_corruptionGuardOn0(void) {
    button_t testBtn = {
        .btnPort = portA,
        .pinNo = 1,
        .state = BUTTON_RELEASED,
        .integrator = -50,
        .pressedAction = fakePressAction,
        .releasedAction = fakeReleaseAction};

    readPin_ExpectAndReturn(portA, 1, 0);
    // This shouldn't trigger a call to stopTimmers
    samplingTimerCallback(&testBtn);
    TEST_ASSERT_EQUAL_INT32(0, testBtn.integrator);

    // But what if the corruption happens with the button being pressed?
    testBtn.state = BUTTON_PRESSED;
    testBtn.integrator = -50;
    readPin_ExpectAndReturn(portA, 1, 0);
    // This SHOULD trigger a call to stopTimmers
    // Expectations for stopButtonTimers. Ignore parameters as we are not testing it here.
    // Retunr false so we exit the execution faster. It doesn't change the outcome
    SLP_isTimerRunning_IgnoreAndReturn(false);
    samplingTimerCallback(&testBtn);
    TEST_ASSERT_EQUAL_INT32(0, testBtn.integrator);
}

void test_corruptionGuardOn1(void) {
    button_t testBtn = {
        .btnPort = portA,
        .pinNo = 1,
        .state = BUTTON_PRESSED,
        .integrator = 50,
        .pressedAction = fakePressAction,
        .releasedAction = fakeReleaseAction};

    readPin_ExpectAndReturn(portA, 1, 0);
    // This SHOULDN'T trigger a call to stopTimmers
    samplingTimerCallback(&testBtn);
    TEST_ASSERT_EQUAL_INT32(INTEGRATOR_TARGET, testBtn.integrator);

    testBtn.state = BUTTON_RELEASED;
    testBtn.integrator = 50;
    readPin_ExpectAndReturn(portA, 1, 0);
    // Expectations for stopButtonTimers. Ignore parameters as we are not testing it here.
    // Retunr false so we exit the execution faster. It doesn't change the outcome
    SLP_isTimerRunning_IgnoreAndReturn(false);
    samplingTimerCallback(&testBtn);
    TEST_ASSERT_EQUAL_INT32(5, testBtn.integrator);
}

// This test checks that the button state is only set to BUTTON_PRESSED when the integrator converges to
// INTEGRATOR_TARGET and the button initial state is BUTTON_RELEASED and that the state is only set to BUTTON_RELEASED
// when the integrator reaches 0 and the initial state is BUTTON_PRESSED.
// This is important because we will use the change of state to stop the timers and we don't want to do it if the
// integrator is, for example, at 0, rises to 1 on a press but then noise returns it to 0.
void test_stateChangesOnlyWhenNeeded(void) {
    // Test that BUTTON_PRESSED is only set if state is BUTTON_RELEASED
    button_t testBtn = {
        .btnPort = portA,
        .pinNo = 1,
        .state = BUTTON_RELEASED,
        .integrator = 0,
        .pressedAction = fakePressAction,
        .releasedAction = fakeReleaseAction,
        .samplingTimerPtr = &samplingTimer,
        .debounceTimerPtr = &debounceTimer};

    // Test a noisy press
    uint32_t buttonExpectedPress[] = {1, 0, 1, 0, 0, 1, 1, 1, 1, 1};
    // integrator values = 1  0  1  0  0  1  2  3  4  5
    // stopButtonTimers should be only called once, when the integrator reaches 5, and not when it reaches 0

    for (uint32_t i = 0; i < (sizeof(buttonExpectedPress) / sizeof(buttonExpectedPress[0])); i++) {
        readPin_ExpectAndReturn(portA, 1, buttonExpectedPress[i]);
    }
    // Stop timer should be called only once, at the last call of samplingTimerCallback()
    // Expectations for stopButtonTimers. Ignore parameters as we are not testing it here.
    // Retunr false so we exit the execution faster. It doesn't change the outcome
    SLP_isTimerRunning_ExpectAndReturn(testBtn.samplingTimerPtr, false);
    SLP_isTimerRunning_ExpectAndReturn(testBtn.debounceTimerPtr, false);
    for (uint32_t i = 0; i < (sizeof(buttonExpectedPress) / sizeof(buttonExpectedPress[0])); i++) {
        samplingTimerCallback(&testBtn);
    }
}

////
// StartButtonTimer()
//
// Also tests getDebounceTime() and getDebounceSamplingPeriod() (statics called by it)
////

// 1) Start a sampling timer that is not already running
//    a) Also test that getDebounceSamplingPeriod() returns the right value
// 2) Start a sampling timer that IS already running
// 3) Start a debounce timer that is not already running
//    a) Also test that getDebounceTime() returns the right value
// 4) Start a debounce timer that IS already running
// 5) Checks that we hit the logging function when an error is returned by the SLP* functions

// extern const uint32_t DEBOUNCE_TIME_MS;
// extern const uint32_t DEBOUNCE_SAMPLING_PERIOD_MS;

void test_startButtonTimer_SamplingTimerNotRunning(void) {
    button_t testButton = {.samplingTimerPtr = &samplingTimer};

    SLP_isTimerRunning_ExpectAndReturn(testButton.samplingTimerPtr, false);
    SLP_startPeriodicTimer_ExpectAndReturn(testButton.samplingTimerPtr, DEBOUNCE_SAMPLING_PERIOD_MS, NULL, &testButton, SLPTIMER_OK);
    // Sadly, the callback is a static function, so I can't test that startPeriodicTimer will be called with it as a parameter
    SLP_startPeriodicTimer_IgnoreArg_callback();

    uint32_t retVal = startButtonTimer(&testButton, TIMER_SAMPLE);
    TEST_ASSERT_EQUAL_UINT32(SLPTIMER_OK, retVal);
}

void test_startButtonTimer_SamplingTimerISRunning(void) {
    button_t testButton = {
        .samplingTimerPtr = &samplingTimer};

    SLP_isTimerRunning_ExpectAndReturn(testButton.samplingTimerPtr, true);
    // startPeriodictimer is not called in this case

    uint32_t retVal = startButtonTimer(&testButton, TIMER_SAMPLE);
    TEST_ASSERT_EQUAL_UINT32(SLPTIMER_OK, retVal);
}

void test_startButtonTimer_DebounceTimerNotRunning(void) {
    button_t testButton = {.debounceTimerPtr = &debounceTimer};

    SLP_isTimerRunning_ExpectAndReturn(testButton.debounceTimerPtr, false);
    SLP_startTimer_ExpectAndReturn(testButton.debounceTimerPtr, DEBOUNCE_TIME_MS, NULL, &testButton, SLPTIMER_OK);
    // Sadly, the callback is a static function, so I can't test that startPeriodicTimer will be called with it as a parameter
    SLP_startTimer_IgnoreArg_callback();

    uint32_t retVal = startButtonTimer(&testButton, TIMER_DEBOUNCE);
    TEST_ASSERT_EQUAL_UINT32(SLPTIMER_OK, retVal);
}
void test_startButtonTimer_DebounceTimerISRunning(void) {
    button_t testButton = {
        .debounceTimerPtr = &debounceTimer};

    SLP_isTimerRunning_ExpectAndReturn(testButton.debounceTimerPtr, true);
    // startPeriodictimer is not called in this case

    uint32_t retVal = startButtonTimer(&testButton, TIMER_DEBOUNCE);
    TEST_ASSERT_EQUAL_UINT32(SLPTIMER_OK, retVal);
}

// This test that the log lines are hit. As in tests we disable logging, this is more something you would spot on the
// coverage report.
void test_startButtonTimer_ErrorOnSLP_startPeriodicTimer(void) {
    button_t testButton = {.samplingTimerPtr = &samplingTimer};

    SLP_isTimerRunning_ExpectAndReturn(testButton.samplingTimerPtr, false);
    SLP_startPeriodicTimer_ExpectAndReturn(testButton.samplingTimerPtr, DEBOUNCE_SAMPLING_PERIOD_MS, NULL, &testButton, SLPTIMER_ERROR);
    // Sadly, the callback is a static function, so I can't test that startPeriodicTimer will be called with it as a parameter
    SLP_startPeriodicTimer_IgnoreArg_callback();

    uint32_t retVal = startButtonTimer(&testButton, TIMER_SAMPLE);
    TEST_ASSERT_EQUAL_UINT32(SLPTIMER_ERROR, retVal);
}

////
// stopButtonTimers
//
// This is a static function, so it has to be tested via the samplingTimerCallback()
////

// 1) Check both timers, both are running, stop them
// 2) Check both timers, both are NOT running, nothing to do
// 3) Check the sampling timer, is running, try to stop it but stop fails

void test_stopButtonTimers_BothAreRunnig(void) {
    // To trigger stopButtonTimers, we set the button as RELEASED and integrator as INTEGRATOR_TARGET - 1, and then
    // mock a readPin showing a press, to get the integrator to the convergence value

    button_t testBtn = {
        .state = BUTTON_RELEASED,
        .integrator = INTEGRATOR_TARGET - 1,
        .samplingTimerPtr = &samplingTimer,
        .debounceTimerPtr = &debounceTimer};

    // Mock the button being pressed. Ignore parameters,  samplingTimerCallback is not being tested here
    readPin_IgnoreAndReturn(1);

    SLP_isTimerRunning_ExpectAndReturn(testBtn.samplingTimerPtr, true);
    SLP_stopTimer_ExpectAndReturn(testBtn.samplingTimerPtr, SLPTIMER_OK);

    SLP_isTimerRunning_ExpectAndReturn(testBtn.debounceTimerPtr, true);
    SLP_stopTimer_ExpectAndReturn(testBtn.debounceTimerPtr, SLPTIMER_OK);

    samplingTimerCallback(&testBtn);
}

void test_stopButtonTimers_NoneRunnig(void) {
    // To trigger stopButtonTimers, we set the button as RELEASED and integrator as INTEGRATOR_TARGET - 1, and then
    // mock a readPin showing a press, to get the integrator to the convergence value

    button_t testBtn = {
        .state = BUTTON_RELEASED,
        .integrator = INTEGRATOR_TARGET - 1,
        .samplingTimerPtr = &samplingTimer,
        .debounceTimerPtr = &debounceTimer};

    // Mock the button being pressed. Ignore parameters,  samplingTimerCallback is not being tested here
    readPin_IgnoreAndReturn(1);

    SLP_isTimerRunning_ExpectAndReturn(testBtn.samplingTimerPtr, false);

    SLP_isTimerRunning_ExpectAndReturn(testBtn.debounceTimerPtr, false);

    samplingTimerCallback(&testBtn);
}

void test_stopButtonTimers_SamplingTimerRunningButErrorOnStop(void) {
    // To trigger stopButtonTimers, we set the button as RELEASED and integrator as INTEGRATOR_TARGET - 1, and then
    // mock a readPin showing a press, to get the integrator to the convergence value

    button_t testBtn = {
        .state = BUTTON_RELEASED,
        .integrator = INTEGRATOR_TARGET - 1,
        .samplingTimerPtr = &samplingTimer,
        .debounceTimerPtr = &debounceTimer};

    // Mock the button being pressed. Ignore parameters,  samplingTimerCallback is not being tested here
    readPin_IgnoreAndReturn(1);

    SLP_isTimerRunning_ExpectAndReturn(testBtn.samplingTimerPtr, true);
    SLP_stopTimer_ExpectAndReturn(testBtn.samplingTimerPtr, SLPTIMER_ERROR);

    samplingTimerCallback(&testBtn);
}

void test_stopButtonTimers_DebounceTimersRunningButErrorOnStop(void) {
    // To trigger stopButtonTimers, we set the button as RELEASED and integrator as INTEGRATOR_TARGET - 1, and then
    // mock a readPin showing a press, to get the integrator to the convergence value

    button_t testBtn = {
        .state = BUTTON_RELEASED,
        .integrator = INTEGRATOR_TARGET - 1,
        .samplingTimerPtr = &samplingTimer,
        .debounceTimerPtr = &debounceTimer};

    // Mock the button being pressed. Ignore parameters,  samplingTimerCallback is not being tested here
    readPin_IgnoreAndReturn(1);

    SLP_isTimerRunning_ExpectAndReturn(testBtn.samplingTimerPtr, false);
    SLP_isTimerRunning_ExpectAndReturn(testBtn.debounceTimerPtr, true);
    SLP_stopTimer_ExpectAndReturn(testBtn.debounceTimerPtr, SLPTIMER_ERROR);

    samplingTimerCallback(&testBtn);
}

////
// sleeptimerDebounceCallback() and sleeptimerSamplingCallback()
//
// Static functions that are called as callbacks of timers expiring, so they require to include the .c file. Urgh!
////

// The debounce timer times out, so we pass a button to its callback and turn both timers off
void test_sleepTimerDebounceCallback(void) {
    button_t testBtn = {
        .samplingTimerPtr = &samplingTimer,
        .debounceTimerPtr = &debounceTimer};

    SLP_isTimerRunning_ExpectAndReturn(testBtn.samplingTimerPtr, true);
    SLP_stopTimer_ExpectAndReturn(testBtn.samplingTimerPtr, SLPTIMER_OK);

    SLP_isTimerRunning_ExpectAndReturn(testBtn.debounceTimerPtr, true);
    SLP_stopTimer_ExpectAndReturn(testBtn.debounceTimerPtr, SLPTIMER_OK);

    // Pass null as the timerHandle_t, is not used, we don't care
    sleeptimerDebounceCallback(NULL, &testBtn);
}

// The sampling timer times out, so we run the samplingTimerCallback.
// Let's assume that the button is as default (released, integrator on 0, GPIO returns pressed)
void test_sleeptimerSamplingCallback(void) {
    button_t testBtn = {
        .integrator = 0,
        .state = BUTTON_RELEASED};

    readPin_IgnoreAndReturn(1);

    // The timerHandle is not used, we can pass NULL
    sleeptimerSamplingCallback(NULL, &testBtn);
    // Integrator will increase
    TEST_ASSERT_EQUAL_UINT32(1, testBtn.integrator);
}