/***************************************************************************//**
 * @file
 * @brief Silicon Labs PSA Crypto Driver AEAD functions.
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
#include "sli_se_driver_key_management.h"
#include "sli_se_driver_aead.h"

#include "sl_se_manager.h"
#include "sl_se_manager_cipher.h"
#include "sli_se_manager_internal.h"

#include "sli_psa_driver_common.h"

#include <string.h>

#if defined(PSA_WANT_ALG_CCM)                 \
  || defined(PSA_WANT_ALG_GCM)                \
  || (defined(PSA_WANT_ALG_CHACHA20_POLY1305) \
  && (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT))

static psa_status_t check_aead_parameters(const psa_key_attributes_t *attributes,
                                          psa_algorithm_t alg,
                                          size_t nonce_length,
                                          size_t additional_data_length)
{
  size_t tag_length = PSA_AEAD_TAG_LENGTH(psa_get_key_type(attributes),
                                          psa_get_key_bits(attributes),
                                          alg);

#if !defined(PSA_WANT_ALG_GCM)
  (void)additional_data_length;
#endif // PSA_WANT_ALG_GCM

  switch (PSA_ALG_AEAD_WITH_SHORTENED_TAG(alg, 0)) {
#if defined(PSA_WANT_ALG_CCM)
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 0):
      // verify key type
      if (psa_get_key_type(attributes) != PSA_KEY_TYPE_AES) {
        return PSA_ERROR_NOT_SUPPORTED;
      }
      switch (psa_get_key_bits(attributes)) {
        case 128: // Fallthrough
        case 192: // Fallthrough
        case 256:
          break;
        default:
          return PSA_ERROR_INVALID_ARGUMENT;
      }
      // verify nonce and tag lengths
      if (tag_length < 4 || tag_length > 16 || tag_length % 2 != 0
          || nonce_length < 7 || nonce_length > 13) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }
      break;
#endif // PSA_WANT_ALG_CCM
#if defined(PSA_WANT_ALG_GCM)
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, 0):
      // AD are limited to 2^64 bits, so 2^61 bytes.
      // We need not check if SIZE_MAX (max of size_t) is less than 2^61 (0x2000000000000000)
#if SIZE_MAX > 0x2000000000000000ull
      if (additional_data_length >> 61 != 0) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }
#else // SIZE_MAX > 0x2000000000000000ull
      (void) additional_data_length;
#endif // SIZE_MAX > 0x2000000000000000ull
      // verify key type
      if (psa_get_key_type(attributes) != PSA_KEY_TYPE_AES) {
        return PSA_ERROR_NOT_SUPPORTED;
      }
      switch (psa_get_key_bits(attributes)) {
        case 128: // Fallthrough
        case 192: // Fallthrough
        case 256:
          break;
        default:
          return PSA_ERROR_INVALID_ARGUMENT;
      }
      // verify nonce and tag lengths
      if ((tag_length < 4) || (tag_length > 16)) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }
      if (nonce_length == 0) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }
#if !defined(SLI_PSA_SUPPORT_GCM_IV_CALCULATION)
      if (nonce_length != 12) {
        return PSA_ERROR_NOT_SUPPORTED;
      }
#endif // SLI_PSA_SUPPORT_GCM_IV_CALCULATION
      break;
#endif // PSA_WANT_ALG_GCM
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
#if defined(PSA_WANT_ALG_CHACHA20_POLY1305)
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CHACHA20_POLY1305, 0):
      // verify key type
      if (psa_get_key_type(attributes) != PSA_KEY_TYPE_CHACHA20
          || psa_get_key_bits(attributes) != 256) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }

      // verify nonce and tag lengths
      if (nonce_length != 12 || tag_length != 16) {
        return PSA_ERROR_NOT_SUPPORTED;
      }
      break;
#endif // PSA_WANT_ALG_CHACHA20_POLY1305
#endif // VAULT
    default:
      return PSA_ERROR_NOT_SUPPORTED;
  }
  return PSA_SUCCESS;
}

#endif // PSA_WANT_ALG_CCM || PSA_WANT_ALG_GCM || PSA_WANT_ALG_CHACHA20_POLY1305

#if defined(SLI_PSA_SUPPORT_GCM_IV_CALCULATION) && defined(PSA_WANT_ALG_GCM)
/* Do GCM in software in case the IV isn't 12 bytes, since that's the only
 * thing the accelerator supports. */
