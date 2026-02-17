/*******************************************************************************
 * @file
 * @brief Core application logic.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories
 *Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is
 *Silicon Laboratories Inc.
 *
 * This software is provided 'as-is',
 *without any express or implied warranty.
 *In no event will the authors be held
 *liable for any damages arising from the
 *use of this software.
 *
 * Permission is granted to anyone to use
 *this software for any purpose, including
 *commercial applications, and to alter it
 *and redistribute it freely, subject to
 *the following restrictions:
 *
 * 1. The origin of this software must not
 *be misrepresented; you must not claim
 *that you wrote the original software. If
 *you use this software in a product, an
 *acknowledgment in the product
 *documentation would be appreciated but
 *is not required.
 * 2. Altered source versions must be
 *plainly marked as such, and must not be
 *    misrepresented as being the original
 *software.
 * 3. This notice may not be removed or
 *altered from any source distribution.
 *
 ******************************************************************************/
#include "stdbool.h"

// Ignore a cast-align warning in some cmsis header
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-align"
#include "em_common.h"
#pragma GCC diagnostic pop

#include "em_gpio.h"
#include "em_timer.h"

// Ignore a sign conversion warning in sl_sleeptimer.h
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wpedantic"
#include "sl_bluetooth.h"
#pragma GCC diagnostic pop

#include "app.h"
#include "app_assert.h"
#include "app_log.h"
#include "gatt_db.h"
#include "pin_config.h"
#include "sl_sleeptimer.h"

#define DEBUG_EFM_USER
#include "sl_assert.h"

// My Headers
#include "PWMControl.h"
#include "buttonActions.h"
#include "buttons.h"
#include "effectControl.h"
#include "gpio_HW.h"
#include "timer_HW.h"

// The advertising set handle allocated from Bluetooth stack.
static uint8_t advertising_set_handle = 0xff;

// Our buttons
button_t button0 = {0};
button_t button1 = {0};

// And our quad encoders
quad_encoder_t quad0 = {0};
quad_encoder_t quad1 = {0};

// Used in the interrupt to signal we have to process the LED Effects
volatile bool processLEDEffects = true;

/******************************************************************************
 * Application Init.
 *****************************************************************************/
void app_init(void) {
    /////////////////////////////////////////////////////////////////////////////
    // Put your additional application init code here!                         //
    // This is called once during start-up.                                    //
    /////////////////////////////////////////////////////////////////////////////
    app_log_error("Booted up!\r\n");

    enableGPIOClock();
    // setPinMode(portC, pwm0_PIN, MODE_PUSHPULL, 0);
    // setPinMode(portC, pwm1_PIN, MODE_PUSHPULL, 0);
    // setPinMode(portC, pwm2_PIN, MODE_PUSHPULL, 0);
    // TODO: there's no need, that I can see, of keeping the interrupt number, and I'm certainly not using it anywhere
    // If anything, it could be stored internally in the button_t struct
    // Init GPIOs for buttons and quads
    uint32_t button0GPIOIntNo = 0;
    uint32_t quad0GPIOIntNo = 0;
    uint32_t button1GPIOIntNo = 0;
    uint32_t quad1GPIOIntNo = 0;
    initButton(&button0, (pinPort_t)btn0_PORT, btn0_PIN, button0Pressed, button0Released);
    initQuadEncoder(&quad0, (pinPort_t)quad0_0_PORT, quad0_0_PIN, (pinPort_t)quad0_1_PORT, quad0_1_PIN, quad0ClockWise,
                    quad0CounterClockWise);
    initButton(&button1, (pinPort_t)btn1_PORT, btn1_PIN, button1Pressed, button1Released);
    initQuadEncoder(&quad1, (pinPort_t)quad1_0_PORT, quad1_0_PIN, (pinPort_t)quad1_1_PORT, quad1_1_PIN, quad1ClockWise,
                    quad1CounterClockWise);
    btnError_t retVal = configureButtonInterrupts(&button0, gpioCallbackButton, &button0GPIOIntNo);
    app_assert((retVal == BTN_OK), "Error configuring Btn 0 interrupts\r\n");
    retVal = configureQuadratureInterrupts(&quad0, gpioCallbackQuad, &quad0GPIOIntNo);
    app_assert((retVal == BTN_OK), "Error configuring Quad 0 interrupts\r\n");
    retVal = configureButtonInterrupts(&button1, gpioCallbackButton, &button1GPIOIntNo);
    app_assert((retVal == BTN_OK), "Error configuring Btn 1 interrupts\r\n");
    retVal = configureQuadratureInterrupts(&quad1, gpioCallbackQuad, &quad1GPIOIntNo);
    app_assert((retVal == BTN_OK), "Error configuring Quad 1 interrupts\r\n");

    // Init PWM on TIMER0
    initTimer0PWM(PWM_FREQUENCY);
    initTimer0CCChannel(CC_CHANNEL_0, (pinPort_t)pwm0_PORT, pwm0_PIN, PWM_ACTIVE_HIGH);
    initTimer0CCChannel(CC_CHANNEL_1, (pinPort_t)pwm1_PORT, pwm1_PIN, PWM_ACTIVE_HIGH);
    initTimer0CCChannel(CC_CHANNEL_2, (pinPort_t)pwm2_PORT, pwm2_PIN, PWM_ACTIVE_HIGH);

    breatheParams_t breatheEffectParams = DEFAULT_BREATHE_PARAMS;

    fillBreatheEffectLUT(&breatheEffectParams);

    initLEDStrips();
}

