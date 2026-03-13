#include <unity.h>

#include "buttons.h"

// Rest of the headers required
#include "app_assert.h"
#include "app_log.h"
#include "mock_debounce.h"
#include "mock_gpio_HW.h"
#include "mock_interrupt_HW.h"
#include "mock_sleepyTimers_HW.h"

// Forward declarations for STATIC elements of the module
extern void samplingTimerCallback(timerHandlePtr_t handlePtr, void* data);
extern void longPressTimerCallback(timerHandlePtr_t handlePtr, void* data);

// Some timer handlers for the buttons
uint32_t dummyA = 0;
uint32_t dummyB = 0;
timerHandlePtr_t longPressTimerPtr = (timerHandlePtr_t)&dummyA;  // give some non-null values to the pointers
timerHandlePtr_t samplingTimerPtr = (timerHandlePtr_t)&dummyB;

// Values for globals lost in the mocking process
const uint32_t DEBOUNCE_SAMPLING_PERIOD_MS = 5UL;  // read the button every this ms

// Fake Actions
void fakePressedAction(void* ctx) { (void)ctx; }

bool fakeLongPressedActionFlag = false;
void fakeLongPressedAction(void* ctx) {
    (void)ctx;
    fakeLongPressedActionFlag = true;
}

void fakeReleasedAction(void* ctx) { (void)ctx; }

void setUp() {
    // test assertions
    assertSetUp();
    fakeLongPressedActionFlag = false;
}

void tearDown() {
    // check assertions
    assertTearDownCheck();
}

////
// init Buttons
////

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
    SLP_reserveTimer_ExpectAndReturn(&(testBtn.longPressTimerPtr), SLPTIMER_OK);
    SLP_reserveTimer_ReturnThruPtr_handlePtr((timerHandlePtr_t*)&dummy1);
    SLP_reserveTimer_ExpectAndReturn(&(testBtn.samplingTimerPtr), SLPTIMER_OK);
    SLP_reserveTimer_ReturnThruPtr_handlePtr((timerHandlePtr_t*)&dummy2);

    initButton(&testBtn, expectedPort, expectedPin, fakePressedAction, fakeLongPressedAction, fakeReleasedAction);
    // Check retVal is true
    TEST_ASSERT_EQUAL_UINT32(expectedPort, testBtn.btnPort);
    TEST_ASSERT_EQUAL_UINT32(expectedPin, testBtn.pinNo);
    TEST_ASSERT_EQUAL_UINT32(BUTTON_RELEASED, testBtn.state);
    TEST_ASSERT_EQUAL_INT32(0, testBtn.integrator);
    TEST_ASSERT_EQUAL_PTR(fakePressedAction, testBtn.pressedAction);
    TEST_ASSERT_EQUAL_PTR(fakeLongPressedAction, testBtn.longPressedAction);
    TEST_ASSERT_EQUAL_PTR(fakeReleasedAction, testBtn.releasedAction);
    TEST_ASSERT_NOT_NULL(testBtn.longPressTimerPtr);
    TEST_ASSERT_NOT_NULL(testBtn.samplingTimerPtr);
}

void test_initButtons_FirstReservedTimerFails(void) {
    button_t testBtn = {0};

    // Set expectations
    setPinMode_Ignore();
    // Expect assert
    assertExpectFailure();
    SLP_reserveTimer_ExpectAnyArgsAndReturn(SLPTIMER_NO_TIMERS_AVAILABLE);
    // Only one call to SLP_reserveTimer

    initButton(&testBtn, 0, 0, NULL, NULL, NULL);
}

void test_initButtons_SecondReservedTimerFails(void) {
    button_t testBtn = {0};

    // Test second timer assert
    // Set expectations
    setPinMode_Ignore();
    // Expect assert
    assertExpectFailure();
    SLP_reserveTimer_ExpectAnyArgsAndReturn(SLPTIMER_OK);
    SLP_reserveTimer_ExpectAnyArgsAndReturn(SLPTIMER_NO_TIMERS_AVAILABLE);

    initButton(&testBtn, 0, 0, NULL, NULL, NULL);
}

