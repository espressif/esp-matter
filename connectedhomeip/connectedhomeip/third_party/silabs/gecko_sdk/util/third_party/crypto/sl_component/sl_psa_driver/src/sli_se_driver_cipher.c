/***************************************************************************//**
 * @file
 * @brief Silicon Labs PSA Crypto Driver Cipher functions.
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

#include "em_device.h"

#if defined(SEMAILBOX_PRESENT)

#include "psa/crypto.h"
#include "psa/crypto_extra.h"
#include "sli_se_driver_cipher.h"
#include "sl_se_manager.h"
#include "sl_se_manager_cipher.h"

#include "sli_psa_driver_common.h"
#include "sli_se_driver_key_management.h"

#include <string.h>

#if (defined(PSA_WANT_KEY_TYPE_AES)        \
  && (defined(PSA_WANT_ALG_ECB_NO_PADDING) \
  || defined(PSA_WANT_ALG_CTR)             \
  || defined(PSA_WANT_ALG_CFB)             \
  || defined(PSA_WANT_ALG_OFB)             \
  || defined(PSA_WANT_ALG_CCM)             \
  || defined(PSA_WANT_ALG_CBC_NO_PADDING)  \
  || defined(PSA_WANT_ALG_CBC_PKCS7)))     \
  || (defined(PSA_WANT_KEY_TYPE_CHACHA20)  \
  && defined(PSA_WANT_ALG_STREAM_CIPHER))

/**
 * @brief
 *   Validate that the given key desc has the correct properties
 *   to be used for a cipher operation
 * @param key_desc
 *   Pointer to a key descriptor
 * @return
 *   PSA_SUCCESS if all is good
 *   PSA_ERROR_INVALID_ARGUMENT otherwise
 */
static psa_status_t validate_key_type(const sl_se_key_descriptor_t *key_desc)
{
  sl_se_key_type_t sl_key_type = key_desc->type;
  // Check with if (..) since switch does not support multiple equal entries
  // (AES 256 and CHACHA20 has same sl_key_type value)
  if (sl_key_type == SL_SE_KEY_TYPE_AES_128
      || sl_key_type == SL_SE_KEY_TYPE_AES_192
      || sl_key_type == SL_SE_KEY_TYPE_AES_256
      #if _SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT
      || sl_key_type == SL_SE_KEY_TYPE_CHACHA20
      #endif // VAULT
      ) {
    return PSA_SUCCESS;
  }

  return PSA_ERROR_INVALID_ARGUMENT;
}

// Validate combination of key and algorithm
static psa_status_t
validate_key_algorithm_match(psa_algorithm_t alg,
                             const psa_key_attributes_t *attributes)
{
  switch (alg) {
    case PSA_ALG_ECB_NO_PADDING:
    case PSA_ALG_CTR:
    case PSA_ALG_CFB:
    case PSA_ALG_OFB:
    case PSA_ALG_CCM_STAR_NO_TAG:
    case PSA_ALG_CBC_NO_PADDING:
    case PSA_ALG_CBC_PKCS7:
      if (psa_get_key_type(attributes) != PSA_KEY_TYPE_AES) {
        return PSA_ERROR_NOT_SUPPORTED;
      }
      break;
    #if _SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT
    case PSA_ALG_STREAM_CIPHER:
      if (psa_get_key_type(attributes) != PSA_KEY_TYPE_CHACHA20) {
        return PSA_ERROR_NOT_SUPPORTED;
      }
      break;
    #endif // VAULT
    default:
      return PSA_ERROR_NOT_SUPPORTED;
  }
  return PSA_SUCCESS;
}

#endif // PSA_WANT_ALG_* && PSA_WANT_KEY_TYPE_*

/** Encrypt a message using a symmetric cipher.
 *
 * This function encrypts a message with a random IV (initialization
 * vector). Use the multipart operation interface with a
 * #psa_cipher_operation_t object to provide other forms of IV.
 *
 * \param handle                Handle to the key to use for the operation.
 *                              It must remain valid until the operation
 *                              terminates.
 * \param alg                   The cipher algorithm to compute
 *                              (\c PSA_ALG_XXX value such that
 *                              #PSA_ALG_IS_CIPHER(\p alg) is true).
 * \param[in] input             Buffer containing the message to encrypt.
 * \param input_length          Size of the \p input buffer in bytes.
 * \param[out] output           Buffer where the output is to be written.
 *                              The output contains the IV followed by
 *                              the ciphertext proper.
 * \param output_size           Size of the \p output buffer in bytes.
 * \param[out] output_length    On success, the number of bytes
 *                              that make up the output.
 *
 * \retval #PSA_SUCCESS
 *         Success.
 * \retval #PSA_ERROR_INVALID_HANDLE
 * \retval #PSA_ERROR_NOT_PERMITTED
 * \retval #PSA_ERROR_INVALID_ARGUMENT
 *         \p handle is not compatible with \p alg.
 * \retval #PSA_ERROR_NOT_SUPPORTED
 *         \p alg is not supported or is not a cipher algorithm.
 * \retval #PSA_ERROR_BUFFER_TOO_SMALL
 * \retval #PSA_ERROR_INSUFFICIENT_MEMORY
 * \retval #PSA_ERROR_COMMUNICATION_FAILURE
 * \retval #PSA_ERROR_HARDWARE_FAILURE
 * \retval #PSA_ERROR_CORRUPTION_DETECTED
 * \retval #PSA_ERROR_STORAGE_FAILURE
 * \retval #PSA_ERROR_BAD_STATE
 *         The library has not been previously initialized by psa_crypto_init().
 *         It is implementation-dependent whether a failure to initialize
 *         results in this error code.
 */
psa_status_t sli_se_driver_cipher_encrypt(const psa_key_attributes_t *attributes,
                                          const uint8_t *key_buffer,
                                          size_t key_buffer_size,
                                          psa_algorithm_t alg,
                                          const uint8_t *iv,
                                          size_t iv_length,
                                          const uint8_t *input,
                                          size_t input_length,
                                          uint8_t *output,
                                          size_t output_size,
                                          size_t *output_length)
{
#if (defined(PSA_WANT_KEY_TYPE_AES)                                            \
  && (defined(PSA_WANT_ALG_ECB_NO_PADDING)                                     \
  || defined(PSA_WANT_ALG_CTR)                                                 \
  || defined(PSA_WANT_ALG_CFB)                                                 \
  || defined(PSA_WANT_ALG_OFB)                                                 \
  || defined(PSA_WANT_ALG_CCM)                                                 \
  || defined(PSA_WANT_ALG_CBC_NO_PADDING)                                      \
  || defined(PSA_WANT_ALG_CBC_PKCS7)))                                         \
  || ((defined(PSA_WANT_KEY_TYPE_CHACHA20)                                     \
  && (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)) \
  && defined(PSA_WANT_ALG_STREAM_CIPHER))

#if defined(MBEDTLS_PSA_CRYPTO_C)
#if (defined(PSA_WANT_KEY_TYPE_AES)                                            \
  && (defined(PSA_WANT_ALG_CTR)                                                \
  || defined(PSA_WANT_ALG_CFB)                                                 \
  || defined(PSA_WANT_ALG_OFB)                                                 \
  || defined(PSA_WANT_ALG_CCM)                                                 \
  || defined(PSA_WANT_ALG_CBC_NO_PADDING)                                      \
  || defined(PSA_WANT_ALG_CBC_PKCS7)))                                         \
  || ((defined(PSA_WANT_KEY_TYPE_CHACHA20)                                     \
  && (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)) \
  && defined(PSA_WANT_ALG_STREAM_CIPHER))
  uint8_t tmp_buf[16] = { 0 };
#endif
#if (defined(PSA_WANT_KEY_TYPE_AES)       \
  && (defined(PSA_WANT_ALG_CTR)           \
  || defined(PSA_WANT_ALG_CFB)            \
  || defined(PSA_WANT_ALG_OFB)            \
  || defined(PSA_WANT_ALG_CCM)            \
  || defined(PSA_WANT_ALG_CBC_NO_PADDING) \
  || defined(PSA_WANT_ALG_CBC_PKCS7)))
  uint8_t final_block[16] = { 0 };
