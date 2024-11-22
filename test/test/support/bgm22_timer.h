#ifndef BGM22_TIMER_H
#define BGM22_TIMER_H
#define TIMER_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup BGM22_TIMER TIMER
 * @{
 * @brief BGM22 TIMER Register Declaration.
 *****************************************************************************/

/** TIMER CC Register Group Declaration. */
typedef struct timer_cc_typedef{
  __IOM uint32_t CFG;                                /**< CC Channel Configuration Register                  */
  __IOM uint32_t CTRL;                               /**< CC Channel Control Register                        */
  __IOM uint32_t OC;                                 /**< OC Channel Value Register                          */
  uint32_t       RESERVED0[1U];                      /**< Reserved for future use                            */
  __IOM uint32_t OCB;                                /**< OC Channel Value Buffer Register                   */
  __IM uint32_t  ICF;                                /**< IC Channel Value Register                          */
  __IM uint32_t  ICOF;                               /**< IC Channel Value Overflow Register                 */
  uint32_t       RESERVED1[1U];                      /**< Reserved for future use                            */
} TIMER_CC_TypeDef;

/** TIMER Register Declaration. */
typedef struct timer_typedef{
  __IM uint32_t    IPVERSION;                   /**< IP version ID                                      */
  __IOM uint32_t   CFG;                         /**< Configuration Register                             */
  __IOM uint32_t   CTRL;                        /**< Control Register                                   */
  __IOM uint32_t   CMD;                         /**< Command Register                                   */
  __IM uint32_t    STATUS;                      /**< Status Register                                    */
  __IOM uint32_t   IF;                          /**< Interrupt Flag Register                            */
  __IOM uint32_t   IEN;                         /**< Interrupt Enable Register                          */
  __IOM uint32_t   TOP;                         /**< Counter Top Value Register                         */
  __IOM uint32_t   TOPB;                        /**< Counter Top Value Buffer Register                  */
  __IOM uint32_t   CNT;                         /**< Counter Value Register                             */
  uint32_t         RESERVED0[1U];               /**< Reserved for future use                            */
  __IOM uint32_t   LOCK;                        /**< TIMER Configuration Lock Register                  */
  __IOM uint32_t   EN;                          /**< module en                                          */
  uint32_t         RESERVED1[11U];              /**< Reserved for future use                            */
  TIMER_CC_TypeDef CC[3U];                      /**< Compare/Capture Channel                            */
  uint32_t         RESERVED2[8U];               /**< Reserved for future use                            */
  __IOM uint32_t   DTCFG;                       /**< DTI Configuration Register                         */
  __IOM uint32_t   DTTIMECFG;                   /**< DTI Time Configuration Register                    */
  __IOM uint32_t   DTFCFG;                      /**< DTI Fault Configuration Register                   */
  __IOM uint32_t   DTCTRL;                      /**< DTI Control Register                               */
  __IOM uint32_t   DTOGEN;                      /**< DTI Output Generation Enable Register              */
  __IM uint32_t    DTFAULT;                     /**< DTI Fault Register                                 */
  __IOM uint32_t   DTFAULTC;                    /**< DTI Fault Clear Register                           */
  __IOM uint32_t   DTLOCK;                      /**< DTI Configuration Lock Register                    */
  uint32_t         RESERVED3[960U];             /**< Reserved for future use                            */
  __IM uint32_t    IPVERSION_SET;               /**< IP version ID                                      */
  __IOM uint32_t   CFG_SET;                     /**< Configuration Register                             */
  __IOM uint32_t   CTRL_SET;                    /**< Control Register                                   */
  __IOM uint32_t   CMD_SET;                     /**< Command Register                                   */
  __IM uint32_t    STATUS_SET;                  /**< Status Register                                    */
  __IOM uint32_t   IF_SET;                      /**< Interrupt Flag Register                            */
  __IOM uint32_t   IEN_SET;                     /**< Interrupt Enable Register                          */
  __IOM uint32_t   TOP_SET;                     /**< Counter Top Value Register                         */
  __IOM uint32_t   TOPB_SET;                    /**< Counter Top Value Buffer Register                  */
  __IOM uint32_t   CNT_SET;                     /**< Counter Value Register                             */
  uint32_t         RESERVED4[1U];               /**< Reserved for future use                            */
  __IOM uint32_t   LOCK_SET;                    /**< TIMER Configuration Lock Register                  */
  __IOM uint32_t   EN_SET;                      /**< module en                                          */
  uint32_t         RESERVED5[11U];              /**< Reserved for future use                            */
  TIMER_CC_TypeDef CC_SET[3U];                  /**< Compare/Capture Channel                            */
  uint32_t         RESERVED6[8U];               /**< Reserved for future use                            */
  __IOM uint32_t   DTCFG_SET;                   /**< DTI Configuration Register                         */
  __IOM uint32_t   DTTIMECFG_SET;               /**< DTI Time Configuration Register                    */
  __IOM uint32_t   DTFCFG_SET;                  /**< DTI Fault Configuration Register                   */
  __IOM uint32_t   DTCTRL_SET;                  /**< DTI Control Register                               */
  __IOM uint32_t   DTOGEN_SET;                  /**< DTI Output Generation Enable Register              */
  __IM uint32_t    DTFAULT_SET;                 /**< DTI Fault Register                                 */
  __IOM uint32_t   DTFAULTC_SET;                /**< DTI Fault Clear Register                           */
  __IOM uint32_t   DTLOCK_SET;                  /**< DTI Configuration Lock Register                    */
  uint32_t         RESERVED7[960U];             /**< Reserved for future use                            */
  __IM uint32_t    IPVERSION_CLR;               /**< IP version ID                                      */
  __IOM uint32_t   CFG_CLR;                     /**< Configuration Register                             */
  __IOM uint32_t   CTRL_CLR;                    /**< Control Register                                   */
  __IOM uint32_t   CMD_CLR;                     /**< Command Register                                   */
  __IM uint32_t    STATUS_CLR;                  /**< Status Register                                    */
  __IOM uint32_t   IF_CLR;                      /**< Interrupt Flag Register                            */
  __IOM uint32_t   IEN_CLR;                     /**< Interrupt Enable Register                          */
  __IOM uint32_t   TOP_CLR;                     /**< Counter Top Value Register                         */
  __IOM uint32_t   TOPB_CLR;                    /**< Counter Top Value Buffer Register                  */
  __IOM uint32_t   CNT_CLR;                     /**< Counter Value Register                             */
  uint32_t         RESERVED8[1U];               /**< Reserved for future use                            */
  __IOM uint32_t   LOCK_CLR;                    /**< TIMER Configuration Lock Register                  */
  __IOM uint32_t   EN_CLR;                      /**< module en                                          */
  uint32_t         RESERVED9[11U];              /**< Reserved for future use                            */
  TIMER_CC_TypeDef CC_CLR[3U];                  /**< Compare/Capture Channel                            */
  uint32_t         RESERVED10[8U];              /**< Reserved for future use                            */
  __IOM uint32_t   DTCFG_CLR;                   /**< DTI Configuration Register                         */
  __IOM uint32_t   DTTIMECFG_CLR;               /**< DTI Time Configuration Register                    */
  __IOM uint32_t   DTFCFG_CLR;                  /**< DTI Fault Configuration Register                   */
  __IOM uint32_t   DTCTRL_CLR;                  /**< DTI Control Register                               */
  __IOM uint32_t   DTOGEN_CLR;                  /**< DTI Output Generation Enable Register              */
  __IM uint32_t    DTFAULT_CLR;                 /**< DTI Fault Register                                 */
  __IOM uint32_t   DTFAULTC_CLR;                /**< DTI Fault Clear Register                           */
  __IOM uint32_t   DTLOCK_CLR;                  /**< DTI Configuration Lock Register                    */
  uint32_t         RESERVED11[960U];            /**< Reserved for future use                            */
  __IM uint32_t    IPVERSION_TGL;               /**< IP version ID                                      */
  __IOM uint32_t   CFG_TGL;                     /**< Configuration Register                             */
  __IOM uint32_t   CTRL_TGL;                    /**< Control Register                                   */
  __IOM uint32_t   CMD_TGL;                     /**< Command Register                                   */
  __IM uint32_t    STATUS_TGL;                  /**< Status Register                                    */
  __IOM uint32_t   IF_TGL;                      /**< Interrupt Flag Register                            */
  __IOM uint32_t   IEN_TGL;                     /**< Interrupt Enable Register                          */
  __IOM uint32_t   TOP_TGL;                     /**< Counter Top Value Register                         */
  __IOM uint32_t   TOPB_TGL;                    /**< Counter Top Value Buffer Register                  */
  __IOM uint32_t   CNT_TGL;                     /**< Counter Value Register                             */
  uint32_t         RESERVED12[1U];              /**< Reserved for future use                            */
  __IOM uint32_t   LOCK_TGL;                    /**< TIMER Configuration Lock Register                  */
  __IOM uint32_t   EN_TGL;                      /**< module en                                          */
  uint32_t         RESERVED13[11U];             /**< Reserved for future use                            */
  TIMER_CC_TypeDef CC_TGL[3U];                  /**< Compare/Capture Channel                            */
  uint32_t         RESERVED14[8U];              /**< Reserved for future use                            */
  __IOM uint32_t   DTCFG_TGL;                   /**< DTI Configuration Register                         */
  __IOM uint32_t   DTTIMECFG_TGL;               /**< DTI Time Configuration Register                    */
  __IOM uint32_t   DTFCFG_TGL;                  /**< DTI Fault Configuration Register                   */
  __IOM uint32_t   DTCTRL_TGL;                  /**< DTI Control Register                               */
  __IOM uint32_t   DTOGEN_TGL;                  /**< DTI Output Generation Enable Register              */
  __IM uint32_t    DTFAULT_TGL;                 /**< DTI Fault Register                                 */
  __IOM uint32_t   DTFAULTC_TGL;                /**< DTI Fault Clear Register                           */
  __IOM uint32_t   DTLOCK_TGL;                  /**< DTI Configuration Lock Register                    */
} TIMER_TypeDef;
/** @} End of group BGM22_TIMER */

#endif