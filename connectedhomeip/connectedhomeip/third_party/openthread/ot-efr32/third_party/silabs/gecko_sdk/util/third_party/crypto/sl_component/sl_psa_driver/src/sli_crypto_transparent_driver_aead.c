/***************************************************************************//**
 * @file
 * @brief Silicon Labs PSA Crypto Transparent Driver AEAD functions.
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

/*
 *  NIST SP800-38C compliant CCM implementation
 *
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include "em_device.h"

#if defined(CRYPTO_PRESENT)

#include "sli_crypto_transparent_types.h"
#include "sli_crypto_transparent_functions.h"
#include "sli_psa_driver_common.h"
#include "crypto_management.h"
#include "psa/crypto.h"
#include "em_crypto.h"
#include "em_core.h"
#include <string.h>

#if defined(PSA_WANT_ALG_CCM)

//sli_ccm_crypt_and_tag has been taken from mbed TLS and adapted to use the
//CRYPTO accelerator, hence the above copyright notice.

static psa_status_t sli_ccm_crypt_and_tag(sli_crypto_transparent_aead_operation_t *operation,
                                          psa_encrypt_or_decrypt_t mode,
                                          size_t length,
                                          const unsigned char *iv,
                                          size_t iv_len,
                                          const unsigned char *add,
                                          size_t add_len,
                                          const unsigned char *input,
                                          unsigned char *output,
                                          unsigned char *tag,
                                          size_t tag_len);
#endif // PSA_WANT_ALG_CCM

#if defined(PSA_WANT_ALG_GCM)
static void sli_gcm_crypt_and_tag(sli_crypto_transparent_aead_operation_t *operation,
                                  psa_encrypt_or_decrypt_t mode,
                                  size_t length,
                                  const unsigned char *iv,
                                  size_t iv_len,
                                  const unsigned char *add,
                                  size_t add_len,
                                  const unsigned char *input,
                                  unsigned char *output,
                                  size_t tag_len,
                                  unsigned char *tag);
#endif // PSA_WANT_ALG_GCM

#if defined(PSA_WANT_ALG_CCM) || defined(PSA_WANT_ALG_GCM)

static psa_status_t check_aead_parameters(const psa_key_attributes_t *attributes,
                                          psa_algorithm_t alg,
                                          size_t nonce_length,
                                          size_t additional_data_length)
{
#if !defined(PSA_WANT_ALG_GCM)
  (void) additional_data_length;
#endif // PSA_WANT_ALG_GCM
  size_t tag_length = PSA_AEAD_TAG_LENGTH(psa_get_key_type(attributes),
                                          psa_get_key_bits(attributes),
                                          alg);

  switch (PSA_ALG_AEAD_WITH_SHORTENED_TAG(alg, 0)) {
#if defined(PSA_WANT_ALG_CCM)
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 0):
      if (psa_get_key_type(attributes) != PSA_KEY_TYPE_AES) {
        return PSA_ERROR_NOT_SUPPORTED;
      }
      if (tag_length < 4
          || tag_length > 16
          || tag_length % 2 != 0
          || nonce_length < 7
          || nonce_length > 13) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }
      break;
#endif // PSA_WANT_ALG_CCM
#if defined(PSA_WANT_ALG_GCM)
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, 0):
      if (psa_get_key_type(attributes) != PSA_KEY_TYPE_AES) {
        return PSA_ERROR_NOT_SUPPORTED;
      }
      // AD are limited to 2^64 bits, so 2^61 bytes.
      // We need not check if SIZE_MAX (max of size_t) is less than 2^61 (0x2000000000000000)
#if SIZE_MAX > 0x2000000000000000ull
      if (additional_data_length >> 61 != 0) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }
#else // SIZE_MAX > 0x2000000000000000ull
      (void) additional_data_length;
#endif // SIZE_MAX > 0x2000000000000000ull
      if (tag_length > 16 || tag_length < 4) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }
      if (nonce_length == 0) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }
#if !defined(SLI_PSA_SUPPORT_GCM_IV_CALCULATION)
      if (nonce_length != 12) {
        return PSA_ERROR_NOT_SUPPORTED;
      }
#endif // ! SLI_PSA_SUPPORT_GCM_IV_CALCULATION
      break;
#endif // PSA_WANT_ALG_GCM
    default:
      return PSA_ERROR_NOT_SUPPORTED;
  }

  switch (psa_get_key_bits(attributes)) {
    case 128:
      break;
    case 192:
      return PSA_ERROR_NOT_SUPPORTED;
    case 256:
      break;
    default:
      return PSA_ERROR_INVALID_ARGUMENT;
  }

  return PSA_SUCCESS;
}

#endif // PSA_WANT_ALG_CCM || PSA_WANT_ALG_GCM

psa_status_t sli_crypto_transparent_aead_encrypt(const psa_key_attributes_t *attributes,
                                                 const uint8_t *key_buffer,
                                                 size_t key_buffer_size,
                                                 psa_algorithm_t alg,
                                                 const uint8_t *nonce,
                                                 size_t nonce_length,
                                                 const uint8_t *additional_data,
                                                 size_t additional_data_length,
                                                 const uint8_t *plaintext,
                                                 size_t plaintext_length,
                                                 uint8_t *ciphertext,
                                                 size_t ciphertext_size,
                                                 size_t *ciphertext_length)
{
#if defined(PSA_WANT_ALG_CCM) || defined(PSA_WANT_ALG_GCM)
  if (ciphertext_size <= plaintext_length) {
    return PSA_ERROR_BUFFER_TOO_SMALL;
  }

  size_t tag_length = 0;
  psa_status_t psa_status = sli_crypto_transparent_aead_encrypt_tag(
    attributes, key_buffer, key_buffer_size, alg,
    nonce, nonce_length,
    additional_data, additional_data_length,
    plaintext, plaintext_length,
    ciphertext, plaintext_length, ciphertext_length,
    &ciphertext[plaintext_length], ciphertext_size - plaintext_length, &tag_length);

  if (psa_status == PSA_SUCCESS) {
    *ciphertext_length += tag_length;
  }

  return psa_status;
#else // PSA_WANT_ALG_CCM || PSA_WANT_ALG_GCM

  (void)attributes;
  (void)key_buffer;
  (void)key_buffer_size;
  (void)alg;
  (void)nonce;
  (void)nonce_length;
  (void)additional_data;
  (void)additional_data_length;
  (void)plaintext;
  (void)plaintext_length;
  (void)ciphertext;
  (void)ciphertext_size;
  (void)ciphertext_length;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_CCM || PSA_WANT_ALG_GCM
}

psa_status_t sli_crypto_transparent_aead_decrypt(const psa_key_attributes_t *attributes,
                                                 const uint8_t *key_buffer,
                                                 size_t key_buffer_size,
                                                 psa_algorithm_t alg,
                                                 const uint8_t *nonce,
                                                 size_t nonce_length,
                                                 const uint8_t *additional_data,
                                                 size_t additional_data_length,
                                                 const uint8_t *ciphertext,
                                                 size_t ciphertext_length,
                                                 uint8_t *plaintext,
                                                 size_t plaintext_size,
                                                 size_t *plaintext_length)
{
#if defined(PSA_WANT_ALG_CCM) || defined(PSA_WANT_ALG_GCM)
  if (attributes == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  size_t tag_length = PSA_AEAD_TAG_LENGTH(psa_get_key_type(attributes),
                                          psa_get_key_bits(attributes),
                                          alg);

  if (ciphertext_length < tag_length
      || ciphertext == NULL
      || (tag_length > 16)) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // Split the tag in its own buffer to avoid potential issues when the
  // plaintext buffer extends into the tag area
  uint8_t check_tag[16];
  memcpy(check_tag, &ciphertext[ciphertext_length - tag_length], tag_length);

  return sli_crypto_transparent_aead_decrypt_tag(
    attributes, key_buffer, key_buffer_size, alg,
    nonce, nonce_length,
    additional_data, additional_data_length,
    ciphertext, ciphertext_length - tag_length,
    check_tag, tag_length,
    plaintext, plaintext_size, plaintext_length);
#else // PSA_WANT_ALG_CCM || PSA_WANT_ALG_GCM

  (void)attributes;
  (void)key_buffer;
  (void)key_buffer_size;
  (void)alg;
  (void)nonce;
  (void)nonce_length;
  (void)additional_data;
  (void)additional_data_length;
  (void)plaintext;
  (void)plaintext_size;
  (void)plaintext_length;
  (void)ciphertext;
  (void)ciphertext_length;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_CCM || PSA_WANT_ALG_GCM
}

psa_status_t sli_crypto_transparent_aead_encrypt_tag(const psa_key_attributes_t *attributes,
                                                     const uint8_t *key_buffer,
                                                     size_t key_buffer_size,
                                                     psa_algorithm_t alg,
                                                     const uint8_t *nonce,
                                                     size_t nonce_length,
                                                     const uint8_t *additional_data,
                                                     size_t additional_data_length,
                                                     const uint8_t *plaintext,
                                                     size_t plaintext_length,
                                                     uint8_t *ciphertext,
                                                     size_t ciphertext_size,
                                                     size_t *ciphertext_length,
                                                     uint8_t *tag,
                                                     size_t tag_size,
                                                     size_t *tag_length)
{
#if defined(PSA_WANT_ALG_CCM) || defined(PSA_WANT_ALG_GCM)

  if (key_buffer == NULL
      || attributes == NULL
      || nonce == NULL
      || (additional_data == NULL && additional_data_length > 0)
      || (plaintext == NULL && plaintext_length > 0)
      || (plaintext_length > 0 && (ciphertext == NULL || ciphertext_size == 0))
      || ciphertext_length == NULL || tag_length == NULL
      || tag_size == 0 || tag == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  *tag_length = PSA_AEAD_TAG_LENGTH(psa_get_key_type(attributes),
                                    psa_get_key_bits(attributes),
                                    alg);
  size_t key_length = psa_get_key_bits(attributes) / 8;

  // Verify that the driver supports the given parameters
  psa_status_t psa_status = check_aead_parameters(attributes,
                                                  alg,
                                                  nonce_length,
                                                  additional_data_length);
  if (psa_status != PSA_SUCCESS) {
    return psa_status;
  }

  if (key_buffer_size < key_length) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // Check sufficient output buffer size.
  if ((ciphertext_size < plaintext_length)
      || (tag_size < *tag_length)) {
    return PSA_ERROR_BUFFER_TOO_SMALL;
  }

  // Our drivers only support full or no overlap between input and output
  // buffers. So in the case of partial overlap, copy the input buffer into
  // the output buffer and process it in place as if the buffers fully
  // overlapped.
  if ((ciphertext > plaintext) && (ciphertext < (plaintext + plaintext_length))) {
    memmove(ciphertext, plaintext, plaintext_length);
    plaintext = ciphertext;
  }

  sli_crypto_transparent_aead_operation_t operation = { 0 };
  operation.key_len = key_length;
  memcpy(operation.key, key_buffer, key_length);

  switch (PSA_ALG_AEAD_WITH_SHORTENED_TAG(alg, 0)) {
#if defined(PSA_WANT_ALG_CCM)
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 0):
      // Verify key type
      if (psa_get_key_type(attributes) != PSA_KEY_TYPE_AES) {
        return PSA_ERROR_NOT_SUPPORTED;
      }

      psa_status = sli_ccm_crypt_and_tag(&operation,
                                         PSA_CRYPTO_DRIVER_ENCRYPT,
                                         plaintext_length,
                                         nonce,
                                         nonce_length,
                                         additional_data,
                                         additional_data_length,
                                         plaintext,
                                         ciphertext,
                                         tag,
                                         *tag_length);
      break;
#endif // PSA_WANT_ALG_CCM
#if defined(PSA_WANT_ALG_GCM)
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, 0): {
      // Verify key type
      if (psa_get_key_type(attributes) != PSA_KEY_TYPE_AES) {
        return PSA_ERROR_NOT_SUPPORTED;
      }

      sli_gcm_crypt_and_tag(&operation,
                            PSA_CRYPTO_DRIVER_ENCRYPT,
                            plaintext_length,
                            nonce,
                            nonce_length,
                            additional_data,
                            additional_data_length,
                            plaintext,
                            ciphertext,
                            *tag_length,
                            tag);

      sli_psa_zeroize(&operation, sizeof(operation));

      psa_status = PSA_SUCCESS;
      break;
    }
#endif // PSA_WANT_ALG_GCM
    default:
      return PSA_ERROR_NOT_SUPPORTED;
  }

  if (psa_status == PSA_SUCCESS) {
    *ciphertext_length = plaintext_length;
  } else {
    *ciphertext_length = 0;
    *tag_length = 0;
  }

  return psa_status;

#else // PSA_WANT_ALG_CCM || PSA_WANT_ALG_GCM

  (void)attributes;
  (void)key_buffer;
  (void)key_buffer_size;
  (void)alg;
  (void)nonce;
  (void)nonce_length;
  (void)additional_data;
  (void)additional_data_length;
  (void)plaintext;
  (void)plaintext_length;
  (void)ciphertext;
  (void)ciphertext_size;
  (void)ciphertext_length;
  (void)tag;
  (void)tag_size;
  (void)tag_length;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_CCM || PSA_WANT_ALG_GCM
}

psa_status_t sli_crypto_transparent_aead_decrypt_tag(const psa_key_attributes_t *attributes,
                                                     const uint8_t *key_buffer,
                                                     size_t key_buffer_size,
                                                     psa_algorithm_t alg,
                                                     const uint8_t *nonce,
                                                     size_t nonce_length,
                                                     const uint8_t *additional_data,
                                                     size_t additional_data_length,
                                                     const uint8_t *ciphertext,
                                                     size_t ciphertext_length,
                                                     const uint8_t *tag,
                                                     size_t tag_length,
                                                     uint8_t *plaintext,
                                                     size_t plaintext_size,
                                                     size_t *plaintext_length)
{
#if defined(PSA_WANT_ALG_CCM) || defined(PSA_WANT_ALG_GCM)
  if (attributes == NULL
      || key_buffer == NULL
      || nonce == NULL
      || (additional_data == NULL && additional_data_length > 0)
      || (ciphertext == NULL && ciphertext_length > 0)
      || (plaintext == NULL && plaintext_size > 0)
      || plaintext_length == NULL
      || tag == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  size_t key_length = psa_get_key_bits(attributes) / 8;

  // Verify that the driver supports the given parameters
  psa_status_t psa_status = check_aead_parameters(attributes,
                                                  alg,
                                                  nonce_length,
                                                  additional_data_length);
  if (psa_status != PSA_SUCCESS) {
    return psa_status;
  }

  if (key_buffer_size < key_length) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // Check sufficient output buffer size.
  if (plaintext_size < ciphertext_length) {
    return PSA_ERROR_BUFFER_TOO_SMALL;
  }

  // Our drivers only support full or no overlap between input and output
  // buffers. So in the case of partial overlap, copy the input buffer into
  // the output buffer and process it in place as if the buffers fully
  // overlapped.
  if ((plaintext > ciphertext) && (plaintext < (ciphertext + ciphertext_length))) {
    memmove(plaintext, ciphertext, ciphertext_length);
    ciphertext = plaintext;
  }

  sli_crypto_transparent_aead_operation_t operation = { 0 };
  operation.key_len = key_length;
  memcpy(operation.key, key_buffer, key_length);

  uint8_t calc_tag[16] = { 0 };
  uint32_t diff = 0;

  switch (PSA_ALG_AEAD_WITH_SHORTENED_TAG(alg, 0)) {
#if defined(PSA_WANT_ALG_CCM)
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 0):
      // Verify key type
      if (psa_get_key_type(attributes) != PSA_KEY_TYPE_AES) {
        return PSA_ERROR_NOT_SUPPORTED;
      }

      psa_status = sli_ccm_crypt_and_tag(&operation,
                                         PSA_CRYPTO_DRIVER_DECRYPT,
                                         ciphertext_length,
                                         nonce,
                                         nonce_length,
                                         additional_data,
                                         additional_data_length,
                                         ciphertext,
                                         plaintext,
                                         calc_tag,
                                         tag_length);

      if (psa_status != PSA_SUCCESS) {
        return psa_status;
      }

      // Check that the provided tag equals the calculated one
      // (in constant time). Note that the tag returned by ccm_auth_crypt
      // is encrypted, so we don't have to decrypt the tag.
      diff = sli_psa_safer_memcmp(calc_tag, tag, tag_length);
      sli_psa_zeroize(calc_tag, tag_length);

      if (diff != 0) {
        *plaintext_length = 0;
        sli_psa_zeroize(plaintext, ciphertext_length);
        psa_status = PSA_ERROR_INVALID_SIGNATURE;
      } else {
        *plaintext_length = ciphertext_length;
        psa_status = PSA_SUCCESS;
      }

      break;
#endif // PSA_WANT_ALG_CCM
#if defined(PSA_WANT_ALG_GCM)
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, 0): {
      // Verify key type
      if (psa_get_key_type(attributes) != PSA_KEY_TYPE_AES) {
        return PSA_ERROR_NOT_SUPPORTED;
      }

      sli_gcm_crypt_and_tag(&operation,
                            PSA_CRYPTO_DRIVER_DECRYPT,
                            ciphertext_length,
                            nonce,
                            nonce_length,
                            additional_data,
                            additional_data_length,
                            ciphertext,
                            plaintext,
                            tag_length,
                            calc_tag);

      sli_psa_zeroize(&operation, sizeof(operation));

      // Check that the provided tag equals the calculated one
      // (in constant time). Note that the tag computed
      // is encrypted, so we don't have to decrypt check_tag.
      diff = sli_psa_safer_memcmp(calc_tag, tag, tag_length);
      sli_psa_zeroize(calc_tag, tag_length);

      if (diff != 0) {
        *plaintext_length = 0;
        sli_psa_zeroize(plaintext, ciphertext_length);
        psa_status = PSA_ERROR_INVALID_SIGNATURE;
      } else {
        *plaintext_length = ciphertext_length;
        psa_status = PSA_SUCCESS;
      }

      break;
    }
#endif // PSA_WANT_ALG_GCM
    default:
      return PSA_ERROR_NOT_SUPPORTED;
  }

  return psa_status;

#else // PSA_WANT_ALG_CCM || PSA_WANT_ALG_GCM

  (void)attributes;
  (void)key_buffer;
  (void)key_buffer_size;
  (void)alg;
  (void)nonce;
  (void)nonce_length;
  (void)additional_data;
  (void)additional_data_length;
  (void)plaintext;
  (void)plaintext_size;
  (void)plaintext_length;
  (void)ciphertext;
  (void)ciphertext_length;
  (void)tag;
  (void)tag_length;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_CCM || PSA_WANT_ALG_GCM
}

/*******************************************************************************
 **************************    LOCAL FUNCTIONS   *******************************
 ******************************************************************************/

