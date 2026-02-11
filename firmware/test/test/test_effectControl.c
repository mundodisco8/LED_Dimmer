#include <unity.h>

// StdLib libraries
#include <string.h>

// Our libraries
#include "effectControl.h"
#include "mock_PWMControl.h"
#include "mock_timer_HW.h"

/**
 * Extern bits to test
 */
extern uint16_t gausianBreatheLUT[BREATHE_LUT_SIZE];

// Instances of breathe effects for each channel
extern breatheControl_t breatheCh1;
extern breatheControl_t breatheCh2;
extern breatheControl_t breatheCh3;

// Default parameters of the breathe effect, in case none are provided
extern breatheParams_t defaultParams;

// Instances of LED Strips
extern LED_t LEDCh1;
extern LED_t LEDCh2;
extern LED_t LEDCh3;

extern void effectControl_ChangeBrightness(CCChannel_t channel);

/*
 * Unit tests for "effectControl.h"
 */
void setUp(void) {
    memset(gausianBreatheLUT, 0, sizeof(gausianBreatheLUT));
    // Expectation for initLEDStrips()
    getPWMFrequency_ExpectAndReturn(1000UL);
    getPWMFrequency_ExpectAndReturn(1000UL);
    getPWMFrequency_ExpectAndReturn(1000UL);
    initLEDStrips();
}

void tearDown(void) {}

/**
 * fillBreatheLUT
 * - Create a LUT with the passed values
 * - If NULL passed as pointer, use default params
 */

void test_fillBreatheLUT_passedParams(void) {
    breatheParams_t testParams = {.maxBrightness = 100, .minBrightness = 10, .beta = .8, .gamma = .2, .LUTSize = 20};

    // Got this values from the Jupiter Notebook on docs
    uint16_t expectedValues[20] = {1003, 1008, 1020, 1046, 1100, 1205, 1395,  1716, 2218, 2946,
                                   3922, 5121, 6459, 7794, 8942, 9723, 10000, 9723, 8942, 7794};

    fillBreatheEffectLUT(&testParams);

    TEST_ASSERT_EQUAL_UINT16_ARRAY(expectedValues, gausianBreatheLUT, testParams.LUTSize);
}

void test_fillBreatheLUT_passedNull(void) {
    uint16_t expectedValues[BREATHE_LUT_SIZE] = {0};

    fillBreatheEffectLUT(NULL);

    TEST_ASSERT_EQUAL_UINT16_ARRAY(expectedValues, gausianBreatheLUT, BREATHE_LUT_SIZE);
}

/**
 * Init LED Strips
 */

