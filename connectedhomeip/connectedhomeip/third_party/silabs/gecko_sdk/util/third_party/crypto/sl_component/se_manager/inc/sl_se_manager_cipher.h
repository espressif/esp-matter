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
#ifndef SL_SE_MANAGER_CIPHER_H
#define SL_SE_MANAGER_CIPHER_H

#include "em_device.h"

#if defined(SEMAILBOX_PRESENT) || defined(DOXYGEN)

/// @addtogroup sl_se_manager
/// @{

/***************************************************************************//**
 * @addtogroup sl_se_manager_cipher Cipher
 *
 * @brief
 *   Symmetric encryption, AEAD and MAC.
 *
 * @details
 *   API for performing symmetric encryption, Authenticated Encryption and
 *   Additional Data (AEAD) operations, and computing Message Authentication
 *   Codes (MACs) using the Secure Engine.
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
 *   AES-ECB block encryption/decryption.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] key
 *   Pointer to sl_se_key_descriptor_t structure.
 *
 * @param[in] mode
 *   Crypto operation type (encryption or decryption).
 *
 * @param[in] length
 *   Length of the input data.
 *
 * @param[in] input
 *   Buffer holding the input data.
 *
 * @param[out] output
 *   Buffer holding the output data.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_aes_crypt_ecb(sl_se_command_context_t *cmd_ctx,
                                const sl_se_key_descriptor_t *key,
                                sl_se_cipher_operation_t mode,
                                size_t length,
                                const unsigned char *input,
                                unsigned char *output);

/***************************************************************************//**
 * @brief
 *   AES-CBC buffer encryption/decryption.
 *
 * @note
 *   Length should be a multiple of the block size (16 bytes).
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] key
 *   Pointer to sl_se_key_descriptor_t structure.
 *
 * @param[in] mode
 *   Crypto operation type (encryption or decryption).
 *
 * @param[in] length
 *   Length of the input data.
 *
 * @param[in,out] iv
 *   Initialization vector (updated after use).
 *
 * @param[in] input
 *   Buffer holding the input data.
 *
 * @param[out] output
 *   Buffer holding the output data.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_aes_crypt_cbc(sl_se_command_context_t *cmd_ctx,
                                const sl_se_key_descriptor_t *key,
                                sl_se_cipher_operation_t mode,
                                size_t length,
                                unsigned char iv[16],
                                const unsigned char *input,
                                unsigned char *output);

/***************************************************************************//**
 * @brief
 *   AES-CFB128 buffer encryption/decryption.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] key
 *   Pointer to sl_se_key_descriptor_t structure.
 *
 * @param[in] mode
 *   Crypto operation type (encryption or decryption).
 *
 * @param[in] length
 *   Length of the input data.
 *
 * @param[in,out] iv_off
 *   Offset in IV (updated after use).
 *
 * @param[in,out] iv
 *   Initialization vector (updated after use).
 *
 * @param[in] input
 *   Buffer holding the input data.
 *
 * @param[out] output
 *   Buffer holding the output data.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_aes_crypt_cfb128(sl_se_command_context_t *cmd_ctx,
                                   const sl_se_key_descriptor_t *key,
                                   sl_se_cipher_operation_t mode,
                                   size_t length,
                                   uint32_t *iv_off,
                                   unsigned char iv[16],
                                   const unsigned char *input,
                                   unsigned char *output);

/***************************************************************************//**
 * @brief
 *   AES-CFB8 buffer encryption/decryption.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] key
 *   Pointer to sl_se_key_descriptor_t structure.
 *
 * @param[in] mode
 *   Crypto operation type (encryption or decryption).
 *
 * @param[in] length
 *   Length of the input data.
 *
 * @param[in,out] iv
 *   Initialization vector (updated after use).
 *
 * @param[in] input
 *   Buffer holding the input data.
 *
 * @param[out] output
 *   Buffer holding the output data.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_aes_crypt_cfb8(sl_se_command_context_t *cmd_ctx,
                                 const sl_se_key_descriptor_t *key,
                                 sl_se_cipher_operation_t mode,
                                 size_t length,
                                 unsigned char iv[16],
                                 const unsigned char *input,
                                 unsigned char *output);

/***************************************************************************//**
 * @brief
 *   AES-CTR buffer encryption/decryption.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] key
 *   Pointer to sl_se_key_descriptor_t structure.
 *
 * @param[in] length
 *   Length of the input data.
 *
 * @param[in] nc_off
 *   The offset in the current stream_block (for resuming
 *   within current cipher stream). The offset pointer to
 *   should be 0 at the start of a stream.
 *
 * @param[in,out] nonce_counter
 *   The 128-bit nonce and counter.
 *
 * @param[in,out] stream_block
 *   The saved stream-block for resuming (updated after use).
 *
 * @param[in] input
 *   Buffer holding the input data.
 *
 * @param[out] output
 *   Buffer holding the output data.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_aes_crypt_ctr(sl_se_command_context_t *cmd_ctx,
                                const sl_se_key_descriptor_t *key,
                                size_t length,
                                uint32_t *nc_off,
                                unsigned char nonce_counter[16],
                                unsigned char stream_block[16],
                                const unsigned char *input,
                                unsigned char *output);

/***************************************************************************//**
 * @brief
 *   AES-CCM buffer encryption.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] key
 *   Pointer to sl_se_key_descriptor_t structure.
 *
 * @param[in] length
 *   The length of the input data in Bytes.
 *
 * @param[in] iv
 *   Initialization vector (nonce).
 *
 * @param[in] iv_len
 *   The length of the IV in Bytes: 7, 8, 9, 10, 11, 12, or 13.
 *
 * @param[in] add
 *   The additional data field.
 *
 * @param[in] add_len
 *   The length of additional data in Bytes.
 *
 * @param[in] input
 *   The buffer holding the input data.
 *
 * @param[out] output
 *   The buffer holding the output data. Must be at least @p length Bytes wide.
 *
 * @param[in,out] tag
 *   The buffer holding the tag.
 *
 * @param[in] tag_len
 *   The length of the tag to generate in Bytes: 4, 6, 8, 10, 12, 14 or 16.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_ccm_encrypt_and_tag(sl_se_command_context_t *cmd_ctx,
                                      const sl_se_key_descriptor_t *key,
                                      size_t length,
                                      const unsigned char *iv, size_t iv_len,
                                      const unsigned char *add, size_t add_len,
                                      const unsigned char *input,
                                      unsigned char *output,
                                      unsigned char *tag, size_t tag_len);

/***************************************************************************//**
 * @brief
 *   AES-CCM buffer decryption.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] key
 *   Pointer to sl_se_key_descriptor_t structure.
 *
 * @param[in] length
 *   The length of the input data in Bytes.
 *
 * @param[in] iv
 *   Initialization vector.
 *
 * @param[in] iv_len
 *   The length of the IV in Bytes: 7, 8, 9, 10, 11, 12, or 13.
 *
 * @param[in] add
 *   The additional data field.
 *
 * @param[in] add_len
 *   The length of additional data in Bytes.
 *
 * @param[in] input
 *   The buffer holding the input data.
 *
 * @param[out] output
 *   The buffer holding the output data. Must be at least @p length Bytes wide.
 *
 * @param[in] tag
 *   The buffer holding the tag.
 *
 * @param[in] tag_len
 *   The length of the tag in Bytes. Must be 4, 6, 8, 10, 12, 14 or 16.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_ccm_auth_decrypt(sl_se_command_context_t *cmd_ctx,
                                   const sl_se_key_descriptor_t *key,
                                   size_t length,
                                   const unsigned char *iv, size_t iv_len,
                                   const unsigned char *add, size_t add_len,
                                   const unsigned char *input,
                                   unsigned char *output,
                                   const unsigned char *tag, size_t tag_len);

/***************************************************************************//**
 * @brief
 *   Prepare a CCM streaming command context object.
 *
 * @details
 *   Prepare a CCM streaming command context object to be used in subsequent
 *   CCM streaming function calls.
 *
 * @param[in] ccm_ctx
 *   Pointer to a CCM streaming context object.
 *
 * @param[in] cmd_ctx
 *   Pointer to a SE command context object.
 *
 * @param[in] key
 *   Pointer to sl_se_key_descriptor_t structure.
 *
 * @param[in] mode
 *   The operation to perform: SL_SE_ENCRYPT or SL_SE_DECRYPT.
 *
 * @param[in] total_message_length
 *   The total length of the text to encrypt/decrypt
 *
 * @param[in] iv
 *   The initialization vector (commonly referred to as nonce for CCM).
 *
 * @param[in] iv_len
 *   The length of the IV.
 *
 * @param[in] add
 *   The buffer holding the additional data.
 *
 * @param[in] add_len
 *   The length of the additional data.
 *
 * @param[in] tag_len
 *   Encryption: The length of the tag to generate. Must be 0, 4, 6, 8, 10, 12, 14 or 16.
 *   Decryption: The length of the tag to authenticate. Must be 0, 4, 6, 8, 10, 12, 14 or 16.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_ccm_multipart_starts(sl_se_ccm_multipart_context_t *ccm_ctx,
                                       sl_se_command_context_t *cmd_ctx,
                                       const sl_se_key_descriptor_t *key,
                                       sl_se_cipher_operation_t mode,
                                       uint32_t total_message_length,
                                       const uint8_t *iv,
                                       size_t iv_len,
                                       const uint8_t *add,
                                       size_t add_len,
                                       size_t tag_len);

/***************************************************************************//**
 * @brief
 *   This function feeds an input buffer into an ongoing CCM computation.
 *
 *   It is called between sl_se_ccm_multipart_starts() and sl_se_ccm_multipart_finish().
 *   Can be called repeatedly.
 *
 * @param[in] ccm_ctx
 *   Pointer to a CCM streaming context object.
 *
 * @param[in] cmd_ctx
 *   Pointer to a SE command context object.
 *
 * @param[in] key
 *   Pointer to sl_se_key_descriptor_t structure.
 *
 * @param[in] length
 *   The length of the input data. This must be a multiple of 16 except in
 *   the last call before sl_se_ccm_multipart_finish().
 *
 * @param[in] input
 *   Buffer holding the input data, must be at least @p length bytes wide.
 *
 * @param[out] output
 *   Buffer for holding the output data, must be at least @p length bytes wide.
 *
 * @param[out] output_length
 *   Length of data that has been encrypted/decrypted.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/

sl_status_t sl_se_ccm_multipart_update(sl_se_ccm_multipart_context_t *ccm_ctx,
                                       sl_se_command_context_t *cmd_ctx,
                                       const sl_se_key_descriptor_t *key,
                                       size_t length,
                                       const uint8_t *input,
                                       uint8_t *output,
                                       size_t *output_length);

/***************************************************************************//**
 * @brief
 *   Finish a CCM streaming operation and return the resulting CCM tag.
 *
 *   It is called after sl_se_ccm_multipart_update().
 *
 * @param[in] ccm_ctx
 *   Pointer to a CCM streaming context object.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] key
 *   Pointer to sl_se_key_descriptor_t structure.
 *
 * @param[in, out] tag
 *   Encryption: The buffer for holding the tag.
 *   Decryption: The tag to authenticate.
 *
 * @param[in]  tag_size
 *   The size of the tag buffer. Must be equal or greater to the length of the expected tag.
 *
 * @param[out] output
 *   Buffer for holding the output data.
 *
 * @param[in] output_size
 *   Output buffer size. Must be equal or greater to the stored data from
 *   sl_se_ccm_multipart_update (maximum 16 bytes).
 *
 * @param[out] output_length
 *   Length of data that has been encrypted/decrypted.
 *
 * @return
 *   Returns SL_SE_INVALID_SIGNATURE if authentication step fails.
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_ccm_multipart_finish(sl_se_ccm_multipart_context_t *ccm_ctx,
                                       sl_se_command_context_t *cmd_ctx,
                                       const sl_se_key_descriptor_t *key,
                                       uint8_t *tag,
                                       uint8_t  tag_size,
                                       uint8_t *output,
                                       uint8_t output_size,
                                       uint8_t *output_length);

/***************************************************************************//**
 * @brief
 *   This function performs GCM encryption or decryption of a buffer.
 *
 * @note
 *   For encryption, the output buffer can be the same as the input buffer.
 *   For decryption, the output buffer cannot be the same as input buffer.
 *   If the buffers overlap, the output buffer must trail at least 8 bytes
 *   behind the input buffer.
 *
 * @warning
 *   When this function performs a decryption, it outputs the authentication
 *   tag and does not verify that the data is authentic. You should use this
 *   function to perform encryption only. For decryption, use
 *   sl_se_gcm_auth_decrypt() instead.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] key
 *   Pointer to sl_se_key_descriptor_t structure.
 *
 * @param[in] mode
 *   Crypto operation type (encryption or decryption).
 *     - SL_SE_ENCRYPT: The ciphertext is written to @p output and the
 *                      authentication tag is written to @p tag.
 *     - SL_SE_DECRYPT: The plaintext is written to @p output and the
 *                      authentication tag is written to @p tag.
 *                      Note that this mode is not recommended, because it does
 *                      not verify the authenticity of the data. For this
 *                      reason, you should use sl_se_gcm_auth_decrypt() instead.
 *
 * @param[in] length
 *   The length of the input data, which is equal to the length of the output
 *   data.
 *
 * @param[in] iv
 *   The initialization vector.
 *
 * @param[in] iv_len
 *   The length of the iv. Must be @b 12 bytes.
 *
 * @param[in] add
 *   The buffer holding the additional data.
 *
 * @param[in] add_len
 *   The length of the additional data in bytes.
 *
 * @param[in] input
 *   The buffer holding the input data. Its size is @b length bytes.
 *
 * @param[out] output
 *   The buffer for holding the output data. It must have room for @b length
 *   bytes.
 *
 * @param[in] tag_len
 *   The length of the tag to generate (in bytes).
 *
 * @param[out] tag
 *   The buffer for holding the tag.
 *
 * @return
 *   SL_STATUS_OK when the command was executed successfully, otherwise an
 *   appropriate error code (@ref sl_status.h).
 ******************************************************************************/
