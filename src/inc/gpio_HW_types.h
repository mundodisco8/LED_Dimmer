#ifndef _GPIO_HW_TYPES_H_
#define _GPIO_HW_TYPES_H_

// TODO: Create a button header, where the buttons are defined with their pins and ports and stuff, and then just exported as something that doesn't require knowledge of that.

// Hack to get same pin modes. Copied from em_gpio.h
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

// Hack to get the same port values than em_gpio.h without having to include it.
typedef enum ports {
    portA = 0,
    portB,
    portC,
    portD,
    portE,
    portF,
} pinPort_t;

#endif  // _GPIO_HW_TYPES_H_