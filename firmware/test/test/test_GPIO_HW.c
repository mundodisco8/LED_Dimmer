#include <string.h>
#include <unity.h>

#include "gpio_HW.h"
#include "mock_em_cmu.h"
#include "mock_em_gpio.h"
#include "testableAsserts.h"

/*
 * Unit tests for "gpio_HW.h"
 */

// Externs to test static stuff
extern GPIO_Port_TypeDef getSiLabsPort(pinPort_t port);
extern GPIO_Mode_TypeDef getSiLabsMode(pinMode_t mode);

void setUp(void) { assertSetUp(); }

void tearDown(void) { assertTearDownCheck(); }

////
// This library is just a wrapper, so tests are quite straightforward, only filled for 100%
// completion. Just make sure calls are made with the correct parameters.
//
// PS: I said the test were trivial, but I caught a bug in enablePinInterrupts, were the direction of the OR op was
// wrong and having no effect! Test everything boys and girls!
////

void test_enableGPIOClock(void) {
    CMU_ClockEnable_Expect(cmuClock_GPIO, true);
    enableGPIOClock();
}

void test_setPinMode(void) {
    pinPort_t testPort = portA;
    uint8_t testPin = 4;
    pinMode_t testMode = MODE_PUSHPULL;
    bool testDout = false;
    GPIO_PinModeSet_Expect(testPort, testPin, testMode, testDout);
    setPinMode(testPort, testPin, testMode, testDout);
}

void test_readPin(void) {
    pinPort_t testPort = portA;
    uint8_t testPin = 4;
    // Expect a certain pin and return HIGH
    GPIO_PinInGet_ExpectAndReturn(testPort, testPin, 1);
    uint32_t pinStatus = readPin(testPort, testPin);
    TEST_ASSERT_EQUAL_UINT32(1, pinStatus);
}

void test_configurePinInterrupt(void) {
    pinPort_t testPort = portA;
    uint8_t testPin = 4;
    pinMode_t testIntNo = 4;
    bool rising = true;
    bool falling = true;
    bool enable = true;
    GPIO_ExtIntConfig_Expect(testPort, testPin, testIntNo, rising, falling, enable);

    configurePinInterrupt(testPort, testPin, testIntNo, rising, falling, enable);
}

void test_enablePinInterrupts_WithNoPinsEnabled(void) {
    uint8_t pinsToEnable = 0x26;

    GPIO_EnabledIntGet_ExpectAndReturn(0x00);
    GPIO_IntEnable_Expect(pinsToEnable);

    enablePinInterrupts(pinsToEnable);
}

void test_enablePinInterrupts_WithSomePinsAlreadyEnabled(void) {
    uint32_t pinsToEnable = 0x26;
    uint32_t enabledPins = 0xF450;

    GPIO_EnabledIntGet_ExpectAndReturn(enabledPins);
    GPIO_IntEnable_Expect(pinsToEnable | enabledPins);

    enablePinInterrupts(pinsToEnable);
}

////
// Static Getters
////

void test_getPort_Success(void) {
    pinPort_t testPortA = portA;
    pinPort_t testPortB = portB;
    pinPort_t testPortC = portC;
    pinPort_t testPortD = portD;

    // Expectations
    GPIO_Port_TypeDef expectedPortA = gpioPortA;
    GPIO_Port_TypeDef expectedPortB = gpioPortB;
    GPIO_Port_TypeDef expectedPortC = gpioPortC;
    GPIO_Port_TypeDef expectedPortD = gpioPortD;

    GPIO_Port_TypeDef returnedPort = getSiLabsPort(testPortA);
    TEST_ASSERT_EQUAL_UINT32(expectedPortA, returnedPort);

    returnedPort = getSiLabsPort(testPortB);
    TEST_ASSERT_EQUAL_UINT32(expectedPortB, returnedPort);

    returnedPort = getSiLabsPort(testPortC);
    TEST_ASSERT_EQUAL_UINT32(expectedPortC, returnedPort);

    returnedPort = getSiLabsPort(testPortD);
    TEST_ASSERT_EQUAL_UINT32(expectedPortD, returnedPort);
}

void test_getPort_AssertsOnPortE(void) {
    pinPort_t testPortE = portE;

    // The BGM220P has no pins in portE
    assertExpectFailure();
    GPIO_Port_TypeDef returnedPort = getSiLabsPort(testPortE);
}

void test_getPort_AssertsOnPortF(void) {
    pinPort_t testPortF = portF;

    // The BGM220P has no pins in portF
    assertExpectFailure();
    GPIO_Port_TypeDef returnedPort = getSiLabsPort(testPortF);
}

void test_getPort_AssertsOnUnknownPort(void) {
    // Assert on unknown port
    assertExpectFailure();
    getSiLabsPort((pinPort_t)0xFF);
}

