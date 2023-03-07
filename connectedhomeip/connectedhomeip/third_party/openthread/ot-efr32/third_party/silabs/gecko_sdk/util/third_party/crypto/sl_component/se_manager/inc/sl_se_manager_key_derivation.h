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
#ifndef SL_SE_MANAGER_KEY_DERIVATION_H
#define SL_SE_MANAGER_KEY_DERIVATION_H

#include "em_device.h"

#if defined(SEMAILBOX_PRESENT) || defined(DOXYGEN)

/// @addtogroup sl_se_manager
/// @{

/***************************************************************************//**
 * @addtogroup sl_se_manager_key_derivation Key derivation
 *
 * @brief
 *   API for key derivation and key agreement (ECDH, EC J-PAKE, HKDF, PBKDF2).
 *
 * @details
 *   Contains key derivation functions (HKDF, PBKDF2) and key agreement
 *   functions (ECDH, ECJPAKE).
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

// -------------------------------
// Elliptic-curve Diffie–Hellman

/***************************************************************************//**
 * @brief
 *   This function computes the shared secret with Elliptic Curve Diffie Hellman
 *   (ECDH) algorithm
 *
 * @details
 *   Performs Elliptic Curve Diffie Hellman shared secret computation.
 *
 * @note
 *   P-521 Elliptic Curve based Elliptic Curve Diffie Hellman (ECDH) expects
 *   a 544 bits (68 bytes) buffer for storing private keys, and
 *   a 1088 bits (136 bytes) buffer for storing public keys and shared secret.
 *   The first 23 bits of d, Qx, Qy and shared secret are padding bits to comply
 *   word-aligned addressing. The padding bits are ignored in the computation.
 *
 *   This function does not implement the value-checking of the shared secret
 *   as described in RFC7748 when using Montgomery keys.
 *
 *   In case of using custom domain curves, \p key_in_priv defines the domain
 *   parameters. Moreover, \p key_in_pub should always contain a public key.
 *   If key_in_pub contains a private key, sl_se_export_public_key() can be
 *   used to export the public key.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] key_in_priv
 *   Our private key.
 *
 * @param[in] key_in_pub
 *   Their public key.
 *
 * @param[out] key_out
 *   Shared secret key. Montgomery curve result is one single coordinate.
 *   Other curve types result in one pair of coordinate.
 *
 * @return
 *   SL_STATUS_INVALID_KEY if \p key_in_pub does not contain a public key
 *   when using custom domain curves, otherwise an appropriate error code
 *   (@ref sl_status.h).
 ******************************************************************************/
sl_status_t sl_se_ecdh_compute_shared_secret(sl_se_command_context_t *cmd_ctx,
                                             const sl_se_key_descriptor_t *key_in_priv,
                                             const sl_se_key_descriptor_t *key_in_pub,
                                             const sl_se_key_descriptor_t *key_out);

// -------------------------------
// EC J-PAKE

/***************************************************************************//**
 * @brief
 *   Check if an EC J-PAKE context is ready for use.
 *
 * @param[in] ctx
 *   The EC J-PAKE context to check. This must be initialized.
 *
 * @return
 *   SL_STATUS_OK when the command was executed successfully, otherwise an
 *   appropriate error code (@ref sl_status.h).
 ******************************************************************************/
sl_status_t sl_se_ecjpake_check(const sl_se_ecjpake_context_t *ctx);

/***************************************************************************//**
 * @brief
 *   Derive the shared secret (TLS: Pre-Master Secret).
 *
 * @param[in] ctx
 *   The EC J-PAKE context to use. This must be initialized, set up and have
 *   performed both round one and two.
 *
 * @param[out] buf
 *   The buffer to write the derived secret to. This must be a writable buffer
 *   of length @p len bytes.
 *
 * @param[in] len
 *   The length of @p buf in bytes.
 *
 * @param[out] olen
 *   The address at which to store the total number of bytes written to @p buf.
 *   This must not be @c NULL.
 *
 * @return
 *   SL_STATUS_OK when the command was executed successfully, otherwise an
 *   appropriate error code (@ref sl_status.h).
 ******************************************************************************/
sl_status_t sl_se_ecjpake_derive_secret(sl_se_ecjpake_context_t *ctx,
                                        unsigned char *buf,
                                        size_t len,
                                        size_t *olen);