#endif
#endif // MBEDTLS_PSA_CRYPTO_C

  // Argument check
  if (key_buffer == NULL
      || key_buffer_size == 0
      || (input == NULL && input_length > 0)
      || (iv == NULL && iv_length > 0)
      || (output == NULL && output_size > 0)
      || output_length == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  psa_status_t psa_status = validate_key_algorithm_match(alg, attributes);
  if (psa_status != PSA_SUCCESS) {
    return psa_status;
  }

  // Ephemeral contexts
  sl_se_command_context_t cmd_ctx = { 0 };
  sl_se_key_descriptor_t key_desc = { 0 };

  sl_status_t status = sl_se_init_command_context(&cmd_ctx);
  if (status != SL_STATUS_OK) {
    return PSA_ERROR_HARDWARE_FAILURE;
  }

  psa_status = sli_se_key_desc_from_input(attributes,
                                          key_buffer,
                                          key_buffer_size,
                                          &key_desc);
  if (psa_status != PSA_SUCCESS) {
    return psa_status;
  }
  psa_status = validate_key_type(&key_desc);
  if (psa_status != PSA_SUCCESS) {
    return psa_status;
  }

  if (input_length == 0) {
    *output_length = 0;
    return PSA_SUCCESS;
  }

  // Our drivers only support full or no overlap between input and output
  // buffers. So in the case of partial overlap, copy the input buffer into
  // the output buffer and process it in place as if the buffers fully
  // overlapped.
  if ((output > input) && (output < (input + input_length))) {
    // Sanity check before copying. Some ciphers have a stricter requirement
    // than this (if an IV is included), but no ciphers will have an output
    // smaller than the input.
    if (output_size < input_length) {
      return PSA_ERROR_INVALID_ARGUMENT;
    }

    memmove(output, input, input_length);
    input = output;
  }

  switch (alg) {
#if defined(PSA_WANT_KEY_TYPE_AES) && defined(PSA_WANT_ALG_ECB_NO_PADDING)
    case PSA_ALG_ECB_NO_PADDING:
      // Check buffer sizes
      if (output_size < input_length) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }

      // We cannot do ECB on non-block sizes
      if (input_length % 16 != 0) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }

      // do the operation
      status = sl_se_aes_crypt_ecb(&cmd_ctx,
                                   &key_desc,
                                   SL_SE_ENCRYPT,
                                   input_length,
                                   input,
                                   output);
      if (status != PSA_SUCCESS) {
        goto exit;
      }
      *output_length = input_length;
      break;
#endif // PSA_WANT_KEY_TYPE_AES && PSA_WANT_ALG_ECB_NO_PADDING
#if defined(MBEDTLS_PSA_CRYPTO_C)
#if defined(PSA_WANT_KEY_TYPE_AES) && defined(PSA_WANT_ALG_CCM)
    case PSA_ALG_CCM_STAR_NO_TAG:
      // Explicit fallthrough
#endif
#if defined(PSA_WANT_KEY_TYPE_AES) && (defined(PSA_WANT_ALG_CTR) || defined(PSA_WANT_ALG_CCM))
    case PSA_ALG_CTR:
      // Check buffer sizes
      if (output_size < input_length) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }
#if defined(PSA_WANT_ALG_CCM)
      if (alg == PSA_ALG_CCM_STAR_NO_TAG) {
        if (iv_length != 13) {
          return PSA_ERROR_INVALID_ARGUMENT;
        }

        // AES-CCM*-no-tag is basically AES-CTR with preformatted IV
        tmp_buf[0] = 1;
        memcpy(&tmp_buf[1], iv, 13);
        tmp_buf[14] = 0;
        tmp_buf[15] = 1;
      } else
#endif
      {
        if (iv_length != 16) {
          return PSA_ERROR_INVALID_ARGUMENT;
        }

        // Write nonce to temporary buf to be used internally by sl_se_aes_crypt_ctr.
        memcpy(tmp_buf, iv, 16);
      }

      // Store final block in a temporary buffer in order to avoid in being overwritten
      // inside of sl_se_aes_crypt_ctr() (hence the separation into two calls).
      if ((input_length & 0x0F) > 0) {
        memcpy(final_block, &input[input_length & ~0x0F], 16);
      }

      // Do multi-block operation if applicable.
      if ((input_length & ~0x0F) > 0) {
        status = sl_se_aes_crypt_ctr(&cmd_ctx,
                                     &key_desc,
                                     input_length & ~0x0F,
                                     NULL,
                                     tmp_buf,
                                     tmp_buf,
                                     input,
                                     output);
        if (status != PSA_SUCCESS) {
          goto exit;
        }
      }

      // Encrypt final block if there is any.
      if ((input_length & 0x0F) > 0) {
        status = sl_se_aes_crypt_ctr(&cmd_ctx,
                                     &key_desc,
                                     input_length & 0x0F,
                                     NULL,
                                     tmp_buf,
                                     tmp_buf,
                                     final_block,
                                     &output[(input_length & ~0x0F)]);
        if (status != PSA_SUCCESS) {
          goto exit;
        }
      }

      *output_length = input_length;
      break;
#endif // PSA_WANT_KEY_TYPE_AES && (PSA_WANT_ALG_CTR || PSA_WANT_ALG_CCM)
#if defined(PSA_WANT_KEY_TYPE_AES) && defined(PSA_WANT_ALG_CFB)
    case PSA_ALG_CFB:
      // Check buffer sizes
      if (output_size < input_length) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }

      if (iv_length != 16) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }

      // Write IV to temporary buf to be used internally by sl_se_aes_crypt_cbf128.
      memcpy(tmp_buf, iv, 16);

      // Store final block in a temporary buffer in order to avoid in being overwritten
      // inside of sl_se_aes_crypt_ctr() (hence the separation into two calls).
      if ((input_length & 0x0F) > 0) {
        memcpy(final_block, &input[input_length & ~0x0F], 16);
      }

      // Do multi-block operation if applicable.
      if ((input_length & ~0x0F) > 0) {
        status = sl_se_aes_crypt_cfb128(&cmd_ctx,
                                        &key_desc,
                                        SL_SE_ENCRYPT,
                                        input_length & ~0x0F,
                                        NULL,
                                        tmp_buf,
                                        input,
                                        output);
        if (status != PSA_SUCCESS) {
          goto exit;
        }
      }

      // Encrypt final block if there is any.
      if ((input_length & 0x0F) > 0) {
        status = sl_se_aes_crypt_cfb128(&cmd_ctx,
                                        &key_desc,
                                        SL_SE_ENCRYPT,
                                        input_length & 0x0F,
                                        NULL,
                                        tmp_buf,
                                        final_block,
                                        &output[(input_length & ~0x0F)]);
        if (status != PSA_SUCCESS) {
          goto exit;
        }
      }

      *output_length = input_length;
      break;
#endif // PSA_WANT_KEY_TYPE_AES && PSA_WANT_ALG_CFB
#if defined(PSA_WANT_KEY_TYPE_AES) && defined(PSA_WANT_ALG_OFB)
    case PSA_ALG_OFB:
    {
      // Check buffer sizes
      if (output_size < input_length) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }

      if (iv_length != 16) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }

      // Write IV to temporary buf to be used internally by sl_se_aes_crypt_ecb.
      memcpy(tmp_buf, iv, 16);

      size_t data_length = input_length;
      size_t n = 0;

      // Use final_block as a temporary storage in order to avoid input being
      // overwritten by the output (in case of buffer overlap).
      memcpy(final_block, input, 16);

      // Loop over input data to create output.
      do {
        if (n == 0) {
          status = sl_se_aes_crypt_ecb(&cmd_ctx,
                                       &key_desc,
                                       SL_SE_ENCRYPT,
                                       16,
                                       tmp_buf,
                                       tmp_buf);
          if (status != SL_STATUS_OK) {
            goto exit;
          }
        }
        uint8_t tmp_input_val = final_block[n];
        final_block[n] = input[16 + input_length - data_length];
        output[input_length - data_length] = tmp_input_val ^ tmp_buf[n];
        n = (n + 1) & 0x0F;
      } while (data_length--);

      *output_length = input_length;
    }
    break;
