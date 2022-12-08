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

#include "inttypes.h"

#ifdef TEST
#ifndef EXTERN
#define EXTERN extern
#endif
#endif

////
// Definition of TIMER0 register
////

/* IO definitions (access restrictions to peripheral registers) */
/**
    \defgroup CMSIS_glob_defs CMSIS Global Defines

    <strong>IO Type Qualifiers</strong> are used
    \li to specify the access to peripheral variables.
    \li for automatic generation of peripheral register debug information.
*/
#ifdef __cplusplus
#define __I volatile /*!< Defines 'read only' permissions */
#else
#define __I volatile const /*!< Defines 'read only' permissions */
#endif
#define __O volatile  /*!< Defines 'write only' permissions */
#define __IO volatile /*!< Defines 'read / write' permissions */

/* following defines should be used for structure members */
#define __IM volatile const /*! Defines 'read only' structure member permissions */
#define __OM volatile       /*! Defines 'write only' structure member permissions */
#define __IOM volatile      /*! Defines 'read / write' structure member permissions */

/** TIMER CC Register Group Declaration. */
typedef struct {
    __IOM uint32_t CFG;     /**< CC Channel Configuration Register                  */
    __IOM uint32_t CTRL;    /**< CC Channel Control Register                        */
    __IOM uint32_t OC;      /**< OC Channel Value Register                          */
    uint32_t RESERVED0[1U]; /**< Reserved for future use                            */
    __IOM uint32_t OCB;     /**< OC Channel Value Buffer Register                   */
    __IM uint32_t ICF;      /**< IC Channel Value Register                          */
    __IM uint32_t ICOF;     /**< IC Channel Value Overflow Register                 */
    uint32_t RESERVED1[1U]; /**< Reserved for future use                            */
} TIMER_CC_TypeDef;