static psa_status_t sli_se_driver_software_gcm(sl_se_command_context_t *cmd_ctx,
                                               sl_se_key_descriptor_t *key_desc,
                                               const uint8_t* nonce,
                                               size_t nonce_length,
                                               const uint8_t* additional_data,
                                               size_t additional_data_length,
                                               const uint8_t* input,
                                               uint8_t* output,
                                               size_t plaintext_length,
                                               size_t tag_length,
                                               uint8_t* tag,
                                               bool encrypt_ndecrypt)
{
  // Step 1: calculate H = Ek(0)
  uint8_t Ek[16] = { 0 };
  psa_status_t status = sl_se_aes_crypt_ecb(cmd_ctx,
                                            key_desc,
                                            SL_SE_ENCRYPT,
                                            sizeof(Ek),
                                            (const unsigned char *)Ek,
                                            Ek);

  if (status != SL_STATUS_OK) {
    return PSA_ERROR_HARDWARE_FAILURE;
  }

  // Step 2: calculate IV = GHASH(H, {}, IV)
  uint8_t iv[16] = { 0 };
  uint64_t HL[16], HH[16];

  sli_psa_software_ghash_setup(Ek, HL, HH);

  for (size_t i = 0; i < nonce_length; i += 16) {
    // Mix in IV
    for (size_t j = 0; j < (nonce_length - i > 16 ? 16 : nonce_length - i); j++) {
      iv[j] ^= nonce[i + j];
    }
    // Update result
    sli_psa_software_ghash_multiply(HL, HH, iv, iv);
  }

  iv[12] ^= (nonce_length * 8) >> 24;
  iv[13] ^= (nonce_length * 8) >> 16;
  iv[14] ^= (nonce_length * 8) >>  8;
  iv[15] ^= (nonce_length * 8) >>  0;

  sli_psa_software_ghash_multiply(HL, HH, iv, iv);

  // Step 3: Calculate first counter block for tag generation
  uint8_t tagbuf[16] = { 0 };
  status = sl_se_aes_crypt_ecb(cmd_ctx,
                               key_desc,
                               SL_SE_ENCRYPT,
                               sizeof(iv),
                               (const unsigned char *)iv,
                               tagbuf);

  if (status != SL_STATUS_OK) {
    return PSA_ERROR_HARDWARE_FAILURE;
  }

  // If we're decrypting, mix in the to-be-checked tag value before transforming
  if (!encrypt_ndecrypt) {
    for (size_t i = 0; i < tag_length; i++) {
      tagbuf[i] ^= tag[i];
    }
  }

  // Step 4: increment IV (ripple increment)
  for (size_t i = 0; i < 16; i++) {
    iv[15 - i]++;

    if (iv[15 - i] != 0) {
      break;
    }
  }

  // Step 5: Accumulate additional data
  memset(Ek, 0, sizeof(Ek));
  for (size_t i = 0; i < additional_data_length; i += 16) {
    // Mix in additional data as much as we have
    for (size_t j = 0;
         j < (additional_data_length - i > 16 ? 16 : additional_data_length - i);
         j++) {
      Ek[j] ^= additional_data[i + j];
    }

    sli_psa_software_ghash_multiply(HL, HH, Ek, Ek);
  }

  // Step 6: If we're decrypting, accumulate the ciphertext before it gets transformed
  if (!encrypt_ndecrypt) {
    for (size_t i = 0; i < plaintext_length; i += 16) {
      // Mix in ciphertext
      for (size_t j = 0;
           j < (plaintext_length - i > 16 ? 16 : plaintext_length - i);
           j++) {
        Ek[j] ^= input[i + j];
      }

      sli_psa_software_ghash_multiply(HL, HH, Ek, Ek);
    }
  }

  // Step 7: transform data using AES-CTR
  uint32_t nc = 0;
  uint8_t nc_buff[16];
  status = sl_se_aes_crypt_ctr(cmd_ctx,
                               key_desc,
                               plaintext_length,
                               &nc,
                               iv,
                               nc_buff,
                               input,
                               output);
  if (status != SL_STATUS_OK) {
    return PSA_ERROR_HARDWARE_FAILURE;
  }

  // Step 8: If we're encrypting, accumulate the ciphertext now
  if (encrypt_ndecrypt) {
    for (size_t i = 0; i < plaintext_length; i += 16) {
      // Mix in ciphertext
      for (size_t j = 0;
           j < (plaintext_length - i > 16 ? 16 : plaintext_length - i);
           j++) {
        Ek[j] ^= output[i + j];
      }

      sli_psa_software_ghash_multiply(HL, HH, Ek, Ek);
    }
  }

  // Step 9: add len(A) || len(C) block to tag calculation
  uint64_t bitlen = additional_data_length * 8;
  Ek[0]  ^= bitlen >> 56;
  Ek[1]  ^= bitlen >> 48;
  Ek[2]  ^= bitlen >> 40;
  Ek[3]  ^= bitlen >> 32;
  Ek[4]  ^= bitlen >> 24;
  Ek[5]  ^= bitlen >> 16;
  Ek[6]  ^= bitlen >>  8;
  Ek[7]  ^= bitlen >>  0;

  bitlen = plaintext_length * 8;
  Ek[8]  ^= bitlen >> 56;
  Ek[9]  ^= bitlen >> 48;
  Ek[10] ^= bitlen >> 40;
  Ek[11] ^= bitlen >> 32;
  Ek[12] ^= bitlen >> 24;
  Ek[13] ^= bitlen >> 16;
  Ek[14] ^= bitlen >>  8;
  Ek[15] ^= bitlen >>  0;

  sli_psa_software_ghash_multiply(HL, HH, Ek, Ek);

  // Step 10: calculate tag value
  for (size_t i = 0; i < tag_length; i++) {
    tagbuf[i] ^= Ek[i];
  }

  // Step 11: output tag for encrypt operation, check tag for decrypt
  if (encrypt_ndecrypt) {
    memcpy(tag, tagbuf, tag_length);
  } else {
    uint8_t accumulator = 0;
    for (size_t i = 0; i < tag_length; i++) {
      accumulator |= tagbuf[i];
    }
    if (accumulator != 0) {
      return PSA_ERROR_INVALID_SIGNATURE;
    }
  }

  return PSA_SUCCESS;
}
#endif // SLI_PSA_SUPPORT_GCM_IV_CALCULATION && PSA_WANT_ALG_GCM