#if defined(PSA_WANT_ALG_CCM)

__STATIC_INLINE
void update_cbc_mac(CRYPTO_TypeDef *crypto, unsigned char *b)
{
  // Every call to the crypto accelerator follows the following conventions:
  //  DATA0: Data to be processed
  //  DATA1: Counter
  //  DATA2: Tag
  //  DATA3: Temporary register
  while ((crypto->STATUS & CRYPTO_STATUS_SEQRUNNING) != 0) ;
  CRYPTO_DataWriteUnaligned(&crypto->DATA0, (uint8_t *)b);
  CRYPTO_EXECUTE_3(crypto,
                   CRYPTO_CMD_INSTR_DATA2TODATA0XOR,
                   CRYPTO_CMD_INSTR_AESENC,
                   CRYPTO_CMD_INSTR_DATA0TODATA2);
}

#endif // PSA_WANT_ALG_CCM
/*
 * The GCM algorithm is based on two main functions, GHASH and GCTR, defined
 * in http://csrc.nist.gov/publications/nistpubs/800-38D/SP-800-38D.pdf
 *
 * The GHASH and GCTR fuctions can be implemented with the following
 * instruction sequences that can be executed by the CRYPTO peripheral.
 */

/*
 * GCTR_SEQUENCE for encryption and decryption.
 *
 * Procedure is executed with following assumptions:
 * - data to be processed is in DATA0
 * - DATA1 contains counter which is preincremented
 * - DATA3 is used as temporary register
 * - result is stored in DATA0
 *
 *   CRYPTO_CMD_INSTR_DATA0TODATA3
 *   CRYPTO_CMD_INSTR_DATA1INC
 *   CRYPTO_CMD_INSTR_DATA1TODATA0
 *   CRYPTO_CMD_INSTR_AESENC
 *   CRYPTO_CMD_INSTR_DATA3TODATA0XOR
 */

/*
 * GHASH_SEQUENCE for encryption, decryption and tag
 *
 * Procedure is executed with following assumptions:
 * - data to be processed is in DATA0 ( A[i] or C[i] )
 * - temporary GHASH is stored in DDATA0
 * - H is stored in DATA2 (part of DDATA3)
 * - DDATA1 is used temporary for holding input to the MMUL instruction
 *   (MMUL DDATA0 = (DDATA1 * V1) mod P V1 != DDATA0,DDATA1)
 *
 *  Note: The DDATA3 in SELDDATA0DDATA3 is for the BBSWAP128.
 *
 *  CRYPTO_CMD_INSTR_SELDDATA0DDATA2
 *  CRYPTO_CMD_INSTR_XOR
 *  CRYPTO_CMD_INSTR_BBSWAP128
 *  CRYPTO_CMD_INSTR_DDATA0TODDATA1
 *  CRYPTO_CMD_INSTR_SELDDATA0DDATA3
 *  CRYPTO_CMD_INSTR_MMUL
 *  CRYPTO_CMD_INSTR_BBSWAP128
 */

