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

#if defined(SEMAILBOX_PRESENT)

#include "psa/crypto.h"
#include "sli_se_transparent_types.h"
#include "sli_se_transparent_functions.h"

#if defined(PSA_WANT_ALG_SHA_1)    \
  || defined(PSA_WANT_ALG_SHA_224) \
  || defined(PSA_WANT_ALG_SHA_256) \
  || defined(PSA_WANT_ALG_SHA_384) \
  || defined(PSA_WANT_ALG_SHA_512)

#include "sl_se_manager.h"
#include "sl_se_manager_hash.h"

#include <string.h>

#endif // PSA_WANT_ALG_SHA_*

psa_status_t sli_se_transparent_hash_setup(sli_se_transparent_hash_operation_t *operation,
                                           psa_algorithm_t alg)
{
#if defined(PSA_WANT_ALG_SHA_1)                                        \
  || defined(PSA_WANT_ALG_SHA_224)                                     \
  || defined(PSA_WANT_ALG_SHA_256)                                     \
  || ((defined(PSA_WANT_ALG_SHA_384) || defined(PSA_WANT_ALG_SHA_512)) \
  && (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT))

  if (operation == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // reset context
  memset(&operation->streaming_contexts, 0, sizeof(operation->streaming_contexts));

  // create ephemeral contexts
  sl_se_command_context_t ephemeral_se_ctx;
  sl_status_t status = SL_STATUS_INVALID_PARAMETER;

  switch (alg) {
#if defined(PSA_WANT_ALG_SHA_1)
    case PSA_ALG_SHA_1:
      operation->hash_type = SL_SE_HASH_SHA1;
      status = sl_se_hash_sha1_multipart_starts(&(operation->streaming_contexts.sha1_context),
                                                &ephemeral_se_ctx);
      break;
#endif // PSA_WANT_ALG_SHA_1
#if defined(PSA_WANT_ALG_SHA_224)
    case PSA_ALG_SHA_224:
      operation->hash_type = SL_SE_HASH_SHA224;
      status = sl_se_hash_sha224_multipart_starts(&(operation->streaming_contexts.sha224_context),
                                                  &ephemeral_se_ctx);
      break;
#endif // PSA_WANT_ALG_SHA_224
#if defined(PSA_WANT_ALG_SHA_256)
    case PSA_ALG_SHA_256:
      operation->hash_type = SL_SE_HASH_SHA256;
      status = sl_se_hash_sha256_multipart_starts(&(operation->streaming_contexts.sha256_context),
                                                  &ephemeral_se_ctx);
      break;
#endif // PSA_WANT_ALG_SHA_256
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT) || defined(DOXYGEN)
#if defined(PSA_WANT_ALG_SHA_384)
    case PSA_ALG_SHA_384:
      operation->hash_type = SL_SE_HASH_SHA384;
      status = sl_se_hash_sha384_multipart_starts(&(operation->streaming_contexts.sha384_context),
                                                  &ephemeral_se_ctx);
      break;
#endif // PSA_WANT_ALG_SHA_384
#if defined(PSA_WANT_ALG_SHA_512)
    case PSA_ALG_SHA_512:
      operation->hash_type = SL_SE_HASH_SHA512;
      status = sl_se_hash_sha512_multipart_starts(&(operation->streaming_contexts.sha512_context),
                                                  &ephemeral_se_ctx);
      break;
#endif // PSA_WANT_ALG_SHA_512
#endif // VAULT
    default:
      return PSA_ERROR_NOT_SUPPORTED;
  }

  if (status == SL_STATUS_OK) {
    return PSA_SUCCESS;
  } else {
    return PSA_ERROR_HARDWARE_FAILURE;
  }

#else // PSA_WANT_ALG_SHA_*

  (void) operation;
  (void) alg;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_SHA_*
}

