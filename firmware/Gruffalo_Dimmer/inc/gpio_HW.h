/**
 * @file gpio_HW.h
 * @author Joel Santos (jsantosrico@gmail.com)
 * @brief Hardware Abstraction Layer (HAL) for GPIO and External Interrupts.
 * @version 0.1
 * @date 2026-03-13
 * * @copyright Copyright (c) 2026
 * * @defgroup GPIO_HW GPIO Hardware Module
 * @{
 *
 * ### Detailed Description
 * This module provides a simplified interface for managing General Purpose I/O pins
 * on the Silicon Labs EFR32/BGM220 platform. It abstracts port/pin configurations,
 * interrupt management, and low-power wake-up settings.
 *
 * ### Key Features
 * * **Port Abstraction:** Maps custom @ref pinPort_t types to platform-specific
 * GPIO port definitions.
 * * **Interrupt Management:** Simplifies the configuration of edge-triggered
 * interrupts (rising, falling, or both).
 * * **Low Power Support:** Provides dedicated logic for configuring pins as
 * EM4 (Energy Mode 4) wake-up sources.
 *
 * ### Usage Flow
 * 1. **Power Up:** Call @ref enableGPIOClock before interacting with any registers.
 * 2. **Setup:** Define pin modes (Input, Push-pull, etc.) via @ref setPinMode.
 * 3. **Interrupts:** Use @ref configurePinInterrupt followed by @ref enablePinInterrupts
 * to handle asynchronous user events.
 *
 * @note Accessing GPIO registers without enabling the clock via @ref enableGPIOClock
 * will result in a hardware HardFault.
 */
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