/*
 * CRYPTO register usage:
 *  DATA0 - temporary result and plaintext
 *  DATA1 - Counter state
 *  DATA2 - Hash key
 *  DATA3 - used temporary
 *
 *  DDATA0 - temporary results in the GHASH function (Xi-1)
 *  DDATA1 - temporary for MMUL operands
 *  DDATA2 - overlaps DATA0 and DATA1
 *  DDATA3 - overlaps DATA2 and DATA3
 *  DDATA4 - KEYBUF
 */

#if defined(PSA_WANT_ALG_GCM)

__STATIC_INLINE void gcm_restore_crypto_state(sli_crypto_transparent_aead_operation_t *operation,
                                              CRYPTO_TypeDef *crypto)
{
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();

  // Restore state
  if (operation->key_len == 16U) {
    // Set AES-128 mode
    crypto->CTRL = CRYPTO_CTRL_AES_AES128
                   | CRYPTO_CTRL_INCWIDTH_INCWIDTH4 | CRYPTO_CTRL_DMA0RSEL_DATA0;
  } else {
    // Set AES-256 mode
    crypto->CTRL = CRYPTO_CTRL_AES_AES256
                   | CRYPTO_CTRL_INCWIDTH_INCWIDTH4 | CRYPTO_CTRL_DMA0RSEL_DATA0;
  }

  CRYPTO_KeyBufWriteUnaligned(crypto,
                              operation->key,
                              (operation->key_len == 16U) ? cryptoKey128Bits
                              : cryptoKey256Bits);

  crypto->WAC = CRYPTO_WAC_MODULUS_GCMBIN128 | CRYPTO_WAC_RESULTWIDTH_128BIT;
  CRYPTO_DataWrite(&crypto->DATA2, operation->ctx.gcm_ctx.ghash_key);
  CRYPTO_DataWrite(&crypto->DATA1, operation->ctx.gcm_ctx.gctr_state);
  // Write GHASH state (128 bits) temporarily to DATA0 (in DDATA2) then
  // move it to DDATA0 (256 bits) where CRYPTO expects it.
  CRYPTO_DataWrite(&crypto->DATA0, operation->ctx.gcm_ctx.ghash_state);
  crypto->CMD = CRYPTO_CMD_INSTR_DDATA2TODDATA0;

  CORE_EXIT_CRITICAL();
}

// Start GCM streaming operation
static void sli_gcm_starts(sli_crypto_transparent_aead_operation_t *operation,
                           CRYPTO_TypeDef *device,
                           psa_encrypt_or_decrypt_t mode,
                           const unsigned char *iv,
                           size_t iv_len)
{
  bool            store_state_and_release;
  CRYPTO_TypeDef *crypto;
  CORE_DECLARE_IRQ_STATE;

  (void) iv_len; // checked in check_aead_parameters()

  // AD are limited to 2^64 bits, so 2^61 bytes.
  // We need not check if SIZE_MAX (max of size_t) is less than 2^61 (0x2000000000000000)
#if SIZE_MAX > 0x2000000000000000ull
#error not supported
  if (((uint64_t) add_len) >> 61 != 0) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }
#endif

  // Check if this context has already acquired a crypto device, which means
  // the caller should be mbedtls_gcm_crypt_and_tag() which will perform GCM
  // on a full block and call starts, update, finish in a sequence meaning we
  // will not need to store the state in between.
  if (device == NULL) {
    crypto = crypto_management_acquire();
    store_state_and_release = true;
  } else {
    store_state_and_release = false;
    crypto = device;
  }

  // Setup CRTL and KEY
  //   - AES-128 or AES-256 mode
  //   - width of counter in CTR cipher mode to 4 bytes.
  //   - associate DATA0 with DMA instructions which are used in order to
  //     synchronize with MCU load/store during instruction sequence loops
  //     (a DMA engine is not used).
  if (operation->key_len == 16U) {
    // Set AES-128 mode
    crypto->CTRL = CRYPTO_CTRL_AES_AES128
                   | CRYPTO_CTRL_INCWIDTH_INCWIDTH4 | CRYPTO_CTRL_DMA0RSEL_DATA0;
  } else {
    // Set AES-256 mode
    crypto->CTRL = CRYPTO_CTRL_AES_AES256
                   | CRYPTO_CTRL_INCWIDTH_INCWIDTH4 | CRYPTO_CTRL_DMA0RSEL_DATA0;
  }

  CRYPTO_KeyBufWriteUnaligned(crypto,
                              operation->key,
                              (operation->key_len == 16U) ? cryptoKey128Bits
                              : cryptoKey256Bits);

  // Set GCM modulus, operand and result widths
  crypto->WAC   = CRYPTO_WAC_MODULUS_GCMBIN128 | CRYPTO_WAC_RESULTWIDTH_128BIT;
  crypto->SEQCTRL  = 16;
  crypto->SEQCTRLB = 0;

  // Load the initial counter value Y0 into DATA1.
  //   Y0 = IV | 1
  //   That is, the 12 byte Initial Vector concatenated with the initial
  //   counter value 1 in the least significant 32bit word.
  memcpy(operation->ctx.gcm_ctx.gctr_state, iv, 12);
  operation->ctx.gcm_ctx.gctr_state[3] = 0x01000000;
  CRYPTO_DataWrite(&crypto->DATA1, operation->ctx.gcm_ctx.gctr_state);

  CORE_ENTER_CRITICAL();

  // Organize and Compute the initial values.
  CRYPTO_EXECUTE_6(crypto,
                   // Compute hash key H=E(K,0) in DATA2 (LSB DDATA3)
                   CRYPTO_CMD_INSTR_DATA0TODATA0XOR,// DATA0 = 0
                   CRYPTO_CMD_INSTR_AESENC,         // DATA0 = AESENC(DATA0)
                   CRYPTO_CMD_INSTR_SELDDATA2DDATA2,// Select DATA0 for BBSWAP
                   CRYPTO_CMD_INSTR_BBSWAP128,      // Swap bit in H, for GMAC
                   // Move resulting hash key (DDATA0 LSB) to DATA2 (DDATA3 LSB)
                   CRYPTO_CMD_INSTR_DDATA0TODDATA3,
                   // Prepare GHASH(H,A)
                   CRYPTO_CMD_INSTR_CLR             // DDATA0 = 0
                   );

  CORE_EXIT_CRITICAL();

#if defined(SLI_PSA_SUPPORT_GCM_IV_CALCULATION)
  // When len(IV) != 12, then IV = GHASH(zero-extended-IV)
  uint32_t temp[4];
  if (iv_len != 12) {
    // Start GHASH from a zero state
    // DATA2 has the hash key
    // DDATA0 has the initial state (all-zero)
    crypto->SEQCTRL = (iv_len & 0xFFFFFFF0u) + ((iv_len & 0xF) ? 16 : 0) + 16;

    CORE_ENTER_CRITICAL();

    // Run GHASH sequence on IV input
    CRYPTO_EXECUTE_8(crypto,
                     // Load IV input block
                     CRYPTO_CMD_INSTR_DMA0TODATA,       // DATA0 = input block
                     // GHASH_SEQUENCE (see desc above)
                     CRYPTO_CMD_INSTR_SELDDATA0DDATA2,  // V0 = DDATA0, V1 = DDATA2
                     CRYPTO_CMD_INSTR_XOR,              // DDATA0 = DDATA0 ^ DDATA2
                     CRYPTO_CMD_INSTR_BBSWAP128,        // bitswap DDATA0[127:0]
                     CRYPTO_CMD_INSTR_DDATA0TODDATA1,   // DDATA1 = DDATA0
                     CRYPTO_CMD_INSTR_SELDDATA0DDATA3,  // V0 = DDATA0, V1 = DDATA3
                     CRYPTO_CMD_INSTR_MMUL,             // DDATA0 = DDATA1 * DDATA3 mod P
                     CRYPTO_CMD_INSTR_BBSWAP128         // bitswap DDATA0[127:0]
                     );

    // First loop through and write data for all complete blocks
    size_t iv_bytes_left = iv_len;
    while (iv_bytes_left >= 16) {
      iv_bytes_left -= 16;
      // Wait for sequencer to accept data
      while (!(crypto->STATUS & CRYPTO_STATUS_DMAACTIVE)) ;
      CRYPTO_DataWriteUnaligned(&crypto->DATA0, iv);
      iv += 16;
    }
    if (iv_bytes_left > 0) {
      // For last incomplete block, use temporary buffer for zero padding.
      memset(temp, 0, sizeof(temp));
      memcpy(temp, iv, iv_bytes_left);
      while (!(crypto->STATUS & CRYPTO_STATUS_DMAACTIVE)) ;
      CRYPTO_DataWrite(&crypto->DATA0, temp);
    }
    // Finish with adding in len(C)
    uint64_t bitlen = iv_len * 8;
    while (!(crypto->STATUS & CRYPTO_STATUS_DMAACTIVE)) ;
    crypto->DATA0 = 0;
    crypto->DATA0 = 0;
    crypto->DATA0 = __REV((uint32_t) (bitlen >> 32));
    crypto->DATA0 = __REV((uint32_t) bitlen);

    // Wait for completion
    while (!CRYPTO_InstructionSequenceDone(crypto)) ;

    CORE_EXIT_CRITICAL();

    // Copy out calculated IV to context and DATA1
    crypto->CMD = CRYPTO_CMD_INSTR_DDATA0TODDATA2;
    crypto->CMD = CRYPTO_CMD_INSTR_DATA0TODATA1;
    CRYPTO_DataRead(&crypto->DATA0, operation->ctx.gcm_ctx.gctr_state);

    // Reset GHASH state
    crypto->CMD = CRYPTO_CMD_INSTR_CLR;
  }
#endif // SLI_PSA_SUPPORT_GCM_IV_CALCULATION

  // Remember IV since we need it for the finish operation
  CRYPTO_DataRead(&crypto->DATA1, operation->ctx.gcm_ctx.iv);

  // Remember mode and additional authentication length
  operation->direction = mode;
  // Reset data length to zero.
  operation->processed_len = 0;

  if (store_state_and_release) {
    CORE_ENTER_CRITICAL();
    // Move GHASH state in DDATA0 temporarily to DATA0 (in DDATA2) in order to
    // read only the 128 bits value (since DDATA0 is 256 bits wide).
    crypto->CMD = CRYPTO_CMD_INSTR_DDATA0TODDATA2;
    CRYPTO_DataRead(&crypto->DATA0, operation->ctx.gcm_ctx.ghash_state);
    CRYPTO_DataRead(&crypto->DATA2, operation->ctx.gcm_ctx.ghash_key);
    CORE_EXIT_CRITICAL();

    crypto_management_release(crypto);
  }
  return;
}

