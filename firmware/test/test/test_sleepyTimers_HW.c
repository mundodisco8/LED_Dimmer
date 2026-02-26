#include <unity.h>

#include "app_log.h"
#include "sl_bluetooth_config.h"
#include "sl_status.h"
#include "sleepyTimers_HW.h"

// We can mock the real sl_sleeptimers! YAY!
#include "mock_sl_sleeptimer.h"
// TEST_INCLUDE_PATH("../src/simplicity_sdk_2024.6.2/platform/common/inc/")
// #include "../src/simplicity_sdk_2024.6.2/platform/common/inc/sl_status.h"

/*
 * Unit tests for "sleepyTimers_HW.c"
 */

// Test Callaback functions. They don't do anything, but we use them to pass as callbacks for timers, and to check they
// are correctly being assigned
void dummyCallback(timerHandlePtr_t* handle, void* data) {
    (void)handle;
    (void)data;
}

void setUp(void) { SLP_resetTimersUsed(); }

void tearDown(void) { setPrintingState(DEBUG_PRINT_DISABLE); }

void test_reserveTimer_success(void) {
    slpTimerStatus_t expectedRetVal = SLPTIMER_OK;
    timerHandlePtr_t testPtr = NULL;

    slpTimerStatus_t retVal = SLP_reserveTimer(&testPtr);
    TEST_ASSERT_EQUAL(expectedRetVal, retVal);
    TEST_ASSERT_NOT_NULL(testPtr);
}

void test_reserveTimer_ReserveNTimersSuccess(void) {
    slpTimerStatus_t expectedRetVal = SLPTIMER_OK;
    timerHandlePtr_t testPtrArray[SL_BT_CONFIG_MAX_SOFTWARE_TIMERS] = {NULL};

    for (uint32_t i = 0; i < SL_BT_CONFIG_MAX_SOFTWARE_TIMERS; i++) {
        slpTimerStatus_t retVal = SLP_reserveTimer(&(testPtrArray[i]));
        TEST_ASSERT_EQUAL(expectedRetVal, retVal);
        TEST_ASSERT_NOT_NULL(testPtrArray[i]);
    }

    // Now, because we can't get the address of the timer array, the only check I can do is to see if the distance
    // between the pointers is the size of a struct
    size_t timerHandleStructSize = sizeof(sl_sleeptimer_timer_handle_t);
    for (uint32_t i = 0; i < SL_BT_CONFIG_MAX_SOFTWARE_TIMERS - 1; i++) {
        // Treat the pointers as uint8_t*, so the result of the substraction is in bytes (otherwise it would be 1)
        ptrdiff_t offset = (uint8_t*)testPtrArray[i + 1] - (uint8_t*)testPtrArray[i];
        TEST_ASSERT_EQUAL(timerHandleStructSize, offset);
    }
}

void test_reserveTimer_ReserveNPlusOneTimer(void) {
    slpTimerStatus_t expectedRetVal = SLPTIMER_NO_TIMERS_AVAILABLE;
    timerHandlePtr_t testPtr = NULL;  // I will overwrite this pointer
    slpTimerStatus_t retVal = SLPTIMER_OK;
    // Call N+1 times
    for (uint32_t i = 0; i < SL_BT_CONFIG_MAX_SOFTWARE_TIMERS + 1; i++) {
        retVal = SLP_reserveTimer(&testPtr);
    }

    TEST_ASSERT_EQUAL(expectedRetVal, retVal);
    TEST_ASSERT_NULL(testPtr);
}

// startTimer
// - Success, start the timer with the correct params
// - Success-ish, get a warning if timer is already running
// - Error, timer could not be started

void test_startTimer_Success(void) {
    slpTimerStatus_t expectedRetVal = SLPTIMER_OK;
    timerHandlePtr_t testPtr = NULL;
    uint32_t testTimeOut = 100;
    uint32_t testContext = 4;  // Context passed to the callback, can be anything
    uint8_t testPriority = 0;
    uint16_t testFlags = 0;

    // Set Expectations
    sl_sleeptimer_start_timer_ms_ExpectAndReturn((sl_sleeptimer_timer_handle_t*)testPtr, testTimeOut,
                                                 (sl_sleeptimer_timer_callback_t)dummyCallback, &testContext,
                                                 testPriority, testFlags, SL_STATUS_OK);

    slpTimerStatus_t retVal = SLP_startTimer(testPtr, testTimeOut, TIMER_ONE_SHOT, dummyCallback, &testContext);
    TEST_ASSERT_EQUAL(expectedRetVal, retVal);
}

