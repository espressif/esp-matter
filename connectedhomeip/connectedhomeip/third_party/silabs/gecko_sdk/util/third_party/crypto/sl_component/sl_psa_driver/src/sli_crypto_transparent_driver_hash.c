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

#if defined(CRYPTO_PRESENT)

#include "sli_crypto_transparent_types.h"
#include "sli_crypto_transparent_functions.h"

#if defined(PSA_WANT_ALG_SHA_1)    \
  || defined(PSA_WANT_ALG_SHA_224) \
  || defined(PSA_WANT_ALG_SHA_256)

#include "crypto_management.h"
#include "em_crypto.h"
#include "em_core.h"
#include "sl_assert.h"
#include <string.h>

// SHA-1, SHA-224 and SHA-256 share the same counter and block sizes, which
// can be regarded as constant since SHA-384 and SHA-512 aren't supported.
#define SHA_COUNTER_SIZE 2
#define SHA_BLOCK_SIZE 64

#if defined(PSA_WANT_ALG_SHA_1)
static const uint8_t init_state_sha1[32] =
{
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
static const uint8_t init_state_sha224[32] =
{
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
static const uint8_t init_state_sha256[32] =
{
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

static const unsigned char sha_padding[64] =
{
  0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static int crypto_hash_process(psa_algorithm_t algo,
                               const uint32_t* state_in,
                               const uint32_t* blockdata,
                               uint32_t* state_out,
                               uint32_t num_blocks);

#endif // PSA_WANT_ALG_SHA_*

psa_status_t sli_crypto_transparent_hash_setup(sli_crypto_transparent_hash_operation_t *operation,
                                               psa_algorithm_t alg)
{
#if defined(PSA_WANT_ALG_SHA_1)    \
  || defined(PSA_WANT_ALG_SHA_224) \
  || defined(PSA_WANT_ALG_SHA_256)

  if (operation == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  memset(operation, 0, sizeof(sli_crypto_transparent_hash_operation_t));

  switch (alg) {
#if defined(PSA_WANT_ALG_SHA_1)
    case PSA_ALG_SHA_1:
      memcpy(operation->state, init_state_sha1, sizeof(init_state_sha1));
      break;
#endif // PSA_WANT_ALG_SHA_1
#if defined(PSA_WANT_ALG_SHA_224)
    case PSA_ALG_SHA_224:
      memcpy(operation->state, init_state_sha224, sizeof(init_state_sha224));
      break;
#endif // PSA_WANT_ALG_SHA_224
#if defined(PSA_WANT_ALG_SHA_256)
    case PSA_ALG_SHA_256:
      memcpy(operation->state, init_state_sha256, sizeof(init_state_sha256));
      break;
#endif // PSA_WANT_ALG_SHA_256
    default:
      return PSA_ERROR_NOT_SUPPORTED;
  }

  operation->hash_type = alg;
  operation->total[0] = 0;
  operation->total[1] = 0;

  return PSA_SUCCESS;

#else // PSA_WANT_ALG_SHA_*

  (void)operation;
  (void)alg;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_SHA_*
}

psa_status_t sli_crypto_transparent_hash_update(sli_crypto_transparent_hash_operation_t *operation,
                                                const uint8_t *input,
                                                size_t input_length)
{
#if defined(PSA_WANT_ALG_SHA_1)    \
  || defined(PSA_WANT_ALG_SHA_224) \
  || defined(PSA_WANT_ALG_SHA_256)

  if (operation == NULL
      || (input == NULL && input_length > 0)) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  size_t blocks, fill, left;

  switch (operation->hash_type) {
#if defined(PSA_WANT_ALG_SHA_1)
    case PSA_ALG_SHA_1:
#endif // PSA_WANT_ALG_SHA_1
#if defined(PSA_WANT_ALG_SHA_224)
    case PSA_ALG_SHA_224:
#endif // PSA_WANT_ALG_SHA_224
#if defined(PSA_WANT_ALG_SHA_256)
    case PSA_ALG_SHA_256:
#endif // PSA_WANT_ALG_SHA_256
    break;
    default:
      return PSA_ERROR_BAD_STATE;
  }

  if (input_length == 0) {
    return PSA_SUCCESS;
  }

  left = (operation->total[0] & (SHA_BLOCK_SIZE - 1));
  fill = SHA_BLOCK_SIZE - left;

  operation->total[0] += input_length;

  // Ripple counter
  if (operation->total[0] < input_length) {
    operation->total[1] += 1;
  }

  if ((left > 0) && (input_length >= fill)) {
    memcpy((void*)(((uint32_t)operation->buffer) + left), input, fill);
    crypto_hash_process(operation->hash_type,
                        operation->state,
                        operation->buffer,
                        operation->state,
                        1);

    input += fill;
    input_length -= fill;
    left = 0;
  }

  if (input_length >= SHA_BLOCK_SIZE) {
    blocks = input_length / SHA_BLOCK_SIZE;

    if (((uint32_t)input & 0x3) == 0) {
      // word-aligned input can be processed directly
      crypto_hash_process(operation->hash_type,
                          operation->state,
                          (const uint32_t*)input,
                          operation->state,
                          blocks);
    } else {
      // Unaligned input needs to be aligned and processed blockwise
      for (size_t i = 0; i < blocks; i++) {
        memcpy(operation->buffer, &input[i * SHA_BLOCK_SIZE], SHA_BLOCK_SIZE);
        crypto_hash_process(operation->hash_type,
                            operation->state,
                            operation->buffer,
                            operation->state,
                            1);
      }
    }

    input += SHA_BLOCK_SIZE * blocks;
    input_length -= SHA_BLOCK_SIZE * blocks;
  }

  if (input_length > 0) {
    memcpy((void*)(((uint32_t)operation->buffer) + left), input, input_length);
  }

  return PSA_SUCCESS;

#else // PSA_WANT_ALG_SHA_*

  (void)operation;
  (void)input;
  (void)input_length;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_SHA_*
}

psa_status_t sli_crypto_transparent_hash_finish(sli_crypto_transparent_hash_operation_t *operation,
                                                uint8_t *hash,
                                                size_t hash_size,
                                                size_t *hash_length)
{
#if defined(PSA_WANT_ALG_SHA_1)    \
  || defined(PSA_WANT_ALG_SHA_224) \
  || defined(PSA_WANT_ALG_SHA_256)

  size_t last_data_byte, num_pad_bytes, output_size;
  uint8_t msglen[8];

  if (operation == NULL
      || (hash_length == NULL && hash_size > 0)
      || (hash == NULL && hash_size > 0)) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  switch (operation->hash_type) {
#if defined(PSA_WANT_ALG_SHA_1)
    case PSA_ALG_SHA_1:
      output_size = 20;
      break;
#endif // PSA_WANT_ALG_SHA_1
#if defined(PSA_WANT_ALG_SHA_224)
    case PSA_ALG_SHA_224:
      output_size = 28;
      break;
#endif // PSA_WANT_ALG_SHA_224
#if defined(PSA_WANT_ALG_SHA_256)
    case PSA_ALG_SHA_256:
      output_size = 32;
      break;
#endif // PSA_WANT_ALG_SHA_256
    default:
      return PSA_ERROR_BAD_STATE;
  }

  if (hash_size < output_size) {
    return PSA_ERROR_BUFFER_TOO_SMALL;
  }

  /* Convert counter value to bits, and put in big-endian array */
  uint8_t residual = 0;
  for (size_t i = 0; i < SHA_COUNTER_SIZE; i++) {
    size_t msglen_index = ( (SHA_COUNTER_SIZE - i) * sizeof(uint32_t) ) - 1;

    msglen[msglen_index - 0] = ((operation->total[i] << 3) + residual) & 0xFF;
    msglen[msglen_index - 1] = (operation->total[i] >> 5) & 0xFF;
    msglen[msglen_index - 2] = (operation->total[i] >> 13) & 0xFF;
    msglen[msglen_index - 3] = (operation->total[i] >> 21) & 0xFF;

    residual = (operation->total[i] >> 29) & 0xFF;
  }

  last_data_byte = (operation->total[0] & (SHA_BLOCK_SIZE - 1) );
  num_pad_bytes = (last_data_byte < (SHA_BLOCK_SIZE - (SHA_COUNTER_SIZE * 4)) )
                  ? ( (SHA_BLOCK_SIZE - (SHA_COUNTER_SIZE * 4)) - last_data_byte)
                  : ( ((2 * SHA_BLOCK_SIZE) - (SHA_COUNTER_SIZE * 4)) - last_data_byte);

  psa_status_t status;
  status = sli_crypto_transparent_hash_update(operation, sha_padding, num_pad_bytes);
  if (status != PSA_SUCCESS) {
    return status;
  }

  status = sli_crypto_transparent_hash_update(operation, msglen, (SHA_COUNTER_SIZE * 4));
  if (status != PSA_SUCCESS) {
    return status;
  }

  *hash_length = output_size;
  memcpy(hash, operation->state, output_size);

  return PSA_SUCCESS;

#else // PSA_WANT_ALG_SHA_*

  (void)operation;
  (void)hash;
  (void)hash_size;
  (void)hash_length;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_SHA_*
}

psa_status_t sli_crypto_transparent_hash_abort(sli_crypto_transparent_hash_operation_t *operation)
{
#if defined(PSA_WANT_ALG_SHA_1)    \
  || defined(PSA_WANT_ALG_SHA_224) \
  || defined(PSA_WANT_ALG_SHA_256)

  if (operation != NULL) {
    // Accelerator does not keep state, so just zero out the context and we're good
    memset(operation, 0, sizeof(sli_crypto_transparent_hash_operation_t));
  }

  return PSA_SUCCESS;

#else // PSA_WANT_ALG_SHA_*

  (void)operation;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_SHA_*
}

psa_status_t sli_crypto_transparent_hash_compute(psa_algorithm_t alg,
                                                 const uint8_t *input,
                                                 size_t input_length,
                                                 uint8_t *hash,
                                                 size_t hash_size,
                                                 size_t *hash_length)
{
#if defined(PSA_WANT_ALG_SHA_1)    \
  || defined(PSA_WANT_ALG_SHA_224) \
  || defined(PSA_WANT_ALG_SHA_256)

  sli_crypto_transparent_hash_operation_t operation;

  if ((input == NULL && input_length > 0)
      || (hash == NULL && hash_size > 0)
      || (hash_length == NULL && hash_size > 0)) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  psa_status_t status = sli_crypto_transparent_hash_setup(&operation, alg);
  if (status != PSA_SUCCESS) {
    return status;
  }

  size_t output_size;
  switch (operation.hash_type) {
#if defined(PSA_WANT_ALG_SHA_1)
    case PSA_ALG_SHA_1:
      output_size = 20;
      break;
#endif // PSA_WANT_ALG_SHA_1
#if defined(PSA_WANT_ALG_SHA_224)
    case PSA_ALG_SHA_224:
      output_size = 28;
      break;
#endif // PSA_WANT_ALG_SHA_224
#if defined(PSA_WANT_ALG_SHA_256)
    case PSA_ALG_SHA_256:
      output_size = 32;
      break;
#endif // PSA_WANT_ALG_SHA_256
    default:
      return PSA_ERROR_BAD_STATE;
  }

  // Verify sufficient buffer size for hash digest.
  // Unecessesary to wait for this error to be returned from the finish function.
  if (hash_size < output_size) {
    return PSA_ERROR_BUFFER_TOO_SMALL;
  }

  status = sli_crypto_transparent_hash_update(&operation,
                                              input,
                                              input_length);
  if (status != PSA_SUCCESS) {
    return status;
  }

  status = sli_crypto_transparent_hash_finish(&operation,
                                              hash,
                                              hash_size,
                                              hash_length);

  return status;

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

#if defined(PSA_WANT_ALG_SHA_1)    \
  || defined(PSA_WANT_ALG_SHA_224) \
  || defined(PSA_WANT_ALG_SHA_256)

static int crypto_hash_process(psa_algorithm_t algo,
                               const uint32_t* state_in,
                               const uint32_t* blockdata,
                               uint32_t* state_out,
                               uint32_t num_blocks)
{
  CORE_DECLARE_IRQ_STATE;
  CRYPTO_TypeDef *crypto = crypto_management_acquire();

  switch (algo) {
#if defined(PSA_WANT_ALG_SHA_1)
    case PSA_ALG_SHA_1:
      crypto->CTRL = CRYPTO_CTRL_SHA_SHA1;
      break;
#endif // PSA_WANT_ALG_SHA_1
#if defined(PSA_WANT_ALG_SHA_224) || defined(PSA_WANT_ALG_SHA_256)
    case PSA_ALG_SHA_224:
    case PSA_ALG_SHA_256:
      crypto->CTRL = CRYPTO_CTRL_SHA_SHA2;
      break;
#endif // PSA_WANT_ALG_SHA_224 || PSA_WANT_ALG_SHA_256
    default:
      return PSA_ERROR_BAD_STATE;
  }

  crypto->WAC      = 0;
  crypto->IEN      = 0;

  /* Set result width of MADD32 operation. */
  CRYPTO_ResultWidthSet(crypto, cryptoResult256Bits);

  /* Clear sequence control registers */
  crypto->SEQCTRL  = 0;
  crypto->SEQCTRLB = 0;

  /* Get state in big-endian */
  uint32_t statedata[8];
  for (size_t i = 0; i < 8; i++) {
    statedata[i] = __REV(state_in[i]);
  }

  /* Put the state into crypto */
  CORE_ENTER_CRITICAL();
  CRYPTO_DDataWrite(&crypto->DDATA1, statedata);
  CORE_EXIT_CRITICAL();

  CRYPTO_EXECUTE_3(crypto,
                   CRYPTO_CMD_INSTR_DDATA1TODDATA0,
                   CRYPTO_CMD_INSTR_DDATA1TODDATA2,
                   CRYPTO_CMD_INSTR_SELDDATA0DDATA1);

  /* Load the data block(s) */
  for ( size_t i = 0; i < num_blocks; i++ ) {
    CORE_ENTER_CRITICAL();
    CRYPTO_QDataWrite(&crypto->QDATA1BIG, &blockdata[i * SHA_BLOCK_SIZE / sizeof(uint32_t)]);
    CORE_EXIT_CRITICAL();

    /* Process the loaded data block */
    CRYPTO_EXECUTE_3(crypto,
                     CRYPTO_CMD_INSTR_SHA,
                     CRYPTO_CMD_INSTR_MADD32,
                     CRYPTO_CMD_INSTR_DDATA0TODDATA1);
  }

  /* Fetch state of the hash algorithm */
  CORE_ENTER_CRITICAL();
  CRYPTO_DDataRead(&crypto->DDATA0, state_out);
  CORE_EXIT_CRITICAL();

  crypto_management_release(crypto);

  /* Store state in little-endian */
  for (size_t i = 0; i < 8; i++) {
    state_out[i] = __REV(state_out[i]);
  }

  return PSA_SUCCESS;
}

#endif // PSA_WANT_ALG_SHA_*

psa_status_t sli_crypto_transparent_hash_clone(const sli_crypto_transparent_hash_operation_t *source_operation,
                                               sli_crypto_transparent_hash_operation_t *target_operation)
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

#endif // defined(CRYPTO_PRESENT)
