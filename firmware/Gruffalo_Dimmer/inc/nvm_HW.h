/**
 * @file nvm_HW.h
 * @author Joel Santos (jsantosrico@gmail.com)
 * @brief Public API for interfacing with the NVM3 non-volatile storage module.
 * @version 0.1
 * @date 2026-03-13
 * * @copyright Copyright (c) 2026
 * * @defgroup NVM_HW NVM Hardware Module
 * @{
 *
 * ### Detailed Description
 * This module provides an abstraction layer for the Silicon Labs NVM3 (Non-Volatile
 * Memory) data storage system. It is specifically tailored to persist LED
 * configuration states across power cycles.
 *
 * ### Key Features
 * * **Persistent Contexts:** Saves and restores @ref LEDContext_t structures,
 * including brightness, animation types, and timing periods.
 * * **Key-Value Mapping:** Uses unique 32-bit keys (@ref nvmKeys_t) to manage
 * independent data objects for each LED channel.
 *
 * ### Usage Flow
 * 1. **Initialization:** Call @ref nvm_init at system startup to open the
 * default NVM3 instance.
 * 2. **Restoration:** Use @ref nvm_readLEDContext to reload saved states into
 * RAM during the application boot sequence.
 * 3. **Persistence:** Call @ref nvm_writeLEDContext whenever a user modifies
 * a setting (e.g., via encoders or buttons) to ensure the change survives a reset.
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