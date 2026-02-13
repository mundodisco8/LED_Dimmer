#include <unity.h>

// StdLib libraries
#include <string.h>

// Our libraries
#include "effectControl.h"
#include "mock_PWMControl.h"

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
 * NOTE: Also tests getLEDStruct()
 */

void test_getLEDBrightness_Success(void) {
    uint32_t expectedBrightness = 1234UL;  // default brightness
    LEDChannel_t testChannel = LED_CHANNEL_1;
    // Prepare test
    setLEDBrightness(testChannel, expectedBrightness);

    uint32_t retVal = getLEDBrightness(testChannel);

    TEST_ASSERT_EQUAL_UINT32(expectedBrightness, retVal);

    testChannel = LED_CHANNEL_2;
    uint32_t expectedCh2Brightness = 1000;
    setLEDBrightness(testChannel, expectedCh2Brightness);

    retVal = getLEDBrightness(testChannel);

    TEST_ASSERT_EQUAL_UINT32(expectedCh2Brightness, retVal);

    testChannel = LED_CHANNEL_3;
    uint32_t expectedCh3Brightness = 1500;
    setLEDBrightness(testChannel, expectedCh3Brightness);

    retVal = getLEDBrightness(testChannel);

    TEST_ASSERT_EQUAL_UINT32(expectedCh3Brightness, retVal);
}

