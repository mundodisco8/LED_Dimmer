#ifndef BGM220PC22WGA_H
#define BGM220PC22WGA_H

#include "micro_registers.h"

#include "core_cm33.h"    /* Core Header File */
#include "bgm22_timer.h"

// NOTE: This is the definition of TIMER0, which in the SDK uses a particular address. This won't work when testing, as
// we don't know what's stored in that address in the computer...
// #define TIMER0_NS_BASE               (0x50048000UL) /* TIMER0_NS base address */
// #define TIMER0_BASE               (TIMER0_NS_BASE)           /* TIMER0 base address */
// #define TIMER0                  ((TIMER_TypeDef *) TIMER0_BASE)                           /**< TIMER0 base pointer */
// EXTERN TIMER_TypeDef* TIMER0;


#endif // BGM220PC22WGA_H