// Protection against null pointers: because of shortcircuiting in OR comparisons, there's only need to test the three
// checks individually, and that will cover all the cases of the if statement
void test_initButtons_ButtonIsNullPointer(void) {
    button_t* testPtr = NULL;

    // Set expectations
    assertExpectFailure();

    initButton(testPtr, portA, 1, fakePressedAction, fakeLongPressedAction, fakeReleasedAction);
}

////
// init Quadrature encoder
////
// Fake, empty actions for the encoder
void fakeCWAction(void* ctx) { (void)ctx; }
void fakeCCWAction(void* ctx) { (void)ctx; }

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

    initQuadEncoder(&testQuad, expectedPortPin0, expectedPin0No, expectedPortPin1, expectedPin1No, fakeCWAction,
                    fakeCCWAction);
    TEST_ASSERT_EQUAL_UINT32(expectedPin0No, testQuad.pin0No);
    TEST_ASSERT_EQUAL_UINT32(expectedPortPin0, testQuad.pin0Port);
    TEST_ASSERT_EQUAL_UINT32(expectedPin1No, testQuad.pin1No);
    TEST_ASSERT_EQUAL_UINT32(expectedPortPin1, testQuad.pin1Port);
    TEST_ASSERT_EQUAL_PTR(fakeCWAction, testQuad.clockWiseAction);
    TEST_ASSERT_EQUAL_PTR(fakeCCWAction, testQuad.counterClockWiseAction);
}

void test_initQuadrature_QuadIsNull(void) {
    quad_encoder_t* quadPtr = NULL;

    // Set expectations
    assertExpectFailure();

    initQuadEncoder(quadPtr, 0, 0, 0, 0, NULL, NULL);
}

////
// configureButtonInterrupts
////

void fakeButtonCallback(uint8_t intNo, void* ctx) {
    (void)intNo;
    (void)ctx;
}

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
    configureButtonInterrupts(&testBtn, fakeButtonCallback, &intNo);
    TEST_ASSERT_EQUAL_UINT32(testBtn.pinNo, intNo);
}

// Test configureGPIOInterrupts, but setting the interrupt fails with INTERRUPT_UNAVAILABLE
void test_configureButtonInterrupts_NoInterruptsAvailable(void) {
    button_t testBtn = {
        .btnPort = portA,
        .pinNo = 1,
    };
    uint32_t testIntNo = 0;
    uint32_t intNoReturned = 0xFF;

    // Set expectations
    assertExpectFailure();
    setInterruptCallbackWCtx_ExpectAndReturn(testBtn.pinNo, fakeButtonCallback, &testBtn, intNoReturned);

    // No other calls have to be made and the function exits
    configureButtonInterrupts(&testBtn, fakeButtonCallback, &testIntNo);
}

void test_configButtonInts_PassedButtonPtrIsNull(void) {
    button_t* btnPtr = NULL;
    uint32_t intNoReturned = 0;

    // Set expectations
    assertExpectFailure();

    configureButtonInterrupts(btnPtr, fakeButtonCallback, &intNoReturned);
}

void test_configButtonInts_intNumberPtrIsNull(void) {
    button_t btn = {0};

    // Set expectations
    assertExpectFailure();

    configureButtonInterrupts(&btn, fakeButtonCallback, NULL);
}

////
// configureQuadratureInterrupts
////

// fake empty callback function
void fakeQuadCallback(uint8_t intNo, void* ctx) {
    (void)intNo;
    (void)ctx;
}

