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

// This define has to be passed to as a compilation flag for it to work
#ifndef DEBUG_LOG_SUPRESS

#define app_log_critical(msg, ...)                                        \
    {                                                                     \
        char buffer[256] = {0};                                           \
        snprintf(buffer, sizeof(buffer), msg __VA_OPT__(, ) __VA_ARGS__); \
        printf("%s - %s", "☠️", buffer);                                   \
    }
#define app_log_error(msg, ...)                                           \
    {                                                                     \
        char buffer[256] = {0};                                           \
        snprintf(buffer, sizeof(buffer), msg __VA_OPT__(, ) __VA_ARGS__); \
        printf("%s - %s", "💣", buffer);                                  \
    }
#define app_log_warning(msg, ...)                                         \
    {                                                                     \
        char buffer[256] = {0};                                           \
        snprintf(buffer, sizeof(buffer), msg __VA_OPT__(, ) __VA_ARGS__); \
        printf("%s - %s", "⚠️", buffer);                                   \
    }
#define app_log_info(msg, ...)                                            \
    {                                                                     \
        char buffer[256] = {0};                                           \
        snprintf(buffer, sizeof(buffer), msg __VA_OPT__(, ) __VA_ARGS__); \
        printf("%s - %s", "ℹ️", buffer);                                   \
    }
#define app_log_debug(msg, ...)                                           \
    {                                                                     \
        char buffer[256] = {0};                                           \
        snprintf(buffer, sizeof(buffer), msg __VA_OPT__(, ) __VA_ARGS__); \
        printf("%s - %s", "🐞", buffer);                                  \
    }
#else
#define app_log_error(msg, ...)
#define app_log_warning(msg, ...)
#define app_log_info(msg, ...)
#define app_log_debug(msg, ...)
#endif  // DEBUG_LOG_SUPRESS

#endif  // APP_LOG_H