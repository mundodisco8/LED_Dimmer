#include <string.h>
#include <unity.h>

#include "PWMControl.h"
#include "mock_timer_HW.h"
/*
 * Unit tests for "PWMControl.h"
 */

extern volatile uint32_t dutyCycle[3];
extern uint32_t gammaLookUp[];

////
// Static bits
////
extern void buildGammaLookUpTable(void);
extern void configureTimerPWMFrequency(uint32_t frequencyHz);

void setUp(void) { memset((uint8_t*)gammaLookUp, 0x00, 101); }

void tearDown(void) {}

////
// initTimer0HW and initTimer0CCChannel
////

// This is just a choreography, check functions are named in the right order

void test_initTimer0PWM_Success(void) {
    uint32_t testClockFreq = 32000000;
    uint32_t testFreq = 250;
    uint32_t ExpectedTimerTopValue = (testClockFreq / testFreq) - 1;
    TIMHW_initTimer0Clock_Expect();
    TIMHW_initTimer0_Expect(false);
    TIMHW_getTimer0Frequency_ExpectAndReturn(testClockFreq);
    TIMHW_setTimer0TopValue_ExpectAndReturn(ExpectedTimerTopValue, TIMER_OK);
    TIMHW_getTimer0TopValue_ExpectAndReturn(ExpectedTimerTopValue);

    initTimer0PWM(testFreq);
}

void test_initTimer0CCChannel(void) {
    CCChannel_t testChannel = CC_CHANNEL_0;
    polarity_t testPol = PWM_ACTIVE_HIGH;
    pinPort_t testPort = portB;
    uint32_t testPinNo = 3;

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
    uint32_t testFreq = 6249;
    TIMHW_getTimer0Frequency_ExpectAndReturn(clockFreq);
    // There's no way to test the freq was correct, except checking that TOP is set to the right value
    uint32_t expectedTop = (clockFreq / (testFreq)) - 1U;
    TIMHW_setTimer0TopValue_ExpectAndReturn(expectedTop, TIMER_OK);
    TIMHW_getTimer0TopValue_IgnoreAndReturn(1000);  // expectation for buildGammaLookUpTable

    configureTimerPWMFrequency(testFreq);
}

void test_configureTimerPWMFrequency_Freq_is_less_than_MIN_PWM_FREQ(void) {
    uint32_t clockFreq = 38400000;
    uint32_t testFreq = 0;
    TIMHW_getTimer0Frequency_ExpectAndReturn(clockFreq);
    // Requested freq is too low, set TOP to get the minimum freq of 250Hz
    uint32_t expectedTop = (clockFreq / (MIN_PWM_FREQ)) - 1U;
    TIMHW_setTimer0TopValue_ExpectAndReturn(expectedTop, TIMER_OK);
    TIMHW_getTimer0TopValue_IgnoreAndReturn(1000);  // expectation for buildGammaLookUpTable

    configureTimerPWMFrequency(testFreq);
}

void test_configureTimerPWMFrequency_Freq_is_too_high(void) {
    uint32_t clockFreq = 38400000;
    uint32_t testFreq = 38400000;  // would give 1 level of quantization: 0 or 100%
    TIMHW_getTimer0Frequency_ExpectAndReturn(clockFreq);
    // requested freq is too hihg, Set the TOP value for the fastest signal achievable with 4096 levels
    uint32_t expectedTop = MIN_PWM_LEVELS - 1;
    TIMHW_setTimer0TopValue_ExpectAndReturn(expectedTop, TIMER_OK);
    TIMHW_getTimer0TopValue_IgnoreAndReturn(1000);  // expectation for buildGammaLookUpTable

    configureTimerPWMFrequency(testFreq);
}

////
// getPWMFrequency
//
// - Just check the correct functions are called and return the right value
////

void test_getPWMFrequency(void) {
    uint32_t expectedTimerClock = 1000;
    uint32_t expectedTop = 10;
    uint32_t expectedFreq = 100;

    // expectations
    TIMHW_getTimer0Frequency_ExpectAndReturn(expectedTimerClock);
    TIMHW_getTimer0TopValue_ExpectAndReturn(expectedTop);

    uint32_t retVal = getPWMFrequency();

    TEST_ASSERT_EQUAL_UINT32(expectedFreq, retVal);
}

////
// buildGammaLookUpTable()
////

