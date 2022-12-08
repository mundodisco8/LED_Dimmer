#include "timer_HW.h"

#include <string.h>
#include <unity.h>

#include "testable_micro_registers.h"

#include "mock_em_cmu.h"
#include "mock_em_timer.h"
#include "mock_gpio_HW.h"

/*
 * Unit tests for "timer_HW.h"
 */

TIMER_TypeDef TIMER0_meat = {0};  // where the meat of TIMER0 is.
GPIO_TypeDef GPIO_meat = {0};

void setUp(void) {
    memset(&TIMER0_meat, 0x00, sizeof(TIMER_TypeDef));
    TIMER0 = &TIMER0_meat;
    memset(&GPIO_meat, 0x00, sizeof(GPIO_TypeDef));
    GPIO = &GPIO_meat;
}

void tearDown(void) {
}

////
// setChannelOutputCompare
////

/***
 * Tests:
 * 1) Setting a new compare value calls TIMER_CompareSet with the correct params and returns TIMER_OK
 * 2) If TIMER0 is not enabled, return TIMER_DISBLED_BEFORE_WRITING_SYNC
 */

void test_setChannelOutputCompare_CompareValueIsSet(void) {
    uint32_t expectedCompValue = 100;
    CCChannel_t testChannel = CC_CHANNEL_0;

    // "Enable" TIMER0
    TIMER0->EN = TIMER_EN_EN;
    TIMER_CompareSet_Expect(TIMER0, testChannel, expectedCompValue);

    timerStatus_t status = TIMHW_setChannelOutputCompare(testChannel, expectedCompValue);
    TEST_ASSERT_EQUAL_INT32(TIMER_OK, status);
}

void test_setChannelOutputCompare_TimerIsDisabled(void) {
    uint32_t expectedCompValue = 100;
    CCChannel_t testChannel = CC_CHANNEL_0;
    // TIMER0 starts disabled by default, no need to do anything to cover that case.
    timerStatus_t status = TIMHW_setChannelOutputCompare(testChannel, expectedCompValue);
    TEST_ASSERT_EQUAL_INT32(TIMER_DISBLED_BEFORE_WRITING_SYNC, status);
}

////
// setChannelBufferedOutputCompare
////

/***
 * Tests:
 * 1) Setting a new compare value calls TIMER_CompareBufSet with the correct params and returns TIMER_OK
 * 2) If TIMER0 is not enabled, return TIMER_DISBLED_BEFORE_WRITING_SYNC
 */

void test_setChannelBufferedOutputCompare_CompareValueIsSet(void) {
    uint32_t expectedCompValue = 100;
    CCChannel_t testChannel = CC_CHANNEL_0;

    // "Enable" TIMER0
    TIMER0->EN = TIMER_EN_EN;
    TIMER_CompareBufSet_Expect(TIMER0, testChannel, expectedCompValue);

    timerStatus_t status = TIMHW_setChannelBufferedOutputCompare(testChannel, expectedCompValue);
    TEST_ASSERT_EQUAL_INT32(TIMER_OK, status);
}

void test_setChannelBufferedOutputCompare_TimerIsDisabled(void) {
    uint32_t expectedCompValue = 100;
    CCChannel_t testChannel = CC_CHANNEL_0;
    // TIMER0 starts disabled by default, no need to do anything to cover that case.
    timerStatus_t status = TIMHW_setChannelBufferedOutputCompare(testChannel, expectedCompValue);
    TEST_ASSERT_EQUAL_INT32(TIMER_DISBLED_BEFORE_WRITING_SYNC, status);
}

////
// setTimer0TopValue
////

/***
 * Tests:
 * 1) Setting a new Top value calls TIMER_TopSet with the correct params and returns TIMER_OK
 * 2) If TIMER0 is not enabled, return TIMER_DISBLED_BEFORE_WRITING_SYNC
 */

void test_setTimer0TopValue_ValueIsSet(void) {
    uint32_t expectedTopValue = 100;

    // "Enable" TIMER0
    TIMER0->EN = TIMER_EN_EN;
    TIMER_TopSet_Expect(TIMER0, expectedTopValue);

    timerStatus_t status = TIMHW_setTimer0TopValue(expectedTopValue);
    TEST_ASSERT_EQUAL_INT32(TIMER_OK, status);
}

void test_setTimer0TopValue_TimerIsDisabled(void) {
    uint32_t expectedTopValue = 100;
    // TIMER0 starts disabled by default, no need to do anything to cover that case.
    timerStatus_t status = TIMHW_setTimer0TopValue(expectedTopValue);
    TEST_ASSERT_EQUAL_INT32(TIMER_DISBLED_BEFORE_WRITING_SYNC, status);
}

void test_initTimer0Clock(void) {
    CMU_ClockEnable_Expect(cmuClock_TIMER0, true);

    TIMHW_initTimer0Clock();
}

////
// getters and setters
////

// Tested only to get 100% coverage
void test_getTimer0TopValue(void) {
    uint32_t expectedTop = 145;
    TIMER_TopGet_ExpectAndReturn(TIMER0, expectedTop);

    uint32_t top = TIMHW_getTimer0TopValue();
    TEST_ASSERT_EQUAL_INT32(expectedTop, top);
}

// Tested only to get 100% coverage
void test_getTimerFrequency(void) {
    uint32_t expectedFreq = 1000000;
    CMU_ClockFreqGet_ExpectAndReturn(cmuClock_TIMER0, expectedFreq);

    uint32_t freq = TIMHW_getTimerFrequency();
    TEST_ASSERT_EQUAL_INT32(expectedFreq, freq);
}