// Add authentication data.
static void crypto_gcm_update_partial_ad(sli_crypto_transparent_aead_operation_t *operation,
                                         CRYPTO_TypeDef *device,
                                         const unsigned char *add,
                                         size_t add_len)
{
  bool            store_state_and_release;
  CRYPTO_TypeDef *crypto;
  uint32_t        temp[4];
  unsigned int    complete_blocks_in_bytes;
  bool            last_block_is_incomplete;
  CORE_DECLARE_IRQ_STATE;

  // Check if this context has already acquired a crypto device, which means
  // the caller should be mbedtls_gcm_crypt_and_tag() which will perform GCM
  // on a full block and call starts, update, finish in a sequence meaning we
  // will not need to store the state in between.
  if (device == NULL) {
    crypto = crypto_management_acquire();
    store_state_and_release = true;
    gcm_restore_crypto_state(operation, crypto);
  } else {
    store_state_and_release = false;
    crypto = device;
  }

  // Process additional authentication data if present.
  crypto->SEQCTRLB = 0; // Sequence B is not used for auth data

  operation->add_len += add_len;
  while (add_len) {
    if (add_len > (_CRYPTO_SEQCTRL_LENGTHA_MASK & 0xFFFFFFF0u)) {
      complete_blocks_in_bytes = _CRYPTO_SEQCTRL_LENGTHA_MASK & 0xFFFFFFF0u;
      last_block_is_incomplete = false;
      add_len -= complete_blocks_in_bytes;
    } else {
      // Calculate total sequence length 16*num_complete_blocks, plus 16 if
      // there is an incomplete block at the end.
      last_block_is_incomplete = add_len & 0xF;
      complete_blocks_in_bytes = add_len & 0xFFFFFFF0u;
      add_len = add_len & 0xF;
    }

    // Set SEQCTRL_LENGTHA to loop through all blocks
    crypto->SEQCTRL = complete_blocks_in_bytes + (last_block_is_incomplete ? 16 : 0);

    CORE_ENTER_CRITICAL();

    // Run GHASH sequence on additional authentication data
    CRYPTO_EXECUTE_8(crypto,
                     // Load additional auth data
                     CRYPTO_CMD_INSTR_DMA0TODATA,
                     // GHASH_SEQUENCE (see desc above)
                     CRYPTO_CMD_INSTR_SELDDATA0DDATA2,
                     CRYPTO_CMD_INSTR_XOR,
                     CRYPTO_CMD_INSTR_BBSWAP128,
                     CRYPTO_CMD_INSTR_DDATA0TODDATA1,
                     CRYPTO_CMD_INSTR_SELDDATA0DDATA3,
                     CRYPTO_CMD_INSTR_MMUL,
                     CRYPTO_CMD_INSTR_BBSWAP128
                     );

    // Loop through all complete blocks and write additional auth data to CRYPTO.
    while (complete_blocks_in_bytes >= 16) {
      complete_blocks_in_bytes  -= 16;
      // Wait for sequencer to accept data
      while (!(crypto->STATUS & CRYPTO_STATUS_DMAACTIVE)) ;
      CRYPTO_DataWriteUnaligned(&crypto->DATA0, add);
      add      += 16;
    }
    if (last_block_is_incomplete) {
      // For last incomplete block, use temporary buffer for zero padding.
      memset(temp, 0, sizeof(temp));
      memcpy(temp, add, add_len);
      while (!(crypto->STATUS & CRYPTO_STATUS_DMAACTIVE)) ;
      CRYPTO_DataWrite(&crypto->DATA0, temp);

      // Done,, set add_len to zero to exit loop.
      add_len = 0;
    }
    // Wait for completion
    while (!CRYPTO_InstructionSequenceDone(crypto)) ;

    CORE_EXIT_CRITICAL();
  }
  if (store_state_and_release) {
    CORE_ENTER_CRITICAL();
    // Move GHASH state in DDATA0 temporarily to DATA0 (in DDATA2) in order to
    // read only the 128 bits value (since DDATA0 is 256 bits wide).
    crypto->CMD = CRYPTO_CMD_INSTR_DDATA0TODDATA2;
    CRYPTO_DataRead(&crypto->DATA0, operation->ctx.gcm_ctx.ghash_state);
    CRYPTO_DataRead(&crypto->DATA2, operation->ctx.gcm_ctx.ghash_key);
    CORE_EXIT_CRITICAL();

    crypto_management_release(crypto);
  }
  return;
}

// Update a GCM streaming operation with more input data to be
// encrypted or decrypted.
static void sli_gcm_update(sli_crypto_transparent_aead_operation_t *operation,
                           CRYPTO_TypeDef *device,
                           size_t length,
                           const unsigned char *input,
                           unsigned char *output)
{
  bool            restore_state_and_release;
  CRYPTO_TypeDef *crypto;
  uint32_t        temp[4];
  unsigned int    complete_blocks_in_bytes;
  bool            last_block_is_incomplete;
  CORE_DECLARE_IRQ_STATE;

  if (length == 0) {
    return;
  }

  // Total length is restricted to 2^39 - 256 bits, ie 2^36 - 2^5 bytes
  // Also check for possible overflow.
  // We need not check if SIZE_MAX (max of size_t) is less than 2^36-2^5 (0xFFFFFFFE0)
#if SIZE_MAX > 0xFFFFFFFE0ull
#error not supported
  if (operation->processed_len + length < operation->processed_len
      || (uint64_t) operation->processed_len + length > 0xFFFFFFFE0ull) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }
#endif

  // Check if this context has already acquired a crypto device, which means
  // the caller should be mbedtls_gcm_crypt_and_tag() which will perform GCM
  // on a full block and call starts, update, finish in a sequence meaning we
  // will not need to store the state in between.
  if (device == NULL) {
    crypto = crypto_management_acquire();
    restore_state_and_release = true;
    gcm_restore_crypto_state(operation, crypto);
  } else {
    restore_state_and_release = false;
    crypto = device;
  }

  operation->processed_len += length;

  while (length) {
    if (length > (_CRYPTO_SEQCTRL_LENGTHA_MASK & 0xFFFFFFF0u)) {
      last_block_is_incomplete = false;
      complete_blocks_in_bytes = _CRYPTO_SEQCTRL_LENGTHA_MASK & 0xFFFFFFF0u;
      length -= complete_blocks_in_bytes;
    } else {
      // Calculate total sequence length 16*num_complete_blocks, plus 16 if
      // there is an incomplete block at the end.
      last_block_is_incomplete = length & 0xF;
      complete_blocks_in_bytes = length & 0xFFFFFFF0u;
      length = length & 0xF; // length of last incomplete block
    }

    CORE_ENTER_CRITICAL();

    if (operation->direction == PSA_CRYPTO_DRIVER_DECRYPT) {
      crypto->SEQCTRL  = complete_blocks_in_bytes + (last_block_is_incomplete ? 16 : 0);
      crypto->SEQCTRLB = 0;

      // Start decryption sequence
      CRYPTO_EXECUTE_14(crypto,
                        CRYPTO_CMD_INSTR_DMA0TODATA,  // Load Ciphertext

                        // GHASH_SEQUENCE (see desc above)
                        CRYPTO_CMD_INSTR_SELDDATA0DDATA2,
                        CRYPTO_CMD_INSTR_XOR,
                        CRYPTO_CMD_INSTR_BBSWAP128,
                        CRYPTO_CMD_INSTR_DDATA0TODDATA1,
                        CRYPTO_CMD_INSTR_SELDDATA0DDATA3,
                        CRYPTO_CMD_INSTR_MMUL,
                        CRYPTO_CMD_INSTR_BBSWAP128,

                        // GCTR_SEQUENCE (see desc above)
                        CRYPTO_CMD_INSTR_DATA0TODATA3,
                        CRYPTO_CMD_INSTR_DATA1INC,
                        CRYPTO_CMD_INSTR_DATA1TODATA0,
                        CRYPTO_CMD_INSTR_AESENC,
                        CRYPTO_CMD_INSTR_DATA3TODATA0XOR,
                        CRYPTO_CMD_INSTR_DATATODMA0   // Store Plaintext
                        );
    } else {
      // For encryption we need to handle the last block differently if it is
      // incomplete. We need to zeroize bits outside len(PT) in DATA0 before
      // the GHASH operation. We do this by using a DMA0TODATA instruction in
      // the B sequence, meaning that the sequencer will wait for the MCU core
      // to zeroize bits and write them back to DATA0.
      crypto->SEQCTRL  = complete_blocks_in_bytes;
      crypto->SEQCTRLB = last_block_is_incomplete ? 16 : 0;

      // Start encryption sequence
      CRYPTO_EXECUTE_17(crypto,
                        CRYPTO_CMD_INSTR_DMA0TODATA,  // Load Plaintext

                        // GCTR_SEQUENCE (see desc above)
                        CRYPTO_CMD_INSTR_DATA0TODATA3,
                        CRYPTO_CMD_INSTR_DATA1INC,
                        CRYPTO_CMD_INSTR_DATA1TODATA0,
                        CRYPTO_CMD_INSTR_AESENC,
                        CRYPTO_CMD_INSTR_DATA3TODATA0XOR,
                        CRYPTO_CMD_INSTR_DATATODMA0,  // Store Ciphertext

                        CRYPTO_CMD_INSTR_EXECIFB,
                        CRYPTO_CMD_INSTR_DMA0TODATA,  // Load X XOR MSB(CIPH(CB))
                        CRYPTO_CMD_INSTR_EXECALWAYS,

                        // GHASH_SEQUENCE (see desc above)
                        CRYPTO_CMD_INSTR_SELDDATA0DDATA2,
                        CRYPTO_CMD_INSTR_XOR,
                        CRYPTO_CMD_INSTR_BBSWAP128,
                        CRYPTO_CMD_INSTR_DDATA0TODDATA1,
                        CRYPTO_CMD_INSTR_SELDDATA0DDATA3,
                        CRYPTO_CMD_INSTR_MMUL,
                        CRYPTO_CMD_INSTR_BBSWAP128
                        );
    }

    // Loop through all complete blocks, write input data and read output data.
    while (complete_blocks_in_bytes >= 16) {
      complete_blocks_in_bytes -= 16;
      // Wait for sequencer to accept data
      while (!(crypto->STATUS & CRYPTO_STATUS_DMAACTIVE)) ;
      CRYPTO_DataWriteUnaligned(&crypto->DATA0, input);
      input   += 16;
      // Wait for sequencer to finish iteration and make data available
      while (!(crypto->STATUS & CRYPTO_STATUS_DMAACTIVE)) ;
      CRYPTO_DataReadUnaligned(&crypto->DATA0, output);
      output  += 16;
    }

    if (last_block_is_incomplete) {
      // The last block is incomplete, so we need to zero pad bits outside len(PT)
      // Use temporary buffer for zero padding
      memset(temp, 0, sizeof(temp));
      memcpy(temp, input, length);

      while (!(crypto->STATUS & CRYPTO_STATUS_DMAACTIVE)) ;
      // Write last input data (PT/CT)
      CRYPTO_DataWrite(&crypto->DATA0, temp);
      while (!(crypto->STATUS & CRYPTO_STATUS_DMAACTIVE)) ;
      // Read last output data (CT/PT)
      CRYPTO_DataRead(&crypto->DATA0, temp);

      if (operation->direction == PSA_CRYPTO_DRIVER_ENCRYPT) {
        // For encryption, when the last block is incomplete we need to
        // zeroize bits outside len(PT) in DATA0 before the GHASH operation.
        memset(&((uint8_t*)temp)[length], 0, 16 - length);
        CRYPTO_DataWrite(&crypto->DATA0, temp);
      }

      // Copy to output buffer now while CRYPTO performs GHASH.
      memcpy(output, temp, length);

      // Done,, set length to zero to exit loop.
      length = 0;
    }

    // Wait for completion
    while (!CRYPTO_InstructionSequenceDone(crypto)) ;

    CORE_EXIT_CRITICAL();
  }

  if (restore_state_and_release) {
    CORE_ENTER_CRITICAL();
    CRYPTO_DataRead(&crypto->DATA1, operation->ctx.gcm_ctx.gctr_state);
    // Move GHASH state in DDATA0 temporarily to DATA2 (in DDATA3) in order to
    // read only the 128 bits value (since DDATA0 is 256 bits wide).
    crypto->CMD = CRYPTO_CMD_INSTR_DDATA0TODDATA3;
    CRYPTO_DataRead(&crypto->DATA2, operation->ctx.gcm_ctx.ghash_state);
    CORE_EXIT_CRITICAL();

    crypto_management_release(crypto);
  }

  return;
}

