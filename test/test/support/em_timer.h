#ifndef EM_TIMER_H_
#define EM_TIMER_H_

#include <stdbool.h>

#include "em_device.h"

#include "micro_registers.h"

// FAKE EM_TIMER for testing

////
// Default configs
////

/** Input capture event control. */
#define _TIMER_CC_CTRL_ICEVCTRL_EVERYEDGE 0x00000000UL       /**< Mode EVERYEDGE for TIMER_CC_CTRL            */
#define _TIMER_CC_CTRL_ICEVCTRL_EVERYSECONDEDGE 0x00000001UL /**< Mode EVERYSECONDEDGE for TIMER_CC_CTRL      */
#define _TIMER_CC_CTRL_ICEVCTRL_RISING 0x00000002UL          /**< Mode RISING for TIMER_CC_CTRL               */
#define _TIMER_CC_CTRL_ICEVCTRL_FALLING 0x00000003UL
typedef enum {
    /** PRS output pulse, interrupt flag, and DMA request set on every capture. */
    timerEventEveryEdge = _TIMER_CC_CTRL_ICEVCTRL_EVERYEDGE,
    /** PRS output pulse, interrupt flag, and DMA request set on every second capture. */
    timerEventEvery2ndEdge = _TIMER_CC_CTRL_ICEVCTRL_EVERYSECONDEDGE,
    /**
     * PRS output pulse, interrupt flag, and DMA request set on rising edge (if
     * input capture edge = BOTH).
     */
    timerEventRising = _TIMER_CC_CTRL_ICEVCTRL_RISING,
    /**
     * PRS output pulse, interrupt flag, and DMA request set on falling edge (if
     * input capture edge = BOTH).
     */
    timerEventFalling = _TIMER_CC_CTRL_ICEVCTRL_FALLING
} TIMER_Event_TypeDef;

/** Input capture edge select. */
#define _TIMER_CC_CTRL_ICEDGE_RISING 0x00000000UL  /**< Mode RISING for TIMER_CC_CTRL               */
#define _TIMER_CC_CTRL_ICEDGE_FALLING 0x00000001UL /**< Mode FALLING for TIMER_CC_CTRL              */
#define _TIMER_CC_CTRL_ICEDGE_BOTH 0x00000002UL    /**< Mode BOTH for TIMER_CC_CTRL                 */
#define _TIMER_CC_CTRL_ICEDGE_NONE 0x00000003UL    /**< Mode NONE for TIMER_CC_CTRL                 */
typedef enum {
    /** Rising edges detected. */
    timerEdgeRising = _TIMER_CC_CTRL_ICEDGE_RISING,

    /** Falling edges detected. */
    timerEdgeFalling = _TIMER_CC_CTRL_ICEDGE_FALLING,

    /** Both edges detected. */
    timerEdgeBoth = _TIMER_CC_CTRL_ICEDGE_BOTH,

    /** No edge detection, leave signal as is. */
    timerEdgeNone = _TIMER_CC_CTRL_ICEDGE_NONE
} TIMER_Edge_TypeDef;

typedef uint8_t TIMER_PRSSEL_TypeDef;

/** Compare/capture output action. */
#define _TIMER_CC_CTRL_CUFOA_NONE 0x00000000UL   /**< Mode NONE for TIMER_CC_CTRL                 */
#define _TIMER_CC_CTRL_CUFOA_TOGGLE 0x00000001UL /**< Mode TOGGLE for TIMER_CC_CTRL               */
#define _TIMER_CC_CTRL_CUFOA_CLEAR 0x00000002UL  /**< Mode CLEAR for TIMER_CC_CTRL                */
#define _TIMER_CC_CTRL_CUFOA_SET 0x00000003UL
typedef enum {
    /** No action. */
    timerOutputActionNone = _TIMER_CC_CTRL_CUFOA_NONE,

    /** Toggle on event. */
    timerOutputActionToggle = _TIMER_CC_CTRL_CUFOA_TOGGLE,

    /** Clear on event. */
    timerOutputActionClear = _TIMER_CC_CTRL_CUFOA_CLEAR,

    /** Set on event. */
    timerOutputActionSet = _TIMER_CC_CTRL_CUFOA_SET
} TIMER_OutputAction_TypeDef;