void test_configQuadInts_ConfigsInts(void) {
    quad_encoder_t testQuad = {
        .pin0No = 1,
        .pin0Port = portA,
    };
    uint32_t pinNoReturned = 0;

    setInterruptCallbackWCtx_ExpectAndReturn(testQuad.pin0No, fakeQuadCallback, &testQuad, testQuad.pin0No);
    configurePinInterrupt_Expect(testQuad.pin0Port, testQuad.pin0No, testQuad.pin0No, false, true, true);
    enablePinInterrupts_Expect(1 << testQuad.pin0No);
    configureQuadratureInterrupts(&testQuad, fakeQuadCallback, &pinNoReturned);
}

void test_configQuadInts_AllIntsInUse(void) {
    quad_encoder_t testQuad = {
        .pin0No = 1,
        .pin0Port = portA,
    };
    uint32_t pinNoReturned = 0;

    // Set expectations
    assertExpectFailure();

    setInterruptCallbackWCtx_ExpectAndReturn(testQuad.pin0No, fakeQuadCallback, &testQuad, 0xFF);
    configureQuadratureInterrupts(&testQuad, fakeQuadCallback, &pinNoReturned);
}

void test_configQuadInts_NullPointerGuards_QuadPointer(void) {
    quad_encoder_t* quadPtr = NULL;
    uint32_t pinNoReturned = 0;

    // Set expectations
    assertExpectFailure();

    configureQuadratureInterrupts(quadPtr, fakeQuadCallback, &pinNoReturned);
}

void test_configQuadInts_NullPointerGuards_IntNumberPointer(void) {
    quad_encoder_t testQuad = {0};
    uint32_t* pinNoPtr = NULL;

    // Set expectations
    assertExpectFailure();

    configureQuadratureInterrupts(&testQuad, fakeQuadCallback, pinNoPtr);
}

////
// buttonSetState
// - Success
// - Null pointer
////

void test_buttonSetState_Success(void) {
    button_t testButton = {0};
    buttonState_t testState = BUTTON_PRESSED;

    // Set Expectations
    buttonState_t expectedState = BUTTON_RELEASED;

    buttonSetState(&testButton, expectedState);

    TEST_ASSERT_EQUAL_UINT32(expectedState, testButton.state);
    TEST_ASSERT_EQUAL_UINT32(testState, testButton.prevState);
}

void test_buttonSetState_NullPointer(void) {
    // Set Expectations
    buttonState_t expectedState = BUTTON_RELEASED;

    // Set expectations
    assertExpectFailure();

    buttonSetState(NULL, expectedState);
}

void test_buttonGetState_Success(void) {
    button_t testButton = {0};

    // Set expectations
    buttonState_t expectedState = BUTTON_LONGPRESSED;
    testButton.state = expectedState;

    buttonState_t returnedState = buttonGetState(&testButton);
    TEST_ASSERT_EQUAL_UINT32(expectedState, returnedState);
}

////
// StartButtonTimer()
//
// Also tests getDebounceTime() and getDebounceSamplingPeriod() (statics called by it)
////

// 1) Start a sampling timer that is not already running
//    a) Also test that getDebounceSamplingPeriod() returns the right value
// 2) Start a sampling timer that IS already running
// 3) Checks that we hit the logging function when an error is returned by the SLP* functions

void test_startButtonTimer_SamplingTimerNotRunning(void) {
    button_t testButton = {.samplingTimerPtr = samplingTimerPtr};

    SLP_isTimerRunning_ExpectAndReturn(testButton.samplingTimerPtr, false);
    SLP_startTimer_ExpectAndReturn(testButton.samplingTimerPtr, DEBOUNCE_SAMPLING_PERIOD_MS, TIMER_ONE_SHOT,
                                   samplingTimerCallback, &testButton, SLPTIMER_OK);

    uint32_t retVal = startButtonTimer(&testButton, TIMER_SAMPLE);
    TEST_ASSERT_EQUAL_UINT32(BTN_OK, retVal);
}

