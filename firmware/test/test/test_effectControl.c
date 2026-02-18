#include <unity.h>

// StdLib librarie
#include <string.h>

// Silabs libs
#include "app_assert.h"

// Our libraries
#include "effectControl.h"
#include "mock_PWMControl.h"

/**
 * Extern bits to test
 */
extern uint32_t gausianBreatheLUT[BREATHE_LUT_SIZE];

// Instances of breathe effects for each channel
extern breatheControl_t breatheCh1;
extern breatheControl_t breatheCh2;
extern breatheControl_t breatheCh3;

// Default parameters of the breathe effect, in case none are provided
extern breatheParams_t defaultParams;
extern const uint32_t BREATHE_DEFAULT_PERIOD_MS;

// Instances of LED Strips
extern LED_t LEDCh1;
extern LED_t LEDCh2;
extern LED_t LEDCh3;

extern void effectControl_FadeBrightness(CCChannel_t channel);
extern void effectControl_Breathe(CCChannel_t channel);

/*
 * Unit tests for "effectControl.h"
 */
void setUp(void) {
    // Enable testable asserts
    assertSetUp();

    memset(gausianBreatheLUT, 0, sizeof(gausianBreatheLUT));
    fillBreatheEffectLUT(&DEFAULT_BREATHE_PARAMS);
    // Expectation for initLEDStrips()
    getPWMFrequency_ExpectAndReturn(1000UL);
    getPWMFrequency_ExpectAndReturn(1000UL);
    getPWMFrequency_ExpectAndReturn(1000UL);
    initLEDStrips();
}

void tearDown(void) { assertTearDownCheck(); }

/**
 * fillBreatheLUT
 * - Create a LUT with the passed values
 * - If NULL passed as pointer, use default params
 */

void test_fillBreatheLUT_passedParams(void) {
    breatheParams_t testParams = {
        .maxBrightness = 10000, .minBrightness = 1000, .beta = .8, .gamma = .2, .LUTSize = 20};

    // Got this values from the Jupiter Notebook on docs
    uint32_t expectedValues[20] = {1003, 1008, 1020, 1046, 1100, 1205, 1395,  1716, 2218, 2946,
                                   3922, 5121, 6459, 7794, 8942, 9723, 10000, 9723, 8942, 7794};

    fillBreatheEffectLUT(&testParams);

    TEST_ASSERT_EQUAL_UINT32_ARRAY(expectedValues, gausianBreatheLUT, testParams.LUTSize);
}

void test_fillBreatheLUT_passedNull(void) {
    // Paint the LUT, which was initialised during setup, so we can test that it was not touched by fillBreatheEffectLUT
    memset(gausianBreatheLUT, 0xAB, sizeof(gausianBreatheLUT));
    uint32_t expectedValues[BREATHE_LUT_SIZE] = {0};
    // Paint the expectedValues array with the same value
    memset(expectedValues, 0xAB, sizeof(expectedValues));

    fillBreatheEffectLUT(NULL);

    TEST_ASSERT_EQUAL_UINT32_ARRAY(expectedValues, gausianBreatheLUT, BREATHE_LUT_SIZE);
}

/**
 * Init LED Strips
 */

