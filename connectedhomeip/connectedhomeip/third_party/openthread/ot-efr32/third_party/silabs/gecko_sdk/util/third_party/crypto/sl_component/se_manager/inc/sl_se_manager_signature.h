/***************************************************************************//**
 * @file
 * @brief Silicon Labs Secure Engine Manager API.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
#ifndef SL_SE_MANAGER_SIGNATURE_H
#define SL_SE_MANAGER_SIGNATURE_H

#include "em_device.h"

#if defined(SEMAILBOX_PRESENT) || defined(DOXYGEN)

/// @addtogroup sl_se_manager
/// @{

/***************************************************************************//**
 * @addtogroup sl_se_manager_signature Signature
 *
 * @brief
 *   Digital Signature Algorithms (ECDSA, EdDSA).
 *
 * @details
 *   API for using digital signatures with the SE.
 *
 * @{
 ******************************************************************************/

#include "sl_se_manager_key_handling.h"
#include "sl_se_manager_types.h"
#include "em_se.h"
#include "sl_status.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// Prototypes

/***************************************************************************//**
 * @brief
 *   This function computes Elliptic-Curve Cryptography (ECC) digital signatures
 *   of a message.
 *
 * @note
 *   Edwards-curve Digital Signature Algorithm (EdDSA) generates a message digest
 *   using the input message for computing signatures. The input parameters
 *   \p hash_alg and \p hashed_message do not apply for EdDSA.
 *
 *   P-521 Elliptic Curve for Elliptic Curve Digital Signature Algorithm (ECDSA)
 *   expects a 544 bits (68 bytes) buffer for storing private keys,
 *   and a 1088 bits (136 bytes) buffer for storing public keys and signatures.
 *   The first 23 bits of d, Qx, Qy, R and S are padding bits to comply
 *   word-aligned addressing.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] key
 *   Pointer to sl_se_key_descriptor_t structure.
 *
 * @param[in] hash_alg
 *   Which hashing algorithm to use. Ignored for EdDSA keys, since EdDSA always
 *   uses SHA-512 for Ed25519 and SHA-3 for Ed448.
 *
 * @param[in] hashed_message
 *   The input message is a message digest. Ignored for EdDSA keys, and treated
 *   as false.
 *
 * @param[in] message
 *   The message to be used to compute the signature.
 *
 * @param[in] message_len
 *   The length of message.
 *
 * @param[out] signature
 *   The computed signature.
 *
 * @param[in] signature_len
 *   The length of the computed signature.
 *
 * @return
 *   SL_STATUS_OK when the command was executed successfully, otherwise an
 *   appropiate error code (@ref sl_status.h).
 ******************************************************************************/
sl_status_t sl_se_ecc_sign(sl_se_command_context_t *cmd_ctx,
                           const sl_se_key_descriptor_t *key,
                           sl_se_hash_type_t hash_alg,
                           bool hashed_message,
                           const unsigned char *message,
                           size_t message_len,
                           unsigned char *signature,
                           size_t signature_len);

/***************************************************************************//**
 * @brief
 *   This function verifies Elliptic-Curve Cryptography (ECC) digital signatures
 *   of a message.
 *
 * @note
 *   The input parameters \p hash_alg and \p hashed_message do not apply for
 *   Edwards-curve Digital Signature Algorithm (EdDSA).
 *
 *   P-521 Elliptic Curve for Elliptic Curve Digital Signature Algorithm (ECDSA)
 *   expects a 544 bits (68 bytes) buffer for storing private keys,
 *   and a 1088 bits (136 bytes) buffer for storing public keys and signatures.
 *   The first 23 bits of d, Qx, Qy, R and S are padding bits to comply
 *   word-aligned addressing.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] key
 *   Pointer to sl_se_key_descriptor_t structure.
 *
 * @param[in] hash_alg
 *   Which hashing algorithm to use. Ignored for EdDSA keys, since EdDSA always
 *   uses SHA-512 for Ed25519 and SHA-3 for Ed448.
 *
 * @param[in] hashed_message
 *   The input message is a message digest. Ignored for EdDSA keys, and treated
 *   as false.
 *
 * @param[in] message
 *   The message to be used to compute signatures.
 *
 * @param[in] message_len
 *   The length of message.
 *
 * @param[in] signature
 *   The signature to be verified.
 *
 * @param[in] signature_len
 *   The length of signature.
 *
 * @return
 *   SL_STATUS_OK if the signature is successfully verified, otherwise an
 *   appropiate error code (@ref sl_status.h).
 ******************************************************************************/
sl_status_t sl_se_ecc_verify(sl_se_command_context_t *cmd_ctx,
                             const sl_se_key_descriptor_t *key,
                             sl_se_hash_type_t hash_alg,
                             bool hashed_message,
                             const unsigned char *message,
                             size_t message_len,
                             const unsigned char *signature,
                             size_t signature_len);

#ifdef __cplusplus
}
#endif

/// @} (end addtogroup sl_se_manager_signature)
/// @} (end addtogroup sl_se_manager)

#endif // defined(SEMAILBOX_PRESENT)

#endif // SL_SE_MANAGER_SIGNATURE_H