psa_status_t sli_se_driver_aead_encrypt(const psa_key_attributes_t *attributes,
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
#if defined(PSA_WANT_ALG_CCM)                 \
  || defined(PSA_WANT_ALG_GCM)                \
  || (defined(PSA_WANT_ALG_CHACHA20_POLY1305) \
  && (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT))

  if (ciphertext_size <= plaintext_length) {
    return PSA_ERROR_BUFFER_TOO_SMALL;
  }

  size_t tag_length = 0;
  psa_status_t psa_status = sli_se_driver_aead_encrypt_tag(
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

#else // PSA_WANT_ALG_CCM || PSA_WANT_ALG_GCM || PSA_WANT_ALG_CHACHA20_POLY1305

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

#endif // PSA_WANT_ALG_CCM || PSA_WANT_ALG_GCM || PSA_WANT_ALG_CHACHA20_POLY1305
}

psa_status_t sli_se_driver_aead_decrypt(const psa_key_attributes_t *attributes,
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
#if defined(PSA_WANT_ALG_CCM)                 \
  || defined(PSA_WANT_ALG_GCM)                \
  || (defined(PSA_WANT_ALG_CHACHA20_POLY1305) \
  && (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT))
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

  return sli_se_driver_aead_decrypt_tag(
    attributes, key_buffer, key_buffer_size, alg,
    nonce, nonce_length,
    additional_data, additional_data_length,
    ciphertext, ciphertext_length - tag_length,
    check_tag, tag_length,
    plaintext, plaintext_size, plaintext_length);

#else // PSA_WANT_ALG_CCM || PSA_WANT_ALG_GCM || PSA_WANT_ALG_CHACHA20_POLY1305

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

#endif // PSA_WANT_ALG_CCM || PSA_WANT_ALG_GCM || PSA_WANT_ALG_CHACHA20_POLY1305
}

psa_status_t sli_se_driver_aead_encrypt_tag(const psa_key_attributes_t *attributes,
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
#if defined(PSA_WANT_ALG_CCM)                 \
  || defined(PSA_WANT_ALG_GCM)                \
  || (defined(PSA_WANT_ALG_CHACHA20_POLY1305) \
  && (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT))

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

  sl_status_t status;
  psa_status_t psa_status;
  *tag_length = PSA_AEAD_TAG_LENGTH(psa_get_key_type(attributes),
                                    psa_get_key_bits(attributes),
                                    alg);

  // Verify that the driver supports the given parameters
  psa_status = check_aead_parameters(attributes, alg, nonce_length, additional_data_length);
  if (psa_status != PSA_SUCCESS) {
    return psa_status;
  }

  // Ephemeral contexts
  sl_se_command_context_t cmd_ctx = { 0 };
  sl_se_key_descriptor_t key_desc = { 0 };

  status = sl_se_init_command_context(&cmd_ctx);
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

  psa_status = PSA_ERROR_BAD_STATE;
  switch (PSA_ALG_AEAD_WITH_SHORTENED_TAG(alg, 0)) {
#if defined(PSA_WANT_ALG_CCM)
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 0):

      status = sl_se_ccm_encrypt_and_tag(&cmd_ctx,
                                         &key_desc,
                                         plaintext_length,
                                         nonce, nonce_length,
                                         additional_data, additional_data_length,
                                         plaintext,
                                         ciphertext,
                                         tag, *tag_length);

      if (status == SL_STATUS_INVALID_PARAMETER) {
        psa_status = PSA_ERROR_INVALID_ARGUMENT;
      } else if (status == SL_STATUS_OK) {
        psa_status = PSA_SUCCESS;
      } else {
        psa_status = PSA_ERROR_HARDWARE_FAILURE;
      }
      break;
#endif // PSA_WANT_ALG_CCM

#if defined(PSA_WANT_ALG_GCM)
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, 0):
      if (nonce_length == 12) {
        status = sl_se_gcm_crypt_and_tag(&cmd_ctx,
                                         &key_desc,
                                         SL_SE_ENCRYPT,
                                         plaintext_length,
                                         nonce, nonce_length,
                                         additional_data, additional_data_length,
                                         plaintext,
                                         ciphertext,
                                         *tag_length, tag);

        if (status == SL_STATUS_INVALID_PARAMETER) {
          psa_status = PSA_ERROR_INVALID_ARGUMENT;
        } else if (status == SL_STATUS_OK) {
          psa_status = PSA_SUCCESS;
        } else {
          psa_status = PSA_ERROR_HARDWARE_FAILURE;
        }
      }
#if defined(SLI_PSA_SUPPORT_GCM_IV_CALCULATION)
      else {
        psa_status = sli_se_driver_software_gcm(&cmd_ctx,
                                                &key_desc,
                                                nonce, nonce_length,
                                                additional_data, additional_data_length,
                                                plaintext,
                                                ciphertext,
                                                plaintext_length,
                                                *tag_length,
                                                tag,
                                                true);
      }
#else // SLI_PSA_SUPPORT_GCM_IV_CALCULATION
      else {
        psa_status = PSA_ERROR_NOT_SUPPORTED;
      }
#endif // SLI_PSA_SUPPORT_GCM_IV_CALCULATION
      break;
#endif // PSA_WANT_ALG_GCM

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
#if defined(PSA_WANT_ALG_CHACHA20_POLY1305)
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CHACHA20_POLY1305, 0):
    {
      #if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_1)
      // EFR32xG21 doesn't support the special case where both the message
      // and additional data length are zero.
      if (plaintext_length == 0 && additional_data_length == 0) {
        return PSA_ERROR_NOT_SUPPORTED;
      }
      #endif

      uint8_t tagbuf[16];

      status = sl_se_chacha20_poly1305_encrypt_and_tag(&cmd_ctx,
                                                       &key_desc,
                                                       plaintext_length,
                                                       nonce,
                                                       additional_data, additional_data_length,
                                                       plaintext,
                                                       ciphertext,
                                                       tagbuf);

      if (status == SL_STATUS_INVALID_PARAMETER) {
        psa_status = PSA_ERROR_INVALID_ARGUMENT;
      } else if (status == SL_STATUS_OK) {
        memcpy(tag, tagbuf, *tag_length);
        psa_status = PSA_SUCCESS;
      } else {
        psa_status = PSA_ERROR_HARDWARE_FAILURE;
      }
      break;
    }
