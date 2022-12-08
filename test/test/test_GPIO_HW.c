#include "gpio_HW.h"

#include <unity.h>

#include <string.h>

#include "mock_em_gpio.h"
#include "mock_em_cmu.h"

/*
 * Unit tests for "gpio_HW.h"
 */

void setUp(void) {
}

void tearDown(void) {
}

////
// This library is just a wrapper, so tests are quite straightforward, only filled for 100%
// completion. Just make sure calls are made with the correct parameters.
//
// PS: I said the test were trivial, but I caught a bug in enablePinInterrupts, were the direction of the OR op was
// wrong and having no effect! Test everything boys and girls!
////

void test_enableGPIOClock(void) {
    char* s[100] = {0};
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