#include <unity.h>

#include "nvm_HW.h"

// Silab's headers
#include "app_log.h"
#include "mock_nvm3_generic.h"
// #include "nvm3.h"
#include "nvm3_default_config.h"  // Config stuff to use the default nvm3 area and not need to mess with the linkerfile
#include "nvm3_hal_flash.h"       // driver for internal flash on nvm3

// our headers
#include "mock_effectControl.h"  // driver for internal flash on nvm3

/*
 * Unit tests for "nvm_HW.h"
 */

void setUp(void) {}

void tearDown(void) {}