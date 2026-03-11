#ifndef _EFFECT_CONTROL_H_
#define _EFFECT_CONTROL_H_

#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>

// Our libs
#include "timer_HW_types.h"

/**
  * @brief Size of the Breathe Effect brightness LUT. 0% + 99 samples ascending + 100% + 99 samples descending
  */
#define BREATHE_LUT_SIZE 200U

/**
 * @brief The minimum value for the breathe period, in ms
 */
extern const uint32_t BREATHE_MIN_PERIOD_MS;

/**
 * @brief The number of independent LED channels in the board
 */
extern const uint32_t NUM_LED_CHANNELS;
/**
 * @brief Error Codes for the effect Control library
 */
typedef enum efferr {
    EFF_OK = 0,
    EFF_NULLPTR = 1,
    EFF_BADPERIOD = 2,
    EFF_BADCHANNEL = 3,
    EFF_BADANIMATION = 4,
    EFF_ERROR,
} efferr_t;

/**
 * @brief LED channel numbers
 */
typedef enum channelNo {
    LED_CHANNEL_1 = 0,
    LED_CHANNEL_2,
    LED_CHANNEL_3,
    LED_CHANNELS_ALL,  // All channels selected
} LEDChannel_t;

/**
 * @brief Type with the valid animation effects for an LED
 */
typedef enum anim {
    ANIM_FIXED = 0,    // Constant Brightness
    ANIM_BREATHE = 1,  // Breathe Effect
    ANIM_MAX_EFFECTS
} anim_t;

/*
 * @brief Struct to control changes in brightness
 *
 */
typedef struct brightnessControl {
    uint32_t targetBrightness;       // The desired brightness we want to fade into.
                                     // also used to scale the brethe effect brightness.
    uint32_t currentBrightness;      // The brightness level we are currently at (used when fading in/out)
    bool brightChangeRequestedFlag;  // Used as an aux to know when to update the delta
    uint32_t delta;                  // Brightness  increase / decrease step
} brightnessControl_t;

/**
 * @brief Struct containing the parameters needed to define the Gaussian curve played on the breathe effect
 */
typedef struct breatheParams {
    uint32_t maxBrightness;  // Maximum brightness in percent with two decimal places as a uint (0-10000)
    uint32_t minBrightness;  // Minimum brightness in percent with two decimal places as a uint (0-10000)
    float beta;              // Relative point in the curve where it reaches its peak (0 first sample, 1 last sample)
    float gamma;             // Determines the curve's width, the closer to 0, the narrower (darker) the curve
    uint32_t LUTSize;        // The size of the LUT holding the samples
} breatheParams_t;

/**
 * @brief Struct containing the control parameters of a breathe effect (params required to play)
 */
typedef struct breatheControl {
    uint32_t periodms;        // period of the breathe effect in ms
    uint32_t wavesPerSample;  // Number of PWM waves to play per sample of the LUT
    uint32_t currWave;        // Current PWM waves played for a sample of the LUT
    uint32_t currLUTIndex;    // The index of the LUT that is being played
} breatheControl_t;

/**
 * @brief This struct contains all the data required to play any LED effect on an LED channel
 */
typedef struct LED {
    anim_t currAnimation;                // The type of animation currently being played in the LED
    uint32_t pmwPeriodms;                // The PWM period in ms //TODO: I think this can safely go
    CCChannel_t CCChannel;               // The PWM channel associated to this LED
    brightnessControl_t brightnessCtrl;  // Brightness Control Parameters
    breatheControl_t breatheCtrl;        // Parameters of the breathe effect for this LED
} LED_t;

/**
 * @brief This struct stores all the data required to restore an LED channel after the system wakes from sleep
 */
typedef struct LEDContext {
    anim_t currAnimation;        // Stores the current animation being played in an LED Channel
    uint32_t currBrightness;     // Stores the current max brightness of an LED Channel
    uint32_t currBreathePeriod;  // Stores the current period of the Breathe effect on an LED Channel
} LEDContext_t;

/**
 * @brief Default parameters of the breathe effect, in case none are provided
 *
 */
extern const breatheParams_t DEFAULT_BREATHE_PARAMS;

/**
 * @brief Fills the breateh effect LUT with the correct parameters as specified by effectParams
 */
void fillBreatheEffectLUT(const breatheParams_t* effectParams_ptr);

/**
 * @brief Initialises the LED structs. Because the HW is fixed, we can also fix the memory and do everything inside
 */
efferr_t initLEDStrips(void);

/**
 * @brief Sets the brightness of one of the LED Channels.
 */
uint32_t setLEDBrightness(const LEDChannel_t channel, uint32_t percent);
/**
 * @brief Returns a pointer to the LED struct for the desired channel
 */
uint32_t getLEDBrightness(const LEDChannel_t channel);

/**
 * @brief Sets the brightness of one of the LED Channels.
 */
efferr_t setBreathePeriod(const LEDChannel_t channelNo, const uint32_t newPeriodms);
/**
 * @brief Get the Breathe Period for the requested LED channel
 */
uint32_t getBreathePeriod(const LEDChannel_t channelNo);

/**
 * @brief Sets a new animation type to be played on one of the LED Channels.
 */
efferr_t setAnimation(const LEDChannel_t channelNo, const anim_t newAnimation);
/**
 * @brief Sets a new animation type to be played on one of the LED Channels.
 */
anim_t getAnimation(const LEDChannel_t channelNo);

/**
 * @brief Returns a pointer to the LED struct for the desired channel
 */
LED_t* getLEDStruct(const LEDChannel_t channelNo);

/**
 * @brief Get the name of the current Effect as a string
 */
char* getEffectName(const anim_t effectType);

/**
 * @brief Function triggered by an overflow interrupt in TIME0. Controls the brightness of the LEDs and the effects
 * played on them
 */
void effectControlLoop(void);
#endif  // _EFFECT_CONTROL_H_