#include "interrupt_HW.h"

#include <unity.h>

#include "mock_gpiointerrupt.h"

/***
 * Unit tests for "interrupt_HW.h"
 *
 * It's a pretty straightforward wrapper, so not much to do here.
 * Just trying to cover 100%
 */

void setUp(void) {
}

void tearDown(void) {
}


void test_initialiseInterrupts(void) {
    GPIOINT_Init_Expect();

    initialiseInterrupts();
}

// dummy callback for testing
void dummyCallback(uint8_t intNo){}

void test_setInterruptCallback(void) {
    uint32_t pinNo = 5;

    GPIOINT_CallbackRegister_Expect(pinNo, dummyCallback);

    setInterruptCallback(pinNo, dummyCallback);
}

// dummy callback with context for testing
void dummyCallbackWithContext(uint8_t intNo, void *ctx){}

void test_setInterruptCallback_InterruptRequestedNotInUse(void) {
    uint32_t pinNo = 5;

    GPIOINT_CallbackRegisterExt_ExpectAndReturn(pinNo, dummyCallbackWithContext, NULL, pinNo);

    uint32_t registeredInterrupt = setInterruptCallbackWCtx(pinNo, dummyCallbackWithContext, NULL);
    TEST_ASSERT_EQUAL_UINT32(pinNo, registeredInterrupt);
}

void test_setInterruptCallback_NoInterruptsAvailable(void) {
    uint32_t pinNo = 5;
    // 0xFF is returned when the no interrupts are free
    GPIOINT_CallbackRegisterExt_ExpectAndReturn(pinNo, dummyCallbackWithContext, NULL, 0xFF);

    uint32_t registeredInterrupt = setInterruptCallbackWCtx(pinNo, dummyCallbackWithContext, NULL);
    TEST_ASSERT_EQUAL_UINT32(0xFF, registeredInterrupt);
}