#include <unity.h>

#include "app_assert.h"
#include "debounce.h"
#include "mock_buttons.h"
#include "mock_gpio_HW.h"
#include "mock_sleepyTimers_HW.h"

/*
 * Unit tests for "debounce.h
"
 */

// Globals that are defined in `buttons`. They don't exist now because we are mocking it
const uint32_t LONGPRESS_TIME_MS = 3000UL;
const uint32_t DEBOUNCE_TIME_MS = 75UL;
const uint32_t DEBOUNCE_SAMPLING_PERIOD_MS = 5UL;
const int32_t INTEGRATOR_TARGET = 5;

// Fake actions, they set a flag so we can check they were called
static bool fakePressActionFlag = false;
static bool fakeReleaseActionFlag = false;
void fakePressAction(void* ctx) {
    (void)ctx;
    fakePressActionFlag = true;
}
void fakeReleaseAction(void* ctx) {
    (void)ctx;
    static bool fakeReleaseActionFlag = true;
}

void setUp() {
    fakePressActionFlag = false;
    fakeReleaseActionFlag = false;
    // test assertions
    assertSetUp();
}

void tearDown() {
    // check assertions
    assertTearDownCheck();
}

////
// debounceButton
////

// List of Tests for debounceButton
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
    pinPort_t testPort = portA;
    uint8_t testPin = 1U;
    buttonState_t testState = BUTTON_RELEASED;
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
    startButtonTimer_ExpectAndReturn(&testBtn, TIMER_SAMPLE, BTN_OK);

    debounceButton(&testBtn);
    TEST_ASSERT_EQUAL_INT32(expectedIntegratorValue, testBtn.integrator);
    TEST_ASSERT_EQUAL_UINT32(expectedDebounceCyles, testBtn.debounceCycles);
}

void test_IntegratorDecreases(void) {
    pinPort_t testPort = portA;
    uint8_t testPin = 1U;
    buttonState_t testState = BUTTON_RELEASED;
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
    startButtonTimer_ExpectAndReturn(&testBtn, TIMER_SAMPLE, BTN_OK);

    debounceButton(&testBtn);
    TEST_ASSERT_EQUAL_INT32(expectedIntegratorValue, testBtn.integrator);
    TEST_ASSERT_EQUAL_UINT32(expectedDebounceCyles, testBtn.debounceCycles);
}

void test_IntegratorNotBelow0(void) {
    pinPort_t testPort = portA;
    uint8_t testPin = 1U;
    buttonState_t testState = BUTTON_RELEASED;  // as to not trigger the button state change part of the code
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
    startButtonTimer_ExpectAndReturn(&testBtn, TIMER_SAMPLE, BTN_OK);

    debounceButton(&testBtn);
    TEST_ASSERT_EQUAL_INT32(expectedIntegratorValue, testBtn.integrator);
    TEST_ASSERT_EQUAL_UINT32(expectedDebounceCyles, testBtn.debounceCycles);
}

void test_IntegratorNotAboveMax(void) {
    pinPort_t testPort = portA;
    uint8_t testPin = 1U;
    buttonState_t testState = BUTTON_PRESSED;  // as to not trigger the button state change part of the code
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
    startButtonTimer_ExpectAndReturn(&testBtn, TIMER_SAMPLE, BTN_OK);

    debounceButton(&testBtn);
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

    // Expectations
    readPin_ExpectAndReturn(portA, 1, 1);
    debounceButton(&testBtn);

    // Same thing with the release action
    testBtn.integrator = 0;
    testBtn.state = BUTTON_PRESSED;
    readPin_ExpectAndReturn(portA, 1, 0);

    debounceButton(&testBtn);
}

void test_IntegratorDecreasesMultiple(void) {
    pinPort_t testPort = portA;
    uint8_t testPin = 1U;
    buttonState_t testState = BUTTON_RELEASED;
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
    startButtonTimer_ExpectAndReturn(&testBtn, TIMER_SAMPLE, BTN_OK);

    debounceButton(&testBtn);

    readPin_ExpectAndReturn(testPort, testPin, expectedPinState);
    startButtonTimer_ExpectAndReturn(&testBtn, TIMER_SAMPLE, BTN_OK);

    debounceButton(&testBtn);

    readPin_ExpectAndReturn(testPort, testPin, expectedPinState);
    startButtonTimer_ExpectAndReturn(&testBtn, TIMER_SAMPLE, BTN_OK);

    debounceButton(&testBtn);

    TEST_ASSERT_EQUAL_INT32(expectedIntegratorValue, testBtn.integrator);
    TEST_ASSERT_EQUAL_UINT32(expectedDebounceCyles, testBtn.debounceCycles);
}