/***************************************************************************//**
 * @brief
 *   This clears an EC J-PAKE context and frees any embedded data structure.
 *
 * @param[in] ctx
 *   The EC J-PAKE context to free. This may be @c NULL, in which case this
 *   function does nothing. If it is not @c NULL, it must point to an
 *   initialized EC J-PAKE context.
 *
 * @return
 *   SL_STATUS_OK when the command was executed successfully, otherwise an
 *   appropriate error code (@ref sl_status.h).
 ******************************************************************************/
sl_status_t sl_se_ecjpake_free(sl_se_ecjpake_context_t *ctx);

/***************************************************************************//**
 * @brief
 *   Initialize an EC J-PAKE context.
 *
 * @param[in] ctx
 *   The EC J-PAKE context to initialize. This must not be @c NULL.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @return
 *   SL_STATUS_OK when the command was executed successfully, otherwise an
 *   appropriate error code (@ref sl_status.h).
 ******************************************************************************/
sl_status_t sl_se_ecjpake_init(sl_se_ecjpake_context_t *ctx,
                               sl_se_command_context_t *cmd_ctx);

/***************************************************************************//**
 * @brief
 *   Read and process the first round message (TLS: contents of the
 *   Client/ServerHello extension, excluding extension type and length bytes).
 *
 * @param[in] ctx
 *   The EC J-PAKE context to use. This must be initialized and set up.
 *
 * @param[in] buf
 *   The buffer holding the first round message. This must be a readable buffer
 *   of length @p len bytes.
 *
 * @param[in] len
 *   The length in bytes of @p buf.
 *
 * @return
 *   SL_STATUS_OK when the command was executed successfully, otherwise an
 *   appropriate error code (@ref sl_status.h).
 ******************************************************************************/
sl_status_t sl_se_ecjpake_read_round_one(sl_se_ecjpake_context_t *ctx,
                                         const unsigned char *buf,
                                         size_t len);

/***************************************************************************//**
 * @brief
 *   Read and process the second round message (TLS: contents of the
 *   Client/ServerKeyExchange).
 *
 * @param[in] ctx
 *   The EC J-PAKE context to use. This must be initialized and set up and already
 *   have performed round one.
 *
 * @param[in] buf
 *   The buffer holding the second round message. This must be a readable buffer
 *   of length @p len bytes.
 *
 * @param[in] len
 *   The length in bytes of @p buf.
 *
 * @return
 *   SL_STATUS_OK when the command was executed successfully, otherwise an
 *   appropriate error code (@ref sl_status.h).
 ******************************************************************************/
sl_status_t sl_se_ecjpake_read_round_two(sl_se_ecjpake_context_t *ctx,
                                         const unsigned char *buf,
                                         size_t len);

/***************************************************************************//**
 * @brief
 *   Set up an EC J-PAKE context for use.
 *
 * @note
 *   Currently the only values for hash/curve allowed by the standard are
 *   @ref SL_SE_HASH_SHA256 / @ref SL_SE_KEY_TYPE_ECC_P256.
 *
 * @param[in] ctx
 *   The EC J-PAKE context to set up. This must be initialized.
 *
 * @param[in] role
 *   The role of the caller. This must be either @ref SL_SE_ECJPAKE_CLIENT or
 *   @ref SL_SE_ECJPAKE_SERVER.
 *
 * @param[in] hash
 *   The identifier of the hash function to use, for example
 *   @ref SL_SE_HASH_SHA256.
 *
 * @param[in] curve
 *   The identifier of the elliptic curve to use, for example
 *   @ref SL_SE_KEY_TYPE_ECC_P256.
 *
 * @param[in] secret
 *   The pre-shared secret (passphrase). This must be a readable buffer of
 *   length @p len bytes. It need only be valid for the duration of this call.
 *
 * @param[in] len
 *   The length of the pre-shared secret @p secret.
 *
 * @return
 *   SL_STATUS_OK when the command was executed successfully, otherwise an
 *   appropriate error code (@ref sl_status.h).
 ******************************************************************************/
sl_status_t sl_se_ecjpake_setup(sl_se_ecjpake_context_t *ctx,
                                sl_se_ecjpake_role_t role,
                                sl_se_hash_type_t hash,
                                uint32_t curve,
                                const unsigned char *secret,
                                size_t len);

/***************************************************************************//**
 * @brief
 *   Generate and write the first round message (TLS: contents of the
 *   Client/ServerHello extension, excluding extension type and length bytes).
 *
 * @param[in] ctx
 *   The EC J-PAKE context to use. This must be initialized and set up.
 *
 * @param[out] buf
 *   The buffer to write the contents to. This must be a writable buffer of
 *   length @p len bytes.
 *
 * @param[in] len
 *   The length of @p buf in bytes.
 *
 * @param[out] olen
 *   The address at which to store the total number of bytes written to @p buf.
 *   This must not be @c NULL.
 *
 * @return
 *   SL_STATUS_OK when the command was executed successfully, otherwise an
 *   appropriate error code (@ref sl_status.h).
 ******************************************************************************/
