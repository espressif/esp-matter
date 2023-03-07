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

#if defined(CRYPTOACC_PRESENT)

#include "sli_cryptoacc_transparent_functions.h"
#include "sli_psa_driver_common.h"
#include "cryptoacc_management.h"
// Replace inclusion of psa/crypto_xxx.h with the new psa driver commong
// interface header file when it becomes available.
#include "psa/crypto_platform.h"
#include "psa/crypto_sizes.h"
#include "psa/crypto_struct.h"
#include "sx_aes.h"
#include "sx_hash.h"
#include "sx_errors.h"
#include <string.h>

#if defined(PSA_WANT_ALG_HMAC)
static psa_status_t sli_cryptoacc_hmac_validate_key(
  const psa_key_attributes_t *attributes,
  psa_algorithm_t alg,
  sx_hash_fct_t *hash_fct,
  size_t *digest_length)
{
  // Check key type and output size
  if (psa_get_key_type(attributes) != PSA_KEY_TYPE_HMAC) {
    // For HMAC, key type is strictly enforced
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  switch (PSA_ALG_HMAC_GET_HASH(alg)) {
    case PSA_ALG_SHA_1:
      *hash_fct = e_SHA1;
      *digest_length = 20;
      break;
    case PSA_ALG_SHA_224:
      *hash_fct = e_SHA224;
      *digest_length = 28;
      break;
    case PSA_ALG_SHA_256:
      *hash_fct = e_SHA256;
      *digest_length = 32;
      break;
    default:
      return PSA_ERROR_NOT_SUPPORTED;
  }

  return PSA_SUCCESS;
}
#endif // PSA_WANT_ALG_HMAC

#if defined(PSA_WANT_ALG_CMAC)
static psa_status_t sli_cryptoacc_cmac_validate_key(
  const psa_key_attributes_t *attributes)
{
  // Check key type and size
  if (psa_get_key_type(attributes) != PSA_KEY_TYPE_AES) {
    // CMAC could be used with DES
    return PSA_ERROR_NOT_SUPPORTED;
  }

  switch (psa_get_key_bits(attributes) / 8) {
    case 16:
    case 24:
    case 32:
      break;
    default:
      // There's no other AES algorithm in existence
      return PSA_ERROR_INVALID_ARGUMENT;
  }

  return PSA_SUCCESS;
}
#endif // PSA_WANT_ALG_CMAC

psa_status_t sli_cryptoacc_transparent_mac_compute(const psa_key_attributes_t *attributes,
                                                   const uint8_t *key_buffer,
                                                   size_t key_buffer_size,
                                                   psa_algorithm_t alg,
                                                   const uint8_t *input,
                                                   size_t input_length,
                                                   uint8_t *mac,
                                                   size_t mac_size,
                                                   size_t *mac_length)
{
#if defined(PSA_WANT_ALG_HMAC) || defined(PSA_WANT_ALG_CMAC)

  if (key_buffer == NULL
      || attributes == NULL
      || mac == NULL
      || mac_length == NULL
      || ((input == NULL) && (input_length > 0))) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  psa_status_t status;
  uint32_t sx_ret;
  size_t key_size = psa_get_key_bits(attributes) / 8;

#if defined(PSA_WANT_ALG_HMAC)
  if (PSA_ALG_IS_HMAC(alg)) {
    sx_hash_fct_t sx_hash_alg;
    size_t digest_length;

    status = sli_cryptoacc_hmac_validate_key(attributes, alg, &sx_hash_alg, &digest_length);
    if (status != PSA_SUCCESS) {
      return status;
    }

    if ((PSA_MAC_TRUNCATED_LENGTH(alg) > 0)
        && (PSA_MAC_TRUNCATED_LENGTH(alg) < digest_length)) {
      digest_length = PSA_MAC_TRUNCATED_LENGTH(alg);
    }

    if (mac_size < digest_length) {
      return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    // Acquire exclusive access to the CRYPTOACC hardware
    status = cryptoacc_management_acquire();
    if (status != PSA_SUCCESS) {
      return status;
    }

    // Execute the HMAC operation
    sx_ret = sx_hmac_blk(sx_hash_alg,
                         block_t_convert(key_buffer, key_size),
                         block_t_convert(input, input_length),
                         block_t_convert(mac, digest_length));

    status = cryptoacc_management_release();

    if (sx_ret != CRYPTOLIB_SUCCESS) {
      return PSA_ERROR_HARDWARE_FAILURE;
    }
    if (status != PSA_SUCCESS) {
      return status;
    }

    // Report generated hmac length
    *mac_length = digest_length;
    return PSA_SUCCESS;
  }
#endif // PSA_WANT_ALG_HMAC

  // If not HMAC, continue with the regular MAC algos
  switch (PSA_ALG_FULL_LENGTH_MAC(alg)) {
#if defined(PSA_WANT_ALG_CMAC)
    case PSA_ALG_CMAC:
    {
      status = sli_cryptoacc_cmac_validate_key(attributes);
      if (status != PSA_SUCCESS) {
        return status;
      }

      if (key_buffer_size < key_size) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }

      size_t output_length = PSA_MAC_TRUNCATED_LENGTH(alg);
      if (output_length == 0) {
        output_length = 16;
      } else if (output_length > 16) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }
      if (mac_size < output_length) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
      }

      uint8_t sx_mac_buf[BLK_CIPHER_MAC_SIZE];

      // Setup DMA descriptors
      block_t key_sxblk = block_t_convert(key_buffer, key_size);
      block_t input_sxblk = block_t_convert(input, input_length);
      block_t mac_sxblk = block_t_convert(sx_mac_buf, BLK_CIPHER_MAC_SIZE);

      // Acquire exclusive access to the CRYPTOACC hardware
      status = cryptoacc_management_acquire();
      if (status != PSA_SUCCESS) {
        return status;
      }

      // Execute the CMAC operation
      sx_ret = sx_aes_cmac_generate((const block_t *)&key_sxblk,
                                    (const block_t *)&input_sxblk,
                                    &mac_sxblk);

      status = cryptoacc_management_release();

      if (sx_ret != CRYPTOLIB_SUCCESS) {
        status = PSA_ERROR_HARDWARE_FAILURE;
      }

      if (status == PSA_SUCCESS) {
        memcpy(mac, sx_mac_buf, output_length);
        *mac_length = output_length;
      } else {
        *mac_length = 0;
      }

      memset(sx_mac_buf, 0, BLK_CIPHER_MAC_SIZE);
      break;
    }
#endif // PSA_WANT_ALG_CMAC
    default:
      status = PSA_ERROR_NOT_SUPPORTED;
  }