void test_IntegratorIncreasesMultiple(void) {
    pinPort_t testPort = portA;
    uint8_t testPin = 1U;
    buttonState_t testState = BUTTON_RELEASED;
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
    startButtonTimer_ExpectAndReturn(&testBtn, TIMER_SAMPLE, BTN_OK);

    debounceButton(&testBtn);

    readPin_ExpectAndReturn(testPort, testPin, expectedPinState);
    startButtonTimer_ExpectAndReturn(&testBtn, TIMER_SAMPLE, BTN_OK);

    debounceButton(&testBtn);

    readPin_ExpectAndReturn(testPort, testPin, expectedPinState);
    startButtonTimer_ExpectAndReturn(&testBtn, TIMER_SAMPLE, BTN_OK);

    debounceButton(&testBtn);

    TEST_ASSERT_EQUAL_INT32(expectedIntegratorValue, testBtn.integrator);
    TEST_ASSERT_EQUAL_UINT32(expectedDebounceCyles, testBtn.debounceCycles);
}

void test_fromReleasedToPressed(void) {
    pinPort_t testPort = portA;
    uint8_t testPin = 1U;
    buttonState_t testState = BUTTON_RELEASED;
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
        startButtonTimer_ExpectAndReturn(&testBtn, TIMER_SAMPLE, BTN_OK);

        // Press the button for (INTEGRATOR_TARGET -1) times
        debounceButton(&testBtn);
        // button must remain RELEASED
        TEST_ASSERT_EQUAL_UINT32(BUTTON_RELEASED, testBtn.state);
        TEST_ASSERT_EQUAL_UINT32(i + 1, testBtn.integrator);
    }
    // Press the button an additional time, the button state must switch
    readPin_ExpectAndReturn(testPort, testPin, expectedPinState);

    debounceButton(&testBtn);

    TEST_ASSERT_EQUAL_UINT32(BUTTON_PRESSED, testBtn.state);
    TEST_ASSERT_LESS_OR_EQUAL_INT64(timeAtPress, testBtn.lastPressMs);
    TEST_ASSERT_EQUAL_UINT32(expectedIntegratorValue, testBtn.integrator);
    TEST_ASSERT_EQUAL_UINT32(expectedDebounceCyles, testBtn.debounceCycles);
}

void test_fromPressedToReleased(void) {
    pinPort_t testPort = portA;
    uint8_t testPin = 1U;
    buttonState_t testState = BUTTON_PRESSED;
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
        startButtonTimer_ExpectAndReturn(&testBtn, TIMER_SAMPLE, BTN_OK);

        // Press the button for (INTEGRATOR_TARGET -1) times
        debounceButton(&testBtn);
        // button must remain RELEASED
        TEST_ASSERT_EQUAL_UINT32(BUTTON_PRESSED, testBtn.state);
        TEST_ASSERT_EQUAL_UINT32(INTEGRATOR_TARGET - (i + 1), testBtn.integrator);
    }
    // Press the button an additional time, the button state must switch
    readPin_ExpectAndReturn(testPort, testPin, expectedPinState);
    // If the button was released, we DON'T record when it happened

    debounceButton(&testBtn);

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
    buttonState_t testState = BUTTON_RELEASED;
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
        startButtonTimer_ExpectAndReturn(&testBtn, TIMER_SAMPLE, BTN_OK);

        debounceButton(&testBtn);
        // button must remain RELEASED
        TEST_ASSERT_EQUAL_UINT32(buttonExpectedState, testBtn.state);
        TEST_ASSERT_EQUAL_UINT32(integrator, testBtn.integrator);
        TEST_ASSERT_EQUAL_UINT32(i + 1, testBtn.debounceCycles);
    }

    debounceButton(&testBtn);
    // button must remain RELEASED, and the integrator and debounce cycles must be reset too
    TEST_ASSERT_EQUAL_UINT32(buttonExpectedState, testBtn.state);
    TEST_ASSERT_EQUAL_UINT32(0UL, testBtn.debounceCycles);
    TEST_ASSERT_EQUAL_UINT32(0UL, testBtn.integrator);
}

void test_someNoisyInputs_ButtonPressed(void) {
    pinPort_t testPort = portA;
    uint8_t testPin = 1U;
    buttonState_t testState = BUTTON_PRESSED;
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
        startButtonTimer_ExpectAndReturn(&testBtn, TIMER_SAMPLE, BTN_OK);

        debounceButton(&testBtn);
        // button must remain PRESSED
        TEST_ASSERT_EQUAL_UINT32(buttonExpectedState, testBtn.state);
        TEST_ASSERT_EQUAL_UINT32(integrator, testBtn.integrator);
        TEST_ASSERT_EQUAL_UINT32(i + 1, testBtn.debounceCycles);
    }

    debounceButton(&testBtn);
    // button must remain RELEASED, and the integrator and debounce cycles must be reset too
    TEST_ASSERT_EQUAL_UINT32(buttonExpectedState, testBtn.state);
    TEST_ASSERT_EQUAL_UINT32(0UL, testBtn.debounceCycles);
    TEST_ASSERT_EQUAL_UINT32(INTEGRATOR_TARGET, testBtn.integrator);
}
