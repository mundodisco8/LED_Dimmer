#include "testableAsserts.h"

#include <stdbool.h>
#include <stdio.h>
#include <unity.h>

/**
 * @brief Flag that controls if assertions are expected on this test
 */
static bool plannedAssertsFlag = false;

/**
 * @brief Resets the state of the plannedAssertsFlag.
 * @note call on the setUp()
 * @param  none
 */
void assertSetUp(void) { plannedAssertsFlag = false; }

/**
 * @brief Check at tearDown for expected assertions. If we were expected to assert, but didn't, this code catches it and
 * marks the test as failed
 * @param None
 */
void assertTearDownCheck(void) {
    if (plannedAssertsFlag) {
        TEST_FAIL_MESSAGE("Test failed. An assertion was expected, but it didn't happen!");
    }
}

/**
 * @brief Marks this test to expect an assertion.
 * If code asserts unexpectedly, the test will fail, but if we are testing what happen when we assert, then we can mark
 * it and then the assertion would not fail the test (it would actually pass it)
 * @param  None
 */
void assertExpectFailure(void) { plannedAssertsFlag = true; }

/**
 * @brief A buffer to hold the string representation of the condition in the assert, in order to print it
 */
char bufferino[256] = {0};

/**
 * @brief A function to test asserts.
 * This function replaces an assert in the code. We can mark the assertion as expected and test that the code asserts
 * successfully when it should, and also it triggers a test error when the assertion is not expected, to let you correct
 * the mistake without needing to run the code.
 *
 * @param expr the expression evaluated, as a string, so it can be printed
 * @param msg the message passed to app_assert to print in case of assertion
 */
void testable_appAssert(char* expr, char* msg) {
    if (!plannedAssertsFlag) {
        printf("ASSERTING!\r\n");
        printf("Expression \"%s -> %s\" failed ", expr, msg);
        printf("\r\n");
        TEST_FAIL_MESSAGE("Unexpected Assertion Triggered");
    } else {
        //The tearDown still runs, set the false to flag as to not trigger the tearDown check. Make sure it's called
        //before the TEST_PASS()
        plannedAssertsFlag = false;
        //We expected the assertion, end the test here, as there's nothing else to do
        TEST_PASS();
    }
}