/** Timer compare/capture mode. */
#define _TIMER_CC_CFG_MODE_MASK 0x3UL                 /**< Bit mask for TIMER_MODE                     */
#define _TIMER_CC_CFG_MODE_OFF 0x00000000UL           /**< Mode OFF for TIMER_CC_CFG                   */
#define _TIMER_CC_CFG_MODE_INPUTCAPTURE 0x00000001UL  /**< Mode INPUTCAPTURE for TIMER_CC_CFG          */
#define _TIMER_CC_CFG_MODE_OUTPUTCOMPARE 0x00000002UL /**< Mode OUTPUTCOMPARE for TIMER_CC_CFG         */
#define _TIMER_CC_CFG_MODE_PWM 0x00000003UL           /**< Mode PWM for TIMER_CC_CFG                   */
typedef enum {
#if defined(_TIMER_CC_CTRL_MODE_MASK)
    timerCCModeOff = _TIMER_CC_CTRL_MODE_OFF,               /**< Channel turned off. */
    timerCCModeCapture = _TIMER_CC_CTRL_MODE_INPUTCAPTURE,  /**< Input capture. */
    timerCCModeCompare = _TIMER_CC_CTRL_MODE_OUTPUTCOMPARE, /**< Output compare. */
    timerCCModePWM = _TIMER_CC_CTRL_MODE_PWM                /**< Pulse-Width modulation. */
#endif
#if defined(_TIMER_CC_CFG_MODE_MASK)
        timerCCModeOff = _TIMER_CC_CFG_MODE_OFF,           /**< Channel turned off. */
    timerCCModeCapture = _TIMER_CC_CFG_MODE_INPUTCAPTURE,  /**< Input capture. */
    timerCCModeCompare = _TIMER_CC_CFG_MODE_OUTPUTCOMPARE, /**< Output compare. */
    timerCCModePWM = _TIMER_CC_CFG_MODE_PWM                /**< Pulse-Width modulation. */
#endif
} TIMER_CCMode_TypeDef;

/** PRS Output configuration. */
typedef enum {
    timerPrsOutputPulse = 0,                     /**< Pulse PRS output from a channel. */
    timerPrsOutputLevel = 1,                     /**< PRS output follows CC out level. */
    timerPrsOutputDefault = timerPrsOutputPulse, /**< Default PRS output behavior. */
} TIMER_PrsOutput_t;

/** PRS input type */
#define _TIMER_CC_CFG_INSEL_MASK 0x60000UL             /**< Bit mask for TIMER_INSEL                    */
#define _TIMER_CC_CFG_INSEL_PRSSYNC 0x00000001UL       /**< Mode PRSSYNC for TIMER_CC_CFG               */
#define _TIMER_CC_CFG_INSEL_PRSASYNCLEVEL 0x00000002UL /**< Mode PRSASYNCLEVEL for TIMER_CC_CFG         */
#define _TIMER_CC_CFG_INSEL_PRSASYNCPULSE 0x00000003UL /**< Mode PRSASYNCPULSE for TIMER_CC_CFG         */
typedef enum {
    timerPrsInputNone = 0x0,                                     /**< No PRS input. */
    timerPrsInputSync = _TIMER_CC_CFG_INSEL_PRSSYNC,             /**< Synchronous PRS selected. */
    timerPrsInputAsyncLevel = _TIMER_CC_CFG_INSEL_PRSASYNCLEVEL, /**< Asynchronous level PRS selected. */
    timerPrsInputAsyncPulse = _TIMER_CC_CFG_INSEL_PRSASYNCPULSE, /**< Asynchronous pulse PRS selected. */
} TIMER_PrsInput_TypeDef;