/** TIMER Register Declaration. */
typedef struct {
    __IM uint32_t IPVERSION;      /**< IP version ID                                      */
    __IOM uint32_t CFG;           /**< Configuration Register                             */
    __IOM uint32_t CTRL;          /**< Control Register                                   */
    __IOM uint32_t CMD;           /**< Command Register                                   */
    __IM uint32_t STATUS;         /**< Status Register                                    */
    __IOM uint32_t IF;            /**< Interrupt Flag Register                            */
    __IOM uint32_t IEN;           /**< Interrupt Enable Register                          */
    __IOM uint32_t TOP;           /**< Counter Top Value Register                         */
    __IOM uint32_t TOPB;          /**< Counter Top Value Buffer Register                  */
    __IOM uint32_t CNT;           /**< Counter Value Register                             */
    uint32_t RESERVED0[1U];       /**< Reserved for future use                            */
    __IOM uint32_t LOCK;          /**< TIMER Configuration Lock Register                  */
    __IOM uint32_t EN;            /**< module en                                          */
    uint32_t RESERVED1[11U];      /**< Reserved for future use                            */
    TIMER_CC_TypeDef CC[3U];      /**< Compare/Capture Channel                            */
    uint32_t RESERVED2[8U];       /**< Reserved for future use                            */
    __IOM uint32_t DTCFG;         /**< DTI Configuration Register                         */
    __IOM uint32_t DTTIMECFG;     /**< DTI Time Configuration Register                    */
    __IOM uint32_t DTFCFG;        /**< DTI Fault Configuration Register                   */
    __IOM uint32_t DTCTRL;        /**< DTI Control Register                               */
    __IOM uint32_t DTOGEN;        /**< DTI Output Generation Enable Register              */
    __IM uint32_t DTFAULT;        /**< DTI Fault Register                                 */
    __IOM uint32_t DTFAULTC;      /**< DTI Fault Clear Register                           */
    __IOM uint32_t DTLOCK;        /**< DTI Configuration Lock Register                    */
    uint32_t RESERVED3[960U];     /**< Reserved for future use                            */
    __IM uint32_t IPVERSION_SET;  /**< IP version ID                                      */
    __IOM uint32_t CFG_SET;       /**< Configuration Register                             */
    __IOM uint32_t CTRL_SET;      /**< Control Register                                   */
    __IOM uint32_t CMD_SET;       /**< Command Register                                   */
    __IM uint32_t STATUS_SET;     /**< Status Register                                    */
    __IOM uint32_t IF_SET;        /**< Interrupt Flag Register                            */
    __IOM uint32_t IEN_SET;       /**< Interrupt Enable Register                          */
    __IOM uint32_t TOP_SET;       /**< Counter Top Value Register                         */
    __IOM uint32_t TOPB_SET;      /**< Counter Top Value Buffer Register                  */
    __IOM uint32_t CNT_SET;       /**< Counter Value Register                             */
    uint32_t RESERVED4[1U];       /**< Reserved for future use                            */
    __IOM uint32_t LOCK_SET;      /**< TIMER Configuration Lock Register                  */
    __IOM uint32_t EN_SET;        /**< module en                                          */
    uint32_t RESERVED5[11U];      /**< Reserved for future use                            */
    TIMER_CC_TypeDef CC_SET[3U];  /**< Compare/Capture Channel                            */
    uint32_t RESERVED6[8U];       /**< Reserved for future use                            */
    __IOM uint32_t DTCFG_SET;     /**< DTI Configuration Register                         */
    __IOM uint32_t DTTIMECFG_SET; /**< DTI Time Configuration Register                    */
    __IOM uint32_t DTFCFG_SET;    /**< DTI Fault Configuration Register                   */
    __IOM uint32_t DTCTRL_SET;    /**< DTI Control Register                               */
    __IOM uint32_t DTOGEN_SET;    /**< DTI Output Generation Enable Register              */
    __IM uint32_t DTFAULT_SET;    /**< DTI Fault Register                                 */
    __IOM uint32_t DTFAULTC_SET;  /**< DTI Fault Clear Register                           */
    __IOM uint32_t DTLOCK_SET;    /**< DTI Configuration Lock Register                    */
    uint32_t RESERVED7[960U];     /**< Reserved for future use                            */
    __IM uint32_t IPVERSION_CLR;  /**< IP version ID                                      */
    __IOM uint32_t CFG_CLR;       /**< Configuration Register                             */
    __IOM uint32_t CTRL_CLR;      /**< Control Register                                   */
    __IOM uint32_t CMD_CLR;       /**< Command Register                                   */
    __IM uint32_t STATUS_CLR;     /**< Status Register                                    */
    __IOM uint32_t IF_CLR;        /**< Interrupt Flag Register                            */
    __IOM uint32_t IEN_CLR;       /**< Interrupt Enable Register                          */
    __IOM uint32_t TOP_CLR;       /**< Counter Top Value Register                         */
    __IOM uint32_t TOPB_CLR;      /**< Counter Top Value Buffer Register                  */
    __IOM uint32_t CNT_CLR;       /**< Counter Value Register                             */
    uint32_t RESERVED8[1U];       /**< Reserved for future use                            */
    __IOM uint32_t LOCK_CLR;      /**< TIMER Configuration Lock Register                  */
    __IOM uint32_t EN_CLR;        /**< module en                                          */
    uint32_t RESERVED9[11U];      /**< Reserved for future use                            */
    TIMER_CC_TypeDef CC_CLR[3U];  /**< Compare/Capture Channel                            */
    uint32_t RESERVED10[8U];      /**< Reserved for future use                            */
    __IOM uint32_t DTCFG_CLR;     /**< DTI Configuration Register                         */
    __IOM uint32_t DTTIMECFG_CLR; /**< DTI Time Configuration Register                    */
    __IOM uint32_t DTFCFG_CLR;    /**< DTI Fault Configuration Register                   */
    __IOM uint32_t DTCTRL_CLR;    /**< DTI Control Register                               */
    __IOM uint32_t DTOGEN_CLR;    /**< DTI Output Generation Enable Register              */
    __IM uint32_t DTFAULT_CLR;    /**< DTI Fault Register                                 */
    __IOM uint32_t DTFAULTC_CLR;  /**< DTI Fault Clear Register                           */
    __IOM uint32_t DTLOCK_CLR;    /**< DTI Configuration Lock Register                    */
    uint32_t RESERVED11[960U];    /**< Reserved for future use                            */
    __IM uint32_t IPVERSION_TGL;  /**< IP version ID                                      */
    __IOM uint32_t CFG_TGL;       /**< Configuration Register                             */
    __IOM uint32_t CTRL_TGL;      /**< Control Register                                   */
    __IOM uint32_t CMD_TGL;       /**< Command Register                                   */
    __IM uint32_t STATUS_TGL;     /**< Status Register                                    */
    __IOM uint32_t IF_TGL;        /**< Interrupt Flag Register                            */
    __IOM uint32_t IEN_TGL;       /**< Interrupt Enable Register                          */
    __IOM uint32_t TOP_TGL;       /**< Counter Top Value Register                         */
    __IOM uint32_t TOPB_TGL;      /**< Counter Top Value Buffer Register                  */
    __IOM uint32_t CNT_TGL;       /**< Counter Value Register                             */
    uint32_t RESERVED12[1U];      /**< Reserved for future use                            */
    __IOM uint32_t LOCK_TGL;      /**< TIMER Configuration Lock Register                  */
    __IOM uint32_t EN_TGL;        /**< module en                                          */
    uint32_t RESERVED13[11U];     /**< Reserved for future use                            */
    TIMER_CC_TypeDef CC_TGL[3U];  /**< Compare/Capture Channel                            */
    uint32_t RESERVED14[8U];      /**< Reserved for future use                            */
    __IOM uint32_t DTCFG_TGL;     /**< DTI Configuration Register                         */
    __IOM uint32_t DTTIMECFG_TGL; /**< DTI Time Configuration Register                    */
    __IOM uint32_t DTFCFG_TGL;    /**< DTI Fault Configuration Register                   */
    __IOM uint32_t DTCTRL_TGL;    /**< DTI Control Register                               */
    __IOM uint32_t DTOGEN_TGL;    /**< DTI Output Generation Enable Register              */
    __IM uint32_t DTFAULT_TGL;    /**< DTI Fault Register                                 */
    __IOM uint32_t DTFAULTC_TGL;  /**< DTI Fault Clear Register                           */
    __IOM uint32_t DTLOCK_TGL;    /**< DTI Configuration Lock Register                    */
} TIMER_TypeDef;

