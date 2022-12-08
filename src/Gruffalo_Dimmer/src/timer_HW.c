////
// timer_HW
//
// Wrapper for calls to the em_timer library to use TIMER0 as a PWM generator.
//
// This is not a generalisation of em_timer. For now, it just assumes that we are going to use TIMER0. If we need to
// extend it, it wouldn't be much more work. It is also designed to set TIMER0 in PWM mode.
//
////

#include "timer_HW.h"

#include <stddef.h>

#include "em_cmu.h"
#include "em_timer.h"
#include "em_gpio.h"

#include "app_log.h"

#include "gpio_HW.h"

////
// Set Register Values
////

// Mark as unused. I don't want to delete them in case they are useful at some point later.
static void TIMHW_timerModuleEnable(void) __attribute__ ((unused));
static void TIMHW_timerModuleDisable(void)__attribute__ ((unused));

// GCOVR_EXCL_START
static void TIMHW_timerModuleEnable(void) {
    TIMER0->EN_SET = TIMER_EN_EN;
}

static void TIMHW_timerModuleDisable(void) {
    TIMER0->EN_CLR = TIMER_EN_EN;
}
// GCOVR_EXCL_STOP

// Sets the value of the Compare Register (unbuffered)
// NOTE: WRITES TO A SYNC type register, requires TIMER0 module to be ENABLED
// Parameters: channel, the channel to set
//             compareValue: the new value of the compare Register
// Returns: TIMER_OK on success, TIMER_DISABLED_BEFORE_WRITING_SYNC if the timer0 module was disabled
//          before calling this function
timerStatus_t TIMHW_setChannelOutputCompare(CCChannel_t channel, uint32_t compareValue) {
    // TIMER_CompareSet writes to TIMER_CC0_OC -> RWH Sync -> module needs to be enabled
    if (TIMER0->EN == 0) { // Timer module is disabled!
        app_log_error("Enable TIMER0 module before calling setChannelOutputCompare()!\r\n");
        return TIMER_DISBLED_BEFORE_WRITING_SYNC;
    } // else, timer module is enabled, we can proceed
    TIMER_CompareSet(TIMER0, channel, compareValue); // GCOV_EXCL_LINE
    return TIMER_OK;
}

// Sets the value of the buffered Compare Register (buffered). The change will be effective on the
// next compare match.
// NOTE: WRITES TO A SYNC type register, requires TIMER0 module to be ENABLED
// Parameters: channel, the channel to set
//             compareValue: the new value of the compare Register
// Returns: TIMER_OK on success, TIMER_DISABLED_BEFORE_WRITING_SYNC if the timer0 module was disabled
timerStatus_t TIMHW_setChannelBufferedOutputCompare(CCChannel_t channel, uint32_t compareValue) {
    // TIMER_CompareSet writes to TIMER_CC0_OCB -> RWH Sync -> module needs to be enabled
    if (TIMER0->EN == 0) { // Timer module is disabled!
        app_log_error("Enable TIMER0 module before calling setChannelBufferedOutputCompare()!\r\n");
        return TIMER_DISBLED_BEFORE_WRITING_SYNC;
    } // else, timer module is enabled, we can proceed
    TIMER_CompareBufSet(TIMER0, channel, compareValue);
    return TIMER_OK;
}

// Set the value of the TOP register on TIMER0
// NOTE: WRITES TO A SYNC type register, requires TIMER0 module to be ENABLED
// Parameters: top: the value to set as the top value for the count on TIMER0
//          before calling this function
timerStatus_t TIMHW_setTimer0TopValue(uint32_t top) {
    // TIMER_CompareSet writes to TIMER_TOP -> RWH Sync -> module needs to be enabled
    if (TIMER0->EN == 0) { // Timer module is disabled!
        app_log_error("Enable TIMER0 module before calling setTimer0TopValue()!\r\n");
        return TIMER_DISBLED_BEFORE_WRITING_SYNC;
    } // else, timer module is enabled, we can proceed
    TIMER_TopSet(TIMER0, top);
    return TIMER_OK;
}

// Gets the value of the TOP register on TIMER0
// Returns: the value of the TOP register in TIMER0
uint32_t TIMHW_getTimer0TopValue(void) {
    return TIMER_TopGet(TIMER0);
}

// Returns the clock frequency of TIMER0
// Returns: the frequency of TIMER0 in Hz
uint32_t TIMHW_getTimerFrequency(void) {
    return CMU_ClockFreqGet(cmuClock_TIMER0);
}

////
// Init functions
////

