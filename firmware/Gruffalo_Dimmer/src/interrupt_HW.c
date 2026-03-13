/**
 * @file interrupt_HW.c
 * @author Joel Santos (jsantosrico@gmail.com)
 * @brief Implementation of the GPIO interrupt dispatcher and callback wrappers.
 * @version 0.1
 * @date 2026-03-13
 * * @copyright Copyright (c) 2026
 * * @details
 * Interfaces directly with the Silicon Labs `gpiointerrupt.h` SDK.
 * * ### Implementation Details
 * * **Interrupt Dispatching:** Automatically handles the vectoring of both
 * "Odd" and "Even" GPIO interrupts via the `GPIOINT_Init()` call.
 * * **Extended Registration:** Implements logic to handle `INTERRUPT_UNAVAILABLE`
 * errors, logging failures via the `app_log` interface if the system runs out
 * of available hardware interrupt slots.
 * * **Context Management:** Supports passing `void*` context pointers to
 * callbacks, enabling object-oriented patterns in C (e.g., passing a `button_t`
 * pointer to a generic handler).
 */
#include "interrupt_HW.h"

// Silabs SDK headers
// Ignore a cast-align warning in some cmsis header
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-align"
#include "gpiointerrupt.h"
// Ignore a sign conversion in sl_sleeptimer.h
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include "app_log.h"
#pragma GCC diagnostic pop
#include "cmsis_nvic_virtual.h"
#include "em_timer.h"

/**
 * @brief Wrapper for GPIOINT_Init()
 * Initialises the GPIO Odd and Even interrupts.
 */
void enableGPIOInts(void) { GPIOINT_Init(); }

/**
 * @brief Wrapper for GPIOINT_CallbackRegister()
 * Registers a callback to a pin interrupt number. If the interrupt is already registered, it overwrites it.
 * NOTE: check the comments on setInterruptCallbackWCtx to see the differences in behaviour in both.
 *
 * @param pinNo a uint32_t with the interrupt number to associate the callback with
 * @param callbackPtr callbackPtr: a pointer to a callback of type callbackPtr_t, to be called when the interrupt triggers
 */
void setInterruptCallback(uint8_t pinNo, callbackPtr_t callbackPtr) { GPIOINT_CallbackRegister(pinNo, callbackPtr); }

/**
 * @brief Wrapper for GPIOINT_CallbackRegisterExt()
 * NOTE: Has to be called before configurePinInterrupt. GPIOINT_CallbackRegister() and GPIOINT_CallbackRegisterExt()
 * have different behaviours, which is quite anoying. CallbackRegister() registers the interrupt number passed as
 * parameter, and if it's in use, then overwrites it. CallbackRegisterExt() checks if the interrupt number passed IS IN
 * USE AND IF IT IS, REGISTERS AND RETURNS A DIFFERENT NUMBER. This requires calling GPIO_ExtIntConfig() after
 * GPIOINT_CallbackRegisterExt() has been called, with the value returned by GPIOINT_CallbackRegisterExt() as parameter
 *
 * @param pinNo a uint32_t with the interrupt number to associate the callback with
 * @param cbCtxPtr a pointer to a callback of type callbackCtxPtr_t, to be called when the interrupt triggers
 * @param ctxPtr a pointer to the context to be passed to the callback function.
 * @return uint32_t the interrupt number registered, which will be the same to pinNo if it's not registered when this function
 *          is called, or something else if the pinNo interrupt is already registered.
 */
uint32_t setInterruptCallbackWCtx(uint8_t pinNo, callbackCtxPtr_t cbCtxPtr, void* ctxPtr) {
    uint32_t retVal = GPIOINT_CallbackRegisterExt(pinNo, cbCtxPtr, ctxPtr);
    if (retVal == INTERRUPT_UNAVAILABLE) {
        app_log_error("Error 0x%04" PRIX32 " registering interrupt callback with Ctx\r\n", retVal);
    }
    return retVal;
}

/**
 * @brief Initialises the TIMER0 interrupts.
 */
void enableTIMER0Int(void) { NVIC_EnableIRQ(TIMER0_IRQn); }