void test_startChannelPWM(void) {
    // We set the CCn pin enable bit of the ROUTEEN register
    uint32_t expectedRegister = 0x00000001;
    TIMHW_startChannelPWM(CC_CHANNEL_0);

    TEST_ASSERT_EQUAL_UINT32(expectedRegister, GPIO->TIMERROUTE_SET[0].ROUTEEN);
    // We set now Channel 1. In the HW, in the ROUTEEN register, both pins would be set
    // Because we are interfacing with the _SET version of the register, our code only will write in the
    // CC1 pin, not in CC0 | CC1
    expectedRegister = 0x00000002;

    TIMHW_startChannelPWM(CC_CHANNEL_1);
    TEST_ASSERT_EQUAL_UINT32(expectedRegister, GPIO->TIMERROUTE_SET[0].ROUTEEN);
}

// Same thing as startChannelPWM but with the CLR register
void test_stopChannelPWM(void) {
    // We set the CCn pin enable bit of the ROUTEEN register
    uint32_t expectedRegister = 0x00000001;
    TIMHW_stopChannelPWM(CC_CHANNEL_0);

    TEST_ASSERT_EQUAL_UINT32(expectedRegister, GPIO->TIMERROUTE_CLR[0].ROUTEEN);
    // We set now Channel 1. In the HW, in the ROUTEEN register, both pins would be set
    // Because we are interfacing with the _CLR version of the register, our code only will write in the
    // CC1 pin, not in CC0 | CC1
    expectedRegister = 0x00000002;

    TIMHW_stopChannelPWM(CC_CHANNEL_1);
    TEST_ASSERT_EQUAL_UINT32(expectedRegister, GPIO->TIMERROUTE_CLR[0].ROUTEEN);
}

////
// Config Functions
////

// Not much to test, as the config struct is local to the functions, we can't check it.

void test_configCCChannelPWM(void) {
    CCChannel_t expectedChannel = CC_CHANNEL_0;

    TIMER_InitCC_Expect(TIMER0, expectedChannel, NULL);
    TIMER_InitCC_IgnoreArg_init();
    // Additionally, TIMER_InitCC() will enable the Timer, something TIMHW_setChannelOutputCompare() is
    // expecting
    TIMER0->EN = TIMER_EN_EN;
    // This will trigger three calls to TIMHW_setChannelOutputCompare(), one for each channel
    TIMER_CompareSet_Expect(TIMER0, CC_CHANNEL_0, 0);
    TIMER_CompareSet_Expect(TIMER0, CC_CHANNEL_1, 0);
    TIMER_CompareSet_Expect(TIMER0, CC_CHANNEL_2, 0);

    TIMHW_configCCChannelPWM(expectedChannel, PWM_ACTIVE_HIGH);
}

void test_TIMHW_startTimer0(void) {
    TIMER_Init_Expect(TIMER0, NULL);
    TIMER_Init_IgnoreArg_init();

    TIMHW_startTimer0();
}

////
// TIMHW_setCCChannelPin
////

// Not much to test either, just a switch that sets three variables
void test_setCCChannelPin(void) {
    pinPort_t expectedPortCC0 = portA;
    uint32_t expectedPinCC0 = 4;
    CCChannel_t testChannel = CC_CHANNEL_0;

    // TIMHW_setCCChannelPin writes to the GPIO_TIMER0_CCnROUTE register
    //  bits [1:0]: port
    //  bits [19:16]: pin
    uint32_t expectedRegCC0 = expectedPortCC0 | expectedPinCC0 << 16;

    setPinMode_Expect(expectedPortCC0, expectedPinCC0, MODE_PUSHPULL, 0);

    timerStatus_t status = TIMHW_setCCChannelPin(expectedPortCC0, expectedPinCC0, testChannel);

    TEST_ASSERT_EQUAL_UINT32(TIMER_OK, status);
    TEST_ASSERT_EQUAL_UINT32(expectedRegCC0, GPIO->TIMERROUTE[0].CC0ROUTE);

    // Same with the other channels, to hit all the cases of the switch
    pinPort_t expectedPortCC1 = portB;
    uint32_t expectedPinCC1 = 3;
    testChannel = CC_CHANNEL_1;

    uint32_t expectedRegCC1 = expectedPortCC1 | expectedPinCC1 << 16;

    setPinMode_Expect(expectedPortCC1, expectedPinCC1, MODE_PUSHPULL, 0);

    status = TIMHW_setCCChannelPin(expectedPortCC1, expectedPinCC1, testChannel);

    TEST_ASSERT_EQUAL_UINT32(TIMER_OK, status);
    TEST_ASSERT_EQUAL_UINT32(expectedRegCC1, GPIO->TIMERROUTE[0].CC1ROUTE);

    pinPort_t expectedPortCC2 = portD;
    uint32_t expectedPinCC2 = 1;
    testChannel = CC_CHANNEL_2;

    uint32_t expectedRegCC2 = expectedPortCC2 | expectedPinCC2 << 16;

    setPinMode_Expect(expectedPortCC2, expectedPinCC2, MODE_PUSHPULL, 0);

    status = TIMHW_setCCChannelPin(expectedPortCC2, expectedPinCC2, testChannel);

    TEST_ASSERT_EQUAL_UINT32(TIMER_OK, status);
    TEST_ASSERT_EQUAL_UINT32(expectedRegCC2, GPIO->TIMERROUTE[0].CC2ROUTE);
}

void test_setCCChannelPin_BadChannel(void) {
    pinPort_t expectedPortCC0 = portA;
    uint32_t expectedPinCC0 = 4;
    CCChannel_t testChannel = (CCChannel_t)6;

    timerStatus_t status = TIMHW_setCCChannelPin(expectedPortCC0, expectedPinCC0, testChannel);

    TEST_ASSERT_EQUAL_UINT32(TIMER_WRONG_CC_CHANNEL, status);
}