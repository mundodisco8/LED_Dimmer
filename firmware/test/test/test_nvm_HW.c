#include <unity.h>

#include "nvm_HW.h"

// Silab's headers
#include "app_log.h"
#include "mock_nvm3_generic.h"
// #include "nvm3.h"
#include "nvm3_default_config.h"  // Config stuff to use the default nvm3 area and not need to mess with the linkerfile
#include "nvm3_hal_flash.h"       // driver for internal flash on nvm3
#include "sl_status.h"

// our headers
#include "mock_effectControl.h"  // driver for internal flash on nvm3

// Static data that we access in the test
extern nvm3_Init_t nvm3_init_data;
extern nvm3_Handle_t handle;

/*
 * Unit tests for "nvm_HW.h"
 */

void setUp(void) {}

void tearDown(void) {}

////
// NVM init
////

void test_nvm_init_Success(void) {
    // Set Expectations
    nvm3_open_ExpectAndReturn(&handle, &nvm3_init_data, SL_STATUS_OK);

    nvm_init();
}

////
// nvm_readLEDContext
////

void test_nvm_readLEDContext_Success(void) {
    LEDContext_t testContext = {0};
    nvmKeys_t testKey        = NVM3_KEY_LED1_CONTEXT;

    // Set Expectations
    uint32_t expectedObejcts     = 3;
    size_t expectedLen           = sizeof(LEDContext_t);
    LEDContext_t expectedContext = {.currAnimation = ANIM_FIXED, .currBreathePeriod = 1000UL, .currBrightness = 5000UL};
    uint32_t expectedType        = NVM3_OBJECTTYPE_DATA;
    // Count nvm objects
    // NOTE: countObjects is an inline to call enumObjects. countObjects doesn't get mocked, but enumObjects does.
    // Finally, during compilation, enumObjects is replaced for countObjects and that way, we can mock
    // Not countobjects, because it calls enumObjects? inlining stuff...
    // nvm3_countObjects_ExpectAndReturn(&handle, testObejcts);
    nvm3_enumObjects_ExpectAndReturn(&handle, NULL, 0, NVM3_KEY_MIN, NVM3_KEY_MAX, expectedObejcts);
    // Get nvm object
    nvm3_getObjectInfo_ExpectAndReturn(&handle, testKey, NULL, NULL, SL_STATUS_OK);
    nvm3_getObjectInfo_IgnoreArg_type();  // can't reach the data, as it's internal in the function's context
    nvm3_getObjectInfo_ReturnThruPtr_type(&expectedType);
    nvm3_getObjectInfo_IgnoreArg_len();  // can't reach the data, as it's internal in the function's context
    nvm3_getObjectInfo_ReturnThruPtr_len(&expectedLen);
    // read nvm object
    nvm3_readData_ExpectAndReturn(&handle, testKey, &testContext, sizeof(LEDContext_t), SL_STATUS_OK);
    nvm3_readData_ReturnThruPtr_value(&expectedContext);

    nvm_readLEDContext(testKey, &testContext);
    TEST_ASSERT_EQUAL_UINT32((uint32_t)expectedContext.currAnimation, (uint32_t)testContext.currAnimation);
    TEST_ASSERT_EQUAL_UINT32(expectedContext.currBreathePeriod, testContext.currBreathePeriod);
    TEST_ASSERT_EQUAL_UINT32(expectedContext.currBrightness, testContext.currBrightness);
}

void test_nvm_readLEDContext_NVMIsEmpty(void) {
    LEDContext_t testContext = {0};
    nvmKeys_t testKey        = NVM3_KEY_LED1_CONTEXT;

    // Set Expectations
    uint32_t expectedObjects = 0;
    // Count nvm objects
    nvm3_enumObjects_ExpectAndReturn(&handle, NULL, 0, NVM3_KEY_MIN, NVM3_KEY_MAX, expectedObjects);

    nvm_readLEDContext(testKey, &testContext);
}

