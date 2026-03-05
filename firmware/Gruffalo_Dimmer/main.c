/***************************************************************************/ /**
 * @file
 * @brief main() function.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
#include "app.h"
#include "sl_component_catalog.h"
#include "sl_system_init.h"
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
#include "sl_power_manager.h"
#endif  // SL_CATALOG_POWER_MANAGER_PRESENT
#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "sl_system_kernel.h"
#else  // SL_CATALOG_KERNEL_PRESENT
#include "sl_system_process_action.h"
#endif  // SL_CATALOG_KERNEL_PRESENT

#include "sl_power_manager_debug.h"
#include "timer_HW.h"
void my_events_callback(sl_power_manager_em_t from, sl_power_manager_em_t to);

bool sleepOnce = true;

#define EM_EVENT_MASK_ALL                                                                             \
    (SL_POWER_MANAGER_EVENT_TRANSITION_ENTERING_EM0 | SL_POWER_MANAGER_EVENT_TRANSITION_LEAVING_EM0 | \
     SL_POWER_MANAGER_EVENT_TRANSITION_ENTERING_EM1 | SL_POWER_MANAGER_EVENT_TRANSITION_LEAVING_EM1 | \
     SL_POWER_MANAGER_EVENT_TRANSITION_ENTERING_EM2 | SL_POWER_MANAGER_EVENT_TRANSITION_LEAVING_EM2)

sl_power_manager_em_transition_event_handle_t event_handle;
sl_power_manager_em_transition_event_info_t event_info = {
    .event_mask = EM_EVENT_MASK_ALL,
    .on_event = my_events_callback,
};

extern bool sleepFlag;
int main(void) {
    // Initialize Silicon Labs device, system, service(s) and protocol stack(s).
    // Note that if the kernel is present, processing task(s) will be created by
    // this call.
    sl_system_init();

    // Initialize the application. For example, create periodic timer(s) or
    // task(s) if the kernel is present.
    app_init();

    sl_power_manager_subscribe_em_transition_event(&event_handle, &event_info);
    // sl_power_manager_debug_print_em_requirements();

#if defined(SL_CATALOG_KERNEL_PRESENT)
    // Start the kernel. Task(s) created in app_init() will start running.
    sl_system_kernel_start();
#else  // SL_CATALOG_KERNEL_PRESENT
    while (1) {
        // Do not remove this call: Silicon Labs components process action routine
        // must be called from the super loop.
        sl_system_process_action();

        // Application process.
        app_process_action();

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
        // Let the CPU go to sleep if the system allows it.
        // sl_power_manager_sleep();
#endif
    }
#endif  // SL_CATALOG_KERNEL_PRESENT
}

#include "app_log.h"
char* string_lookup_table[] = {
    "EM0", "EM1", "EM2", "EM3", "EM4",
};
#include "sleepyTimers_HW.h"
void my_events_callback(sl_power_manager_em_t from, sl_power_manager_em_t to) {
    app_log_debug("Event:%s-%s\r\n", string_lookup_table[from], string_lookup_table[to]);
    uint64_t delay = SLP_getSystemTickInMs() + 1000;
    uint64_t now;
    do {
        now = SLP_getSystemTickInMs();
    } while (now < delay);
}