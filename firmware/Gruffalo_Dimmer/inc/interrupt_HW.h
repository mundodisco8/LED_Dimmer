/**
 * @file interrupt_HW.h
 * @author Joel Santos (jsantosrico@gmail.com)
 * @brief Public API for GPIO Interrupt management and callback registration.
 * @version 0.1
 * @date 2026-03-13
 * * @copyright Copyright (c) 2026
 * * @defgroup INTERRUPT_HW Interrupt Hardware Module
 * @{
 *
 * ### Detailed Description
 * This module provides a wrapper around the Silicon Labs `GPIOINT` library. It
 * facilitates the registration of callback functions for external interrupts,
 * supporting both standard callbacks and context-aware callbacks (with void* pointers).
 *
 * ### Important Note on Registration
 * There is a significant difference in behavior between standard and extended registration:
 * * **Standard (@ref setInterruptCallback):** Overwrites any existing callback on
 * the specified interrupt number.
 * * **Extended (@ref setInterruptCallbackWCtx):** Checks if the interrupt is in
 * use. If it is, it finds and returns a **different** available interrupt number.
 * Use the return value of this function to configure your GPIO pin hardware.
 *
 * ### Initialization Sequence
 * 1. Initialize the GPIO interrupt dispatcher: @ref enableGPIOInts.
 * 2. Register the desired callback: @ref setInterruptCallback or @ref setInterruptCallbackWCtx.
 * 3. Configure the hardware pin to trigger the returned interrupt number.
 */
#ifndef INTERRUPT_HW_H
#define INTERRUPT_HW_H

#include "interrupt_HW_types.h"
#include "inttypes.h"

/**
 * @brief Wrapper for GPIOINT_Init()
 */
void enableGPIOInts(void);

/**
 * @brief Wrapper for GPIOINT_CallbackRegister()
 * Registers a callback to a pin interrupt number. If the interrupt is already registered, it overwrites it.
 * NOTE: check the comments on setInterruptCallbackWCtx to see the differences in behaviour in both.
 */
void setInterruptCallback(uint8_t pinNo, callbackPtr_t cbPtr);
/**
 * @brief Wrapper for GPIOINT_CallbackRegisterExt()
 * NOTE: Has to be called before configurePinInterrupt. GPIOINT_CallbackRegister() and GPIOINT_CallbackRegisterExt()
 * have different behaviours, which is quite anoying. CallbackRegister() registers the interrupt number passed as
 * parameter, and if it's in use, then overwrites it. CallbackRegisterExt() checks if the interrupt number passed IS IN
 * USE AND IF IT IS, REGISTERS AND RETURNS A DIFFERENT NUMBER. This requires calling GPIO_ExtIntConfig() after
 * GPIOINT_CallbackRegisterExt() has been called, with the value returned by GPIOINT_CallbackRegisterExt() as parameter
 */
uint32_t setInterruptCallbackWCtx(uint8_t pinNo, callbackCtxPtr_t cbCtxPtr, void* ctxPtr);

/**
 * @brief Initialises the TIMER0 interrupts.
 */
void enableTIMER0Int(void);

#endif  // INTERRUPT_HW_H