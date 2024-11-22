#include "PWMControl.h"

#include <unity.h>

#include "mock_timer_HW.h"
/*
 * Unit tests for "PWMControl.h"
 */

// Need these, as they are externed to a module that is n
volatile float dutyCycle0;
volatile float dutyCycle1;
volatile float dutyCycle2;

void setUp(void) {
}

void tearDown(void) {
}

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
    TIMHW_getTimer0TopValue_IgnoreAndReturn(1000); // expectation for buildGammaLookUpTable

    configureTimerPWMFrequency(testFreq);
}

void test_configureTimerPWMFrequency_Freq_is_less_than_MIN_PWM_FREQ(void) {
    uint32_t clockFreq = 38400000;
    uint32_t testFreq = 0;
    TIMHW_getTimer0Frequency_ExpectAndReturn(clockFreq);
    // Requested freq is too low, set TOP to get the minimum freq of 250Hz
    uint32_t expectedTop = (clockFreq / (MIN_PWM_FREQ)) - 1U;
    TIMHW_setTimer0TopValue_ExpectAndReturn(expectedTop, TIMER_OK);
    TIMHW_getTimer0TopValue_IgnoreAndReturn(1000); // expectation for buildGammaLookUpTable

    configureTimerPWMFrequency(testFreq);
}

void test_configureTimerPWMFrequency_Freq_is_too_high(void) {
    uint32_t clockFreq = 38400000;
    uint32_t testFreq = 38400000; // would give 1 level of quantization: 0 or 100%
    TIMHW_getTimer0Frequency_ExpectAndReturn(clockFreq);
    // requested freq is too hihg, Set the TOP value for the fastest signal achievable with 4096 levels
    uint32_t expectedTop = MIN_PWM_LEVELS - 1;
    TIMHW_setTimer0TopValue_ExpectAndReturn(expectedTop, TIMER_OK);
    TIMHW_getTimer0TopValue_IgnoreAndReturn(1000); // expectation for buildGammaLookUpTable

    configureTimerPWMFrequency(testFreq);
}

////
// buildGammaLookUpTable()
////

// It's static, so it can only be tested through configureTimerPWMFrequency()
// There isn't much to test here, as the gamma formula it's what it is
#define gammaLuTSize 101
extern uint32_t gammaLookUp[gammaLuTSize];
extern const uint32_t GAMMA_VALUE;

void test_buildGammaLookUpTable_checkTable(void){
    uint32_t clockFreq = 38400000;
    uint32_t testFreq = 250;

    uint32_t testLuT[gammaLuTSize] = {0};
    // Build the expected gammaLuT for this freq
    for (uint32_t i = 0; i < gammaLuTSize; i++) {
        testLuT[i] = (uint32_t)((pow(i, GAMMA_VALUE) / pow (gammaLuTSize, GAMMA_VALUE)) * 153599 + .5);
    }

    // Expectations for configurePWMFrequency
    TIMHW_getTimer0Frequency_ExpectAndReturn(clockFreq);
    // Requested freq is too low, set TOP to get the minimum freq of 250Hz
    uint32_t expectedTop = (clockFreq / (MIN_PWM_FREQ)) - 1U;
    TIMHW_setTimer0TopValue_ExpectAndReturn(expectedTop, TIMER_OK);

    // expectation for buildGammaLookUpTable
    TIMHW_getTimer0TopValue_ExpectAndReturn(expectedTop);

    configureTimerPWMFrequency(testFreq);

    TEST_ASSERT_EQUAL_MEMORY(testLuT, gammaLookUp, gammaLuTSize);
}

void test_setDutyCycle_DoesItsThing(void) {
    CCChannel_t testChannel = CC_CHANNEL_0;
    uint8_t percent = 80;
    uint32_t topValue = 4096;
    uint32_t expectedCompareValue = topValue * percent / 100;

    TIMHW_getTimer0TopValue_ExpectAndReturn(topValue);
    TIMHW_setT0ChannelBufferedOutputCompare_ExpectAndReturn(testChannel, expectedCompareValue, TIMER_OK);

    setDutyCycle(testChannel, percent);
}

void test_setDutyCycle_PercentIsOver100(void) {
    CCChannel_t testChannel = CC_CHANNEL_0;
    int8_t percent = 120;
    uint32_t topValue = 4096UL;
    uint32_t expectedCompareValue = topValue;

    TIMHW_getTimer0TopValue_ExpectAndReturn(topValue);
    // TODO: review
    TIMHW_setT0ChannelBufferedOutputCompare_ExpectAndReturn(testChannel, expectedCompareValue, TIMER_OK);

    setDutyCycle(testChannel, percent);
}

void test_setBrightness_DoesItsThing(void) {
    CCChannel_t testChannel = CC_CHANNEL_0;
    uint8_t percent = 80;
    uint32_t expectedCompareValue = gammaLookUp[percent];

    TIMHW_setT0ChannelBufferedOutputCompare_ExpectAndReturn(testChannel, expectedCompareValue, TIMER_OK);

    setBrightness(testChannel, percent);
}

void test_setBrightness_PercentIsOver100(void) {
    CCChannel_t testChannel = CC_CHANNEL_0;
    uint8_t percent = 120;
    uint32_t topValue = 4097;
    uint32_t expectedCompareValue = topValue;

    TIMHW_getTimer0TopValue_ExpectAndReturn(topValue);
    TIMHW_setT0ChannelBufferedOutputCompare_ExpectAndReturn(testChannel, expectedCompareValue, TIMER_OK);

    setBrightness(testChannel, percent);
}