// Finish GCM streaming operation
static void sli_gcm_finish(sli_crypto_transparent_aead_operation_t *operation,
                           CRYPTO_TypeDef *device,
                           unsigned char *tag,
                           size_t tag_len)
{
  uint64_t        bit_len;
  uint32_t        temp[4];
  CRYPTO_TypeDef *crypto;
  CORE_DECLARE_IRQ_STATE;

  bool            restore_state_and_release;
  // Check if this context has already acquired a crypto device, which means
  // the caller should be mbedtls_gcm_crypt_and_tag() which will perform GCM
  // on a full block and call starts, update, finish in a sequence meaning we
  // will not need to store the state in between.
  if (device == NULL) {
    crypto = crypto_management_acquire();
    restore_state_and_release = true;
    gcm_restore_crypto_state(operation, crypto);
  } else {
    restore_state_and_release = false;
    crypto = device;
  }

  // Compute last part of the GHASH and authentication tag:
  //
  // Xm+n+1 = (Xm+n XOR (len(A)|len(C))) * H
  //
  // Compute the final authentication tag now.
  // T = MSBt (GHASH(H, A, C) XOR E(K, Y0))
  // where GHASH(H, A, C) is Xm+n+1 which is stored in DATA0 (LSWord of DDATA2)
  // and Counter value Yn is stored in DATA1. We assume N (in Yn) is not bigger
  // than 2^32 so E(K, Y0) derived by clearing DATA1 which gives Y0.
  crypto->SEQCTRL = 16;
  crypto->SEQCTRLB = 0;

  CORE_ENTER_CRITICAL();

  // Temporarily load length fields value 128 bits into DATA0 register
  // Then move it into LSB of DDATA1 (for optimal performance)
  bit_len = (uint64_t)operation->add_len * 8;
  crypto->DATA0 = __REV((uint32_t) (bit_len >> 32));
  crypto->DATA0 = __REV((uint32_t) bit_len);
  bit_len = (uint64_t)operation->processed_len * 8;
  crypto->DATA0 = __REV((uint32_t) (bit_len >> 32));
  crypto->DATA0 = __REV((uint32_t) bit_len);

  // Calculate last part of GHASH (in DDATA0)
  // adding in length fields in DATA0 ( LSB of DDATA2 )
  CRYPTO_EXECUTE_7(crypto,
                   // See descripton of GHASH_SEQUENCE above.
                   CRYPTO_CMD_INSTR_SELDDATA0DDATA2, // A[i] and Xi-1
                   CRYPTO_CMD_INSTR_XOR,
                   CRYPTO_CMD_INSTR_BBSWAP128,
                   CRYPTO_CMD_INSTR_DDATA0TODDATA1,
                   CRYPTO_CMD_INSTR_SELDDATA0DDATA3, // temp result and H
                   CRYPTO_CMD_INSTR_MMUL,       // Xi is stored in DDATA0
                   CRYPTO_CMD_INSTR_BBSWAP128);

  while (!CRYPTO_InstructionSequenceDone(crypto)) ;

  // With the GHASH result in DDATA0, we can reinstate the initial IV
  // and calculate tag = GHASH ^ AESENC(IV)
  CRYPTO_DataWrite(&crypto->DATA0, operation->ctx.gcm_ctx.iv);
  CRYPTO_EXECUTE_3(crypto,
                   CRYPTO_CMD_INSTR_DDATA0TODDATA3,   // GHASH result to DATA2
                   // Calculate AuthTag
                   CRYPTO_CMD_INSTR_AESENC,
                   CRYPTO_CMD_INSTR_DATA2TODATA0XOR); // DATA0 = DATA0 ^ DATA2

  // Wait for completion
  while (!CRYPTO_InstructionSequenceDone(crypto)) ;

  if (tag_len) {
    CRYPTO_DataRead(&crypto->DATA0, temp);
    memcpy(tag, temp, tag_len);
  }

  CORE_EXIT_CRITICAL();

  if (restore_state_and_release) {
    crypto_management_release(crypto);
  }
  return;
}

// Perform full GCM block encryption or decryption operation with tag.
static void sli_gcm_crypt_and_tag(sli_crypto_transparent_aead_operation_t *operation,
                                  psa_encrypt_or_decrypt_t mode,
                                  size_t length,
                                  const unsigned char *iv,
                                  size_t iv_len,
                                  const unsigned char *add,
                                  size_t add_len,
                                  const unsigned char *input,
                                  unsigned char *output,
                                  size_t tag_len,
                                  unsigned char *tag)
{
  CRYPTO_TypeDef *device = crypto_management_acquire();

  sli_gcm_starts(operation, device, mode, iv, iv_len);
  crypto_gcm_update_partial_ad(operation, device, add, add_len);
  sli_gcm_update(operation, device, length, input, output);
  sli_gcm_finish(operation, device, tag, tag_len);

  crypto_management_release(device);

  return;
}

#endif // PSA_WANT_ALG_GCM

#if defined(PSA_WANT_ALG_CCM)

__STATIC_INLINE void ccm_restore_crypto_state(sli_crypto_transparent_aead_operation_t *operation,
                                              CRYPTO_TypeDef *crypto)
{
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();

  CRYPTO_KeyBufWriteUnaligned(crypto,
                              operation->key,
                              (operation->key_len == 16U) ? cryptoKey128Bits
                              : cryptoKey256Bits);

  CRYPTO_DataWriteUnaligned(&crypto->DATA1, (uint8_t *)operation->ctx.ccm_ctx.cctr_state);

  CORE_EXIT_CRITICAL();
}

static void crypto_ccm_update_partial_ad(sli_crypto_transparent_aead_operation_t *operation,
                                         CRYPTO_TypeDef *device,
                                         const unsigned char *add,
                                         size_t add_len)
{
  size_t len_left;
  unsigned char b[16];
  const unsigned char *src;

  CRYPTO_TypeDef *crypto;
  CORE_DECLARE_IRQ_STATE;

  // Check if this context has already acquired a crypto device, which means
  // the caller should be sli_ccm_crypt_and_tag() which will perform CCM
  // on a full block and call starts, update, finish in a sequence meaning we
  // will not need to store the state in between.
  if (device == NULL) {
    crypto = crypto_management_acquire();
    ccm_restore_crypto_state(operation, crypto);
  } else {
    crypto = device;
  }

  // If there is additional data, update CBC-MAC with
  // add_len, add, 0 (padding to a block boundary).
  if (add_len > 0) {
    size_t use_len;
    len_left = add_len;
    src = add;
    CORE_ENTER_CRITICAL();
    if (operation->first_update_ad_op) {
      memset(b, 0, sizeof(b));
      b[0] = (unsigned char)((operation->add_len >> 8) & 0xFF);
      b[1] = (unsigned char)((operation->add_len) & 0xFF);

      use_len = len_left < 16 - 2 ? len_left : 16 - 2;
      memcpy(b + 2, src, use_len);
      len_left -= use_len;
      src += use_len;

      update_cbc_mac(crypto, b);
      operation->first_update_ad_op = false;
    }

    while (len_left > 0) {
      use_len = len_left > 16 ? 16 : len_left;

      memset(b, 0, sizeof(b));
      memcpy(b, src, use_len);
      update_cbc_mac(crypto, b);

      len_left -= use_len;
      src += use_len;
    }

    CORE_EXIT_CRITICAL();
  }
}

// Start CCM streaming operation.
static psa_status_t sli_ccm_starts(sli_crypto_transparent_aead_operation_t *operation,
                                   CRYPTO_TypeDef *device,
                                   psa_encrypt_or_decrypt_t mode,
                                   const unsigned char *iv,
                                   size_t iv_len,
                                   size_t tag_len,
                                   size_t message_len)
{
  unsigned char i;
  unsigned char q;
  size_t len_left;
  unsigned char b[16];
  unsigned char ctr[16];

  CRYPTO_TypeDef *crypto;
  CORE_DECLARE_IRQ_STATE;

  // Check remaining length requirements: SP800-38C A.1
  // Additional requirement: a < 2^16 - 2^8 to simplify the code.
  // 'length' checked later (when writing it to the first block)

  // Also, loosen the requirements to enable support for CCM* (IEEE 802.15.4).

  bool            store_state_and_release;
  // Check if this context has already acquired a crypto device, which means
  // the caller should be sli_ccm_crypt_and_tag() which will perform CCM
  // on a full block and call starts, update, finish in a sequence meaning we
  // will not need to store the state in between.
  if (device == NULL) {
    crypto = crypto_management_acquire();
    store_state_and_release = true;
  } else {
    store_state_and_release = false;
    operation->first_update_ad_op = true;
    crypto = device;
  }

  // Remember mode and initialize additional authentication length.
  operation->add_len = operation->ctx.preinit.add_len;
  operation->direction = mode;
  // Reset data length to zero.
  operation->processed_len = 0;
  // Save IV and IV length for finish step.
  operation->ctx.ccm_ctx.iv_len = iv_len;
  memcpy(operation->ctx.ccm_ctx.iv, iv, iv_len);

  q = 16 - 1 - (unsigned char) iv_len;

  // First block B_0:
  // 0        .. 0        flags
  // 1        .. iv_len   nonce (aka iv)
  // iv_len+1 .. 15       length
  //
  // With flags as (bits):
  // 7        0
  // 6        add present?
  // 5 .. 3   (t - 2) / 2
  // 2 .. 0   q - 1

  b[0] = 0;
  b[0] |= (operation->add_len > 0) << 6;
  b[0] |= ((tag_len - 2) / 2) << 3;
  b[0] |= q - 1;

  memcpy(b + 1, iv, iv_len);

  for (i = 0, len_left = message_len; i < q; i++, len_left >>= 8) {
    b[15 - i] = (unsigned char)(len_left & 0xFF);
  }

  if (len_left > 0) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  crypto->WAC = 0;
  crypto->CTRL = 0;

  // Set the encryption key.
  CORE_ENTER_CRITICAL();
  CRYPTO_KeyBufWriteUnaligned(crypto,
                              operation->key,
                              (operation->key_len == 16U) ? cryptoKey128Bits
                              : cryptoKey256Bits);

  // Clear tag register (DATA2)
  for (uint32_t i = 0; i < 4; ++i) {
    crypto->DATA2 = 0;
  }

  // Start CBC-MAC with first block.
  update_cbc_mac(crypto, b);
  CORE_EXIT_CRITICAL();

  // Prepare counter block for encryption:
  // 0        .. 0        flags
  // 1        .. iv_len   nonce (aka iv)
  // iv_len+1 .. 15       counter (initially 1)

  // With flags as (bits):
  // 7 .. 3   0
  // 2 .. 0   q - 1

  ctr[0] = q - 1;
  memcpy(ctr + 1, iv, iv_len);
  memset(ctr + 1 + iv_len, 0, q);
  ctr[15] = 1;

  // Write the counter to CRYPTO.
  CORE_ENTER_CRITICAL();
  CRYPTO_DataWriteUnaligned(&crypto->DATA1, (uint8_t *)ctr);
  CORE_EXIT_CRITICAL();

  if (store_state_and_release) {
    CORE_ENTER_CRITICAL();
    CRYPTO_DataReadUnaligned(&crypto->DATA1, (uint8_t *)operation->ctx.ccm_ctx.cctr_state);
    CORE_EXIT_CRITICAL();
    crypto_management_release(crypto);
  }
  return PSA_SUCCESS;
}

