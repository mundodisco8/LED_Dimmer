#ifndef EM_GPIO_H_
#define EM_GPIO_H_

#include <inttypes.h>
#include <stdbool.h>

#include "micro_registers.h"

////
// Defines needed to get GPIO_Port_Typedef correctly defined
////
#define GPIO_PA_COUNT 9U /**< Number of pins on port PA */
#define GPIO_PB_COUNT 5U /**< Number of pins on port PB */
#define GPIO_PC_COUNT 8U /**< Number of pins on port PC */
#define GPIO_PD_COUNT 3U

#define _GPIO_PORT_A_PIN_COUNT GPIO_PA_COUNT
#define _GPIO_PORT_B_PIN_COUNT GPIO_PB_COUNT
#define _GPIO_PORT_C_PIN_COUNT GPIO_PC_COUNT
#define _GPIO_PORT_D_PIN_COUNT GPIO_PD_COUNT
#define _GPIO_PORT_E_PIN_COUNT 0
#define _GPIO_PORT_F_PIN_COUNT 0
#define _GPIO_PORT_G_PIN_COUNT 0
#define _GPIO_PORT_H_PIN_COUNT 0
#define _GPIO_PORT_I_PIN_COUNT 0
#define _GPIO_PORT_J_PIN_COUNT 0
#define _GPIO_PORT_K_PIN_COUNT 0

#define _GPIO_PORT_A_PIN_MASK (GPIO_PA_MASK)
#define _GPIO_PORT_B_PIN_MASK (GPIO_PB_MASK)
#define _GPIO_PORT_C_PIN_MASK (GPIO_PC_MASK)
#define _GPIO_PORT_D_PIN_MASK (GPIO_PD_MASK)
#define _GPIO_PORT_E_PIN_MASK 0x0000UL
#define _GPIO_PORT_F_PIN_MASK 0x0000UL
#define _GPIO_PORT_G_PIN_MASK 0x0000UL
#define _GPIO_PORT_H_PIN_MASK 0x0000UL
#define _GPIO_PORT_I_PIN_MASK 0x0000UL
#define _GPIO_PORT_J_PIN_MASK 0x0000UL
#define _GPIO_PORT_K_PIN_MASK 0x0000UL

#define SL_ENUM(name)     \
    typedef uint8_t name; \
    enum name##_enum

SL_ENUM(GPIO_Port_TypeDef) {
#if (_GPIO_PORT_A_PIN_COUNT > 0)
    /** Port A. */
    gpioPortA = 0,
#endif
#if (_GPIO_PORT_B_PIN_COUNT > 0)
    /** Port B. */
        gpioPortB = 1,
#endif
#if (_GPIO_PORT_C_PIN_COUNT > 0)
    /** Port C. */
        gpioPortC = 2,
#endif
#if (_GPIO_PORT_D_PIN_COUNT > 0)
    /** Port D. */
        gpioPortD = 3,
#endif
#if (_GPIO_PORT_E_PIN_COUNT > 0)
    /** Port E. */
        gpioPortE = 4,
#endif
#if (_GPIO_PORT_F_PIN_COUNT > 0)
    /** Port F. */
        gpioPortF = 5,
#endif
#if (_GPIO_PORT_G_PIN_COUNT > 0)
    /** Port G. */
        gpioPortG = 6,
#endif
#if (_GPIO_PORT_H_PIN_COUNT > 0)
    /** Port H. */
        gpioPortH = 7,
#endif
#if (_GPIO_PORT_I_PIN_COUNT > 0)
    /** Port I. */
        gpioPortI = 8,
#endif
#if (_GPIO_PORT_J_PIN_COUNT > 0)
    /** Port J. */
        gpioPortJ = 9,
#endif
#if (_GPIO_PORT_K_PIN_COUNT > 0)
    /** Port K. */
        gpioPortK = 10,
#endif
};

    ////
    // Defines needed to get GPIO_Mode_TypeDef correctly defined
    ////

#define SL_ENUM_GENERIC(name, type) \
    typedef type name;              \
    enum name##_enum

