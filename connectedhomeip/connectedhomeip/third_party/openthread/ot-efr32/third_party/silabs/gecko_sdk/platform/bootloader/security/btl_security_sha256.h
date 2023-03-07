/***************************************************************************//**
 * @file
 * @brief SHA-256 digest functionality for Silicon Labs bootloader.
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
#ifndef BTL_SECURITY_SHA256_H
#define BTL_SECURITY_SHA256_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/***************************************************************************//**
 * @addtogroup Components
 * @{
 * @addtogroup Security
 * @{
 * @addtogroup SHA_256
 * @{
 * @brief SHA-256 digest functionality for the bootloader
 * @details
 ******************************************************************************/

/// Number of bytes in a SHA-256 digest
#define BTL_SECURITY_SHA256_DIGEST_LENGTH   32

/***************************************************************************//**
 * Initialize SHA256 context variable.
 *
 * @param ctx Pointer to the SHA256 context variable to be initialized
 *
 * Wipes out the SHA256 context variable and sets it up for re-use.
 ******************************************************************************/
void btl_initSha256(void *ctx);

/***************************************************************************//**
 * Run data through the SHA256 hashing function.
 *
 * @param ctx  Pointer to the SHA256 context variable
 * @param data Pointer to an array of binary data to add to the SHA256
 *   calculation in progress
 * @param length Length of the byte array with data.
 *
 ******************************************************************************/
void btl_updateSha256(void *ctx, const void *data, size_t length);

/***************************************************************************//**
 * Finalize the SHA256 calculation.
 *
 * @param ctx Pointer to the SHA256 context variable to be initialized
 *
 * Finalizes the running SHA256 calculation. After finalization, the SHA value
 *   in the context variable will be valid and no more data can be added.
 ******************************************************************************/
void btl_finalizeSha256(void *ctx);

/***************************************************************************//**
 * Compare the SHA256 from the context variable to a known value.
 *
 * @param ctx    Pointer to the SHA256 context variable to be initialized
 * @param sha256 Byte array containing SHA256 value to compare to
 * @return @ref BOOTLOADER_OK if both hash values are equal, else error code
 *         from @ref BOOTLOADER_ERROR_SECURITY_BASE range.
 *
 * Convenience function to compare a known SHA256 hash against the result of
 *   a calculation.
 ******************************************************************************/
int32_t btl_verifySha256(void *ctx, const void *sha256);

/** @} addtogroup SHA_256 */
/** @} addtogroup Security */
/** @} addtogroup Components */
#endif // BTL_SECURITY_SHA256_H