/**
 * BreatheSetPeriod
 * - New period is set and
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
    uint32_t testCurrentBrightness = 0;
    // Instead of dealing with LUTs, set an arbitrary target compare  for an arbitrary target percent
    uint32_t testTargetBrightness = 5000;
    uint32_t testPWMFreq = 250;  // 250 updates of brightness per second
    uint32_t expectedDelta = (testTargetBrightness - testCurrentBrightness) / testPWMFreq;
    uint32_t expectedPercentToSet = testCurrentBrightness + expectedDelta;

    // Prepare test
    LEDCh1.brightnessCtrl.currentBrightness = testCurrentBrightness;
    setLEDBrightness(LED_CHANNEL_1, testTargetBrightness);

    // Set Expectations
    getPWMFrequency_ExpectAndReturn(testPWMFreq);
    setDutyCycle_Expect(expectedChannel, expectedPercentToSet, true);

    effectControl_ChangeBrightness(expectedChannel);
}

// This test the case where the brightness is decreased
void test_effectControl_ChangeBrightness_DecreaseBrightness(void) {
    CCChannel_t expectedChannel = CC_CHANNEL_0;
    uint32_t testCurrentBrightness = 8000;
    // Instead of dealing with LUTs, set an arbitrary target compare  for an arbitrary target percent
    uint32_t testTargetBrightness = 4000;
    uint32_t testPWMFreq = 250;  // 250 updates of brightness per second
    uint32_t expectedDelta = (testCurrentBrightness - testTargetBrightness) / testPWMFreq;
    uint32_t expectedPercentToSet = testCurrentBrightness - expectedDelta;

    // Prepare test
    LEDCh1.brightnessCtrl.currentBrightness = testCurrentBrightness;
    setLEDBrightness(LED_CHANNEL_1, testTargetBrightness);

    // Set Expectations
    getPWMFrequency_ExpectAndReturn(testPWMFreq);
    setDutyCycle_Expect(expectedChannel, expectedPercentToSet, true);

    effectControl_ChangeBrightness(expectedChannel);
}

// This test the case where the brightness is decreased
void test_effectControl_ChangeBrightness_DoNothingIfNoChangesRequired(void) {
    CCChannel_t expectedChannel = CC_CHANNEL_0;
    uint32_t testCurrentBrightness = 2000;
    // Instead of dealing with LUTs, set an arbitrary target compare  for an arbitrary target percent
    uint32_t testTargetBrightness = 2000;
    uint32_t testPWMFreq = 250;  // 250 updates of brightness per second

    // Prepare test
    LEDCh1.brightnessCtrl.currentBrightness = testCurrentBrightness;
    setLEDBrightness(LED_CHANNEL_1, testTargetBrightness);

    effectControl_ChangeBrightness(expectedChannel);
}

// Test the case where the change in brightness is lesser than the delta. Normally the last update before getting to the
// target
void test_effectControl_ChangeBrightness_IncreaseBrightnessLessThanDelta(void) {
    CCChannel_t expectedChannel = CC_CHANNEL_0;

    uint32_t testCurrentBrightness = 998;
    uint32_t testTargetBrightness = 1000;
    uint32_t testDelta = 100;  // delta is way more than the required change in brightness

    uint32_t expectedPercentToSet = testTargetBrightness;

    // Prepare test
    LEDCh1.brightnessCtrl.currentBrightness = testCurrentBrightness;
    LEDCh1.brightnessCtrl.targetBrightness = testTargetBrightness;
    LEDCh1.brightnessCtrl.delta = testDelta;
    LEDCh1.brightnessCtrl.brightChangeRequestedFlag = false;  // make sure we don't recalculate the delta!

    // Set Expectations
    setDutyCycle_Expect(expectedChannel, expectedPercentToSet, true);

    effectControl_ChangeBrightness(expectedChannel);
}

// Tests that, if the freq is too high for the difference, we step at the smallest delta, which is 1
void test_effectControl_ChangeBrightness_DeltaCantBe0(void) {
    CCChannel_t expectedChannel = CC_CHANNEL_0;
    uint32_t testCurrentBrightness = 0;
    uint32_t testTargetBrightness = 1000;
    uint32_t testPWMFreq = 10000;  // 10000 updates of brightness per second, way more than leves of difference
    uint32_t expectedDelta = 1;
    uint32_t expectedPercentToSet = testCurrentBrightness + expectedDelta;

    // Prepare test
    LEDCh1.brightnessCtrl.currentBrightness = testCurrentBrightness;
    setLEDBrightness(LED_CHANNEL_1, testTargetBrightness);

    // Set Expectations
    getPWMFrequency_ExpectAndReturn(testPWMFreq);
    setDutyCycle_Expect(expectedChannel, expectedPercentToSet, true);

    effectControl_ChangeBrightness(CC_CHANNEL_0);
}

// Check that, if brightness is changed AGAIN while we are triggering a change, the delta changes accordingly so the
// target brightness is still reached a second after the last change (not the first change)
void test_effectControl_ChangeBrightness_ChangeDuringChange(void) {
    // First Change - Sets the delta to 4, so we do 250 updates at 4, reaching from 0 to 1000 in 250 updates
    CCChannel_t expectedChannel = CC_CHANNEL_0;
    uint32_t testCurrentBrightness = 0;
    uint32_t testTargetBrightness = 500;
    uint32_t testPWMFreq = 250;  // 250 updates of brightness per second
    uint32_t expectedDelta = (testTargetBrightness - testCurrentBrightness) / testPWMFreq;  // 500 / 250 = 4
    uint32_t expectedPercentToSet = testCurrentBrightness + expectedDelta;

    // Prepare test
    LEDCh1.brightnessCtrl.currentBrightness = testCurrentBrightness;
    setLEDBrightness(LED_CHANNEL_1, testTargetBrightness);

    // Set Expectations
    getPWMFrequency_ExpectAndReturn(testPWMFreq);
    setDutyCycle_Expect(expectedChannel, expectedPercentToSet, true);

    effectControl_ChangeBrightness(expectedChannel);

    // At this point, currentBrightness is at 4, delta remains at 4
    expectedPercentToSet = LEDCh1.brightnessCtrl.currentBrightness + expectedDelta;

    // Set Expectations
    setDutyCycle_Expect(expectedChannel, expectedPercentToSet, true);

    effectControl_ChangeBrightness(expectedChannel);

    // At this point, currentBrightness is at 8
    // New level of brightness, 8008, delta must change
    uint32_t newTargetBrightness = 8008;  // the 8 at the end is just so delta is a round number
    setLEDBrightness(LED_CHANNEL_1, newTargetBrightness);
    // Delta should be 8008 - 8 / 250 = 32
    expectedDelta = (newTargetBrightness - LEDCh1.brightnessCtrl.currentBrightness) / testPWMFreq;
    expectedPercentToSet = LEDCh1.brightnessCtrl.currentBrightness + expectedDelta;

    // Set Expectations
    getPWMFrequency_ExpectAndReturn(testPWMFreq);
    setDutyCycle_Expect(expectedChannel, expectedPercentToSet, true);

    effectControl_ChangeBrightness(expectedChannel);
}

// This just increases coverage by testing the rest of the channels
void test_effectControl_ChangeBrightness_CoverageTestAllChannels(void) {
    // Set target and current to same value so _ChangeBrightness leaves at the earliest
    uint32_t testCurrentBrightness = 500;
    uint32_t testTargetBrightness = 500;

    // Prepare test
    LEDCh2.brightnessCtrl.currentBrightness = testCurrentBrightness;
    setLEDBrightness(LED_CHANNEL_2, testTargetBrightness);

    effectControl_ChangeBrightness(LED_CHANNEL_2);

    // Prepare test
    LEDCh3.brightnessCtrl.currentBrightness = testCurrentBrightness;
    setLEDBrightness(LED_CHANNEL_3, testTargetBrightness);

    effectControl_ChangeBrightness(LED_CHANNEL_3);
}