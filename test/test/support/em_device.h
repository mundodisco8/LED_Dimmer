#ifndef EM_DEVICE_H
#define EM_DEVICE_H

////
// Minimal em_device.h to get the SDK libraries working in unit testing
////

// #define __STATIC_INLINE static inline
#define __STATIC_INLINE static inline

////
// IRQn definition, needed to mock app_log.h

/** Interrupt Number Definition */
typedef enum IRQn{
  /******  Cortex-M Processor Exceptions Numbers ******************************************/
  NonMaskableInt_IRQn    = -14,             /*!< -14 Cortex-M Non Maskable Interrupt      */
  HardFault_IRQn         = -13,             /*!< -13 Cortex-M Hard Fault Interrupt        */
  MemoryManagement_IRQn  = -12,             /*!< -12 Cortex-M Memory Management Interrupt */
  BusFault_IRQn          = -11,             /*!< -11 Cortex-M Bus Fault Interrupt         */
  UsageFault_IRQn        = -10,             /*!< -10 Cortex-M Usage Fault Interrupt       */
  SVCall_IRQn            = -5,              /*!< -5  Cortex-M SV Call Interrupt           */
  DebugMonitor_IRQn      = -4,              /*!< -4  Cortex-M Debug Monitor Interrupt     */
  PendSV_IRQn            = -2,              /*!< -2  Cortex-M Pend SV Interrupt           */
  SysTick_IRQn           = -1,              /*!< -1  Cortex-M System Tick Interrupt       */

  /******  BGM22 Peripheral Interrupt Numbers ******************************************/

  CRYPTOACC_IRQn         = 0,  /*!<  0 EFR32 CRYPTOACC Interrupt */
  TRNG_IRQn              = 1,  /*!<  1 EFR32 TRNG Interrupt */
  PKE_IRQn               = 2,  /*!<  2 EFR32 PKE Interrupt */
  SMU_SECURE_IRQn        = 3,  /*!<  3 EFR32 SMU_SECURE Interrupt */
  SMU_S_PRIVILEGED_IRQn  = 4,  /*!<  4 EFR32 SMU_S_PRIVILEGED Interrupt */
  SMU_NS_PRIVILEGED_IRQn = 5,  /*!<  5 EFR32 SMU_NS_PRIVILEGED Interrupt */
  EMU_IRQn               = 6,  /*!<  6 EFR32 EMU Interrupt */
  TIMER0_IRQn            = 7,  /*!<  7 EFR32 TIMER0 Interrupt */
  TIMER1_IRQn            = 8,  /*!<  8 EFR32 TIMER1 Interrupt */
  TIMER2_IRQn            = 9,  /*!<  9 EFR32 TIMER2 Interrupt */
  TIMER3_IRQn            = 10, /*!< 10 EFR32 TIMER3 Interrupt */
  TIMER4_IRQn            = 11, /*!< 11 EFR32 TIMER4 Interrupt */
  RTCC_IRQn              = 12, /*!< 12 EFR32 RTCC Interrupt */
  USART0_RX_IRQn         = 13, /*!< 13 EFR32 USART0_RX Interrupt */
  USART0_TX_IRQn         = 14, /*!< 14 EFR32 USART0_TX Interrupt */
  USART1_RX_IRQn         = 15, /*!< 15 EFR32 USART1_RX Interrupt */
  USART1_TX_IRQn         = 16, /*!< 16 EFR32 USART1_TX Interrupt */
  ICACHE0_IRQn           = 17, /*!< 17 EFR32 ICACHE0 Interrupt */
  BURTC_IRQn             = 18, /*!< 18 EFR32 BURTC Interrupt */
  LETIMER0_IRQn          = 19, /*!< 19 EFR32 LETIMER0 Interrupt */
  SYSCFG_IRQn            = 20, /*!< 20 EFR32 SYSCFG Interrupt */
  LDMA_IRQn              = 21, /*!< 21 EFR32 LDMA Interrupt */
  LFXO_IRQn              = 22, /*!< 22 EFR32 LFXO Interrupt */
  LFRCO_IRQn             = 23, /*!< 23 EFR32 LFRCO Interrupt */
  ULFRCO_IRQn            = 24, /*!< 24 EFR32 ULFRCO Interrupt */
  GPIO_ODD_IRQn          = 25, /*!< 25 EFR32 GPIO_ODD Interrupt */
  GPIO_EVEN_IRQn         = 26, /*!< 26 EFR32 GPIO_EVEN Interrupt */
  I2C0_IRQn              = 27, /*!< 27 EFR32 I2C0 Interrupt */
  I2C1_IRQn              = 28, /*!< 28 EFR32 I2C1 Interrupt */
  EMUDG_IRQn             = 29, /*!< 29 EFR32 EMUDG Interrupt */
  EMUSE_IRQn             = 30, /*!< 30 EFR32 EMUSE Interrupt */
  AGC_IRQn               = 31, /*!< 31 EFR32 AGC Interrupt */
  BUFC_IRQn              = 32, /*!< 32 EFR32 BUFC Interrupt */
  FRC_PRI_IRQn           = 33, /*!< 33 EFR32 FRC_PRI Interrupt */
  FRC_IRQn               = 34, /*!< 34 EFR32 FRC Interrupt */
  MODEM_IRQn             = 35, /*!< 35 EFR32 MODEM Interrupt */
  PROTIMER_IRQn          = 36, /*!< 36 EFR32 PROTIMER Interrupt */
  RAC_RSM_IRQn           = 37, /*!< 37 EFR32 RAC_RSM Interrupt */
  RAC_SEQ_IRQn           = 38, /*!< 38 EFR32 RAC_SEQ Interrupt */
  RDMAILBOX_IRQn         = 39, /*!< 39 EFR32 RDMAILBOX Interrupt */
  RFSENSE_IRQn           = 40, /*!< 40 EFR32 RFSENSE Interrupt */
  PRORTC_IRQn            = 41, /*!< 41 EFR32 PRORTC Interrupt */
  SYNTH_IRQn             = 42, /*!< 42 EFR32 SYNTH Interrupt */
  WDOG0_IRQn             = 43, /*!< 43 EFR32 WDOG0 Interrupt */
  HFXO0_IRQn             = 44, /*!< 44 EFR32 HFXO0 Interrupt */
  HFRCO0_IRQn            = 45, /*!< 45 EFR32 HFRCO0 Interrupt */
  CMU_IRQn               = 46, /*!< 46 EFR32 CMU Interrupt */
  AES_IRQn               = 47, /*!< 47 EFR32 AES Interrupt */
  IADC_IRQn              = 48, /*!< 48 EFR32 IADC Interrupt */
  MSC_IRQn               = 49, /*!< 49 EFR32 MSC Interrupt */
  DPLL0_IRQn             = 50, /*!< 50 EFR32 DPLL0 Interrupt */
  PDM_IRQn               = 51, /*!< 51 EFR32 PDM Interrupt */
  SW0_IRQn               = 52, /*!< 52 EFR32 SW0 Interrupt */
  SW1_IRQn               = 53, /*!< 53 EFR32 SW1 Interrupt */
  SW2_IRQn               = 54, /*!< 54 EFR32 SW2 Interrupt */
  SW3_IRQn               = 55, /*!< 55 EFR32 SW3 Interrupt */
  KERNEL0_IRQn           = 56, /*!< 56 EFR32 KERNEL0 Interrupt */
  KERNEL1_IRQn           = 57, /*!< 57 EFR32 KERNEL1 Interrupt */
  M33CTI0_IRQn           = 58, /*!< 58 EFR32 M33CTI0 Interrupt */
  M33CTI1_IRQn           = 59, /*!< 59 EFR32 M33CTI1 Interrupt */
  EMUEFP_IRQn            = 60, /*!< 60 EFR32 EMUEFP Interrupt */
  DCDC_IRQn              = 61, /*!< 61 EFR32 DCDC Interrupt */
  EUART0_RX_IRQn         = 62, /*!< 62 EFR32 EUART0_RX Interrupt */
  EUART0_TX_IRQn         = 63, /*!< 63 EFR32 EUART0_TX Interrupt */
} IRQn_Type;

// #define CMU_PRESENT
// #define _SILICON_LABS_32B_SERIES_2
// #define _SILICON_LABS_32B_SERIES_2_CONFIG                 2
// #warning "fake em_device.h in use"

#endif /* EM_DEVICE_H */