void test_initEDStripts_success(void) {
    efferr_t expectedRetVal = EFF_OK;

    // Expectations
    LED_t defaultLEDConfig = {.currAnimation = ANIM_FIXED,
                              .brightnessCtrl.targetBrightness = 5000UL,
                              .brightnessCtrl.delta = 0UL,
                              .brightnessCtrl.brightChangeRequestedFlag = true,
                              .pmwPeriodms = 1UL,
                              .breatheCtrl.currLUTIndex = 0UL,
                              .breatheCtrl.currWave = 0UL,
                              .breatheCtrl.periodms = 0UL,
                              .breatheCtrl.wavesPerSample = 0UL};

    // Expectations - Use 38400
    getPWMFrequency_ExpectAndReturn(1000);
    getPWMFrequency_ExpectAndReturn(1000);
    getPWMFrequency_ExpectAndReturn(1000);

    efferr_t retVal = initLEDStrips();

    TEST_ASSERT_EQUAL_UINT32(expectedRetVal, retVal);
    // NOTE: This fails because padding is not being initialised! WTF? Anyway, I think clearing the padding is insane
    // just for this test to pass, so I'm going to test individual members... grrrrr
    // TEST_ASSERT_EQUAL_MEMORY(&expectedLED1Ch, &LEDCh1, sizeof(LED_t));
    TEST_ASSERT_EQUAL_UINT32(defaultLEDConfig.currAnimation, LEDCh1.currAnimation);

    TEST_ASSERT_EQUAL_UINT32(defaultLEDConfig.brightnessCtrl.targetBrightness, LEDCh1.brightnessCtrl.targetBrightness);
    TEST_ASSERT_EQUAL_UINT32(defaultLEDConfig.brightnessCtrl.delta, LEDCh1.brightnessCtrl.delta);
    TEST_ASSERT_TRUE(LEDCh1.brightnessCtrl.brightChangeRequestedFlag);

    TEST_ASSERT_EQUAL_UINT32(defaultLEDConfig.pmwPeriodms, LEDCh1.pmwPeriodms);

    TEST_ASSERT_EQUAL_UINT32(defaultLEDConfig.breatheCtrl.currLUTIndex, LEDCh1.breatheCtrl.currLUTIndex);
    TEST_ASSERT_EQUAL_UINT32(defaultLEDConfig.breatheCtrl.currWave, LEDCh1.breatheCtrl.currWave);
    TEST_ASSERT_EQUAL_UINT32(defaultLEDConfig.breatheCtrl.periodms, LEDCh1.breatheCtrl.periodms);
    TEST_ASSERT_EQUAL_UINT32(defaultLEDConfig.breatheCtrl.wavesPerSample, LEDCh1.breatheCtrl.wavesPerSample);

    // Channel 2
    TEST_ASSERT_EQUAL_UINT32(defaultLEDConfig.currAnimation, LEDCh2.currAnimation);

    TEST_ASSERT_EQUAL_UINT32(defaultLEDConfig.brightnessCtrl.targetBrightness, LEDCh2.brightnessCtrl.targetBrightness);
    TEST_ASSERT_EQUAL_UINT32(defaultLEDConfig.brightnessCtrl.delta, LEDCh2.brightnessCtrl.delta);
    TEST_ASSERT_TRUE(LEDCh2.brightnessCtrl.brightChangeRequestedFlag);

    TEST_ASSERT_EQUAL_UINT32(defaultLEDConfig.pmwPeriodms, LEDCh2.pmwPeriodms);

    TEST_ASSERT_EQUAL_UINT32(defaultLEDConfig.breatheCtrl.currLUTIndex, LEDCh2.breatheCtrl.currLUTIndex);
    TEST_ASSERT_EQUAL_UINT32(defaultLEDConfig.breatheCtrl.currWave, LEDCh2.breatheCtrl.currWave);
    TEST_ASSERT_EQUAL_UINT32(defaultLEDConfig.breatheCtrl.periodms, LEDCh2.breatheCtrl.periodms);
    TEST_ASSERT_EQUAL_UINT32(defaultLEDConfig.breatheCtrl.wavesPerSample, LEDCh2.breatheCtrl.wavesPerSample);

    // Channel 3
    TEST_ASSERT_EQUAL_UINT32(defaultLEDConfig.currAnimation, LEDCh3.currAnimation);

    TEST_ASSERT_EQUAL_UINT32(defaultLEDConfig.brightnessCtrl.targetBrightness, LEDCh3.brightnessCtrl.targetBrightness);
    TEST_ASSERT_EQUAL_UINT32(defaultLEDConfig.brightnessCtrl.delta, LEDCh3.brightnessCtrl.delta);
    TEST_ASSERT_TRUE(LEDCh3.brightnessCtrl.brightChangeRequestedFlag);

    TEST_ASSERT_EQUAL_UINT32(defaultLEDConfig.pmwPeriodms, LEDCh3.pmwPeriodms);

    TEST_ASSERT_EQUAL_UINT32(defaultLEDConfig.breatheCtrl.currLUTIndex, LEDCh3.breatheCtrl.currLUTIndex);
    TEST_ASSERT_EQUAL_UINT32(defaultLEDConfig.breatheCtrl.currWave, LEDCh3.breatheCtrl.currWave);
    TEST_ASSERT_EQUAL_UINT32(defaultLEDConfig.breatheCtrl.periodms, LEDCh3.breatheCtrl.periodms);
    TEST_ASSERT_EQUAL_UINT32(defaultLEDConfig.breatheCtrl.wavesPerSample, LEDCh3.breatheCtrl.wavesPerSample);
}