void test_initLEDStrips_success(void) {
    efferr_t expectedRetVal = EFF_OK;

    // Expectations
    LED_t defaultLEDConfig = {.currAnimation = ANIM_FIXED,
                              .brightnessCtrl.targetBrightness = 5000UL,
                              .brightnessCtrl.delta = 0UL,
                              .brightnessCtrl.brightChangeRequestedFlag = true,
                              .pmwPeriodms = 1UL,
                              .breatheCtrl.currLUTIndex = 0UL,
                              .breatheCtrl.currWave = 0UL,
                              .breatheCtrl.periodms = BREATHE_DEFAULT_PERIOD_MS,
                              .breatheCtrl.wavesPerSample = BREATHE_DEFAULT_PERIOD_MS / BREATHE_LUT_SIZE};

    // Expectations - Use 38400
    getPWMFrequency_ExpectAndReturn(1000);
    getPWMFrequency_ExpectAndReturn(1000);
    getPWMFrequency_ExpectAndReturn(1000);

    setBreathePeriod(LED_CHANNEL_1, BREATHE_DEFAULT_PERIOD_MS);
    setBreathePeriod(LED_CHANNEL_2, BREATHE_DEFAULT_PERIOD_MS);
    setBreathePeriod(LED_CHANNEL_3, BREATHE_DEFAULT_PERIOD_MS);

    efferr_t retVal = initLEDStrips();

    TEST_ASSERT_EQUAL_UINT32(expectedRetVal, retVal);
    // NOTE: This fails because padding is not being initialised! WTF? Anyway, I think clearing the padding is insane
    // just for this test to pass, so I'm going to test individual members... grrrrr
    // TEST_ASSERT_EQUAL_MEMORY(&expectedLED1Ch, &LEDCh1, sizeof(LED_t));
    TEST_ASSERT_EQUAL_UINT32(defaultLEDConfig.currAnimation, LEDCh1.currAnimation);
    TEST_ASSERT_EQUAL_UINT32(CC_CHANNEL_0, LEDCh1.CCChannel);

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
    TEST_ASSERT_EQUAL_UINT32(CC_CHANNEL_1, LEDCh2.CCChannel);

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
    TEST_ASSERT_EQUAL_UINT32(CC_CHANNEL_2, LEDCh3.CCChannel);

    TEST_ASSERT_EQUAL_UINT32(defaultLEDConfig.brightnessCtrl.targetBrightness, LEDCh3.brightnessCtrl.targetBrightness);
    TEST_ASSERT_EQUAL_UINT32(defaultLEDConfig.brightnessCtrl.delta, LEDCh3.brightnessCtrl.delta);
    TEST_ASSERT_TRUE(LEDCh3.brightnessCtrl.brightChangeRequestedFlag);

    TEST_ASSERT_EQUAL_UINT32(defaultLEDConfig.pmwPeriodms, LEDCh3.pmwPeriodms);

    TEST_ASSERT_EQUAL_UINT32(defaultLEDConfig.breatheCtrl.currLUTIndex, LEDCh3.breatheCtrl.currLUTIndex);
    TEST_ASSERT_EQUAL_UINT32(defaultLEDConfig.breatheCtrl.currWave, LEDCh3.breatheCtrl.currWave);
    TEST_ASSERT_EQUAL_UINT32(defaultLEDConfig.breatheCtrl.periodms, LEDCh3.breatheCtrl.periodms);
    TEST_ASSERT_EQUAL_UINT32(defaultLEDConfig.breatheCtrl.wavesPerSample, LEDCh3.breatheCtrl.wavesPerSample);
}

void test_initLEDStrips_assert(void) {
    efferr_t expectedRetVal = EFF_OK;

    LED_t defaultLEDConfig = {.currAnimation = ANIM_FIXED,
                              .brightnessCtrl.targetBrightness = 5000UL,
                              .brightnessCtrl.delta = 0UL,
                              .brightnessCtrl.brightChangeRequestedFlag = true,
                              .pmwPeriodms = 1UL,
                              .breatheCtrl.currLUTIndex = 0UL,
                              .breatheCtrl.currWave = 0UL,
                              .breatheCtrl.periodms = 0UL,
                              .breatheCtrl.wavesPerSample = 0UL};

    // Expectations
    assertExpectFailure();
    getPWMFrequency_ExpectAndReturn(5000);
    // and there are no more calls to getPWMFrequency because the test will end there

    efferr_t retVal = initLEDStrips();
}

/**
 * getLEDStruct
 * - Trivial to test, see we get the correct pointer
 */

