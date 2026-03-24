#include "LEDIndicator.h"

// SiLabs headers
#include "pin_config.h"

#ifdef TEST
#define STATIC
#else  // TEST
#define STATIC static
#endif  // TEST

/**
 * @brief Instance of the LED indicator. Pins are set according to "pin_config.h"
 */
STATIC LEDIndicator_t indicator = {.ch1_pin  = LED0_PIN,
                                   .ch1_port = MY_PORT(LED0_PORT),
                                   .ch2_pin  = LED1_PIN,
                                   .ch2_port = MY_PORT(LED1_PORT),
                                   .ch3_pin  = LED2_PIN,
                                   .ch3_port = MY_PORT(LED2_PORT)};

/**
 * @brief Initialises the LED indicator gpio pins in output mode, pins cleared
 */
void LEDIndicator_init(void) {
    setPinMode(indicator.ch1_port, indicator.ch1_pin, MODE_PUSHPULL, PIN_CLEAR);
    setPinMode(indicator.ch2_port, indicator.ch2_pin, MODE_PUSHPULL, PIN_CLEAR);
    setPinMode(indicator.ch3_port, indicator.ch3_pin, MODE_PUSHPULL, PIN_CLEAR);
}

/**
 * @brief Sets the indication of the desired channels on the LED indicator
 *
 * @param newChannel the new desired channel to indicate. Accepts ORing for crazy combinations if needed.
 */
void LEDIndicator_setChannel(channel_t newChannel) {
    if (CHANNEL_1 == (newChannel & CHANNEL_1)) {
        writePin(indicator.ch1_port, PIN_SET, indicator.ch1_pin);
    } else {
        writePin(indicator.ch1_port, PIN_CLEAR, indicator.ch1_pin);
    }

    if (CHANNEL_2 == (newChannel & CHANNEL_2)) {
        writePin(indicator.ch2_port, PIN_SET, indicator.ch2_pin);
    } else {
        writePin(indicator.ch2_port, PIN_CLEAR, indicator.ch2_pin);
    }

    if (CHANNEL_3 == (newChannel & CHANNEL_3)) {
        writePin(indicator.ch3_port, PIN_SET, indicator.ch3_pin);
    } else {
        writePin(indicator.ch3_port, PIN_CLEAR, indicator.ch3_pin);
    }
}