#endif // PSA_WANT_KEY_TYPE_AES && PSA_WANT_ALG_OFB
#if defined(PSA_WANT_KEY_TYPE_AES)           \
    && (defined(PSA_WANT_ALG_CBC_NO_PADDING) \
    || defined(PSA_WANT_ALG_CBC_PKCS7))
    case PSA_ALG_CBC_NO_PADDING:
      // We cannot do CBC without padding on non-block sizes.
      if (input_length % 16 != 0) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }
    // fall through
    case PSA_ALG_CBC_PKCS7:
      // Check buffer sizes
      if (alg == PSA_ALG_CBC_NO_PADDING) {
        if (output_size < input_length) {
          return PSA_ERROR_BUFFER_TOO_SMALL;
        }
      } else {
        if (output_size < 16 + (input_length & ~0xF)) {
          return PSA_ERROR_BUFFER_TOO_SMALL;
        }
      }

      if (iv_length != 16) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }

      // Write IV to temporary buf to be used internally by sl_se_aes_crypt_cbf128.
      memcpy(tmp_buf, iv, 16);

      // Store last block (if non-blocksize input-length) to temporary buffer to be used in padding.
      if (alg == PSA_ALG_CBC_PKCS7) {
        memcpy(final_block, &input[input_length & ~0xF], input_length & 0xF);
      }

      // CBC-encrypt all but the last block
      if (input_length >= 16) {
        status = sl_se_aes_crypt_cbc(&cmd_ctx,
                                     &key_desc,
                                     SL_SE_ENCRYPT,
                                     input_length & ~0xF,
                                     tmp_buf,
                                     input,
                                     output);
        if (status != SL_STATUS_OK) {
          goto exit;
        }
      }

      // Process final block.
      if (alg == PSA_ALG_CBC_PKCS7) {
        // Add PKCS7 padding.
        memset(&final_block[input_length & 0xF], 16 - (input_length & 0xF), 16 - (input_length & 0xF));

        // Store IV (last ciphertext block) in temp buffer to avoid messing up output.
        if (input_length >= 16) {
          memcpy(tmp_buf, &output[(input_length & ~0xF) - 16], 16);
        }

        // CBC-encrypt the last block.
        status = sl_se_aes_crypt_cbc(&cmd_ctx,
                                     &key_desc,
                                     SL_SE_ENCRYPT,
                                     16,
                                     tmp_buf,
                                     final_block,
                                     final_block);

        if (status != SL_STATUS_OK) {
          goto exit;
        }

        // Copy to output.
        memcpy(&output[(input_length & ~0xF)], final_block, 16);
        *output_length = (input_length & ~0xF) + 16;
      } else {
        *output_length = input_length;
      }
      break;
#endif // PSA_WANT_KEY_TYPE_AES && (PSA_WANT_ALG_CBC_PKCS7 || PSA_WANT_ALG_CBC_NO_PADDING)
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
#if defined(PSA_WANT_KEY_TYPE_CHACHA20) && defined(PSA_WANT_ALG_STREAM_CIPHER)
    case PSA_ALG_STREAM_CIPHER:
      if (psa_get_key_type(attributes) != PSA_KEY_TYPE_CHACHA20) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }

      // check buffer sizes
      if (output_size < input_length) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }

      if (iv_length != 12) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }

      // PSA Crypto dictates that the initial counter for ChaCha20 starts
      // at zero (unless using the multi-part API)
      memset(tmp_buf, 0, 4);
      memcpy(&tmp_buf[4], iv, 12);

      status = sl_se_chacha20_crypt(&cmd_ctx,
                                    SL_SE_ENCRYPT,
                                    &key_desc,
                                    input_length,
                                    tmp_buf,
                                    &tmp_buf[4],
                                    input,
                                    output);
      if (status != SL_STATUS_OK) {
        goto exit;
      }

      *output_length = input_length;
      break;
#endif // PSA_WANT_KEY_TYPE_CHACHA20 && PSA_WANT_ALG_STREAM_CIPHER
#endif // VAULT
#endif // MBEDTLS_PSA_CRYPTO_C
    default:
      (void)attributes;
      (void)key_buffer;
      (void)key_buffer_size;
      (void)alg;
      (void)iv;
      (void)iv_length;
      (void)input;
      (void)input_length;
      (void)output;
      (void)output_size;
      (void)output_length;
      return PSA_ERROR_NOT_SUPPORTED;
  }

  exit:
  if (status != SL_STATUS_OK) {
    memset(output, 0, output_size);
    *output_length = 0;
    if (status == SL_STATUS_FAIL) {
      // This specific code maps to 'does not exist' for builtin keys
      return PSA_ERROR_DOES_NOT_EXIST;
    } else {
      return PSA_ERROR_HARDWARE_FAILURE;
    }
  } else {
    return PSA_SUCCESS;
  }

#else // PSA_WANT_ALG_* && PSA_WANT_KEY_TYPE_*

  (void)attributes;
  (void)key_buffer;
  (void)key_buffer_size;
  (void)alg;
  (void)iv;
  (void)iv_length;
  (void)input;
  (void)input_length;
  (void)output;
  (void)output_size;
  (void)output_length;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_* && PSA_WANT_KEY_TYPE_*
}

/** Decrypt a message using a symmetric cipher.
 *
 * This function decrypts a message encrypted with a symmetric cipher.
 *
 * \param handle                Handle to the key to use for the operation.
 *                              It must remain valid until the operation
 *                              terminates.
 * \param alg                   The cipher algorithm to compute
 *                              (\c PSA_ALG_XXX value such that
 *                              #PSA_ALG_IS_CIPHER(\p alg) is true).
 * \param[in] input             Buffer containing the message to decrypt.
 *                              This consists of the IV followed by the
 *                              ciphertext proper.
 * \param input_length          Size of the \p input buffer in bytes.
 * \param[out] output           Buffer where the plaintext is to be written.
 * \param output_size           Size of the \p output buffer in bytes.
 * \param[out] output_length    On success, the number of bytes
 *                              that make up the output.
 *
 * \retval #PSA_SUCCESS
 *         Success.
 * \retval #PSA_ERROR_INVALID_HANDLE
 * \retval #PSA_ERROR_NOT_PERMITTED
 * \retval #PSA_ERROR_INVALID_ARGUMENT
 *         \p handle is not compatible with \p alg.
 * \retval #PSA_ERROR_NOT_SUPPORTED
 *         \p alg is not supported or is not a cipher algorithm.
 * \retval #PSA_ERROR_BUFFER_TOO_SMALL
 * \retval #PSA_ERROR_INSUFFICIENT_MEMORY
 * \retval #PSA_ERROR_COMMUNICATION_FAILURE
 * \retval #PSA_ERROR_HARDWARE_FAILURE
 * \retval #PSA_ERROR_STORAGE_FAILURE
 * \retval #PSA_ERROR_CORRUPTION_DETECTED
 * \retval #PSA_ERROR_BAD_STATE
 *         The library has not been previously initialized by psa_crypto_init().
 *         It is implementation-dependent whether a failure to initialize
 *         results in this error code.
 */
psa_status_t sli_se_driver_cipher_decrypt(const psa_key_attributes_t *attributes,
                                          const uint8_t *key_buffer,
                                          size_t key_buffer_size,
                                          psa_algorithm_t alg,
                                          const uint8_t *input,
                                          size_t input_length,
                                          uint8_t *output,
                                          size_t output_size,
                                          size_t *output_length)
{
#if (defined(PSA_WANT_KEY_TYPE_AES)                                           \
  && (defined(PSA_WANT_ALG_ECB_NO_PADDING)                                    \
  || defined(PSA_WANT_ALG_CTR)                                                \
  || defined(PSA_WANT_ALG_CFB)                                                \
  || defined(PSA_WANT_ALG_OFB)                                                \
  || defined(PSA_WANT_ALG_CCM)                                                \
  || defined(PSA_WANT_ALG_CBC_NO_PADDING)                                     \
  || defined(PSA_WANT_ALG_CBC_PKCS7)))                                        \
  || (defined(PSA_WANT_KEY_TYPE_CHACHA20)                                     \
  && (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT) \
  && defined(PSA_WANT_ALG_STREAM_CIPHER))

