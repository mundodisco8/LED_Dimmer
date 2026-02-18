/**
 * @file testableAsserts.h
 * @author Joel Santos (jsantosrico@gmail.com)
 * @brief a crappy way to test assertions in unit testing
 * @version 0.1
 * @date 2026-02-17
 *
 * @copyright Copyright (c) 2026
 *
 * Testable assertions
 *
 * This module provides a way to test assertions. We can test that code that would assert will assert, and still mark it
 * as a passed test!
 *
 * Instructions - Setup:
 * - Add the `assertSetUp()` to the `setUp()` function and the `assertTearDownCheck()` to the `tearDown()`.
 * - If the code asserts and we were not expecting it, the test will fail an a message pointing to the line asserting
 *   will be printed in the test output.
 * - If we want to test the assert condition (expected assertions), call `assertExpectFailure()` in your test
 *   expectations. When the test asserts, the expectation will make the test pass.
 * - If we want to test the assert condition (expected assertions) but the test doesn't assert, the tearDown() will
 *   catch it and mark the test as failed and print a message regarding the failure.
 *
 * ```
 *  [in your test file]
 *
 *  void SetUp(void) {
        [...]
        assertSetUp();
        [...]
 *  }
 *
 *  void tearDown(void) {
        [...]
        assertTearDownCheck();
        [...]
 *  }
 * ```
 *
  * ```
 * [in a unit test]
 *      // Set Expectations
 *      assertExpectFailure();
 *      [...]
 *
 *      myFuncThatWillAssert();
 * ```
 */

#ifndef _TESTABLE_ASSERTIONS_H_
#define _TESTABLE_ASSERTIONS_H_

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>

/** @brief Resets the state of the plannedAssertsFlag.
 * @note call on the setUp()
 */
void assertSetUp(void);
/**
 * @brief Check at tearDown for expected assertions. If we were expected to assert, but didn't, this code catches it and
 * marks the test as failed
 * @param None
 */
void assertTearDownCheck(void);
/** @brief Marks this test to expect an assertion.
 */
void assertExpectFailure(void);

extern char bufferino[256];

// Replacement of SiLabs app_assert that can be tested
// Explanation of the macro:
// if the expression fails (assertion triggers), print the message in an intermediate buffer and pass it to
// testable_appAssert, together with the expresion that failed itself (as a string, not just the result), because
// sprintf would have a trailing comma `sprintf(bufferino, "My String", );`
// Example: if we have app_assert( a == 4, "a is not 4! it's actually %d", a), it would pass
//          - pass as false (5 != 4)
//          - expr as "a == 4"
//          - msg as "a is not 4! it's actually 5"
// NOTE: I found it fails if VA_ARGS is empty (if message is just a string, with no formatting).
// GCC supports __VA_OPT__, and I'm not sure how it works, so check the documentation
// I think the trick is that, if `...` has parameters, __VA_OPT__ is replaced with ", " and then followed by the
// __VA_ARGS__, and if it's empty, then there is no replacement and __VA_ARGS__ is replaced with "", so we don't have
// the trailing comma issue.
#define app_assert(expr, msg, ...)                          \
    if (!(expr)) {                                          \
        sprintf(bufferino, msg __VA_OPT__(, ) __VA_ARGS__); \
        testable_appAssert(#expr, bufferino);               \
    }

/** @brief A function to test asserts
 */
void testable_appAssert(char* expr, char* msg);

#endif  // _TESTABLE_ASSERTIONS_H_