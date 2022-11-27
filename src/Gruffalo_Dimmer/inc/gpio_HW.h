#ifndef gpio_HW_H
#define gpio_HW_H

#include "inttypes.h"
#include "stdbool.h"

#include "gpio_HW_types.h"

void enableGPIOClock(void);

// Read the state of an input pin
uint32_t readPin(pinPort_t port, uint32_t pinNo);

// Set the pin mode
void setPinMode(pinPort_t port, uint32_t pin, pinMode_t mode, bool dout);

void configurePinInterrupt(pinPort_t port, uint32_t pinNo, uint32_t intNo, bool risingEdge, bool fallingEdge,
                           bool enable);
void enablePinInterrupts(uint32_t pinsToEnable);

#endif  // gpio_HW_H
