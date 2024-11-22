#include "sleepyTimers_HW.h"

#include <unity.h>

#include "sl_status.h"
#include "app_log.h"
#include "sl_bluetooth_config.h"

// We can mock the real sl_sleeptimers! YAY!
#include "mock_sl_sleeptimer.h"
// TEST_INCLUDE_PATH("../src/simplicity_sdk_2024.6.2/platform/common/inc/")
// #include "../src/simplicity_sdk_2024.6.2/platform/common/inc/sl_status.h"

/*
 * Unit tests for "sleepyTimers_HW.c"
 */

void setUp(void) {
    SLP_resetTimersUsed();
}

void tearDown(void) {
}



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
    timerHandlePtr_t testPtr = NULL; // I will overwrite this pointer
    slpTimerStatus_t retVal = SLPTIMER_OK;
    // Call N+1 times
    for (uint32_t i = 0; i < SL_BT_CONFIG_MAX_SOFTWARE_TIMERS + 1; i++) {
        retVal = SLP_reserveTimer(&testPtr);
    }

    TEST_ASSERT_EQUAL(expectedRetVal, retVal);
    TEST_ASSERT_NULL(testPtr);
}