sl_status_t sl_se_ecjpake_write_round_one(sl_se_ecjpake_context_t *ctx,
                                          unsigned char *buf,
                                          size_t len,
                                          size_t *olen);

/***************************************************************************//**
 * @brief
 *   Generate and write the second round message (TLS: contents of the
 *   Client/ServerKeyExchange).
 *
 * @param[in] ctx
 *   The EC J-PAKE context to use. This must be initialized, set up, and already
 *   have performed round one.
 *
 * @param[out] buf
 *   The buffer to write the round two contents to. This must be a writable
 *   buffer of length @p len bytes.
 *
 * @param[in] len
 *   The size of @p buf in bytes.
 *
 * @param[out] olen
 *   The address at which to store the total number of bytes written to @p buf.
 *   This must not be @c NULL.
 *
 * @return
 *   SL_STATUS_OK when the command was executed successfully, otherwise an
 *   appropriate error code (@ref sl_status.h).
 ******************************************************************************/
sl_status_t sl_se_ecjpake_write_round_two(sl_se_ecjpake_context_t *ctx,
                                          unsigned char *buf,
                                          size_t len,
                                          size_t *olen);

// -------------------------------
// Key derivation functions

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT) || defined(DOXYGEN)
/***************************************************************************//**
 * @brief
 *   Derive a pseudorandom key from the input key material using HKDF.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] in_key
 *   Pointer to the input key material.
 *
 * @param[in] hash
 *   Which hashing algorithm to use.
 *
 * @param[in] salt
 *   An optional salt value (a non-secret random value).
 *
 * @param[in] salt_len
 *   The length of the salt.
 *
 * @param[in] info
 *   An optional context and application specific information string.
 *
 * @param[in] info_len
 *   The length of info.
 *
 * @param[in,out] out_key
 *   Pointer to the generated key material. The length member of out_key is
 *   used to request a given length of the generated key.
 *
 * @return
 *   SL_STATUS_OK if the signature is successfully verified, otherwise an
 *   appropriate error code (@ref sl_status.h).
 ******************************************************************************/
sl_status_t sl_se_derive_key_hkdf(sl_se_command_context_t *cmd_ctx,
                                  const sl_se_key_descriptor_t *in_key,
                                  sl_se_hash_type_t hash,
                                  const unsigned char *salt,
                                  size_t salt_len,
                                  const unsigned char *info,
                                  size_t info_len,
                                  sl_se_key_descriptor_t *out_key);

/***************************************************************************//**
 * @brief
 *   Derive a pseudorandom key from the input key material using PBKDF2.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] in_key
 *   Pointer to the input key material.
 *
 * @param[in] prf
 *   The underlying psuedorandom function (PRF) to use in the algorithm. The
 *   most common choice of HMAC-SHA-{1, 224, 256, 384, 512} is supported on all
 *   Series-2 devices (with Vault High Security). Newer chips, EFR32xG23 and
 *   later, also support usage of AES-CMAC-PRF-128.
 *
 * @param[in] salt
 *   An optional salt value (a non-secret random value).
 *
 * @param[in] salt_len
 *   The length of the salt.
 *
 * @param[in] iterations
 *   The number of iterations to use. Up to 16384 iterations is supported.
 *
 * @param[in,out] out_key
 *   Pointer to the generated key material. The length member of out_key is
 *   used to request a given length of the generated key.
 *
 * @return
 *   SL_STATUS_OK if the signature is successfully verified, otherwise an
 *   appropriate error code (@ref sl_status.h).
 ******************************************************************************/
sl_status_t sl_se_derive_key_pbkdf2(sl_se_command_context_t *cmd_ctx,
                                    const sl_se_key_descriptor_t *in_key,
                                    sl_se_pbkdf2_prf_type_t prf,
                                    const unsigned char *salt,
                                    size_t salt_len,
                                    uint32_t iterations,
                                    sl_se_key_descriptor_t *out_key);

#endif // (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)

#ifdef __cplusplus
}
#endif

/// @} (end addtogroup sl_se_manager_key_derivation)
/// @} (end addtogroup sl_se_manager)

#endif // defined(SEMAILBOX_PRESENT)

#endif // SL_SE_MANAGER_KEY_DERIVATION_H
