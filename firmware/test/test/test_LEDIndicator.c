#include <unity.h>

// StdLib
#include <stdio.h>

// Our libs
#include "LEDIndicator.h"
#include "mock_gpio_HW.h"

/*
 * Unit tests for "LEDIndicator.h"
 */

extern LEDIndicator_t indicator;

void setUp(void) { memset((uint8_t*)&indicator, 0, sizeof(LEDIndicator_t)); }

void tearDown(void) {}

void test_LEDIndicator_Init_Success(void) {
    // Set Expectations
    setPinMode_Expect(indicator.ch1_port, indicator.ch1_pin, MODE_PUSHPULL, PIN_CLEAR);
    setPinMode_Expect(indicator.ch2_port, indicator.ch2_pin, MODE_PUSHPULL, PIN_CLEAR);
    setPinMode_Expect(indicator.ch3_port, indicator.ch3_pin, MODE_PUSHPULL, PIN_CLEAR);

    // Run function
    LEDIndicator_init();
}

////
// LEDIndicator_setChannel
// - Set one channel
// - Set all channels
// - Clear all channels
////

void test_LEDIndicator_setChannel_OneChannel(void) {
    channel_t testChannel = CHANNEL_1;
    pinPort_t testPort    = portC;
    uint32_t testPin      = 4;

    // Set Expectations
    indicator.ch1_port = testPort;
    indicator.ch1_pin  = testPin;
    writePin_Expect(indicator.ch1_port, PIN_SET, indicator.ch1_pin);
    writePin_Expect(indicator.ch2_port, PIN_CLEAR, indicator.ch2_pin);
    writePin_Expect(indicator.ch3_port, PIN_CLEAR, indicator.ch3_pin);

    // Run the code
    LEDIndicator_setChannel(testChannel);
}

void test_LEDIndicator_setChannel_SetAllChannels(void) {
    channel_t testChannel = CHANNEL_ALL;
    pinPort_t testPort1   = portC;
    uint32_t testPin1     = 4;
    pinPort_t testPort2   = portB;
    uint32_t testPin2     = 1;
    pinPort_t testPort3   = portA;
    uint32_t testPin3     = 2;

    // Set Expectations
    indicator.ch1_port = testPort1;
    indicator.ch1_pin  = testPin1;
    indicator.ch2_port = testPort2;
    indicator.ch2_pin  = testPin2;
    indicator.ch3_port = testPort3;
    indicator.ch3_pin  = testPin3;
    writePin_Expect(indicator.ch1_port, PIN_SET, indicator.ch1_pin);
    writePin_Expect(indicator.ch2_port, PIN_SET, indicator.ch2_pin);
    writePin_Expect(indicator.ch3_port, PIN_SET, indicator.ch3_pin);

    // Run the code
    LEDIndicator_setChannel(testChannel);
}

void test_LEDIndicator_setChannel_ClearAllChannels(void) {
    channel_t testChannel = CHANNEL_NONE;
    pinPort_t testPort1   = portC;
    uint32_t testPin1     = 4;
    pinPort_t testPort2   = portB;
    uint32_t testPin2     = 1;
    pinPort_t testPort3   = portA;
    uint32_t testPin3     = 2;

    // Set Expectations
    indicator.ch1_port = testPort1;
    indicator.ch1_pin  = testPin1;
    indicator.ch2_port = testPort2;
    indicator.ch2_pin  = testPin2;
    indicator.ch3_port = testPort3;
    indicator.ch3_pin  = testPin3;
    writePin_Expect(indicator.ch1_port, PIN_CLEAR, indicator.ch1_pin);
    writePin_Expect(indicator.ch2_port, PIN_CLEAR, indicator.ch2_pin);
    writePin_Expect(indicator.ch3_port, PIN_CLEAR, indicator.ch3_pin);

    // Run the code
    LEDIndicator_setChannel(testChannel);
}