/** TIMER compare/capture initialization structure. */
typedef struct {
    /** Input capture event control. */
    TIMER_Event_TypeDef eventCtrl;

    /** Input capture edge select. */
    TIMER_Edge_TypeDef edge;

    /**
     * Peripheral reflex system trigger selection. Only applicable if @p prsInput
     * is enabled.
     */
    TIMER_PRSSEL_TypeDef prsSel;

    /** Counter underflow output action. */
    TIMER_OutputAction_TypeDef cufoa;

    /** Counter overflow output action. */
    TIMER_OutputAction_TypeDef cofoa;

    /** Counter match output action. */
    TIMER_OutputAction_TypeDef cmoa;

    /** Compare/capture channel mode. */
    TIMER_CCMode_TypeDef mode;

    /** Enable digital filter. */
    bool filter;

    /** Select TIMERnCCx (false) or PRS input (true). */
    bool prsInput;

    /**
     * Compare output initial state. Only used in Output Compare and PWM mode.
     * When true, the compare/PWM output is set high when the counter is
     * disabled. When counting resumes, this value will represent the initial
     * value for the compare/PWM output. If the bit is cleared, the output
     * will be cleared when the counter is disabled.
     */
    bool coist;

    /** Invert output from compare/capture channel. */
    bool outInvert;

    /**
     * PRS output configuration. PRS output from a timer can either be a
     * pulse output or a level output that follows the CC out value.
     */
    TIMER_PrsOutput_t prsOutput;

#if defined(_TIMER_CC_CFG_INSEL_MASK)
    /** When PRS input is used this field is used to configure the type of
     * PRS input. */
    TIMER_PrsInput_TypeDef prsInputType;
#endif
} TIMER_InitCC_TypeDef;

/** Prescaler. */
#define _TIMER_CFG_PRESC_MASK 0xFFC0000UL     /**< Bit mask for TIMER_PRESC                    */
#define _TIMER_CFG_PRESC_DIV1 0x00000000UL    /**< Mode DIV1 for TIMER_CFG                     */
#define _TIMER_CFG_PRESC_DIV2 0x00000001UL    /**< Mode DIV2 for TIMER_CFG                     */
#define _TIMER_CFG_PRESC_DIV4 0x00000003UL    /**< Mode DIV4 for TIMER_CFG                     */
#define _TIMER_CFG_PRESC_DIV8 0x00000007UL    /**< Mode DIV8 for TIMER_CFG                     */
#define _TIMER_CFG_PRESC_DIV16 0x0000000FUL   /**< Mode DIV16 for TIMER_CFG                    */
#define _TIMER_CFG_PRESC_DIV32 0x0000001FUL   /**< Mode DIV32 for TIMER_CFG                    */
#define _TIMER_CFG_PRESC_DIV64 0x0000003FUL   /**< Mode DIV64 for TIMER_CFG                    */
#define _TIMER_CFG_PRESC_DIV128 0x0000007FUL  /**< Mode DIV128 for TIMER_CFG                   */
#define _TIMER_CFG_PRESC_DIV256 0x000000FFUL  /**< Mode DIV256 for TIMER_CFG                   */
#define _TIMER_CFG_PRESC_DIV512 0x000001FFUL  /**< Mode DIV512 for TIMER_CFG                   */
#define _TIMER_CFG_PRESC_DIV1024 0x000003FFUL /**< Mode DIV1024 for TIMER_CFG                  */
typedef enum {
#if defined(_TIMER_CTRL_PRESC_MASK)
    timerPrescale1 = _TIMER_CTRL_PRESC_DIV1,      /**< Divide by 1. */
    timerPrescale2 = _TIMER_CTRL_PRESC_DIV2,      /**< Divide by 2. */
    timerPrescale4 = _TIMER_CTRL_PRESC_DIV4,      /**< Divide by 4. */
    timerPrescale8 = _TIMER_CTRL_PRESC_DIV8,      /**< Divide by 8. */
    timerPrescale16 = _TIMER_CTRL_PRESC_DIV16,    /**< Divide by 16. */
    timerPrescale32 = _TIMER_CTRL_PRESC_DIV32,    /**< Divide by 32. */
    timerPrescale64 = _TIMER_CTRL_PRESC_DIV64,    /**< Divide by 64. */
    timerPrescale128 = _TIMER_CTRL_PRESC_DIV128,  /**< Divide by 128. */
    timerPrescale256 = _TIMER_CTRL_PRESC_DIV256,  /**< Divide by 256. */
    timerPrescale512 = _TIMER_CTRL_PRESC_DIV512,  /**< Divide by 512. */
    timerPrescale1024 = _TIMER_CTRL_PRESC_DIV1024 /**< Divide by 1024. */
#endif
#if defined(_TIMER_CFG_PRESC_MASK)
        timerPrescale1 = _TIMER_CFG_PRESC_DIV1,  /**< Divide by 1. */
    timerPrescale2 = _TIMER_CFG_PRESC_DIV2,      /**< Divide by 2. */
    timerPrescale4 = _TIMER_CFG_PRESC_DIV4,      /**< Divide by 4. */
    timerPrescale8 = _TIMER_CFG_PRESC_DIV8,      /**< Divide by 8. */
    timerPrescale16 = _TIMER_CFG_PRESC_DIV16,    /**< Divide by 16. */
    timerPrescale32 = _TIMER_CFG_PRESC_DIV32,    /**< Divide by 32. */
    timerPrescale64 = _TIMER_CFG_PRESC_DIV64,    /**< Divide by 64. */
    timerPrescale128 = _TIMER_CFG_PRESC_DIV128,  /**< Divide by 128. */
    timerPrescale256 = _TIMER_CFG_PRESC_DIV256,  /**< Divide by 256. */
    timerPrescale512 = _TIMER_CFG_PRESC_DIV512,  /**< Divide by 512. */
    timerPrescale1024 = _TIMER_CFG_PRESC_DIV1024 /**< Divide by 1024. */
#endif
} TIMER_Prescale_TypeDef;

