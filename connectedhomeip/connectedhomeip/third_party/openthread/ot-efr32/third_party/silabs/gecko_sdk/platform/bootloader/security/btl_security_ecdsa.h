/***************************************************************************//**
 * @file
 * @brief ECDSA signing functionality for Silicon Labs bootloader
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
#ifndef BTL_SECURITY_ECDSA_H
#define BTL_SECURITY_ECDSA_H

#include <stdint.h>
#include <stdbool.h>
#include "api/btl_errorcode.h"

/***************************************************************************//**
 * @addtogroup Components
 * @{
 * @addtogroup Security
 * @{
 * @addtogroup ECDSA
 * @{
 * @brief ECDSA signing functionality for the bootloader
 * @details
 ******************************************************************************/

/// Number of bytes of data to verify the signature against
#define BTL_SECURITY_ECDSA_SHA256_LENGTH    32
/// Number of bytes in the EC points that the signature consists of
#define BTL_SECURITY_ECDSA_POINT_LENGTH     32

/***************************************************************************//**
 * Verify an ECDSA signature of a SHA256-hash using secp256r1.
 *
 * Verifies the authenticity of data by checking the ECDSA signature of the
 * data's SHA256-hash. This function is only for use with the secp256r1
 * curve. The public key which the signature is validated against will be
 * retrieved from the respective tokens in the lockbits-page.
 *
 * @param sha256     The hash of the data which is authenticated
 * @param signatureR Byte array (MSB first) of R-point of the ECDSA signature
 * @param signatureS Byte array (MSB first) of S-point of the ECDSA signature
 * @param keyX       Pointer to the X coordinate of the ECDSA public key
 * @param keyY       Pointer to the Y coordinate of the ECDSA public key
 * @return @ref BOOTLOADER_OK if signature is valid, else error code
 *         in @ref BOOTLOADER_ERROR_SECURITY_BASE range.
 ******************************************************************************/
int32_t btl_verifyEcdsaP256r1(const uint8_t *sha256,
                              const uint8_t *signatureR,
                              const uint8_t *signatureS,
                              const uint8_t *keyX,
                              const uint8_t *keyY);

/** @} addtogroup ECDSA */
/** @} addtogroup Security */
/** @} addtogroup Components */
#endif // BTL_SECURITY_ECDSA_H
