/***************************************************************************//**
 * @file
 * @brief Silicon Labs PSA Crypto Transparent Driver Cipher functions.
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

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "em_device.h"

#if defined(CRYPTO_PRESENT)
#include "sli_crypto_transparent_functions.h"
#include "sli_psa_driver_common.h"
#include "psa/crypto.h"
#include "crypto_management.h"
#include "em_crypto.h"
#include "em_core.h"
#include <string.h>

#if defined(PSA_WANT_KEY_TYPE_AES) \
  && (defined(PSA_WANT_ALG_ECB_NO_PADDING) || defined(PSA_WANT_ALG_OFB))

static psa_status_t sl_crypto_aes_crypt_ecb_single(const uint8_t *key_buffer,
                                                   size_t key_buffer_size,
                                                   psa_encrypt_or_decrypt_t mode,
                                                   const uint8_t input[16],
                                                   uint8_t output[16]);

#endif // PSA_WANT_KEY_TYPE_AES && (PSA_WANT_ALG_ECB_NO_PADDING || PSA_WANT_ALG_OFB)

#if defined(PSA_WANT_KEY_TYPE_AES)         \
  && (defined(PSA_WANT_ALG_CBC_NO_PADDING) \
  || defined(PSA_WANT_ALG_CBC_PKCS7))

static psa_status_t sl_crypto_aes_crypt_cbc(const uint8_t *key_buffer,
                                            size_t key_buffer_size,
                                            psa_encrypt_or_decrypt_t mode,
                                            size_t length,
                                            uint8_t iv[16],
                                            const uint8_t *input,
                                            uint8_t *output);

#endif // PSA_WANT_KEY_TYPE_AES && PSA_WANT_ALG_CBC_*

#if defined(PSA_WANT_KEY_TYPE_AES) && defined(PSA_WANT_ALG_CFB)

static psa_status_t sl_crypto_aes_crypt_cfb128(const uint8_t *key_buffer,
                                               size_t key_buffer_size,
                                               psa_encrypt_or_decrypt_t mode,
                                               size_t length,
                                               size_t *iv_off,
                                               uint8_t iv[16],
                                               const uint8_t *input,
                                               uint8_t *output);

#endif // PSA_WANT_KEY_TYPE_AES && PSA_WANT_ALG_CFB

#if defined(PSA_WANT_KEY_TYPE_AES) && defined(PSA_WANT_ALG_OFB)

static psa_status_t sl_crypto_aes_crypt_ofb(const uint8_t *key_buffer,
                                            size_t key_buffer_size,
                                            size_t length,
                                            size_t *iv_off,
                                            uint8_t iv[16],
                                            const uint8_t *input,
                                            uint8_t *output);

#endif // PSA_WANT_KEY_TYPE_AES && PSA_WANT_ALG_OFB

#if defined(PSA_WANT_KEY_TYPE_AES) && (defined(PSA_WANT_ALG_CTR) || defined(PSA_WANT_ALG_CCM))

static psa_status_t sl_crypto_aes_crypt_ctr(const uint8_t *key_buffer,
                                            size_t key_buffer_size,
                                            size_t length,
                                            size_t *nc_off,
                                            uint8_t nonce_counter[16],
                                            uint8_t stream_block[16],
                                            const uint8_t *input,
                                            uint8_t *output);

#endif // PSA_WANT_KEY_TYPE_AES && (PSA_WANT_ALG_CTR || PSA_WANT_ALG_CCM)

#if (defined(PSA_WANT_KEY_TYPE_AES)        \
  && (defined(PSA_WANT_ALG_ECB_NO_PADDING) \
  || defined(PSA_WANT_ALG_CTR)             \
  || defined(PSA_WANT_ALG_CFB)             \
  || defined(PSA_WANT_ALG_OFB)             \
  || defined(PSA_WANT_ALG_CCM)             \
  || defined(PSA_WANT_ALG_CBC_NO_PADDING)  \
  || defined(PSA_WANT_ALG_CBC_PKCS7)))

static psa_status_t sl_crypto_aes_get_decryption_key(const uint8_t *in_key,
                                                     size_t in_key_size,
                                                     uint8_t *out_key,
                                                     size_t out_key_size);

#endif // PSA_WANT_ALG_AES && PSA_WANT_KEY_TYPE_*

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
psa_status_t sli_crypto_transparent_cipher_encrypt(const psa_key_attributes_t *attributes,
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
#if (defined(PSA_WANT_KEY_TYPE_AES)        \
  && (defined(PSA_WANT_ALG_ECB_NO_PADDING) \
  || defined(PSA_WANT_ALG_CTR)             \
  || defined(PSA_WANT_ALG_CFB)             \
  || defined(PSA_WANT_ALG_OFB)             \
  || defined(PSA_WANT_ALG_CCM)             \
  || defined(PSA_WANT_ALG_CBC_NO_PADDING)  \
  || defined(PSA_WANT_ALG_CBC_PKCS7)))

  // Argument check
  if (key_buffer == NULL
      || key_buffer_size == 0
      || (input == NULL && input_length > 0)
      || (output == NULL && output_size > 0)
      || output_length == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // Check key type and size.
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
      if (key_buffer_size < (psa_get_key_bits(attributes) / 8)
          || !(psa_get_key_bits(attributes) == 128
               || psa_get_key_bits(attributes) == 256)) {
        return PSA_ERROR_NOT_SUPPORTED;
      }
      break;
    default:
      return PSA_ERROR_NOT_SUPPORTED;
  }

  if (input_length == 0) {
    // We don't need to do anything if the input length is zero.
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

  size_t key_len = psa_get_key_bits(attributes) / 8;

  psa_status_t status = PSA_ERROR_HARDWARE_FAILURE;

#if defined(MBEDTLS_PSA_CRYPTO_C)         \
  && (defined(PSA_WANT_ALG_CTR)           \
  || defined(PSA_WANT_ALG_CFB)            \
  || defined(PSA_WANT_ALG_OFB)            \
  || defined(PSA_WANT_ALG_CCM)            \
  || defined(PSA_WANT_ALG_CBC_NO_PADDING) \
  || defined(PSA_WANT_ALG_CBC_PKCS7))
  uint8_t temp_iv[16];
#endif

  switch (alg) {
#if defined(PSA_WANT_ALG_ECB_NO_PADDING)
    case PSA_ALG_ECB_NO_PADDING:
      // Check buffer sizes
      if (output_size < input_length) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }

      // We cannot do ECB on non-block sizes
      if (input_length % 16 != 0) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }

      for (size_t i = 0; i < input_length; i += 16) {
        // Encrypt each block independently
        status = sl_crypto_aes_crypt_ecb_single(key_buffer,
                                                key_len,
                                                PSA_CRYPTO_DRIVER_ENCRYPT,
                                                &input[i],
                                                &output[i]);
        if (status != PSA_SUCCESS) {
          goto exit;
        }
      }

      *output_length = input_length;
      break;
#endif // PSA_WANT_ALG_ECB_NO_PADDING
#if defined(MBEDTLS_PSA_CRYPTO_C)
#if defined(PSA_WANT_ALG_CCM)
    case PSA_ALG_CCM_STAR_NO_TAG:
      // Explicit fallthrough
#endif
#if defined(PSA_WANT_ALG_CTR) || defined(PSA_WANT_ALG_CCM)
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
        temp_iv[0] = 1;
        memcpy(&temp_iv[1], iv, 13);
        temp_iv[14] = 0;
        temp_iv[15] = 1;
      } else
#endif
      {
        if (iv_length != 16) {
          return PSA_ERROR_INVALID_ARGUMENT;
        }

        memcpy(temp_iv, iv, 16);
      }

      uint8_t tmp_buf[16];

      // Do encryption
      status = sl_crypto_aes_crypt_ctr(key_buffer,
                                       key_len,
                                       input_length,
                                       NULL,
                                       temp_iv,
                                       tmp_buf,
                                       input,
                                       output);

      if (status != PSA_SUCCESS) {
        goto exit;
      }

      *output_length = input_length;
      break;
#endif // PSA_WANT_ALG_CTR || PSA_WANT_ALG_CCM
#if defined(PSA_WANT_ALG_CFB)
    case PSA_ALG_CFB:
      // Check buffer sizes
      if (output_size < input_length) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }

      if (iv_length != 16) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }

      memcpy(temp_iv, iv, 16);

      // Do encryption
      status = sl_crypto_aes_crypt_cfb128(key_buffer,
                                          key_len,
                                          PSA_CRYPTO_DRIVER_ENCRYPT,
                                          input_length,
                                          NULL,
                                          temp_iv,
                                          input,
                                          output);
      if (status != PSA_SUCCESS) {
        goto exit;
      }

      *output_length = input_length;
      break;
#endif // PSA_WANT_ALG_CFB
#if defined(PSA_WANT_ALG_OFB)
    case PSA_ALG_OFB:
      // Check buffer sizes
      if (output_size < input_length) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }

      if (iv_length != 16) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }

      memcpy(temp_iv, iv, 16);

      // Do encryption
      status = sl_crypto_aes_crypt_ofb(key_buffer,
                                       key_len,
                                       input_length,
                                       NULL,
                                       temp_iv,
                                       input,
                                       output);

      if (status != PSA_SUCCESS) {
        goto exit;
      }

      *output_length = input_length;
      break;
#endif // PSA_WANT_ALG_OFB
#if defined(PSA_WANT_ALG_CBC_NO_PADDING) || defined(PSA_WANT_ALG_CBC_PKCS7)
    case PSA_ALG_CBC_NO_PADDING:
      // We cannot do CBC without padding on non-block sizes.
      if (input_length % 16 != 0) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }
    // Fall-through
    case PSA_ALG_CBC_PKCS7:
      // Check buffer sizes
      if (alg == PSA_ALG_CBC_NO_PADDING) {
        if (output_size < input_length) {
          return PSA_ERROR_INVALID_ARGUMENT;
        }
      } else {
        if (output_size < ((input_length & ~0xF) + 16)) {
          return PSA_ERROR_INVALID_ARGUMENT;
        }
      }

      if (iv_length != 16) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }

      memcpy(temp_iv, iv, 16);

      // Store the final block before encryption to prevent it from
      // being overwritten. The internal buffer in sl_crypto_aes_crypt_cbc
      // isn't retained across calls.
      uint8_t final_block[16];
      memcpy(final_block, &input[input_length & ~0xF], input_length & 0xF);

      // CBC-encrypt all but the last block
      status = sl_crypto_aes_crypt_cbc(key_buffer,
                                       key_len,
                                       PSA_CRYPTO_DRIVER_ENCRYPT,
                                       input_length & ~0xF,
                                       temp_iv,
                                       input,
                                       output);

      if (status != PSA_SUCCESS) {
        goto exit;
      }

      // Process final block, if any
      if (alg == PSA_ALG_CBC_PKCS7) {
        // Add PKCS padding
        memset(&final_block[input_length & 0xF],
               16 - (input_length & 0xF),
               16 - (input_length & 0xF));

        if (input_length >= 16) {
          memcpy(temp_iv, &output[(input_length & ~0xF) - 16], 16);
        }

        // CBC-encrypt the last block
        status = sl_crypto_aes_crypt_cbc(key_buffer,
                                         key_len,
                                         PSA_CRYPTO_DRIVER_ENCRYPT,
                                         16,
                                         temp_iv,
                                         final_block,
                                         final_block);
        // Copy to output
        memcpy(&output[(input_length & ~0xF)], final_block, 16);
        *output_length = (input_length & ~0xF) + 16;
      } else {
        *output_length = input_length;
      }
      break;
#endif // PSA_WANT_ALG_CBC_PKCS7 || PSA_WANT_ALG_CBC_NO_PADDING
#endif /* MBEDTLS_PSA_CRYPTO_C */
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
  if (status != PSA_SUCCESS) {
    memset(output, 0, output_size);
    *output_length = 0;
    return PSA_ERROR_HARDWARE_FAILURE;
  } else {
    return PSA_SUCCESS;
  }