#endif // PSA_WANT_ALG_CHACHA20_POLY1305
#endif // VAULT
  }

  if (psa_status == PSA_SUCCESS) {
    *ciphertext_length = plaintext_length;
  } else {
    *ciphertext_length = 0;
    *tag_length = 0;
  }

  return psa_status;

#else // PSA_WANT_ALG_CCM || PSA_WANT_ALG_GCM || PSA_WANT_ALG_CHACHA20_POLY1305

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

#endif // PSA_WANT_ALG_CCM || PSA_WANT_ALG_GCM || PSA_WANT_ALG_CHACHA20_POLY1305
}

psa_status_t sli_se_driver_aead_decrypt_tag(const psa_key_attributes_t *attributes,
                                            const uint8_t *key_buffer,
                                            size_t key_buffer_size,
                                            psa_algorithm_t alg,
                                            const uint8_t *nonce,
                                            size_t nonce_length,
                                            const uint8_t *additional_data,
                                            size_t additional_data_length,
                                            const uint8_t *ciphertext,
                                            size_t ciphertext_length,
                                            const uint8_t* tag,
                                            size_t tag_length,
                                            uint8_t *plaintext,
                                            size_t plaintext_size,
                                            size_t *plaintext_length)
{
#if defined(PSA_WANT_ALG_CCM)                 \
  || defined(PSA_WANT_ALG_GCM)                \
  || (defined(PSA_WANT_ALG_CHACHA20_POLY1305) \
  && (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT))
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

  sl_status_t status;
  psa_status_t psa_status;

  // Verify that the driver supports the given parameters
  psa_status = check_aead_parameters(attributes, alg, nonce_length, additional_data_length);
  if (psa_status != PSA_SUCCESS) {
    return psa_status;
  }

  // Ephemeral contexts
  sl_se_command_context_t cmd_ctx = { 0 };
  sl_se_key_descriptor_t key_desc = { 0 };

  status = sl_se_init_command_context(&cmd_ctx);
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

  psa_status = PSA_ERROR_BAD_STATE;
  switch (PSA_ALG_AEAD_WITH_SHORTENED_TAG(alg, 0)) {
#if defined(PSA_WANT_ALG_CCM)
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 0):

      status = sl_se_ccm_auth_decrypt(&cmd_ctx,
                                      &key_desc,
                                      ciphertext_length,
                                      nonce, nonce_length,
                                      additional_data, additional_data_length,
                                      ciphertext,
                                      plaintext,
                                      tag, tag_length);

      if (status == SL_STATUS_INVALID_PARAMETER) {
        return PSA_ERROR_INVALID_ARGUMENT;
      } else if (status == SL_STATUS_INVALID_SIGNATURE) {
        return PSA_ERROR_INVALID_SIGNATURE;
      } else if (status == SL_STATUS_OK) {
        *plaintext_length = ciphertext_length;
        psa_status = PSA_SUCCESS;
      } else {
        return PSA_ERROR_HARDWARE_FAILURE;
      }
      break;
#endif // PSA_WANT_ALG_CCM

#if defined(PSA_WANT_ALG_GCM)
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, 0):
      if (nonce_length == 12) {
        status = sl_se_gcm_auth_decrypt(&cmd_ctx,
                                        &key_desc,
                                        ciphertext_length,
                                        nonce, nonce_length,
                                        additional_data, additional_data_length,
                                        ciphertext,
                                        plaintext,
                                        tag_length, tag);

        if (status == SL_STATUS_INVALID_PARAMETER) {
          return PSA_ERROR_INVALID_ARGUMENT;
        } else if (status == SL_STATUS_INVALID_SIGNATURE) {
          return PSA_ERROR_INVALID_SIGNATURE;
        } else if (status == SL_STATUS_OK) {
          *plaintext_length = ciphertext_length;
          psa_status = PSA_SUCCESS;
        } else {
          return PSA_ERROR_HARDWARE_FAILURE;
        }
      }
#if defined(SLI_PSA_SUPPORT_GCM_IV_CALCULATION)
      else {
        psa_status = sli_se_driver_software_gcm(&cmd_ctx,
                                                &key_desc,
                                                nonce, nonce_length,
                                                additional_data, additional_data_length,
                                                ciphertext,
                                                plaintext,
                                                ciphertext_length,
                                                tag_length,
                                                (uint8_t*) tag,
                                                false);
        if (psa_status == PSA_SUCCESS) {
          *plaintext_length = ciphertext_length;
        }
      }
#else // SLI_PSA_SUPPORT_GCM_IV_CALCULATION
      else {
        psa_status = PSA_ERROR_NOT_SUPPORTED;
      }
#endif // SLI_PSA_SUPPORT_GCM_IV_CALCULATION
      break;