sl_status_t sl_se_gcm_crypt_and_tag(sl_se_command_context_t *cmd_ctx,
                                    const sl_se_key_descriptor_t *key,
                                    sl_se_cipher_operation_t mode,
                                    size_t length,
                                    const unsigned char *iv,
                                    size_t iv_len,
                                    const unsigned char *add,
                                    size_t add_len,
                                    const unsigned char *input,
                                    unsigned char *output,
                                    size_t tag_len,
                                    unsigned char *tag);

/***************************************************************************//**
 * @brief
 *   This function performs a GCM authenticated decryption of a buffer.
 *
 * @note
 *   The output buffer cannot be the same as input buffer. If the buffers
 *   overlap, the output buffer must trail at least 8 bytes behind the input
 *   buffer.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] key
 *   Pointer to sl_se_key_descriptor_t structure.
 *
 * @param[in] length
 *   The length of the ciphertext to decrypt, which is also the length of the
 *   decrypted plaintext.
 *
 * @param[in] iv
 *   The initialization vector.
 *
 * @param[in] iv_len
 *   The length of the iv. Must be @b 12 bytes.
 *
 * @param[in] add
 *   The buffer holding the additional data.
 *
 * @param[in] add_len
 *   The length of the additional data in bytes.
 *
 * @param[in] tag
 *   The buffer holding the tag to verify.
 *
 * @param[in] tag_len
 *   The length of the tag to verify (in bytes).
 *
 * @param[in] input
 *   The buffer holding the ciphertext. Its size is @b length bytes.
 *
 * @param[out] output
 *   The buffer for holding the decrypted plaintext. It must have room for
 *   @b length bytes.
 *
 * @return
 *   SL_STATUS_OK when the command was executed successfully, otherwise an
 *   appropriate error code (@ref sl_status.h).
 ******************************************************************************/
