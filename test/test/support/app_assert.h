#ifndef APP_ASSERT_H
#define APP_ASSERT_H

/*****************************************************************************************
 *
 * FAKE APP_ASSERT.H USED IN TESTS. CONTAINS THE MINIMUM AMOUNT OF IT TO RUN THEM
 *
 ****************************************************************************************/

#ifdef PRINT_ENABLED
    #define app_assert_status_f(...)    \
        do{                             \
            printf("ASSERTING! ");          \
            printf (__VA_ARGS__);           \
    } while(0);
#else
    #define app_assert_status_f(...)
#endif // PRINT_ENABLED

#endif // APP_ASSERT_H