#endif // PSA_WANT_ALG_CCM

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
#if defined(PSA_WANT_ALG_CHACHA20_POLY1305)
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CHACHA20_POLY1305, 0):

      #if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_1)
      // EFR32xG21 doesn't support the special case where both the message
      // and additional data length are zero.
      if (ciphertext_length == 0 && additional_data_length == 0) {
        return PSA_ERROR_NOT_SUPPORTED;
      }
      #endif

      // Vault devices currently do not support ChaCha20-Poly1305 with truncated
      // tag lengths. RFC8439 also disallows truncating the tag.
      if (tag_length != 16) {
        return PSA_ERROR_NOT_SUPPORTED;
      }

      status = sl_se_chacha20_poly1305_auth_decrypt(&cmd_ctx,
                                                    &key_desc,
                                                    ciphertext_length,
                                                    nonce,
                                                    additional_data, additional_data_length,
                                                    ciphertext,
                                                    plaintext,
                                                    tag);

      if (status == SL_STATUS_INVALID_PARAMETER) {
        return PSA_ERROR_INVALID_ARGUMENT;
      } else if (status == SL_STATUS_INVALID_SIGNATURE) {
        return PSA_ERROR_INVALID_SIGNATURE;
      } else if (status == SL_STATUS_OK) {
        *plaintext_length = ciphertext_length;
        psa_status = PSA_SUCCESS;
      } else {
        return PSA_ERROR_HARDWARE_FAILURE;
      }
      break;
#endif // PSA_WANT_ALG_CHACHA20_POLY1305
#endif // VAULT
  }

  return psa_status;

#else // PSA_WANT_ALG_CCM || PSA_WANT_ALG_GCM || PSA_WANT_ALG_CHACHA20_POLY1305

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

#endif // PSA_WANT_ALG_CCM || PSA_WANT_ALG_GCM || PSA_WANT_ALG_CHACHA20_POLY1305
}
psa_status_t sli_se_driver_aead_encrypt_decrypt_setup(sli_se_driver_aead_operation_t *operation,
                                                      const psa_key_attributes_t *attributes,
                                                      const uint8_t *key_buffer,
                                                      size_t key_buffer_size,
                                                      psa_algorithm_t alg,
                                                      sl_se_cipher_operation_t operation_direction,
                                                      uint8_t *key_storage_buffer,
                                                      size_t key_storage_buffer_size,
                                                      size_t key_storage_overhead)
{
#if defined(PSA_WANT_ALG_CCM) \
  || defined(PSA_WANT_ALG_GCM)

  if (operation == NULL
      || attributes == NULL
      || key_buffer == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  size_t key_bits = psa_get_key_bits(attributes);
  size_t key_size = PSA_BITS_TO_BYTES(key_bits);

  if (key_buffer_size < key_size) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // Validate key type
  if (psa_get_key_type(attributes) != PSA_KEY_TYPE_AES) {
    return PSA_ERROR_NOT_SUPPORTED;
  }

  // Validate tag length.
  if ( PSA_AEAD_TAG_LENGTH(psa_get_key_type(attributes), key_bits, alg) > 16 ) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // Reset context
  memset(operation, 0, sizeof(*operation));

  // Validate operation
  switch (PSA_ALG_AEAD_WITH_SHORTENED_TAG(alg, 0)) {
#if defined(PSA_WANT_ALG_GCM)
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, 0):
      operation->alg = alg;
      break;
#endif
#if defined(PSA_WANT_ALG_CCM)
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 0):
      operation->alg = alg;
      break;
#endif
    default:
      return PSA_ERROR_NOT_SUPPORTED;
  }

  // Prepare key descriptor
  psa_status_t psa_status = sli_se_key_desc_from_input(attributes,
                                                       key_buffer,
                                                       key_buffer_size,
                                                       &(operation->key_desc));
  if (psa_status != PSA_SUCCESS) {
    return psa_status;
  }

  // Verify length and copy key material to context
  uint32_t key_len = 0;
  sl_status_t status = sli_key_get_size(&(operation->key_desc), &key_len);
  if (status != SL_STATUS_OK) {
    return PSA_ERROR_HARDWARE_FAILURE;
  }

  switch (key_len) {
    case 16: // Fallthrough
    case 24: // Fallthrough
    case 32:
      break;
    default:
      return PSA_ERROR_INVALID_ARGUMENT;
  }

  if (key_storage_buffer_size < key_storage_overhead + key_len) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }
  memcpy(key_storage_buffer, operation->key_desc.storage.location.buffer.pointer, key_storage_overhead + key_len);

  // Point key_descriptor at internal copy of key
  operation->key_desc.storage.location.buffer.pointer = key_storage_buffer;

  // Set direction of operation
  operation->ctx.preinit.direction = operation_direction;
  return PSA_SUCCESS;

#else // PSA_WANT_ALG_CCM || PSA_WANT_ALG_GCM

  (void)operation;
  (void)attributes;
  (void)key_buffer;
  (void)key_buffer_size;
  (void)alg;
  (void)operation_direction;
  (void)key_storage_buffer;
  (void)key_storage_buffer_size;
  (void)key_storage_overhead;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_CCM || PSA_WANT_ALG_GCM
}

