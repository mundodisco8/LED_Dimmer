#ifndef GPIOINTERRUPT_H
#define GPIOINTERRUPT_H

/*****************************************************************************************
 *
 * FAKE GPIOINTERRUPT.H USED IN TESTS. CONTAINS THE MINIMUM AMOUNT OF IT TO RUN THEM
 *
 ****************************************************************************************/

#include <inttypes.h>

#define INTERRUPT_UNAVAILABLE                         (0xFF) ///< A MACRO for Interrupt Un-available.

typedef void (*GPIOINT_IrqCallbackPtr_t)(uint8_t intNo);
typedef void (*GPIOINT_IrqCallbackPtrExt_t)(uint8_t intNo, void *ctx);

void GPIOINT_Init(void);
void GPIOINT_CallbackRegister(uint8_t intNo, GPIOINT_IrqCallbackPtr_t callbackPtr);
unsigned int GPIOINT_CallbackRegisterExt(uint8_t pin, GPIOINT_IrqCallbackPtrExt_t callbackPtr, void *callbackCtx);

#endif // GPIOINTERRUPT_H