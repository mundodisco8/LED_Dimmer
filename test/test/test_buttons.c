#include "buttons.h"

#include <unity.h>

#include <string.h>
#include "inttypes.h"

#include "mock_gpio_HW.h"
#include "mock_interrupt_HW.h"
#include "mock_sleepyTimers_HW.h"

void setUp() {
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
    pinPort_t expectedPort = portA;
    uint8_t expectedPin = 1;
    pinMode_t expectedMode = MODE_INPUT;
    bool expectedDout = false;
    // Test Magic: I need to return a pointer to valid memory. The "right" thing to do would be to define two timerHandler
    // structs, and get pointers to them, but buttons.c does not know anything about the structs (we are using an
    // incomplete) type. The code is not going to use the memory, so we can just define two variables, and use their
    // memory as if it was a timerhHandler.

    uint8_t dummy1 = 0;
    uint8_t dummy2 = 0;

    // Set Expectations
    setPinMode_Expect(expectedPort, expectedPin, expectedMode, expectedDout);
    SLP_reserveTimer_ExpectAndReturn(&(testBtn.debounceTimerPtr), SLPTIMER_OK);
    SLP_reserveTimer_ReturnThruPtr_handlePtr(&dummy1);
    SLP_reserveTimer_ExpectAndReturn(&(testBtn.samplingTimerPtr), SLPTIMER_OK);
    SLP_reserveTimer_ReturnThruPtr_handlePtr(&dummy2);

    uint32_t retVal = initButton(&testBtn, expectedPort, expectedPin, fakePressedAction, fakeReleasedAction);
    // Check retVal is true
    TEST_ASSERT_EQUAL_UINT32(BTN_OK, retVal);
    TEST_ASSERT_EQUAL_UINT32(expectedPort, testBtn.btnPort);
    TEST_ASSERT_EQUAL_UINT32(expectedPin, testBtn.pinNo);
    TEST_ASSERT_EQUAL_UINT32(BUTTON_RELEASED, testBtn.state);
    TEST_ASSERT_EQUAL_INT32(0, testBtn.integrator);
    TEST_ASSERT_EQUAL_PTR(fakePressedAction, testBtn.pressedAction);
    TEST_ASSERT_EQUAL_PTR(fakeReleasedAction, testBtn.releasedAction);
    TEST_ASSERT_NOT_NULL(testBtn.debounceTimerPtr);
    TEST_ASSERT_NOT_NULL(testBtn.samplingTimerPtr);
}

// Protection against null pointers: because of shortcircuiting in OR comparisons, there's only need to test the three
// checks individually, and that will cover all the cases of the if statement
void test_initButtons_ButtonIsNullPointer(void) {
    button_t* testPtr = NULL;
    uint32_t retVal = initButton(testPtr, portA, 1, fakePressedAction, fakeReleasedAction);
    TEST_ASSERT_EQUAL_INT32(BTN_NULL_POINTER_PASSED, retVal);
}

// TODO: is needed?
void test_initButtons_TimersAreNullPtrs(void) {
    button_t* testPtr = {0};
    uint32_t retVal = initButton(testPtr, portA, 1, fakePressedAction, fakeReleasedAction);
    TEST_ASSERT_EQUAL_INT32(BTN_NULL_POINTER_PASSED, retVal);

    retVal = initButton(testPtr, portA, 1, fakePressedAction, fakeReleasedAction);
    TEST_ASSERT_EQUAL_INT32(BTN_NULL_POINTER_PASSED, retVal);

}

////
// init Quadrature encoder
////
// Fake, empty actions for the encoder
void fakeCWAction(void) {}
void fakeCCWAction(void) {}

void test_initQuadratureDoesItsJob(void) {
    uint8_t expectedPin0No = 1;
    pinPort_t expectedPortPin0 = portB;
    uint8_t expectedPin1No = 2;
    pinPort_t expectedPortPin1 = portC;
    pinMode_t expectedPinMode = MODE_INPUT_PULL;
    bool expectedDout = false;
    quad_encoder_t testQuad = {0};

    // Set Expectations
    setPinMode_Expect(expectedPortPin0, expectedPin0No, expectedPinMode, expectedDout);
    setPinMode_Expect(expectedPortPin1, expectedPin1No, expectedPinMode, expectedDout);

    uint32_t retVal = initQuadEncoder(&testQuad, expectedPortPin0, expectedPin0No, expectedPortPin1, expectedPin1No, fakeCWAction, fakeCCWAction);
    TEST_ASSERT_EQUAL_UINT32(BTN_OK, retVal);
    TEST_ASSERT_EQUAL_UINT32(expectedPin0No, testQuad.pin0No);
    TEST_ASSERT_EQUAL_UINT32(expectedPortPin0, testQuad.pin0Port);
    TEST_ASSERT_EQUAL_UINT32(expectedPin1No, testQuad.pin1No);
    TEST_ASSERT_EQUAL_UINT32(expectedPortPin1, testQuad.pin1Port);
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