#define _GPIO_P_MODEL_MODE0_DEFAULT 0x00000000UL              /**< Mode DEFAULT for GPIO_P_MODEL               */
#define _GPIO_P_MODEL_MODE0_DISABLED 0x00000000UL             /**< Mode DISABLED for GPIO_P_MODEL              */
#define _GPIO_P_MODEL_MODE0_INPUT 0x00000001UL                /**< Mode INPUT for GPIO_P_MODEL                 */
#define _GPIO_P_MODEL_MODE0_INPUTPULL 0x00000002UL            /**< Mode INPUTPULL for GPIO_P_MODEL             */
#define _GPIO_P_MODEL_MODE0_INPUTPULLFILTER 0x00000003UL      /**< Mode INPUTPULLFILTER for GPIO_P_MODEL       */
#define _GPIO_P_MODEL_MODE0_PUSHPULL 0x00000004UL             /**< Mode PUSHPULL for GPIO_P_MODEL              */
#define _GPIO_P_MODEL_MODE0_PUSHPULLALT 0x00000005UL          /**< Mode PUSHPULLALT for GPIO_P_MODEL           */
#define _GPIO_P_MODEL_MODE0_WIREDOR 0x00000006UL              /**< Mode WIREDOR for GPIO_P_MODEL               */
#define _GPIO_P_MODEL_MODE0_WIREDORPULLDOWN 0x00000007UL      /**< Mode WIREDORPULLDOWN for GPIO_P_MODEL       */
#define _GPIO_P_MODEL_MODE0_WIREDAND 0x00000008UL             /**< Mode WIREDAND for GPIO_P_MODEL              */
#define _GPIO_P_MODEL_MODE0_WIREDANDFILTER 0x00000009UL       /**< Mode WIREDANDFILTER for GPIO_P_MODEL        */
#define _GPIO_P_MODEL_MODE0_WIREDANDPULLUP 0x0000000AUL       /**< Mode WIREDANDPULLUP for GPIO_P_MODEL        */
#define _GPIO_P_MODEL_MODE0_WIREDANDPULLUPFILTER 0x0000000BUL /**< Mode WIREDANDPULLUPFILTER for GPIO_P_MODEL  */
#define _GPIO_P_MODEL_MODE0_WIREDANDALT 0x0000000CUL          /**< Mode WIREDANDALT for GPIO_P_MODEL           */
#define _GPIO_P_MODEL_MODE0_WIREDANDALTFILTER 0x0000000DUL    /**< Mode WIREDANDALTFILTER for GPIO_P_MODEL     */
#define _GPIO_P_MODEL_MODE0_WIREDANDALTPULLUP 0x0000000EUL    /**< Mode WIREDANDALTPULLUP for GPIO_P_MODEL     */
#define _GPIO_P_MODEL_MODE0_WIREDANDALTPULLUPFILTER 0x0000000FUL