#if !defined(PSA_WANT_ALG_CMAC)
  (void)key_buffer_size;
#endif // !PSA_WANT_ALG_CMAC

  return status;

#else // PSA_WANT_ALG_HMAC) || PSA_WANT_ALG_CMAC

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

#endif // PSA_WANT_ALG_HMAC) || PSA_WANT_ALG_CMAC
}

// Make sure that the two locations of 'alg' are in the same place, since we access them
// interchangeably.
#if defined(PSA_WANT_ALG_HMAC)
_Static_assert(offsetof(sli_cryptoacc_transparent_mac_operation_t, hmac.alg)
               == offsetof(sli_cryptoacc_transparent_mac_operation_t, cipher_mac.alg),
               "hmac.alg and cipher_mac.alg are not aliases");
#endif // PSA_WANT_ALG_HMAC

psa_status_t sli_cryptoacc_transparent_mac_sign_setup(sli_cryptoacc_transparent_mac_operation_t *operation,
                                                      const psa_key_attributes_t *attributes,
                                                      const uint8_t *key_buffer,
                                                      size_t key_buffer_size,
                                                      psa_algorithm_t alg)
{
#if defined(PSA_WANT_ALG_HMAC) || defined(PSA_WANT_ALG_CMAC)

  if (operation == NULL
      || attributes == NULL
      || (key_buffer == NULL && key_buffer_size > 0)) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  size_t key_size = psa_get_key_bits(attributes) / 8;
  psa_status_t status = PSA_ERROR_INVALID_ARGUMENT;

  if (key_size > key_buffer_size) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // start by resetting context
  memset(operation, 0, sizeof(*operation));

#if defined(PSA_WANT_ALG_HMAC)
  if (PSA_ALG_IS_HMAC(alg)) {
    sx_hash_fct_t sx_hash_alg;
    size_t digest_length;
    status = sli_cryptoacc_hmac_validate_key(attributes, alg, &sx_hash_alg, &digest_length);
    if (status != PSA_SUCCESS) {
      return status;
    }

    if (PSA_MAC_TRUNCATED_LENGTH(alg) > digest_length) {
      return PSA_ERROR_INVALID_ARGUMENT;
    }

    // CRYPTOACC does not support multipart HMAC. Construct it from hashing instead.
    psa_algorithm_t hash_alg = PSA_ALG_HMAC_GET_HASH(alg);

    // Reduce the key if larger than a block
    if (key_size > sizeof(operation->hmac.opad)) {
      status = sli_cryptoacc_transparent_hash_compute(
        hash_alg,
        key_buffer,
        key_size,
        operation->hmac.opad,
        sizeof(operation->hmac.opad),
        &key_size);
      if (status != PSA_SUCCESS) {
        return status;
      }
    } else if (key_size > 0) {
      memcpy(operation->hmac.opad, key_buffer, key_size);
    }

    // Calculate inner padding in opad buffer and start a multipart hash with it
    for (size_t i = 0; i < key_size; i++) {
      operation->hmac.opad[i] ^= 0x36;
    }
    memset(&operation->hmac.opad[key_size], 0x36, sizeof(operation->hmac.opad) - key_size);

    status = sli_cryptoacc_transparent_hash_setup(
      &operation->hmac.hash_ctx,
      hash_alg);
    if (status != PSA_SUCCESS) {
      return status;
    }

    status = sli_cryptoacc_transparent_hash_update(
      &operation->hmac.hash_ctx,
      operation->hmac.opad, sizeof(operation->hmac.opad));
    if (status != PSA_SUCCESS) {
      return status;
    }

    // Calculate outer padding and store it for finalisation
    for (size_t i = 0; i < sizeof(operation->hmac.opad); i++) {
      operation->hmac.opad[i] ^= 0x36 ^ 0x5C;
    }

    operation->hmac.alg = alg;
    return PSA_SUCCESS;
  }
#endif // PSA_WANT_ALG_HMAC

  // If not HMAC, check other algos
  switch (PSA_ALG_FULL_LENGTH_MAC(alg)) {
#if defined(PSA_WANT_ALG_CMAC)
    case PSA_ALG_CMAC:
      status = sli_cryptoacc_cmac_validate_key(attributes);
      if (status != PSA_SUCCESS) {
        return status;
      }

      if (key_buffer_size < key_size) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }

      memcpy(operation->cipher_mac.key, key_buffer, key_size);
      operation->cipher_mac.key_len = key_size;

      operation->cipher_mac.alg = alg;
      status = PSA_SUCCESS;
      break;
#endif // PSA_WANT_ALG_CMAC
    default:
      status = PSA_ERROR_NOT_SUPPORTED;
      break;
  }