/**
 * setLEDBrightness()
 * - Set the brightness for the channel
 * - Requested brightness is out of bounds
 */

void test_setLEDBrightness_Success(void) {
    LEDChannel_t testChannel = LED_CHANNEL_1;
    uint32_t testPercent = 7600;
    uint32_t expectedPercent = testPercent;

    efferr_t percentSet = setLEDBrightness(testChannel, testPercent);

    TEST_ASSERT_EQUAL_UINT32(expectedPercent, percentSet);
    TEST_ASSERT_EQUAL_UINT32(testPercent, LEDCh1.brightnessCtrl.targetBrightness);
    TEST_ASSERT_TRUE(LEDCh1.brightnessCtrl.brightChangeRequestedFlag);
}

void test_setLEDBrightness_BrightnessOverflow(void) {
    LEDChannel_t testChannel = LED_CHANNEL_1;
    uint32_t testPercent = 12000;
    uint32_t expectedPercent = 10000;

    uint32_t percentSet = setLEDBrightness(testChannel, testPercent);

    TEST_ASSERT_EQUAL_UINT32(expectedPercent, percentSet);
    TEST_ASSERT_EQUAL_UINT32(expectedPercent, LEDCh1.brightnessCtrl.targetBrightness);
    TEST_ASSERT_TRUE(LEDCh1.brightnessCtrl.brightChangeRequestedFlag);
}

void test_setLEDBrightness_BrightnessUnderflow(void) {
    LEDChannel_t testChannel = LED_CHANNEL_1;
    uint32_t testPercent = (uint32_t)-1000;
    uint32_t expectedPercent = 0;

    uint32_t percentSet = setLEDBrightness(testChannel, testPercent);

    TEST_ASSERT_EQUAL_UINT32(expectedPercent, percentSet);
    TEST_ASSERT_EQUAL_UINT32(expectedPercent, LEDCh1.brightnessCtrl.targetBrightness);
    TEST_ASSERT_TRUE(LEDCh1.brightnessCtrl.brightChangeRequestedFlag);
}

/**
 * getLEDBrightness()
 * - Success
 */

void test_getLEDBrightness_Success(void) {
    uint32_t expectedBrightness = 1234UL;  // default brightness
    CCChannel_t testChannel = LED_CHANNEL_1;
    // Prepare test
    LEDCh1.brightnessCtrl.targetBrightness = expectedBrightness;

    uint32_t retVal = getLEDBrightness(testChannel);

    TEST_ASSERT_EQUAL_UINT32(expectedBrightness, retVal);
}

/**
 * BreatheSetPeriod
 * - New period is set and
 * NOTE: Also tests getLEDStruct()
 */

void test_breatheSetPeriod_Succes(void) {
    efferr_t expectedRetVal = EFF_OK;

    // Set up
    uint32_t testNewPeriod = 1000;  // in ms

    uint32_t expectedPWMFreq = 1000;
    uint32_t expectedWavesPerSample = 5;  // 200 samples at 1ms per sample -> 1000ms / (200 * 1ms) = 5

    LEDCh1.pmwPeriodms = 1000 / expectedPWMFreq;

    efferr_t retVal = breatheSetPeriod(LED_CHANNEL_1, testNewPeriod);

    TEST_ASSERT_EQUAL_UINT32(expectedRetVal, retVal);
    TEST_ASSERT_EQUAL_UINT32(testNewPeriod, LEDCh1.breatheCtrl.periodms);
    TEST_ASSERT_EQUAL_UINT32(expectedWavesPerSample, LEDCh1.breatheCtrl.wavesPerSample);

    retVal = breatheSetPeriod(LED_CHANNEL_2, testNewPeriod);

    TEST_ASSERT_EQUAL_UINT32(expectedRetVal, retVal);
    TEST_ASSERT_EQUAL_UINT32(testNewPeriod, LEDCh1.breatheCtrl.periodms);
    TEST_ASSERT_EQUAL_UINT32(expectedWavesPerSample, LEDCh1.breatheCtrl.wavesPerSample);

    retVal = breatheSetPeriod(LED_CHANNEL_3, testNewPeriod);

    TEST_ASSERT_EQUAL_UINT32(expectedRetVal, retVal);
    TEST_ASSERT_EQUAL_UINT32(testNewPeriod, LEDCh1.breatheCtrl.periodms);
    TEST_ASSERT_EQUAL_UINT32(expectedWavesPerSample, LEDCh1.breatheCtrl.wavesPerSample);
}