sl_status_t sl_se_gcm_auth_decrypt(sl_se_command_context_t *cmd_ctx,
                                   const sl_se_key_descriptor_t *key,
                                   size_t length,
                                   const unsigned char *iv,
                                   size_t iv_len,
                                   const unsigned char *add,
                                   size_t add_len,
                                   const unsigned char *input,
                                   unsigned char *output,
                                   size_t tag_len,
                                   const unsigned char *tag);

/***************************************************************************//**
 * @brief
 *   This function calculates the full generic CMAC on the input buffer with
 *   the provided key.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] key
 *   Pointer to sl_se_key_descriptor_t structure.
 *
 * @param[in] input
 *   Buffer holding the input data, must be at least @p input_len bytes wide.
 *
 * @param[in] input_len
 *   The length of the input data in bytes.
 *
 * @param[out] output
 *   Buffer holding the 16-byte output data, must be at least 16 bytes wide.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_cmac(sl_se_command_context_t *cmd_ctx,
                       const sl_se_key_descriptor_t *key,
                       const unsigned char *input,
                       size_t input_len,
                       unsigned char *output);

/***************************************************************************//**
 * @brief
 *   Prepare a CMAC streaming command context object.
 *
 * @details
 *   Prepare a CMAC streaming command context object to be used in subsequent
 *   CMAC streaming function calls.
 *
 * @param[in] cmac_ctx
 *   Pointer to a CMAC streaming context object.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] key
 *   Pointer to sl_se_key_descriptor_t structure.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_cmac_multipart_starts(sl_se_cmac_multipart_context_t *cmac_ctx,
                                        sl_se_command_context_t *cmd_ctx,
                                        const sl_se_key_descriptor_t *key);

/***************************************************************************//**
 * @brief
 *   Deprecated, please switch to using \ref sl_se_cmac_multipart_starts().
 *
 *   Prepare a CMAC streaming command context object.
 *
 * @details
 *   Prepare a CMAC streaming command context object to be used in subsequent
 *   CMAC streaming function calls.
 *
 * @param[in] cmac_ctx
 *   Pointer to a CMAC streaming context object.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] key
 *   Pointer to sl_se_key_descriptor_t structure.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_cmac_starts(sl_se_cmac_streaming_context_t *cmac_ctx,
                              sl_se_command_context_t *cmd_ctx,
                              const sl_se_key_descriptor_t *key) SL_DEPRECATED_API_SDK_3_3;

/***************************************************************************//**
 * @brief
 *   This function feeds an input buffer into an ongoing CMAC computation.
 *
 * @details
 *   It is called between sl_se_cmac_multipart_starts() and sl_se_cmac_multipart_finish().
 *   Can be called repeatedly.
 *
 * @param[in,out] cmac_ctx
 *   Pointer to a CMAC streaming context object.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] key
 *   Pointer to sl_se_key_descriptor_t structure.
 *
 * @param[in] input
 *   Buffer holding the input data, must be at least @p input_len bytes wide.
 *
 * @param[in] input_len
 *   The length of the input data in bytes.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_cmac_multipart_update(sl_se_cmac_multipart_context_t *cmac_ctx,
                                        sl_se_command_context_t *cmd_ctx,
                                        const sl_se_key_descriptor_t *key,
                                        const uint8_t *input,
                                        size_t input_len);

/***************************************************************************//**
 * @brief
 *   Deprecated, please switch to using \ref sl_se_cmac_multipart_update().
 *
 *   This function feeds an input buffer into an ongoing CMAC computation.
 *
 * @details
 *   It is called between sl_se_cmac_multipart_starts() and sl_se_cmac_multipart_finish().
 *   Can be called repeatedly.
 *
 * @param[in,out] cmac_ctx
 *   Pointer to a CMAC streaming context object.
 *
 * @param[in] input
 *   Buffer holding the input data, must be at least @p input_len bytes wide.
 *
 * @param[in] input_len
 *   The length of the input data in bytes.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/

sl_status_t sl_se_cmac_update(sl_se_cmac_streaming_context_t *cmac_ctx,
                              const uint8_t *input,
                              size_t input_len) SL_DEPRECATED_API_SDK_3_3;

/***************************************************************************//**
 * @brief
 *   Finish a CMAC streaming operation and return the resulting CMAC tag.
 *
 * @details
 *   It is called after sl_se_cmac_multipart_update().
 *
 * @param[in,out] cmac_ctx
 *   Pointer to a CMAC streaming context object.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] key
 *   Pointer to sl_se_key_descriptor_t structure.
 *
 * @param[out] output
 *   Buffer holding the 16-byte CMAC tag, must be at least 16 bytes wide.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_cmac_multipart_finish(sl_se_cmac_multipart_context_t *cmac_ctx,
                                        sl_se_command_context_t *cmd_ctx,
                                        const sl_se_key_descriptor_t *key,
                                        uint8_t *output);

/***************************************************************************//**
 * @brief
 *   Deprecated, please switch to using \ref sl_se_cmac_multipart_finish().
 *
 *   Finish a CMAC streaming operation and return the resulting CMAC tag.
 *
 * @details
 *   It is called after sl_se_cmac_update().
 *
 * @param[in,out] cmac_ctx
 *   Pointer to a CMAC streaming context object.
 *
 * @param[out] output
 *   Buffer holding the 16-byte CMAC tag, must be at least 16 bytes wide.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_cmac_finish(sl_se_cmac_streaming_context_t *cmac_ctx,
                              uint8_t *output) SL_DEPRECATED_API_SDK_3_3;

/***************************************************************************//**
 * @brief
 *   Deprecated, please switch to using \ref sl_se_gcm_multipart_starts().
 *
 *   Prepare a GCM streaming command context object.
 * @details
 *   Prepare a GCM streaming command context object to be used in subsequent
 *   GCM streaming function calls.
 *
 * @param[in] gcm_ctx
 *   Pointer to a GCM streaming context object.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] key
 *   Pointer to sl_se_key_descriptor_t structure.
 *
 * @param mode
 *   The operation to perform: SL_SE_ENCRYPT or SL_SE_DECRYPT.
 *
 * @param iv
 *   The initialization vector.
 *
 * @param iv_len
 *   The length of the IV.
 *
 * @param add
 *   The buffer holding the additional data, or NULL if @p add_len is 0.
 *
 * @param add_len
 *   The length of the additional data. If 0, @p  add is NULL.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_gcm_starts(sl_se_gcm_streaming_context_t *gcm_ctx,
                             sl_se_command_context_t *cmd_ctx,
                             const sl_se_key_descriptor_t *key,
                             sl_se_cipher_operation_t mode,
                             const uint8_t *iv,
                             size_t iv_len,
                             const uint8_t *add,
                             size_t add_len) SL_DEPRECATED_API_SDK_3_3;

/***************************************************************************//**
 * @brief
 *   Prepare a GCM streaming command context object.
 *
 * @details
 *   Prepare a GCM streaming command context object to be used in subsequent
 *   GCM streaming function calls.
 *
 * @param[in, out] gcm_ctx
 *   Pointer to a GCM streaming context object.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] key
 *   Pointer to sl_se_key_descriptor_t structure.
 *
 * @param[in] mode
 *   The operation to perform: SL_SE_ENCRYPT or SL_SE_DECRYPT.
 *
 * @param[in] iv
 *   The initialization vector.
 *
 * @param[in] iv_len
 *   The length of the IV.
 *
 * @param[in] add
 *   The buffer holding the additional data, or NULL if @p add_len is 0.
 *
 * @param[in] add_len
 *   The length of the additional data. If 0, @p  add is NULL.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_gcm_multipart_starts(sl_se_gcm_multipart_context_t *gcm_ctx,
                                       sl_se_command_context_t *cmd_ctx,
                                       const sl_se_key_descriptor_t *key,
                                       sl_se_cipher_operation_t mode,
                                       const uint8_t *iv,
                                       size_t iv_len,
                                       const uint8_t *add,
                                       size_t add_len);

/***************************************************************************//**
 * @brief
 *  Deprecated, please switch to using \ref sl_se_gcm_multipart_update().
 *
 *   This function feeds an input buffer into an ongoing GCM computation.
 *
 *   It is called between sl_se_gcm_starts() and sl_se_gcm_finish().
 *   Can be called repeatedly.
 *
 * @param[in] gcm_ctx
 *   Pointer to a GCM streaming context object.
 *
 * @param[in] length
 *   The length of the input data. This must be a multiple of 16 except in
 *   the last call before sl_se_gcm_finish().
 *
 * @param[in] input
 *   Buffer holding the input data, must be at least @p length bytes wide.
 *
 * @param[out] output
 *   Buffer for holding the output data, must be at least @p length bytes wide.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_gcm_update(sl_se_gcm_streaming_context_t *gcm_ctx,
                             size_t length,
                             const uint8_t *input,
                             uint8_t *output) SL_DEPRECATED_API_SDK_3_3;

/***************************************************************************//**
 * @brief
 *   This function feeds an input buffer into an ongoing GCM computation.
 *
 *   It is called between sl_se_gcm_multipart_starts() and sl_se_gcm_multiapart_finish().
 *   Can be called repeatedly.
 *
 * @param[in, out] gcm_ctx
 *   Pointer to a GCM streaming context object.
 *
 * @param[in] length
 *   The length of the input data.
 *
 * @param[in] input
 *   Buffer holding the input data, must be at least @p length bytes wide.
 *
 * @param[out] output
 *   Buffer for holding the output data, must be at least @p length bytes wide.
 *
 * @param[out] output_length
 *   Length of data that has been encrypted/decrypted.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_gcm_multipart_update(sl_se_gcm_multipart_context_t *gcm_ctx,
                                       sl_se_command_context_t *cmd_ctx,
                                       const sl_se_key_descriptor_t *key,
                                       size_t length,
                                       const uint8_t *input,
                                       uint8_t *output,
                                       size_t *output_length);

/***************************************************************************//**
 * @brief
 *   Deprecated, please switch to using \ref sl_se_gcm_multipart_finish().
 *
 *   Finish a GCM streaming operation and return the resulting GCM tag.
 *
 *   It is called after sl_se_gcm_update().
 *
 * @param[in] gcm_ctx
 *   Pointer to a GCM streaming context object.
 *
 * @param[out] tag
 *   The buffer for holding the tag.
 *
 * @param[in]  tag_len
 *   The length of the tag to generate. Must be at least four.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_gcm_finish(sl_se_gcm_streaming_context_t *gcm_ctx,
                             uint8_t *tag,
                             size_t   tag_len) SL_DEPRECATED_API_SDK_3_3;

/***************************************************************************//**
 * @brief
 *   Finish a GCM streaming operation and return the resulting GCM tag.
 *
 *   It is called after sl_se_gcm_multipart_update().
 *
 * @param[in, out] gcm_ctx
 *   Pointer to a GCM streaming context object.
 *
 * @param[in, out] tag
 *   Encryption: The buffer for holding the tag.
 *   Decryption: The tag to authenticate.
 *
 * @param[in]  tag_length
 *   Encryption: Length of the output tag.
 *   Decryption: Length of tag to verify
 *
 * @param[out] output
 *   Buffer for holding the output data.
 *
 * @param[in] output_size
 *   Output buffer size. Must be equal or greater to the stored data from
 *   sl_se_gcm_multipart_update (stored data is maximum 16 bytes).
 *
 * @param[out] output_length
 *   Length of data that has been encrypted/decrypted.
 *
 * @return
 *   Returns SL_SE_INVALID_SIGNATURE if authentication step fails.
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_gcm_multipart_finish(sl_se_gcm_multipart_context_t *gcm_ctx,
                                       sl_se_command_context_t *cmd_ctx,
                                       const sl_se_key_descriptor_t *key,
                                       uint8_t *tag,
                                       uint8_t tag_length,
                                       uint8_t *output,
                                       uint8_t output_size,
                                       uint8_t *output_length);

/***************************************************************************//**
 * @brief
 *   Compute a HMAC on a full message.
 *
 * @details
 *   This function computes a Keyed-Hashed Message Authentication Code (HMAC)
 *   for the given input message. HMAC can be used with any iterative
 *   cryptographic hash function, e.g., SHA-1 in combination with a
 *   secret shared key.  The cryptographic strength of HMAC depends on the
 *   properties of the underlying hash function. For instance, if the algorithm
 *   is chosen to be SHA-256, it will generate a 32 bytes HMAC.
 *   This function supports SHA-1, SHA-224, SHA-256, SHA-384 and SHA-512.
 *   The key can be of any length. If the key is shorter than the block size
 *   of the hash function the SE will append zeros to the key so the key size
 *   matches the block size of the hash function. If the key is longer than the
 *   block size of the hash function the key will be hashed to
 *   produce a key digest, then append zeros so the resulting 'hashed' key size
 *   matches the block size of the hash function. In any case the minimal
 *   recommended key length is the digest size of the hash function.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] key
 *   Pointer to sl_se_key_descriptor_t structure specifying the key to use in
 *   the HMAC computation.
 *
 * @param[in] hash_type
 *   Which hashing algorithm to use.
 *
 * @param[in] message
 *   Pointer to the message buffer to compute the hash/digest from.
 *
 * @param[in] message_len
 *   Number of bytes in message.
 *
 * @param[out] output
 *   Pointer to memory buffer to store the final HMAC output.
 *
 * @param[in]  output_len
 *   The length of the HMAC output memory buffer, must be at least the size
 *   of the corresponding hash type.
 *
 * @return Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_hmac(sl_se_command_context_t *cmd_ctx,
                       const sl_se_key_descriptor_t *key,
                       sl_se_hash_type_t hash_type,
                       const uint8_t *message,
                       size_t message_len,
                       uint8_t *output,
                       size_t output_len);

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT) || defined(DOXYGEN)
/***************************************************************************//**
 * @brief
 *   ChaCha20 buffer encryption/decryption, as defined by RFC8439 section 2.4.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] mode
 *   Crypto operation type (encryption or decryption).
 *
 * @param[in] key
 *   Pointer to sl_se_key_descriptor_t structure.
 *
 * @param[in] length
 *   Length of the input data.
 *
 * @param[in] initial_counter
 *   The initial counter value as defined in RFC8439 section 2.4.
 *
 * @param[in] nonce
 *   The nonce, also called initialisation vector, as defined in RFC8439 section
 *   2.4.
 *
 * @param[in] input
 *   Buffer holding the input data.
 *
 * @param[out] output
 *   Buffer holding the output data.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_chacha20_crypt(sl_se_command_context_t *cmd_ctx,
                                 sl_se_cipher_operation_t mode,
                                 const sl_se_key_descriptor_t *key,
                                 size_t length,
                                 const unsigned char initial_counter[4],
                                 const unsigned char nonce[12],
                                 const unsigned char *input,
                                 unsigned char *output);

/***************************************************************************//**
 * @brief
 *   ChaCha20-Poly1305 authenticated encryption with additional data, as defined
 *   by RFC8439 section 2.8.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] key
 *   Pointer to sl_se_key_descriptor_t structure.
 *
 * @param[in] length
 *   The length of the input data in bytes.
 *
 * @param[in] nonce
 *   The nonce, also called initialisation vector, as defined in RFC8439 section
 *   2.8.
 *
 * @param[in] add
 *   The buffer holding additional authenticated data. Can be NULL if @p add_len
 *   equals 0.
 *
 * @param[in] add_len
 *   The length of the additional authenticated data in bytes.
 *
 * @param[in] input
 *   The buffer holding the plaintext input.
 *
 * @param[out] output
 *   The buffer holding the ciphertext output. Can be NULL, in which case the
 *   generated ciphertext will be discarded. Must be at least @p length bytes
 *   wide.
 *
 * @param[out] tag
 *   The buffer holding the tag. This function will produce a 128-bit tag, so
 *   this buffer must be at least 16 bytes wide. Can be NULL, in which case the
 *   generated tag will be discarded.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_chacha20_poly1305_encrypt_and_tag(sl_se_command_context_t *cmd_ctx,
                                                    const sl_se_key_descriptor_t *key,
                                                    size_t length,
                                                    const unsigned char nonce[12],
                                                    const unsigned char *add, size_t add_len,
                                                    const unsigned char *input,
                                                    unsigned char *output,
                                                    unsigned char *tag);

/***************************************************************************//**
 * @brief
 *   ChaCha20-Poly1305 authenticated decryption with additional data, as defined
 *   by RFC8439 section 2.8.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] key
 *   Pointer to sl_se_key_descriptor_t structure.
 *
 * @param[in] length
 *   The length of the input data in Bytes.
 *
 * @param[in] nonce
 *   The nonce, also called initialisation vector, as defined in RFC8439 section
 *   2.8.
 *
 * @param[in] add
 *   The buffer holding additional authenticated data. Can be NULL if @p add_len
 *   equals 0.
 *
 * @param[in] add_len
 *   The length of the additional authenticated data in bytes.
 *
 * @param[in] input
 *   The buffer holding the ciphertext to decrypt. Can be NULL if @p length
 *   equals 0.
 *
 * @param[out] output
 *   The buffer holding the plaintext output. Can be NULL, in which case the
 *   decrypted plaintext will be discarded, or when @p length equals 0. Must be
 *   at least @p length bytes wide.
 *
 * @param[in] tag
 *   The buffer holding the tag to verify.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_chacha20_poly1305_auth_decrypt(sl_se_command_context_t *cmd_ctx,
                                                 const sl_se_key_descriptor_t *key,
                                                 size_t length,
                                                 const unsigned char nonce[12],
                                                 const unsigned char *add, size_t add_len,
                                                 const unsigned char *input,
                                                 unsigned char *output,
                                                 const unsigned char tag[16]);

/***************************************************************************//**
 * @brief
 *   Generate a Poly1305 MAC (message authentication code) for a given message
 *   using an ephemeral key derived using ChaCha20.
 *
 * @note
 *   This function first derives a Poly1305 key based on a ChaCha20 key and
 *   nonce, which are input to this function. The key derivation adheres to
 *   RFC8439 section 2.6. The derived key is then used to calculate a MAC of the
 *   input data, according to RFC8439 section 2.5.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] key
 *   Pointer to sl_se_key_descriptor_t structure containing a ChaCha20 key.
 *
 * @param[in] length
 *   The length of the input data in Bytes.
 *
 * @param[in] nonce
 *   The nonce, also called initialisation vector, as defined in RFC8439 section
 *   2.6.
 *
 * @param[in] input
 *   The buffer holding the input data.
 *
 * @param[out] tag
 *   The buffer holding the tag. This function will produce a 128-bit tag, so
 *   this buffer must be at least 16 bytes wide.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_poly1305_genkey_tag(sl_se_command_context_t *cmd_ctx,
                                      const sl_se_key_descriptor_t *key,
                                      size_t length,
                                      const unsigned char nonce[12],
                                      const unsigned char *input,
                                      unsigned char *tag);

#endif // (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)

#ifdef __cplusplus
}
#endif

/// @} (end addtogroup sl_se_manager_cipher)
/// @} (end addtogroup sl_se_manager)

#endif // defined(SEMAILBOX_PRESENT)

#endif // SL_SE_MANAGER_CIPHER_H