// Update a CCM streaming operation with more input data to be
// encrypted or decrypted.
static psa_status_t sli_ccm_update(sli_crypto_transparent_aead_operation_t *operation,
                                   CRYPTO_TypeDef *device,
                                   size_t length,
                                   const unsigned char *input,
                                   unsigned char *output)
{
  size_t len_left;
  unsigned char b[16];
  const unsigned char *src;
  unsigned char *dst;

  CRYPTO_TypeDef *crypto;
  CORE_DECLARE_IRQ_STATE;

  if (length == 0) {
    return PSA_SUCCESS;
  }

  bool restore_state_and_release;
  // Check if this context has already acquired a crypto device, which means
  // the caller should be sli_ccm_crypt_and_tag() which will perform CCM
  // on a full block and call starts, update, finish in a sequence meaning we
  // will not need to store the state in between.
  if (device == NULL) {
    crypto = crypto_management_acquire();
    restore_state_and_release = true;
    ccm_restore_crypto_state(operation, crypto);
  } else {
    restore_state_and_release = false;
    crypto = device;
  }

  operation->processed_len += length;

  // Authenticate and {en,de}crypt the message.

  // The only difference between encryption and decryption is
  // the respective order of authentication and {en,de}cryption.
  len_left = length;
  src = input;
  dst = output;

  uint8_t out_buf[16];

  CORE_ENTER_CRITICAL();

  while (len_left > 0) {
    size_t use_len = len_left > 16 ? 16 : len_left;

    if (operation->direction == PSA_CRYPTO_DRIVER_ENCRYPT) {
      memset(b, 0, sizeof(b));
      memcpy(b, src, use_len);
      update_cbc_mac(crypto, b);
    }

    // Encrypt a message block with CTR.
    CRYPTO_DataWriteUnaligned(&crypto->DATA3, (uint8_t *)src);
    CRYPTO_EXECUTE_4(crypto,
                     // Generate key stream block.
                     CRYPTO_CMD_INSTR_DATA1TODATA0,
                     CRYPTO_CMD_INSTR_AESENC,
                     // Apply the key stream block to input.
                     CRYPTO_CMD_INSTR_DATA3TODATA0XOR,
                     // Increment counter.
                     CRYPTO_CMD_INSTR_DATA1INC);
    while ((crypto->STATUS & CRYPTO_STATUS_SEQRUNNING) != 0) ;
    CRYPTO_DataReadUnaligned(&crypto->DATA0, (uint8_t *)out_buf);

    memcpy(dst, out_buf, use_len);

    if (operation->direction == PSA_CRYPTO_DRIVER_DECRYPT) {
      memset(b, 0, sizeof(b));
      memcpy(b, dst, use_len);
      update_cbc_mac(crypto, b);
    }

    dst += use_len;
    src += use_len;
    len_left -= use_len;
  }

  CORE_EXIT_CRITICAL();
  // Save state and release.
  if (restore_state_and_release) {
    CORE_ENTER_CRITICAL();
    CRYPTO_DataReadUnaligned(&crypto->DATA1, (uint8_t *)operation->ctx.ccm_ctx.cctr_state);
    CORE_EXIT_CRITICAL();
    crypto_management_release(crypto);
  }
  return PSA_SUCCESS;
}

// Finish CCM streaming operation
static psa_status_t sli_ccm_finish(sli_crypto_transparent_aead_operation_t *operation,
                                   CRYPTO_TypeDef *device,
                                   unsigned char *tag,
                                   size_t tag_len)
{
  unsigned char i;
  unsigned char q;
  unsigned char ctr[16];
  uint8_t out_buf[16];
  CRYPTO_TypeDef *crypto;

  CORE_DECLARE_IRQ_STATE;

  bool restore_state_and_release;
  // Check if this context has already acquired a crypto device, which means
  // the caller should be sli_ccm_crypt_and_tag() which will perform CCM
  // on a full block and call starts, update, finish in a sequence meaning we
  // will not need to store the state in between.
  if (device == NULL) {
    crypto = crypto_management_acquire();
    restore_state_and_release = true;
    ccm_restore_crypto_state(operation, crypto);
  } else {
    restore_state_and_release = false;
    crypto = device;
  }

  q = 16 - 1 - (unsigned char) operation->ctx.ccm_ctx.iv_len;

  ctr[0] = q - 1;
  memcpy(ctr + 1, operation->ctx.ccm_ctx.iv, operation->ctx.ccm_ctx.iv_len);
  memset(ctr + 1 + operation->ctx.ccm_ctx.iv_len, 0, q);
  ctr[15] = 1;

  // Authentication: reset counter and crypt/mask internal tag.
  for (i = 0; i < q; i++) {
    ctr[15 - i] = 0;
  }

  CORE_ENTER_CRITICAL();

  // Encrypt the tag with CTR.
  CRYPTO_DataWriteUnaligned(&crypto->DATA0, (uint8_t *)ctr);

  CRYPTO_EXECUTE_2(crypto,
                   // Generate key stream block.
                   CRYPTO_CMD_INSTR_AESENC,
                   // Apply the key stream block to the tag.
                   CRYPTO_CMD_INSTR_DATA2TODATA0XOR);

  while ((crypto->STATUS & CRYPTO_STATUS_SEQRUNNING) != 0) ;

  CRYPTO_DataReadUnaligned(&crypto->DATA0, (uint8_t *)out_buf);
  CORE_EXIT_CRITICAL();

  memcpy(tag, out_buf, tag_len);

  if (restore_state_and_release) {
    crypto_management_release(crypto);
  }

  return PSA_SUCCESS;
}
/*
 * Definition of CCM:
 * http://csrc.nist.gov/publications/nistpubs/800-38C/SP800-38C_updated-July20_2007.pdf
 * RFC 3610 "Counter with CBC-MAC (CCM)"
 *
 * Related:
 * RFC 5116 "An Interface and Algorithms for Authenticated Encryption"
 */

// Perform full CCM block encryption or decryption operation with tag
static psa_status_t sli_ccm_crypt_and_tag(sli_crypto_transparent_aead_operation_t *operation,
                                          psa_encrypt_or_decrypt_t mode,
                                          size_t length,
                                          const unsigned char *iv,
                                          size_t iv_len,
                                          const unsigned char *add,
                                          size_t add_len,
                                          const unsigned char *input,
                                          unsigned char *output,
                                          unsigned char *tag,
                                          size_t tag_len)
{
  psa_status_t status;
  CRYPTO_TypeDef *device = crypto_management_acquire();

  operation->ctx.preinit.add_len = add_len;

  status = sli_ccm_starts(operation, device, mode, iv, iv_len, tag_len, length);
  if (status != PSA_SUCCESS) {
    return status;
  }

  crypto_ccm_update_partial_ad(operation, device, add, add_len);

  status = sli_ccm_update(operation, device, length, input, output);

  if (status != PSA_SUCCESS) {
    return status;
  }
  status = sli_ccm_finish(operation, device, tag, tag_len);

  if (status != PSA_SUCCESS) {
    return status;
  }

  crypto_management_release(device);

  return PSA_SUCCESS;
}

#endif // PSA_WANT_ALG_CCM

#if defined(PSA_WANT_ALG_GCM) || defined(PSA_WANT_ALG_CCM)
static psa_status_t transparent_aead_encrypt_decrypt_setup(sli_crypto_transparent_aead_operation_t *operation,
                                                           const psa_key_attributes_t *attributes,
                                                           const uint8_t *key_buffer,
                                                           size_t key_buffer_size,
                                                           psa_algorithm_t alg,
                                                           psa_encrypt_or_decrypt_t operation_direction)
{
  if (operation == NULL
      || attributes == NULL
      || key_buffer == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  memset(operation, 0, sizeof(*operation));

  size_t key_bits = psa_get_key_bits(attributes);
  size_t key_size = PSA_BITS_TO_BYTES(key_bits);

  if (key_buffer_size < key_size) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  if (sizeof(operation->key) < key_size) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  switch (psa_get_key_bits(attributes)) {
    case 128:
      break;
    case 192:
      return PSA_ERROR_NOT_SUPPORTED;
    case 256:
      break;
    default:
      return PSA_ERROR_INVALID_ARGUMENT;
  }

  // Validate key type.
  if (psa_get_key_type(attributes) != PSA_KEY_TYPE_AES) {
    return PSA_ERROR_NOT_SUPPORTED;
  }

  // Validate tag length.
  if ( PSA_AEAD_TAG_LENGTH(psa_get_key_type(attributes), key_bits, alg) > 16 ) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // Validate operation.
  switch (PSA_ALG_AEAD_WITH_SHORTENED_TAG(alg, 0)) {
#if defined (PSA_WANT_ALG_GCM)
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, 0):
      operation->alg = alg;
      break;
#endif
#if defined (PSA_WANT_ALG_CCM)
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 0):
      operation->alg = alg;
      operation->first_update_ad_op = true;
      break;
#endif
    default:
      return PSA_ERROR_NOT_SUPPORTED;
  }

  operation->key_len = key_size;
  memcpy(operation->key, key_buffer, key_size);

  operation->direction = operation_direction;

  return PSA_SUCCESS;
}
#endif //PSA_WANT_ALG_GCM || PSA_WANT_ALG_CCM

