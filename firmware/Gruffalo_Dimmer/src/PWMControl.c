////
// PWMControl
//
// Sets TIMER0 in PWM mode and provides control for its three CC channels
//
// To initialise the module
// 1) Call initTimer0() to start TIMER0's HW
// 2) Configure the required CC Chanels with initTimer0CCChannel()
// 3) Set the required brightess with setBrightness(). To get a certain duty cycle, call setDutyCycle() instead
//
////

#include "PWMControl.h"

// C Standard libs
#include <math.h>

// SDK includes
// Ignore a cast-align warning in some cmsis header and a sign conversion in
// sl_sleeptimer.h
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-align"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include "app_log.h"
#pragma GCC diagnostic pop

// My includes
#include "brightnessLUT.h"
#include "timer_HW.h"

// Trickery to allow testing of static elements. Better to mess a bit with the code than to overcomplicate tests
#ifdef TEST
#define STATIC
#else
#define STATIC static
#endif

/**
 * An elegant macro implementation of min, as seen in https://stackoverflow.com/questions/3437404/min-and-max-in-c
 */
#define min(a, b)               \
    ({                          \
        __typeof__(a) _a = (a); \
        __typeof__(b) _b = (b); \
        _a < _b ? _a : _b;      \
    })

// Gamma correction
// To correct the intensity of the LEDs to match the perception of our eyes, we need to do some maths™
const float GAMMA_VALUE = 2.3;

// NOTE: unused, but will be needed if we use the LUT generation in the code
/**
 * @brief The gammaLookUp stores the gamma-corrected values for brightness. This way, brightness will be modeled in a
 * similar way to our eye and behave more linearly.
 * The table stores values from
 */
// #define GAMMA_LUT_SIZE 1UL                          // 101, so we have values for each percentile [0-100UL]
// STATIC uint32_t gammaLookUp[GAMMA_LUT_SIZE] = {0};  // size 101 to get from 0% to 100UL% -> 101 values

// PWM Parameters
// never set a PWM frequency that results in less than 12 bits of resolution
const uint32_t MIN_PWM_LEVELS = 4096UL;

// Never set a PWM frequency of less than 250Hz.
const uint32_t MIN_PWM_FREQ = 250UL;
// TODO: We can't get over 1KHz, as we are using ms to do some maths. We would have to change to μs to avoid this.
const uint32_t MAX_PWM_FREQ = 1000UL;

/**
 * @brief The frequency at which the PWM runs
 * NOTE: CHANGING THIS REQUIRES REBUILDING THE GAMMA-CORRECTED LUT!
 */
const uint32_t PWM_FREQUENCY = 1000UL;

// With a 32 bit register and a clock at 38400000 and up-down count
// Min PWM Freq -> 250Hz -> TOP is 76800
// Max PWM Freq -> TOP to 4096 -> 38400000 / (4096 * 2) = 4687.5Hz

typedef struct LEDChannel {
    uint8_t brightness;      // in % [0-100UL]
    uint32_t breathePeriod;  // Duration of a breathe effect period in ms
} LEDChannel_t;

typedef struct LEDArray {
    LEDChannel_t channel0;
    LEDChannel_t channel1;
    LEDChannel_t channel2;
} LEDArray_t;

////
// Forward Declarations
////

STATIC void configureTimerPWMFrequency(const uint32_t frequencyHz);

/**
 * @brief Start TIMER0's HW in PWM mode
 * @param PWMFreqHz the desired PWM frequency in Hz
 */
void initTimer0PWM(uint32_t PWMFreqHz) {
    TIMHW_initTimer0Clock();
    TIMHW_initTimer0(false);
    configureTimerPWMFrequency(PWMFreqHz);
}

/**
 * @brief Sets the PWM signal frequency by adjusting the TOP value of the counter
 * As a side-effect, it rebuilds the lookup tables.
 * @param frequencyHz the requested frequency in Hz
 */
STATIC void configureTimerPWMFrequency(uint32_t frequencyHz) {
    // Check the input parameter
    uint32_t timerFreq = TIMHW_getTimer0Frequency();
    // Check that frequency is not too low for LEDs
    if (frequencyHz < MIN_PWM_FREQ) {
        app_assert(frequencyHz < MIN_PWM_FREQ, "\r\nFrequency (%" PRIu32 "Hz) is too low!", frequencyHz);
    } else if (frequencyHz > (timerFreq / MIN_PWM_LEVELS)) {
        // Freq is so high we are losing resolution
        app_assert(frequencyHz > (timerFreq / MIN_PWM_LEVELS),
                   "\r\nFrequency (%" PRIu32 "Hz) is too high! Resolution would be too low", frequencyHz);
        frequencyHz = timerFreq / MIN_PWM_LEVELS;
    } else if (frequencyHz > MAX_PWM_FREQ) {
        app_assert(frequencyHz > MAX_PWM_FREQ, "\r\nFrequency (%" PRIu32 "Hz) is too high!", frequencyHz);
    }
    // Else, frequencyHz is within range (>4096 quantization levels AND >250Hz)
    // Configure TIMER frequency
    uint32_t top = (timerFreq / (2 * frequencyHz));
    TIMHW_setTimer0TopValue(top);
    app_log_debug("Freq %" PRIu32 " / Top set to %" PRIu32 "\r\n", timerFreq, TIMHW_getTimer0TopValue());
    // A change in the frequency will require a rebuild of the lookup tables
    // NOTE: I moved to a const table stored in flash, but if I were to change the frequency on-the-fly, the LUT would
    // have to be updated here
    // buildGammaLookUpTable(top);
}

/*
 * @brief Starts the CC module of a channel of TIMER0 as PWM mode.
 * @param channel the channel to configure
 * @param port the port of the GPIO pin to set as output
 * @param pinNo the number of the GPIO pin to set as output
 * @param polarity the polarity of the PWM signal (active low or active high)
 */
void initTimer0CCChannel(const CCChannel_t channel, const pinPort_t port, const uint8_t pinNo,
                         const polarity_t polarity) {
    // Enable the CC Channel
    TIMHW_enableChannelCompCapUnit(channel);
    // Initialise the CC pin for the channel
    TIMHW_setCCChannelPin(port, pinNo, channel);
    // TODO: add assert here for the case of bad freq
    TIMHW_configCCChannelPWM(channel, polarity);
    TIMHW_startTimer0();
}

/**
 * @brief Gets the freq of the PWM signal, based on the current clock freq and the TOP value
 * @return a uint32_t with the frequency of the PWM signal.
 */
uint32_t getPWMFrequency(void) { return (TIMHW_getTimer0Frequency() / (2 * TIMHW_getTimer0TopValue())); }

/**
 * @brief Sets the Duty Cycle of the PWM signal on one of TIMER0's channels. Brightness is gamma-corrected
 * NOTE: expects TIMER- to be enabled when called
 * @param channel the channel to set the brightness level
 * @param percent required birghtness in percent as an integer, with two decimal places -> 9521 is 95.21%
 * @param setBuffered a boolean indicating if we want to set the value using the buffer, or write to the compare
 * register straight away
 */
void setDutyCycle(const CCChannel_t channel, uint32_t percent, bool setBuffered) {
    if (setBuffered) {
        TIMHW_setT0ChannelOutputCompareBuffered(channel, brightnessLookUp[percent]);
    } else {
        TIMHW_setT0ChannelOutputCompare(channel, brightnessLookUp[percent]);
    }
}