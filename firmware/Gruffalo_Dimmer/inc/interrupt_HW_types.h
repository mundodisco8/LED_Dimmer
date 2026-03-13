/**
 * @file interrupt_HW_types.h
 * @author Joel Santos (jsantosrico@gmail.com)
 * @brief datatypes for the interrupt_HW module
 * @version 0.1
 * @date 2026-03-13
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef _INTERRUPT_HW_TYPES_H_
#define _INTERRUPT_HW_TYPES_H_

#include "inttypes.h"

/**
 * @brief typedef for interrupt callback functions
 */
typedef void (*callbackPtr_t)(uint8_t intNo);

/**
 * @brief typedef for interrupt callback functions with context
 */
typedef void (*callbackCtxPtr_t)(uint8_t intNo, void* ctx);

#endif  // _INTERRUPT_HW_TYPES_H_