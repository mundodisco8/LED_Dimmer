/**
 * @file timer_HW.h
 * @author Joel Santos (jsantosrico@gmail.com)
 * @brief Public API for the TIMER0 Hardware Abstraction Layer.
 * @version 0.1
 * @date 2026-03-13
 * * @copyright Copyright (c) 2026
 * * @defgroup TIMER_HW Timer Hardware Module
 * @{
 *
 * ### Detailed Description
 * This module provides a wrapper for the `em_timer` library, specifically optimized
 * to use **TIMER0** as a high-speed PWM generator. It abstracts register access
 * for frequency calculation, duty cycle updates, and pin routing.
 *
 * ### Key Features
 * * **PWM Configuration:** Specialized setup for Up/Down counting mode to generate
 * center-aligned PWM signals.
 * * **Buffered Writes:** Supports buffered updates to TOP and Compare registers
 * to ensure glitch-free transitions at the start of new cycles.
 * * **Clock Management:** Provides safe initialization to prevent hardware
 * HardFaults during register access.
 *
 * ### Critical Safety Note
 * * **Module Enable Requirement:** Functions that write to **SYNC** type registers
 * (like @ref TIMHW_setTimer0TopValue or @ref TIMHW_setT0ChannelOutputCompare)
 * require the TIMER0 module to be **ENABLED**. Writing while disabled will trigger
 * a @ref TIMER_DISBLED_BEFORE_WRITING_SYNC error.
 *
 * ### Initialization Sequence
 * 1. Enable hardware clock: @ref TIMHW_initTimer0Clock.
 * 2. Configure basic timer parameters: @ref TIMHW_initTimer0.
 * 3. Configure Capture/Compare channels: @ref TIMHW_configCCChannelPWM.
 * 4. Enable overflow interrupts if required: @ref TIMHW_enableTimer0OverflowInt.
 */
#ifndef _TIMER_HW_H
#define _TIMER_HW_H

#include <inttypes.h>
#include <stdbool.h>

#include "gpio_HW_types.h"
#include "timer_HW_types.h"

/**
 * @brief Starts TIMER0
 */
void TIMHW_startTimer0(void);
/**
 * @brief Stops TIMER0
 */
void TIMHW_stopTimer0(void);

/**
 * @brief Initalises the HW clock of TIMER0
 * NOTE: If a module clock is disabled, the registers of that module are not accessible and accessing such registers
 * will hardfault the Cortex core.
 */
void TIMHW_initTimer0Clock(void);
/**
 * @brief Initialises and starts TIMER0
 * Sets all the compare values to 0, so all the PWM signals start with 0% duty cycle.
 */
void TIMHW_initTimer0(bool startTimerOnInit);

/**
 * @brief Sets a pin as output and associates a GPIO pin with one of the Compare/Capture channels of TIMER0
 */
timerStatus_t TIMHW_setCCChannelPin(pinPort_t pinPort, uint8_t pinNo, CCChannel_t channel);
/**
 * @brief Configures the Capture/Compare module in PWM mode for one of the CC channels of TIMER0
 * Asserts if channel is invalid (when calling TIMER_InitCC())
 */
void TIMHW_configCCChannelPWM(CCChannel_t channel, polarity_t polarity);
/**
 * @brief Enables the Compare/Capture Module for the selected channel on TIMER0
 */
void TIMHW_enableChannelCompCapUnit(CCChannel_t channel);
/**
 * @brief Disables the Compare/Capture Module for the selected channel on TIMER0
 */
void TIMHW_disableChannelCompCapUnit(CCChannel_t channel);

/**
 * @brief Sets the value of the Compare Register (unbuffered)
 * NOTE: WRITES TO A SYNC type register, requires TIMER0 module to be ENABLED
 */
timerStatus_t TIMHW_setT0ChannelOutputCompare(CCChannel_t channel, uint32_t compareValue);
/**
 * @brief Sets the value of the buffered Compare Register (buffered). The change will be effective on the
 * next compare match.
 * NOTE: WRITES TO A SYNC type register, requires TIMER0 module to be ENABLED
 */
timerStatus_t TIMHW_setT0ChannelOutputCompareBuffered(CCChannel_t channel, uint32_t compareValue);
/**
 * @brief gets the Compare / Capture value (NOTE: ACTUAL VALUE, NOT THE BUFFER)
 */
uint32_t TIMHW_getTimer0CompareValue(const CCChannel_t channel);
/**
 * @brief Set the value of the TOP register on TIMER0
 * NOTE: WRITES TO A SYNC type register, requires TIMER0 module to be ENABLED
 */
timerStatus_t TIMHW_setTimer0TopValue(const uint32_t top);
/**
 * @brief Set the value of the TOP register on TIMER0
 * NOTE: WRITES TO A SYNC type register, requires TIMER0 module to be ENABLED
 */
timerStatus_t TIMHW_setTimer0TopValueBuffered(const uint32_t top);
/**
 * @brief gets the TOP value (NOTE: ACTUAL VALUE, NOT THE BUFFER)
 */
uint32_t TIMHW_getTimer0TopValue(void);
/**
 * @brief Returns the clock frequency of TIMER0 taking the prescaler into account
 */
uint32_t TIMHW_getTimer0Frequency(void);
#endif  // _TIMER_HW_H