void test_nvm_readLEDContext_ObjectNotFound(void) {
    LEDContext_t testContext = {0};
    nvmKeys_t testKey        = NVM3_KEY_LED1_CONTEXT;

    // Set Expectations
    uint32_t expecetdObejcts = 3;
    // Count nvm objects
    // Not countobjects, because it calls enumObjects? inlining stuff...
    // nvm3_countObjects_ExpectAndReturn(&handle, testObejcts);
    nvm3_enumObjects_ExpectAndReturn(&handle, NULL, 0, NVM3_KEY_MIN, NVM3_KEY_MAX, expecetdObejcts);
    // Get nvm object
    nvm3_getObjectInfo_ExpectAndReturn(&handle, testKey, NULL, NULL, SL_STATUS_NOT_FOUND);
    nvm3_getObjectInfo_IgnoreArg_type();  // can't reach the data, as it's internal in the function's context
    nvm3_getObjectInfo_IgnoreArg_len();   // can't reach the data, as it's internal in the function's context

    nvm_readLEDContext(testKey, &testContext);
}

void test_nvm_readLEDContext_ErrorFindingObject(void) {
    LEDContext_t testContext = {0};
    nvmKeys_t testKey        = NVM3_KEY_LED1_CONTEXT;

    // Set Expectations
    uint32_t expectedObejcts = 3;
    // Count nvm objects
    // Not countobjects, because it calls enumObjects? inlining stuff...
    // nvm3_countObjects_ExpectAndReturn(&handle, testObejcts);
    nvm3_enumObjects_ExpectAndReturn(&handle, NULL, 0, NVM3_KEY_MIN, NVM3_KEY_MAX, expectedObejcts);
    // Get nvm object
    nvm3_getObjectInfo_ExpectAndReturn(&handle, testKey, NULL, NULL, SL_STATUS_FAIL);
    nvm3_getObjectInfo_IgnoreArg_type();  // can't reach the data, as it's internal in the function's context
    nvm3_getObjectInfo_IgnoreArg_len();   // can't reach the data, as it's internal in the function's context

    nvm_readLEDContext(testKey, &testContext);
}

void test_nvm_readLEDContext_WrongObjectDataLenght(void) {
    LEDContext_t testContext = {0};
    nvmKeys_t testKey        = NVM3_KEY_LED1_CONTEXT;

    // Set Expectations
    uint32_t expectedObejcts = 3;
    size_t expectedLen       = 100;
    uint32_t expectedType    = NVM3_OBJECTTYPE_DATA;
    // Count nvm objects
    // Not countobjects, because it calls enumObjects? inlining stuff...
    // nvm3_countObjects_ExpectAndReturn(&handle, testObejcts);
    nvm3_enumObjects_ExpectAndReturn(&handle, NULL, 0, NVM3_KEY_MIN, NVM3_KEY_MAX, expectedObejcts);
    // Get nvm object
    nvm3_getObjectInfo_ExpectAndReturn(&handle, testKey, NULL, NULL, SL_STATUS_OK);
    nvm3_getObjectInfo_IgnoreArg_type();  // can't reach the data, as it's internal in the function's context
    nvm3_getObjectInfo_ReturnThruPtr_type(&expectedType);
    nvm3_getObjectInfo_IgnoreArg_len();  // can't reach the data, as it's internal in the function's context
    nvm3_getObjectInfo_ReturnThruPtr_len(&expectedLen);
    // Erase all data
    nvm3_eraseAll_ExpectAndReturn(&handle, SL_STATUS_OK);

    nvm_readLEDContext(testKey, &testContext);
}

void test_nvm_readLEDContext_WrongObjectType(void) {
    LEDContext_t testContext = {0};
    nvmKeys_t testKey        = NVM3_KEY_LED1_CONTEXT;

    // Set Expectations
    uint32_t expectedObejcts = 3;
    size_t expectedLen       = sizeof(LEDContext_t);
    uint32_t expectedType    = NVM3_OBJECTTYPE_COUNTER;
    // Count nvm objects
    // Not countobjects, because it calls enumObjects? inlining stuff...
    // nvm3_countObjects_ExpectAndReturn(&handle, testObejcts);
    nvm3_enumObjects_ExpectAndReturn(&handle, NULL, 0, NVM3_KEY_MIN, NVM3_KEY_MAX, expectedObejcts);
    // Get nvm object
    nvm3_getObjectInfo_ExpectAndReturn(&handle, testKey, NULL, NULL, SL_STATUS_OK);
    nvm3_getObjectInfo_IgnoreArg_type();  // can't reach the data, as it's internal in the function's context
    nvm3_getObjectInfo_ReturnThruPtr_type(&expectedType);
    nvm3_getObjectInfo_IgnoreArg_len();  // can't reach the data, as it's internal in the function's context
    nvm3_getObjectInfo_ReturnThruPtr_len(&expectedLen);

    nvm_readLEDContext(testKey, &testContext);
}