void test_getMode_Success(void) {
    pinMode_t testModeDisabled = MODE_DISABLED;
    pinMode_t testModeInput = MODE_INPUT;
    pinMode_t testModeInputPull = MODE_INPUT_PULL;
    pinMode_t testModeInputPullFilter = MODE_INPUT_PULL_FILTER;
    pinMode_t testModePushPull = MODE_PUSHPULL;

    // Expectations
    GPIO_Mode_TypeDef expectedModeDisabled = gpioModeDisabled;
    GPIO_Mode_TypeDef expectedModeInput = gpioModeInput;
    GPIO_Mode_TypeDef expectedModeInputPull = gpioModeInputPull;
    GPIO_Mode_TypeDef expectedModeInputPullFilter = gpioModeInputPullFilter;
    GPIO_Mode_TypeDef expectedModePushPull = gpioModePushPull;

    GPIO_Mode_TypeDef returnedMode = getSiLabsMode(testModeDisabled);
    TEST_ASSERT_EQUAL_UINT32(expectedModeDisabled, returnedMode);

    returnedMode = getSiLabsMode(testModeInput);
    TEST_ASSERT_EQUAL_UINT32(expectedModeInput, returnedMode);

    returnedMode = getSiLabsMode(testModeInputPull);
    TEST_ASSERT_EQUAL_UINT32(expectedModeInputPull, returnedMode);

    returnedMode = getSiLabsMode(testModeInputPullFilter);
    TEST_ASSERT_EQUAL_UINT32(expectedModeInputPullFilter, returnedMode);

    returnedMode = getSiLabsMode(testModePushPull);
    TEST_ASSERT_EQUAL_UINT32(expectedModePushPull, returnedMode);
}

void test_getMode_AssertsOnUknownMode(void) {
    pinMode_t testModeUnknown = (pinMode_t)0xFF;

    // Expect assert on unknown mode
    assertExpectFailure();
    getSiLabsMode(testModeUnknown);
}

// SetPinUpForEM4WakeUp
// * Success
// * Pin is not a WakeUpPin -> asserts

void test_SetPinUpForEM4WakeUp_Success(void) {
    pinPort_t testPort = portA;
    uint8_t testPin = 5;

    // Set Expectations
    pinMode_t expectedMode = MODE_INPUT_PULL_FILTER;
    bool expectedDout = true;
    uint32_t expectedEM4Mask = GPIO_IEN_EM4WUIEN0;
    GPIO_PinModeSet_Expect(testPort, testPin, expectedMode, expectedDout);
    GPIO_EM4EnablePinWakeup_Expect(expectedEM4Mask, 0);

    setPinUpForEM4WakeUp(testPort, testPin);

    testPort = portB;
    testPin = 1;
    expectedEM4Mask = GPIO_IEN_EM4WUIEN3;
    GPIO_PinModeSet_Expect(testPort, testPin, expectedMode, expectedDout);
    GPIO_EM4EnablePinWakeup_Expect(expectedEM4Mask, 0);

    setPinUpForEM4WakeUp(testPort, testPin);

    testPort = portB;
    testPin = 3;
    expectedEM4Mask = GPIO_IEN_EM4WUIEN4;
    GPIO_PinModeSet_Expect(testPort, testPin, expectedMode, expectedDout);
    GPIO_EM4EnablePinWakeup_Expect(expectedEM4Mask, 0);

    setPinUpForEM4WakeUp(testPort, testPin);

    testPort = portC;
    testPin = 0;
    expectedEM4Mask = GPIO_IEN_EM4WUIEN6;
    GPIO_PinModeSet_Expect(testPort, testPin, expectedMode, expectedDout);
    GPIO_EM4EnablePinWakeup_Expect(expectedEM4Mask, 0);

    setPinUpForEM4WakeUp(testPort, testPin);

    testPort = portC;
    testPin = 5;
    expectedEM4Mask = GPIO_IEN_EM4WUIEN7;
    GPIO_PinModeSet_Expect(testPort, testPin, expectedMode, expectedDout);
    GPIO_EM4EnablePinWakeup_Expect(expectedEM4Mask, 0);

    setPinUpForEM4WakeUp(testPort, testPin);

    testPort = portC;
    testPin = 7;
    expectedEM4Mask = GPIO_IEN_EM4WUIEN8;
    GPIO_PinModeSet_Expect(testPort, testPin, expectedMode, expectedDout);
    GPIO_EM4EnablePinWakeup_Expect(expectedEM4Mask, 0);

    setPinUpForEM4WakeUp(testPort, testPin);

    testPort = portD;
    testPin = 2;
    expectedEM4Mask = GPIO_IEN_EM4WUIEN9;
    GPIO_PinModeSet_Expect(testPort, testPin, expectedMode, expectedDout);
    GPIO_EM4EnablePinWakeup_Expect(expectedEM4Mask, 0);

    setPinUpForEM4WakeUp(testPort, testPin);
}

void test_SetPinUpForEM4WakeUp_AssertsOnNonWakeUpPin(void) {
    pinPort_t testPort = portA;
    uint8_t testPin = 0;
    assertExpectFailure();

    setPinUpForEM4WakeUp(testPort, testPin);
}