// Initalises the HW clock of TIMER0
// NOTE: If a module clock is disabled, the registers of that module are not accessible and accessing such registers
// will hardfault the Cortex core.
void TIMHW_initTimer0Clock(void) {
    CMU_ClockEnable(cmuClock_TIMER0, true);
}

// Sets a pin as output and associates a GPIO pin with one of the Compare/Capture channels of TIMER0
// Parameters: pinPort: the pin port
//             pinNo: the pin number
//             channel: the channel to associate the pin with
timerStatus_t TIMHW_setCCChannelPin(pinPort_t pinPort, uint8_t pinNo, CCChannel_t channel) {
    volatile uint32_t* routeRegister = NULL;
    uint32_t portShift               = 0;
    uint8_t pinShift                = 0;

    // Associate the pin with the correct channel's CC pin
    // Select the right register and bit displacements for that register
    switch (channel) {
        case CC_CHANNEL_0: {
            routeRegister = &GPIO->TIMERROUTE[TIMER_NUM(TIMER0)].CC0ROUTE;
            portShift     = _GPIO_TIMER_CC0ROUTE_PORT_SHIFT;
            pinShift      = _GPIO_TIMER_CC0ROUTE_PIN_SHIFT;
            break;
        }
        case CC_CHANNEL_1: {
            routeRegister = &GPIO->TIMERROUTE[TIMER_NUM(TIMER0)].CC1ROUTE;
            portShift     = _GPIO_TIMER_CC1ROUTE_PORT_SHIFT;
            pinShift      = _GPIO_TIMER_CC1ROUTE_PIN_SHIFT;
            break;
        }
        case CC_CHANNEL_2: {
            routeRegister = &GPIO->TIMERROUTE[TIMER_NUM(TIMER0)].CC2ROUTE;
            portShift     = _GPIO_TIMER_CC2ROUTE_PORT_SHIFT;
            pinShift      = _GPIO_TIMER_CC2ROUTE_PIN_SHIFT;
            break;
        }
        default: {
            app_log_error("Incorrect Channel Selected\r\n");
            return TIMER_WRONG_CC_CHANNEL;
            break;
        }
    }
    // Set PWM pin as output starting low.
    setPinMode(pinPort, pinNo, MODE_PUSHPULL, 0);
    // Set the pin and port for this CC channel
    *routeRegister = (uint32_t)(pinPort << portShift) | (pinNo << pinShift);
    return TIMER_OK;
}

// Configures the Capture/Compare module in PWM mode for one of the CC channels of TIMER0
//  Parameters: channel: the channel to set in PWM mode
//              polarity: a polarity_t data type with the required polarity of the PWM signal
//  Asserts if channel is invalid (when calling TIMER_InitCC())
void TIMHW_configCCChannelPWM(CCChannel_t channel, polarity_t polarity) {
    // Set CC channel parameters. Based on TIMER_INITCC_DEFAULT, but mode is PWM, match action set to toggle, detection
    // edges set to both and polarity defined by parameter.
    TIMER_InitCC_TypeDef channel_init = {
        /** Input capture event control. */
        .eventCtrl = timerEventEveryEdge,  // Event on every capture.
        /** Input capture edge select. */
        .edge = timerEdgeBoth,  // Input capture edge on both edges.
        /** Peripheral reflex system trigger selection. Only applicable if prsInput is enabled.*/
        .prsSel = 0,  // Not used by default, select PRS channel 0.
        /** Counter underflow output action. */
        .cufoa = timerOutputActionNone,  // No action on underflow.
        /** Counter overflow output action. */
        .cofoa = timerOutputActionNone,  // No action on overflow.
        /** Counter match output action. */
        .cmoa = timerOutputActionToggle,  // Toggle on match
        /** Compare/capture channel mode. */
        .mode = timerCCModePWM,  // Mode PWM
        /** Enable digital filter. */
        .filter = false,  // Disable filter.
        /** Select TIMERnCCx (false) or PRS input (true). */
        .prsInput = false, /* No PRS input. */
        /**
         * Compare output initial state. Only used in Output Compare and PWM mode.
         * When true, the compare/PWM output is set high when the counter is
         * disabled. When counting resumes, this value will represent the initial
         * value for the compare/PWM output. If the bit is cleared, the output
         * will be cleared when the counter is disabled.
         */
        .coist = false,  // Clear output when counter disabled.
        /** Invert output from compare/capture channel. */
        .outInvert = (polarity == PWM_ACTIVE_LOW),  // Set polarity based on parameter
        /** PRS output configuration. PRS output from a timer can either be a pulse output or a level output that
         * follows the CC out value.*/
        .prsOutput = timerPrsOutputDefault,  // Use default PRS output configuration.
        /** When PRS input is used this field is used to configure the type of
         * PRS input. */
        .prsInputType = timerPrsInputNone  // No PRS input, so input type is none.
    };

    // TIMER_InitCC
    // 1) Disables module
    // 2) Writes to TIMER_CCx_CFG -> CONFIG type needs to be disabled
    // 3) Reenables the module
    // 4) Writes to TIMER_CCx_CTRL -> SYNC type, needs to be enabled
    // 5) leaves module enabled
    TIMER_InitCC(TIMER0, channel, &channel_init);
    // Set output compare values for all channels to 0
    // NOTE: no need to check the return value, because TIMHW_setChannelOutputCompare() can only fail if run when the
    // Timer is disabled, but TIMER_InitCC() leaves it enabled.
    TIMHW_setChannelOutputCompare(CC_CHANNEL_0, 0);
    TIMHW_setChannelOutputCompare(CC_CHANNEL_1, 0);
    TIMHW_setChannelOutputCompare(CC_CHANNEL_2, 0);
}

