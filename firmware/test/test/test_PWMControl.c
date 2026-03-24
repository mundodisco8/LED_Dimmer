#include <string.h>
#include <unity.h>

#include "PWMControl.h"
#include "app_assert.h"
#include "brightnessLUT.h"
#include "mock_timer_HW.h"
/*
 * Unit tests for "PWMControl.h"
 */

////
// Static bits
////
extern void configureTimerPWMFrequency(uint32_t frequencyHz);

void setUp(void) {
    // Assertions setup
    assertSetUp();
}

void tearDown(void) {
    // Check assertions
    assertTearDownCheck();
}

////
// initTimer0HW and initTimer0CCChannel
////

// This is just a choreography, check functions are named in the right order

void test_initTimer0PWM_Success(void) {
    uint32_t testClockFreq         = 32000000;
    uint32_t testFreq              = 250;
    uint32_t ExpectedTimerTopValue = (testClockFreq / (2 * testFreq));  // top value formula for up-down count
    TIMHW_initTimer0Clock_Expect();
    TIMHW_initTimer0_Expect(false);
    TIMHW_getTimer0Frequency_ExpectAndReturn(testClockFreq);
    TIMHW_setTimer0TopValue_ExpectAndReturn(ExpectedTimerTopValue, TIMER_OK);

    initTimer0PWM(testFreq);
}

void test_initTimer0CCChannel(void) {
    CCChannel_t testChannel = CC_CHANNEL_0;
    polarity_t testPol      = PWM_ACTIVE_HIGH;
    pinPort_t testPort      = portB;
    uint32_t testPinNo      = 3;

    // Set Expectations
    TIMHW_enableChannelCompCapUnit_Expect(testChannel);
    TIMHW_setCCChannelPin_ExpectAndReturn(testPort, testPinNo, testChannel, TIMER_OK);
    TIMHW_configCCChannelPWM_Expect(testChannel, testPol);
    TIMHW_startTimer0_Expect();

    initTimer0CCChannel(testChannel, testPort, testPinNo, testPol);
}

////
// configureTimerPWMFrequency()
////

// 1) Test the right value is set as top
// 2) Protect against freq set being 0 (0 division)
// 3) Protect against freq set so high that we have less than 12 bit of resolution

extern const uint32_t MIN_PWM_FREQ;
extern const uint32_t MIN_PWM_LEVELS;

void test_configureTimerPWMFrequency_TopValueIsSet(void) {
    uint32_t clockFreq = 38400000;
    uint32_t testFreq  = 800;
    TIMHW_getTimer0Frequency_ExpectAndReturn(clockFreq);
    // There's no way to test the freq was correct, except checking that TOP is set to the right value
    uint32_t expectedTop = (clockFreq / (2 * testFreq));
    TIMHW_setTimer0TopValue_ExpectAndReturn(expectedTop, TIMER_OK);

    configureTimerPWMFrequency(testFreq);
}

void test_configureTimerPWMFrequency_Freq_is_less_than_MIN_PWM_FREQ(void) {
    uint32_t testFreq = 0;

    // Set expectations
    assertExpectFailure();

    configureTimerPWMFrequency(testFreq);
}

void test_configureTimerPWMFrequency_Freq_is_too_high(void) {
    uint32_t testFreq = 38400000;  // would give 1 level of quantization: 0 or 100%

    // Set expectations
    assertExpectFailure();

    configureTimerPWMFrequency(testFreq);
}

////
// getPWMFrequency
//
// - Just check the correct functions are called and return the right value
////

void test_getPWMFrequency(void) {
    uint32_t expectedTimerClock = 1000;
    uint32_t expectedTop        = 10;
    uint32_t expectedFreq       = 50;  // (1000 / (2 * 10))

    // expectations
    TIMHW_getTimer0Frequency_ExpectAndReturn(expectedTimerClock);
    TIMHW_getTimer0TopValue_ExpectAndReturn(expectedTop);

    uint32_t retVal = getPWMFrequency();

    TEST_ASSERT_EQUAL_UINT32(expectedFreq, retVal);
}

////
// buildGammaLookUpTable()
////

//extern void buildGammaLookUpTable(const uint32_t maxValue);
// extern const uint32_t GAMMA_VALUE;
// #define gammaLuTSize 101
// void test_buildGammaLookUpTable_checkTable(void) {
//     uint32_t clockFreq = 38400000;
//     uint32_t testPWMFreq = 1000;

//     // Got this table from the Jupyter notebook in /docs
//     uint32_t testLuT[gammaLuTSize] = {
//         0,     1,     5,     12,    23,    39,    59,    85,    115,   151,   192,   240,   293,   352,   417,
//         489,   567,   652,   744,   842,   948,   1060,  1180,  1307,  1441,  1583,  1733,  1890,  2055,  2228,
//         2408,  2597,  2794,  2998,  3212,  3433,  3663,  3901,  4148,  4403,  4667,  4940,  5221,  5512,  5811,
//         6119,  6437,  6763,  7099,  7443,  7797,  8161,  8534,  8916,  9307,  9709,  10119, 10540, 10970, 11410,
//         11860, 12319, 12789, 13268, 13757, 14257, 14766, 15286, 15816, 16356, 16906, 17467, 18038, 18619, 19211,
//         19813, 20426, 21050, 21684, 22329, 22984, 23650, 24327, 25015, 25714, 26423, 27144, 27875, 28617, 29371,
//         30135, 30911, 31698, 32496, 33305, 34126, 34958, 35801, 36656, 37522, 38399};

//     uint32_t expectedTop = (clockFreq / (testPWMFreq)) - 1U;

//     buildGammaLookUpTable(expectedTop);

//     TEST_ASSERT_EQUAL_MEMORY(testLuT, gammaLookUp, gammaLuTSize);
// }

////
// Setters And Getters
////

void test_setDutyCycle_SuccessBuffered(void) {
    CCChannel_t testChannel       = CC_CHANNEL_0;
    uint32_t percent              = 8000;
    uint32_t expectedCompareValue = brightnessLookUp[percent];

    // Set Expectations
    TIMHW_setT0ChannelOutputCompareBuffered_ExpectAndReturn(testChannel, expectedCompareValue, TIMER_OK);

    setDutyCycle(testChannel, percent, true);
}

void test_setDutyCycle_SuccessNonBuffered(void) {
    CCChannel_t testChannel       = CC_CHANNEL_0;
    uint32_t percent              = 8000;
    uint32_t expectedCompareValue = brightnessLookUp[percent];

    // Set Expectations
    TIMHW_setT0ChannelOutputCompare_ExpectAndReturn(testChannel, expectedCompareValue, TIMER_OK);

    setDutyCycle(testChannel, percent, false);
}