/** Clock select. */
#define _TIMER_CFG_CLKSEL_MASK 0x300UL                  /**< Bit mask for TIMER_CLKSEL                   */
#define _TIMER_CFG_CLKSEL_PRESCEM01GRPACLK 0x00000000UL /**< Mode PRESCEM01GRPACLK for TIMER_CFG         */
#define _TIMER_CFG_CLKSEL_CC1 0x00000001UL              /**< Mode CC1 for TIMER_CFG                      */
#define _TIMER_CFG_CLKSEL_TIMEROUF 0x00000002UL         /**< Mode TIMEROUF for TIMER_CFG                 */
typedef enum {
#if defined(_TIMER_CTRL_CLKSEL_MASK)
    timerClkSelHFPerClk = _TIMER_CTRL_CLKSEL_PRESCHFPERCLK, /**< Prescaled HFPER / HFPERB clock. */
    timerClkSelCC1 = _TIMER_CTRL_CLKSEL_CC1,                /**< Compare/Capture Channel 1 Input. */
    timerClkSelCascade = _TIMER_CTRL_CLKSEL_TIMEROUF        /**< Cascaded clocked by underflow or overflow by lower numbered timer. */
#endif
#if defined(_TIMER_CFG_CLKSEL_MASK)
        timerClkSelHFPerClk = _TIMER_CFG_CLKSEL_PRESCEM01GRPACLK, /**< Prescaled EM01GRPA clock. */
    timerClkSelCC1 = _TIMER_CFG_CLKSEL_CC1,                       /**< Compare/Capture Channel 1 Input. */
    timerClkSelCascade = _TIMER_CFG_CLKSEL_TIMEROUF               /**< Cascaded clocked by underflow or overflow by lower numbered timer. */
#endif
} TIMER_ClkSel_TypeDef;

/** Input edge action. */
#define _TIMER_CTRL_FALLA_NONE 0x00000000UL        /**< Mode NONE for TIMER_CTRL                    */
#define _TIMER_CTRL_FALLA_START 0x00000001UL       /**< Mode START for TIMER_CTRL                   */
#define _TIMER_CTRL_FALLA_STOP 0x00000002UL        /**< Mode STOP for TIMER_CTRL                    */
#define _TIMER_CTRL_FALLA_RELOADSTART 0x00000003UL /**< Mode RELOADSTART for TIMER_CTRL             */
typedef enum {
    /** No action taken. */
    timerInputActionNone = _TIMER_CTRL_FALLA_NONE,

    /** Start counter without reload. */
    timerInputActionStart = _TIMER_CTRL_FALLA_START,

    /** Stop counter without reload. */
    timerInputActionStop = _TIMER_CTRL_FALLA_STOP,

    /** Reload and start counter. */
    timerInputActionReloadStart = _TIMER_CTRL_FALLA_RELOADSTART
} TIMER_InputAction_TypeDef;

