#include <string.h>
#include <unity.h>

#include "buttons.h"
#include "inttypes.h"
#include "mock_gpio_HW.h"
#include "mock_interrupt_HW.h"
#include "mock_sleeptimer_HW.h"
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
    uint32_t retVal = initButton(&testBtn, portA, 1, fakePressedAction, fakeReleasedAction);
    // Check retVal is true
    TEST_ASSERT_EQUAL_UINT32(BTN_OK, retVal);
    TEST_ASSERT_EQUAL_UINT32(portA, testBtn.btnPort);
    TEST_ASSERT_EQUAL_UINT32(1, testBtn.pinNo);
    TEST_ASSERT_EQUAL_UINT32(BUTTON_RELEASED, testBtn.state);
    TEST_ASSERT_EQUAL_INT32(0, testBtn.integrator);
    TEST_ASSERT_EQUAL_PTR(fakePressedAction, testBtn.pressedAction);
    TEST_ASSERT_EQUAL_PTR(fakeReleasedAction, testBtn.releasedAction);
}

// Protection against null pointers: button_t ptr is Null
void test_initButtonsNullPointer(void) {
    button_t* testPtr = NULL;
    uint32_t retVal = initButton(testPtr, portA, 1, fakePressedAction, fakeReleasedAction);
    TEST_ASSERT_EQUAL_INT32(BTN_NULL_POINTER_PASSED, retVal);
}
////
// init Quadrature encoder
////
// Fake, empty actions for the encoder
void fakeCWAction(void) {}
void fakeCCWAction(void) {}

void test_initQuadratureDoesItsJob(void) {
    uint32_t pin0No = 1;
    pinPort_t port0 = portB;
    uint32_t pin1No = 2;
    pinPort_t port1 = portC;
    quad_encoder_t testQuad = {0};
    uint32_t retVal = initQuadEncoder(&testQuad, port0, pin0No, port1, pin1No, fakeCWAction, fakeCCWAction);
    TEST_ASSERT_EQUAL_UINT32(BTN_OK, retVal);
    TEST_ASSERT_EQUAL_UINT32(pin0No, testQuad.pin0No);
    TEST_ASSERT_EQUAL_UINT32(port0, testQuad.pin0Port);
    TEST_ASSERT_EQUAL_UINT32(pin1No, testQuad.pin1No);
    TEST_ASSERT_EQUAL_UINT32(port1, testQuad.pin1Port);
    TEST_ASSERT_EQUAL_PTR(fakeCWAction, testQuad.clockWiseAction);
    TEST_ASSERT_EQUAL_PTR(fakeCCWAction, testQuad.counterClockWiseAction);
}

void test_initQuadrature_QuadIsNull(void) {
    quad_encoder_t* quadPtr = NULL;
    uint32_t retVal = initQuadEncoder(quadPtr, 0, 0, 0, 0, NULL, NULL);
    TEST_ASSERT_EQUAL_UINT32(BTN_NULL_POINTER_PASSED, retVal);
}

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
    uint32_t intNo = 0;

    setInterruptCallbackWCtx_ExpectAndReturn(testBtn.pinNo, fakeButtonCallback, &testBtn, testBtn.pinNo);
    configurePinInterrupt_Expect(testBtn.btnPort, testBtn.pinNo, testBtn.pinNo, true, true, true);
    enablePinInterrupts_Expect(1 << testBtn.pinNo);
    uint32_t retVal = configureButtonInterrupts(&testBtn, fakeButtonCallback, &intNo);
    TEST_ASSERT_EQUAL_UINT32(BTN_OK, retVal);
    TEST_ASSERT_EQUAL_UINT32(testBtn.pinNo, intNo);
}

// Test configureGPIOInterrupts, but setting the interrupt fails with INTERRUPT_UNAVAILABLE
void test_configureButtonInterrupts_NoInterruptsAvailable(void) {
    button_t testBtn = {
        .btnPort = portA,
        .pinNo = 1,
    };
    uint32_t intNoReturned = 0;

    setInterruptCallbackWCtx_ExpectAndReturn(testBtn.pinNo, fakeButtonCallback, &testBtn, 0xFF);
    // No other calls have to be made and the function exits
    uint32_t retVal = configureButtonInterrupts(&testBtn, fakeButtonCallback, &intNoReturned);
    TEST_ASSERT_EQUAL_UINT32(BTN_NO_INTS_AVAILABLE, retVal);
}

void test_configButtonInts_PassedButtonPtrIsNull(void) {
    button_t* btnPtr = NULL;
    uint32_t intNoReturned = 0;
    uint32_t retVal = configureButtonInterrupts(btnPtr, fakeButtonCallback, &intNoReturned);
    TEST_ASSERT_EQUAL_UINT32(BTN_NULL_POINTER_PASSED, retVal);
}

void test_configButtonInts_intNumberPtrIsNull(void) {
    button_t btn = {0};
    uint32_t retVal = configureButtonInterrupts(&btn, fakeButtonCallback, NULL);
    TEST_ASSERT_EQUAL_UINT32(BTN_NULL_POINTER_PASSED, retVal);
}

////
// configureQuadratureInterrupts
////

// fake empty callback function
void fakeQuadCallback(uint8_t intNo, void* ctx) {}

void test_configQuadInts_ConfigsInts(void) {
    quad_encoder_t testQuad = {
        .pin0No = 1,
        .pin0Port = portA,
    };
    uint32_t pinNoReturned = 0;

    setInterruptCallbackWCtx_ExpectAndReturn(testQuad.pin0No, fakeQuadCallback, &testQuad, testQuad.pin0No);
    configurePinInterrupt_Expect(testQuad.pin0Port, testQuad.pin0No, testQuad.pin0No, false, true, true);
    enablePinInterrupts_Expect(1 << testQuad.pin0No);
    uint32_t retVal = configureQuadratureInterrupts(&testQuad, fakeQuadCallback, &pinNoReturned);
    TEST_ASSERT_EQUAL_HEX32(BTN_OK, retVal);
}

void test_configQuadInts_AllIntsInUse(void) {
    quad_encoder_t testQuad = {
        .pin0No = 1,
        .pin0Port = portA,
    };
    uint32_t pinNoReturned = 0;

    setInterruptCallbackWCtx_ExpectAndReturn(testQuad.pin0No, fakeQuadCallback, &testQuad, 0xFF);
    uint32_t retVal = configureQuadratureInterrupts(&testQuad, fakeQuadCallback, &pinNoReturned);
    TEST_ASSERT_EQUAL_HEX32(BTN_NO_INTS_AVAILABLE, retVal);
}

void test_configQuadInts_NullPointerGuards_QuadPointer(void) {
    quad_encoder_t* quadPtr = NULL;
    uint32_t pinNoReturned = 0;

    uint32_t retVal = configureQuadratureInterrupts(quadPtr, fakeQuadCallback, &pinNoReturned);
    TEST_ASSERT_EQUAL_HEX32(BTN_NULL_POINTER_PASSED, retVal);
}

void test_configQuadInts_NullPointerGuards_IntNumberPointer(void) {
    quad_encoder_t testQuad = {0};
    uint32_t* pinNoPtr = NULL;

    uint32_t retVal = configureQuadratureInterrupts(&testQuad, fakeQuadCallback, pinNoPtr);
    TEST_ASSERT_EQUAL_HEX32(BTN_NULL_POINTER_PASSED, retVal);
}