#ifndef APP_LOG_H
#define APP_LOG_H

/*****************************************************************************************
 *
 * FAKE APP_LOG.H USED IN TESTS. CONTAINS THE MINIMUM AMOUNT OF IT TO RUN THEM
 *
 ****************************************************************************************/

#ifdef PRINT_ENABLED
    #define app_log_level(LEVEL, ...)    \
        do{                             \
            printf("%s: ", LEVEL);          \
            printf (__VA_ARGS__);           \
    } while(0);

    #define app_log_error(...) app_log_level("ERROR", __VA_ARGS__);
    #define app_log_warning(...) app_log_level("WARNING", __VA_ARGS__);
        #define app_log_info(...) app_log_level("INFO", __VA_ARGS__);
    #define app_log_debug(...) app_log_level("DEBUG", __VA_ARGS__);
#else
    #define app_log_error(...)
    #define app_log_warning(...)
    #define app_log_info(...)
    #define app_log_debug(...)
#endif // PRINT_ENABLED

#endif // APP_LOG_H