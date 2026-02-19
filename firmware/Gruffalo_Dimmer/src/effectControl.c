/**
 * LED Effect Control
 *
 * Because in this iteration of the code I'm went baremetal, I will control the LED effects via interrupts instead of
 * relying in timings. Let's see if it's the right decision or if I regret it and move back to an RTOS and use timers :)
 *
 * The tick for an effect will be the completion of a PWM cycle. The overflow interrupt will determine if changes need
 * to be done in the current PWM.
 * */

#include "effectControl.h"

// StdLib
#include <math.h>

// SiLabs headers
#include "app_assert.h"
#include "app_log.h"

// Our Libs
#include "PWMControl.h"

// Little trick to test static stuff
#ifdef TEST
#define STATIC
#else
#define STATIC static
#endif

////
// Consts and static stuff
////

/**
 * @brief The number of LED channels, set by HW
 */
const uint32_t NUM_LED_CHANNELS = 3UL;

/**
 * @brief The LED brightness at startup
 */
const uint32_t DEFAULT_LED_BRIGHTNESS = 5000UL;

/**
 * @brief The minimum and default values for the breathe period, in ms
 */
const uint32_t BREATHE_MIN_PERIOD_MS = BREATHE_LUT_SIZE;
const uint32_t BREATHE_DEFAULT_PERIOD_MS = 5000;

/**
 * @brief Default values for the breathe effect
 */
const uint32_t BREATHE_MAX_BRIGHTNESS = MAX_BRIGHTNESS;
const uint32_t BREATHE_MIN_BRIGHTNESS = 1000;  // 10%
const double BREATHE_BETA = 0.5L;              // peak at half the period
const double BREATHE_GAMMA = .15L;

/**
 * @brief Contains the pre-calculated values for the breathe effect brightness levels. uint16_t as it contains a percent value
 * with 2 significative numbers (10000 = 100.00%)
 */
STATIC uint32_t gausianBreatheLUT[BREATHE_LUT_SIZE] = {0UL};

/**
 * @brief Default parameters of the breathe effect, in case none are provided
 *
 */
const breatheParams_t DEFAULT_BREATHE_PARAMS = {.maxBrightness = BREATHE_MAX_BRIGHTNESS,
                                                .minBrightness = BREATHE_MIN_BRIGHTNESS,
                                                .beta = BREATHE_BETA,  // set the breathe effect peak at half the period
                                                .gamma = BREATHE_GAMMA,
                                                .LUTSize = BREATHE_LUT_SIZE};

// Instances of LED Strips
STATIC LED_t LEDCh1 = {.currAnimation = ANIM_FIXED};
STATIC LED_t LEDCh2 = {.currAnimation = ANIM_FIXED};
STATIC LED_t LEDCh3 = {.currAnimation = ANIM_FIXED};

/**
 * @brief Fills the breateh effect LUT with the correct parameters as specified by effectParams
 * @param effectParams_ptr a pointer to a breatheParams_t struct with the parameters of the breathe effect
 */
void fillBreatheEffectLUT(const breatheParams_t* effectParams_ptr) {
    if (effectParams_ptr != NULL) {
        for (uint32_t i = 0; i < effectParams_ptr->LUTSize; i++) {
            float aux = effectParams_ptr->minBrightness +
                        (effectParams_ptr->maxBrightness - effectParams_ptr->minBrightness) *
                            exp(-(pow(((float)i / effectParams_ptr->LUTSize) - effectParams_ptr->beta, 2.0L) /
                                  (2 * pow(effectParams_ptr->gamma, 2.0L))));
            gausianBreatheLUT[i] = (uint16_t)lroundf(aux);
        }
    }
}

/**
 * @brief Initialises the LED structs. Because the HW is fixed, we can also fix the memory and do everything inside
 * the function without having to pass anything.
 * @param LED_ptr
 * @return
 */
