#include "gpio_HW.h"

// Silabs SDK headers
#include "app_assert.h"
// Ignore a cast-align warning in some cmsis header
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-align"
#include "em_gpio.h"
#pragma GCC diagnostic pop
#include "em_cmu.h"

#ifdef TEST
#define STATIC
#else  // TEST
#define STATIC static
#endif  // TEST

////
// Getters to translate from our enums to the SiLabs's ones
////

/**
 * @brief Translates from our port enum to the Silabs's one
 * @param port the gpio port as a pinport_t
 * @return the equivalent GPIO_Port_TypeDef pin port
 */
STATIC GPIO_Port_TypeDef getSiLabsPort(pinPort_t port) {
    switch (port) {
        case portA: {
            return gpioPortA;
        }
        case portB: {
            return gpioPortB;
        }
        case portC: {
            return gpioPortC;
        }
        case portD: {
            return gpioPortD;
        }
        case portE: {
            app_assert(false, "Port E doesn't have pins in the BGM220P");
        }
        case portF: {
            app_assert(false, "Port F doesn't have pins in the BGM220P");
        }
        default: {
            break;  // do nothing, fall through and assert
        }
    }
    app_assert(false, "Unknown port");
    return 0;  // wil never return 0, it should assert
}

/**
 * @brief Translates from our pin mode to the Silabs's one
 * @param mode the pin mode as a pinMode_t
 * @return the equivalent GPIO_Mode_TypeDef pin mode
 */
STATIC GPIO_Mode_TypeDef getSiLabsMode(pinMode_t mode) {
    // NOTE: I'll only do this for now, but add more if needed!
    switch (mode) {
        case MODE_DISABLED: {
            return gpioModeDisabled;
        }
        case MODE_INPUT: {
            return gpioModeInput;
        }
        case MODE_INPUT_PULL: {
            return gpioModeInputPull;
        }
        case MODE_INPUT_PULL_FILTER: {
            return gpioModeInputPullFilter;
        }
        case MODE_PUSHPULL: {
            return gpioModePushPull;
        }
        default: {
            break;
        }
    }
    app_assert(false, "Unknown pin mode");
    return gpioModeDisabled;  // should never return this, will assert first
}

////
// Wrapper Functions
////

/**
 * @brief Enables the GPIO clock
 * NOTE: If a module clock is disabled, the registers of that module are not accessible and accessing such registers
 * will hardfault the Cortex core.
 *
 */
void enableGPIOClock(void) { CMU_ClockEnable(cmuClock_GPIO, true); }

/**
 * @brief Set a pin Up (Wrapper for GPIO_PinModeSet)
 * @param port the pin port
 * @param pin the pin number
 * @param mode the pin mde
 * @param dout the value for the Out register
 */
void setPinMode(pinPort_t port, uint8_t pin, pinMode_t mode, bool dout) {
    GPIO_PinModeSet(getSiLabsPort(port), pin, getSiLabsMode(mode), dout);
}

/**
 * @brief Reads an input pin's value
 * @param port the pin port
 * @param pin the pin number
 * @return uint32_t the current state of the pin, 0 if clear and 1 if set
 */
uint32_t readPin(pinPort_t port, uint8_t pinNo) { return GPIO_PinInGet(getSiLabsPort(port), pinNo); }

/**
 * @brief Configures a GPIO interrupt
 * @param port the port of the pin to read
 * @param pinNo the number of pin to read
 * @param intNo the number of interrupt to register
 * @param risingEdge enable interrupt in Rising Edge
 * @param fallingEdge enable interrupt in falling Edge
 * @param enable true to enable the interrupt or false to leave it disabled.
 */
void configurePinInterrupt(pinPort_t port, uint8_t pinNo, uint32_t intNo, bool risingEdge, bool fallingEdge,
                           bool enable) {
    GPIO_ExtIntConfig(getSiLabsPort(port), pinNo, intNo, risingEdge, fallingEdge, enable);
}

/**
 * @brief Adds the pin interrupts to the enabled interrupts register.
 * It ORs the value passed to the currently enabled ones, so it can set interrupts, but it can't
 * clear existing ones.
 * @param pinsToEnable flags of the pins to enable interrupts
 */
void enablePinInterrupts(uint32_t pinsToEnable) {
    uint32_t intEnabled = GPIO_EnabledIntGet();
    pinsToEnable |= intEnabled;
    GPIO_IntEnable(pinsToEnable);
}

/**
 * @brief Set the Pin Up For EM4 Wake Up
 * @param port the port of the pin to read
 * @param pinNo the number of pin to read
 */
void setPinUpForEM4WakeUp(pinPort_t port, uint8_t pin) {
    // Check if the pin can wake up on EM4
    uint32_t EM4WakeUpPinMask = 0xFF;  // invalid value

    if ((portA == port) && (5 == pin)) {
        EM4WakeUpPinMask = GPIO_IEN_EM4WUIEN0;
    } else if ((portB == port) && (1 == pin)) {
        EM4WakeUpPinMask = GPIO_IEN_EM4WUIEN3;
    } else if ((portB == port) && (3 == pin)) {
        EM4WakeUpPinMask = GPIO_IEN_EM4WUIEN4;
    } else if ((portC == port) && (0 == pin)) {
        EM4WakeUpPinMask = GPIO_IEN_EM4WUIEN6;
    } else if ((portC == port) && (5 == pin)) {
        EM4WakeUpPinMask = GPIO_IEN_EM4WUIEN7;
    } else if ((portC == port) && (7 == pin)) {
        EM4WakeUpPinMask = GPIO_IEN_EM4WUIEN8;
    } else if ((portD == port) && (2 == pin)) {
        EM4WakeUpPinMask = GPIO_IEN_EM4WUIEN9;
    }  // else leave as 0xFF

    if (0xFF == EM4WakeUpPinMask) {
        app_assert(false, "Pin %u in port %u is not an EM4 WakeUp pin", pin, port);
    }
    // The pin has to be set as input
    GPIO_PinModeSet(port, pin, MODE_INPUT_PULL_FILTER, true);
    // And then enable it as EM4 wakeup pin
    GPIO_EM4EnablePinWakeup(EM4WakeUpPinMask, 0);
}