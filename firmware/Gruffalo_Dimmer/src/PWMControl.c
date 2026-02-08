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

/**
 * @brief The gammaLookUp stores the gamma-corrected values for brightness. This way, brightness will be modeled in a
 * similar way to our eye and behave more linearly.
 * The table stores values from
 */
#define GAMMA_LUT_SIZE 101UL                        // 101, so we have values for each percentile [0-100UL]
STATIC uint32_t gammaLookUp[GAMMA_LUT_SIZE] = {0};  // size 101 to get from 0% to 100UL% -> 101 values

// PWM Parameters
// never set a PWM frequency that results in less than 12 bits of resolution
const uint32_t MIN_PWM_LEVELS = 4096UL;
// Never set a PWM frequency of less than 250Hz.
const uint32_t MIN_PWM_FREQ = 250UL;

// With a 32 bit register and a clock at 38400000
// Min PWM Freq -> 250Hz -> TOP is 153599
// Max PWM Freq -> TOP to 4096 -> 38400000 / 4096 = 9375Hz

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

STATIC void buildGammaLookUpTable(void);
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
        // Set freq to the minimum, MIN_PWM_FREQ
        frequencyHz = MIN_PWM_FREQ;
    } else if (frequencyHz > (timerFreq / MIN_PWM_LEVELS)) {
        // Freq is so high we are losing resolution
        frequencyHz = timerFreq / MIN_PWM_LEVELS;
    }  // Else, frequencyHz is within range (>4096 quantization levels AND >250Hz)
    // Configure TIMER frequency
    uint32_t top = (timerFreq / (frequencyHz)) - 1UL;  // -1 because the counter "counts" 0
    TIMHW_setTimer0TopValue(top);
    // A change in the frequency will require a rebuild of the lookup tables
    buildGammaLookUpTable();
}

/**
 * @brief Builds the Gamma-adjusted brighness lookup table.
 */
STATIC void buildGammaLookUpTable(void) {
    uint32_t top = TIMHW_getTimer0TopValue();
    // We will need this one to normalize the gamma curve from 0 to 1
    float maxGammaValue = pow((GAMMA_LUT_SIZE - 1), GAMMA_VALUE);
    for (uint32_t i = 0; i < GAMMA_LUT_SIZE; i++) {
        float aux = (pow(i, GAMMA_VALUE) / maxGammaValue);       // returns a value normalised to 1
        gammaLookUp[i] = min(top, (uint32_t)lround(aux * top));  // gets the value relative to TOP
        // // gammaLutSize - 1 because the table is 0 indexed, and to show values from 0 to 100UL, we need 101 positions
        // uint32_t aux = (uint32_t)((pow((((float)i / (GAMMA_LUT_SIZE - 1)) * top), GAMMA_VALUE)) * top + .5);
        // if (aux > top) {
        //     // That +.5, I don't know where it comes from,
        //     aux = top;
        // }
    }
    // DEBUG: print the table to check the values
    // for (uint32_t i = 0; i < GAMMA_LUT_SIZE; i++) {
    //     app_log_info("%" PRIu32 " -> Gamma-corrected %" PRIu32 "\r\n", i, gammaLookUp[i]);
    // }
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
uint32_t getPWMFrequency(void) { return (TIMHW_getTimer0Frequency() / TIMHW_getTimer0TopValue()); }

/**
 * @brief Returns the Compare register value to get the expected duty cycle for percent
 * @param percent the required duty cycle, as a percent with two decimal places, as an integer
 * @return a uint32_t the value to set the compare register to get the required duty cycle
 */
uint32_t percentToCompare(uint32_t percent) {
    // Because we have a low PWM freq, we have a lot of resolution available, and it would be a same not to use it, but
    // at the same time, because we use a LUT for the gamma-corrected brightness, we don't want to have too much
    // granularity.
    // My approach is something in between, passing the bightness as percents with two decimal values, but the gamma LUT
    // will only store values for "integer" percentages and linearly interpolate the decimal places. Let's see if that
    // works. I could probably calculate the gamma-corrected values on the spot, I have an APU, but that's how I started
    // and I don't want to redo it.

    uint32_t compareValue;

    // Guard for percent being an stupid number
    if (percent > MAX_BRIGHTNESS) {
        percent = MAX_BRIGHTNESS;
    }
    // DEBUG: print set value
    // app_log_debug("Setting %" PRIu32 ".%" PRIu32 " -> COMP %" PRIu32, (percent / 100UL), (percent % 100UL),
    //               gammaLookUp[percent / 100UL]);
    // Get the integer part of the percent value and check it's brightness
    compareValue = gammaLookUp[percent / 100UL];
    if (percent % 100UL != 0) {
        // We have to interpolate as percent is not integer
        uint32_t fractional = percent % 100UL;
        // the next brightness level
        uint32_t aux = gammaLookUp[(percent / 100UL) + 1];
        // So the interpolated brightness is
        compareValue = compareValue + (((aux - compareValue) * fractional) / 100UL);
    }

    return compareValue;
}

/**
 * @brief Sets the Duty Cycle of the PWM signal on one of TIMER0's channels. Brightness is gamma-corrected
 * NOTE: expects TIMER- to be enabled when called
 * @param channel the channel to set the brightness level
 * @param percent required birghtness in percent as an integer, with two decimal places -> 9521 is 95.21%
 * @param setBuffered a boolean indicating if we want to set the value using the buffer, or write to the compare
 * register straight away
 */
void setDutyCycle(const CCChannel_t channel, uint32_t percent, bool setBuffered) {
    // Because we have a low PWM freq, we have a lot of resolution available, and it would be a same not to use it, but
    // at the same time, because we use a LUT for the gamma-corrected brightness, we don't want to have too much
    // granularity.
    // My approach is something in between, passing the bightness as percents with two decimal values, but the gamma LUT
    // will only store values for "integer" percentages and linearly interpolate the decimal places. Let's see if that
    // works. I could probably calculate the gamma-corrected values on the spot, I have an APU, but that's how I started
    // and I don't want to redo it.

    uint32_t compareValue = percentToCompare(percent);

    if (setBuffered) {
        TIMHW_setT0ChannelOutputCompareBuffered(channel, compareValue);
    } else {
        TIMHW_setT0ChannelOutputCompare(channel, compareValue);
    }
}