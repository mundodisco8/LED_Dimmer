#ifndef EM_CMU_H
#define EM_CMU_H

/*****************************************************************************************
 *
 * FAKE EM_CMU.H USED IN TESTS. CONTAINS THE MINIMUM AMOUNT OF IT TO RUN
 *
 ****************************************************************************************/

#include <inttypes.h>
#include <stdbool.h>

typedef uint32_t CMU_Clock_TypeDef;

enum CMU_Clock_TypeDef_enum { cmuClock_GPIO, cmuClock_TIMER0 };

uint32_t CMU_ClockFreqGet(CMU_Clock_TypeDef clock);
void CMU_ClockEnable(CMU_Clock_TypeDef clock, bool enable);

#endif  // EM_CMU_H