#if defined(PSA_WANT_KEY_TYPE_AES)   \
  && (defined(PSA_WANT_ALG_CTR)      \
  || defined(PSA_WANT_ALG_CCM)       \
  || defined(PSA_WANT_ALG_CBC_PKCS7) \
  || defined(PSA_WANT_ALG_CBC_NO_PADDING))
  uint8_t tmp_buf[16] = { 0 };
#endif
#if (defined(PSA_WANT_KEY_TYPE_AES)                                            \
  && (defined(PSA_WANT_ALG_CTR)                                                \
  || defined(PSA_WANT_ALG_OFB)                                                 \
  || defined(PSA_WANT_ALG_CFB)                                                 \
  || defined(PSA_WANT_ALG_CCM)                                                 \
  || defined(PSA_WANT_ALG_CBC_PKCS7)                                           \
  || defined(PSA_WANT_ALG_CBC_NO_PADDING)))                                    \
  || ((defined(PSA_WANT_KEY_TYPE_CHACHA20)                                     \
  && (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)) \
  && defined(PSA_WANT_ALG_STREAM_CIPHER))
  uint8_t iv_buf[16] = { 0 };
#endif

  // Argument check.
  if (key_buffer == NULL
      || key_buffer_size == 0
      || (input == NULL && input_length > 0)
      || (output == NULL && output_size > 0)
      || output_length == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  psa_status_t psa_status = validate_key_algorithm_match(alg, attributes);
  if (psa_status != PSA_SUCCESS) {
    return psa_status;
  }

  // Ephemeral contexts.
  sl_se_command_context_t cmd_ctx = { 0 };
  sl_se_key_descriptor_t key_desc = { 0 };

  sl_status_t status = sl_se_init_command_context(&cmd_ctx);
  if (status != SL_STATUS_OK) {
    return PSA_ERROR_HARDWARE_FAILURE;
  }

  psa_status = sli_se_key_desc_from_input(attributes,
                                          key_buffer,
                                          key_buffer_size,
                                          &key_desc);
  if (psa_status != PSA_SUCCESS) {
    return psa_status;
  }
  psa_status = validate_key_type(&key_desc);
  if (psa_status != PSA_SUCCESS) {
    return psa_status;
  }

  if (input_length == 0) {
    *output_length = 0;
    return PSA_SUCCESS;
  }

  // Our drivers only support full or no overlap between input and output
  // buffers. So in the case of partial overlap, copy the input buffer into
  // the output buffer and process it in place as if the buffers fully
  // overlapped.
  if ((output > input) && (output < (input + input_length))) {
    // Sanity check before copying. Some ciphers have a stricter requirement
    // than this (if an IV is included), but no ciphers will have an output
    // smaller than the input.
    if (output_size < input_length) {
      return PSA_ERROR_INVALID_ARGUMENT;
    }

    memmove(output, input, input_length);
    input = output;
  }

  switch (alg) {
#if defined(PSA_WANT_KEY_TYPE_AES) && defined(PSA_WANT_ALG_ECB_NO_PADDING)
    case PSA_ALG_ECB_NO_PADDING:
      // Check buffer sizes.
      if (output_size < input_length) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
      }

      // We cannot do ECB on non-block sizes.
      if (input_length % 16 != 0) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }

      // Do the operation.
      status = sl_se_aes_crypt_ecb(&cmd_ctx,
                                   &key_desc,
                                   SL_SE_DECRYPT,
                                   input_length,
                                   input,
                                   output);

      *output_length = input_length;
      break;
#endif // PSA_WANT_KEY_TYPE_AES && PSA_WANT_ALG_ECB_NO_PADDING
#if defined(PSA_WANT_KEY_TYPE_AES) && defined(PSA_WANT_ALG_CCM)
    case PSA_ALG_CCM_STAR_NO_TAG:
      // Explicit fallthrough
#endif
#if defined(PSA_WANT_KEY_TYPE_AES) && (defined(PSA_WANT_ALG_CTR) || defined(PSA_WANT_ALG_CCM))
    case PSA_ALG_CTR:
      // Check buffer sizes.
#if defined(PSA_WANT_ALG_CCM)
      if (alg == PSA_ALG_CCM_STAR_NO_TAG) {
        if (output_size < input_length - 13) {
          return PSA_ERROR_BUFFER_TOO_SMALL;
        }

        // AES-CCM*-no-tag is basically AES-CTR with preformatted IV
        iv_buf[0] = 1;
        memcpy(&iv_buf[1], input, 13);
        iv_buf[14] = 0;
        iv_buf[15] = 1;
        input += 13;
        input_length -= 13;
      } else
#endif
      {
        if (output_size < input_length - 16) {
          return PSA_ERROR_BUFFER_TOO_SMALL;
        }

        // Write IV to temporary buf to be used internally by sl_se_aes_crypt_ctr.
        memcpy(iv_buf, input, 16);
        input += 16;
        input_length -= 16;
      }

      status = sl_se_aes_crypt_ctr(&cmd_ctx,
                                   &key_desc,
                                   input_length,
                                   NULL,
                                   iv_buf,
                                   tmp_buf,
                                   input,
                                   output);

      *output_length = input_length;
      break;
#endif // PSA_WANT_KEY_TYPE_AES && (PSA_WANT_ALG_CTR || PSA_WANT_ALG_CCM)
#if defined(PSA_WANT_KEY_TYPE_AES) && defined(PSA_WANT_ALG_CFB)
    case PSA_ALG_CFB:
      // Check buffer sizes.
      if (output_size < input_length - 16) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }

      // Write IV to temporary buf to be used internally by sl_se_aes_crypt_cfb128.
      memcpy(iv_buf, input, 16);

      status = sl_se_aes_crypt_cfb128(&cmd_ctx,
                                      &key_desc,
                                      SL_SE_DECRYPT,
                                      input_length - 16,
                                      NULL,
                                      iv_buf,
                                      &input[16],
                                      output);

      *output_length = input_length - 16;
      break;
#endif // PSA_WANT_KEY_TYPE_AES && PSA_WANT_ALG_CFB
#if defined(PSA_WANT_KEY_TYPE_AES) && defined(PSA_WANT_ALG_OFB)
    case PSA_ALG_OFB:
    {
      // Check buffer sizes.
      if (output_size < input_length - 16) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
      }

      // Write IV to temporary buf to be used internally by sl_se_aes_crypt_ecb.
      memcpy(iv_buf, input, 16);

      input += 16;
      size_t data_length = input_length - 16;
      size_t n = 0;

      // Loop over input data to create output.
      while (data_length--) {
        if (n == 0) {
          status = sl_se_aes_crypt_ecb(&cmd_ctx,
                                       &key_desc,
                                       SL_SE_ENCRYPT,
                                       16,
                                       iv_buf,
                                       iv_buf);
          if (status != SL_STATUS_OK) {
            goto exit;
          }
        }
        *output++ = *input++ ^ iv_buf[n];

        n = (n + 1) & 0x0F;
      }

      *output_length = input_length - 16;
    }
    break;