psa_status_t sli_se_transparent_hash_update(sli_se_transparent_hash_operation_t *operation,
                                            const uint8_t *input,
                                            size_t input_length)
{
#if defined(PSA_WANT_ALG_SHA_1)                                        \
  || defined(PSA_WANT_ALG_SHA_224)                                     \
  || defined(PSA_WANT_ALG_SHA_256)                                     \
  || ((defined(PSA_WANT_ALG_SHA_384) || defined(PSA_WANT_ALG_SHA_512)) \
  && (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT))

  if (operation == NULL
      || (input == NULL && input_length > 0)) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // create ephemeral contexts
  sl_se_command_context_t ephemeral_se_ctx;
  sl_status_t status = sl_se_init_command_context(&ephemeral_se_ctx);
  if (status != SL_STATUS_OK) {
    return PSA_ERROR_HARDWARE_FAILURE;
  }

  status = sl_se_hash_multipart_update((void*)&(operation->streaming_contexts), &ephemeral_se_ctx, input, input_length);

  if (status == SL_STATUS_OK) {
    return PSA_SUCCESS;
  } else {
    return PSA_ERROR_HARDWARE_FAILURE;
  }

#else // PSA_WANT_ALG_SHA_*

  (void) operation;
  (void) input;
  (void) input_length;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_SHA_*
}

