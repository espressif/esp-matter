/***************************************************************************//**
 * @file
 * @brief Manufacturing token handling
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
#ifndef BTL_SECURITY_TOKENS_H
#define BTL_SECURITY_TOKENS_H

#include <stdint.h>
#include "em_device.h"
#include "config/btl_config.h"

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

#if defined(LOCKBITS_BASE)
#define PUBKEY_OFFSET_X (0x34A)
#define PUBKEY_OFFSET_Y (0x36A)
#elif defined(SEMAILBOX_PRESENT)
#include <stddef.h>
// Lockbits are placed in the topmost flash page
#define LOCKBITS_BASE ((FLASH_BASE) + (FLASH_SIZE) -(FLASH_PAGE_SIZE))
#if defined(BOOTLOADER_FALLBACK_LEGACY_KEY) && (BOOTLOADER_FALLBACK_LEGACY_KEY == 1)
#include "em_se.h"
#define PUBKEY_OFFSET_X (0x34C)
#define PUBKEY_OFFSET_Y (0x36C)
#endif
// #elif defined(_SILICON_LABS_GECKO_INTERNAL_SDID_200)
#elif defined(_SILICON_LABS_GECKO_INTERNAL_SDID_205) || defined(_SILICON_LABS_GECKO_INTERNAL_SDID_230)
// Lockbits are placed in the topmost flash page
#define LOCKBITS_BASE ((FLASH_BASE) + (FLASH_SIZE) -(FLASH_PAGE_SIZE))
#define PUBKEY_OFFSET_X (0x34C)
#define PUBKEY_OFFSET_Y (0x36C)
// #elif defined(_SILICON_LABS_GECKO_INTERNAL_SDID_205)
#else
#error "Lockbits location unknown"
#endif

/** @endcond */

/***************************************************************************//**
 * @addtogroup Components
 * @{
 * @addtogroup Security
 * @{
 * @addtogroup Tokens
 * @{
 * @brief Manufacturing token handling for the bootloader
 * @details
 ******************************************************************************/

/***************************************************************************//**
 * Get the X component of the ECDSA secp256r1 public key.
 *
 * @return Pointer to X component of the public key
 ******************************************************************************/
const uint8_t* btl_getSignedBootloaderKeyXPtr(void);

/***************************************************************************//**
 * Get the Y component of the ECDSA secp256r1 public key.
 *
 * @return Pointer to Y component of the public key
 ******************************************************************************/
const uint8_t* btl_getSignedBootloaderKeyYPtr(void);

/***************************************************************************//**
 * Get the AES-CCM encryption key.
 *
 * @return Pointer to the AES-CCM key
 ******************************************************************************/
const uint8_t* btl_getImageFileEncryptionKeyPtr(void);

/** @} addtogroup Tokens */
/** @} addtogroup Security */
/** @} addtogroup Components */

#endif // BTL_SECURITY_TOKENS_H