#if !defined(PSA_WANT_ALG_CMAC)
  (void)key_buffer_size;
#endif // !PSA_WANT_ALG_CMAC

  return status;

#else // PSA_WANT_ALG_HMAC) || PSA_WANT_ALG_CMAC

  (void)operation;
  (void)attributes;
  (void)key_buffer;
  (void)key_buffer_size;
  (void)alg;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_HMAC) || PSA_WANT_ALG_CMAC
}

psa_status_t sli_cryptoacc_transparent_mac_verify_setup(sli_cryptoacc_transparent_mac_operation_t *operation,
                                                        const psa_key_attributes_t *attributes,
                                                        const uint8_t *key_buffer,
                                                        size_t key_buffer_size,
                                                        psa_algorithm_t alg)
{
  // Since the PSA Crypto core exposes the verify functionality of the drivers without
  // actually implementing the fallback to 'sign' when the driver doesn't support verify,
  // we need to do this ourselves for the time being.
  return sli_cryptoacc_transparent_mac_sign_setup(operation,
                                                  attributes,
                                                  key_buffer,
                                                  key_buffer_size,
                                                  alg);
}

#if defined(PSA_WANT_ALG_CMAC)
static psa_status_t cryptoacc_cmac_update_blk(sli_cryptoacc_transparent_mac_operation_t *operation,
                                              const uint8_t *input, size_t input_length)
{
  psa_status_t status;
  uint32_t sx_ret;

  const block_t key_blk = block_t_convert(operation->cipher_mac.key, operation->cipher_mac.key_len);
  const block_t input_blk = block_t_convert(input, input_length);
  block_t ctx_blk = block_t_convert(operation->cipher_mac.cmac_ctx, sizeof(operation->cipher_mac.cmac_ctx));

  // Acquire exclusive access to the CRYPTOACC hardware
  status = cryptoacc_management_acquire();
  if (status != PSA_SUCCESS) {
    return status;
  }

  // CMAC state context is already initialized, do update.
  sx_ret = sx_aes_cmac_generate_update(&key_blk,
                                       &input_blk,
                                       (const block_t*)&ctx_blk,
                                       &ctx_blk);

  status = cryptoacc_management_release();

  if (sx_ret != CRYPTOLIB_SUCCESS) {
    return PSA_ERROR_HARDWARE_FAILURE;
  }
  if (status != PSA_SUCCESS) {
    return status;
  }
  return PSA_SUCCESS;
}
#endif // PSA_WANT_ALG_CMAC