SL_ENUM_GENERIC(GPIO_Mode_TypeDef, uint32_t) {
    /** Input disabled. Pull-up if DOUT is set. */
    gpioModeDisabled = _GPIO_P_MODEL_MODE0_DISABLED,
    /** Input enabled. Filter if DOUT is set. */
        gpioModeInput = _GPIO_P_MODEL_MODE0_INPUT,
    /** Input enabled. DOUT determines pull direction. */
        gpioModeInputPull = _GPIO_P_MODEL_MODE0_INPUTPULL,
    /** Input enabled with filter. DOUT determines pull direction. */
        gpioModeInputPullFilter = _GPIO_P_MODEL_MODE0_INPUTPULLFILTER,
    /** Push-pull output. */
        gpioModePushPull = _GPIO_P_MODEL_MODE0_PUSHPULL,
#if defined(_GPIO_P_MODEL_MODE0_PUSHPULLDRIVE)
    /** Push-pull output with drive-strength set by DRIVEMODE. */
        gpioModePushPullDrive = _GPIO_P_MODEL_MODE0_PUSHPULLDRIVE,
#endif
#if defined(_GPIO_P_MODEL_MODE0_PUSHPULLALT)
    /** Push-pull using alternate control. */
        gpioModePushPullAlternate = _GPIO_P_MODEL_MODE0_PUSHPULLALT,
#endif
    /** Wired-or output. */
        gpioModeWiredOr = _GPIO_P_MODEL_MODE0_WIREDOR,
    /** Wired-or output with pull-down. */
        gpioModeWiredOrPullDown = _GPIO_P_MODEL_MODE0_WIREDORPULLDOWN,
    /** Open-drain output. */
        gpioModeWiredAnd = _GPIO_P_MODEL_MODE0_WIREDAND,
    /** Open-drain output with filter. */
        gpioModeWiredAndFilter = _GPIO_P_MODEL_MODE0_WIREDANDFILTER,
    /** Open-drain output with pull-up. */
        gpioModeWiredAndPullUp = _GPIO_P_MODEL_MODE0_WIREDANDPULLUP,
    /** Open-drain output with filter and pull-up. */
        gpioModeWiredAndPullUpFilter = _GPIO_P_MODEL_MODE0_WIREDANDPULLUPFILTER,
#if defined(_GPIO_P_MODEL_MODE0_WIREDANDDRIVE)
    /** Open-drain output with drive-strength set by DRIVEMODE. */
        gpioModeWiredAndDrive = _GPIO_P_MODEL_MODE0_WIREDANDDRIVE,
    /** Open-drain output with filter and drive-strength set by DRIVEMODE. */
        gpioModeWiredAndDriveFilter = _GPIO_P_MODEL_MODE0_WIREDANDDRIVEFILTER,
    /** Open-drain output with pull-up and drive-strength set by DRIVEMODE. */
        gpioModeWiredAndDrivePullUp = _GPIO_P_MODEL_MODE0_WIREDANDDRIVEPULLUP,
    /** Open-drain output with filter, pull-up and drive-strength set by DRIVEMODE. */
        gpioModeWiredAndDrivePullUpFilter = _GPIO_P_MODEL_MODE0_WIREDANDDRIVEPULLUPFILTER
#endif
#if defined(_GPIO_P_MODEL_MODE0_WIREDANDALT)
            /** Open-drain output using alternate control. */
            gpioModeWiredAndAlternate = _GPIO_P_MODEL_MODE0_WIREDANDALT,
    /** Open-drain output using alternate control with filter. */
        gpioModeWiredAndAlternateFilter = _GPIO_P_MODEL_MODE0_WIREDANDALTFILTER,
    /** Open-drain output using alternate control with pull-up. */
        gpioModeWiredAndAlternatePullUp = _GPIO_P_MODEL_MODE0_WIREDANDALTPULLUP,
    /** Open-drain output using alternate control with filter and pull-up. */
        gpioModeWiredAndAlternatePullUpFilter = _GPIO_P_MODEL_MODE0_WIREDANDALTPULLUPFILTER,
#endif
};

////
// Defines needed for configuring the CC pins of a timer
////
#define _GPIO_TIMER_CC0ROUTE_PORT_SHIFT 0  /**< Shift value for GPIO_PORT                   */
#define _GPIO_TIMER_CC1ROUTE_PORT_SHIFT 0  /**< Shift value for GPIO_PORT                   */
#define _GPIO_TIMER_CC2ROUTE_PORT_SHIFT 0  /**< Shift value for GPIO_PORT                   */
#define _GPIO_TIMER_CC0ROUTE_PIN_SHIFT 16  /**< Shift value for GPIO_PIN                    */
#define _GPIO_TIMER_CC1ROUTE_PIN_SHIFT 16  /**< Shift value for GPIO_PIN                    */
#define _GPIO_TIMER_CC2ROUTE_PIN_SHIFT 16  /**< Shift value for GPIO_PIN                    */
#define _GPIO_TIMER_ROUTEEN_CC0PEN_SHIFT 0 /**< Shift value for GPIO_CC0PEN                 */

////
// Declaration of the functions needed. This is a list of the em_gpio module functions that are in
// use in the code. As more functions are added, we can get them here so cmock can mock them.
////

void GPIO_PinModeSet(GPIO_Port_TypeDef port, unsigned int pin, GPIO_Mode_TypeDef mode, unsigned int out);
unsigned int GPIO_PinInGet(GPIO_Port_TypeDef port, unsigned int pin);
void GPIO_ExtIntConfig(GPIO_Port_TypeDef port, unsigned int pin, unsigned int intNo, bool risingEdge, bool fallingEdge, bool enable);
uint32_t GPIO_EnabledIntGet(void);
void GPIO_IntEnable(uint32_t flags);

#endif  // EM_GPIO_H_