psa_status_t sli_se_transparent_hash_finish(sli_se_transparent_hash_operation_t *operation,
                                            uint8_t *hash,
                                            size_t hash_size,
                                            size_t *hash_length)
{
#if defined(PSA_WANT_ALG_SHA_1)                                        \
  || defined(PSA_WANT_ALG_SHA_224)                                     \
  || defined(PSA_WANT_ALG_SHA_256)                                     \
  || ((defined(PSA_WANT_ALG_SHA_384) || defined(PSA_WANT_ALG_SHA_512)) \
  && (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT))

  if (operation == NULL
      || (hash == NULL && hash_size > 0)
      || hash_length == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // create ephemeral contexts
  sl_se_command_context_t ephemeral_se_ctx;
  sl_status_t status = sl_se_init_command_context(&ephemeral_se_ctx);
  if (status != SL_STATUS_OK) {
    return PSA_ERROR_HARDWARE_FAILURE;
  }

  status = sl_se_hash_multipart_finish((void*)&(operation->streaming_contexts), &ephemeral_se_ctx, hash, hash_size);

  // reset context
  memset(&operation->streaming_contexts, 0, sizeof(operation->streaming_contexts));

  if (status == SL_STATUS_OK) {
    switch (operation->hash_type) {
#if defined(PSA_WANT_ALG_SHA_1)
      case SL_SE_HASH_SHA1:
        *hash_length = 20;
        break;
#endif // PSA_WANT_ALG_SHA_1
#if defined(PSA_WANT_ALG_SHA_224)
      case SL_SE_HASH_SHA224:
        *hash_length = 28;
        break;
#endif // PSA_WANT_ALG_SHA_224
#if defined(PSA_WANT_ALG_SHA_256)
      case SL_SE_HASH_SHA256:
        *hash_length = 32;
        break;
#endif // PSA_WANT_ALG_SHA_256
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
#if defined(PSA_WANT_ALG_SHA_384)
      case SL_SE_HASH_SHA384:
        *hash_length = 48;
        break;
#endif // PSA_WANT_ALG_SHA_384
#if defined(PSA_WANT_ALG_SHA_512)
      case SL_SE_HASH_SHA512:
        *hash_length = 64;
        break;
#endif // PSA_WANT_ALG_SHA_512
#endif // VAULT

      default:
        return PSA_ERROR_BAD_STATE;
    }
    return PSA_SUCCESS;
  } else if ( status == SL_STATUS_INVALID_PARAMETER) {
    return PSA_ERROR_BUFFER_TOO_SMALL;
  } else {
    return PSA_ERROR_HARDWARE_FAILURE;
  }

#else // PSA_WANT_ALG_SHA_*

  (void) operation;
  (void) hash;
  (void) hash_size;
  (void) hash_length;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_SHA_*
}

psa_status_t sli_se_transparent_hash_abort(sli_se_transparent_hash_operation_t *operation)
{
#if defined(PSA_WANT_ALG_SHA_1)                                        \
  || defined(PSA_WANT_ALG_SHA_224)                                     \
  || defined(PSA_WANT_ALG_SHA_256)                                     \
  || ((defined(PSA_WANT_ALG_SHA_384) || defined(PSA_WANT_ALG_SHA_512)) \
  && (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT))

  if (operation != NULL) {
    // Accelerator does not keep state, so just zero out the context and we're good
    memset(operation, 0, sizeof(sli_se_transparent_hash_operation_t));
  }

  return PSA_SUCCESS;

#else // PSA_WANT_ALG_SHA_*

  (void) operation;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_SHA_*
}

psa_status_t sli_se_transparent_hash_compute(psa_algorithm_t alg,
                                             const uint8_t *input,
                                             size_t input_length,
                                             uint8_t *hash,
                                             size_t hash_size,
                                             size_t *hash_length)
{
#if defined(PSA_WANT_ALG_SHA_1)                                        \
  || defined(PSA_WANT_ALG_SHA_224)                                     \
  || defined(PSA_WANT_ALG_SHA_256)                                     \
  || ((defined(PSA_WANT_ALG_SHA_384) || defined(PSA_WANT_ALG_SHA_512)) \
  && (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT))

  if ((input == NULL && input_length > 0)
      || (hash == NULL && hash_size > 0)
      || hash_length == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  sl_se_hash_type_t hash_type;
  sl_se_command_context_t ephemeral_se_ctx;

  switch (alg) {
#if defined(PSA_WANT_ALG_SHA_1)
    case PSA_ALG_SHA_1:
      hash_type = SL_SE_HASH_SHA1;
      *hash_length = 20;
      break;
#endif // PSA_WANT_ALG_SHA_1
#if defined(PSA_WANT_ALG_SHA_224)
    case PSA_ALG_SHA_224:
      hash_type = SL_SE_HASH_SHA224;
      *hash_length = 28;
      break;
#endif // PSA_WANT_ALG_SHA_224
#if defined(PSA_WANT_ALG_SHA_256)
    case PSA_ALG_SHA_256:
      hash_type = SL_SE_HASH_SHA256;
      *hash_length = 32;
      break;
#endif // PSA_WANT_ALG_SHA_256
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT) || defined(DOXYGEN)
#if defined(PSA_WANT_ALG_SHA_384)
    case PSA_ALG_SHA_384:
      hash_type = SL_SE_HASH_SHA384;
      *hash_length = 48;
      break;
#endif // PSA_WANT_ALG_SHA_384
#if defined(PSA_WANT_ALG_SHA_512)
    case PSA_ALG_SHA_512:
      hash_type = SL_SE_HASH_SHA512;
      *hash_length = 64;
      break;
#endif // PSA_WANT_ALG_SHA_512
#endif // VAULT
    default:
      return PSA_ERROR_NOT_SUPPORTED;
  }

  if (hash_size < *hash_length) {
    *hash_length = 0;
    return PSA_ERROR_BUFFER_TOO_SMALL;
  }

  sl_status_t status = sl_se_init_command_context(&ephemeral_se_ctx);
  if (status != SL_STATUS_OK) {
    return PSA_ERROR_HARDWARE_FAILURE;
  }

  status = sl_se_hash(&ephemeral_se_ctx,
                      hash_type,
                      input,
                      input_length,
                      hash,
                      hash_size);

  if (status == SL_STATUS_OK) {
    return PSA_SUCCESS;
  } else {
    *hash_length = 0;
    return PSA_ERROR_HARDWARE_FAILURE;
  }

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

psa_status_t sli_se_transparent_hash_clone(const sli_se_transparent_hash_operation_t *source_operation,
                                           sli_se_transparent_hash_operation_t *target_operation)
{
#if defined(PSA_WANT_ALG_SHA_1)                                        \
  || defined(PSA_WANT_ALG_SHA_224)                                     \
  || defined(PSA_WANT_ALG_SHA_256)                                     \
  || ((defined(PSA_WANT_ALG_SHA_384) || defined(PSA_WANT_ALG_SHA_512)) \
  && (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT))

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

  (void) source_operation;
  (void) target_operation;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_SHA_*
}

#endif // defined(SEMAILBOX_PRESENT)
