#ifndef EM_CMU_H_
#define EM_CMU_H_

////
// Minimal em_cmu.h to get the SDK libraries working in unit testing
////

#include "inttypes.h"
#include "stdbool.h"

////
// defines needed to get CMU_Clock_TypeDef correctly defined
// NOTE: CMU_Clock_TypeDef is a monster of an enum, instead of adding the hundreds of defines needed
// I'm going to add them as I need them.
////


#define SL_ENUM_GENERIC(name, type) \
    typedef type name;              \
    enum name##_enum

#define CMU_CLKEN0_EN_REG 1
#define CMU_EN_REG_POS 5U
#define CMU_EN_BIT_POS 0U
#define _CMU_CLKEN0_GPIO_SHIFT 26 /**< Shift value for CMU_GPIO                    */

#define _CMU_CLKEN0_TIMER0_SHIFT                   4                                    /**< Shift value for CMU_TIMER0                  */


SL_ENUM_GENERIC(CMU_Clock_TypeDef, uint32_t) {
    cmuClock_GPIO = (CMU_CLKEN0_EN_REG << CMU_EN_REG_POS) | (_CMU_CLKEN0_GPIO_SHIFT << CMU_EN_BIT_POS),
    cmuClock_TIMER0 = (CMU_CLKEN0_EN_REG << CMU_EN_REG_POS) | (_CMU_CLKEN0_TIMER0_SHIFT << CMU_EN_BIT_POS),
};

////
// Declaration of functions being used in the code
////

void CMU_ClockEnable(CMU_Clock_TypeDef clock, bool enable);
uint32_t CMU_ClockFreqGet(CMU_Clock_TypeDef clock);

#endif  // EM_CMU_H_