psa_status_t sli_se_driver_aead_set_nonce(sli_se_driver_aead_operation_t *operation,
                                          const uint8_t *nonce,
                                          size_t nonce_size)
{
#if defined(PSA_WANT_ALG_CCM)  \
  || defined(PSA_WANT_ALG_GCM) \

  if (operation == NULL
      || nonce == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // Setting nonce twice isn't supported
  if (operation->ctx.preinit.nonce_length != 0) {
    return PSA_ERROR_BAD_STATE;
  }

#if defined(PSA_WANT_ALG_GCM)
  // Non-12-byte IV is not supported for multipart GCM
  if (PSA_ALG_AEAD_WITH_SHORTENED_TAG(operation->alg, 0)
      == PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, 0)) {
    if (nonce_size != 12) {
      return PSA_ERROR_NOT_SUPPORTED;
    }
  }
#endif

  if (nonce_size <= sizeof(operation->ctx.preinit.nonce)) {
    memcpy(operation->ctx.preinit.nonce, nonce, nonce_size);
    operation->ctx.preinit.nonce_length = nonce_size;
    return PSA_SUCCESS;
  } else {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

#else // PSA_WANT_ALG_CCM || PSA_WANT_ALG_GCM

  (void)operation;
  (void)nonce;
  (void)nonce_size;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_CCM || PSA_WANT_ALG_GCM
}

psa_status_t sli_se_driver_aead_set_lengths(sli_se_driver_aead_operation_t *operation,
                                            size_t ad_length,
                                            size_t plaintext_length)
{
#if (defined(PSA_WANT_ALG_CCM)) \
  || defined(PSA_WANT_ALG_GCM)

  if (operation == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // To pass current PSA Crypto test suite, tag length encoded in the
  // algorithm needs to be checked at this point.
  switch (PSA_ALG_AEAD_WITH_SHORTENED_TAG(operation->alg, 0)) {
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 0):
      if ((PSA_ALG_AEAD_GET_TAG_LENGTH(operation->alg) % 2 != 0)
          || PSA_ALG_AEAD_GET_TAG_LENGTH(operation->alg) < 4
          || PSA_ALG_AEAD_GET_TAG_LENGTH(operation->alg) > 16) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }
      break;
#if defined(PSA_WANT_ALG_GCM)
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, 0):
      if (PSA_ALG_AEAD_GET_TAG_LENGTH(operation->alg) < 4
          || PSA_ALG_AEAD_GET_TAG_LENGTH(operation->alg) > 16) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }
      break;
#endif
    default:
      return PSA_ERROR_BAD_STATE;
  }

  if (operation->ad_len != 0 || operation->pt_len != 0) {
    return PSA_ERROR_BAD_STATE;
  }

  operation->ctx.preinit.ad_length = ad_length;
  operation->ctx.preinit.pt_length = plaintext_length;
  return PSA_SUCCESS;

#else // PSA_WANT_ALG_CCM || PSA_WANT_ALG_GCM
  (void)operation;
  (void)ad_length;
  (void)plaintext_length;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_CCM || PSA_WANT_ALG_GCM
}

#if defined(PSA_WANT_ALG_CCM) \
  || defined(PSA_WANT_ALG_GCM)

static psa_status_t aead_start(sli_se_driver_aead_operation_t *operation,
                               const uint8_t *input,
                               size_t input_length)
{
  // Ephemeral contexts
#if defined(PSA_WANT_ALG_GCM) \
  || defined(PSA_WANT_ALG_CCM)
  sli_se_driver_aead_preinit_t preinit = operation->ctx.preinit;
#endif

  sl_se_command_context_t cmd_ctx = { 0 };

  sl_status_t status = sl_se_init_command_context(&cmd_ctx);
  if (status != SL_STATUS_OK) {
    return PSA_ERROR_HARDWARE_FAILURE;
  }

#if defined(PSA_WANT_ALG_CCM)
  uint8_t tag_length = PSA_ALG_AEAD_GET_TAG_LENGTH(operation->alg);
#endif//PSA_WANT_ALG_CCM

  psa_algorithm_t alg = PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG(operation->alg);

  switch (alg) {
#if defined(PSA_WANT_ALG_GCM)
    case PSA_ALG_GCM:
      status = sl_se_gcm_multipart_starts(&operation->ctx.gcm,
                                          &cmd_ctx,
                                          &operation->key_desc,
                                          preinit.direction,
                                          preinit.nonce,
                                          preinit.nonce_length,
                                          input,
                                          input_length);
      if (status != SL_STATUS_OK) {
        return PSA_ERROR_HARDWARE_FAILURE;
      }

      operation->ad_len += input_length;
      return PSA_SUCCESS;
#endif//PSA_WANT_ALG_GCM
#if defined(PSA_WANT_ALG_CCM)
    case PSA_ALG_CCM:
      status = sl_se_ccm_multipart_starts(&operation->ctx.ccm,
                                          &cmd_ctx,
                                          &operation->key_desc,
                                          preinit.direction,
                                          preinit.pt_length,
                                          preinit.nonce,
                                          preinit.nonce_length,
                                          input,
                                          input_length,
                                          tag_length);

      if (status != SL_STATUS_OK) {
        return PSA_ERROR_HARDWARE_FAILURE;
      }
      operation->ad_len += input_length;
      return PSA_SUCCESS;
#endif //PSA_WANT_ALG_CCM
    default:
      (void) input;
      (void) input_length;
      return PSA_ERROR_NOT_SUPPORTED;
  }
}
#endif // PSA_WANT_ALG_CCM || PSA_WANT_ALG_GCM

psa_status_t sli_se_driver_aead_update_ad(sli_se_driver_aead_operation_t *operation,
                                          uint8_t *key_buffer,
                                          const uint8_t *input,
                                          size_t input_length)
{
#if defined(PSA_WANT_ALG_CCM) \
  || defined(PSA_WANT_ALG_GCM)

  if (operation == NULL
      || key_buffer == NULL
      || (input == NULL && input_length > 0)) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  if (operation->alg == 0) {
    return PSA_ERROR_BAD_STATE;
  }

  if (operation->ad_len > 0 || operation->pt_len > 0) {
    return PSA_ERROR_BAD_STATE;
  }

  // Start operation
  if (input_length == 0) {
    return PSA_SUCCESS;
  }

  return aead_start(operation, input, input_length);

#else // PSA_WANT_ALG_CCM || PSA_WANT_ALG_GCM

  (void)operation;
  (void)key_buffer;
  (void)input;
  (void)input_length;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_CCM || PSA_WANT_ALG_GCM
}