/******************************************************************************
 * Application Process Action.
 *****************************************************************************/
void app_process_action(void) {
    /////////////////////////////////////////////////////////////////////////////
    // Put your additional application code here!                              //
    // This is called infinitely.                                              //
    // Do not call blocking functions from here!                               //
    /////////////////////////////////////////////////////////////////////////////

    // NOTE: Prints "../app.c:115 :app_process_action: Assertion 'a == b' failed"
    // uint32_t a = 1;
    // uint32_t b = 2;
    // app_assert_s(a == b);

    // NOTE: Prints "../app.c:105 :app_process_action: Status: 40 = 0x0028 (?) Assertion failed"
    // app_assert_status(40);

    // NOTE: Prints "../app.c:107 :app_process_action: Status: 40 = 0x0028 (?) Assertion failed: Hey 60"
    // app_assert_status_f(40, "Hey %d\r\n", 60);

    // NOTE: Prints "../app.c:111 :app_process_action: Assertion 'a == b' failed: 1 is not 2"
    // uint32_t a = 1;
    // uint32_t b = 2;
    // app_assert( a == b, "%d is not %d", a , b);

    if (processLEDEffects) {
        processLEDEffects = false;
        effectControlLoop();
    }
}

/******************************************************************************
 * Bluetooth stack event handler.
 * This overrides the dummy weak implementation.
 *
 * @param[in] evt Event coming from the Bluetooth stack.
 *****************************************************************************/
void sl_bt_on_event(sl_bt_msg_t* evt) {
    sl_status_t sc;
    bd_addr address;
    uint8_t address_type;
    uint8_t system_id[8];

    switch (SL_BT_MSG_ID(evt->header)) {
        // -------------------------------
        // This event indicates the device has started and the radio is ready.
        // Do not call any stack command before receiving this boot event!
        case sl_bt_evt_system_boot_id:

            // Extract unique ID from BT Address.
            sc = sl_bt_system_get_identity_address(&address, &address_type);
            app_assert_status(sc);

            // Pad and reverse unique ID to get System ID.
            system_id[0] = address.addr[5];
            system_id[1] = address.addr[4];
            system_id[2] = address.addr[3];
            system_id[3] = 0xFF;
            system_id[4] = 0xFE;
            system_id[5] = address.addr[2];
            system_id[6] = address.addr[1];
            system_id[7] = address.addr[0];

            sc = sl_bt_gatt_server_write_attribute_value(gattdb_system_id, 0, sizeof(system_id), system_id);
            app_assert_status(sc);

            // Create an advertising set.
            sc = sl_bt_advertiser_create_set(&advertising_set_handle);
            app_assert_status(sc);

            // Generate data for advertising
            sc = sl_bt_legacy_advertiser_generate_data(advertising_set_handle, sl_bt_advertiser_general_discoverable);
            app_assert_status(sc);

            // Set advertising interval to 100ms.
            sc = sl_bt_advertiser_set_timing(advertising_set_handle,
                                             160,  // min. adv. interval (milliseconds * 1.6)
                                             160,  // max. adv. interval (milliseconds * 1.6)
                                             0,    // adv. duration
                                             0);   // max. num. adv. events
            app_assert_status(sc);
            // Start advertising and enable connections.
            sc = sl_bt_legacy_advertiser_start(advertising_set_handle, sl_bt_advertiser_connectable_scannable);
            app_assert_status(sc);
            break;

        // -------------------------------
        // This event indicates that a new connection was opened.
        case sl_bt_evt_connection_opened_id:
            break;

        // -------------------------------
        // This event indicates that a connection was closed.
        case sl_bt_evt_connection_closed_id:
            // Generate data for advertising
            sc = sl_bt_legacy_advertiser_generate_data(advertising_set_handle, sl_bt_advertiser_general_discoverable);
            app_assert_status(sc);

            // Restart advertising after client has disconnected.
            sc = sl_bt_legacy_advertiser_start(advertising_set_handle, sl_bt_advertiser_connectable_scannable);
            app_assert_status(sc);
            break;

        ///////////////////////////////////////////////////////////////////////////
        // Add additional event handlers here as your application requires!      //
        ///////////////////////////////////////////////////////////////////////////

        // -------------------------------
        // Default event handler.
        default:
            break;
    }
}

// Handles the overflow interrupt on TIMER0
void TIMER0_IRQHandler(void) {
    // Acknowledge the interrupt
    uint32_t flags = TIMER_IntGet(TIMER0);
    TIMER_IntClear(TIMER0, flags);

    // There's a single OF interrupt per timer as there's only a single TOP value (The COUNT value can change and thus
    // there are CCn interrupt flags). We only care about the OF interrupt (at least for now)

    if (!processLEDEffects) {
        processLEDEffects = true;
    }
}