extern const uint32_t GAMMA_VALUE;
#define gammaLuTSize 101
void test_buildGammaLookUpTable_checkTable(void) {
    uint32_t clockFreq = 38400000;
    uint32_t testPWMFreq = 1000;

    // Got this table from the Jupyter notebook in /docs
    uint32_t testLuT[gammaLuTSize] = {
        0,     1,     5,     12,    23,    39,    59,    85,    115,   151,   192,   240,   293,   352,   417,
        489,   567,   652,   744,   842,   948,   1060,  1180,  1307,  1441,  1583,  1733,  1890,  2055,  2228,
        2408,  2597,  2794,  2998,  3212,  3433,  3663,  3901,  4148,  4403,  4667,  4940,  5221,  5512,  5811,
        6119,  6437,  6763,  7099,  7443,  7797,  8161,  8534,  8916,  9307,  9709,  10119, 10540, 10970, 11410,
        11860, 12319, 12789, 13268, 13757, 14257, 14766, 15286, 15816, 16356, 16906, 17467, 18038, 18619, 19211,
        19813, 20426, 21050, 21684, 22329, 22984, 23650, 24327, 25015, 25714, 26423, 27144, 27875, 28617, 29371,
        30135, 30911, 31698, 32496, 33305, 34126, 34958, 35801, 36656, 37522, 38399};

    uint32_t expectedTop = (clockFreq / (testPWMFreq)) - 1U;

    // expectation for buildGammaLookUpTable
    TIMHW_getTimer0TopValue_ExpectAndReturn(expectedTop);

    buildGammaLookUpTable();

    TEST_ASSERT_EQUAL_MEMORY(testLuT, gammaLookUp, gammaLuTSize);
}

////
// Setters And Getters
////

void test_percentToCompare_DoesItsThingIntegerPercent(void) {
    uint32_t percent = 8000;
    // We could build a look up table with buildGammaLookUpTable(), but we are testing! we can cheat!
    uint32_t expectedCompareValue = 100;  // why not? it's a number

    // Make sure the expectedCompareValue is positioned in the look up table in the correct place
    gammaLookUp[(percent / 100)] = expectedCompareValue;

    uint32_t compareValue = percentToCompare(percent);
    TEST_ASSERT_EQUAL_UINT32(expectedCompareValue, compareValue);
}

void test_percentToCompare_Interpolates(void) {
    uint32_t percent = 8050;
    // We could build a look up table with buildGammaLookUpTable(), but we are testing! we can cheat!
    uint32_t expectedCompareValue80 = 100;  // why not? it's a number
    uint32_t expectedCompareValue81 = 120;
    uint32_t expectedInterpolatedCompare = 110;  // halfway between the two

    // Make sure the expectedCompareValue is positioned in the look up table in the correct place
    gammaLookUp[(percent / 100)] = expectedCompareValue80;
    gammaLookUp[(percent / 100) + 1] = expectedCompareValue81;

    uint32_t compareValue = percentToCompare(percent);
    TEST_ASSERT_EQUAL_UINT32(expectedInterpolatedCompare, compareValue);
}

void test_percentToCompare_PercentIsOver100(void) {
    uint32_t percent = 12000;  // 120%

    uint32_t expectedCompare = 1234;  // halfway between the two

    // Make sure the expectedCompareValue is positioned in the look up table in the correct place
    gammaLookUp[100] = expectedCompare;

    uint32_t compareValue = percentToCompare(percent);
    TEST_ASSERT_EQUAL_UINT32(expectedCompare, compareValue);
}

void test_setDutyCycle_SuccessBuffered(void) {
    CCChannel_t testChannel = CC_CHANNEL_0;
    uint32_t percent = 8000;
    // We could build a look up table with buildGammaLookUpTable(), but we are testing! we can cheat!
    uint32_t expectedCompareValue = 100;  // why not? it's a number

    // Make sure the expectedCompareValue is positioned in the look up table in the correct place
    gammaLookUp[(percent / 100)] = expectedCompareValue;

    // Set Expectations
    TIMHW_setT0ChannelOutputCompareBuffered_ExpectAndReturn(testChannel, expectedCompareValue, TIMER_OK);

    setDutyCycle(testChannel, percent, true);
}

void test_setDutyCycle_SuccessNonBuffered(void) {
    CCChannel_t testChannel = CC_CHANNEL_0;
    uint32_t percent = 8000;
    // We could build a look up table with buildGammaLookUpTable(), but we are testing! we can cheat!
    uint32_t expectedCompareValue = 100;  // why not? it's a number

    // Make sure the expectedCompareValue is positioned in the look up table in the correct place
    gammaLookUp[(percent / 100)] = expectedCompareValue;

    // Set Expectations
    TIMHW_setT0ChannelOutputCompare_ExpectAndReturn(testChannel, expectedCompareValue, TIMER_OK);

    setDutyCycle(testChannel, percent, false);
}