psa_status_t sli_cryptoacc_transparent_mac_update(sli_cryptoacc_transparent_mac_operation_t *operation,
                                                  const uint8_t *input,
                                                  size_t input_length)
{
#if defined(PSA_WANT_ALG_HMAC) || defined(PSA_WANT_ALG_CMAC)

  if (operation == NULL
      || input == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  if (input_length == 0 ) {
    return PSA_SUCCESS;
  }

#if defined(PSA_WANT_ALG_HMAC)
  if (PSA_ALG_IS_HMAC(operation->hmac.alg)) {
    return sli_cryptoacc_transparent_hash_update(
      &operation->hmac.hash_ctx,
      input,
      input_length);
  }
#endif // PSA_WANT_ALG_HMAC

  switch (PSA_ALG_FULL_LENGTH_MAC(operation->cipher_mac.alg)) {
#if defined(PSA_WANT_ALG_CMAC)
    case PSA_ALG_CMAC:
    {
      psa_status_t status;
      size_t current_block_len;

      // if the potential last block include data
      // we need to process them first
      current_block_len = operation->cipher_mac.current_block_len;
      if (current_block_len) {
        size_t bytes_to_boundary = 16 - current_block_len;
        // if the total of bytes is smaller than a block, just copy and return
        // else fill up the potential last block
        if (input_length < bytes_to_boundary) {
          memcpy(&operation->cipher_mac.current_block[current_block_len], input, input_length);
          operation->cipher_mac.current_block_len = current_block_len + input_length;
          return PSA_SUCCESS;
        } else {
          // fill up the potential last block
          memcpy(&operation->cipher_mac.current_block[current_block_len], input, bytes_to_boundary);
          operation->cipher_mac.current_block_len = 16;
          input_length -= bytes_to_boundary;
          input += bytes_to_boundary;
        }

        // if there are more input data, the potential last block is not
        // the last block, which means we can process it now,
        if (input_length) {
          // Execute CMAC operation on the single context block
          status = cryptoacc_cmac_update_blk(operation, operation->cipher_mac.current_block, 16);
          if (status != PSA_SUCCESS) {
            return status;
          }
          operation->cipher_mac.current_block_len = 0;
        }
      }

      // Process complete input blocks
      if (input_length > 16) {
        // Calculate the number of bytes in complete blocks to process.
        // If the last block is complete we need to postpone processing it
        // since it may be the last block which should go to sx_aes_cmac_generate_final.
        size_t bytes_to_copy = (input_length - 1) & ~0xFUL;

        // Execute CMAC operation on the single context block
        status = cryptoacc_cmac_update_blk(operation, input, bytes_to_copy);
        if (status != PSA_SUCCESS) {
          return status;
        }

        input_length -= bytes_to_copy;
        input += bytes_to_copy;
      }

      // Store remaining input bytes
      if (input_length) {
        memcpy(operation->cipher_mac.current_block, input, input_length);
        operation->cipher_mac.current_block_len = input_length;
      }

      return PSA_SUCCESS;
    }
#endif // PSA_WANT_ALG_CMAC
    default:
      return PSA_ERROR_BAD_STATE;
  }

#else // PSA_WANT_ALG_HMAC) || PSA_WANT_ALG_CMAC

  (void)operation;
  (void)input;
  (void)input_length;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_HMAC) || PSA_WANT_ALG_CMAC
}

