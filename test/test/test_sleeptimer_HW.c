#include "sleepyTimers_HW.h"

#include <unity.h>

#include "mock_sl_sleeptimer.h"

/*
 * Unit tests for "sleeptimer_HW.h"
 */

// Dummy callback to use in tests
void dummyTimerCallback(timerHandle_t* handle, void* data){}

void setUp(void) {
}

void tearDown(void) {
}

////
// startTimer and startPeriodictimer
////

void test_startTimer_ReturnsOK(void) {
    timerHandle_t handle;
    uint32_t timeout = 1000;

    sl_sleeptimer_start_timer_ms_ExpectAndReturn((sl_sleeptimer_timer_handle_t*)&handle, timeout, NULL, NULL, 0, 0, SL_STATUS_OK);

    uint32_t retVal = SLP_startTimer(&handle, timeout, NULL, NULL);

    TEST_ASSERT_EQUAL_UINT32(TIMER_OK, retVal);
}

void test_startTimer_TimerHandleIsNull(void) {
    uint32_t timeout = 1000;

    sl_sleeptimer_start_timer_ms_ExpectAndReturn(NULL, timeout, NULL, NULL, 0, 0, SL_STATUS_NULL_POINTER);
    uint32_t retVal = SLP_startTimer(NULL, timeout, NULL, NULL);

    TEST_ASSERT_EQUAL_UINT32(SL_STATUS_NULL_POINTER, retVal);
}


void test_startPeriodicTimer_ReturnsOK(void) {
    timerHandle_t handle;
    uint32_t timeout = 1000;

    sl_sleeptimer_start_periodic_timer_ms_ExpectAndReturn((sl_sleeptimer_timer_handle_t*)&handle, timeout, NULL, NULL, 0, 0, SL_STATUS_OK);

    uint32_t retVal = SLP_startPeriodicTimer(&handle, timeout, NULL, NULL);

    TEST_ASSERT_EQUAL_UINT32(TIMER_OK, retVal);
}