psa_status_t sli_se_driver_aead_update(sli_se_driver_aead_operation_t *operation,
                                       uint8_t *key_buffer,
                                       const uint8_t *input,
                                       size_t input_length,
                                       uint8_t *output,
                                       size_t output_size,
                                       size_t *output_length)
{
#if defined(PSA_WANT_ALG_CCM) \
  || defined(PSA_WANT_ALG_GCM)

  (void)key_buffer;
  sl_status_t status;

  sl_se_command_context_t cmd_ctx = { 0 };

  status = sl_se_init_command_context(&cmd_ctx);
  if (status != SL_STATUS_OK) {
    return PSA_ERROR_HARDWARE_FAILURE;
  }

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

  // Start operation
  if (input_length == 0) {
    return PSA_SUCCESS;
  }

  psa_algorithm_t alg = PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG(operation->alg);

  psa_status_t psa_status;

  // Operation isn't initialised unless we have either AD or PT, so if we are
  // still at 0, we need to run the start step.
  if (operation->ad_len == 0 && operation->pt_len == 0) {
    psa_status = aead_start(operation, NULL, 0);
    if (psa_status != PSA_SUCCESS) {
      return psa_status;
    }
  }

  switch (alg) {
#if defined(PSA_WANT_ALG_GCM)
    case PSA_ALG_GCM:
    {
      status = sl_se_gcm_multipart_update(&operation->ctx.gcm,
                                          &cmd_ctx,
                                          &operation->key_desc,
                                          input_length,
                                          input,
                                          output,
                                          output_length);
      if (status != SL_STATUS_OK) {
        return PSA_ERROR_HARDWARE_FAILURE;
      }
      psa_status = PSA_SUCCESS;
      operation->pt_len += input_length;
      break;
    }
#endif // PSA_WANT_ALG_GCM
#if defined(PSA_WANT_ALG_CCM)
    case PSA_ALG_CCM:
    {
      status = sl_se_ccm_multipart_update(&operation->ctx.ccm,
                                          &cmd_ctx,
                                          &operation->key_desc,
                                          input_length,
                                          input,
                                          output,
                                          output_length);
      if (status != SL_STATUS_OK) {
        return PSA_ERROR_HARDWARE_FAILURE;
      }
      psa_status = PSA_SUCCESS;
      operation->pt_len += input_length;
      break;
    }
#endif //PSA_WANT_ALG_CCM
    default:
      psa_status = PSA_ERROR_NOT_SUPPORTED;
      break;
  }

  return psa_status;

#else // PSA_WANT_ALG_CCM || PSA_WANT_ALG_GCM

  (void)operation;
  (void)key_buffer;
  (void)input;
  (void)input_length;
  (void)output;
  (void)output_size;
  (void)output_length;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_CCM || PSA_WANT_ALG_GCM
}