psa_status_t sli_cryptoacc_transparent_mac_sign_finish(sli_cryptoacc_transparent_mac_operation_t *operation,
                                                       uint8_t *mac,
                                                       size_t mac_size,
                                                       size_t *mac_length)
{
#if defined(PSA_WANT_ALG_HMAC) || defined(PSA_WANT_ALG_CMAC)

  if (operation == NULL
      || mac == NULL
      || mac_length == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  psa_status_t status;

#if defined(PSA_WANT_ALG_HMAC)
  if (PSA_ALG_IS_HMAC(operation->hmac.alg)) {
    uint8_t buffer[64 + 32];
    size_t olen;

    // Construct outer hash input from opad and hash result
    memcpy(buffer, operation->hmac.opad, sizeof(operation->hmac.opad));
    memset(operation->hmac.opad, 0, sizeof(operation->hmac.opad));

    status = sli_cryptoacc_transparent_hash_finish(
      &operation->hmac.hash_ctx,
      &buffer[sizeof(operation->hmac.opad)],
      sizeof(buffer) - sizeof(operation->hmac.opad),
      &olen);

    if (status != PSA_SUCCESS) {
      return status;
    }

    // Calculate HMAC
    status = sli_cryptoacc_transparent_hash_compute(
      PSA_ALG_HMAC_GET_HASH(operation->hmac.alg),
      buffer,
      sizeof(operation->hmac.opad) + olen,
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

  // Check algorithm and store if supported
  switch (PSA_ALG_FULL_LENGTH_MAC(operation->cipher_mac.alg)) {
#if defined(PSA_WANT_ALG_CMAC)
    case PSA_ALG_CMAC:
    {
      // Check output size
      size_t requested_length = PSA_MAC_TRUNCATED_LENGTH(operation->cipher_mac.alg);
      if (requested_length == 0) {
        requested_length = BLK_CIPHER_MAC_SIZE;
      } else if (requested_length > BLK_CIPHER_MAC_SIZE) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }

      if (mac_size < requested_length) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
      }

      // Acquire exclusive access to the CRYPTOACC hardware
      status = cryptoacc_management_acquire();
      if (status != PSA_SUCCESS) {
        return status;
      }

      // Setup DMA descriptors
      block_t input_blk = block_t_convert(operation->cipher_mac.current_block,
                                          operation->cipher_mac.current_block_len);
      const block_t key_blk = block_t_convert(operation->cipher_mac.key, operation->cipher_mac.key_len);
      block_t ctx_blk = block_t_convert(operation->cipher_mac.cmac_ctx, sizeof(operation->cipher_mac.cmac_ctx));

      // Execute the first CMAC operation.
      // Receive the final mac in the cmac_ctx buffer and copy the requested
      // number of bytes to the user buffer after.
      uint32_t sx_ret = sx_aes_cmac_generate_final(&key_blk,
                                                   (const block_t *)&input_blk,
                                                   (const block_t *)&ctx_blk,
                                                   &ctx_blk);

      status = cryptoacc_management_release();

      if (sx_ret != CRYPTOLIB_SUCCESS) {
        return PSA_ERROR_HARDWARE_FAILURE;
      }
      if (status != PSA_SUCCESS) {
        return status;
      }

      // Copy the requested number of bytes (max 16 for CMAC) to the user buffer.
      memcpy(mac, operation->cipher_mac.cmac_ctx, requested_length);
      *mac_length = requested_length;

      return PSA_SUCCESS;
    }
    break;
#endif // PSA_WANT_ALG_CMAC
    default:
      return PSA_ERROR_NOT_SUPPORTED;
  }

#else // PSA_WANT_ALG_HMAC) || PSA_WANT_ALG_CMAC

  (void)operation;
  (void)mac;
  (void)mac_size;
  (void)mac_length;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_HMAC) || PSA_WANT_ALG_CMAC
}

psa_status_t sli_cryptoacc_transparent_mac_verify_finish(sli_cryptoacc_transparent_mac_operation_t *operation,
                                                         const uint8_t *mac,
                                                         size_t mac_length)
{
  // Since the PSA Crypto core exposes the verify functionality of the drivers without
  // actually implementing the fallback to 'sign' when the driver doesn't support verify,
  // we need to do this ourselves for the time being.
  uint8_t calculated_mac[PSA_MAC_MAX_SIZE] = { 0 };
  size_t calculated_length = PSA_MAC_MAX_SIZE;

  psa_status_t status = sli_cryptoacc_transparent_mac_sign_finish(
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
}

psa_status_t sli_cryptoacc_transparent_mac_abort(sli_cryptoacc_transparent_mac_operation_t *operation)
{
#if defined(PSA_WANT_ALG_HMAC) || defined(PSA_WANT_ALG_CMAC)

  // There's no state in hardware that we need to preserve, so zeroing out the context suffices.
  if (operation == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  memset(operation, 0, sizeof(*operation));
  return PSA_SUCCESS;

#else // PSA_WANT_ALG_HMAC) || PSA_WANT_ALG_CMAC

  (void)operation;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_HMAC) || PSA_WANT_ALG_CMAC
}

#endif // defined(CRYPTOACC_PRESENT)