#else // PSA_WANT_ALG_* && PSA_WANT_KEY_TYPE_AES

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

#endif // PSA_WANT_ALG_* && PSA_WANT_KEY_TYPE_AES
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
psa_status_t sli_crypto_transparent_cipher_decrypt(const psa_key_attributes_t *attributes,
                                                   const uint8_t *key_buffer,
                                                   size_t key_buffer_size,
                                                   psa_algorithm_t alg,
                                                   const uint8_t *input,
                                                   size_t input_length,
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
  || defined(PSA_WANT_ALG_CBC_PKCS7)))

  // Argument check
  if (key_buffer == NULL
      || key_buffer_size == 0
      || (input == NULL && input_length > 0)
      || (output == NULL && output_size > 0)
      || output_length == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // Check key type and size.
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
      if (key_buffer_size < (psa_get_key_bits(attributes) / 8)
          || !(psa_get_key_bits(attributes) == 128
               || psa_get_key_bits(attributes) == 256)) {
        return PSA_ERROR_NOT_SUPPORTED;
      }
      break;
    default:
      return PSA_ERROR_NOT_SUPPORTED;
  }

  if (input_length == 0) {
    // We don't need to do anything if the input length is zero.
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

  size_t key_len = psa_get_key_bits(attributes) / 8;

  psa_status_t status;

#if defined(PSA_WANT_ALG_CTR)             \
  || defined(PSA_WANT_ALG_CFB)            \
  || defined(PSA_WANT_ALG_OFB)            \
  || defined(PSA_WANT_ALG_CCM)            \
  || defined(PSA_WANT_ALG_CBC_NO_PADDING) \
  || defined(PSA_WANT_ALG_CBC_PKCS7)
  uint8_t temp_iv[16];
#endif
#if defined(PSA_WANT_ALG_ECB_NO_PADDING)  \
  || defined(PSA_WANT_ALG_CBC_NO_PADDING) \
  || defined(PSA_WANT_ALG_CBC_PKCS7)
  uint8_t dec_key[32];
#endif

  switch (alg) {
#if defined(PSA_WANT_ALG_ECB_NO_PADDING)
    case PSA_ALG_ECB_NO_PADDING:
      // Check buffer sizes
      if (output_size < input_length) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }

      // We cannot do ECB on non-block sizes
      if (input_length % 16 != 0) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }

      // The CRYPTO accelerator can't decrypt with the proper key directly,
      // so we need to generate a decryption key to be used first.
      status = sl_crypto_aes_get_decryption_key(key_buffer, key_len, dec_key, key_len);
      if (status != PSA_SUCCESS) {
        return status;
      }

      for (size_t i = 0; i < input_length; i += 16) {
        // Decrypt each block independently using ECB
        status = sl_crypto_aes_crypt_ecb_single(dec_key,
                                                key_len,
                                                PSA_CRYPTO_DRIVER_DECRYPT,
                                                &input[i],
                                                &output[i]);
        if (status != PSA_SUCCESS) {
          goto exit;
        }
      }

      memset(dec_key, 0, 32);
      *output_length = input_length;
      break;