void test_breatheSetPeriod_PeriodIs0(void) {
    efferr_t expectedRetVal = EFF_BADPERIOD;
    uint32_t testPeriod = 0;

    efferr_t retVal = breatheSetPeriod(LED_CHANNEL_1, testPeriod);

    TEST_ASSERT_EQUAL_UINT32(expectedRetVal, retVal);
}

void test_breatheSetPeriod_PeriodIsNotMultiplePWM(void) {
    // Set up
    efferr_t expectedRetVal = EFF_OK;
    uint32_t testPWMFreq = 100;  // 100Hz, period is 10ms
    // LUT size is 200
    uint32_t testPeriod = 10550;          // won't be a multiple of the PWM period: 10ms * 200 samples
    uint32_t expectedwavesPerSample = 5;  // waves per sample is rounded, in this case down

    LEDCh1.pmwPeriodms = 1000 / testPWMFreq;

    efferr_t retVal = breatheSetPeriod(LED_CHANNEL_1, testPeriod);

    TEST_ASSERT_EQUAL_UINT32(expectedRetVal, retVal);
    TEST_ASSERT_EQUAL_UINT32(testPeriod, LEDCh1.breatheCtrl.periodms);
    TEST_ASSERT_EQUAL_UINT32(expectedwavesPerSample, LEDCh1.breatheCtrl.wavesPerSample);

    // Number is always floored
    testPeriod = 11550;          // won't be a multiple of the PWM period: 10ms * 200 samples
    expectedwavesPerSample = 6;  // number must be rounded up this time

    retVal = breatheSetPeriod(LED_CHANNEL_1, testPeriod);

    TEST_ASSERT_EQUAL_UINT32(expectedRetVal, retVal);
    TEST_ASSERT_EQUAL_UINT32(testPeriod, LEDCh1.breatheCtrl.periodms);
    TEST_ASSERT_EQUAL_UINT32(expectedwavesPerSample, LEDCh1.breatheCtrl.wavesPerSample);
}

////
// Effect Control
//
// -
////

// This case tests that when the current brightness level is not the target level, the brightness is changed.
void test_effectControl_ChangeBrightness_IncreaseBrightness(void) {
    CCChannel_t expectedChannel = CC_CHANNEL_0;
    uint32_t testCurrentCompare = 0;
    // Instead of dealing with LUTs, set an arbitrary target compare  for an arbitrary target percent
    uint32_t expectedLEDTargetBrightness = 500;
    uint32_t expectedTargetCompare = 1000;
    uint32_t testPWMFreq = 250;  // 250 updates of brightness per second
    uint32_t expectedDelta = (expectedTargetCompare - testCurrentCompare) / testPWMFreq;
    uint32_t expectedCompareToSet = testCurrentCompare + expectedDelta;

    // Prepare test
    LEDCh1.brightnessCtrl.targetBrightness = expectedLEDTargetBrightness;
    LEDCh1.brightnessCtrl.brightChangeRequestedFlag = true;

    // Set Expectations
    TIMHW_getTimer0CompareValue_ExpectAndReturn(expectedChannel, testCurrentCompare);
    percentToCompare_ExpectAndReturn(expectedLEDTargetBrightness, expectedTargetCompare);
    getPWMFrequency_ExpectAndReturn(testPWMFreq);
    TIMHW_setT0ChannelOutputCompareBuffered_ExpectAndReturn(expectedChannel, expectedCompareToSet, TIMER_OK);

    effectControl_ChangeBrightness(CC_CHANNEL_0);

    // Paranoid test: check that it works when the compares are in the "high half"
    testCurrentCompare = UINT32_MAX - 1000;
    expectedTargetCompare = UINT32_MAX - 500;
    LEDCh1.brightnessCtrl.targetBrightness = expectedLEDTargetBrightness;
    LEDCh1.brightnessCtrl.brightChangeRequestedFlag = true;
    expectedDelta = (expectedTargetCompare - testCurrentCompare) / testPWMFreq;
    expectedCompareToSet = testCurrentCompare + expectedDelta;

    // Set Expectations
    TIMHW_getTimer0CompareValue_ExpectAndReturn(expectedChannel, testCurrentCompare);
    percentToCompare_ExpectAndReturn(expectedLEDTargetBrightness, expectedTargetCompare);
    getPWMFrequency_ExpectAndReturn(testPWMFreq);
    TIMHW_setT0ChannelOutputCompareBuffered_ExpectAndReturn(expectedChannel, expectedCompareToSet, TIMER_OK);

    effectControl_ChangeBrightness(CC_CHANNEL_0);
}

