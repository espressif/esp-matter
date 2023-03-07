/***************************************************************************//**
 * @file
 * @brief Silicon Labs PSA Crypto Opaque Driver Cipher functions.
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
#include "psa/crypto_platform.h"

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT) \
  || defined(MBEDTLS_PSA_CRYPTO_BUILTIN_KEYS)

#include "psa/crypto.h"
#include "sli_se_opaque_types.h"
#include "sli_se_opaque_functions.h"

#include "sl_se_manager.h"
#include "sl_se_manager_cipher.h"

#include "sli_se_driver_cipher.h"
#include "sli_se_driver_key_management.h"

#include <string.h>

#if (defined(PSA_WANT_KEY_TYPE_AES)        \
  && (defined(PSA_WANT_ALG_ECB_NO_PADDING) \
  || defined(PSA_WANT_ALG_CTR)             \
  || defined(PSA_WANT_ALG_CFB)             \
  || defined(PSA_WANT_ALG_OFB)             \
  || defined(PSA_WANT_ALG_CBC_NO_PADDING)  \
  || defined(PSA_WANT_ALG_CBC_PKCS7)))

static void update_key_from_context(sli_se_opaque_cipher_operation_t* ctx)
{
  // Point the key to the buffer
  ctx->operation.key_desc.storage.location.buffer.pointer = ctx->key;
}

static psa_status_t initialize_key_in_context(const psa_key_attributes_t *attributes,
                                              sli_se_opaque_cipher_operation_t *operation,
                                              const uint8_t *key_buffer,
                                              size_t key_buffer_size)
{
  // Double check that the location of the key actually is
  // as expected for this driver.
  if (PSA_KEY_LIFETIME_GET_LOCATION(psa_get_key_lifetime(attributes))
      != PSA_KEY_LOCATION_SLI_SE_OPAQUE) {
    return PSA_ERROR_NOT_SUPPORTED;
  }

  // Initialize the key descriptor.
  psa_status_t psa_status = sli_se_key_desc_from_input(attributes,
                                                       key_buffer,
                                                       key_buffer_size,
                                                       &operation->operation.key_desc);
  if (psa_status != PSA_SUCCESS) {
    return psa_status;
  }

  // Copy the key material -- could be either a built-in or a wrapped key.
  sli_se_opaque_key_context_header_t *key_context_header =
    (sli_se_opaque_key_context_header_t *)key_buffer;
  if (key_context_header->builtin_key_id != 0) { // Built-in key.
    memcpy(operation->key,
           key_buffer,
           sizeof(sli_se_opaque_key_context_header_t));
    operation->key_len = sizeof(sli_se_opaque_key_context_header_t);
  } else { // Wrapped key.
    size_t key_size = PSA_BITS_TO_BYTES(psa_get_key_bits(attributes));
    size_t offset = offsetof(sli_se_opaque_wrapped_key_context_t, wrapped_buffer);
    if (key_buffer_size < key_size + sizeof(sli_se_opaque_wrapped_key_context_t)) {
      return PSA_ERROR_INVALID_ARGUMENT;
    }
    if (sizeof(operation->key) < key_size + SLI_SE_WRAPPED_KEY_OVERHEAD) {
      return PSA_ERROR_INVALID_ARGUMENT;
    }
    memcpy(operation->key,
           key_buffer + offset,
           key_size + SLI_SE_WRAPPED_KEY_OVERHEAD);
    operation->key_len = key_size + SLI_SE_WRAPPED_KEY_OVERHEAD;
  }

  return PSA_SUCCESS;
}

#endif // PSA_WANT_KEY_TYPE_AES && PSA_WANT_ALG_*

psa_status_t sli_se_opaque_cipher_encrypt(const psa_key_attributes_t *attributes,
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
  || defined(PSA_WANT_ALG_CBC_NO_PADDING)  \
  || defined(PSA_WANT_ALG_CBC_PKCS7)))     \
  || (defined(PSA_WANT_KEY_TYPE_CHACHA20)  \
  && defined(PSA_WANT_ALG_STREAM_CIPHER))

  return sli_se_driver_cipher_encrypt(attributes,
                                      key_buffer,
                                      key_buffer_size,
                                      alg,
                                      iv,
                                      iv_length,
                                      input,
                                      input_length,
                                      output,
                                      output_size,
                                      output_length);

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

psa_status_t sli_se_opaque_cipher_decrypt(const psa_key_attributes_t *attributes,
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
  || defined(PSA_WANT_ALG_CBC_NO_PADDING)  \
  || defined(PSA_WANT_ALG_CBC_PKCS7)))     \
  || (defined(PSA_WANT_KEY_TYPE_CHACHA20)  \
  && defined(PSA_WANT_ALG_STREAM_CIPHER))

  return sli_se_driver_cipher_decrypt(attributes,
                                      key_buffer,
                                      key_buffer_size,
                                      alg,
                                      input,
                                      input_length,
                                      output,
                                      output_size,
                                      output_length);

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

psa_status_t sli_se_opaque_cipher_encrypt_setup(sli_se_opaque_cipher_operation_t *operation,
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
  || defined(PSA_WANT_ALG_CBC_NO_PADDING)  \
  || defined(PSA_WANT_ALG_CBC_PKCS7)))

  if (operation == NULL || attributes == NULL || key_buffer == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }
  // Reset context
  memset(operation, 0, sizeof(*operation));

  psa_status_t psa_status = sli_se_driver_cipher_encrypt_setup(&operation->operation,
                                                               attributes,
                                                               alg);
  if (psa_status != PSA_SUCCESS) {
    return psa_status;
  }

  // Copy key into context
  psa_status = initialize_key_in_context(attributes,
                                         operation,
                                         key_buffer,
                                         key_buffer_size);
  return psa_status;

#else // PSA_WANT_ALG_* && PSA_WANT_KEY_TYPE_AES

  (void)operation;
  (void)attributes;
  (void)key_buffer;
  (void)key_buffer_size;
  (void)alg;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_* && PSA_WANT_KEY_TYPE_AES
}

psa_status_t sli_se_opaque_cipher_decrypt_setup(sli_se_opaque_cipher_operation_t *operation,
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
  || defined(PSA_WANT_ALG_CBC_NO_PADDING)  \
  || defined(PSA_WANT_ALG_CBC_PKCS7)))

  if (operation == NULL || attributes == NULL || key_buffer == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // Reset context
  memset(operation, 0, sizeof(*operation));

  psa_status_t psa_status = sli_se_driver_cipher_decrypt_setup(&operation->operation,
                                                               attributes,
                                                               alg);
  if (psa_status != PSA_SUCCESS) {
    return psa_status;
  }

  // Copy key into context
  psa_status = initialize_key_in_context(attributes,
                                         operation,
                                         key_buffer,
                                         key_buffer_size);
  return psa_status;

#else // PSA_WANT_ALG_* && PSA_WANT_KEY_TYPE_AES

  (void)operation;
  (void)attributes;
  (void)key_buffer;
  (void)key_buffer_size;
  (void)alg;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_* && PSA_WANT_KEY_TYPE_AES
}

psa_status_t sli_se_opaque_cipher_set_iv(sli_se_opaque_cipher_operation_t *operation,
                                         const uint8_t *iv,
                                         size_t iv_length)
{
#if (defined(PSA_WANT_KEY_TYPE_AES)        \
  && (defined(PSA_WANT_ALG_ECB_NO_PADDING) \
  || defined(PSA_WANT_ALG_CTR)             \
  || defined(PSA_WANT_ALG_CFB)             \
  || defined(PSA_WANT_ALG_OFB)             \
  || defined(PSA_WANT_ALG_CBC_NO_PADDING)  \
  || defined(PSA_WANT_ALG_CBC_PKCS7)))

  if (operation == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  if (operation->key_len == 0) {
    // context hasn't been properly initialised
    return PSA_ERROR_BAD_STATE;
  }

  return sli_se_driver_cipher_set_iv(&operation->operation, iv, iv_length);

#else // PSA_WANT_ALG_* && PSA_WANT_KEY_TYPE_AES

  (void)operation;
  (void)iv;
  (void)iv_length;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_* && PSA_WANT_KEY_TYPE_AES
}

psa_status_t sli_se_opaque_cipher_update(sli_se_opaque_cipher_operation_t *operation,
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
  || defined(PSA_WANT_ALG_CBC_NO_PADDING)  \
  || defined(PSA_WANT_ALG_CBC_PKCS7)))

  // Argument check
  if (operation == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // For wrapped keys, set the key correctly
  sli_se_opaque_key_context_header_t *key_context_header =
    (sli_se_opaque_key_context_header_t *)operation->key;
  if (key_context_header->builtin_key_id == 0) {
    update_key_from_context(operation);
  }

  // Compute
  return sli_se_driver_cipher_update(&operation->operation,
                                     input,
                                     input_length,
                                     output,
                                     output_size,
                                     output_length);

#else // PSA_WANT_ALG_* && PSA_WANT_KEY_TYPE_AES

  (void)operation;
  (void)input;
  (void)input_length;
  (void)output;
  (void)output_size;
  (void)output_length;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_* && PSA_WANT_KEY_TYPE_AES
}

psa_status_t sli_se_opaque_cipher_finish(sli_se_opaque_cipher_operation_t *operation,
                                         uint8_t *output,
                                         size_t output_size,
                                         size_t *output_length)
{
#if (defined(PSA_WANT_KEY_TYPE_AES)        \
  && (defined(PSA_WANT_ALG_ECB_NO_PADDING) \
  || defined(PSA_WANT_ALG_CTR)             \
  || defined(PSA_WANT_ALG_CFB)             \
  || defined(PSA_WANT_ALG_OFB)             \
  || defined(PSA_WANT_ALG_CBC_NO_PADDING)  \
  || defined(PSA_WANT_ALG_CBC_PKCS7)))

  if (operation == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // For wrapped keys, set the key correctly
  sli_se_opaque_key_context_header_t *key_context_header =
    (sli_se_opaque_key_context_header_t *)operation->key;
  if (key_context_header->builtin_key_id == 0) {
    update_key_from_context(operation);
  }

  // Compute
  return sli_se_driver_cipher_finish(&operation->operation,
                                     output,
                                     output_size,
                                     output_length);

#else // PSA_WANT_ALG_* && PSA_WANT_KEY_TYPE_AES

  (void)operation;
  (void)output;
  (void)output_size;
  (void)output_length;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_* && PSA_WANT_KEY_TYPE_AES
}

psa_status_t sli_se_opaque_cipher_abort(sli_se_opaque_cipher_operation_t *operation)
{
#if (defined(PSA_WANT_KEY_TYPE_AES)        \
  && (defined(PSA_WANT_ALG_ECB_NO_PADDING) \
  || defined(PSA_WANT_ALG_CTR)             \
  || defined(PSA_WANT_ALG_CFB)             \
  || defined(PSA_WANT_ALG_OFB)             \
  || defined(PSA_WANT_ALG_CBC_NO_PADDING)  \
  || defined(PSA_WANT_ALG_CBC_PKCS7)))

  if (operation != NULL) {
    // Wipe context
    memset(operation, 0, sizeof(sli_se_opaque_cipher_operation_t));
  }

  return PSA_SUCCESS;

#else // PSA_WANT_ALG_* && PSA_WANT_KEY_TYPE_AES

  (void)operation;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_* && PSA_WANT_KEY_TYPE_AES
}

#endif // VAULT || MBEDTLS_PSA_CRYPTO_BUILTIN_KEYS
#endif // defined(SEMAILBOX_PRESENT)
