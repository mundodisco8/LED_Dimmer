#ifndef gpio_HW_H
#define gpio_HW_H

#include "inttypes.h"
#include "stdbool.h"

// Hack to get the same port values than em_gpio.h without having to it.
typedef enum ports {
    portA = 0,
    portB,
    portC,
    portD,
    portE,
    portF,
} pin_port_t;

// Read the state of an input pin
uint32_t readPin(pin_port_t port, uint32_t pinNo);

void configurePinInterrupt(pin_port_t port, uint32_t pinNo, uint32_t intNo, bool risingEdge, bool fallingEdge,
                           bool enable);
void enablePinInterrupts(uint32_t pinsToEnable);

#endif  // gpio_HW_H
