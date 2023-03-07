/***************************************************************************//**
 * @file
 * @brief Silicon Labs PSA Crypto Transparent Driver Mac functions.
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
#include "sli_se_transparent_types.h"
#include "sli_se_transparent_functions.h"
#include "sli_psa_driver_common.h"

#include <string.h>

//------------------------------------------------------------------------------
// Static asserts

// Make sure that the two locations of 'alg' are in the same place, since we
// access them interchangeably.
#if defined(PSA_WANT_ALG_HMAC)
_Static_assert(offsetof(sli_se_transparent_mac_operation_t, hmac.alg)
               == offsetof(sli_se_transparent_mac_operation_t,
                           cipher_mac.operation.alg),
               "hmac.alg and cipher_mac.oepration.alg are not aliases");
#endif

//------------------------------------------------------------------------------
// Static functions

#if defined(PSA_WANT_ALG_CMAC) || defined(PSA_WANT_ALG_CBC_MAC)
static psa_status_t sli_se_transparent_driver_symmetric_key_from_context(
  sl_se_key_descriptor_t* key_desc,
  sli_se_transparent_mac_operation_t* operation)
{
  // Point to transparent key buffer as storage location
  key_desc->storage.method = SL_SE_KEY_STORAGE_EXTERNAL_PLAINTEXT;
  key_desc->storage.location.buffer.pointer = operation->cipher_mac.key;
  key_desc->storage.location.buffer.size = sizeof(operation->cipher_mac.key);
  key_desc->size = operation->cipher_mac.key_len;

  switch (PSA_ALG_FULL_LENGTH_MAC(operation->cipher_mac.operation.alg)) {
    case PSA_ALG_CBC_MAC:
    case PSA_ALG_CMAC:
      if (key_desc->size == 16) {
        key_desc->type = SL_SE_KEY_TYPE_AES_128;
      } else if (key_desc->size == 24) {
        key_desc->type = SL_SE_KEY_TYPE_AES_192;
      } else if (key_desc->size == 32) {
        key_desc->type = SL_SE_KEY_TYPE_AES_256;
      } else {
        return PSA_ERROR_BAD_STATE;
      }
      break;
    default:
      return PSA_ERROR_BAD_STATE;
  }

  return PSA_SUCCESS;
}
#endif // PSA_WANT_ALG_CMAC || PSA_WANT_ALG_CBC_MAC

#if defined(PSA_WANT_ALG_CMAC)     \
  || defined(PSA_WANT_ALG_CBC_MAC) \
  || defined(PSA_WANT_ALG_HMAC)
static psa_status_t sli_se_transparent_driver_symmetric_key_from_psa(
  sl_se_key_descriptor_t* key_desc,
  const psa_key_attributes_t *attributes,
  const uint8_t *key_buffer,
  size_t key_buffer_size)
{
  // Point to transparent key buffer as storage location
  key_desc->storage.method = SL_SE_KEY_STORAGE_EXTERNAL_PLAINTEXT;
  key_desc->storage.location.buffer.pointer = (uint8_t *)key_buffer;
  key_desc->storage.location.buffer.size = key_buffer_size;

  // Verify and set key attributes
  psa_key_type_t keytype = psa_get_key_type(attributes);

  switch (keytype) {
    #if defined(PSA_WANT_ALG_CMAC) || defined(PSA_WANT_ALG_CBC_MAC)
    case PSA_KEY_TYPE_AES: {
      switch (psa_get_key_bits(attributes)) {
        case 128:
          key_desc->size = 16;
          key_desc->type = SL_SE_KEY_TYPE_AES_128;
          break;
        case 192:
          key_desc->size = 24;
          key_desc->type = SL_SE_KEY_TYPE_AES_192;
          break;
        case 256:
          key_desc->size = 32;
          key_desc->type = SL_SE_KEY_TYPE_AES_256;
          break;
        default:
          return PSA_ERROR_NOT_SUPPORTED;
      }
      break;
    }
    #endif // PSA_WANT_ALG_CMAC || PSA_WANT_ALG_CBC_MAC
    #if defined(PSA_WANT_ALG_HMAC)
    case PSA_KEY_TYPE_HMAC: {
      key_desc->size = psa_get_key_bits(attributes) / 8;
      key_desc->type = SL_SE_KEY_TYPE_SYMMETRIC;
      break;
    }
    #endif // PSA_WANT_ALG_HMAC
    default:
      return PSA_ERROR_INVALID_ARGUMENT;
      break;
  }

  if (key_buffer_size < key_desc->size) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  return PSA_SUCCESS;
}
#endif // PSA_WANT_ALG_CMAC || PSA_WANT_ALG_CBC_MAC || PSA_WANT_ALG_HMAC

//------------------------------------------------------------------------------
// Driver entry points

psa_status_t sli_se_transparent_mac_compute(
  const psa_key_attributes_t *attributes,
  const uint8_t *key_buffer,
  size_t key_buffer_size,
  psa_algorithm_t alg,
  const uint8_t *input,
  size_t input_length,
  uint8_t *mac,
  size_t mac_size,
  size_t *mac_length)
{
#if defined(PSA_WANT_ALG_CMAC)     \
  || defined(PSA_WANT_ALG_CBC_MAC) \
  || defined(PSA_WANT_ALG_HMAC)

  if (key_buffer == NULL
      || attributes == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // Ephemeral contexts
  sl_se_key_descriptor_t key_desc = { 0 };
  psa_status_t psa_status
    = sli_se_transparent_driver_symmetric_key_from_psa(&key_desc,
                                                       attributes,
                                                       key_buffer,
                                                       key_buffer_size);
  if (psa_status != PSA_SUCCESS) {
    return psa_status;
  }

  return sli_se_driver_mac_compute(&key_desc,
                                   alg,
                                   input,
                                   input_length,
                                   mac,
                                   mac_size,
                                   mac_length);

#else // PSA_WANT_ALG_CMAC || PSA_WANT_ALG_CBC_MAC || PSA_WANT_ALG_HMAC

  (void)attributes;
  (void)key_buffer;
  (void)key_buffer_size;
  (void)alg;
  (void)input;
  (void)input_length;
  (void)mac;
  (void)mac_size;
  (void)mac_length;

  return PSA_ERROR_NOT_SUPPORTED;
#endif
}

psa_status_t sli_se_transparent_mac_sign_setup(
  sli_se_transparent_mac_operation_t *operation,
  const psa_key_attributes_t *attributes,
  const uint8_t *key_buffer,
  size_t key_buffer_size,
  psa_algorithm_t alg)
{
#if defined(PSA_WANT_ALG_CMAC)     \
  || defined(PSA_WANT_ALG_CBC_MAC) \
  || defined(PSA_WANT_ALG_HMAC)

  if (operation == NULL
      || attributes == NULL
      || (key_buffer == NULL && key_buffer_size > 0)) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  psa_status_t status;

  // start by resetting context
  memset(operation, 0, sizeof(*operation));

  #if defined(PSA_WANT_ALG_HMAC)
  if (PSA_ALG_IS_HMAC(alg)) {
    // SE does not support multipart HMAC. Construct it from hashing instead.
    // Check key type and output size
    if (psa_get_key_type(attributes) != PSA_KEY_TYPE_HMAC) {
      // For HMAC, key type is strictly enforced
      return PSA_ERROR_INVALID_ARGUMENT;
    }

    psa_algorithm_t hash_alg = PSA_ALG_HMAC_GET_HASH(alg);
    size_t digest_len = PSA_HASH_LENGTH(hash_alg);

    if (PSA_MAC_TRUNCATED_LENGTH(alg) > digest_len) {
      return PSA_ERROR_INVALID_ARGUMENT;
    }

    // Setup the hash accumulator first, such that we can return early for non-
    // supported hash functions and avoid potentially overflowing buffer lengths.
    status = sli_se_transparent_hash_setup(&operation->hmac.hash_ctx,
                                           hash_alg);
    if (status != PSA_SUCCESS) {
      return status;
    }

    size_t keylen = psa_get_key_bits(attributes) / 8;
    size_t blocklen
      = (hash_alg == PSA_ALG_SHA_384 || hash_alg == PSA_ALG_SHA_512) ? 128 : 64;

    if (key_buffer_size < keylen) {
      return PSA_ERROR_INVALID_ARGUMENT;
    }

    // Reduce the key if larger than a block
    if (keylen > blocklen) {
      status = sli_se_transparent_hash_compute(
        hash_alg,
        key_buffer,
        keylen,
        operation->hmac.opad,
        sizeof(operation->hmac.opad),
        &keylen);
      if (status != PSA_SUCCESS) {
        return status;
      }
    } else if (keylen > 0) {
      memcpy(operation->hmac.opad, key_buffer, keylen);
    }

    // Calculate inner padding in opad buffer and start a multipart hash with it
    for (size_t i = 0; i < keylen; i++) {
      operation->hmac.opad[i] ^= 0x36;
    }
    memset(&operation->hmac.opad[keylen], 0x36, blocklen - keylen);

    status = sli_se_transparent_hash_update(
      &operation->hmac.hash_ctx,
      operation->hmac.opad, blocklen);
    if (status != PSA_SUCCESS) {
      return status;
    }

    // Calculate outer padding and store it for finalisation
    for (size_t i = 0; i < blocklen; i++) {
      operation->hmac.opad[i] ^= 0x36 ^ 0x5C;
    }

    operation->hmac.alg = alg;
    return PSA_SUCCESS;
  }
  #endif // PSA_WANT_ALG_HMAC

  #if defined(PSA_WANT_ALG_CMAC) || defined(PSA_WANT_ALG_CBC_MAC)
  status = sli_se_driver_mac_sign_setup(&(operation->cipher_mac.operation),
                                        attributes,
                                        alg);
  if (status != PSA_SUCCESS) {
    return status;
  }

  operation->cipher_mac.key_len = psa_get_key_bits(attributes) / 8;
  switch (operation->cipher_mac.key_len) {
    case 16:
      if (key_buffer_size < 16) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }
      memcpy(operation->cipher_mac.key, key_buffer, 16);
      break;
    case 24:
      if (key_buffer_size < 24) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }
      memcpy(operation->cipher_mac.key, key_buffer, 24);
      break;
    case 32:
      if (key_buffer_size < 32) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }
      memcpy(operation->cipher_mac.key, key_buffer, 32);
      break;
    default:
      return PSA_ERROR_INVALID_ARGUMENT;
  }

  return PSA_SUCCESS;
  #else // PSA_WANT_ALG_CMAC || PSA_WANT_ALG_CBC_MAC
  return PSA_ERROR_NOT_SUPPORTED;
  #endif // PSA_WANT_ALG_CMAC || PSA_WANT_ALG_CBC_MAC

#else // PSA_WANT_ALG_CMAC || PSA_WANT_ALG_CBC_MAC || PSA_WANT_ALG_HMAC

  (void)operation;
  (void)attributes;
  (void)key_buffer;
  (void)key_buffer_size;
  (void)alg;

  return PSA_ERROR_NOT_SUPPORTED;
#endif
}

psa_status_t sli_se_transparent_mac_verify_setup(
  sli_se_transparent_mac_operation_t *operation,
  const psa_key_attributes_t *attributes,
  const uint8_t *key_buffer,
  size_t key_buffer_size,
  psa_algorithm_t alg)
{
  // Since the PSA Crypto core exposes the verify functionality of the drivers
  // without actually implementing the fallback to 'sign' when the driver
  // doesn't support verify, we need to do this ourselves for the time being.
  return sli_se_transparent_mac_sign_setup(operation,
                                           attributes,
                                           key_buffer,
                                           key_buffer_size,
                                           alg);
}

psa_status_t sli_se_transparent_mac_update(
  sli_se_transparent_mac_operation_t *operation,
  const uint8_t *input,
  size_t input_length)
{
#if defined(PSA_WANT_ALG_CMAC)     \
  || defined(PSA_WANT_ALG_CBC_MAC) \
  || defined(PSA_WANT_ALG_HMAC)

  if (operation == NULL
      || (input == NULL && input_length > 0)) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  #if defined(PSA_WANT_ALG_HMAC)
  if (PSA_ALG_IS_HMAC(operation->hmac.alg)) {
    return sli_se_transparent_hash_update(
      &operation->hmac.hash_ctx,
      input,
      input_length);
  }
  #endif

  #if defined(PSA_WANT_ALG_CMAC) || defined(PSA_WANT_ALG_CBC_MAC)
  // Ephemeral contexts
  sl_se_key_descriptor_t key_desc = { 0 };

  psa_status_t psa_status
    = sli_se_transparent_driver_symmetric_key_from_context(&key_desc,
                                                           operation);
  if (psa_status != PSA_SUCCESS) {
    return psa_status;
  }

  return sli_se_driver_mac_update(&(operation->cipher_mac.operation),
                                  &key_desc,
                                  input,
                                  input_length);
  #else // PSA_WANT_ALG_CMAC || PSA_WANT_ALG_CBC_MAC
  return PSA_ERROR_NOT_SUPPORTED;
  #endif // PSA_WANT_ALG_CMAC || PSA_WANT_ALG_CBC_MAC

#else // PSA_WANT_ALG_CMAC || PSA_WANT_ALG_CBC_MAC || PSA_WANT_ALG_HMAC

  (void)operation;
  (void)input;
  (void)input_length;

  return PSA_ERROR_NOT_SUPPORTED;
#endif
}

psa_status_t sli_se_transparent_mac_sign_finish(
  sli_se_transparent_mac_operation_t *operation,
  uint8_t *mac,
  size_t mac_size,
  size_t *mac_length)
{
#if defined(PSA_WANT_ALG_CMAC)     \
  || defined(PSA_WANT_ALG_CBC_MAC) \
  || defined(PSA_WANT_ALG_HMAC)

  if (operation == NULL
      || mac == NULL
      || mac_size == 0
      || mac_length == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  #if defined(PSA_WANT_ALG_HMAC)
  if (PSA_ALG_IS_HMAC(operation->hmac.alg)) {
    uint8_t buffer[sizeof(operation->hmac.opad)
                   + (sizeof(operation->hmac.opad) / 2)];
    size_t olen = 0;
    psa_algorithm_t hash_alg = PSA_ALG_HMAC_GET_HASH(operation->hmac.alg);

    #if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_SE)
    if (hash_alg == PSA_ALG_SHA_384 || hash_alg == PSA_ALG_SHA_512) {
      // Could only reach here if the programmer has made some errors. Take the
      // safe approach of checking just in case, in order to avoid certain
      // buffer overflows.
      return PSA_ERROR_BAD_STATE;
    }
    size_t blocklen = 64;
    #else // (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_SE)
    size_t blocklen
      = (hash_alg == PSA_ALG_SHA_384 || hash_alg == PSA_ALG_SHA_512) ? 128 : 64;
    #endif // (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_SE)

    // Construct outer hash input from opad and hash result
    memcpy(buffer, operation->hmac.opad, blocklen);
    memset(operation->hmac.opad, 0, sizeof(operation->hmac.opad));

    psa_status_t status = sli_se_transparent_hash_finish(
      &operation->hmac.hash_ctx,
      &buffer[blocklen],
      sizeof(buffer) - blocklen,
      &olen);
    if (status != PSA_SUCCESS) {
      return status;
    }

    // Calculate HMAC
    status = sli_se_transparent_hash_compute(
      hash_alg,
      buffer,
      blocklen + olen,
      buffer,
      sizeof(buffer),
      &olen);
    if (status != PSA_SUCCESS) {
      return status;
    }

    // Copy out a potentially truncated HMAC
    size_t requested_length = PSA_MAC_TRUNCATED_LENGTH(operation->hmac.alg);
    if (requested_length == 0) {
      requested_length = olen;
    }

    if (requested_length > mac_size) {
      memset(buffer, 0, sizeof(buffer));
      return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    memcpy(mac, buffer, requested_length);
    *mac_length = requested_length;
    memset(buffer, 0, sizeof(buffer));
    return PSA_SUCCESS;
  }
  #endif // PSA_WANT_ALG_HMAC

  #if defined(PSA_WANT_ALG_CMAC) || defined(PSA_WANT_ALG_CBC_MAC)
  // Ephemeral contexts
  sl_se_key_descriptor_t key_desc = { 0 };

  psa_status_t status = sli_se_transparent_driver_symmetric_key_from_context(
    &key_desc,
    operation);
  if (status != PSA_SUCCESS) {
    return status;
  }

  return sli_se_driver_mac_sign_finish(&(operation->cipher_mac.operation),
                                       &key_desc,
                                       mac,
                                       mac_size,
                                       mac_length);
  #else // PSA_WANT_ALG_CMAC || PSA_WANT_ALG_CBC_MAC
  return PSA_ERROR_NOT_SUPPORTED;
  #endif // PSA_WANT_ALG_CMAC || PSA_WANT_ALG_CBC_MAC

#else // PSA_WANT_ALG_CMAC || PSA_WANT_ALG_CBC_MAC || PSA_WANT_ALG_HMAC

  (void)operation;
  (void)mac;
  (void)mac_size;
  (void)mac_length;

  return PSA_ERROR_NOT_SUPPORTED;
#endif
}

psa_status_t sli_se_transparent_mac_verify_finish(
  sli_se_transparent_mac_operation_t *operation,
  const uint8_t *mac,
  size_t mac_length)
{
#if defined(PSA_WANT_ALG_CMAC)     \
  || defined(PSA_WANT_ALG_CBC_MAC) \
  || defined(PSA_WANT_ALG_HMAC)

  // Since the PSA Crypto core exposes the verify functionality of the drivers
  // without actually implementing the fallback to 'sign' when the driver
  // doesn't support verify, we need to do this ourselves for the time being.
  uint8_t calculated_mac[PSA_MAC_MAX_SIZE] = { 0 };
  size_t calculated_length = PSA_MAC_MAX_SIZE;

  psa_status_t status = sli_se_transparent_mac_sign_finish(
    operation,
    calculated_mac, sizeof(calculated_mac), &calculated_length);
  if (status != PSA_SUCCESS) {
    return status;
  }

  if (mac_length > sizeof(calculated_mac)) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  if (sli_psa_safer_memcmp(mac, calculated_mac, mac_length) != 0) {
    status = PSA_ERROR_INVALID_SIGNATURE;
  } else {
    status = PSA_SUCCESS;
  }

  memset(calculated_mac, 0, sizeof(calculated_mac));
  return status;

#else // PSA_WANT_ALG_CMAC || PSA_WANT_ALG_CBC_MAC || PSA_WANT_ALG_HMAC

  (void)operation;
  (void)mac;
  (void)mac_length;

  return PSA_ERROR_NOT_SUPPORTED;
#endif
}

psa_status_t sli_se_transparent_mac_abort(
  sli_se_transparent_mac_operation_t *operation)
{
#if defined(PSA_WANT_ALG_CMAC)     \
  || defined(PSA_WANT_ALG_CBC_MAC) \
  || defined(PSA_WANT_ALG_HMAC)

  // There's no state in hardware that we need to preserve, so zeroing out the
  // context suffices.
  if (operation == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  memset(operation, 0, sizeof(*operation));

  return PSA_SUCCESS;

#else // PSA_WANT_ALG_CMAC || PSA_WANT_ALG_CBC_MAC || PSA_WANT_ALG_HMAC

  (void)operation;

  return PSA_ERROR_NOT_SUPPORTED;
#endif
}

#endif // defined(SEMAILBOX_PRESENT)
