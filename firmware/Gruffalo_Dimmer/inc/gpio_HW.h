#ifndef gpio_HW_H
#define gpio_HW_H

#include "gpio_HW_types.h"
#include "inttypes.h"
#include "stdbool.h"

/**
 * @brief Enables the GPIO clock
 * NOTE: If a module clock is disabled, the registers of that module are not accessible and accessing such registers
 * will hardfault the Cortex core.
 */
void enableGPIOClock(void);

/**
 * @brief Reads an input pin's value
 */
uint32_t readPin(pinPort_t port, uint8_t pinNo);

/**
 * @brief Set a pin Up (Wrapper for GPIO_PinModeSet)
 */
void setPinMode(pinPort_t port, uint8_t pin, pinMode_t mode, bool dout);
/**
 * @brief Set the Pin Up For EM4 Wake Up
 */
void setPinUpForEM4WakeUp(pinPort_t port, uint8_t pin);

/**
 * @brief Configures a GPIO interrupt
 */
void configurePinInterrupt(pinPort_t port, uint8_t pinNo, uint32_t intNo, bool risingEdge, bool fallingEdge,
                           bool enable);
/**
 * @brief Adds the pin interrupts to the enabled interrupts register.
 */
void enablePinInterrupts(uint32_t pinsToEnable);

#endif  // gpio_HW_H
