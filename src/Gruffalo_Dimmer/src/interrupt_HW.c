#include "interrupt_HW.h"

#include "gpiointerrupt.h"
#include "app_log.h"

// Wrapper for GPIOINT_Init()
// Initialises the GPIO Odd and Even interrupts.
void initialiseInterrupts(void) {
    GPIOINT_Init();
}

// Wrapper for GPIOINT_CallbackRegister()
// Registers a callback to a pin interrupt number. If the interrupt is already registered, it overwrites it.
// NOTE: check the comments on setInterruptCallbackWCtx to see the differences in behaviour in both.
// Parameters: pinNo: a uint32_t with the interrupt number to associate the callback with
//             callbackPtr: a pointer to a callback of type callbackPtr_t, to be called when the interrupt triggers
// Returns: nothing
void setInterruptCallback(uint32_t pinNo, callbackPtr_t callbackPtr) {
    GPIOINT_CallbackRegister(pinNo, callbackPtr);
}

// Wrapper for GPIOINT_CallbackRegisterExt()
// NOTE: Has to be called before configurePinInterrupt. GPIOINT_CallbackRegister() and GPIOINT_CallbackRegisterExt()
// have different behaviours, which is quite anoying. CallbackRegister() registers the interrupt number passed as
// parameter, and if it's in use, then overwrites it. CallbackRegisterExt() checks if the interrupt number passed IS IN
// USE AND IF IT IS, REGISTERS AND RETURNS A DIFFERENT NUMBER. This requires calling GPIO_ExtIntConfig() after
// GPIOINT_CallbackRegisterExt() has been called, with the value returned by GPIOINT_CallbackRegisterExt() as parameter
//
// Parameters: pinNo: a uint32_t with the interrupt number to associate the callback with
//             cbCtxPtr: a pointer to a callback of type callbackCtxPtr_t, to be called when the interrupt triggers
//             ctxPtr: a pointer to the context to be passed to the callback function.
// Returns: the interrupt number registered, which will be the same to pinNo if it's not registered when this function
//          is called, or something else if the pinNo interrupt is already registered.
uint32_t setInterruptCallbackWCtx(uint32_t pinNo, callbackCtxPtr_t cbCtxPtr, void* ctxPtr) {
    uint32_t retVal = GPIOINT_CallbackRegisterExt(pinNo, cbCtxPtr, ctxPtr);
    if (retVal == INTERRUPT_UNAVAILABLE) {
        app_log_error("Error 0x%04X registering interrupt callback with Ctx\r\n", retVal);
    }
    return retVal;
}