psa_status_t sli_crypto_transparent_aead_encrypt_setup(sli_crypto_transparent_aead_operation_t *operation,
                                                       const psa_key_attributes_t *attributes,
                                                       const uint8_t *key_buffer,
                                                       size_t key_buffer_size,
                                                       psa_algorithm_t alg)
{
#if defined(PSA_WANT_ALG_GCM) || defined(PSA_WANT_ALG_CCM)

  return transparent_aead_encrypt_decrypt_setup(operation, attributes, key_buffer, key_buffer_size,
                                                alg, PSA_CRYPTO_DRIVER_ENCRYPT);

#else //PSA_WANT_ALG_GCM || PSA_WANT_ALG_CCM
  (void)operation;
  (void)attributes;
  (void)key_buffer;
  (void)key_buffer_size;
  (void)alg;
  return PSA_ERROR_NOT_SUPPORTED;
#endif //PSA_WANT_ALG_GCM || PSA_WANT_ALG_CCM
}

psa_status_t sli_crypto_transparent_aead_decrypt_setup(sli_crypto_transparent_aead_operation_t *operation,
                                                       const psa_key_attributes_t *attributes,
                                                       const uint8_t *key_buffer,
                                                       size_t key_buffer_size,
                                                       psa_algorithm_t alg)
{
#if defined(PSA_WANT_ALG_GCM) || defined(PSA_WANT_ALG_CCM)

  return transparent_aead_encrypt_decrypt_setup(operation, attributes, key_buffer, key_buffer_size,
                                                alg, PSA_CRYPTO_DRIVER_DECRYPT);

#else //PSA_WANT_ALG_GCM || PSA_WANT_ALG_CCM
  (void)operation;
  (void)attributes;
  (void)key_buffer;
  (void)key_buffer_size;
  (void)alg;
  return PSA_ERROR_NOT_SUPPORTED;
#endif //PSA_WANT_ALG_GCM || PSA_WANT_ALG_CCM
}

psa_status_t sli_crypto_transparent_aead_set_nonce(sli_crypto_transparent_aead_operation_t *operation,
                                                   const uint8_t *nonce,
                                                   size_t nonce_size)
{
#if defined(PSA_WANT_ALG_GCM) || defined(PSA_WANT_ALG_CCM)
  if (operation == NULL || nonce == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // Not able to set nonce twice.
  if (operation->ctx.preinit.nonce_length != 0) {
    return PSA_ERROR_BAD_STATE;
  }

  if (nonce_size > sizeof(operation->ctx.preinit.nonce)) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // Validate nonce length.
  switch (PSA_ALG_AEAD_WITH_SHORTENED_TAG(operation->alg, 0)) {
#if defined(PSA_WANT_ALG_GCM)
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, 0):
    {
      if (nonce_size != 12) {
        return PSA_ERROR_NOT_SUPPORTED;
      }
      break;
    }
#endif // PSA_WANT_ALG_GCM
#if defined(PSA_WANT_ALG_CCM)
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 0):
    {
      if (nonce_size < 7 || nonce_size > 13) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }
      break;
    }
#endif // PSA_WANT_ALG_CCM
    default:
      return PSA_ERROR_NOT_SUPPORTED;
  }

  memcpy(operation->ctx.preinit.nonce, nonce, nonce_size);
  operation->ctx.preinit.nonce_length = nonce_size;

  return PSA_SUCCESS;

#else // PSA_WANT_ALG_GCM || PSA_WANT_ALG_CCM
  (void)operation;
  (void)nonce;
  (void)nonce_size;
  return PSA_ERROR_NOT_SUPPORTED;
#endif // PSA_WANT_ALG_GCM || PSA_WANT_ALG_CCM
}

psa_status_t sli_crypto_transparent_aead_set_lengths(sli_crypto_transparent_aead_operation_t *operation,
                                                     size_t ad_length,
                                                     size_t plaintext_length)
{
#if defined(PSA_WANT_ALG_GCM) || defined(PSA_WANT_ALG_CCM)
  if (operation == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // To pass current PSA Crypto test suite, tag length encoded in the
  // algorithm needs to be checked at this point.
  switch (PSA_ALG_AEAD_WITH_SHORTENED_TAG(operation->alg, 0)) {
#if defined(PSA_WANT_ALG_GCM)
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, 0):
      (void)ad_length;
      (void)plaintext_length;
      if (PSA_ALG_AEAD_GET_TAG_LENGTH(operation->alg) < 4
          || PSA_ALG_AEAD_GET_TAG_LENGTH(operation->alg) > 16) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }
      break;
#endif
#if defined(PSA_WANT_ALG_CCM)
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 0):
      if (PSA_ALG_AEAD_GET_TAG_LENGTH(operation->alg) < 4
          || PSA_ALG_AEAD_GET_TAG_LENGTH(operation->alg) > 16
          || PSA_ALG_AEAD_GET_TAG_LENGTH(operation->alg) % 2 != 0) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }
      if (ad_length >= 0xFF00) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }
      operation->ctx.preinit.add_len = ad_length;
      operation->total_length = plaintext_length;
      break;
#endif
    default:
      return PSA_ERROR_BAD_STATE;
  }

  return PSA_SUCCESS;
#else // PSA_WANT_ALG_GCM || PSA_WANT_ALG_CCM
  (void)operation;
  (void)ad_length;
  (void)plaintext_length;
  return PSA_ERROR_NOT_SUPPORTED;
#endif // PSA_WANT_ALG_GCM || PSA_WANT_ALG_CCM
}

#if defined(PSA_WANT_ALG_GCM) || defined(PSA_WANT_ALG_CCM)
static psa_status_t crypto_aead_start(sli_crypto_transparent_aead_operation_t *operation)
{
  switch (PSA_ALG_AEAD_WITH_SHORTENED_TAG(operation->alg, 0)) {
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, 0):
#if defined PSA_WANT_ALG_GCM
      {
        sli_gcm_starts(operation,
                       NULL,
                       operation->direction,
                       operation->ctx.preinit.nonce,
                       operation->ctx.preinit.nonce_length);
        break;
      }
#endif //  PSA_WANT_ALG_GCM
#if defined(PSA_WANT_ALG_CCM)
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 0):
    {
      size_t tag_length = PSA_ALG_AEAD_GET_TAG_LENGTH(operation->alg);
      sli_ccm_starts(operation,
                     NULL,
                     operation->direction,
                     operation->ctx.preinit.nonce,
                     operation->ctx.preinit.nonce_length,
                     tag_length,
                     operation->total_length);
      break;
    }
#endif // PSA_WANT_ALG_CCM
    default:
      return PSA_ERROR_NOT_SUPPORTED;
  }
  return PSA_SUCCESS;
}
#endif // PSA_WANT_ALG_GCM || PSA_WANT_ALG_CCM

#if defined(PSA_WANT_ALG_GCM) || defined(PSA_WANT_ALG_CCM)
static psa_status_t crypto_aead_update_partial_ad(sli_crypto_transparent_aead_operation_t *operation,
                                                  CRYPTO_TypeDef *device,
                                                  const uint8_t *input,
                                                  size_t input_length)
{
  // First update_ad call.
  if (operation->add_len == 0) {
    crypto_aead_start(operation);
  }

  switch (PSA_ALG_AEAD_WITH_SHORTENED_TAG(operation->alg, 0)) {
#if defined(PSA_WANT_ALG_GCM)
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, 0):
    {
      crypto_gcm_update_partial_ad(operation,
                                   device,
                                   input,
                                   input_length);
      break;
    }
#endif // PSA_WANT_ALG_GCM
#if defined(PSA_WANT_ALG_CCM)
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 0):
    {
      crypto_ccm_update_partial_ad(operation,
                                   device,
                                   input,
                                   input_length);
      break;
    }
#endif // PSA_WANT_ALG_CCM
    default:
      return PSA_ERROR_NOT_SUPPORTED;
  }
  return PSA_SUCCESS;
}
#endif // PSA_WANT_ALG_GCM || PSA_WANT_ALG_CCM

psa_status_t sli_crypto_transparent_aead_update_ad(sli_crypto_transparent_aead_operation_t *operation,
                                                   const uint8_t *input,
                                                   size_t input_length)
{
#if defined(PSA_WANT_ALG_GCM) || defined(PSA_WANT_ALG_CCM)
  if (operation == NULL
      || (input == NULL && input_length > 0)) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  if (operation->alg == 0) {
    return PSA_ERROR_BAD_STATE;
  }

  if (operation->processed_len > 0) {
    return PSA_ERROR_BAD_STATE;
  }

  // AD are limited to 2^64 bits, so 2^61 bytes.
  // We need not check if SIZE_MAX (max of size_t) is less than 2^61 (0x2000000000000000).
#if SIZE_MAX > 0x2000000000000000ull
  if (input_length >> 61 != 0) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }
#endif

  // To make sure crypto_aead_start is not called twice.
  if (input_length == 0) {
    return PSA_SUCCESS;
  }

  // The following logic enables processing of partial non-blocksize authentication data.

  uint8_t block_size = 16;

#if defined(PSA_WANT_ALG_CCM)
  if (PSA_ALG_AEAD_WITH_SHORTENED_TAG(operation->alg, 0) == PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 0)) {
    if (operation->first_update_ad_op) {
      // CCM only processes 14 bytes on its first update CBC-MAC operation.
      block_size = 14;
    }
  }
#endif

  if (operation->unprocessed_aad_length > 16) {
    // Context is not valid.
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // Store up to 16 bytes in context in case there is more data coming. If there is no more
  // authentication data: stored data will be processed in the first update operation or in
  // the finish/verify operation.
  if ((operation->unprocessed_aad_length + input_length) < block_size && input_length < block_size) {
    memcpy(operation->unprocessed_block + operation->unprocessed_aad_length, input, input_length);
    operation->unprocessed_aad_length += input_length;
    return PSA_SUCCESS;
  }

  if (operation->unprocessed_aad_length > 0) {
    // If present, process any stored authenication data first.
    memcpy(operation->unprocessed_block + operation->unprocessed_aad_length, input, block_size
           - operation->unprocessed_aad_length);
    input += (block_size - operation->unprocessed_aad_length);
    input_length -= (block_size - operation->unprocessed_aad_length);
    operation->unprocessed_aad_length = 0;

    crypto_aead_update_partial_ad(operation,
                                  NULL,
                                  operation->unprocessed_block,
                                  block_size);

    if (input_length < block_size) {
      memcpy(operation->unprocessed_block, input, input_length);
      operation->unprocessed_aad_length = input_length;
      return PSA_SUCCESS;
    }
  }

  uint8_t incomplete_block = (input_length - block_size) % 16;

  if (incomplete_block > 0) {
    memcpy(operation->unprocessed_block, input + (input_length - incomplete_block), incomplete_block);
    operation->unprocessed_aad_length = incomplete_block;
    input_length -= incomplete_block;
  }

  crypto_aead_update_partial_ad(operation,
                                NULL,
                                input,
                                input_length);

  return PSA_SUCCESS;

#else // PSA_WANT_ALG_GCM || PSA_WANT_ALG_CCM
  (void)operation;
  (void)input;
  (void)input_length;
  return PSA_ERROR_NOT_SUPPORTED;
#endif // PSA_WANT_ALG_GCM || PSA_WANT_ALG_CCM
}