// NOTE: This is the definition of TIMER0, which uses a particular address. This won't work when testing, as we don't
// know what's stored in that address in the computer...
// #define TIMER0_NS_BASE               (0x50048000UL) /* TIMER0_NS base address */
// #define TIMER0_BASE               (TIMER0_NS_BASE)           /* TIMER0 base address */
// #define TIMER0                  ((TIMER_TypeDef *) TIMER0_BASE)                           /**< TIMER0 base pointer */

// MAGIC line that makes registers testable!
EXTERN TIMER_TypeDef* TIMER0;

// // #ifdef PROVIDE_MEMORY
// TIMER_TypeDef TIMER0_meat;
// TIMER0 = &TIMER0_meat;
// // #endif

////
// Fake pointer to the GPIO register
// NOTE: this GPIO_TypeDef is quite big, with 10s of members with their own typedeffed type...
// I'm going to add only the members that I need, and add as I need more of them
///

/* IO definitions (access restrictions to peripheral registers) */
#define __IOM volatile /*! Defines 'read / write' structure member permissions */

typedef struct {
    __IOM uint32_t ROUTEEN;    /**< TIMER0 pin enable                                  */
    __IOM uint32_t CC0ROUTE;   /**< CC0 port/pin select                                */
    __IOM uint32_t CC1ROUTE;   /**< CC1 port/pin select                                */
    __IOM uint32_t CC2ROUTE;   /**< CC2 port/pin select                                */
    __IOM uint32_t CDTI0ROUTE; /**< CDTI0 port/pin select                              */
    __IOM uint32_t CDTI1ROUTE; /**< CDTI1 port/pin select                              */
    __IOM uint32_t CDTI2ROUTE; /**< CDTI2 port/pin select                              */
    uint32_t RESERVED0[1U];    /**< Reserved for future use                            */
} GPIO_TIMERROUTE_TypeDef;

typedef struct {
    GPIO_TIMERROUTE_TypeDef TIMERROUTE[5U];     /**< timer0 DBUS config registers                       */
    GPIO_TIMERROUTE_TypeDef TIMERROUTE_SET[5U]; /**< timer0 DBUS config registers                       */
    GPIO_TIMERROUTE_TypeDef TIMERROUTE_CLR[5U]; /**< timer0 DBUS config registers                       */
} GPIO_TypeDef;

// MAGIC line that makes registers testable!
EXTERN GPIO_TypeDef* GPIO;

#endif // _MICRO_REGISTERS_H_