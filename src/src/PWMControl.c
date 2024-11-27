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
#   define STATIC
#else
#   define STATIC static
#endif

// Gamma correction
// To correct the intensity of the LEDs to match the perception of our eyes, we need to do some maths™
const uint32_t GAMMA_VALUE = 2;
// To make things easy, we will use a lookup table to store the required compare values for a certain
// frequency to get that percentage of brightness.
#define gammaLuTSize 101
STATIC uint32_t gammaLookUp[gammaLuTSize] = {0};  // size 101 to get from 0% to 100% -> 101 values

// PWM Parameters
// never set a PWM frequency that results in less than 12 bits of resolution
const uint32_t MIN_PWM_LEVELS = 4096;
// Never set a PWM frequency of less than 250Hz.
const uint32_t MIN_PWM_FREQ = 250;

// With a 32 bit register and a clock at 38400000
// Min PWM Freq -> 250Hz -> TOP is 153599
// Max PWM Freq -> TOP to 4096 -> 38400000 / 4096 = 9375Hz

/// @brief Stores the current
STATIC volatile uint32_t dutyCycle[3] = {0};

////
// Forward Declarations
////

STATIC void buildGammaLookUpTable(void);

// Start TIMER0's HW
void initTimer0PWM(uint32_t PWMFreqHz) {
    TIMHW_initTimer0Clock();
    TIMHW_initTimer0(false);
    configureTimerPWMFrequency(PWMFreqHz);
}

// Starts the CC module of a channel of TIMER0 as PWM mode.
// Parameters: channel: the channel to configure
//             port: the port of the GPIO pin to set as output
//             pinNo: the number of the GPIO pin to set as output
//             polarity: the polarity of the PWM signal (active low or active high)
void initTimer0CCChannel(CCChannel_t channel, pinPort_t port, uint8_t pinNo, polarity_t polarity) {
    // Enable the CC Channel
    TIMHW_enableChannelCompCapUnit(channel);
    // Initialise the CC pin for the channel
    TIMHW_setCCChannelPin(port, pinNo, channel);
    // TODO: add assert here for the case of bad freq
    TIMHW_configCCChannelPWM(channel, polarity);
    TIMHW_startTimer0();
}

// Sets the PWM signal frequency by adjusting the TOP value of the counter
// As a side-effect, it rebuilds the lookup tables.
// Parameters: frequencyHz: the requested frequency in Hz
void configureTimerPWMFrequency(uint32_t frequencyHz) {
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
    uint32_t top = (timerFreq / (frequencyHz)) - 1U;
    TIMHW_setTimer0TopValue(top);
    // A change in the frequency will require a rebuild of the lookup tables
    buildGammaLookUpTable();
}

// Builds the Gamma-adjusted brighness lookup table.
STATIC void buildGammaLookUpTable(void) {
    uint32_t top = TIMHW_getTimer0TopValue();
    for (uint32_t i = 0; i < gammaLuTSize; i++) {
        gammaLookUp[i] = (uint32_t)((pow(i, GAMMA_VALUE) / pow (gammaLuTSize, GAMMA_VALUE)) * top + .5);
    }
    // DEBUG: print the table to check the values
    for (uint32_t i = 0; i < gammaLuTSize; i++) {
        app_log_info("%"PRIu32"% -> %"PRIu32"\r\n", i, gammaLookUp[i]);
    }
}

// Sets the Duty Cycle of the PWM signal on one of TIMER0's channels
// NOTE: expects TIMER- to be enabled when called
// Parameters: channel: the channel to set the PWM duty cycle
//             percent: the value for the duty cycle
void setDutyCycle(CCChannel_t channel, int8_t percent) {
    // Guard for percent being an stupid number
    if (percent > 100) {
        percent = 100;
    }
    // DEBUG: print set value
    // uint32_t top = TIMHW_getTimer0TopValue();
    // app_log_debug("Setting %d%: %"PRIu32"/%"PRIu32, percent, gammaLookUp[percent], TIMER_TopGet(TIMER0));
    // TODO: not sure why this doesn't work (why does the top value get reset on each count)
    // TIMHW_setT0ChannelBufferedOutputCompare(channel, gammaLookUp[percent]);
    // HACK: while we work with interrupts
    dutyCycle[channel] = gammaLookUp[percent];
}

// Gets the COUNT value for the requested channel
// Parameters: channel: the channel whose top value we want to get
uint32_t getDutyCycle(CCChannel_t channel) {
    return dutyCycle[channel];
}

// Sets the brightness level for one of TIMER0's channels. The brighness is adjusted using gamma correction
// Parameters: channel: the channel to set the PWM duty cycle
//             percent: the relative brightness level, from 0 to 100
void setBrightness(CCChannel_t channel, int8_t percent) {
    uint32_t compareValue = 0;
    // Guard for percent being an stupid number
    if (percent >= 100) {
        // Makes sure that 100% gets 100% Duty Cycle
        compareValue = TIMHW_getTimer0TopValue();
    } else {
        compareValue = gammaLookUp[percent];
    }
    // app_log_debug("Setting %d%: %"PRIu32"/%"PRIu32, percent, compareValue, getTimer0TopValue());
    // Set compare value using gamma correction lookup table
    TIMHW_setT0ChannelBufferedOutputCompare(channel, compareValue);
}
