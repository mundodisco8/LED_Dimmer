#include <string.h>
#include <unity.h>

#include "buttons.h"
#include "inttypes.h"
#include "mock_gpio_HW.h"
#include "mock_interrupt_HW.h"
#include "mock_timers_HW.h"
#include "pin_config.h"

extern button_t button0;
extern button_t button1;

void setUp() {
    memset(&button0, 0x00, sizeof(button_t));
    memset(&button1, 0x00, sizeof(button_t));
}

void tearDown() {
}

////
// init Buttons
////

// Check that buttons are initialised correctly
void fakePressedAction(void) {}
void fakeReleasedAction(void) {}

void test_initButtonsDoesItsJob(void) {
    button_t testBtn = {0};
    initButton(&testBtn, portA, 1, fakePressedAction, fakeReleasedAction);
    TEST_ASSERT_EQUAL_UINT32(portA, testBtn.btnPort);
    TEST_ASSERT_EQUAL_UINT32(1, testBtn.pinNo);
    TEST_ASSERT_EQUAL_UINT32(BUTTON_RELEASED, testBtn.state);
    TEST_ASSERT_EQUAL_INT32(0, testBtn.integrator);
    TEST_ASSERT_EQUAL_PTR(fakePressedAction, testBtn.pressedAction);
    TEST_ASSERT_EQUAL_PTR(fakeReleasedAction, testBtn.releasedAction);
}

////
// gpioCallbackButton1
////

// // Check that timers are set
// extern void gpioCallbackButton1(uint8_t intNo, void* ctx);

// void test_CallbackButton1DoesItsJob(void) {
//     setSamplingTimerBtn1_ExpectAndReturn(&button1, 0);
//     gpioCallbackButton1(btn1_PIN, &button1);
// }

////
// configureButtonInterrupts
////

void fakeButtonCallback(uint8_t intNo, void* ctx) {}

// Test configureGPIOInterrupts, make sure the right calls are made to set up interrupts
void test_configureButtonInterrupts_Works(void) {
    button_t testBtn = {
        .btnPort = portA,
        .pinNo = 1,
    };

    setInterruptCallbackWCtx_ExpectAndReturn(testBtn.pinNo, fakeButtonCallback, &testBtn, testBtn.pinNo);
    configurePinInterrupt_Expect(testBtn.btnPort, testBtn.pinNo, testBtn.pinNo, true, true, true);
    enablePinInterrupts_Expect(1 << testBtn.pinNo);
    uint32_t retVal = configureButtonInterrupts(&testBtn, fakeButtonCallback);
    TEST_ASSERT_EQUAL_UINT32(testBtn.pinNo, retVal);
}

// Test configureGPIOInterrupts, but setting the interrupt fails with INTERRUPT_UNAVAILABLE
void test_configureButtonInterrupts_NoInterruptsAvailable(void) {
    button_t testBtn = {
        .btnPort = portA,
        .pinNo = 1,
    };

    setInterruptCallbackWCtx_ExpectAndReturn(testBtn.pinNo, fakeButtonCallback, &testBtn, 0xFF);
    // No other calls have to be made and the function exits
    uint32_t retVal = configureButtonInterrupts(&testBtn, fakeButtonCallback);
    TEST_ASSERT_EQUAL_UINT32(0xFF, retVal);
}