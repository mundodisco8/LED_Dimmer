/****
 * Fake App Log Config
 *
 * This file is a replacement of app_log_config.
 *
 * If logging is enabled, app_log_level() and its variants end up requiring a function called app_log_append().
 * This function is defined in a macro (if APP_LOG_ENABLE = 1) as:
 *      #define app_log_append(...)            \
 *          sl_iostream_printf(app_log_iostream, \
 *                              __VA_ARGS__)
 * Because sl_iostream_printf() is defined elsewhere, is not included in the mocks, and the test won't compile.
 * I tried a couple of things, but I thought that the best way to tackle this is to just control APP_LOG_LEVEL.
 * Because is set to 1 in the project, in a file, it's not possible to change it with a compiler flag. I created
 * this fake file to set it to 0.
 *
 * There's more stuff on app_log_config, but it seems it's not needed for now.
 */

#define APP_LOG_ENABLE 0