#endif // PSA_WANT_KEY_TYPE_AES && PSA_WANT_ALG_OFB
#if defined(PSA_WANT_KEY_TYPE_AES)           \
    && (defined(PSA_WANT_ALG_CBC_NO_PADDING) \
    || defined(PSA_WANT_ALG_CBC_PKCS7))
    case PSA_ALG_CBC_NO_PADDING:
      // We cannot do CBC without padding on non-block sizes.
      if (input_length % 16 != 0) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }
    // fall through
    case PSA_ALG_CBC_PKCS7: {
      size_t full_blocks;
      if (alg == PSA_ALG_CBC_NO_PADDING) {
        if (output_size < input_length - 16) {
          return PSA_ERROR_BUFFER_TOO_SMALL;
        }
        full_blocks = (input_length - 16) / 16;
      } else {
        // Check correct input amount
        if (input_length < 32
            || ((input_length & 0xF) != 0)) {
          return PSA_ERROR_INVALID_ARGUMENT;
        }
        // Check output has enough room for at least n-1 blocks.
        if (output_size < (input_length - 32)) {
          return PSA_ERROR_BUFFER_TOO_SMALL;
        }
        full_blocks = (input_length - 32) / 16;
      }

      // Write IV to temporary buf to be used internally by sl_se_aes_crypt_cbc.
      memcpy(iv_buf, input, 16);

      // CBC-decrypt all but the last block.
      if (full_blocks > 0) {
        status = sl_se_aes_crypt_cbc(&cmd_ctx,
                                     &key_desc,
                                     SL_SE_DECRYPT,
                                     full_blocks * 16,
                                     iv_buf,
                                     &input[16],
                                     output);
        if (status != SL_STATUS_OK) {
          goto exit;
        }
      }

      // Process final block.
      if (alg == PSA_ALG_CBC_PKCS7) {
        // Store last block to temporary buffer to be used in removing the padding.
        memcpy(tmp_buf, &input[input_length - 16], 16);

        // CBC-decrypt the last block.
        status = sl_se_aes_crypt_cbc(&cmd_ctx,
                                     &key_desc,
                                     SL_SE_DECRYPT,
                                     16,
                                     iv_buf,
                                     tmp_buf,
                                     tmp_buf);
        if (status != SL_STATUS_OK) {
          goto exit;
        }

        // Validate padding.
        uint8_t pad_bytes = tmp_buf[15];
        psa_status = sli_psa_validate_pkcs7_padding(tmp_buf,
                                                    16,
                                                    pad_bytes);
        if (psa_status != PSA_SUCCESS) {
          *output_length = 0;
          return psa_status;
        }

        if (output_size < (input_length - 16 - pad_bytes)) {
          *output_length = 0;
          return PSA_ERROR_BUFFER_TOO_SMALL;
        }

        // Copy non-padding bytes.
        memcpy(&output[full_blocks * 16], tmp_buf, 16 - pad_bytes);
        *output_length = input_length - 16 - pad_bytes;
      } else {
        *output_length = input_length - 16;
      }
      break;
    }
#endif // PSA_WANT_KEY_TYPE_AES && (PSA_WANT_ALG_CBC_PKCS7 || PSA_WANT_ALG_CBC_NO_PADDING)
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
#if defined(PSA_WANT_KEY_TYPE_CHACHA20) && defined(PSA_WANT_ALG_STREAM_CIPHER)
    case PSA_ALG_STREAM_CIPHER:
      if (psa_get_key_type(attributes) != PSA_KEY_TYPE_CHACHA20) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }

      // check buffer sizes.
      if (output_size < input_length - 12) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
      }

      // PSA Crypto dictates that the initial counter for ChaCha20 starts
      // at zero (unless using the multi-part API)
      memset(iv_buf, 0, 4);

      status = sl_se_chacha20_crypt(&cmd_ctx,
                                    SL_SE_DECRYPT,
                                    &key_desc,
                                    input_length - 12, // - 12 due to the nonce.
                                    iv_buf,
                                    input,
                                    &input[12],
                                    output);

      *output_length = input_length - 12;
      break;
#endif // PSA_WANT_KEY_TYPE_CHACHA20 && PSA_WANT_ALG_STREAM_CIPHER
#endif
    default:
      return PSA_ERROR_NOT_SUPPORTED;
  }

#if defined(PSA_WANT_KEY_TYPE_AES)   \
  && (defined(PSA_WANT_ALG_OFB)      \
  || defined(PSA_WANT_ALG_CBC_PKCS7) \
  || defined(PSA_WANT_ALG_CBC_NO_PADDING))
  exit:
#endif

  if (status != SL_STATUS_OK) {
    memset(output, 0, output_size);
    *output_length = 0;
    if (status == SL_STATUS_FAIL) {
      // This specific code maps to 'does not exist' for builtin keys
      return PSA_ERROR_DOES_NOT_EXIST;
    } else {
      return PSA_ERROR_HARDWARE_FAILURE;
    }
  } else {
    return PSA_SUCCESS;
  }

#else // PSA_WANT_ALG_* && PSA_WANT_KEY_TYPE_*

  (void)attributes;
  (void)key_buffer;
  (void)key_buffer_size;
  (void)alg;
  (void)input;
  (void)input_length;
  (void)output;
  (void)output_size;
  (void)output_length;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_* && PSA_WANT_KEY_TYPE_*
}

psa_status_t sli_se_driver_cipher_encrypt_setup(sli_se_driver_cipher_operation_t *operation,
                                                const psa_key_attributes_t *attributes,
                                                psa_algorithm_t alg)
{
#if (defined(PSA_WANT_KEY_TYPE_AES)        \
  && (defined(PSA_WANT_ALG_ECB_NO_PADDING) \
  || defined(PSA_WANT_ALG_CTR)             \
  || defined(PSA_WANT_ALG_CFB)             \
  || defined(PSA_WANT_ALG_OFB)             \
  || defined(PSA_WANT_ALG_CCM)             \
  || defined(PSA_WANT_ALG_CBC_NO_PADDING)  \
  || defined(PSA_WANT_ALG_CBC_PKCS7)))     \
  || (defined(PSA_WANT_KEY_TYPE_CHACHA20)  \
  && defined(PSA_WANT_ALG_STREAM_CIPHER))

  if (operation == NULL || attributes == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // Reset context
  memset(operation, 0, sizeof(*operation));

  // Set up context
  memcpy(&operation->alg, &alg, sizeof(alg));
  operation->direction = SL_SE_ENCRYPT;

  // Validate combination of key and algorithm
  return validate_key_algorithm_match(alg, attributes);

#else // PSA_WANT_ALG_* && PSA_WANT_KEY_TYPE_*

  (void)operation;
  (void)attributes;
  (void)alg;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_* && PSA_WANT_KEY_TYPE_*
}

psa_status_t sli_se_driver_cipher_decrypt_setup(sli_se_driver_cipher_operation_t *operation,
                                                const psa_key_attributes_t *attributes,
                                                psa_algorithm_t alg)
{
#if (defined(PSA_WANT_KEY_TYPE_AES)        \
  && (defined(PSA_WANT_ALG_ECB_NO_PADDING) \
  || defined(PSA_WANT_ALG_CTR)             \
  || defined(PSA_WANT_ALG_CFB)             \
  || defined(PSA_WANT_ALG_OFB)             \
  || defined(PSA_WANT_ALG_CCM)             \
  || defined(PSA_WANT_ALG_CBC_NO_PADDING)  \
  || defined(PSA_WANT_ALG_CBC_PKCS7)))     \
  || (defined(PSA_WANT_KEY_TYPE_CHACHA20)  \
  && defined(PSA_WANT_ALG_STREAM_CIPHER))

  if (operation == NULL || attributes == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // Reset context
  memset(operation, 0, sizeof(*operation));

  // Set up context
  memcpy(&operation->alg, &alg, sizeof(alg));
  operation->direction = SL_SE_DECRYPT;

  // Validate combination of key and algorithm
  return validate_key_algorithm_match(alg, attributes);

  #else // PSA_WANT_ALG_* && PSA_WANT_KEY_TYPE_*

  (void)operation;
  (void)attributes;
  (void)alg;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_* && PSA_WANT_KEY_TYPE_*
}