void test_getLEDStruct_Success(void) {
    LED_t* retVal = getLEDStruct(LED_CHANNEL_1);

    TEST_ASSERT_EQUAL_PTR(&LEDCh1, retVal);

    retVal = getLEDStruct(LED_CHANNEL_2);

    TEST_ASSERT_EQUAL_PTR(&LEDCh2, retVal);

    retVal = getLEDStruct(LED_CHANNEL_3);

    TEST_ASSERT_EQUAL_PTR(&LEDCh3, retVal);

    // Bad channel, returns 1
    retVal = getLEDStruct((LEDChannel_t)150);

    TEST_ASSERT_EQUAL_PTR(&LEDCh1, retVal);
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
 * getEffectName
 */

void test_getEffectName_Success(void) {
    char* name = getEffectName(ANIM_FIXED);
    TEST_ASSERT_EQUAL_STRING("Fixed Brightness", name);

    name = getEffectName(ANIM_BREATHE);
    TEST_ASSERT_EQUAL_STRING("Breathe Effect", name);
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
 * setBreathePeriod
 * - New period is set and
 */

void test_setBreathePeriod_Succes(void) {
    efferr_t expectedRetVal = EFF_OK;

    // Set up
    uint32_t testNewPeriod = 1000;  // in ms

    uint32_t expectedPWMFreq = 1000;
    uint32_t expectedWavesPerSample = 5;  // 200 samples at 1ms per sample -> 1000ms / (200 * 1ms) = 5

    LEDCh1.pmwPeriodms = 1000 / expectedPWMFreq;

    efferr_t retVal = setBreathePeriod(LED_CHANNEL_1, testNewPeriod);

    TEST_ASSERT_EQUAL_UINT32(expectedRetVal, retVal);
    TEST_ASSERT_EQUAL_UINT32(testNewPeriod, LEDCh1.breatheCtrl.periodms);
    TEST_ASSERT_EQUAL_UINT32(expectedWavesPerSample, LEDCh1.breatheCtrl.wavesPerSample);
}

void test_setBreathePeriod_PeriodIs0(void) {
    efferr_t expectedRetVal = EFF_BADPERIOD;
    uint32_t testPeriod = 0;

    efferr_t retVal = setBreathePeriod(LED_CHANNEL_1, testPeriod);

    TEST_ASSERT_EQUAL_UINT32(expectedRetVal, retVal);
}

void test_setBreathePeriod_PeriodIsNotMultiplePWM(void) {
    // Set up
    efferr_t expectedRetVal = EFF_OK;
    uint32_t testPWMFreq = 100;  // 100Hz, period is 10ms
    // LUT size is 200
    uint32_t testPeriod = 10550;          // won't be a multiple of the PWM period: 10ms * 200 samples
    uint32_t expectedwavesPerSample = 5;  // waves per sample is rounded, in this case down

    LEDCh1.pmwPeriodms = 1000 / testPWMFreq;

    efferr_t retVal = setBreathePeriod(LED_CHANNEL_1, testPeriod);

    TEST_ASSERT_EQUAL_UINT32(expectedRetVal, retVal);
    TEST_ASSERT_EQUAL_UINT32(testPeriod, LEDCh1.breatheCtrl.periodms);
    TEST_ASSERT_EQUAL_UINT32(expectedwavesPerSample, LEDCh1.breatheCtrl.wavesPerSample);

    // Number is always floored
    testPeriod = 11550;          // won't be a multiple of the PWM period: 10ms * 200 samples
    expectedwavesPerSample = 6;  // number must be rounded up this time

    retVal = setBreathePeriod(LED_CHANNEL_1, testPeriod);

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
void test_effectControl_FadeBrightness_IncreaseBrightness(void) {
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

    effectControl_FadeBrightness(expectedChannel);
}

// This test the case where the brightness is decreased
void test_effectControl_FadeBrightness_DecreaseBrightness(void) {
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

    effectControl_FadeBrightness(expectedChannel);
}

// This test the case where the brightness is decreased
void test_effectControl_FadeBrightness_DoNothingIfNoChangesRequired(void) {
    CCChannel_t expectedChannel = CC_CHANNEL_0;
    uint32_t testCurrentBrightness = 2000;
    // Instead of dealing with LUTs, set an arbitrary target compare  for an arbitrary target percent
    uint32_t testTargetBrightness = 2000;
    uint32_t testPWMFreq = 250;  // 250 updates of brightness per second

    // Prepare test
    LEDCh1.brightnessCtrl.currentBrightness = testCurrentBrightness;
    setLEDBrightness(LED_CHANNEL_1, testTargetBrightness);

    effectControl_FadeBrightness(expectedChannel);
}

// Test the case where the change in brightness is lesser than the delta. Normally the last update before getting to the
// target
void test_effectControl_FadeBrightness_IncreaseBrightnessLessThanDelta(void) {
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

    effectControl_FadeBrightness(expectedChannel);
}

// Tests that, if the freq is too high for the difference, we step at the smallest delta, which is 1
void test_effectControl_FadeBrightness_DeltaCantBe0(void) {
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

    effectControl_FadeBrightness(CC_CHANNEL_0);
}

// Check that, if brightness is changed AGAIN while we are triggering a change, the delta changes accordingly so the
// target brightness is still reached a second after the last change (not the first change)
void test_effectControl_FadeBrightness_ChangeDuringChange(void) {
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

    effectControl_FadeBrightness(expectedChannel);

    // At this point, currentBrightness is at 4, delta remains at 4
    expectedPercentToSet = LEDCh1.brightnessCtrl.currentBrightness + expectedDelta;

    // Set Expectations
    setDutyCycle_Expect(expectedChannel, expectedPercentToSet, true);

    effectControl_FadeBrightness(expectedChannel);

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

    effectControl_FadeBrightness(expectedChannel);
}

// This just increases coverage by testing the rest of the channels
void test_effectControl_FadeBrightness_CoverageTestAllChannels(void) {
    // Set target and current to same value so _ChangeBrightness leaves at the earliest
    uint32_t testCurrentBrightness = 500;
    uint32_t testTargetBrightness = 500;

    // Prepare test
    LEDCh2.brightnessCtrl.currentBrightness = testCurrentBrightness;
    setLEDBrightness(LED_CHANNEL_2, testTargetBrightness);

    effectControl_FadeBrightness(LED_CHANNEL_2);

    // Prepare test
    LEDCh3.brightnessCtrl.currentBrightness = testCurrentBrightness;
    setLEDBrightness(LED_CHANNEL_3, testTargetBrightness);

    effectControl_FadeBrightness(LED_CHANNEL_3);
}

/**
 * effectControl_Breathe
 * -
 * - Increase sample
 * - No light if period is 0
 * - Period has decreased and wavesPerSample is now below currWave
 * - LUT index overflows to 0
 */

void test_effectControl_Breathe_setsBrightness(void) {
    LEDChannel_t testLEDChannel = LED_CHANNEL_1;
    uint32_t testCurrWave = 0;            // played n waves...
    uint32_t testWavesPerSample = 10;     // ... and we reset in the next
    uint32_t testBrightnessScale = 8000;  // scale brightness to 80%
    uint32_t testSampleToPlay = 10;

    CCChannel_t expectedPWMChannel = CC_CHANNEL_0;
    uint32_t expectedPercentToSet = (gausianBreatheLUT[testSampleToPlay] * testBrightnessScale) / MAX_BRIGHTNESS;
    uint32_t expectedCurrWave = getLEDStruct(testLEDChannel)->breatheCtrl.currWave + 1;

    getLEDStruct(testLEDChannel)->breatheCtrl.currWave = testCurrWave;
    getLEDStruct(testLEDChannel)->breatheCtrl.wavesPerSample = testWavesPerSample;
    getLEDStruct(testLEDChannel)->breatheCtrl.currLUTIndex = testSampleToPlay;
    getLEDStruct(testLEDChannel)->brightnessCtrl.targetBrightness = testBrightnessScale;

    // Set Expectations
    setDutyCycle_Expect(expectedPWMChannel, expectedPercentToSet, true);

    effectControl_Breathe(testLEDChannel);
    // Check the current wave counter has increased
    TEST_ASSERT_EQUAL_UINT32(expectedCurrWave, getLEDStruct(testLEDChannel)->breatheCtrl.currWave);
}

void test_effectControl_Breathe_MoveToNextSample(void) {
    LEDChannel_t testLEDChannel = LED_CHANNEL_1;
    uint32_t testCurrWave = 4;                       // played n waves...
    uint32_t testWavesPerSample = testCurrWave + 1;  // ... and we reset in the next
    uint32_t testSampleToPlay = 10;
    uint32_t testBrightnessScale = 1234;  // scale brightness to 80%

    CCChannel_t expectedPWMChannel = CC_CHANNEL_0;
    uint32_t expectedCurrWave = 0;  // currWave should reset
    uint32_t expectedLUTIdx = testSampleToPlay + 1;
    uint32_t expectedPercentToSet = (gausianBreatheLUT[testSampleToPlay] * testBrightnessScale) / MAX_BRIGHTNESS;

    getLEDStruct(testLEDChannel)->breatheCtrl.currLUTIndex = testSampleToPlay;
    getLEDStruct(testLEDChannel)->breatheCtrl.currWave = testCurrWave;
    getLEDStruct(testLEDChannel)->breatheCtrl.wavesPerSample = testWavesPerSample;
    getLEDStruct(testLEDChannel)->brightnessCtrl.targetBrightness = testBrightnessScale;
    // Set Expectations
    setDutyCycle_Expect(expectedPWMChannel, expectedPercentToSet,
                        true);  // make sure we play the correct sample and not "the next"

    effectControl_Breathe(testLEDChannel);
    // Check the current wave counter has reset
    TEST_ASSERT_EQUAL_UINT32(expectedCurrWave, getLEDStruct(testLEDChannel)->breatheCtrl.currWave);
    // Check the LUT index has increased
    TEST_ASSERT_EQUAL_UINT32(expectedLUTIdx, getLEDStruct(testLEDChannel)->breatheCtrl.currLUTIndex);
}

void test_effectControl_Breathe_CurrWaveOverWavesPerSample(void) {
    LEDChannel_t testLEDChannel = LED_CHANNEL_1;
    uint32_t testLUTIndex = 5;
    uint32_t testCurrWave = 10;  // supose our current wave is 10
    // And we reduce period so that we only play...
    uint32_t testWavesPerSample = 5;  // 5 waves per sample

    getLEDStruct(testLEDChannel)->breatheCtrl.currLUTIndex = testLUTIndex;
    getLEDStruct(testLEDChannel)->breatheCtrl.currWave = testCurrWave;
    getLEDStruct(testLEDChannel)->breatheCtrl.wavesPerSample = testWavesPerSample;

    CCChannel_t expectedPWMChannel = CC_CHANNEL_0;
    uint32_t expectedCurrWave = 0;  // currWave should reset
    uint32_t expectedLUTIdx = getLEDStruct(testLEDChannel)->breatheCtrl.currLUTIndex + 1;

    // Set Expectations
    setDutyCycle_ExpectAnyArgs();  // No need to check args of this in this test, it's out of its scope

    effectControl_Breathe(testLEDChannel);
    // Check the current wave counter has reset
    TEST_ASSERT_EQUAL_UINT32(expectedCurrWave, getLEDStruct(testLEDChannel)->breatheCtrl.currWave);
    // Check the LUT index has increased
    TEST_ASSERT_EQUAL_UINT32(expectedLUTIdx, getLEDStruct(testLEDChannel)->breatheCtrl.currLUTIndex);
}

void test_effectControl_Breathe_LUTIndexOverflow(void) {
    LEDChannel_t testLEDChannel = LED_CHANNEL_1;
    uint32_t testCurrWave = 4;                       // played n waves...
    uint32_t testWavesPerSample = testCurrWave + 1;  // ... and we reset in the next
    uint32_t testLUTIdx = BREATHE_LUT_SIZE;
    getLEDStruct(testLEDChannel)->breatheCtrl.currLUTIndex = testLUTIdx;
    getLEDStruct(testLEDChannel)->breatheCtrl.currWave = testCurrWave;
    getLEDStruct(testLEDChannel)->breatheCtrl.wavesPerSample = testWavesPerSample;

    CCChannel_t expectedPWMChannel = CC_CHANNEL_0;
    uint32_t expectedCurrWave = 0;  // currWave should reset
    uint32_t expectedLUTIdx = 0;    // lutIndex should reset

    // Set Expectations
    setDutyCycle_ExpectAnyArgs();

    effectControl_Breathe(testLEDChannel);
    // Check the current wave counter has reset
    TEST_ASSERT_EQUAL_UINT32(expectedCurrWave, getLEDStruct(testLEDChannel)->breatheCtrl.currWave);
    // Check the LUT index has increased
    TEST_ASSERT_EQUAL_UINT32(expectedLUTIdx, getLEDStruct(testLEDChannel)->breatheCtrl.currLUTIndex);
}

void test_effectControl_Breathe_WavesPerSampleIs0(void) {
    LEDChannel_t testLEDChannel = LED_CHANNEL_1;
    uint32_t testWavesPerSample = 0;         //
    uint32_t testCurrWave = 4;               // just put some values here to see that they don't change
    uint32_t testLUTIdx = BREATHE_LUT_SIZE;  // same here
    getLEDStruct(testLEDChannel)->breatheCtrl.currLUTIndex = testLUTIdx;
    getLEDStruct(testLEDChannel)->breatheCtrl.currWave = testCurrWave;
    getLEDStruct(testLEDChannel)->breatheCtrl.wavesPerSample = testWavesPerSample;

    // Set Expectations - expectation is that setDutyCycle is not called!

    effectControl_Breathe(testLEDChannel);
    // Check the current wave counter has reset
    TEST_ASSERT_EQUAL_UINT32(testCurrWave, getLEDStruct(testLEDChannel)->breatheCtrl.currWave);
    // Check the LUT index has increased
    TEST_ASSERT_EQUAL_UINT32(testLUTIdx, getLEDStruct(testLEDChannel)->breatheCtrl.currLUTIndex);
}