// This test the case where the brightness is decreased
void test_effectControl_ChangeBrightness_DecreaseBrightness(void) {
    CCChannel_t expectedChannel = CC_CHANNEL_0;
    uint32_t testCurrentCompare = 2000;
    // Instead of dealing with LUTs, set an arbitrary target compare  for an arbitrary target percent
    uint32_t expectedLEDTargetBrightness = 500;
    uint32_t expectedTargetCompare = 1000;
    uint32_t testPWMFreq = 250;  // 250 updates of brightness per second
    uint32_t expectedDelta = (testCurrentCompare - expectedTargetCompare) / testPWMFreq;
    uint32_t expectedCompareToSet = testCurrentCompare - expectedDelta;

    // Prepare test
    LEDCh1.brightnessCtrl.targetBrightness = expectedLEDTargetBrightness;
    LEDCh1.brightnessCtrl.brightChangeRequestedFlag = true;

    // Set Expectations
    TIMHW_getTimer0CompareValue_ExpectAndReturn(expectedChannel, testCurrentCompare);
    percentToCompare_ExpectAndReturn(expectedLEDTargetBrightness, expectedTargetCompare);
    getPWMFrequency_ExpectAndReturn(testPWMFreq);
    TIMHW_setT0ChannelOutputCompareBuffered_ExpectAndReturn(expectedChannel, expectedCompareToSet, TIMER_OK);

    effectControl_ChangeBrightness(CC_CHANNEL_0);

    // Paranoid test: check that it works when the compares are in the "high half"
    testCurrentCompare = UINT32_MAX - 500;
    expectedTargetCompare = UINT32_MAX - 1000;
    LEDCh1.brightnessCtrl.targetBrightness = expectedLEDTargetBrightness;
    LEDCh1.brightnessCtrl.brightChangeRequestedFlag = true;
    expectedDelta = (testCurrentCompare - expectedTargetCompare) / testPWMFreq;
    expectedCompareToSet = testCurrentCompare - expectedDelta;

    // Set Expectations
    TIMHW_getTimer0CompareValue_ExpectAndReturn(expectedChannel, testCurrentCompare);
    percentToCompare_ExpectAndReturn(expectedLEDTargetBrightness, expectedTargetCompare);
    getPWMFrequency_ExpectAndReturn(testPWMFreq);
    TIMHW_setT0ChannelOutputCompareBuffered_ExpectAndReturn(expectedChannel, expectedCompareToSet, TIMER_OK);

    effectControl_ChangeBrightness(CC_CHANNEL_0);
}