efferr_t initLEDStrips(void) {
    // No need to pass anything to it as we are initting all the strips we have to default values
    LED_t* channels[3] = {&LEDCh1, &LEDCh2, &LEDCh3};
    // breatheControl_t* breatheCtrlArray[3] = {&breatheCh1, &breatheCh2, &breatheCh3};
    // brightnessControl_t* brightnessCtrlArray = {&brightnessCh1, &brightnessCh2, &brightnessCh3};

    for (uint32_t i = 0; i < NUM_LED_CHANNELS; i++) {
        // Get next pointer fron the list of channels
        LED_t* currChannel_ptr = channels[i];
        // Set animation to fixed brightness
        currChannel_ptr->currAnimation = ANIM_FIXED;
        // Set brightness to starting default, and set the brightness Change Request Flag
        currChannel_ptr->brightnessCtrl.targetBrightness = DEFAULT_LED_BRIGHTNESS;
        currChannel_ptr->brightnessCtrl.delta = 0;  // will be calculated in the effect control loop
        currChannel_ptr->brightnessCtrl.brightChangeRequestedFlag = true;
        // Set PWM period from clock. multiplied by 1000 to get ms
        currChannel_ptr->pmwPeriodms = 1000UL / (getPWMFrequency());
        // Assert if the PWM period is 0, as that we use it as  a divisor!
        app_assert(currChannel_ptr->pmwPeriodms != 0, "\r\nPWM Period is %" PRIu32 "!", currChannel_ptr->pmwPeriodms);
        // Init breatheControl_t for that LED, set to 0 as we are starting with fixed brightness
        currChannel_ptr->breatheCtrl.periodms = 0;
        currChannel_ptr->breatheCtrl.wavesPerSample = 0;
        currChannel_ptr->breatheCtrl.currWave = 0;
        currChannel_ptr->breatheCtrl.currLUTIndex = 0;
    }

    setBreathePeriod(LED_CHANNEL_1, BREATHE_DEFAULT_PERIOD_MS);
    setBreathePeriod(LED_CHANNEL_2, BREATHE_DEFAULT_PERIOD_MS);
    setBreathePeriod(LED_CHANNEL_3, BREATHE_DEFAULT_PERIOD_MS);

    // Link LEDs and PWM channels
    LEDCh1.CCChannel = CC_CHANNEL_0;
    LEDCh2.CCChannel = CC_CHANNEL_1;
    LEDCh3.CCChannel = CC_CHANNEL_2;

    return EFF_OK;
}

/**
 * @brief Returns a pointer to the LED struct for the desired channel
 * @param channelNo the channel which LED struct we want to retrieve
 * @return a pointer to the LED_t struct of the channel passed as parameter. On wrong channel, defaults to LED Ch 1
 */
LED_t* getLEDStruct(const LEDChannel_t channelNo) {
    LED_t* LED_ptr = NULL;
    switch (channelNo) {
        case LED_CHANNEL_2: {
            LED_ptr = &LEDCh2;
            break;
        }
        case LED_CHANNEL_3: {
            LED_ptr = &LEDCh3;
            break;
        }
        case LED_CHANNEL_1: /* fallthrough */
        default: {
            // Default, on wrong channel, return Ch 1
            LED_ptr = &LEDCh1;
            break;
        }
    }
    return LED_ptr;
}

/**
 * @brief Get the name of the current Effect as a string
 * @param effectType a type of effect
 * @return char* a null terminated string with the name of the effect
 */
char* getEffectName(const anim_t effectType) {
    switch (effectType) {
        case ANIM_BREATHE: {
            return "Breathe Effect";
        }
        case ANIM_FIXED: /* fall-through */
        default: {
            return "Fixed Brightness";
        }
    }
}

/**
 * @brief Sets the brightness of one of the LED Channels.
 * @param channel the channel to set the PWM duty cycle
 * @param percent the relative brightness level in percent with two decimal places, as an integer, from 0 to 10000
 * @return the brightness value set
 */