psa_status_t sli_se_driver_cipher_set_iv(sli_se_driver_cipher_operation_t *operation,
                                         const uint8_t *iv,
                                         size_t iv_length)
{
#if (defined(PSA_WANT_KEY_TYPE_AES)       \
  && (defined(PSA_WANT_ALG_CTR)           \
  || defined(PSA_WANT_ALG_CFB)            \
  || defined(PSA_WANT_ALG_OFB)            \
  || defined(PSA_WANT_ALG_CCM)            \
  || defined(PSA_WANT_ALG_CBC_NO_PADDING) \
  || defined(PSA_WANT_ALG_CBC_PKCS7)))    \
  || (defined(PSA_WANT_KEY_TYPE_CHACHA20) \
  && defined(PSA_WANT_ALG_STREAM_CIPHER))

  if (operation == NULL || iv == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  if (iv_length > sizeof(operation->iv)) {
    // IV can't be larger than what our state can store
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  if (operation->iv_len != 0) {
    // IV was set previously
    return PSA_ERROR_BAD_STATE;
  }

  switch (operation->alg) {
    case PSA_ALG_ECB_NO_PADDING:
      if (iv_length > 0) {
        return PSA_ERROR_INVALID_ARGUMENT;
      } else {
        return PSA_SUCCESS;
      }
    case PSA_ALG_CTR:
    case PSA_ALG_CFB:
    case PSA_ALG_OFB:
    case PSA_ALG_CBC_NO_PADDING:
    case PSA_ALG_CBC_PKCS7:
      if (iv_length != 16) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }
      memcpy(operation->iv, iv, iv_length);
      break;
    case PSA_ALG_CCM_STAR_NO_TAG:
      // Preformat the IV for CCM*-no-tag here, such that the remainder
      // of the processing for this algorithm boils down to AES-CTR
      if (iv_length != 13) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }
      operation->iv[0] = 1;
      memcpy(&operation->iv[1], iv, iv_length);
      operation->iv[14] = 0;
      operation->iv[15] = 1;
      iv_length = 16;
      break;
    #if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
    case PSA_ALG_STREAM_CIPHER:
      // PSA Crypto supports multiple IV input lengths for ChaCha20
      // refer to the doc for PSA_ALG_STREAM_CIPHER
      if (iv_length == 12) {
        // Set initial counter value to zero
        memset(operation->iv, 0, 4);
        memcpy(&operation->iv[4], iv, iv_length);
      } else if (iv_length == 16) {
        // Initial counter value is stored little-endian in the first four bytes
        // This makes our lives easier: since this driver will only run on
        // little-endian machines, we can just cast it to a uint32.
        memcpy(operation->iv, iv, iv_length);
      } else if (iv_length == 8) {
        // "original" ChaCha20: 8-byte IV and 8-byte counter (which is initialised to 0)
        // We currently don't support this format
        return PSA_ERROR_NOT_SUPPORTED;
      } else {
        return PSA_ERROR_INVALID_ARGUMENT;
      }
      break;
    #endif
    default:
      return PSA_ERROR_BAD_STATE;
  }

  operation->iv_len = iv_length;

  return PSA_SUCCESS;

#else // PSA_WANT_ALG_* && PSA_WANT_KEY_TYPE_*

  (void)operation;
  (void)iv;
  (void)iv_length;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_* && PSA_WANT_KEY_TYPE_*
}