// Initialises and starts TIMER0
// Sets all the compare values to 0, so all the PWM signals start with 0% duty cycle.
void TIMHW_startTimer0(void) {
    // Initialize TIMER with TIMER_INIT_DEFAULT values
    TIMER_Init_TypeDef timer_init = {
        /** Start counting when initialization completed. */
        .enable = true, /* Enable timer when initialization completes. */
        /** Counter shall keep running during debug halt. */
        .debugRun = false, /* Stop counter during debug halt. */
        /** Prescaling factor, if HFPER / HFPERB clock used. */
        .prescale = timerPrescale1, /* No prescaling. */
        /** Clock selection. */
        .clkSel = timerClkSelHFPerClk, /* Select HFPER / HFPERB clock. */
        /** 2x Count mode, counter increments/decrements by 2, meant for PWM mode. */
        .count2x = false, /* Not 2x count mode. */
        /** ATI (Always Track Inputs) makes CCPOL always track the polarity of the inputs. */
        .ati = false, /* No ATI. */
        /** Action on falling input edge. */
        .fallAction = timerInputActionNone, /* No action on falling input edge. */
        /** Action on rising input edge. */
        .riseAction = timerInputActionNone, /* No action on rising input edge. */
        /** Counting mode. */
        .mode = timerModeUp, /* Up-counting. */
        /** DMA request clear on active. */
        .dmaClrAct = false, /* Do not clear DMA requests when DMA channel is active. */
        /** Select X2 or X4 quadrature decode mode (if used). */
        .quadModeX4 = false, /* Select X2 quadrature decode mode (if used). */
        /** Determines if only counting up or down once. */
        .oneShot = false, /* Disable one shot. */
        /** Timer start/stop/reload by other timers. */
        .sync = false /* Not started/stopped/reloaded by other timers. */
    };


    // NOTE: for joel, regarding writing to CONFIG and SYNC registers (see 19.3.1)
    // TIMER_Init() writes to these regs in this order. Because it enables and disables accordingly
    // it doesn't matter the initial state.
    // First disable the timer
    // 1) timer->EN_CLR // type RW enable: can be written at any time
    // Write CFG and reenable
    // 2) timer->CFG    // type RW Config -> Write only when disabled
    // 3) timer->EN_SET // type RW enable: can be written at any time
    // Finish setting up
    // 4) timer->CMD   // W Sync -> write only when enabled
    // 5) timer->CTRL  // RW Sync -> write or read only when enabled
    // 6) timer->CNT   // RW and H (updated by HW) SYNC -> RW only when enabled
    TIMER_Init(TIMER0, &timer_init);
}

// Enables the pin in the required channel, starting the PWM signal
// Parameters: channel: the channel to set in PWM mode
void TIMHW_startChannelPWM(CCChannel_t channel) {
    // We don't need to read the current value of the register and OR it with the new value because we are not writing
    // to the register itself, but to the _SET version of it.
    GPIO->TIMERROUTE_SET[TIMER_NUM(TIMER0)].ROUTEEN = 1 << (channel + _GPIO_TIMER_ROUTEEN_CC0PEN_SHIFT);
}

// Disables the pin in the required channel, stoping the PWM signal
// Parameters: channel: the channel to set in PWM mode
void TIMHW_stopChannelPWM(CCChannel_t channel) {
    // We don't need to read the current value of the register and OR it with the new value because we are not writing
    // to the register itself, but to the _CLR version of it.
    GPIO->TIMERROUTE_CLR[TIMER_NUM(TIMER0)].ROUTEEN = 1 << (channel + _GPIO_TIMER_ROUTEEN_CC0PEN_SHIFT);
}