psa_status_t sli_crypto_transparent_aead_update(sli_crypto_transparent_aead_operation_t *operation,
                                                const uint8_t *input,
                                                size_t input_length,
                                                uint8_t *output,
                                                size_t output_size,
                                                size_t *output_length)
{
#if defined(PSA_WANT_ALG_GCM) || defined(PSA_WANT_ALG_CCM)

  if (output_size < input_length) {
    return PSA_ERROR_BUFFER_TOO_SMALL;
  }

  if (operation == NULL
      || ((input == NULL || output == NULL) && input_length > 0)
      || output_length == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  if (operation->alg == 0) {
    return PSA_ERROR_BAD_STATE;
  }

  // If present, process authentication data.
  if (operation->unprocessed_aad_length > 0) {
    if (operation->unprocessed_aad_length > 16) {
      return PSA_ERROR_INVALID_ARGUMENT;
    }
    crypto_aead_update_partial_ad(operation,
                                  NULL,
                                  operation->unprocessed_block,
                                  operation->unprocessed_aad_length);
    operation->unprocessed_aad_length = 0;
    memset(operation->unprocessed_block, 0, sizeof(operation->unprocessed_block));
  }

  if (input_length == 0) {
    return PSA_SUCCESS;
  }

  // Deal with input that are not a multiple of 16.
  if ((operation->unprocessed_length + input_length) < 16 && input_length < 16) {
    if (operation->unprocessed_length > 16) {
      // Context is not valid.
      return PSA_ERROR_INVALID_ARGUMENT;
    }

    // Fill context buffer and end operation.
    memcpy(operation->unprocessed_block + operation->unprocessed_length, input, input_length);
    operation->unprocessed_length += input_length;
    *output_length = 0;

    return PSA_SUCCESS;
  }

  if (operation->processed_len == 0 && operation->add_len == 0) {
    // Multipart operation not initialized.
    crypto_aead_start(operation);
  }

  uint8_t input_offset = 0;
  uint8_t output_offset = 0;

  if (operation->unprocessed_length != 0) {
    if (operation->unprocessed_length > 16) {
      // Context is not valid.
      return PSA_ERROR_INVALID_ARGUMENT;
    }

    // If there is data stored in the context it must be processed first.
    input_offset = 16 - operation->unprocessed_length;

    memcpy(operation->unprocessed_block + operation->unprocessed_length, input, input_offset);

#if defined(PSA_WANT_ALG_GCM)
    if (PSA_ALG_AEAD_WITH_SHORTENED_TAG(operation->alg, 0) == PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, 0)) {
      sli_gcm_update(operation,
                     NULL,
                     sizeof(operation->unprocessed_block),
                     operation->unprocessed_block,
                     output);
    }
#endif
#if defined(PSA_WANT_ALG_CCM)
    if (PSA_ALG_AEAD_WITH_SHORTENED_TAG(operation->alg, 0) == PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 0)) {
      sli_ccm_update(operation,
                     NULL,
                     sizeof(operation->unprocessed_block),
                     operation->unprocessed_block,
                     output);
    }
#endif

    input_length -= input_offset;
    output_offset += 16;
    input += input_offset;
    output += output_offset;

    sli_psa_zeroize(operation->unprocessed_block, sizeof(operation->unprocessed_block));
    operation->unprocessed_length = 0;

    if (input_length < 16) {
      // Fill context buffer and end operation.
      memcpy(operation->unprocessed_block, input, input_length);
      operation->unprocessed_length = input_length;
      *output_length = output_offset;

      return PSA_SUCCESS;
    }
  }
  // Store data that is not a multiple of 16 in context.
  uint8_t res_data_length = input_length % 16;
  memcpy(operation->unprocessed_block, input + (input_length - res_data_length),
         res_data_length);
  operation->unprocessed_length = res_data_length;
  input_length -= res_data_length;

  // Our drivers only support full or no overlap between input and output
  // buffers. So in the case of partial overlap, copy the input buffer into
  // the output buffer and process it in place as if the buffers fully
  // overlapped.
  if ((output > input) && (output < (input + input_length))) {
    memmove(output, input, input_length);
    input = output;
  }

  switch (PSA_ALG_AEAD_WITH_SHORTENED_TAG(operation->alg, 0)) {
#if defined(PSA_WANT_ALG_GCM)
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, 0):
    {
      sli_gcm_update(operation,
                     NULL,
                     input_length,
                     input,
                     output);

      *output_length = input_length + output_offset;
      break;
    }
#endif // PSA_WANT_ALG_GCM
#if defined(PSA_WANT_ALG_CCM)
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 0):
    {
      sli_ccm_update(operation,
                     NULL,
                     input_length,
                     input,
                     output);

      *output_length = input_length + output_offset;
      break;
    }
#endif
    default:
      return PSA_ERROR_NOT_SUPPORTED;
  }

  return PSA_SUCCESS;
#else // PSA_WANT_ALG_GCM || PSA_WANT_ALG_CCM
  (void)operation;
  (void)input;
  (void)input_length;
  (void)output;
  (void)output_size;
  (void)output_length;
  return PSA_ERROR_NOT_SUPPORTED;
#endif // PSA_WANT_ALG_GCM || PSA_WANT_ALG_CCM
}

psa_status_t sli_crypto_transparent_aead_finish(sli_crypto_transparent_aead_operation_t *operation,
                                                uint8_t *ciphertext,
                                                size_t ciphertext_size,
                                                size_t *ciphertext_length,
                                                uint8_t *tag,
                                                size_t tag_size,
                                                size_t *tag_length)
{
#if defined(PSA_WANT_ALG_GCM) || defined(PSA_WANT_ALG_CCM)

  (void)ciphertext;
  (void)ciphertext_size;

  if (operation == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  uint32_t tag_len = PSA_ALG_AEAD_GET_TAG_LENGTH(operation->alg);

  if (tag_size < tag_len) {
    return PSA_ERROR_BUFFER_TOO_SMALL;
  }

  *ciphertext_length = 0;

  // If present, process authentication data.
  if (operation->unprocessed_aad_length > 0) {
    if (operation->unprocessed_aad_length > 16) {
      return PSA_ERROR_INVALID_ARGUMENT;
    }
    crypto_aead_update_partial_ad(operation,
                                  NULL,
                                  operation->unprocessed_block,
                                  operation->unprocessed_aad_length);
    operation->unprocessed_aad_length = 0;
    memset(operation->unprocessed_block, 0, 16);
  }

  if ((operation->processed_len == 0) && (operation->add_len == 0)) {
    // Multipart operation not initialized.
    crypto_aead_start(operation);
  }

  switch (PSA_ALG_AEAD_WITH_SHORTENED_TAG(operation->alg, 0)) {
#if defined(PSA_WANT_ALG_GCM)
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, 0):
    {
      if (operation->unprocessed_length) {
        // Any unprocessed data in context must be processed first.
        if (ciphertext_size < operation->unprocessed_length) {
          return PSA_ERROR_INVALID_ARGUMENT;
        }
        sli_gcm_update(operation,
                       NULL,
                       operation->unprocessed_length,
                       operation->unprocessed_block,
                       ciphertext);

        *ciphertext_length = operation->unprocessed_length;
      }
      sli_gcm_finish(operation,
                     NULL,
                     tag,
                     tag_size);

      break;
    }
#endif //PSA_WANT_ALG_GCM
#if defined(PSA_WANT_ALG_CCM)
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 0):
    {
      if (operation->unprocessed_length) {
        // Any unprocessed data in context must be processed first.
        if (ciphertext_size < operation->unprocessed_length) {
          return PSA_ERROR_INVALID_ARGUMENT;
        }
        sli_ccm_update(operation,
                       NULL,
                       operation->unprocessed_length,
                       operation->unprocessed_block,
                       ciphertext);

        *ciphertext_length = operation->unprocessed_length;
      }
      sli_ccm_finish(operation,
                     NULL,
                     tag,
                     tag_size);
      break;
    }
#endif
    default:
      return PSA_ERROR_NOT_SUPPORTED;
  }

  sli_psa_zeroize(operation, sizeof(*operation));

  *tag_length = tag_size;

  return PSA_SUCCESS;
#else// PSA_WANT_ALG_GCM || PSA_WANT_ALG_CCM
  (void)operation;
  (void)ciphertext;
  (void)ciphertext_size;
  (void)ciphertext_length;
  (void)tag;
  (void)tag_size;
  (void)tag_length;
  return PSA_ERROR_NOT_SUPPORTED;
#endif// PSA_WANT_ALG_GCM || PSA_WANT_ALG_CCM
}

psa_status_t sli_crypto_transparent_aead_verify(sli_crypto_transparent_aead_operation_t *operation,
                                                uint8_t *plaintext,
                                                size_t plaintext_size,
                                                size_t *plaintext_length,
                                                const uint8_t *tag,
                                                size_t tag_length)
{
#if defined(PSA_WANT_ALG_GCM) || defined(PSA_WANT_ALG_CCM)

  psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;

  size_t tag_length_out = 0;

  uint8_t calc_tag[PSA_AEAD_TAG_MAX_SIZE] = { 0 };

  if (tag == NULL || tag_length == 0 ) {
    return PSA_ERROR_INVALID_SIGNATURE;
  }

  if (tag_length != PSA_ALG_AEAD_GET_TAG_LENGTH(operation->alg)) {
    return PSA_ERROR_INVALID_SIGNATURE;
  }

  status = sli_crypto_transparent_aead_finish(operation,
                                              plaintext,
                                              plaintext_size,
                                              plaintext_length,
                                              calc_tag,
                                              tag_length,
                                              &tag_length_out);

  if (status != PSA_SUCCESS) {
    return status;
  }

  if (tag_length != tag_length_out) {
    return PSA_ERROR_BAD_STATE;
  }

  // Check that the provided tag equals the calculated tag
  // (in constant time).
  uint8_t diff = sli_psa_safer_memcmp(calc_tag, tag, tag_length);
  sli_psa_zeroize(calc_tag, tag_length);

  if (diff != 0) {
    return PSA_ERROR_INVALID_SIGNATURE;
  }

  return PSA_SUCCESS;
#else // PSA_WANT_ALG_GCM || PSA_WANT_ALG_CCM
  (void)operation;
  (void)plaintext;
  (void)plaintext_size;
  (void)plaintext_length;
  (void)tag;
  (void)tag_length;
  return PSA_ERROR_NOT_SUPPORTED;
#endif // PSA_WANT_ALG_GCM || PSA_WANT_ALG_CCM
}

psa_status_t sli_crypto_transparent_aead_abort(sli_crypto_transparent_aead_operation_t *operation)
{
  if (operation == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  memset(operation, 0, sizeof(*operation));
  return PSA_SUCCESS;
}

#endif // CRYPTO_PRESENT
