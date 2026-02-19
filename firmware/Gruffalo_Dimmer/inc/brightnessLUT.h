/**
 * @file brightnessLUT.h
 * @author Joel Santos (jsantosrico@gmail.com)
 * @brief Header containing the lookup table for gamma-corrected brightness
 * @version 0.1
 * @date 2026-02-11
 *
 * @copyright Copyright (c) 2026
 *
 * This header contains the look-up table for different brightness levels, given as an integer value representing a
 * percentage with two decimal places (position 0 -> 0%, position 1234 ->12.34%)
 *
 * This allows us:
 *
 * - Having a big table (two decimal places, 10K positions). We have a lot of resolution on the compare value (32 bit
 * counter) so it would be a shame not make use of it by just having 100 values.
 * - It is precalculated and can be stored in flash and thus take no RAM at all!
 *
 * This file is generated from the code on the \"PWM And Breathe Effect\" jupyter notebook in /docs
 *
 */

#ifndef _BRIGHTNESS_LUT_H_
#define _BRIGHTNESS_LUT_H_

#include <inttypes.h>

/**
 * @brief Size of the gamma-corrected brightness look up table
 */
#define GAMMA_LUT_SIZE 10001UL

/**
 * @brief Contains the gamma-corrected brightness values, mapping percentage with two decimal places (as a uint) to the
 * compare register value required to obtain that perceived level of brightness
 *
 * Gamma-value -> 2.3
 *
 */
extern const uint32_t brightnessLookUp[GAMMA_LUT_SIZE];

#endif  // _BRIGHTNESS_LUT_H_