void test_startButtonTimer_SamplingTimerISRunning(void) {
    button_t testButton = {.samplingTimerPtr = samplingTimerPtr};

    SLP_isTimerRunning_ExpectAndReturn(testButton.samplingTimerPtr, true);
    // startPeriodictimer is not called in this case

    uint32_t retVal = startButtonTimer(&testButton, TIMER_SAMPLE);
    TEST_ASSERT_EQUAL_UINT32(BTN_OK, retVal);
}

void test_startButtonTimer_LongPressTimer(void) {
    button_t testButton = {.longPressTimerPtr = longPressTimerPtr};

    SLP_isTimerRunning_ExpectAndReturn(testButton.longPressTimerPtr, false);
    SLP_startTimer_ExpectAndReturn(testButton.longPressTimerPtr, LONGPRESS_TIME_MS, TIMER_ONE_SHOT,
                                   longPressTimerCallback, &testButton, SLPTIMER_OK);
    // Sadly, the callback is a static function, so I can't test that startPeriodicTimer will be called with it as a parameter
    SLP_startTimer_IgnoreArg_callback();

    uint32_t retVal = startButtonTimer(&testButton, TIMER_LONGPRESS);
    TEST_ASSERT_EQUAL_UINT32(BTN_OK, retVal);
}

void test_startButtonTimer_WrongTypeOfTimer(void) {
    button_t testButton = {0};
    // Set expectations
    assertExpectFailure();

    startButtonTimer(&testButton, (btnTimerType_t)0xFFFFFFFF);
}

void test_startButtonTimer_StartTimerReturnsError(void) {
    button_t testButton = {.longPressTimerPtr = longPressTimerPtr};

    SLP_isTimerRunning_ExpectAndReturn(testButton.longPressTimerPtr, false);
    SLP_startTimer_ExpectAndReturn(testButton.longPressTimerPtr, LONGPRESS_TIME_MS, TIMER_ONE_SHOT,
                                   longPressTimerCallback, &testButton, SLPTIMER_ERROR);

    uint32_t retVal = startButtonTimer(&testButton, TIMER_LONGPRESS);
    TEST_ASSERT_EQUAL_UINT32(BTN_ERROR, retVal);
}

void test_startButtonTimer_NullPointer(void) {
    // Set expectations
    assertExpectFailure();

    startButtonTimer(NULL, TIMER_SAMPLE);
}

// This test that the log lines are hit. As in tests we disable logging, this is more something you would spot on the
// coverage report.
void test_startButtonTimer_ErrorOnSLP_startTimer(void) {
    button_t testButton = {.samplingTimerPtr = samplingTimerPtr};

    SLP_isTimerRunning_ExpectAndReturn(testButton.samplingTimerPtr, false);
    SLP_startTimer_ExpectAndReturn(testButton.samplingTimerPtr, DEBOUNCE_SAMPLING_PERIOD_MS, TIMER_ONE_SHOT, NULL,
                                   &testButton, SLPTIMER_ERROR);
    // Sadly, the callback is a static function, so I can't test that startPeriodicTimer will be called with it as a parameter
    SLP_startTimer_IgnoreArg_callback();

    btnError_t retVal = startButtonTimer(&testButton, TIMER_SAMPLE);
    TEST_ASSERT_EQUAL_UINT32(BTN_ERROR, retVal);
}

////
// StopButtonTimer()
// - Success
// - Do nothing if timer is not running
// - Return error if problems stopping timer
// - Assert on null pointer
////

void test_stopButtonTimer_Success(void) {
    button_t testBtn = {.samplingTimerPtr = samplingTimerPtr, .longPressTimerPtr = longPressTimerPtr};

    // Set Expectations
    btnError_t expectedRetVal = BTN_OK;
    SLP_isTimerRunning_ExpectAndReturn(testBtn.samplingTimerPtr, true);
    SLP_stopTimer_ExpectAndReturn(testBtn.samplingTimerPtr, SLPTIMER_OK);

    btnError_t retVal = stopButtonTimer(&testBtn, TIMER_SAMPLE);
    TEST_ASSERT_EQUAL_UINT32(expectedRetVal, retVal);

    // Set Expectations
    SLP_isTimerRunning_ExpectAndReturn(testBtn.longPressTimerPtr, true);
    SLP_stopTimer_ExpectAndReturn(testBtn.longPressTimerPtr, SLPTIMER_OK);

    retVal = stopButtonTimer(&testBtn, TIMER_LONGPRESS);
    TEST_ASSERT_EQUAL_UINT32(expectedRetVal, retVal);
}