void test_startTimer_WarningTimerRunning(void) {
    setPrintingState(DEBUG_PRINT_ENABLE);  // This test should print
    slpTimerStatus_t expectedRetVal = SLPTIMER_OK;
    timerHandlePtr_t testPtr = NULL;
    uint32_t testTimeOut = 100;
    uint32_t testContext = 4;  // Context passed to the callback, can be anything

    // Set Expectations
    sl_sleeptimer_start_timer_ms_ExpectAnyArgsAndReturn(SL_STATUS_NOT_READY);

    slpTimerStatus_t retVal = SLP_startTimer(testPtr, testTimeOut, TIMER_ONE_SHOT, dummyCallback, &testContext);
    TEST_ASSERT_EQUAL(expectedRetVal, retVal);

    // OCD - coverage to 100%
    setPrintingState(DEBUG_PRINT_DISABLE);
    sl_sleeptimer_start_timer_ms_ExpectAnyArgsAndReturn(SL_STATUS_NOT_READY);

    retVal = SLP_startTimer(testPtr, testTimeOut, TIMER_ONE_SHOT, dummyCallback, &testContext);
    TEST_ASSERT_EQUAL(expectedRetVal, retVal);
}

void test_startTimer_Error(void) {
    setPrintingState(DEBUG_PRINT_ENABLE);  // This test should print
    slpTimerStatus_t expectedRetVal = SLPTIMER_ERROR;
    timerHandlePtr_t testPtr = NULL;
    uint32_t testTimeOut = 100;
    uint32_t testContext = 4;  // Context passed to the callback, can be anything

    // Set Expectations
    sl_sleeptimer_start_timer_ms_ExpectAnyArgsAndReturn(SL_STATUS_NULL_POINTER);

    slpTimerStatus_t retVal = SLP_startTimer(testPtr, testTimeOut, TIMER_ONE_SHOT, dummyCallback, &testContext);
    TEST_ASSERT_EQUAL(expectedRetVal, retVal);
}

void test_startPeriodicTimer_Success(void) {
    slpTimerStatus_t expectedRetVal = SLPTIMER_OK;
    timerHandlePtr_t testPtr = NULL;
    uint32_t testTimeOut = 100;
    uint32_t testContext = 4;  // Context passed to the callback, can be anything
    uint8_t testPriority = 0;
    uint16_t testFlags = 0;

    // Set Expectations
    sl_sleeptimer_start_periodic_timer_ms_ExpectAndReturn((sl_sleeptimer_timer_handle_t*)testPtr, testTimeOut,
                                                          (sl_sleeptimer_timer_callback_t)dummyCallback, &testContext,
                                                          testPriority, testFlags, SL_STATUS_OK);

    slpTimerStatus_t retVal = SLP_startTimer(testPtr, testTimeOut, TIMER_RESTART, dummyCallback, &testContext);
    TEST_ASSERT_EQUAL(expectedRetVal, retVal);
}

void test_startTimer_UnexpectedError(void) {
    slpTimerStatus_t expectedRetVal = SLPTIMER_ERROR;
    timerHandlePtr_t testPtr = NULL;
    uint32_t testTimeOut = 100;
    uint32_t testContext = 4;  // Context passed to the callback, can be anything

    // Set Expectations
    sl_sleeptimer_start_timer_ms_ExpectAnyArgsAndReturn(SL_STATUS_FAIL);

    slpTimerStatus_t retVal = SLP_startTimer(testPtr, testTimeOut, TIMER_ONE_SHOT, dummyCallback, &testContext);
    TEST_ASSERT_EQUAL(expectedRetVal, retVal);
}

// stopTimer
// - Success
// - SDK Returns error