void test_nvm_readLEDContext_ErrorReadingData(void) {
    LEDContext_t testContext = {0};
    nvmKeys_t testKey        = NVM3_KEY_LED1_CONTEXT;

    // Set Expectations
    uint32_t expectedObejcts     = 3;
    size_t expectedLen           = sizeof(LEDContext_t);
    LEDContext_t expectedContext = {.currAnimation = ANIM_FIXED, .currBreathePeriod = 1000UL, .currBrightness = 5000UL};
    uint32_t expectedType        = NVM3_OBJECTTYPE_DATA;
    // Count nvm objects
    // Not countobjects, because it calls enumObjects? inlining stuff...
    // nvm3_countObjects_ExpectAndReturn(&handle, testObejcts);
    nvm3_enumObjects_ExpectAndReturn(&handle, NULL, 0, NVM3_KEY_MIN, NVM3_KEY_MAX, expectedObejcts);
    // Get nvm object
    nvm3_getObjectInfo_ExpectAndReturn(&handle, testKey, NULL, NULL, SL_STATUS_OK);
    nvm3_getObjectInfo_IgnoreArg_type();  // can't reach the data, as it's internal in the function's context
    nvm3_getObjectInfo_ReturnThruPtr_type(&expectedType);
    nvm3_getObjectInfo_IgnoreArg_len();  // can't reach the data, as it's internal in the function's context
    nvm3_getObjectInfo_ReturnThruPtr_len(&expectedLen);
    // read nvm object
    nvm3_readData_ExpectAndReturn(&handle, testKey, &testContext, sizeof(LEDContext_t), SL_STATUS_FAIL);
    nvm3_readData_ReturnThruPtr_value(&expectedContext);

    nvm_readLEDContext(testKey, &testContext);
    TEST_ASSERT_EQUAL_UINT32((uint32_t)expectedContext.currAnimation, (uint32_t)testContext.currAnimation);
    TEST_ASSERT_EQUAL_UINT32(expectedContext.currBreathePeriod, testContext.currBreathePeriod);
    TEST_ASSERT_EQUAL_UINT32(expectedContext.currBrightness, testContext.currBrightness);
}

////
// nvm_writeLEDContext
////

void test_nvm_writeLEDContext_Success(void) {
    LEDContext_t testContext = {.currAnimation = ANIM_FIXED, .currBreathePeriod = 1000UL, .currBrightness = 5000UL};
    nvmKeys_t testKey        = NVM3_KEY_LED1_CONTEXT;

    // Set Expectations
    nvm3_writeData_ExpectAndReturn(&handle, testKey, &testContext, sizeof(LEDContext_t), SL_STATUS_OK);

    nvm_writeLEDContext(testKey, &testContext);
}

void test_nvm_writeLEDContext_Error(void) {
    LEDContext_t testContext = {.currAnimation = ANIM_FIXED, .currBreathePeriod = 1000UL, .currBrightness = 5000UL};
    nvmKeys_t testKey        = NVM3_KEY_LED1_CONTEXT;

    // Set Expectations
    nvm3_writeData_ExpectAndReturn(&handle, testKey, &testContext, sizeof(LEDContext_t), SL_STATUS_INVALID_KEY);

    nvm_writeLEDContext(testKey, &testContext);
}

////
// nvm checkForRepack
////

void test_nvmCheckForRepack_RepackNeeded(void) {
    // Set Expectations
    nvm3_repackNeeded_ExpectAndReturn(&handle, true);
    nvm3_repack_ExpectAndReturn(&handle, SL_STATUS_OK);

    nvm_checkForRepack();
}

void test_nvmCheckForRepack_RepackNotNeeded(void) {
    // Set Expectations
    nvm3_repackNeeded_ExpectAndReturn(&handle, false);

    nvm_checkForRepack();
}