psa_status_t sli_se_driver_aead_finish(sli_se_driver_aead_operation_t *operation,
                                       uint8_t *key_buffer,
                                       uint8_t *ciphertext,
                                       size_t ciphertext_size,
                                       size_t *ciphertext_length,
                                       uint8_t *tag,
                                       size_t tag_size,
                                       size_t *tag_length)
{
#if (defined(PSA_WANT_ALG_CCM)) \
  || defined(PSA_WANT_ALG_GCM)

  (void)ciphertext;
  (void)ciphertext_size;
  (void)key_buffer;

  sl_status_t status;
  psa_status_t psa_status;

  sl_se_command_context_t cmd_ctx = { 0 };

  if (operation == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  uint32_t tag_len = PSA_ALG_AEAD_GET_TAG_LENGTH(operation->alg);

  if (tag_size < tag_len) {
    return PSA_ERROR_BUFFER_TOO_SMALL;
  }

  if (ciphertext_length == NULL
      || tag == NULL
      || tag_length == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  *ciphertext_length = 0;

  if (operation->alg == 0) {
    return PSA_ERROR_BAD_STATE;
  }

  // Operation isn't initialised unless we have either AD or PT, so  if we are
  // still at 0, we need to run the start step.
  if (operation->ad_len == 0 && operation->pt_len == 0) {
    psa_status = aead_start(operation, NULL, 0);
    if (psa_status != PSA_SUCCESS) {
      return psa_status;
    }
  }

  psa_algorithm_t alg = PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG(operation->alg);

  status = sl_se_init_command_context(&cmd_ctx);
  if (status != SL_STATUS_OK) {
    return PSA_ERROR_HARDWARE_FAILURE;
  }

  switch (alg) {
#if defined(PSA_WANT_ALG_GCM)
    case PSA_ALG_GCM:
      if (operation->ctx.gcm.mode != SL_SE_ENCRYPT) {
        psa_status = PSA_ERROR_INVALID_ARGUMENT;
        goto exit;
      }
      status = sl_se_gcm_multipart_finish(&operation->ctx.gcm,
                                          &cmd_ctx,
                                          &operation->key_desc,
                                          tag,
                                          tag_len,
                                          ciphertext,
                                          ciphertext_size,
                                          (uint8_t *)ciphertext_length);
      if (status != SL_STATUS_OK) {
        psa_status = PSA_ERROR_HARDWARE_FAILURE;
        goto exit;
      }
      *tag_length = tag_len;
      psa_status = PSA_SUCCESS;
      break;
#endif // PSA_WANT_ALG_GCM
#if defined(PSA_WANT_ALG_CCM)
    case PSA_ALG_CCM:
      if (operation->ctx.ccm.mode != SL_SE_ENCRYPT) {
        psa_status = PSA_ERROR_INVALID_ARGUMENT;
        goto exit;
      }
      status = sl_se_ccm_multipart_finish(&operation->ctx.ccm,
                                          &cmd_ctx,
                                          &operation->key_desc,
                                          tag,
                                          tag_len,
                                          ciphertext,
                                          ciphertext_size,
                                          (uint8_t *)ciphertext_length);

      if (status != SL_STATUS_OK) {
        psa_status = PSA_ERROR_HARDWARE_FAILURE;
        goto exit;
      }
      *tag_length = operation->ctx.ccm.tag_len;
      psa_status = PSA_SUCCESS;
      break;

#endif // PSA_WANT_ALG_CCM
    default:
      (void)tag_size;
      psa_status = PSA_ERROR_NOT_SUPPORTED;
      goto exit;
  }

  exit:

  status = sl_se_deinit_command_context(&cmd_ctx);
  if (status != SL_STATUS_OK) {
    return PSA_ERROR_HARDWARE_FAILURE;
  }

  return psa_status;

#else // PSA_WANT_ALG_CCM || PSA_WANT_ALG_GCM

  (void)operation;
  (void)key_buffer;
  (void)ciphertext;
  (void)ciphertext_size;
  (void)ciphertext_length;
  (void)tag;
  (void)tag_size;
  (void)tag_length;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_CCM || PSA_WANT_ALG_GCM
}

psa_status_t sli_se_driver_aead_verify(sli_se_driver_aead_operation_t *operation,
                                       uint8_t *key_buffer,
                                       uint8_t *plaintext,
                                       size_t plaintext_size,
                                       size_t *plaintext_length,
                                       const uint8_t *tag,
                                       size_t tag_length)
{
#if defined(PSA_WANT_ALG_CCM) \
  || defined(PSA_WANT_ALG_GCM)

  (void)plaintext;
  (void)plaintext_size;
  (void)key_buffer;

  sl_status_t status;
  psa_status_t psa_status;

  sl_se_command_context_t cmd_ctx = { 0 };

  if (operation == NULL || plaintext_length == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  *plaintext_length = 0;

  if (tag == NULL || tag_length == 0 ) {
    return PSA_ERROR_INVALID_SIGNATURE;
  }

  psa_algorithm_t alg = PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG(operation->alg);

  if (operation->alg == 0) {
    return PSA_ERROR_BAD_STATE;
  }

  // Operation isn't initialised unless we have either AD or PT, so  if we are
  // still at 0, we need to run the start step.
  if (operation->ad_len == 0 && operation->pt_len == 0) {
    psa_status = aead_start(operation, NULL, 0);
    if (psa_status != PSA_SUCCESS) {
      return psa_status;
    }
  }

  status = sl_se_init_command_context(&cmd_ctx);
  if (status != SL_STATUS_OK) {
    return PSA_ERROR_HARDWARE_FAILURE;
  }

  switch (alg) {
#if defined(PSA_WANT_ALG_GCM)
    case PSA_ALG_GCM:
      if (operation->ctx.gcm.mode != SL_SE_DECRYPT) {
        psa_status = PSA_ERROR_INVALID_ARGUMENT;
        goto exit;
      }
      status = sl_se_gcm_multipart_finish(&operation->ctx.gcm,
                                          &cmd_ctx,
                                          &operation->key_desc,
                                          (uint8_t *)tag,
                                          tag_length,
                                          plaintext,
                                          plaintext_size,
                                          (uint8_t *)plaintext_length);
      if (status == SL_STATUS_INVALID_SIGNATURE) {
        psa_status = PSA_ERROR_INVALID_SIGNATURE;
        goto exit;
      } else if (status != SL_STATUS_OK) {
        psa_status = PSA_ERROR_HARDWARE_FAILURE;
        goto exit;
      }
      psa_status = PSA_SUCCESS;
      break;
#endif // PSA_WANT_ALG_GCM
#if defined(PSA_WANT_ALG_CCM)
    case PSA_ALG_CCM:
    {
      uint32_t tag_len = PSA_ALG_AEAD_GET_TAG_LENGTH(operation->alg);
      if (tag_length != tag_len) {
        psa_status = PSA_ERROR_INVALID_SIGNATURE;
        goto exit;
      }
      if (operation->ctx.ccm.mode != SL_SE_DECRYPT) {
        psa_status = PSA_ERROR_INVALID_ARGUMENT;
        goto exit;
      }
      status = sl_se_ccm_multipart_finish(&operation->ctx.ccm,
                                          &cmd_ctx,
                                          &operation->key_desc,
                                          (uint8_t *)tag,
                                          tag_length,
                                          plaintext,
                                          plaintext_size,
                                          (uint8_t *)plaintext_length);

      if (status == SL_STATUS_INVALID_SIGNATURE) {
        psa_status = PSA_ERROR_INVALID_SIGNATURE;
        goto exit;
      } else if (status != SL_STATUS_OK) {
        psa_status = PSA_ERROR_HARDWARE_FAILURE;
        goto exit;
      }
      psa_status = PSA_SUCCESS;
      break;
    }
#endif //PSA_WANT_ALG_CCM
    default:
      psa_status = PSA_ERROR_NOT_SUPPORTED;
      goto exit;
  }

  exit:

  status = sl_se_deinit_command_context(&cmd_ctx);
  if (status != SL_STATUS_OK) {
    return PSA_ERROR_HARDWARE_FAILURE;
  }

  return psa_status;

#else // PSA_WANT_ALG_CCM || PSA_WANT_ALG_GCM

  (void)operation;
  (void)key_buffer;
  (void)plaintext;
  (void)plaintext_size;
  (void)plaintext_length;
  (void)tag;
  (void)tag_length;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_CCM || PSA_WANT_ALG_GCM
}

#endif // defined(SEMAILBOX_PRESENT)
