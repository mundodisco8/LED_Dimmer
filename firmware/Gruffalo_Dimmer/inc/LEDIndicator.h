#ifndef _LEDINDICATOR_H_
#define _LEDINDICATOR_H_

// Our headers
#include "gpio_HW.h"

typedef enum channel {
    CHANNEL_NONE = 0x00,  // Clears all the LEDs of the indicator
    CHANNEL_1    = 0x01,  // Turns on channel 1 on the indicator
    CHANNEL_2    = 0x02,  // Turns on channel 2 on the indicator
    CHANNEL_3    = 0x04,  // Turns on channel 3 on the indicator
    CHANNEL_ALL  = 0x07,  // Turns on all the LEDS of the indicator
} channel_t;

typedef struct LEDIndicator {
    pinPort_t ch1_port;
    uint32_t ch1_pin;
    pinPort_t ch2_port;
    uint32_t ch2_pin;
    pinPort_t ch3_port;
    uint32_t ch3_pin;
} LEDIndicator_t;

/**
 * @brief Initialises the LED indicator gpio pins in output mode, pins cleared
 */
void LEDIndicator_init(void);
/**
 * @brief Sets the indication of the desired channels on the LED indicator
 */
void LEDIndicator_setChannel(channel_t newChannel);

#endif  // _LEDINDICATOR_H_