uint32_t setLEDBrightness(const LEDChannel_t channel, uint32_t percent) {
    // No need to check valid return because it asserts on invalid channel
    LED_t* LED_ptr = getLEDStruct(channel);

    if ((int32_t)percent < 0) {
        // Percent is a uint, so we can't check if percent is negative, but we can cast it to a int32_t to do it (which
        // means if the uint32_t > (UINT32T_MAX / 2))
        percent = 0;
    } else if (percent > MAX_BRIGHTNESS) {
        // We need to test for "negative" first, as a "negative" is bigger than MAX_BRIGHTNESS
        percent = MAX_BRIGHTNESS;
    }

    LED_ptr->brightnessCtrl.targetBrightness = percent;
    LED_ptr->brightnessCtrl.brightChangeRequestedFlag = true;

    return percent;
}

/**
 * @brief Get the LED brightness for the requested LED channel
 *
 * @param channel the LED channel to get the brightness level
 * @return uint32_t the brigtness as a integer percent with two decimal places (9895 is 98.95%)
 */
uint32_t getLEDBrightness(const LEDChannel_t channel) {
    // No need to check valid return because it asserts on invalid channel
    return getLEDStruct(channel)->brightnessCtrl.targetBrightness;
}

/**
 * @brief Sets a new period for the breathe effect
 * @param channelNo the LED channel that will get its period changed
 * @param newPeriodms the new period, in ms
 * @return EFF_OK on success, EFF_BADPERIOD if the new period is 0, asserts through getLEDStruct on bad channel
 */
efferr_t setBreathePeriod(const LEDChannel_t channelNo, const uint32_t newPeriodms) {
    // No need to check valid return because it asserts on invalid channel
    LED_t* LED_ptr = getLEDStruct(channelNo);

    // check new Period
    if (newPeriodms == 0) {
        // TODO: Log error? / Assert
        return EFF_BADPERIOD;
    }  // Else, period is not 0
    // set new Period
    // The period should be a multiple of the PWM period, so an integer number of PWM waves is played (technically,
    // n * Tpwm * nSamplesLUT). One option is to coax the newPeriod to be a multiple of the PWM period when a "wrong"
    // period is used as newPeriod. For example, if the period has to be multiple of 100ms, and we try to set it to
    // 490ms, round up to 500ms.
    // The issue I see is that we use a rotary encoder to set the period, it will increase/decrease by a set number, and
    // we might get stuck in a loop where we can't raise / lower the period: in the same example, we are at 500ms,
    // decrease by 10ms, get to 490ms but the code sets the round value of 500ms, so we would not be able to increase
    // or decrease the period.
    // I think the best way to tackle this is to store the "non-integer period" and approximate the wavesPerSample. This
    // means that out period would potentially not match the real one, but the margin of error is within
    // nSamplesLUT * Tpwm / 2 ms
    LED_ptr->breatheCtrl.periodms = newPeriodms;
    // Set wavesPerSample: round result to nearest "wavePerSample"
    LED_ptr->breatheCtrl.wavesPerSample = lroundf(((float)newPeriodms) / (BREATHE_LUT_SIZE * LED_ptr->pmwPeriodms));
    // if we abruptly drop newPeriod, we might find ourselves in a situation where currWave is > than wavesPerSample.
    // This should be handled by the interrupt code, so there are no glitches. I could patch this by setting it to
    // currWave = wavesPerSample - 1 to trigger a buffered COMP at the next wave, but I will leave it for now as it is.

    return EFF_OK;
}

/**
 * @brief Get the Breathe Period for the requested LED channel
 *
 * @param channelNo the LED channel to get the breathe period
 * @return uint32_t the set breathe period in ms
 */
uint32_t getBreathePeriod(const LEDChannel_t channelNo) {
    // No need to check valid return because it asserts on invalid channel
    return getLEDStruct(channelNo)->breatheCtrl.periodms;
}

/**
 * @brief Tracks the current brightness of a LED channel and fades it progressively towards the intended target level
 * @param LEDChannel the channel that needs its brightness adjusted
 */