/** Timer mode. */
#define _TIMER_CFG_MODE_MASK 0x3UL          /**< Bit mask for TIMER_MODE                     */
#define _TIMER_CFG_MODE_UP 0x00000000UL     /**< Mode UP for TIMER_CFG                       */
#define _TIMER_CFG_MODE_DOWN 0x00000001UL   /**< Mode DOWN for TIMER_CFG                     */
#define _TIMER_CFG_MODE_UPDOWN 0x00000002UL /**< Mode UPDOWN for TIMER_CFG                   */
#define _TIMER_CFG_MODE_QDEC 0x00000003UL   /**< Mode QDEC for TIMER_CFG                     */

typedef enum {
#if defined(_TIMER_CTRL_MODE_MASK)
    timerModeUp = _TIMER_CTRL_MODE_UP,         /**< Up-counting. */
    timerModeDown = _TIMER_CTRL_MODE_DOWN,     /**< Down-counting. */
    timerModeUpDown = _TIMER_CTRL_MODE_UPDOWN, /**< Up/down-counting. */
    timerModeQDec = _TIMER_CTRL_MODE_QDEC      /**< Quadrature decoder. */
#endif
#if defined(_TIMER_CFG_MODE_MASK)
        timerModeUp = _TIMER_CFG_MODE_UP,     /**< Up-counting. */
    timerModeDown = _TIMER_CFG_MODE_DOWN,     /**< Down-counting. */
    timerModeUpDown = _TIMER_CFG_MODE_UPDOWN, /**< Up/down-counting. */
    timerModeQDec = _TIMER_CFG_MODE_QDEC      /**< Quadrature decoder. */
#endif
} TIMER_Mode_TypeDef;

/** TIMER initialization structure. */
#define TIMER_CTRL_X2CNT (0x1UL << 4) /**< 2x Count Mode                               */
#define TIMER_CFG_ATI (0x1UL << 16)   /**< Always Track Inputs                         */

typedef struct {
    /** Start counting when initialization completed. */
    bool enable;

    /** Counter shall keep running during debug halt. */
    bool debugRun;

    /** Prescaling factor, if HFPER / HFPERB clock used. */
    TIMER_Prescale_TypeDef prescale;

    /** Clock selection. */
    TIMER_ClkSel_TypeDef clkSel;

#if defined(TIMER_CTRL_X2CNT) && (defined(TIMER_CTRL_ATI) || defined(TIMER_CFG_ATI))
    /** 2x Count mode, counter increments/decrements by 2, meant for PWM mode. */
    bool count2x;

    /** ATI (Always Track Inputs) makes CCPOL always track
     * the polarity of the inputs. */
    bool ati;
#endif

    /** Action on falling input edge. */
    TIMER_InputAction_TypeDef fallAction;

    /** Action on rising input edge. */
    TIMER_InputAction_TypeDef riseAction;

    /** Counting mode. */
    TIMER_Mode_TypeDef mode;

    /** DMA request clear on active. */
    bool dmaClrAct;

    /** Select X2 or X4 quadrature decode mode (if used). */
    bool quadModeX4;

    /** Determines if only counting up or down once. */
    bool oneShot;

    /** Timer start/stop/reload by other timers. */
    bool sync;
} TIMER_Init_TypeDef;

////
// Some defined values
////

#define TIMER_EN_EN (0x1UL << 0) /**< Timer Module Enable                         */
// #define TIMER_NUM(ref)                 (((ref) == TIMER0) ? 0   \
//                                         : ((ref) == TIMER1) ? 1 \
//                                         : ((ref) == TIMER2) ? 2 \
//                                         : ((ref) == TIMER3) ? 3 \
//                                         : ((ref) == TIMER4) ? 4 \
//                                         : -1)
#define TIMER_NUM(ref) (((ref) == TIMER0) ? 0 \
                                          : -1)
////
// Function declarations
////

void TIMER_CompareSet(TIMER_TypeDef *timer, unsigned int ch, uint32_t val);
void TIMER_CompareBufSet(TIMER_TypeDef *timer, unsigned int ch, uint32_t val);
void TIMER_TopSet(TIMER_TypeDef *timer, uint32_t val);
uint32_t TIMER_TopGet(TIMER_TypeDef *timer);
void TIMER_Init(TIMER_TypeDef *timer, const TIMER_Init_TypeDef *init);
void TIMER_InitCC(TIMER_TypeDef *timer, unsigned int ch, const TIMER_InitCC_TypeDef *init);
#endif  // EM_TIMER_H_