psa_status_t sli_se_driver_cipher_update(sli_se_driver_cipher_operation_t *operation,
                                         const uint8_t *input,
                                         size_t input_length,
                                         uint8_t *output,
                                         size_t output_size,
                                         size_t *output_length)
{
#if (defined(PSA_WANT_KEY_TYPE_AES)                                            \
  && (defined(PSA_WANT_ALG_ECB_NO_PADDING)                                     \
  || defined(PSA_WANT_ALG_CTR)                                                 \
  || defined(PSA_WANT_ALG_CFB)                                                 \
  || defined(PSA_WANT_ALG_OFB)                                                 \
  || defined(PSA_WANT_ALG_CCM)                                                 \
  || defined(PSA_WANT_ALG_CBC_NO_PADDING)                                      \
  || defined(PSA_WANT_ALG_CBC_PKCS7)))                                         \
  || ((defined(PSA_WANT_KEY_TYPE_CHACHA20)                                     \
  && (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)) \
  && defined(PSA_WANT_ALG_STREAM_CIPHER))

  // Argument check
  if (operation == NULL
      || (input == NULL && input_length > 0)
      || (output == NULL && output_size > 0)
      || output_length == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // Key desc has been properly set by wrapper function
  const sl_se_key_descriptor_t *key_desc = &operation->key_desc;
  psa_status_t psa_status = validate_key_type(key_desc);
  if (psa_status != PSA_SUCCESS) {
    return psa_status;
  }

  bool lagging;
  size_t bytes_to_boundary = 16 - (operation->processed_length % 16);
  size_t actual_output_length = 0;

  // We need to cache (not return) the whole last block for decryption with
  // padding, otherwise it won't be possible to remove a potential padding block
  // during finish.
  bool cache_full_block = (operation->alg == PSA_ALG_CBC_PKCS7
                           && operation->direction == SL_SE_DECRYPT);

  // Figure out whether the operation is on a lagging or forward-looking cipher
  // Lagging: needs a full block of input data before being able to output
  // Non-lagging: can output the same amount of data as getting fed
  switch (operation->alg) {
    case PSA_ALG_ECB_NO_PADDING:
    case PSA_ALG_CBC_NO_PADDING:
    case PSA_ALG_CBC_PKCS7:
      lagging = true;
      break;
    case PSA_ALG_CTR:
    case PSA_ALG_CCM_STAR_NO_TAG:
    case PSA_ALG_CFB:
    case PSA_ALG_OFB:
      lagging = false;
      break;
    #if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
    case PSA_ALG_STREAM_CIPHER:
      lagging = false;
      break;
    #endif
    default:
      return PSA_ERROR_BAD_STATE;
  }

  if (input_length == 0) {
    // We don't need to do anything if the input length is zero.
    *output_length = 0;
    return PSA_SUCCESS;
  }

  if (lagging) {
    // Early processing if not getting to a full block
    if (cache_full_block
        && bytes_to_boundary == 16
        && operation->processed_length > 0) {
      // Don't overwrite the streaming block if it's currently full.
    } else {
      if (input_length < bytes_to_boundary) {
        memcpy(&operation->streaming_block[operation->processed_length % 16],
               input,
               input_length);
        operation->processed_length += input_length;
        *output_length = actual_output_length;
        return PSA_SUCCESS;
      }
    }

    // We know we'll be computing and outputing at least the completed streaming block
    size_t output_blocks = 1;

    if (input_length > bytes_to_boundary) {
      // plus however many full blocks are left over after filling the stream buffer
      output_blocks += (input_length - bytes_to_boundary) / 16;
      // If we're caching and the sum of already-input and to-be-input data
      // ends up at a block boundary, we won't be outputting the last block
      if (cache_full_block && ((input_length - bytes_to_boundary) % 16 == 0)) {
        output_blocks -= 1;
      }
    }

    if (output_size < (output_blocks * 16)) {
      return PSA_ERROR_BUFFER_TOO_SMALL;
    }
  } else {
    // Early failure if output buffer is too small
    if (output_size < input_length) {
      return PSA_ERROR_BUFFER_TOO_SMALL;
    }
  }

  // Ephemeral contexts
  sl_se_command_context_t cmd_ctx = { 0 };

  sl_status_t status = sl_se_init_command_context(&cmd_ctx);
  if (status != SL_STATUS_OK) {
    return PSA_ERROR_HARDWARE_FAILURE;
  }

  // Our drivers only support full or no overlap between input and output
  // buffers. So in the case of partial overlap, copy the input buffer into
  // the output buffer and process it in place as if the buffers fully
  // overlapped.
  if ((output > input) && (output < (input + input_length))) {
    // Sanity check before copying. Some ciphers have a stricter requirement
    // than this (if an IV is included), but no ciphers will have an output
    // smaller than the input.
    if (output_size < input_length) {
      return PSA_ERROR_INVALID_ARGUMENT;
    }

    memmove(output, input, input_length);
    input = output;
  }

  switch (operation->alg) {
#if defined(PSA_WANT_KEY_TYPE_AES) && defined(PSA_WANT_ALG_ECB_NO_PADDING)
    case PSA_ALG_ECB_NO_PADDING:
      // Read in up to full streaming input block
      if (bytes_to_boundary != 16) {
        memcpy(&operation->streaming_block[operation->processed_length % 16],
               input,
               bytes_to_boundary);
        input += bytes_to_boundary;
        input_length -= bytes_to_boundary;

        status = sl_se_aes_crypt_ecb(&cmd_ctx,
                                     key_desc,
                                     operation->direction,
                                     16,
                                     operation->streaming_block,
                                     output);
        if (status != SL_STATUS_OK) {
          goto exit;
        }

        output += 16;
        actual_output_length += 16;
        operation->processed_length += bytes_to_boundary;
      }

      // Do multi-block operation if applicable
      if (input_length >= 16) {
        size_t operation_size = (input_length / 16) * 16;
        status = sl_se_aes_crypt_ecb(&cmd_ctx,
                                     key_desc,
                                     operation->direction,
                                     operation_size,
                                     input,
                                     output);

        if (status != SL_STATUS_OK) {
          goto exit;
        }

        input += operation_size;
        input_length -= operation_size;
        actual_output_length += operation_size;
        operation->processed_length += operation_size;
      }

      // What's left over in the input buffer will be cleaned up after switch-case
      break;
#endif // PSA_WANT_KEY_TYPE_AES && PSA_WANT_ALG_ECB_NO_PADDING
#if defined(PSA_WANT_KEY_TYPE_AES)           \
    && (defined(PSA_WANT_ALG_CBC_NO_PADDING) \
    || defined(PSA_WANT_ALG_CBC_PKCS7))
    case PSA_ALG_CBC_NO_PADDING:
    // fall through
    case PSA_ALG_CBC_PKCS7:
      if (bytes_to_boundary != 16) {
        memcpy(&operation->streaming_block[operation->processed_length % 16],
               input,
               bytes_to_boundary);
        if (cache_full_block && (bytes_to_boundary == input_length)) {
          // Don't process the streaming block if there is no more input data
        } else {
          status = sl_se_aes_crypt_cbc(&cmd_ctx,
                                       key_desc,
                                       operation->direction,
                                       16,
                                       operation->iv,
                                       operation->streaming_block,
                                       output);
          if (status != PSA_SUCCESS) {
            goto exit;
          }
          output += 16;
          actual_output_length += 16;
        }

        input += bytes_to_boundary;
        input_length -= bytes_to_boundary;
        operation->processed_length += bytes_to_boundary;
      } else if (input_length > 0
                 && cache_full_block
                 && operation->processed_length > 0) {
        // We know there's processing to be done, and that we haven't processed
        // the full block in the streaming buffer yet. Process it now.
        status = sl_se_aes_crypt_cbc(&cmd_ctx,
                                     key_desc,
                                     operation->direction,
                                     16,
                                     operation->iv,
                                     operation->streaming_block,
                                     output);
        if (status != PSA_SUCCESS) {
          goto exit;
        }
        output += 16;
        actual_output_length += 16;
      }

      // Do multi-block operation if applicable
      if (input_length >= 16) {
        size_t operation_size = (input_length / 16) * 16;
        if (cache_full_block && (input_length % 16 == 0)) {
          // Don't decrypt the last block until finish is called, so that we
          // can properly remove the padding before returning it.
          operation_size -= 16;
        }

        if (operation_size > 0) {
          status = sl_se_aes_crypt_cbc(&cmd_ctx,
                                       key_desc,
                                       operation->direction,
                                       operation_size,
                                       operation->iv,
                                       input,
                                       output);
          if (status != PSA_SUCCESS) {
            goto exit;
          }
        } else {
          status = PSA_SUCCESS;
        }

        input += operation_size;
        input_length -= operation_size;
        actual_output_length += operation_size;
        operation->processed_length += operation_size;
      }

      // What's left over in the input buffer will be cleaned up after switch-case
      break;
#endif // PSA_WANT_KEY_TYPE_AES && (PSA_WANT_ALG_CBC_PKCS7 || PSA_WANT_ALG_CBC_NO_PADDING)
#if defined(PSA_WANT_KEY_TYPE_AES) && defined(PSA_WANT_ALG_CCM)
    case PSA_ALG_CCM_STAR_NO_TAG:
      // Explicit fallthrough
#endif // PSA_WANT_KEY_TYPE_AES && PSA_WANT_ALG_CCM
#if defined(PSA_WANT_KEY_TYPE_AES) && (defined(PSA_WANT_ALG_CTR) || defined(PSA_WANT_ALG_CCM))
    case PSA_ALG_CTR:
    {
      uint32_t offset = operation->processed_length % 16;

      status = sl_se_aes_crypt_ctr(&cmd_ctx,
                                   key_desc,
                                   input_length,
                                   &offset,
                                   operation->iv,
                                   operation->streaming_block,
                                   input,
                                   output);

      if (status != SL_STATUS_OK) {
        goto exit;
      }

      input += input_length;
      actual_output_length += input_length;
      operation->processed_length += input_length;
      input_length -= input_length;
      break;
    }
#endif // PSA_WANT_KEY_TYPE_AES && (PSA_WANT_ALG_CTR || PSA_WANT_ALG_CCM)
#if defined(PSA_WANT_KEY_TYPE_AES) && defined(PSA_WANT_ALG_CFB)
    case PSA_ALG_CFB:
    {
      uint32_t offset = operation->processed_length % 16;
      status = sl_se_aes_crypt_cfb128(&cmd_ctx,
                                      key_desc,
                                      operation->direction,
                                      input_length,
                                      &offset,
                                      operation->iv,
                                      input,
                                      output);

      if (status != SL_STATUS_OK) {
        goto exit;
      }

      input += input_length;
      actual_output_length += input_length;
      operation->processed_length += input_length;
      input_length -= input_length;
      break;
    }
#endif // PSA_WANT_KEY_TYPE_AES && PSA_WANT_ALG_CFB
#if defined(PSA_WANT_KEY_TYPE_AES) && defined(PSA_WANT_ALG_OFB)
    case PSA_ALG_OFB:
    {
      size_t data_length = input_length;
      size_t n = operation->processed_length % 16;

      // loop over input data to create output
      while (data_length--) {
        if (n == 0) {
          status = sl_se_aes_crypt_ecb(&cmd_ctx,
                                       key_desc,
                                       SL_SE_ENCRYPT,
                                       16,
                                       operation->iv,
                                       operation->iv);
          if (status != SL_STATUS_OK) {
            goto exit;
          }
        }
        *output++ =  *input++ ^ operation->iv[n];

        n = (n + 1) & 0x0F;
      }

      input += input_length;
      actual_output_length += input_length;
      operation->processed_length += input_length;
      input_length -= input_length;
      break;
    }
#endif // PSA_WANT_KEY_TYPE_AES && PSA_WANT_ALG_OFB
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
#if defined(PSA_WANT_KEY_TYPE_CHACHA20) && defined(PSA_WANT_ALG_STREAM_CIPHER)
    case PSA_ALG_STREAM_CIPHER:
    {
      // counter value is at the start of the IV buffer
      uint32_t ctr_value = *((uint32_t*)operation->iv);

      // If the counter would wrap, refuse the operation
      if (ctr_value > (ctr_value + (input_length / 64))) {
        return PSA_ERROR_BAD_STATE;
      }

      if (operation->processed_length % 64 != 0) {
        // Perform partial block operation until block boundary or end of input
        uint8_t chacha20_block[64] = { 0 };
        size_t offset_in_block = operation->processed_length % sizeof(chacha20_block);
        size_t length_in_block = input_length < (sizeof(chacha20_block) - offset_in_block) ? input_length : (sizeof(chacha20_block) - offset_in_block);
        uint32_t counter_bytes = __REV(ctr_value);

        // Retrieve streaming block
        status = sl_se_chacha20_crypt(&cmd_ctx,
                                      SL_SE_ENCRYPT,
                                      key_desc,
                                      sizeof(chacha20_block),
                                      (const unsigned char*)&counter_bytes,
                                      &operation->iv[4],
                                      chacha20_block,
                                      chacha20_block);

        if (status != SL_STATUS_OK) {
          goto exit;
        }

        // Calculate stream output
        for (size_t i = 0; i < length_in_block; i++) {
          output[i] = input[i] ^ chacha20_block[offset_in_block + i];
        }

        input += length_in_block;
        actual_output_length += length_in_block;
        operation->processed_length += length_in_block;
        input_length -= length_in_block;

        // Update the counter if the block is complete
        if (offset_in_block + length_in_block == sizeof(chacha20_block)) {
          ctr_value++;
        }
      }

      if (input_length > 0) {
        // Perform remainder of operation in a single call
        uint32_t counter_bytes = __REV(ctr_value);

        status = sl_se_chacha20_crypt(&cmd_ctx,
                                      SL_SE_ENCRYPT,
                                      key_desc,
                                      input_length,
                                      (const unsigned char*)&counter_bytes,
                                      &operation->iv[4],
                                      input,
                                      &output[actual_output_length]);

        if (status != SL_STATUS_OK) {
          goto exit;
        }

        // Update the counter with the amount of full blocks processed
        ctr_value += input_length / 64;

        input += input_length;
        actual_output_length += input_length;
        operation->processed_length += input_length;
        input_length -= input_length;
      }

      // Store the updated counter number to the IV buffer
      *((uint32_t*)operation->iv) = ctr_value;
      break;
    }
#endif // PSA_WANT_KEY_TYPE_CHACHA20 && PSA_WANT_ALG_STREAM_CIPHER
#endif // VAULT
    default:
      return PSA_ERROR_BAD_STATE;
  }

  // If there's anything left in the input buffer, copy it to the context
  // This'll only be the case for lagging ciphers
  if (input_length > 0) {
    if (!lagging
        || (input_length >= 16 && !cache_full_block)
        || (input_length > 16 && cache_full_block)) {
      *output_length = 0;
      return PSA_ERROR_BAD_STATE;
    }

    memcpy(operation->streaming_block,
           input,
           input_length);
    operation->processed_length += input_length;
  }

  exit:
  if (status != SL_STATUS_OK) {
    *output_length = 0;
    if (status == SL_STATUS_FAIL) {
      // This specific code maps to 'does not exist' for builtin keys
      return PSA_ERROR_DOES_NOT_EXIST;
    } else {
      return PSA_ERROR_HARDWARE_FAILURE;
    }
  } else {
    *output_length = actual_output_length;
    return PSA_SUCCESS;
  }

#else // PSA_WANT_ALG_* && PSA_WANT_KEY_TYPE_*

  (void)operation;
  (void)input;
  (void)input_length;
  (void)output;
  (void)output_size;
  (void)output_length;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_* && PSA_WANT_KEY_TYPE_*
}