#endif // PSA_WANT_ALG_ECB_NO_PADDING
#if defined(PSA_WANT_ALG_CCM)
    case PSA_ALG_CCM_STAR_NO_TAG:
      // Explicit fallthrough
#endif
#if defined(PSA_WANT_ALG_CTR) || defined(PSA_WANT_ALG_CCM)
    case PSA_ALG_CTR:
      // Check buffer sizes
#if defined(PSA_WANT_ALG_CCM)
      if (alg == PSA_ALG_CCM_STAR_NO_TAG) {
        if (output_size < input_length - 13) {
          return PSA_ERROR_BUFFER_TOO_SMALL;
        }

        // AES-CCM*-no-tag is basically AES-CTR with preformatted IV
        temp_iv[0] = 1;
        memcpy(&temp_iv[1], input, 13);
        temp_iv[14] = 0;
        temp_iv[15] = 1;
        input += 13;
        input_length -= 13;
      } else
#endif
      {
        if (output_size < input_length - 16) {
          return PSA_ERROR_INVALID_ARGUMENT;
        }

        memcpy(temp_iv, input, 16);
        input += 16;
        input_length -= 16;
      }

      // Do decryption
      status = sl_crypto_aes_crypt_ctr(key_buffer,
                                       key_len,
                                       input_length,
                                       NULL,
                                       temp_iv,
                                       temp_iv,
                                       input,
                                       output);

      *output_length = input_length;
      break;
#endif // PSA_WANT_ALG_CTR || PSA_WANT_ALG_CCM
#if defined(PSA_WANT_ALG_OFB)
    case PSA_ALG_OFB:
      // Check buffer sizes
      if (output_size < input_length - 16) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }

      memcpy(temp_iv, input, 16);

      // Do decryption
      status = sl_crypto_aes_crypt_ofb(key_buffer,
                                       key_len,
                                       input_length,
                                       NULL,
                                       temp_iv,
                                       &input[16],
                                       output);

      if (status != PSA_SUCCESS) {
        goto exit;
      }

      *output_length = input_length - 16;
      break;
#endif // PSA_WANT_ALG_OFB
#if defined(PSA_WANT_ALG_CFB)
    case PSA_ALG_CFB:
      // Check buffer sizes
      if (output_size < input_length - 16) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }

      memcpy(temp_iv, input, 16);

      // Do decryption
      status = sl_crypto_aes_crypt_cfb128(key_buffer,
                                          key_len,
                                          PSA_CRYPTO_DRIVER_DECRYPT,
                                          input_length - 16,
                                          NULL,
                                          temp_iv,
                                          &input[16],
                                          output);

      if (status != PSA_SUCCESS) {
        goto exit;
      }

      *output_length = input_length - 16;
      break;
#endif // PSA_WANT_ALG_CFB
#if defined(PSA_WANT_ALG_CBC_NO_PADDING) || defined(PSA_WANT_ALG_CBC_PKCS7)
    case PSA_ALG_CBC_NO_PADDING:
    // Fall-through
    case PSA_ALG_CBC_PKCS7:
      // We cannot do CBC without padding on non-block sizes.
      if (input_length % 16 != 0) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }

      size_t full_blocks;
      if (alg == PSA_ALG_CBC_NO_PADDING) {
        if (output_size < input_length - 16) {
          return PSA_ERROR_INVALID_ARGUMENT;
        }
        full_blocks = (input_length - 16) / 16;
      } else {
        // check output has enough room for at least n-1 blocks
        if (input_length < 32
            || ((input_length & 0xF) != 0)
            || output_size < (input_length - 32)) {
          return PSA_ERROR_INVALID_ARGUMENT;
        }
        full_blocks = (input_length - 32) / 16;
      }

      // Generate decryption key
      status = sl_crypto_aes_get_decryption_key(key_buffer, key_len, dec_key, key_len);
      if (status != PSA_SUCCESS) {
        return status;
      }

      // CBC-decrypt all but the last block
      memcpy(temp_iv, input, 16);
      status = sl_crypto_aes_crypt_cbc(dec_key,
                                       key_len,
                                       PSA_CRYPTO_DRIVER_DECRYPT,
                                       full_blocks * 16,
                                       temp_iv,
                                       &input[16],
                                       output);

      if (status != PSA_SUCCESS) {
        memset(dec_key, 0, 32);
        goto exit;
      }

      // Process final block, if any
      if (alg == PSA_ALG_CBC_PKCS7) {
        uint8_t final_block[16];
        memcpy(final_block, &input[input_length - 16], 16);

        // CBC-decrypt the last block
        memcpy(temp_iv, &input[full_blocks * 16], 16);
        status = sl_crypto_aes_crypt_cbc(dec_key,
                                         key_len,
                                         PSA_CRYPTO_DRIVER_DECRYPT,
                                         16,
                                         temp_iv,
                                         final_block,
                                         final_block);
        if (status != PSA_SUCCESS) {
          goto exit;
        }

        sli_psa_zeroize(dec_key, 32);

        // Check how many bytes of padding to subtract
        uint8_t pad_bytes = final_block[15];

        // Check all padding bytes
        status = sli_psa_validate_pkcs7_padding(final_block,
                                                16,
                                                pad_bytes);
        if (status != PSA_SUCCESS) {
          goto exit;
        }

        if (output_size < (input_length - 16 - pad_bytes)) {
          status = PSA_ERROR_BUFFER_TOO_SMALL;
          goto exit;
        }

        // Copy non-padding bytes
        memcpy(&output[full_blocks * 16], final_block, 16 - pad_bytes);
        *output_length = input_length - 16 - pad_bytes;
      } else {
        sli_psa_zeroize(dec_key, 32);
        *output_length = input_length - 16;
      }
      break;
#endif // PSA_WANT_ALG_CBC_PKCS7 || PSA_WANT_ALG_CBC_NO_PADDING
    default:
      return PSA_ERROR_NOT_SUPPORTED;
  }

#if defined(PSA_WANT_ALG_CBC_NO_PADDING) \
  || defined(PSA_WANT_ALG_CBC_PKCS7)     \
  || defined(PSA_WANT_ALG_OFB)           \
  || defined(PSA_WANT_ALG_CFB)           \
  || defined(PSA_WANT_ALG_ECB_NO_PADDING)
  exit:
#endif
  if (status != PSA_SUCCESS) {
    memset(output, 0, output_size);
    *output_length = 0;
    return status;
  } else {
    return PSA_SUCCESS;
  }

#else // PSA_WANT_ALG_* && PSA_WANT_KEY_TYPE_AES

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

#endif // PSA_WANT_ALG_* && PSA_WANT_KEY_TYPE_AES
}

