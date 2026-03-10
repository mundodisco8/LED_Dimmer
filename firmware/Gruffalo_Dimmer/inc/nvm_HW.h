/**
 * @file nvm_HW.h
 * @author Joel Santos (jsantosrico@gmail.com)
 * @brief provides functions to interface with the nvm3 module
 * @version 0.1
 * @date 2026-03-09
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef _NVM_HW_
#define _NVM_HW_

#include "effectControl.h"

/**
 * @brief NVM Keys for the different items to store
 */
typedef enum nvmKeys {
    NVM3_KEY_LED1_CONTEXT = 0x1001UL,  // Stores the context for LED Channel 1
    NVM3_KEY_LED2_CONTEXT = 0x1002UL,  // Stores the context for LED Channel 2
    NVM3_KEY_LED3_CONTEXT = 0x1003UL,  // Stores the context for LED Channel 3
} nvmKeys_t;

/**
 * @brief Initialises the NVM
 */
void nvm_init(void);

/**
 * @brief reads an LED Channel Context from NVM
 */
void nvm_readLEDContext(const nvmKeys_t key, LEDContext_t* ptrContext);
/**
 * @brief Writes an LED channel context to the NVM, so it can be recovered later
 */
void nvm_writeLEDContext(const nvmKeys_t key, const LEDContext_t* ptrContext);

/**
 * @brief Checks if an nvm repack is needed, and performs one if that's the case
 */
void nvm_checkForRepack(void);
#endif  // _NVM_HW_