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

#endif  // _GPIO_HW_TYPES_H_