psa_status_t sli_crypto_transparent_cipher_encrypt_setup(sli_crypto_transparent_cipher_operation_t *operation,
                                                         const psa_key_attributes_t *attributes,
                                                         const uint8_t *key_buffer,
                                                         size_t key_buffer_size,
                                                         psa_algorithm_t alg)
{
#if (defined(PSA_WANT_KEY_TYPE_AES)        \
  && (defined(PSA_WANT_ALG_ECB_NO_PADDING) \
  || defined(PSA_WANT_ALG_CTR)             \
  || defined(PSA_WANT_ALG_CFB)             \
  || defined(PSA_WANT_ALG_OFB)             \
  || defined(PSA_WANT_ALG_CCM)             \
  || defined(PSA_WANT_ALG_CBC_NO_PADDING)  \
  || defined(PSA_WANT_ALG_CBC_PKCS7)))

  if (operation == NULL || attributes == NULL || key_buffer == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // Reset context
  memset(operation, 0, sizeof(*operation));

  // Set up context
  memcpy(&operation->alg, &alg, sizeof(alg));
  operation->direction = PSA_CRYPTO_DRIVER_ENCRYPT;
  operation->processed_length = 0;

  // Validate combination of key and algorithm
  switch (alg) {
#if defined(PSA_WANT_ALG_ECB_NO_PADDING)
    case PSA_ALG_ECB_NO_PADDING:
#endif // PSA_WANT_ALG_ECB_NO_PADDING
#if defined(PSA_WANT_ALG_CTR)
    case PSA_ALG_CTR:
#endif // PSA_WANT_ALG_CTR
#if defined(PSA_WANT_ALG_CCM)
    case PSA_ALG_CCM_STAR_NO_TAG:
#endif // PSA_WANT_ALG_OFB
#if defined(PSA_WANT_ALG_CFB)
    case PSA_ALG_CFB:
#endif // PSA_WANT_ALG_CFB
#if defined(PSA_WANT_ALG_OFB)
    case PSA_ALG_OFB:
#endif // PSA_WANT_ALG_OFB
#if defined(PSA_WANT_ALG_CBC_NO_PADDING)
    case PSA_ALG_CBC_NO_PADDING:
#endif // PSA_WANT_ALG_CBC_NO_PADDING
#if defined(PSA_WANT_ALG_CBC_PKCS7)
    case PSA_ALG_CBC_PKCS7:
#endif // PSA_WANT_ALG_CBC_PKCS7
    if (psa_get_key_type(attributes) != PSA_KEY_TYPE_AES) {
      return PSA_ERROR_NOT_SUPPORTED;
    }
    break;
    default:
      return PSA_ERROR_NOT_SUPPORTED;
  }

  // Copy key into context
  switch (psa_get_key_bits(attributes)) {
    case 128:
      if (key_buffer_size < 16) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }
      memcpy(operation->key, key_buffer, 16);
      operation->key_len = 16;
      break;
    case 256:
      if (key_buffer_size < 32) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }
      memcpy(operation->key, key_buffer, 32);
      operation->key_len = 32;
      break;
    default:
      return PSA_ERROR_NOT_SUPPORTED;
  }

  return PSA_SUCCESS;

#else // PSA_WANT_ALG_AES && PSA_WANT_KEY_TYPE_AES

  (void)operation;
  (void)attributes;
  (void)key_buffer;
  (void)key_buffer_size;
  (void)alg;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_AES && PSA_WANT_KEY_TYPE_AES
}

psa_status_t sli_crypto_transparent_cipher_decrypt_setup(sli_crypto_transparent_cipher_operation_t *operation,
                                                         const psa_key_attributes_t *attributes,
                                                         const uint8_t *key_buffer,
                                                         size_t key_buffer_size,
                                                         psa_algorithm_t alg)
{
#if (defined(PSA_WANT_KEY_TYPE_AES)        \
  && (defined(PSA_WANT_ALG_ECB_NO_PADDING) \
  || defined(PSA_WANT_ALG_CTR)             \
  || defined(PSA_WANT_ALG_CFB)             \
  || defined(PSA_WANT_ALG_OFB)             \
  || defined(PSA_WANT_ALG_CCM)             \
  || defined(PSA_WANT_ALG_CBC_NO_PADDING)  \
  || defined(PSA_WANT_ALG_CBC_PKCS7)))

  if (operation == NULL || attributes == NULL || key_buffer == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // Reset context
  memset(operation, 0, sizeof(*operation));

  // Set up context
  memcpy(&operation->alg, &alg, sizeof(alg));
  operation->direction = PSA_CRYPTO_DRIVER_DECRYPT;
  operation->processed_length = 0;

  // Validate combination of key and algorithm
  switch (alg) {
#if defined(PSA_WANT_ALG_ECB_NO_PADDING)
    case PSA_ALG_ECB_NO_PADDING:
#endif // PSA_WANT_ALG_ECB_NO_PADDING
#if defined(PSA_WANT_ALG_CTR)
    case PSA_ALG_CTR:
#endif // PSA_WANT_ALG_CTR
#if defined(PSA_WANT_ALG_CCM)
    case PSA_ALG_CCM_STAR_NO_TAG:
#endif // PSA_WANT_ALG_CTR
#if defined(PSA_WANT_ALG_CFB)
    case PSA_ALG_CFB:
#endif // PSA_WANT_ALG_CFB
#if defined(PSA_WANT_ALG_OFB)
    case PSA_ALG_OFB:
#endif // PSA_WANT_ALG_OFB
#if defined(PSA_WANT_ALG_CBC_NO_PADDING)
    case PSA_ALG_CBC_NO_PADDING:
#endif // PSA_WANT_ALG_CBC_NO_PADDING
#if defined(PSA_WANT_ALG_CBC_PKCS7)
    case PSA_ALG_CBC_PKCS7:
#endif // PSA_WANT_ALG_CBC_PKCS7
    if (psa_get_key_type(attributes) != PSA_KEY_TYPE_AES) {
      return PSA_ERROR_NOT_SUPPORTED;
    }
    break;
    default:
      return PSA_ERROR_NOT_SUPPORTED;
  }

  psa_status_t status;

  // Copy key into context
  switch (psa_get_key_bits(attributes)) {
    case 128:
      if (key_buffer_size < 16) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }
      if (alg == PSA_ALG_ECB_NO_PADDING
          || alg == PSA_ALG_CBC_NO_PADDING
          || alg == PSA_ALG_CBC_PKCS7) {
        status = sl_crypto_aes_get_decryption_key(key_buffer, 16, operation->key, 16);
        if (status != PSA_SUCCESS) {
          return status;
        }
      } else {
        memcpy(operation->key, key_buffer, 16);
      }
      operation->key_len = 16;
      break;
    case 256:
      if (key_buffer_size < 32) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }
      if (alg == PSA_ALG_ECB_NO_PADDING
          || alg == PSA_ALG_CBC_NO_PADDING
          || alg == PSA_ALG_CBC_PKCS7) {
        status = sl_crypto_aes_get_decryption_key(key_buffer, 32, operation->key, 32);
        if (status != PSA_SUCCESS) {
          return status;
        }
      } else {
        memcpy(operation->key, key_buffer, 32);
      }
      operation->key_len = 32;
      break;
    default:
      return PSA_ERROR_NOT_SUPPORTED;
  }

  return PSA_SUCCESS;

#else // PSA_WANT_ALG_AES && PSA_WANT_KEY_TYPE_AES

  (void)operation;
  (void)attributes;
  (void)key_buffer;
  (void)key_buffer_size;
  (void)alg;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_AES && PSA_WANT_KEY_TYPE_AES
}

