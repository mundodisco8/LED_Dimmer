#ifndef _REGISTER_TYPES_H_
#define _REGISTER_TYPES_H_

/********************************************************
 *
 * Contains all the stuff needed to define a register type. The Unity people recommend having it in 'micro_registers.h'
 * but we will also need this stuff in our edited em_somethings.h, because those functions take a register of a certain
 * type as parameter. We must add then `micro_registers.` to the relevant `em_something.h`'s AND to the test file, but
 * the problem here is that if we do that, we end up with two compilation units with definitions of the registers, and
 * the linker complains of multiple declarations.
 *
 * To avoid that, I define the register types here, which can be included in `micro_register.h` and any fake
 * `em_something.h' and the linker won't complain.
 */

#include <inttypes.h>

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

#endif // _REGISTER_TYPES_H_