psa_status_t sli_se_driver_cipher_finish(sli_se_driver_cipher_operation_t *operation,
                                         uint8_t *output,
                                         size_t output_size,
                                         size_t *output_length)
{
#if (defined(PSA_WANT_KEY_TYPE_AES)        \
  && (defined(PSA_WANT_ALG_ECB_NO_PADDING) \
  || defined(PSA_WANT_ALG_CTR)             \
  || defined(PSA_WANT_ALG_CFB)             \
  || defined(PSA_WANT_ALG_OFB)             \
  || defined(PSA_WANT_ALG_CCM)             \
  || defined(PSA_WANT_ALG_CBC_NO_PADDING)  \
  || defined(PSA_WANT_ALG_CBC_PKCS7)))     \
  || (defined(PSA_WANT_KEY_TYPE_CHACHA20)  \
  && defined(PSA_WANT_ALG_STREAM_CIPHER))

  // Finalize cipher operation. This will only output data for algorithms which include padding.
  // This is currently only AES-CBC with PKCS#7.

  // Argument check
  if (operation == NULL || (output == NULL && output_size > 0) || output_length == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // Key desc has been properly set by wrapper function
  const sl_se_key_descriptor_t *key_desc = &operation->key_desc;
  psa_status_t psa_status = validate_key_type(key_desc);
  if (psa_status != PSA_SUCCESS) {
    return psa_status;
  }

  switch (operation->alg) {
#if defined(PSA_WANT_KEY_TYPE_AES)           \
    && (defined(PSA_WANT_ALG_ECB_NO_PADDING) \
    || defined(PSA_WANT_ALG_CBC_NO_PADDING))
    case PSA_ALG_ECB_NO_PADDING:
    case PSA_ALG_CBC_NO_PADDING:
      // No-padding operations can't finish if they haven't processed block-size input
      *output_length = 0;
      if (operation->processed_length % 16 != 0) {
        psa_status = PSA_ERROR_INVALID_ARGUMENT;
      } else {
        psa_status = PSA_SUCCESS;
      }
      break;
#endif // PSA_WANT_KEY_TYPE_AES && (PSA_WANT_ALG_ECB_NO_PADDING || PSA_WANT_ALG_CBC_NO_PADDING)
#if defined(PSA_WANT_KEY_TYPE_AES) && defined(PSA_WANT_ALG_CBC_PKCS7)
    case PSA_ALG_CBC_PKCS7:
    {
      // Ephemeral contexts
      sl_se_command_context_t cmd_ctx = { 0 };

      sl_status_t status = sl_se_init_command_context(&cmd_ctx);
      if (status != SL_STATUS_OK) {
        psa_status = PSA_ERROR_HARDWARE_FAILURE;
        break;
      }

      // Calculate padding, update, output final block
      if (operation->direction == SL_SE_ENCRYPT) {
        if (output_size < 16) {
          psa_status = PSA_ERROR_BUFFER_TOO_SMALL;
          break;
        }
        size_t padding_bytes = 16 - (operation->processed_length % 16);
        memset(&operation->streaming_block[16 - padding_bytes], padding_bytes, padding_bytes);

        status = sl_se_aes_crypt_cbc(&cmd_ctx,
                                     key_desc,
                                     SL_SE_ENCRYPT,
                                     16,
                                     operation->iv,
                                     operation->streaming_block,
                                     output);
        if (status != SL_STATUS_OK) {
          *output_length = 0;
          psa_status = PSA_ERROR_HARDWARE_FAILURE;
        } else {
          *output_length = 16;
          psa_status = PSA_SUCCESS;
        }
      } else {
        // Expect full-block input
        if (operation->processed_length % 16 != 0 || operation->processed_length < 16) {
          psa_status = PSA_ERROR_INVALID_ARGUMENT;
          break;
        }

        uint8_t out_buf[16];

        // Decrypt the last block
        status = sl_se_aes_crypt_cbc(&cmd_ctx,
                                     key_desc,
                                     SL_SE_DECRYPT,
                                     16,
                                     operation->iv,
                                     operation->streaming_block,
                                     out_buf);

        if (status != SL_STATUS_OK) {
          *output_length = 0;
          psa_status = PSA_ERROR_HARDWARE_FAILURE;
          break;
        } else {
          psa_status = PSA_SUCCESS;
        }

        size_t padding_bytes = out_buf[15];
        psa_status = sli_psa_validate_pkcs7_padding(out_buf, 16, padding_bytes);

        if (psa_status == PSA_SUCCESS) {
          // The padding was valid
          if (output_size < 16 - padding_bytes) {
            psa_status = PSA_ERROR_BUFFER_TOO_SMALL;
            break;
          }
          memcpy(output, out_buf, 16 - padding_bytes);
          *output_length = 16 - padding_bytes;
        }
      }
    }
    break;
#endif // PSA_WANT_KEY_TYPE_AES && PSA_WANT_ALG_CBC_PKCS7
#if (defined(PSA_WANT_KEY_TYPE_AES)         \
    && (defined(PSA_WANT_ALG_CFB)           \
    || defined(PSA_WANT_ALG_OFB)            \
    || defined(PSA_WANT_ALG_CCM)            \
    || defined(PSA_WANT_ALG_CTR)))          \
    || (defined(PSA_WANT_KEY_TYPE_CHACHA20) \
    && defined(PSA_WANT_ALG_STREAM_CIPHER))
    case PSA_ALG_CTR:
    case PSA_ALG_CCM_STAR_NO_TAG:
    case PSA_ALG_CFB:
    case PSA_ALG_OFB:
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
    case PSA_ALG_STREAM_CIPHER:
#endif // VAULT
      // Actual stream ciphers: nothing to do here.
      *output_length = 0;
      psa_status = PSA_SUCCESS;
      break;
#endif
    default:
      psa_status = PSA_ERROR_BAD_STATE;
  }
  if (psa_status != PSA_SUCCESS) {
    *output_length = 0;
  }
  return psa_status;

#else // PSA_WANT_ALG_AES && PSA_WANT_KEY_TYPE_*

  (void)operation;
  (void)output;
  (void)output_size;
  (void)output_length;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_AES && PSA_WANT_KEY_TYPE_*
}

#endif // defined(SEMAILBOX_PRESENT)
