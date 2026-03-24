/**
 * @file gpio_HW_types.h
 * @author Joel Santos (jsantosrico@gmail.com)
 * @brief data types of the gpio_HW module
 * @version 0.1
 * @date 2026-03-13
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef _GPIO_HW_TYPES_H_
#define _GPIO_HW_TYPES_H_

/**
 * @brief The different modes a pin can be set up
 */
typedef enum pin_mode {
    MODE_DISABLED = 0,
    /** Input enabled. Filter if DOUT is set. */
    MODE_INPUT = 1,
    /** Input enabled. DOUT determines pull direction. */
    MODE_INPUT_PULL = 2,
    /** Input enabled with filter. DOUT determines pull direction. */
    MODE_INPUT_PULL_FILTER = 3,
    /** Push-pull output. */
    MODE_PUSHPULL = 4,
} pinMode_t;

/**
 * @brief The different ports pins can belong to
 */
typedef enum ports {
    portA = 0,
    portB,
    portC,
    portD,
    portE,
    portF,
} pinPort_t;

/**
 * @brief Translates from SiLabs ports to our ports without having to share "em_gpio.h"!
 */
// 1. The Lookup Table (This doesn't redefine SL_GPIO_PORT_A)
#define AS_SHORT_SL_GPIO_PORT_A portA
#define AS_SHORT_SL_GPIO_PORT_B portB
#define AS_SHORT_SL_GPIO_PORT_C portC
#define AS_SHORT_SL_GPIO_PORT_D portD
#define AS_SHORT_SL_GPIO_PORT_E portE
#define AS_SHORT_SL_GPIO_PORT_F portF

// 2. The Conversion Macro
#define TO_PORT(p) AS_SHORT_##p
// --- Usage ---
// If you use it like this:
// TO_PORT(SL_GPIO_PORT_A)
// It expands to: AS_SHORT_SL_GPIO_PORT_A which finally becomes: portA

// 3. Need this to be able to translate #defines
#define MY_PORT(p) TO_PORT(p)
// --- Usage ---
// If you use it like this:
// #define PMW0_PORT SL_GPIO_PORT_A
// MY_PORT(PWM0_PORT)
// It expands to: TO_PORT(SL_GPIO_PORT_A) which expands to AS_SHORT_SL_GPIO_PORT_A wich finally becomes: portA

/**
 * @brief Silly enum for enhanced clarity when setting output pins' states
 */
typedef enum pinState { PIN_CLEAR = 0, PIN_SET = 1 } pinState_t;

#endif  // _GPIO_HW_TYPES_H_