STATIC void effectControl_FadeBrightness(LEDChannel_t LEDChannel) {
    LED_t* LED_ptr = getLEDStruct(LEDChannel);

    const uint32_t currentLevel = LED_ptr->brightnessCtrl.currentBrightness;
    const uint32_t targetLevel = getLEDBrightness(LEDChannel);

    if (targetLevel == currentLevel) {
        return;  // nothing to do
    }  // Else, change the brightness

    // Find the delta: the goal is to change brightness in ~1s. Because we call this loop after each overflow int,
    // we tick at ~PWM freq.

    // Because the "compares" are uints, we can't simply take the absolute value of the difference, because of target is
    // smaller than current, we get the abs of a "negative" which is MASSIVE parsed as uint. We do some casting trickery
    // con control the result.
    uint32_t diff = labs(((int32_t)targetLevel - (int32_t)currentLevel));  // we will use this result a couple of times

    if (LED_ptr->brightnessCtrl.brightChangeRequestedFlag) {
        // We want to calculate the delta only when there's a change in brightness

        uint32_t delta = diff / getPWMFrequency();
        // If PWM freq is bigger than the difference between levels, then delta would be 0. In that case we would step at
        // the minimum delta of 1 (0.01%)
        delta = (delta == 0) ? 1 : delta;
        // Check if the delta is less than the difference between the current compare and the target.
        LED_ptr->brightnessCtrl.brightChangeRequestedFlag = false;
        LED_ptr->brightnessCtrl.delta = delta;
    }

    uint32_t step = LED_ptr->brightnessCtrl.delta;
    if (diff < step) {
        step = diff;  // difference is less than delta, so use difference to hit the target
    }

    if (currentLevel > targetLevel) {
        // Decrease towards target
        LED_ptr->brightnessCtrl.currentBrightness -= step;
    } else if (currentLevel < targetLevel) {
        // Increase towards target
        LED_ptr->brightnessCtrl.currentBrightness += step;
    }  // and if both are equal, don't do anything
    setDutyCycle(LED_ptr->CCChannel, LED_ptr->brightnessCtrl.currentBrightness, true);
}

/**
 * @brief plays a breathe effect on the desirec LED channel
 * @param LEDChannel the LED channel to play the effect.
 */
STATIC void effectControl_Breathe(LEDChannel_t LEDChannel) {
    LED_t* LED_ptr = getLEDStruct(LEDChannel);
    if (LED_ptr->breatheCtrl.wavesPerSample == 0) {
        return;
    }
    // Get the brightess for this sample, scaled by the target Brightness of brightnessCtrl
    uint32_t brightness = gausianBreatheLUT[LED_ptr->breatheCtrl.currLUTIndex] *
                          LED_ptr->brightnessCtrl.targetBrightness / MAX_BRIGHTNESS;
    // Increase currentWave counter
    LED_ptr->breatheCtrl.currWave++;
    // Check if we need to move to the next sample
    if (LED_ptr->breatheCtrl.currWave >= LED_ptr->breatheCtrl.wavesPerSample) {
        LED_ptr->breatheCtrl.currLUTIndex++;
        if (LED_ptr->breatheCtrl.currLUTIndex >= BREATHE_LUT_SIZE) {
            LED_ptr->breatheCtrl.currLUTIndex = 0;
        }
        LED_ptr->breatheCtrl.currWave = 0;
    }
    setDutyCycle(LED_ptr->CCChannel, brightness, true);
}

const LEDChannel_t LEDChannels[] = {LED_CHANNEL_1, LED_CHANNEL_2, LED_CHANNEL_3};

/**
 * @brief Function triggered by an overflow interrupt in TIME0. Controls the brightness of the LEDs and the effects
 * played on them
 */
void effectControlLoop(void) {
    // NOTE: the effect Control Loop runs at the PWM frequency, as it's triggered by the timer overflow interrupt
    // Write OCB to update the duty cycle of the next waveform period
    // Ramp up/down from the current compare value to the target, increasing/decreasing by 1 on each PWM cycle
    for (uint32_t i = 0; i < CH_COUNT; i++) {
        anim_t currEffect = getLEDStruct(LEDChannels[i])->currAnimation;
        if (currEffect == ANIM_FIXED) {
            effectControl_FadeBrightness(LEDChannels[i]);
        } else if (currEffect == ANIM_BREATHE) {
            effectControl_Breathe(LEDChannels[i]);
        }
    }
}