void test_stopTimer_Success(void) {
    slpTimerStatus_t expectedRetVal = SLPTIMER_OK;
    timerHandlePtr_t testPtr = NULL;

    // Set Expectations
    sl_sleeptimer_stop_timer_ExpectAndReturn((sl_sleeptimer_timer_handle_t*)testPtr, SL_STATUS_OK);

    slpTimerStatus_t retVal = SLP_stopTimer(testPtr);

    TEST_ASSERT_EQUAL(expectedRetVal, retVal);
}

void test_stopTimer_Error(void) {
    setPrintingState(DEBUG_PRINT_ENABLE);
    slpTimerStatus_t expectedRetVal = SLPTIMER_ERROR;
    timerHandlePtr_t testPtr = NULL;

    // Set Expectations
    sl_sleeptimer_stop_timer_ExpectAndReturn((sl_sleeptimer_timer_handle_t*)testPtr, SL_STATUS_INVALID_STATE);

    slpTimerStatus_t retVal = SLP_stopTimer(testPtr);

    TEST_ASSERT_EQUAL(expectedRetVal, retVal);

    // OCD - coverage to 100%
    setPrintingState(DEBUG_PRINT_DISABLE);
    sl_sleeptimer_stop_timer_ExpectAndReturn((sl_sleeptimer_timer_handle_t*)testPtr, SL_STATUS_INVALID_STATE);

    retVal = SLP_stopTimer(testPtr);
}

// isTimerRunning
// - Success, and returns whatever the SDK function returns
// - Error checking for the timer, and returns false

void test_isTimerRunning_Success(void) {
    bool expectedRetVal = true;
    timerHandlePtr_t testPtr = NULL;

    // Set Expectations
    bool isTimerRunning = true;
    sl_sleeptimer_is_timer_running_ExpectAndReturn((sl_sleeptimer_timer_handle_t*)testPtr, NULL, SL_STATUS_OK);
    sl_sleeptimer_is_timer_running_IgnoreArg_running();
    sl_sleeptimer_is_timer_running_ReturnThruPtr_running(&isTimerRunning);
    bool retVal = SLP_isTimerRunning(testPtr);

    TEST_ASSERT_EQUAL(expectedRetVal, retVal);
}

void test_isTimerRunning_Error(void) {
    bool expectedRetVal = false;
    timerHandlePtr_t testPtr = NULL;

    // Set Expectations
    sl_sleeptimer_is_timer_running_ExpectAndReturn((sl_sleeptimer_timer_handle_t*)testPtr, &expectedRetVal,
                                                   SL_STATUS_INVALID_STATE);

    bool retVal = SLP_isTimerRunning(testPtr);

    TEST_ASSERT_EQUAL(expectedRetVal, retVal);
}

void test_getSystemTicksInMs_Success(void) {
    uint64_t expectedRetVal = 1000;

    uint64_t expectedTicks = 100;
    sl_sleeptimer_get_tick_count64_ExpectAndReturn(100);
    sl_sleeptimer_tick64_to_ms_ExpectAndReturn(expectedTicks, NULL, SL_STATUS_OK);
    sl_sleeptimer_tick64_to_ms_IgnoreArg_ms();
    sl_sleeptimer_tick64_to_ms_ReturnThruPtr_ms(&expectedRetVal);

    uint64_t retVal = SLP_getSystemTickInMs();
    TEST_ASSERT_EQUAL_INT64(expectedRetVal, retVal);
}

void test_getSystemTicksInMs_Error(void) {
    uint64_t expectedRetVal = 0;

    uint64_t expectedTicks = 100;
    uint64_t dummyValue = 200;
    sl_sleeptimer_get_tick_count64_ExpectAndReturn(100);
    sl_sleeptimer_tick64_to_ms_ExpectAndReturn(expectedTicks, NULL, SL_STATUS_INVALID_STATE);
    sl_sleeptimer_tick64_to_ms_IgnoreArg_ms();
    sl_sleeptimer_tick64_to_ms_ReturnThruPtr_ms(&dummyValue);

    uint64_t retVal = SLP_getSystemTickInMs();
    TEST_ASSERT_EQUAL_INT64(expectedRetVal, retVal);
}