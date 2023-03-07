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
#ifndef SL_SE_MANAGER_HASH_H
#define SL_SE_MANAGER_HASH_H

#include "em_device.h"

#if defined(SEMAILBOX_PRESENT) || defined(DOXYGEN)

/// @addtogroup sl_se_manager
/// @{

/***************************************************************************//**
 * @addtogroup sl_se_manager_hash Hashing
 *
 * @brief
 *   Provides cryptographic hash functions (SHA-1, SHA-224, SHA-256, SHA-384,
 *   SHA-512).
 *
 * @details
 *   Provides API for one-way hashing functions.
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
 *   Produce a message digest (a hash block) using the input data.
 *
 * @details
 *   This function generates a message digest adhering to the given inputs.
 *   For instance, if the algorithm is chosen to be SHA-256, it will generate
 *   a 32 bytes message digest computed based on the input message.
 *   This function supports SHA-1, SHA-256 and SHA-512 algorithms.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] hash_type
 *   Which hashing algorithm to use.
 *
 * @param[in] message
 *   Pointer to the message buffer to compute the hash/digest from.
 *
 * @param[in] message_size
 *   Number of bytes in message.
 *
 * @param[out] digest
 *   Pointer to block of memory to store the final digest.
 *
 * @param[in]  digest_len
 *   The length of the message digest (hash), must be at least the size of the
 *   corresponding hash type.
 *
 * @return Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_hash(sl_se_command_context_t *cmd_ctx,
                       sl_se_hash_type_t hash_type,
                       const uint8_t *message,
                       unsigned int message_size,
                       uint8_t* digest,
                       size_t digest_len);

/***************************************************************************//**
 * @brief
 *   Prepare a SHA1 hash streaming command context object.
 *
 * @details
 *   Prepare a SHA1 hash streaming command context object to be used in
 *   subsequent calls to hash streaming functions sl_se_hash_multipart_update() and
 *   sl_se_hash_multipart_finish().
 *
 * @param[in] sha1_ctx
 *   Pointer to a SHA1 streaming context object.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_hash_sha1_multipart_starts(sl_se_sha1_multipart_context_t *sha1_ctx,
                                             sl_se_command_context_t *cmd_ctx);

/***************************************************************************//**
 * @brief
 *   Deprecated, please switch to using \ref sl_se_hash1_multipart_starts().
 *
 *   Prepare a SHA1 hash streaming command context object.
 *
 * @details
 *   Prepare a SHA1 hash streaming command context object to be used in
 *   subsequent calls to hash streaming functions sl_se_hash_update() and
 *   sl_se_hash_finish().
 *
 * @param[in] hash_ctx
 *   Pointer to a generic hash streaming context object.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] sha1_ctx
 *   Pointer to a SHA1 streaming context object.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_hash_sha1_starts(sl_se_hash_streaming_context_t *hash_ctx,
                                   sl_se_command_context_t *cmd_ctx,
                                   sl_se_sha1_streaming_context_t *sha1_ctx) SL_DEPRECATED_API_SDK_3_3;

/***************************************************************************//**
 * @brief
 *   Prepare a SHA224 hash streaming command context object.
 *
 * @details
 *   Prepare a SHA224 hash streaming command context object to be used in
 *   subsequent calls to hash streaming functions sl_se_hash_multipart_update() and
 *   sl_se_hash_multipart_finish().
 *
 * @param[in] sha224_ctx
 *   Pointer to a SHA224 streaming context object.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_hash_sha224_multipart_starts(sl_se_sha224_multipart_context_t *sha224_ctx,
                                               sl_se_command_context_t *cmd_ctx);

/***************************************************************************//**
 * @brief
 *   Deprecated, please switch to using \ref sl_se_hash224_multipart_starts().
 *
 *   Prepare a SHA224 hash streaming command context object.
 *
 * @details
 *   Prepare a SHA224 hash streaming command context object to be used in
 *   subsequent calls to hash streaming functions sl_se_hash_update() and
 *   sl_se_hash_finish().
 *
 * @param[in] hash_ctx
 *   Pointer to a generic hash streaming context object.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] sha224_ctx
 *   Pointer to a SHA224 streaming context object.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_hash_sha224_starts(sl_se_hash_streaming_context_t *hash_ctx,
                                     sl_se_command_context_t *cmd_ctx,
                                     sl_se_sha224_streaming_context_t *sha224_ctx) SL_DEPRECATED_API_SDK_3_3;

/***************************************************************************//**
 * @brief
 *   Prepare a SHA256 hash streaming command context object.
 *
 * @details
 *   Prepare a SHA256 hash streaming command context object to be used in
 *   subsequent calls to hash streaming functions sl_se_hash_multipart_update() and
 *   sl_se_hash_multipart_finish().
 *
 * @param[in] sha256_ctx
 *   Pointer to a SHA256 streaming context object.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_hash_sha256_multipart_starts(sl_se_sha256_multipart_context_t *sha256_ctx,
                                               sl_se_command_context_t *cmd_ctx);

/***************************************************************************//**
 * @brief
 *   Deprecated, please switch to using \ref sl_se_hash256_multipart_starts().
 *
 *   Prepare a SHA256 hash streaming command context object.
 *
 * @details
 *   Prepare a SHA256 hash streaming command context object to be used in
 *   subsequent calls to hash streaming functions sl_se_hash_update() and
 *   sl_se_hash_finish().
 *
 * @param[in] hash_ctx
 *   Pointer to a generic hash streaming context object.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] sha256_ctx
 *   Pointer to a SHA256 streaming context object.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_hash_sha256_starts(sl_se_hash_streaming_context_t *hash_ctx,
                                     sl_se_command_context_t *cmd_ctx,
                                     sl_se_sha256_streaming_context_t *sha256_ctx) SL_DEPRECATED_API_SDK_3_3;

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT) || defined(DOXYGEN)
/***************************************************************************//**
 * @brief
 *   Prepare a SHA384 streaming command context object.
 *
 * @details
 *   Prepare a SHA384 hash streaming command context object to be used in
 *   subsequent calls to hash streaming functions sl_se_hash_multipart_update() and
 *   sl_se_hash_multipart_finish().
 *
 * @param[in] sha384_ctx
 *   Pointer to a SHA384 streaming context object.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_hash_sha384_multipart_starts(sl_se_sha384_multipart_context_t *sha384_ctx,
                                               sl_se_command_context_t *cmd_ctx);

/***************************************************************************//**
 * @brief
 *   Deprecated, please switch to using \ref sl_se_hash384_multipart_starts().
 *
 *   Prepare a SHA384 streaming command context object.
 *
 * @details
 *   Prepare a SHA384 hash streaming command context object to be used in
 *   subsequent calls to hash streaming functions sl_se_hash_update() and
 *   sl_se_hash_finish().
 *
 * @param[in] hash_ctx
 *   Pointer to a generic hash streaming context object.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] sha384_ctx
 *   Pointer to a SHA384 streaming context object.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_hash_sha384_starts(sl_se_hash_streaming_context_t *hash_ctx,
                                     sl_se_command_context_t *cmd_ctx,
                                     sl_se_sha384_streaming_context_t *sha384_ctx) SL_DEPRECATED_API_SDK_3_3;

/***************************************************************************//**
 * @brief
 *   Prepare a SHA512 streaming command context object.
 *
 * @details
 *   Prepare a SHA512 hash streaming command context object to be used in
 *   subsequent calls to hash streaming functions sl_se_hash_multipart_update() and
 *   sl_se_hash_multipart_finish().
 *
 * @param[in] sha512_ctx
 *   Pointer to a SHA512 streaming context object.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_hash_sha512_multipart_starts(sl_se_sha512_multipart_context_t *sha512_ctx,
                                               sl_se_command_context_t *cmd_ctx);

/***************************************************************************//**
 * @brief
 *   Deprecated, please switch to using \ref sl_se_hash512_multipart_starts().
 *
 *   Prepare a SHA512 streaming command context object.
 *
 * @details
 *   Prepare a SHA512 hash streaming command context object to be used in
 *   subsequent calls to hash streaming functions sl_se_hash_update() and
 *   sl_se_hash_finish().
 *
 * @param[in] hash_ctx
 *   Pointer to a generic hash streaming context object.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] sha512_ctx
 *   Pointer to a SHA512 streaming context object.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_hash_sha512_starts(sl_se_hash_streaming_context_t *hash_ctx,
                                     sl_se_command_context_t *cmd_ctx,
                                     sl_se_sha512_streaming_context_t *sha512_ctx) SL_DEPRECATED_API_SDK_3_3;
#endif // (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)

/***************************************************************************//**
 * @brief
 *   Prepare a hash streaming command context object.
 *
 * @details
 *   Prepare a hash (message digest) streaming command context object to be
 *   used in subsequent calls to hash streaming functions sl_se_hash_multipart_update()
 *   and sl_se_hash_multipart_finish().
 *
 * @param[in] hash_type_ctx
 *   Pointer to a hash streaming context object specific to the hash type
 *   specified by @p hash_type.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] hash_type
 *   Type of hash algoritm
 *
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_hash_multipart_starts(void *hash_type_ctx,
                                        sl_se_command_context_t *cmd_ctx,
                                        sl_se_hash_type_t hash_type);

/***************************************************************************//**
 * @brief
 *   Deprecated, please switch to using \ref sl_se_hash_multipart_starts().
 *
 *   Prepare a hash streaming command context object.
 *
 * @details
 *   Prepare a hash (message digest) streaming command context object to be
 *   used in subsequent calls to hash streaming functions sl_se_hash_update()
 *   and sl_se_hash_finish().
 *
 * @param[in] hash_ctx
 *   Pointer to a generic hash streaming context object.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] hash_type
 *   Type of hash algoritm
 *
 * @param[in] hash_type_ctx
 *   Pointer to a hash streaming context object specific to the hash type
 *   specified by @p hash_type.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_hash_starts(sl_se_hash_streaming_context_t *hash_ctx,
                              sl_se_command_context_t *cmd_ctx,
                              sl_se_hash_type_t hash_type,
                              void *hash_type_ctx) SL_DEPRECATED_API_SDK_3_3;

/***************************************************************************//**
 * @brief
 *   Feeds an input buffer into an ongoing hash computation.
 *
 *   This function is called between @ref sl_se_hash_multipart_starts() and
 *   @ref sl_se_hash_multipart_finish().
 *   This function can be called repeatedly.
 *
 * @param[in] hash_type_ctx
 *   Pointer to a hash streaming context object specific to the hash type
 *   specified by @p hash_type.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] input
 *   Buffer holding the input data, must be at least @p ilen bytes wide.
 *
 * @param[in] input_len
 *   The length of the input data in bytes.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_hash_multipart_update(void *hash_type_ctx,
                                        sl_se_command_context_t *cmd_ctx,
                                        const uint8_t *input,
                                        size_t input_len);

/***************************************************************************//**
 * @brief
 *   Deprecated, please switch to using \ref sl_se_hash_multipart_update().
 *
 *   Feeds an input buffer into an ongoing hash computation.
 *
 *   This function is called between @ref sl_se_hash_starts() and
 *   @ref sl_se_hash_finish().
 *   This function can be called repeatedly.
 *
 * @param[in] hash_ctx
 *   Pointer to a generic hash streaming context object.
 *
 * @param[in] input
 *   Buffer holding the input data, must be at least @p ilen bytes wide.
 *
 * @param[in] input_len
 *   The length of the input data in bytes.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_hash_update(sl_se_hash_streaming_context_t *hash_ctx,
                              const uint8_t *input,
                              size_t input_len) SL_DEPRECATED_API_SDK_3_3;

/***************************************************************************//**
 * @brief
 *   Finish a hash streaming operation and return the resulting hash digest.
 *
 *   This function is called after sl_se_hash_multipart_update().
 *
 * @param[in] hash_type_ctx
 *   Pointer to a hash streaming context object specific to the hash type
 *   specified by @p hash_type.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[out] digest_out
 *   Buffer for holding the message digest (hash), must be at least the size
 *   of the corresponding message digest type.
 *
 * @param[in]  digest_len
 *   The length of the message digest (hash), must be at least the size of the
 *   corresponding hash type.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_hash_multipart_finish(void *hash_type_ctx,
                                        sl_se_command_context_t *cmd_ctx,
                                        uint8_t *digest_out,
                                        size_t digest_len);

/***************************************************************************//**
 * @brief
 *   Deprecated, please switch to using \ref sl_se_hash_multipart_finish().
 *
 *   Finish a hash streaming operation and return the resulting hash digest.
 *
 *   This function is called after sl_se_hash_update().
 *
 * @param[in] hash_ctx
 *   Pointer to a generic hash streaming context object.
 *
 * @param[out] digest_out
 *   Buffer for holding the message digest (hash), must be at least the size
 *   of the corresponding message digest type.
 *
 * @param[in]  digest_len
 *   The length of the message digest (hash), must be at least the size of the
 *   corresponding hash type.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_hash_finish(sl_se_hash_streaming_context_t *hash_ctx,
                              uint8_t *digest_out,
                              size_t digest_len) SL_DEPRECATED_API_SDK_3_3;

#ifdef __cplusplus
}
#endif

/// @} (end addtogroup sl_se_manager_hash)
/// @} (end addtogroup sl_se_manager)

#endif // defined(SEMAILBOX_PRESENT)

#endif // SL_SE_MANAGER_HASH_H