void test_stopButtonTimer_TimerNotRunning(void) {
    button_t testBtn = {.samplingTimerPtr = samplingTimerPtr};

    // Set Expectations
    btnError_t expectedRetVal = BTN_OK;
    SLP_isTimerRunning_ExpectAndReturn(testBtn.samplingTimerPtr, false);

    btnError_t retVal = stopButtonTimer(&testBtn, TIMER_SAMPLE);
    TEST_ASSERT_EQUAL_UINT32(expectedRetVal, retVal);
}

void test_stopButtonTimer_ErrorStoppingTimer(void) {
    button_t testBtn = {.samplingTimerPtr = samplingTimerPtr};

    // Set Expectations
    btnError_t expectedRetVal = BTN_ERROR;
    SLP_isTimerRunning_ExpectAndReturn(testBtn.samplingTimerPtr, true);
    SLP_stopTimer_ExpectAndReturn(testBtn.samplingTimerPtr, SLPTIMER_ERROR);

    btnError_t retVal = stopButtonTimer(&testBtn, TIMER_SAMPLE);
    TEST_ASSERT_EQUAL_UINT32(expectedRetVal, retVal);
}

void test_stopButtonTimer_NullPointer(void) {
    // Set Expectations
    assertExpectFailure();

    stopButtonTimer(NULL, TIMER_SAMPLE);
}

void test_stopButtonTimer_WrongTypeOfTimer(void) {
    button_t testButton = {0};

    // Set expectations
    assertExpectFailure();

    stopButtonTimer(&testButton, (btnTimerType_t)0xFFFFFFFF);
}

// The sampling timer times out, so we run the debounceButton.
// Let's assume that the button is as default (released, integrator on 0, GPIO returns pressed)
void test_sleeptimerSamplingCallback(void) {
    // Let's make things easy and set the button in a way that debounceButton() does the bare minimum
    button_t testBtn = {0};

    // Set expectations
    debounceButton_Expect(&testBtn);

    // The timerHandle is not used, we can pass NULL
    samplingTimerCallback(NULL, &testBtn);
    // Check that the callback was called, as it should have reset debounceCycles
    TEST_ASSERT_EQUAL_UINT32(0UL, testBtn.debounceCycles);
}

// Changes the button state and calls its action if any
void test_sleeptimerLongPressCallback_Success(void) {
    button_t testBtn = {.state = BUTTON_PRESSED, .longPressedAction = fakeLongPressedAction};

    // Set expectations
    buttonState_t expectedState = BUTTON_LONGPRESSED;

    longPressTimerCallback(NULL, &testBtn);
    TEST_ASSERT_EQUAL_UINT32(expectedState, testBtn.state);
    // We expect the fakeLongPressActionFlag to be true, as it was executed
    TEST_ASSERT_TRUE(fakeLongPressedActionFlag);
}

// Changes the button state and calls its action if any
void test_sleeptimerLongPressCallback_SuccessButNoCallback(void) {
    button_t testBtn = {.state = BUTTON_PRESSED, .longPressedAction = NULL};

    // Set expectations
    buttonState_t expectedState = BUTTON_LONGPRESSED;

    longPressTimerCallback(NULL, &testBtn);
    TEST_ASSERT_EQUAL_UINT32(expectedState, testBtn.state);
    // We expect the fakeLongPressActionFlag to be false, as the button didn't have the callback loaded
    TEST_ASSERT_FALSE(fakeLongPressedActionFlag);
}