#ifndef APP_ASSERT_H
#define APP_ASSERT_H

/*****************************************************************************************
 *
 * FAKE APP_ASSERT.H USED IN TESTS. CONTAINS THE MINIMUM AMOUNT OF IT TO RUN THEM
 *
 ****************************************************************************************/

// #ifdef PRINT_ENABLED
// #define app_assert_status_f(...) \
//     do {                         \
//         printf("ASSERTING! ");   \
//         printf(__VA_ARGS__);     \
//     } while (0);
// #else
// #define app_assert_status_f(...)
// #endif  // PRINT_ENABLED

// #ifdef PRINT_ENABLED

// TODO: Work on this, print the assert message and make it work so test pass on assert and fail when it shouldn't
#define app_assert(expr, ...)                \
    if (!(expr)) {                           \
        printf("WORK ON ASSERTS!!!!!!\r\n"); \
        printf("ASSERTING!\r\n");            \
        printf(#expr);                       \
        printf(" -> ");                      \
        printf(__VA_ARGS__);                 \
        printf("\r\n");                      \
    }

// #else
// #define app_assert(expr, ...) printf("NOTPRINTING");
// #endif  // PRINT_ENABLED

#endif  // APP_ASSERT_H