// This test the case where the brightness is decreased
void test_effectControl_ChangeBrightness_DoNothingIfNoChangesRequired(void) {
    CCChannel_t expectedChannel = CC_CHANNEL_0;
    uint32_t testCurrentCompare = 2000;
    // Instead of dealing with LUTs, set an arbitrary target compare  for an arbitrary target percent
    uint32_t expectedLEDTargetBrightness = 500;
    uint32_t expectedTargetCompare = 2000;
    uint32_t testPWMFreq = 250;  // 250 updates of brightness per second

    // Prepare test
    LEDCh1.brightnessCtrl.targetBrightness = expectedLEDTargetBrightness;

    // Set Expectations
    TIMHW_getTimer0CompareValue_ExpectAndReturn(expectedChannel, testCurrentCompare);
    percentToCompare_ExpectAndReturn(expectedLEDTargetBrightness, expectedTargetCompare);
    getPWMFrequency_ExpectAndReturn(testPWMFreq);
    // NO expectation of SetCompare() of any type

    effectControl_ChangeBrightness(CC_CHANNEL_0);
}

// Test the case where the change in brightness is lesser than the delta
// This case tests that when the current brightness level is not the target level, the brightness is changed.
void test_effectControl_ChangeBrightness_IncreaseBrightnessLessThanDelta(void) {
    CCChannel_t expectedChannel = CC_CHANNEL_0;
    uint32_t testCurrentCompare = 998;
    // Instead of dealing with LUTs, set an arbitrary target compare  for an arbitrary target percent
    uint32_t expectedLEDTargetBrightness = 500;
    uint32_t expectedTargetCompare = 1000;
    uint32_t testPWMFreq = 1;  // set freq to 1 so we jump from current to target in one update
    uint32_t expectedCompareToSet = expectedTargetCompare;

    // Prepare test
    LEDCh1.brightnessCtrl.targetBrightness = expectedLEDTargetBrightness;

    // Set Expectations
    TIMHW_getTimer0CompareValue_ExpectAndReturn(expectedChannel, testCurrentCompare);
    percentToCompare_ExpectAndReturn(expectedLEDTargetBrightness, expectedTargetCompare);
    getPWMFrequency_ExpectAndReturn(testPWMFreq);
    TIMHW_setT0ChannelOutputCompareBuffered_ExpectAndReturn(expectedChannel, expectedCompareToSet, TIMER_OK);

    effectControl_ChangeBrightness(CC_CHANNEL_0);
}

// Test the case where the change in brightness is lesser than the delta
// This case tests that when the current brightness level is not the target level, the brightness is changed.
void test_effectControl_ChangeBrightness_DecreaseBrightnessLessThanDelta(void) {
    CCChannel_t expectedChannel = CC_CHANNEL_0;
    uint32_t testCurrentCompare = 1005;
    // Instead of dealing with LUTs, set an arbitrary target compare  for an arbitrary target percent
    uint32_t expectedLEDTargetBrightness = 500;
    uint32_t expectedTargetCompare = 1000;
    uint32_t testPWMFreq = 1;  // set freq to 1 so we jump from current to target in one update
    uint32_t expectedCompareToSet = expectedTargetCompare;

    // Prepare test
    LEDCh1.brightnessCtrl.targetBrightness = expectedLEDTargetBrightness;

    // Set Expectations
    TIMHW_getTimer0CompareValue_ExpectAndReturn(expectedChannel, testCurrentCompare);
    percentToCompare_ExpectAndReturn(expectedLEDTargetBrightness, expectedTargetCompare);
    getPWMFrequency_ExpectAndReturn(testPWMFreq);
    TIMHW_setT0ChannelOutputCompareBuffered_ExpectAndReturn(expectedChannel, expectedCompareToSet, TIMER_OK);

    effectControl_ChangeBrightness(CC_CHANNEL_0);
}

// Tests that, if the freq is too high for the difference, we step at the smallest delta, which is 1
void test_effectControl_ChangeBrightness_DeltaCantBe0(void) {
    CCChannel_t expectedChannel = CC_CHANNEL_0;
    uint32_t testCurrentCompare = 0;
    // Instead of dealing with LUTs, set an arbitrary target compare  for an arbitrary target percent
    uint32_t expectedLEDTargetBrightness = 500;
    uint32_t expectedTargetCompare = 1000;
    uint32_t testPWMFreq = 10000;  // 10000 updates of brightness per second, way more than leves of difference
    uint32_t expectedDelta = 1;
    uint32_t expectedCompareToSet = testCurrentCompare + expectedDelta;

    // Prepare test
    LEDCh1.brightnessCtrl.targetBrightness = expectedLEDTargetBrightness;

    // Set Expectations
    TIMHW_getTimer0CompareValue_ExpectAndReturn(expectedChannel, testCurrentCompare);
    percentToCompare_ExpectAndReturn(expectedLEDTargetBrightness, expectedTargetCompare);
    getPWMFrequency_ExpectAndReturn(testPWMFreq);
    TIMHW_setT0ChannelOutputCompareBuffered_ExpectAndReturn(expectedChannel, expectedCompareToSet, TIMER_OK);

    effectControl_ChangeBrightness(CC_CHANNEL_0);
}

