#ifndef _MICRO_REGISTERS_H_
#define _MICRO_REGISTERS_H_

/*** TESTING REGISTERS!!!
 *
 * Check this post http://vandervoord.net/blog?offset=1432065060082&tag=Unit+Test+How%3F
 * This guy is a freaking genius!
 *
 * So, to sum it up:
 * 1) Registers are normally defined as addresses. These addresses don't make sense in the tested version, as you
 *    probably can't access any arbitrary address, lixe 0x05000001.
 * 2) To avoid this, we create a file with all the registers we need to test. If the registers have type, like the
 *    SiLabs registers, we add whatever is needed for them to be defined. Let's call that file 'micro_registers.h`
 *    NOTE: I slightly modify this, by defining the types in `registerTypes.h`, see note there for reasoning.
 * 3) We copy whatever is needed from the SDK files, and we will arrive to the declaration of our register, something
 *    like
 *    ```
 *    #define PORTA (*(volatile uint32_t*)(0x40001000)) //if the register is defined as a "volatile variable"
 *    #define TIMER0 ((TIMER_TypeDef *) TIMER0_BASE)    //if the register is defined as a "pointer"
 *    ```
 *    We get that declaration and we replace it with
 *    ```
 *    EXTERN volatile uint32_t PORTA; //if it was defined as a "volatile variable"
 *    EXTERN TIMER_TypeDef* TIMER0;   //if it was defined as a "pointer"
 *    ```
 *    Notice the all caps EXTERN, that's != to `extern`, and that's also the key.
 * 4) In the top of our `micro_registers.h`, we type
 *    ```
 *    #ifdef TEST
 *    #ifndef EXTERN
 *    #define EXTERN extern
 *    #endif
 *    #endif
 *    ```
 *    This will define EXTERN as `extern`, and make any references to our fake register as extern.
 * 5) We include `micro_register.h` in our fake SDK file (the ones in test\support). The fake SDK file now includes the
 *    declaration of the register.
 *
 * 6) Everything will compile as the compiler doens't care about the still inexistent fake register, but the linker
 *    will complain.
 * 7) We need a single instance of the `EXTERN fake_reg_t fakeReg;` line to be "real". We create a header called
 *    `testable_micro_registers.h`, and we include it AS THE SECOND INCLUDE, just after our header under test.
 *    This file contains this
 *    ```
 *    #ifndef TESTABLE_MICRO_REGISTERS_H
 *    #define TESTABLE_MICRO_REGISTERS_H
 *
 *    #define EXTERN
 *    #include "micro_registers.h"
 *
 *    #endif
 *    ```
 *    As this file is only added in our test file (that is, only once per test), there's one instance where the
 *    `EXTERN` keyword is replaced by "" (nothing) and our register becomes defined.
 * 8) If you are declaring a pointer, then you will have to provide the memory to point to somewhere else. In the test
 *    you can declare a variable of the required type, and in the setUp phace, make the pointer point to the variable.
 *
 *   YES!
 */

#ifdef TEST
#ifndef EXTERN
#define EXTERN extern
#endif
#endif

#include <inttypes.h>
#include "registerTypes.h"


// NOTE: This is the definition of TIMER0, which uses a particular address. This won't work when testing, as we don't
// know what's stored in that address in the computer...
// #define TIMER0_NS_BASE               (0x50048000UL) /* TIMER0_NS base address */
// #define TIMER0_BASE               (TIMER0_NS_BASE)           /* TIMER0 base address */
// #define TIMER0                  ((TIMER_TypeDef *) TIMER0_BASE)                           /**< TIMER0 base pointer */
EXTERN TIMER_TypeDef* TIMER0;

// MAGIC line that makes registers testable!
EXTERN GPIO_TypeDef* GPIO;

#endif  // _MICRO_REGISTERS_H_