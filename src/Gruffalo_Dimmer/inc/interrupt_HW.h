/*
 * debounce.h
 *
 *  Created on: 15 Nov 2022
 *      Author: joel.santos
 */
#ifndef INTERRUPT_HW_H
#define INTERRUPT_HW_H

#include "inttypes.h"

#include "interrupt_HW_types.h"

// Initialises the GPIO Odd and Even interrupts.
void initialiseInterrupts(void);
// Registers a callback to a pin interrupt number. If the interrupt is already registered, it overwrites it.
void setInterruptCallback(uint32_t pinNo, callbackPtr_t cbPtr);
// Registers a callback with context to a pin interrupt number. If the interrupt is already registered, sets the next
// free interrupt number.
uint32_t setInterruptCallbackWCtx(uint32_t pinNo, callbackCtxPtr_t cbCtxPtr, void* ctxPtr);

#endif // INTERRUPT_HW_H