// Check that, if brightness is changed AGAIN while we are triggering a change, the delta changes accordingly so the
// target brightness is still reached a second after the last change (not the first change)
void test_effectControl_ChangeBrightness_ChangeDuringChange(void) {
    // First Change - Sets the delta to 4, so we do 250 updates at 4, reaching from 0 to 1000 in 250 updates
    CCChannel_t expectedChannel = CC_CHANNEL_0;
    uint32_t testCurrentCompare = 0;
    // Instead of dealing with LUTs, set an arbitrary target compare  for an arbitrary target percent
    uint32_t expectedLEDTargetBrightness = 500;
    uint32_t expectedTargetCompare = 1000;
    uint32_t testPWMFreq = 250;  // 250 updates of brightness per second
    uint32_t expectedDelta = (expectedTargetCompare - testCurrentCompare) / testPWMFreq;
    uint32_t expectedCompareToSet = testCurrentCompare + expectedDelta;

    // Prepare test
    setLEDBrightness(LED_CHANNEL_1, expectedLEDTargetBrightness);

    // Set Expectations
    TIMHW_getTimer0CompareValue_ExpectAndReturn(expectedChannel, testCurrentCompare);
    percentToCompare_ExpectAndReturn(expectedLEDTargetBrightness, expectedTargetCompare);
    getPWMFrequency_ExpectAndReturn(testPWMFreq);
    TIMHW_setT0ChannelOutputCompareBuffered_ExpectAndReturn(expectedChannel, expectedCompareToSet, TIMER_OK);

    effectControl_ChangeBrightness(CC_CHANNEL_0);

    // At this point, COMPARE is at 4
    testCurrentCompare += expectedDelta;
    expectedCompareToSet = testCurrentCompare + expectedDelta;

    // Set Expectations
    TIMHW_getTimer0CompareValue_ExpectAndReturn(expectedChannel, testCurrentCompare);
    percentToCompare_ExpectAndReturn(expectedLEDTargetBrightness, expectedTargetCompare);
    // getPWMFrequency_ExpectAndReturn(testPWMFreq);
    TIMHW_setT0ChannelOutputCompareBuffered_ExpectAndReturn(expectedChannel, expectedCompareToSet, TIMER_OK);

    effectControl_ChangeBrightness(CC_CHANNEL_0);

    // At this point, COMPARE is at 8

    // New level of brightness, 100008
    testCurrentCompare += expectedDelta;
    uint32_t newTargetBrightness = 1000;  // just some value
    expectedTargetCompare =
        10008;  // Made up compare value for new target brightness, throw a 4 to make delta a round number
    setLEDBrightness(LED_CHANNEL_1, newTargetBrightness);
    // Delta should be 10k4 - 4 / 250 = 40
    expectedDelta = (expectedTargetCompare - 4) / testPWMFreq;
    expectedCompareToSet = testCurrentCompare + expectedDelta;  // 48

    // Set Expectations
    TIMHW_getTimer0CompareValue_ExpectAndReturn(expectedChannel, testCurrentCompare);
    percentToCompare_ExpectAndReturn(newTargetBrightness, expectedTargetCompare);
    getPWMFrequency_ExpectAndReturn(testPWMFreq);
    TIMHW_setT0ChannelOutputCompareBuffered_ExpectAndReturn(expectedChannel, expectedCompareToSet, TIMER_OK);

    effectControl_ChangeBrightness(CC_CHANNEL_0);
}