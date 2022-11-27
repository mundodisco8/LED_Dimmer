#include "gpio_HW.h"

#include "em_gpio.h"
#include "em_cmu.h"

// NOTE: If a module clock is disabled, the registers of that module are not accessible and accessing such registers
// will hardfault the Cortex core.
void enableGPIOClock(void) {
    CMU_ClockEnable(cmuClock_GPIO, true);
}

// Wrapper for GPIO_PinModeSet
void setPinMode(pinPort_t port, uint32_t pin, pinMode_t mode, bool dout) {
    GPIO_PinModeSet((GPIO_Port_TypeDef)port, pin, mode, dout);
}


// Wrapper for GPIO_PinInGet()
// Reads the state of an input pin
// Parameters: port: the port of the pin to read
//             pinNo: the number of pin to read
// Returns: 0 if the pin is low, 1 if the pin is HIGH.
uint32_t readPin(pinPort_t port, uint32_t pinNo) {
    return GPIO_PinInGet((GPIO_Port_TypeDef)port, pinNo);
}

// Wrapper for GPIO_ExtIntConfig()
// Configures a GPIO interrupt
// Parameters: port: the port of the pin to read
//             pinNo: the number of pin to read
//             intNo: the number of interrupt to register
//             risingEdge: enable interrupt in Rising Edge
//             fallingEdge: enable interrupt in falling Edge
//             enable: true to enable the interrupt or false to leave it disabled.
void configurePinInterrupt(pinPort_t port, uint32_t pinNo, uint32_t intNo, bool risingEdge, bool fallingEdge,
                        bool enable) {
    GPIO_ExtIntConfig((GPIO_Port_TypeDef)port, pinNo, intNo, risingEdge, fallingEdge, enable);
}

// Adds the pin interrupts to the enabled interrupts register.
// It ORs the value passed to the currently enabled ones, so it can set interrupts, but it can't
// clear existing ones.
// Parameters: pinsToEnable: flags of the pins to enable interrupts.
// Returns: nothing
void enablePinInterrupts(uint32_t pinsToEnable) {
    uint32_t intEnabled = GPIO_EnabledIntGet();
    intEnabled |= pinsToEnable;
    GPIO_IntEnable(pinsToEnable);
}