psa_status_t sli_crypto_transparent_cipher_set_iv(sli_crypto_transparent_cipher_operation_t *operation,
                                                  const uint8_t *iv,
                                                  size_t iv_length)
{
#if (defined(PSA_WANT_KEY_TYPE_AES)       \
  && (defined(PSA_WANT_ALG_CTR)           \
  || defined(PSA_WANT_ALG_CFB)            \
  || defined(PSA_WANT_ALG_OFB)            \
  || defined(PSA_WANT_ALG_CCM)            \
  || defined(PSA_WANT_ALG_CBC_NO_PADDING) \
  || defined(PSA_WANT_ALG_CBC_PKCS7)))

  if (operation == NULL || iv == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  if (iv_length > sizeof(operation->iv)) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  if (operation->iv_len != 0) {
    // IV was set previously
    return PSA_ERROR_BAD_STATE;
  }

  if (operation->key_len == 0) {
    // context hasn't been properly initialised
    return PSA_ERROR_BAD_STATE;
  }

#if defined(PSA_WANT_ALG_CCM)
  if (operation->alg == PSA_ALG_CCM_STAR_NO_TAG) {
    // Preformat the IV for CCM*-no-tag here, such that the remainder
    // of the processing for this algorithm boils down to AES-CTR
    if (iv_length != 13) {
      return PSA_ERROR_INVALID_ARGUMENT;
    }
    operation->iv[0] = 1;
    memcpy(&operation->iv[1], iv, iv_length);
    operation->iv[14] = 0;
    operation->iv[15] = 1;
    operation->iv_len = 16;
  } else
#endif
  if (operation->alg != PSA_ALG_ECB_NO_PADDING) {
    if (iv_length != 16) {
      return PSA_ERROR_INVALID_ARGUMENT;
    }
    operation->iv_len = iv_length;
    memcpy(operation->iv, iv, iv_length);
  } else {
    if (iv_length > 0) {
      return PSA_ERROR_INVALID_ARGUMENT;
    }
  }

  return PSA_SUCCESS;

#else // PSA_WANT_ALG_AES && PSA_WANT_KEY_TYPE_AES

  (void)operation;
  (void)iv;
  (void)iv_length;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_AES && PSA_WANT_KEY_TYPE_AES
}

