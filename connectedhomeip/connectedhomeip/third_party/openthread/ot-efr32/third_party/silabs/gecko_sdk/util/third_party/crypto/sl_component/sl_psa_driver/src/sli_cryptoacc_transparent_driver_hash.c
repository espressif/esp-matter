/***************************************************************************//**
 * @file
 * @brief Silicon Labs PSA Crypto Transparent Driver Hash functions.
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

#include "sli_cryptoacc_transparent_types.h"
#include "sli_cryptoacc_transparent_functions.h"

#if defined(PSA_WANT_ALG_SHA_1)    \
  || defined(PSA_WANT_ALG_SHA_224) \
  || defined(PSA_WANT_ALG_SHA_256)

#include "cryptoacc_management.h"
#include "sx_hash.h"
#include "sx_errors.h"
#include <string.h>

// Define all init vectors.
#if defined(PSA_WANT_ALG_SHA_1)
static const uint8_t init_state_sha1[32] = {
  0x67, 0x45, 0x23, 0x01,
  0xEF, 0xCD, 0xAB, 0x89,
  0x98, 0xBA, 0xDC, 0xFE,
  0x10, 0x32, 0x54, 0x76,
  0xC3, 0xD2, 0xE1, 0xF0,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00
};
#endif // PSA_WANT_ALG_SHA_1
#if defined(PSA_WANT_ALG_SHA_224)
static const uint8_t init_state_sha224[32] = {
  0xC1, 0x05, 0x9E, 0xD8,
  0x36, 0x7C, 0xD5, 0x07,
  0x30, 0x70, 0xDD, 0x17,
  0xF7, 0x0E, 0x59, 0x39,
  0xFF, 0xC0, 0x0B, 0x31,
  0x68, 0x58, 0x15, 0x11,
  0x64, 0xF9, 0x8F, 0xA7,
  0xBE, 0xFA, 0x4F, 0xA4
};
#endif // PSA_WANT_ALG_SHA_224
#if defined(PSA_WANT_ALG_SHA_256)
static const uint8_t init_state_sha256[32] = {
  0x6A, 0x09, 0xE6, 0x67,
  0xBB, 0x67, 0xAE, 0x85,
  0x3C, 0x6E, 0xF3, 0x72,
  0xA5, 0x4F, 0xF5, 0x3A,
  0x51, 0x0E, 0x52, 0x7F,
  0x9B, 0x05, 0x68, 0x8C,
  0x1F, 0x83, 0xD9, 0xAB,
  0x5B, 0xE0, 0xCD, 0x19
};
#endif // PSA_WANT_ALG_SHA_256

#endif // PSA_WANT_ALG_SHA_*

psa_status_t sli_cryptoacc_transparent_hash_setup(sli_cryptoacc_transparent_hash_operation_t *operation,
                                                  psa_algorithm_t alg)
{
#if defined(PSA_WANT_ALG_SHA_1)    \
  || defined(PSA_WANT_ALG_SHA_224) \
  || defined(PSA_WANT_ALG_SHA_256)

  if (operation == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  if (!PSA_ALG_IS_HASH(alg)) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // Reset context.
  memset(operation, 0, sizeof(sli_cryptoacc_transparent_hash_operation_t));

  switch (alg) {
#if defined(PSA_WANT_ALG_SHA_1)
    case PSA_ALG_SHA_1:
      operation->hash_type = e_SHA1;
      memcpy(operation->state, init_state_sha1, SHA1_STATESIZE);
      break;
#endif // PSA_WANT_ALG_SHA_1
#if defined(PSA_WANT_ALG_SHA_224)
    case PSA_ALG_SHA_224:
      operation->hash_type = e_SHA224;
      memcpy(operation->state, init_state_sha224, SHA224_STATESIZE);
      break;
#endif // PSA_WANT_ALG_SHA_224
#if defined(PSA_WANT_ALG_SHA_256)
    case PSA_ALG_SHA_256:
      operation->hash_type = e_SHA256;
      memcpy(operation->state, init_state_sha256, SHA256_STATESIZE);
      break;
#endif // PSA_WANT_ALG_SHA_256
    default:
      return PSA_ERROR_NOT_SUPPORTED;
  }

  operation->total = 0;

  return PSA_SUCCESS;

#else // PSA_WANT_ALG_SHA_*

  (void)operation;
  (void)alg;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_SHA_*
}

psa_status_t sli_cryptoacc_transparent_hash_update(sli_cryptoacc_transparent_hash_operation_t *operation,
                                                   const uint8_t *input,
                                                   size_t input_length)
{
#if defined(PSA_WANT_ALG_SHA_1)    \
  || defined(PSA_WANT_ALG_SHA_224) \
  || defined(PSA_WANT_ALG_SHA_256)

  size_t blocks, fill, left;
  block_t data_in;
  block_t state;
  uint32_t sx_ret;
  psa_status_t status;

  if (operation == NULL
      || (input == NULL && input_length > 0)) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  switch (operation->hash_type) {
#if defined(PSA_WANT_ALG_SHA_1)
    case e_SHA1:
#endif // PSA_WANT_ALG_SHA_1
#if defined(PSA_WANT_ALG_SHA_224)
    case e_SHA224:
#endif // PSA_WANT_ALG_SHA_224
#if defined(PSA_WANT_ALG_SHA_256)
    case e_SHA256:
#endif // PSA_WANT_ALG_SHA_256
    break;
    default:
      // State must have not been initialized by the setup function.
      return PSA_ERROR_BAD_STATE;
  }

  if (input_length == 0) {
    return PSA_SUCCESS;
  }

  state = block_t_convert((uint8_t*)operation->state,
                          sx_hash_get_state_size(operation->hash_type));

  // Same blocksize for all of SHA-256, SHA-224, and SHA-256.
  left = (operation->total & (SHA256_BLOCKSIZE - 1));
  fill = SHA256_BLOCKSIZE - left;

  operation->total += input_length;

  if ((left > 0) && (input_length >= fill)) {
    memcpy((void *)(operation->buffer + left), input, fill);

    data_in = block_t_convert(operation->buffer, SHA256_BLOCKSIZE);

    status = cryptoacc_management_acquire();
    if (status != PSA_SUCCESS) {
      return status;
    }
    sx_ret = sx_hash_update_blk(operation->hash_type, state, data_in);
    status = cryptoacc_management_release();
    if (sx_ret != CRYPTOLIB_SUCCESS
        || status != PSA_SUCCESS) {
      return PSA_ERROR_HARDWARE_FAILURE;
    }

    input += fill;
    input_length -= fill;
    left = 0;
  }

  if (input_length >= SHA256_BLOCKSIZE) {
    blocks = input_length / SHA256_BLOCKSIZE;

    data_in = block_t_convert((uint8_t*)input, SHA256_BLOCKSIZE * blocks);

    status = cryptoacc_management_acquire();
    if (status != PSA_SUCCESS) {
      return status;
    }
    sx_ret = sx_hash_update_blk(operation->hash_type, state, data_in);
    status = cryptoacc_management_release();
    if (sx_ret != CRYPTOLIB_SUCCESS
        || status != PSA_SUCCESS) {
      return PSA_ERROR_HARDWARE_FAILURE;
    }

    input += SHA256_BLOCKSIZE * blocks;
    input_length -= SHA256_BLOCKSIZE * blocks;
  }

  if (input_length > 0) {
    memcpy((void *)(operation->buffer + left), input, input_length);
  }

  return PSA_SUCCESS;

#else // PSA_WANT_ALG_SHA_*

  (void)operation;
  (void)input;
  (void)input_length;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_SHA_*
}

psa_status_t sli_cryptoacc_transparent_hash_finish(sli_cryptoacc_transparent_hash_operation_t *operation,
                                                   uint8_t *hash,
                                                   size_t hash_size,
                                                   size_t *hash_length)
{
#if defined(PSA_WANT_ALG_SHA_1)    \
  || defined(PSA_WANT_ALG_SHA_224) \
  || defined(PSA_WANT_ALG_SHA_256)

  psa_status_t status;
  uint32_t sx_ret;
  block_t state;
  block_t data_in;
  block_t data_out;

  if (operation == NULL
      || (hash_length == NULL && hash_size > 0)
      || (hash == NULL && hash_size > 0)) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  switch (operation->hash_type) {
#if defined(PSA_WANT_ALG_SHA_1)
    case e_SHA1:
#endif // PSA_WANT_ALG_SHA_1
#if defined(PSA_WANT_ALG_SHA_224)
    case e_SHA224:
#endif // PSA_WANT_ALG_SHA_224
#if defined(PSA_WANT_ALG_SHA_256)
    case e_SHA256:
#endif // PSA_WANT_ALG_SHA_256
    break;
    default:
      // State must have not been initialized by the setup function.
      return PSA_ERROR_BAD_STATE;
  }

  if (hash_size < sx_hash_get_digest_size(operation->hash_type)) {
    return PSA_ERROR_BUFFER_TOO_SMALL;
  }

  state = block_t_convert((uint8_t*)operation->state,
                          sx_hash_get_state_size(operation->hash_type));
  data_in = block_t_convert((uint8_t*)operation->buffer,
                            (operation->total & (SHA256_BLOCKSIZE - 1)));

  data_out = block_t_convert((uint8_t*)operation->state,
                             sx_hash_get_state_size(operation->hash_type));

  status = cryptoacc_management_acquire();
  if (status != PSA_SUCCESS) {
    return status;
  }
  sx_ret = sx_hash_finish_blk(operation->hash_type,
                              state,
                              data_in,
                              data_out,
                              operation->total);
  status = cryptoacc_management_release();
  if (sx_ret != CRYPTOLIB_SUCCESS
      || status != PSA_SUCCESS) {
    return PSA_ERROR_HARDWARE_FAILURE;
  }

  *hash_length = sx_hash_get_digest_size(operation->hash_type);
  memcpy(hash, operation->state, *hash_length);
  memset(operation, 0, sizeof(sli_cryptoacc_transparent_hash_operation_t));

  return PSA_SUCCESS;

#else // PSA_WANT_ALG_SHA_*

  (void)operation;
  (void)hash;
  (void)hash_size;
  (void)hash_length;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_SHA_*
}

psa_status_t sli_cryptoacc_transparent_hash_abort(sli_cryptoacc_transparent_hash_operation_t *operation)
{
#if defined(PSA_WANT_ALG_SHA_1)    \
  || defined(PSA_WANT_ALG_SHA_224) \
  || defined(PSA_WANT_ALG_SHA_256)

  if (operation != NULL) {
    // Accelerator does not keep state, so just zero out the context and we're good.
    memset(operation, 0, sizeof(sli_cryptoacc_transparent_hash_operation_t));
  }

  return PSA_SUCCESS;

#else // PSA_WANT_ALG_SHA_*

  (void)operation;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_SHA_*
}

psa_status_t sli_cryptoacc_transparent_hash_compute(psa_algorithm_t alg,
                                                    const uint8_t *input,
                                                    size_t input_length,
                                                    uint8_t *hash,
                                                    size_t hash_size,
                                                    size_t *hash_length)
{
#if defined(PSA_WANT_ALG_SHA_1)    \
  || defined(PSA_WANT_ALG_SHA_224) \
  || defined(PSA_WANT_ALG_SHA_256)

  psa_status_t status;
  uint32_t sx_ret = CRYPTOLIB_INVALID_PARAM;
  block_t data_in;
  block_t data_out;

  if ((input == NULL && input_length > 0)
      || (hash == NULL && hash_size > 0)
      || (hash_length == NULL && hash_size > 0)) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  switch (alg) {
#if defined(PSA_WANT_ALG_SHA_1)
    case PSA_ALG_SHA_1:
      if (hash_size < SHA1_DIGESTSIZE) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
      }
      break;
#endif // PSA_WANT_ALG_SHA_1
#if defined(PSA_WANT_ALG_SHA_224)
    case PSA_ALG_SHA_224:
      if (hash_size < SHA224_DIGESTSIZE) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
      }
      break;
#endif // PSA_WANT_ALG_SHA_224
#if defined(PSA_WANT_ALG_SHA_256)
    case PSA_ALG_SHA_256:
      if (hash_size < SHA256_DIGESTSIZE) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
      }
      break;
#endif // PSA_WANT_ALG_SHA_256
    default:
      return PSA_ERROR_NOT_SUPPORTED;
  }

  data_in = block_t_convert(input, input_length);
  data_out = block_t_convert(hash, hash_size);

  status = cryptoacc_management_acquire();
  if (status != PSA_SUCCESS) {
    return status;
  }

  switch (alg) {
#if defined(PSA_WANT_ALG_SHA_1)
    case PSA_ALG_SHA_1:
      sx_ret = sx_hash_blk(e_SHA1, data_in, data_out);
      *hash_length = SHA1_DIGESTSIZE;
      break;
#endif // PSA_WANT_ALG_SHA_1
#if defined(PSA_WANT_ALG_SHA_224)
    case PSA_ALG_SHA_224:
      sx_ret = sx_hash_blk(e_SHA224, data_in, data_out);
      *hash_length = SHA224_DIGESTSIZE;
      break;
#endif // PSA_WANT_ALG_SHA_224
#if defined(PSA_WANT_ALG_SHA_256)
    case PSA_ALG_SHA_256:
      sx_ret = sx_hash_blk(e_SHA256, data_in, data_out);
      *hash_length = SHA256_DIGESTSIZE;
      break;
#endif // PSA_WANT_ALG_SHA_256
  }

  status = cryptoacc_management_release();
  if (sx_ret != CRYPTOLIB_SUCCESS
      || status != PSA_SUCCESS) {
    *hash_length = 0;
    return PSA_ERROR_HARDWARE_FAILURE;
  }

  return PSA_SUCCESS;

#else // PSA_WANT_ALG_SHA_*

  (void)alg;
  (void)input;
  (void)input_length;
  (void)hash;
  (void)hash_size;
  (void)hash_length;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_SHA_*
}

psa_status_t sli_cryptoacc_transparent_hash_clone(const sli_cryptoacc_transparent_hash_operation_t *source_operation,
                                                  sli_cryptoacc_transparent_hash_operation_t *target_operation)
{
#if defined(PSA_WANT_ALG_SHA_1)    \
  || defined(PSA_WANT_ALG_SHA_224) \
  || defined(PSA_WANT_ALG_SHA_256)

  if (source_operation == NULL
      || target_operation == NULL) {
    return PSA_ERROR_BAD_STATE;
  }

  // Source operation must be active (setup has been called)
  if (source_operation->hash_type == 0) {
    return PSA_ERROR_BAD_STATE;
  }

  // Target operation must be inactive (setup has not been called)
  if (target_operation->hash_type != 0) {
    return PSA_ERROR_BAD_STATE;
  }

  // The operation context does not contain any pointers, and the target operation
  // have already have been initialized, so we can do a direct copy.
  *target_operation = *source_operation;

  return PSA_SUCCESS;

#else // PSA_WANT_ALG_SHA_*

  (void)source_operation;
  (void)target_operation;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_SHA_*
}

#endif // defined(CRYPTOACC_PRESENT)
