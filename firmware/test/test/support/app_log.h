#ifndef APP_LOG_H
#define APP_LOG_H

/*****************************************************************************************
 *
 * FAKE APP_LOG.H USED IN TESTS. CONTAINS THE MINIMUM AMOUNT OF IT TO RUN THEM
 * You will need to enable --verbosity=obnoxious to see the stdout paired with the test
 * that generated (otherwise is bundled together in the TEST OUTPUT section)
 *
 ****************************************************************************************/

#include "unity.h"

// #define app_log_error(msg, ...) testPrint("💣 ERROR", msg __VA_OPT(, ) __VA_ARGS__);
// #define app_log_warning(msg, ...) testPrint("⚠️ WARNING", msg __VA_OPT(, ) __VA_ARGS__);
#define app_log_error(msg, ...) \
    printf("💣 - ");            \
    printf(msg __VA_OPT__(, ) __VA_ARGS__);
#define app_log_debug(msg, ...) \
    printf("⚠️ - ");             \
    printf(msg __VA_OPT__(, ) __VA_ARGS__);
#define app_log_info(msg, ...) \
    printf("ℹ️ - ");            \
    printf(msg __VA_OPT__(, ) __VA_ARGS__);
#define app_log_debug(msg, ...) \
    printf("🐞 - ");            \
    printf(msg __VA_OPT__(, ) __VA_ARGS__);

#endif  // APP_LOG_H