////
// PWMControl
//
// Sets TIMER0 in PWM mode and provides control for its three CC channels
//
// To initialise the module
// 1) Call initTimer0() to start TIMER0's HW
// 2) Configure the required CC Chanels with initTimer0CCChanel()
// 3) Set the required brightess with setBrightness(). To get a certain duty cycle, call setDutyCycle() instead
//
////

#include "PWMControl.h"

#include "timer_HW.h"

#include "math.h"

// Gamma correction
// To correct the intensity of the LEDs to match the perception of our eyes, we need to do some maths™
const uint32_t GAMMA_VALUE = 2;
// To make things easy, we will use a lookup table to store the required compare values for a certain
// frequency to get that percentage of brightness.
uint32_t gammaLookUp[101] = {0};  // size 101 to get from 0% to 100% -> 101 values

////
// Forward Declarations
////

void buildGammaLookUpTable(void);

// Start TIMER0's HW
void initTimer0HW(void) {
    initTimer0Clock();
}

void runTimer0(void) {
    startTimer0();
}

// Starts the CC module of a channel of TIMER0 as PWM mode.
// Parameters: channel: the channel to configure
//             port: the port of the GPIO pin to set as output
//             pinNo: the number of the GPIO pin to set as output
//             polarity: the polarity of the PWM signal (active low or active high)
void initTimer0CCChanel(CCChannel_t channel, pinPort_t port, uint32_t pinNo, uint32_t PWMFreqHz, polarity_t polarity) {
    configCCChannelPWM(channel, polarity);
    setCCChannelPin(port, pinNo, channel);
    configureTimerPWMFrequency(PWMFreqHz);
    startChannelPWM(channel);
}

// Sets the PWM signal frequency by adjusting the TOP value of the counter
// As a side-effect, it rebuilds the lookup tables.
// Parameters: frequencyHz: the requested frequency in Hz
void configureTimerPWMFrequency(uint32_t frequencyHz) {
    // Check the input parameter
    uint32_t clockFreq = getTimerFrequency();
    if (frequencyHz > clockFreq) {
        // The requested freq is higher than the timer freq!
        frequencyHz = clockFreq;
    }  // Else, frequencyHz is
    // Configure TIMER frequency
    uint32_t top = (clockFreq / (frequencyHz)) - 1U;
    setTimer0TopValue(top);
    // A change in the frequency will require a rebuild of the lookup tables
    buildGammaLookUpTable();
}

// Builds the Gamma-adjusted brighness lookup table.
void buildGammaLookUpTable(void) {
    uint32_t lookUpSize = (sizeof(gammaLookUp) / sizeof(gammaLookUp[0]));
    uint32_t top = getTimer0TopValue();
    for (uint32_t i = 0; i < lookUpSize; i++) {
        gammaLookUp[i] = (int)((pow(i, GAMMA_VALUE) / pow (lookUpSize, GAMMA_VALUE)) * top + .5);
    }
    // DEBUG: print the table to check the values
    // for (uint32_t i = 0; i < lookUpSize; i++) {
    //     app_log_info("%d% -> %d\r\n", i, gammaLookUp[i]);
    // }
}

// Sets the Duty Cycle of the PWM signal on one of TIMER0's channels
// Parameters: channel: the channel to set the PWM duty cycle
//             percent: the value for the duty cycle
void setDutyCycle(CCChannel_t channel, uint8_t percent) {
    uint32_t top = getTimer0TopValue();
    // Guard for percent being an stupid number
    if (percent == 100) {
        percent = 100;
    }
    // DEBUG: print set value
    // app_log_debug("Setting %d%: %d/%d", percent, compareValue, TIMER_TopGet(TIMER0));
    setChannelBufferedOutputCompare(channel, (top * percent) / 100);
}

// Sets the brightness level for one of TIMER0's channels. The brighness is adjusted using gamma correction
// Parameters: channel: the channel to set the PWM duty cycle
//             percent: the relative brightness level, from 0 to 100
void setBrightness(CCChannel_t channel, uint8_t percent) {
    uint32_t compareValue = 0;
    // Guard for percent being an stupid number
    if (percent >= 100) {
        // Makes sure that 100% gets 100% Duty Cycle
        compareValue = getTimer0TopValue();
    } else {
        compareValue = gammaLookUp[percent];
    }
    // app_log_debug("Setting %d%: %d/%d", percent, compareValue, getTimer0TopValue());
    // Set compare value using gamma correction lookup table
    setChannelBufferedOutputCompare(channel, compareValue);
}