psa_status_t sli_crypto_transparent_cipher_update(sli_crypto_transparent_cipher_operation_t *operation,
                                                  const uint8_t *input,
                                                  size_t input_length,
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
  || defined(PSA_WANT_ALG_CBC_PKCS7)))

  // Argument check
  if (operation == NULL
      || (input == NULL && input_length > 0)
      || (output == NULL && output_size > 0)
      || output_length == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
  bool lagging;
  size_t bytes_to_boundary = 16 - (operation->processed_length % 16);
  size_t actual_output_length = 0;

  // We need to cache (not return) the whole last block for decryption with
  // padding, otherwise it won't be possible to remove a potential padding block
  // during finish.
  bool cache_full_block = (operation->alg == PSA_ALG_CBC_PKCS7
                           && operation->direction == PSA_CRYPTO_DRIVER_DECRYPT);

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
      // Don't overwrite the streaming block yet if it's currently full.
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
#if defined(PSA_WANT_ALG_ECB_NO_PADDING)
    case PSA_ALG_ECB_NO_PADDING:
    {
      // Read in up to full streaming input block
      if (bytes_to_boundary != 16) {
        memcpy(&operation->streaming_block[operation->processed_length % 16],
               input,
               bytes_to_boundary);
        input += bytes_to_boundary;
        input_length -= bytes_to_boundary;

        status = sl_crypto_aes_crypt_ecb_single(operation->key,
                                                operation->key_len,
                                                operation->direction,
                                                operation->streaming_block,
                                                output);
        if (status != PSA_SUCCESS) {
          goto exit;
        }

        output += 16;
        actual_output_length += 16;
        operation->processed_length += bytes_to_boundary;
      }

      // Do multi-block operation if applicable
      if (input_length >= 16) {
        size_t operation_size = (input_length / 16) * 16;

        for (size_t i = 0; i < operation_size; i += 16) {
          status = sl_crypto_aes_crypt_ecb_single(operation->key,
                                                  operation->key_len,
                                                  operation->direction,
                                                  input,
                                                  output);
          if (status != PSA_SUCCESS) {
            goto exit;
          }

          input += 16;
          input_length -= 16;
          output += 16;
          actual_output_length += 16;
          operation->processed_length += 16;
        }
      }

      // What's left over in the input buffer will be cleaned up after switch-case
      break;
    }
#endif // PSA_WANT_ALG_ECB_NO_PADDING
#if defined(PSA_WANT_ALG_CBC_NO_PADDING) || defined(PSA_WANT_ALG_CBC_PKCS7)
    case PSA_ALG_CBC_NO_PADDING:
    case PSA_ALG_CBC_PKCS7:
    {
      if (bytes_to_boundary != 16) {
        memcpy(&operation->streaming_block[operation->processed_length % 16],
               input,
               bytes_to_boundary);

        if (cache_full_block && (bytes_to_boundary == input_length)) {
          // Don't process the streaming block if there is no more input data
          status = PSA_SUCCESS;
        } else {
          status = sl_crypto_aes_crypt_cbc(operation->key,
                                           operation->key_len,
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
        status = sl_crypto_aes_crypt_cbc(operation->key,
                                         operation->key_len,
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
          status = sl_crypto_aes_crypt_cbc(operation->key,
                                           operation->key_len,
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
    }
#endif // PSA_WANT_ALG_CBC_PKCS7 || PSA_WANT_ALG_CBC_NO_PADDING
#if defined(PSA_WANT_ALG_CCM)
    case PSA_ALG_CCM_STAR_NO_TAG:
      // Explicit fallthrough
#endif // PSA_WANT_ALG_CCM
#if defined(PSA_WANT_ALG_CTR) || defined(PSA_WANT_ALG_CCM)
    case PSA_ALG_CTR:
    {
      size_t offset = operation->processed_length % 16;

      status = sl_crypto_aes_crypt_ctr(operation->key,
                                       operation->key_len,
                                       input_length,
                                       &offset,
                                       operation->iv,
                                       operation->streaming_block,
                                       input,
                                       output);

      if (status != PSA_SUCCESS) {
        goto exit;
      }

      actual_output_length += input_length;
      operation->processed_length += input_length;
      input_length -= input_length;

      break;
    }
#endif // PSA_WANT_ALG_CTR || PSA_WANT_ALG_CCM
#if defined(PSA_WANT_ALG_CFB)
    case PSA_ALG_CFB:
    {
      size_t offset = operation->processed_length % 16;

      status = sl_crypto_aes_crypt_cfb128(operation->key,
                                          operation->key_len,
                                          operation->direction,
                                          input_length,
                                          &offset,
                                          operation->iv,
                                          input,
                                          output);

      if (status != PSA_SUCCESS) {
        goto exit;
      }

      actual_output_length += input_length;
      operation->processed_length += input_length;
      input_length -= input_length;

      break;
    }
#endif // PSA_WANT_ALG_CFB
#if defined(PSA_WANT_ALG_OFB)
    case PSA_ALG_OFB:
    {
      size_t offset = operation->processed_length % 16;

      status = sl_crypto_aes_crypt_ofb(operation->key,
                                       operation->key_len,
                                       input_length,
                                       &offset,
                                       operation->iv,
                                       input,
                                       output);

      if (status != PSA_SUCCESS) {
        goto exit;
      }

      actual_output_length += input_length;
      operation->processed_length += input_length;
      input_length -= input_length;

      break;
    }
#endif // PSA_WANT_ALG_OFB
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
  if (status != PSA_SUCCESS) {
    *output_length = 0;
    return status;
  } else {
    *output_length = actual_output_length;
    return PSA_SUCCESS;
  }

#else // PSA_WANT_ALG_AES && PSA_WANT_KEY_TYPE_AES

  (void)operation;
  (void)input;
  (void)input_length;
  (void)output;
  (void)output_size;
  (void)output_length;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_AES && PSA_WANT_KEY_TYPE_AES
}

psa_status_t sli_crypto_transparent_cipher_finish(sli_crypto_transparent_cipher_operation_t *operation,
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
  || defined(PSA_WANT_ALG_CBC_PKCS7)))

  // Finalize cipher operation. This will only output data for algorithms which include padding.
  // This is currently only AES-CBC with PKCS#7.
  psa_status_t psa_status = PSA_ERROR_BAD_STATE;

  // Argument check
  if (operation == NULL
      || output_length == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  switch (operation->alg) {
#if defined(PSA_WANT_ALG_ECB_NO_PADDING) || defined(PSA_WANT_ALG_CBC_NO_PADDING)
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
#endif // PSA_WANT_ALG_ECB_NO_PADDING || PSA_WANT_ALG_CBC_NO_PADDING
#if defined(PSA_WANT_ALG_CBC_PKCS7)
    case PSA_ALG_CBC_PKCS7:
      if ((output == NULL && output_size > 0)) {
        psa_status = PSA_ERROR_INVALID_ARGUMENT;
        break;
      }

      // Calculate padding, update, output final block
      if (operation->direction == PSA_CRYPTO_DRIVER_ENCRYPT) {
        if (output_size < 16) {
          psa_status = PSA_ERROR_BUFFER_TOO_SMALL;
          break;
        }
        size_t padding_bytes = 16 - (operation->processed_length % 16);
        memset(&operation->streaming_block[16 - padding_bytes], padding_bytes, padding_bytes);

        psa_status = sl_crypto_aes_crypt_cbc(operation->key,
                                             operation->key_len,
                                             PSA_CRYPTO_DRIVER_ENCRYPT,
                                             16,
                                             operation->iv,
                                             operation->streaming_block,
                                             output);

        if (psa_status != PSA_SUCCESS) {
          *output_length = 0;
          psa_status = PSA_ERROR_HARDWARE_FAILURE;
        } else {
          *output_length = 16;
        }
      } else {
        // Expect full-block input
        if (operation->processed_length % 16 != 0 || operation->processed_length < 16) {
          psa_status = PSA_ERROR_INVALID_ARGUMENT;
          break;
        }

        uint8_t out_buf[16];

        // Decrypt the last block
        psa_status = sl_crypto_aes_crypt_cbc(operation->key,
                                             operation->key_len,
                                             PSA_CRYPTO_DRIVER_DECRYPT,
                                             16,
                                             operation->iv,
                                             operation->streaming_block,
                                             out_buf);

        if (psa_status != PSA_SUCCESS) {
          *output_length = 0;
          psa_status = PSA_ERROR_HARDWARE_FAILURE;
          break;
        }

        size_t padding_bytes = out_buf[15];
        psa_status = sli_psa_validate_pkcs7_padding(out_buf,
                                                    16,
                                                    padding_bytes);

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
      break;
#endif // PSA_WANT_ALG_CBC_PKCS7
#if defined(PSA_WANT_ALG_CTR) || defined(PSA_WANT_ALG_CFB) || defined(PSA_WANT_ALG_OFB) || defined(PSA_WANT_ALG_CCM)
    case PSA_ALG_CTR:
    case PSA_ALG_CCM_STAR_NO_TAG:
    case PSA_ALG_CFB:
    case PSA_ALG_OFB:
      // Actual stream ciphers: nothing to do here.
      psa_status = PSA_SUCCESS;
      *output_length = 0;
      break;
#endif // PSA_WANT_ALG_*FB
    default:
      psa_status = PSA_ERROR_BAD_STATE;
  }

#if !defined(PSA_WANT_ALG_CBC_PKCS7)
  (void)output;
  (void)output_size;
#endif // PSA_WANT_ALG_CBC_PKCS7

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

psa_status_t sli_crypto_transparent_cipher_abort(sli_crypto_transparent_cipher_operation_t *operation)
{
#if (defined(PSA_WANT_KEY_TYPE_AES)        \
  && (defined(PSA_WANT_ALG_ECB_NO_PADDING) \
  || defined(PSA_WANT_ALG_CTR)             \
  || defined(PSA_WANT_ALG_CFB)             \
  || defined(PSA_WANT_ALG_OFB)             \
  || defined(PSA_WANT_ALG_CCM)             \
  || defined(PSA_WANT_ALG_CBC_NO_PADDING)  \
  || defined(PSA_WANT_ALG_CBC_PKCS7)))

  // Wipe context
  memset(operation, 0, sizeof(*operation));

  return PSA_SUCCESS;

#else // PSA_WANT_ALG_AES && PSA_WANT_KEY_TYPE_AES

  (void)operation;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_AES && PSA_WANT_KEY_TYPE_AES
}

#if (defined(PSA_WANT_KEY_TYPE_AES)        \
  && (defined(PSA_WANT_ALG_ECB_NO_PADDING) \
  || defined(PSA_WANT_ALG_CTR)             \
  || defined(PSA_WANT_ALG_CFB)             \
  || defined(PSA_WANT_ALG_OFB)             \
  || defined(PSA_WANT_ALG_CCM)             \
  || defined(PSA_WANT_ALG_CBC_NO_PADDING)  \
  || defined(PSA_WANT_ALG_CBC_PKCS7)))

/*
 * Store a decryption key in out_key based on the in_key.
 *
 * This is needed for ECB and CBC since the CRYPTO accelerator cannot decrypt
 * directly with the normal key, but rather needs a particular decrypt key that
 * it generates itself after one round of encryption.
 */
static psa_status_t sl_crypto_aes_get_decryption_key(const uint8_t *in_key,
                                                     size_t in_key_size,
                                                     uint8_t *out_key,
                                                     size_t out_key_size)
{
  CORE_DECLARE_IRQ_STATE;

  if (in_key == NULL
      || out_key == NULL
      || out_key_size != in_key_size) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  CRYPTO_TypeDef *device = crypto_management_acquire();
  device->WAC = 0;
  device->CTRL = 0;

  CORE_ENTER_CRITICAL();
  CRYPTO_KeyBufWriteUnaligned(device,
                              in_key,
                              (in_key_size == 16U) ? cryptoKey128Bits
                              : cryptoKey256Bits);
  CORE_EXIT_CRITICAL();

  // We need to perform one encrypt operation in order to get the
  // proper decryption key.
  device->CMD = CRYPTO_CMD_INSTR_AESENC;
  while ((device->STATUS & CRYPTO_STATUS_INSTRRUNNING) != 0UL) {
    // Wait for operation to complete
  }

  CORE_ENTER_CRITICAL();
  CRYPTO_KeyReadUnaligned(device,
                          out_key,
                          (out_key_size == 16U) ? cryptoKey128Bits
                          : cryptoKey256Bits);
  CORE_EXIT_CRITICAL();

  crypto_management_release(device);

  return PSA_SUCCESS;
}

#endif // PSA_WANT_ALG_AES && PSA_WANT_KEY_TYPE_*

#if defined(PSA_WANT_KEY_TYPE_AES) \
  && (defined(PSA_WANT_ALG_ECB_NO_PADDING) || defined(PSA_WANT_ALG_OFB))

/*
 * AES-ECB encryption/decryption of a single block
 */
static psa_status_t sl_crypto_aes_crypt_ecb_single(const uint8_t *key_buffer,
                                                   size_t key_buffer_size,
                                                   psa_encrypt_or_decrypt_t mode,
                                                   const uint8_t input[16],
                                                   uint8_t output[16])
{
  CORE_DECLARE_IRQ_STATE;

  CRYPTO_TypeDef *device = crypto_management_acquire();
  device->WAC = 0;
  device->CTRL = 0;

  // Write input data to accelerator
  CORE_ENTER_CRITICAL();
  CRYPTO_KeyBufWriteUnaligned(device,
                              key_buffer,
                              (key_buffer_size == 16U) ? cryptoKey128Bits
                              : cryptoKey256Bits);
  CRYPTO_DataWriteUnaligned(&device->DATA0, (uint8_t *)input);
  CORE_EXIT_CRITICAL();

  if (mode == PSA_CRYPTO_DRIVER_ENCRYPT) {
    device->CMD = CRYPTO_CMD_INSTR_AESENC;
  } else {
    device->CMD = CRYPTO_CMD_INSTR_AESDEC;
  }

  while ((device->STATUS & CRYPTO_STATUS_INSTRRUNNING) != 0UL) {
    // Wait for the operation to complete
  }

  CORE_ENTER_CRITICAL();
  CRYPTO_DataReadUnaligned(&device->DATA0, (uint8_t *)output);
  CORE_EXIT_CRITICAL();

  crypto_management_release(device);

  return PSA_SUCCESS;
}

#endif // PSA_WANT_ALG_AES && (PSA_WANT_KEY_TYPE_ECB_NO_PADDING || PSA_WANT_ALG_OFB)

#if defined(PSA_WANT_KEY_TYPE_AES)         \
  && (defined(PSA_WANT_ALG_CBC_NO_PADDING) \
  || defined(PSA_WANT_ALG_CBC_PKCS7))

/*
 * AES-CBC buffer encryption/decryption
 */
static psa_status_t sl_crypto_aes_crypt_cbc(const uint8_t *key_buffer,
                                            size_t key_buffer_size,
                                            psa_encrypt_or_decrypt_t mode,
                                            size_t length,
                                            uint8_t iv[16],
                                            const uint8_t *input,
                                            uint8_t *output)
{
  CORE_DECLARE_IRQ_STATE;
  size_t processed = 0;

  CRYPTO_TypeDef *device = crypto_management_acquire();
  device->WAC = 0;
  device->CTRL = 0;

  // Set the encryption/decryption key and input data
  CORE_ENTER_CRITICAL();
  CRYPTO_KeyBufWriteUnaligned(device,
                              key_buffer,
                              (key_buffer_size == 16U) ? cryptoKey128Bits
                              : cryptoKey256Bits);
  if (mode == PSA_CRYPTO_DRIVER_ENCRYPT) {
    CRYPTO_DataWriteUnaligned(&device->DATA0, (uint8_t *)iv);
  } else {
    CRYPTO_DataWriteUnaligned(&device->DATA2, (uint8_t *)iv);
  }
  CORE_EXIT_CRITICAL();

  uint8_t output_buffer[16] = { 0 };

  while (processed < length) {
    if (mode == PSA_CRYPTO_DRIVER_ENCRYPT) {
      CORE_ENTER_CRITICAL();
      CRYPTO_DataWriteUnaligned(&device->DATA0XOR, (uint8_t *)(&input[processed]));

      // Initiate encryption and wait for it to complete
      device->CMD = CRYPTO_CMD_INSTR_AESENC;
      while ((device->STATUS & CRYPTO_STATUS_INSTRRUNNING) != 0) ;

      // Flush the output buffer for each block
      if (processed >= 16) {
        memcpy(&output[processed - 16], output_buffer, 16);
      }

      CRYPTO_DataReadUnaligned(&device->DATA0, output_buffer);
      CORE_EXIT_CRITICAL();
    } else {
      // Decrypt input block, XOR IV to decrypted text, set ciphertext as next IV
      CORE_ENTER_CRITICAL();
      CRYPTO_DataWriteUnaligned(&device->DATA0, (uint8_t *)(&input[processed]));

      // Initiate decryption sequence and wait for it to complete
      CRYPTO_EXECUTE_4(device,
                       CRYPTO_CMD_INSTR_DATA0TODATA1,
                       CRYPTO_CMD_INSTR_AESDEC,
                       CRYPTO_CMD_INSTR_DATA2TODATA0XOR,
                       CRYPTO_CMD_INSTR_DATA1TODATA2);

      while ((device->STATUS & CRYPTO_STATUS_SEQRUNNING) != 0) ;

      // The output pointer will be before the input pointer on single-shot
      // decrypt (and at the same point for multi-part), so we don't need
      // to use the output buffer.
      CRYPTO_DataReadUnaligned(&device->DATA0, &output[processed]);
      CORE_EXIT_CRITICAL();
    }
    processed += 16;
  }

  if (processed >= 16) {
    if (mode == PSA_CRYPTO_DRIVER_ENCRYPT) {
      // Flush the output buffer and update the supplied IV
      memcpy(&output[processed - 16], output_buffer, 16);
      memcpy(iv, &output[processed - 16], 16);
    } else {
      CORE_ENTER_CRITICAL();
      // Update the supplied IV
      CRYPTO_DataReadUnaligned(&device->DATA2, iv);
      CORE_EXIT_CRITICAL();
    }
  }

  crypto_management_release(device);

  return PSA_SUCCESS;
}

#endif // PSA_WANT_ALG_AES && PSA_WANT_KEY_TYPE_CBC_*

#if defined(PSA_WANT_KEY_TYPE_AES) && defined(PSA_WANT_ALG_CFB)

/*
 * AES-CFB128 buffer encryption/decryption
 */
static psa_status_t sl_crypto_aes_crypt_cfb128(const uint8_t *key_buffer,
                                               size_t key_buffer_size,
                                               psa_encrypt_or_decrypt_t mode,
                                               size_t length,
                                               size_t *iv_off,
                                               uint8_t iv[16],
                                               const uint8_t *input,
                                               uint8_t *output)
{
  size_t n = iv_off ? *iv_off : 0;
  size_t processed = 0;
  CORE_DECLARE_IRQ_STATE;

  if (n > 15) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  uint8_t output_buffer[16] = { 0 };

  while (processed < length) {
    if (n > 0) {
      // Start by filling up the IV
      if (mode == PSA_CRYPTO_DRIVER_ENCRYPT) {
        iv[n] = (uint8_t)(iv[n] ^ input[processed]);
        output[processed] = iv[n];
      } else {
        uint8_t c = input[processed];
        output[processed] = (uint8_t)(c ^ iv[n]);
        iv[n] = (uint8_t) c;
      }
      n = (n + 1) & 0x0F;
      processed++;
      continue;
    } else {
      // Process one ore more blocks of data
      CRYPTO_TypeDef *device = crypto_management_acquire();
      device->WAC = 0;
      device->CTRL = 0;

      CORE_ENTER_CRITICAL();
      CRYPTO_KeyBufWriteUnaligned(device,
                                  key_buffer,
                                  (key_buffer_size == 16U) ? cryptoKey128Bits
                                  : cryptoKey256Bits);
      CRYPTO_DataWriteUnaligned(&device->DATA0, (uint8_t *)iv);
      CORE_EXIT_CRITICAL();

      // Encryption: encrypt IV, encIV xor input -> output and IV
      // Decryption: encrypt IV, encIV xor input -> output, input -> IV
      size_t iterations = (length - processed) / 16;
      for (size_t i = 0; i < iterations; i++ ) {
        device->CMD = CRYPTO_CMD_INSTR_AESENC;
        while ((device->STATUS & CRYPTO_STATUS_INSTRRUNNING) != 0) ;

        CORE_ENTER_CRITICAL();
        if (mode == PSA_CRYPTO_DRIVER_ENCRYPT) {
          CRYPTO_DataWriteUnaligned(&device->DATA0XOR,
                                    (uint8_t *)(&input[processed]));
          // Flush the output buffer
          if (processed >= 16) {
            memcpy(&output[processed - 16], output_buffer, 16);
          }
          CRYPTO_DataReadUnaligned(&device->DATA0, output_buffer);
        } else {
          CRYPTO_DataWriteUnaligned(&device->DATA1,
                                    (uint8_t *)(&input[processed]));
          device->CMD = CRYPTO_CMD_INSTR_DATA1TODATA0XOR;
          // Flush the output buffer
          if (processed >= 16) {
            memcpy(&output[processed - 16], output_buffer, 16);
          }
          CRYPTO_DataReadUnaligned(&device->DATA0, output_buffer);
          device->CMD = CRYPTO_CMD_INSTR_DATA1TODATA0;
        }
        CORE_EXIT_CRITICAL();
        processed += 16;
      }

      CORE_ENTER_CRITICAL();
      CRYPTO_DataReadUnaligned(&device->DATA0, iv);
      CORE_EXIT_CRITICAL();

      // Create a buffer for the last input block
      uint8_t input_buffer[16];
      memcpy(input_buffer, &input[processed], 16);

      // Flush the output buffer
      if (processed >= 16) {
        memcpy(&output[processed - 16], output_buffer, 16);
      }

      while (length - processed > 0) {
        if (n == 0) {
          device->CMD = CRYPTO_CMD_INSTR_AESENC;
          while ((device->STATUS & CRYPTO_STATUS_INSTRRUNNING) != 0) ;
          CORE_ENTER_CRITICAL();
          CRYPTO_DataReadUnaligned(&device->DATA0, iv);
          CORE_EXIT_CRITICAL();
        }

        // Save remainder to iv
        if (mode == PSA_CRYPTO_DRIVER_ENCRYPT) {
          output[processed] = (uint8_t)(iv[n] ^ input_buffer[n]);
          iv[n] = output[processed];
        } else {
          uint8_t c = input_buffer[n];
          output[processed] = (uint8_t)(c ^ iv[n]);
          iv[n] = (uint8_t) c;
        }
        n = (n + 1) & 0x0F;
        processed++;
      }

      crypto_management_release(device);
    }
  }

  if (iv_off) {
    *iv_off = n;
  }

  return PSA_SUCCESS;
}

#endif // PSA_WANT_ALG_AES && PSA_WANT_KEY_TYPE_CFB

#if defined(PSA_WANT_KEY_TYPE_AES) && defined(PSA_WANT_ALG_OFB)

/*
 * AES-OFB (Output Feedback Mode) buffer encryption/decryption
 */
static psa_status_t sl_crypto_aes_crypt_ofb(const uint8_t *key_buffer,
                                            size_t key_buffer_size,
                                            size_t length,
                                            size_t *iv_off,
                                            uint8_t iv[16],
                                            const uint8_t *input,
                                            uint8_t *output)
{
  size_t n = iv_off ? *iv_off : 0;

  if (n > 15) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  psa_status_t status;
  uint8_t output_buffer[16] = { 0 };
  uint8_t input_buffer[16] = { 0 };
  size_t processed = 0;

  memcpy(input_buffer, &input[processed], 16);

  while (length--) {
    if (n == 0) {
      status = sl_crypto_aes_crypt_ecb_single(key_buffer,
                                              key_buffer_size,
                                              PSA_CRYPTO_DRIVER_ENCRYPT,
                                              iv,
                                              iv);

      if (status != PSA_SUCCESS) {
        return status;
      }
    }

    output_buffer[processed % 16] = input_buffer[processed % 16] ^ iv[n];

    n = (n + 1) & 0x0F;
    processed++;

    if (processed % 16 == 0) {
      // Update input buffer
      memcpy(input_buffer, &input[processed], 16);
      // Flush output buffer
      memcpy(&output[processed - 16], output_buffer, 16);
    }
  }

  // Flush remaining bytes in output buffer
  memcpy(&output[processed & (~0xF)], output_buffer, processed & 0xF);

  if (iv_off) {
    *iv_off = n;
  }

  return PSA_SUCCESS;
}

#endif // PSA_WANT_ALG_AES && PSA_WANT_KEY_TYPE_OFB

#if defined(PSA_WANT_KEY_TYPE_AES) && (defined(PSA_WANT_ALG_CTR) || defined(PSA_WANT_ALG_CCM))

/*
 * AES-CTR buffer encryption/decryption
 */
static psa_status_t sl_crypto_aes_crypt_ctr(const uint8_t *key_buffer,
                                            size_t key_buffer_size,
                                            size_t length,
                                            size_t *nc_off,
                                            uint8_t nonce_counter[16],
                                            uint8_t stream_block[16],
                                            const uint8_t *input,
                                            uint8_t *output)
{
  size_t n = nc_off ? *nc_off : 0;
  size_t processed = 0;
  CORE_DECLARE_IRQ_STATE;

  uint8_t output_buffer[16] = { 0 };

  while (processed < length) {
    if (n > 0) {
      // Start by filling up the IV
      output[processed] = (uint8_t)(input[processed] ^ stream_block[n]);
      n = (n + 1) & 0x0F;
      processed++;
    } else {
      // Process one ore more blocks of data
      CRYPTO_TypeDef *device = crypto_management_acquire();
      device->WAC = 0;
      device->CTRL = CRYPTO_CTRL_INCWIDTH_INCWIDTH4;

      CORE_ENTER_CRITICAL();
      CRYPTO_KeyBufWriteUnaligned(device,
                                  key_buffer,
                                  (key_buffer_size == 16U) ? cryptoKey128Bits
                                  : cryptoKey256Bits);
      CRYPTO_DataWriteUnaligned(&device->DATA1, (uint8_t *)nonce_counter);
      CORE_EXIT_CRITICAL();

      // Strategy: encrypt nonce, encNonce xor input -> output, inc(nonce)
      size_t iterations = (length - processed) / 16;
      for (size_t i = 0; i < iterations; i++ ) {
        CORE_ENTER_CRITICAL();
        CRYPTO_EXECUTE_3(device,
                         CRYPTO_CMD_INSTR_DATA1TODATA0,
                         CRYPTO_CMD_INSTR_AESENC,
                         CRYPTO_CMD_INSTR_DATA1INC);

        while ((device->STATUS & CRYPTO_STATUS_SEQRUNNING) != 0) ;

        CRYPTO_DataWriteUnaligned(&device->DATA0XOR,
                                  (uint8_t *)(&input[processed]));

        // Flush the output buffer for each block
        if (processed >= 16) {
          memcpy(&output[processed - 16], output_buffer, 16);
        }

        CRYPTO_DataReadUnaligned(&device->DATA0, output_buffer);
        CORE_EXIT_CRITICAL();

        processed += 16;
      }

      // Create a buffer for the last (partial) input block
      uint8_t input_buffer[16];
      memcpy(input_buffer, &input[processed], 16);

      // Flush the output buffer for the last whole block
      if (processed >= 16) {
        memcpy(&output[processed - 16], output_buffer, 16);
      }

      while (length - processed > 0) {
        if (n == 0) {
          CORE_ENTER_CRITICAL();
          CRYPTO_EXECUTE_3(device,
                           CRYPTO_CMD_INSTR_DATA1TODATA0,
                           CRYPTO_CMD_INSTR_AESENC,
                           CRYPTO_CMD_INSTR_DATA1INC);

          while ((device->STATUS & CRYPTO_STATUS_SEQRUNNING) != 0) ;

          CRYPTO_DataReadUnaligned(&device->DATA0, stream_block);
          CORE_EXIT_CRITICAL();
        }

        // Save remainder to iv
        output[processed] = input_buffer[n] ^ stream_block[n];
        n = (n + 1) & 0x0F;
        processed++;
      }

      // Update nonce counter
      CORE_ENTER_CRITICAL();
      CRYPTO_DataReadUnaligned(&device->DATA1, nonce_counter);
      CORE_EXIT_CRITICAL();

      crypto_management_release(device);
    }
  }

  if (nc_off) {
    *nc_off = n;
  }

  return PSA_SUCCESS;
}

#endif // PSA_WANT_KEY_TYPE_AES && (PSA_WANT_ALG_CTR || PSA_WANT_ALG_CCM)

#endif // defined(CRYPTO_PRESENT)
