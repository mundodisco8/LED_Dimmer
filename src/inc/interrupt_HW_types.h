#ifndef _INTERRUPT_HW_TYPES_H_
#define _INTERRUPT_HW_TYPES_H_

#include "inttypes.h"

//typedef for interrupt callback functions
typedef void (*callbackPtr_t)(uint8_t intNo);
//typedef for interrupt callback functions with context
typedef void (*callbackCtxPtr_t)(uint8_t intNo, void *ctx);

#endif // _INTERRUPT_HW_TYPES_H_