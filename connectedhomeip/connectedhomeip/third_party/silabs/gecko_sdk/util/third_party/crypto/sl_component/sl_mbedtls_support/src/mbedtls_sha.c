/***************************************************************************//**
 * @file
 * @brief SHA-1, SHA-256 and SHA-512 mbedTLS plugin on top of PSA accelerators.
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

/**
 * This file includes an alternative implementation of the SHA functionality in
 * mbed TLS' APIs, using the accelerators incorporated in devices from Silicon Labs.
 *
 * This alternative implementation calls the PSA Crypto drivers provided
 * by Silicon Labs. For details on these drivers, see \ref sl_psa_drivers.
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if (defined(MBEDTLS_SHA256_ALT) && defined(MBEDTLS_SHA256_C)) \
  || (defined(MBEDTLS_SHA1_ALT) && defined(MBEDTLS_SHA1_C))    \
  || (defined(MBEDTLS_SHA512_ALT) && defined(MBEDTLS_SHA512_C))

#include "em_device.h"

#if defined(SEMAILBOX_PRESENT)
#include "sli_se_transparent_functions.h"
#define HASH_IMPLEMENTATION_PRESENT
#define HASH_SETUP_FCT      sli_se_transparent_hash_setup
#define HASH_UPDATE_FCT     sli_se_transparent_hash_update
#define HASH_FINISH_FCT     sli_se_transparent_hash_finish
#define HASH_ABORT_FCT      sli_se_transparent_hash_abort
#define HASH_ONESHOT_FCT    sli_se_transparent_hash_compute
#elif defined(CRYPTO_PRESENT)
#include "sli_crypto_transparent_functions.h"
#define HASH_IMPLEMENTATION_PRESENT
#define HASH_SETUP_FCT      sli_crypto_transparent_hash_setup
#define HASH_UPDATE_FCT     sli_crypto_transparent_hash_update
#define HASH_FINISH_FCT     sli_crypto_transparent_hash_finish
#define HASH_ABORT_FCT      sli_crypto_transparent_hash_abort
#define HASH_ONESHOT_FCT    sli_crypto_transparent_hash_compute
#elif defined(CRYPTOACC_PRESENT)
#include "sli_cryptoacc_transparent_functions.h"
#define HASH_IMPLEMENTATION_PRESENT
#define HASH_SETUP_FCT      sli_cryptoacc_transparent_hash_setup
#define HASH_UPDATE_FCT     sli_cryptoacc_transparent_hash_update
#define HASH_FINISH_FCT     sli_cryptoacc_transparent_hash_finish
#define HASH_ABORT_FCT      sli_cryptoacc_transparent_hash_abort
#define HASH_ONESHOT_FCT    sli_cryptoacc_transparent_hash_compute
#endif

#include "mbedtls/error.h"
#include "mbedtls/platform.h"
#include "mbedtls/platform_util.h"

#if defined(MBEDTLS_SHA1_ALT) && defined(MBEDTLS_SHA1_C)
#include "mbedtls/sha1.h"
#define SHA1_VALIDATE_RET(cond) \
  MBEDTLS_INTERNAL_VALIDATE_RET(cond, MBEDTLS_ERR_SHA1_BAD_INPUT_DATA)
#define SHA1_VALIDATE(cond)  MBEDTLS_INTERNAL_VALIDATE(cond)
#endif /* SHA1 acceleration active */

#if defined(MBEDTLS_SHA256_ALT) && defined(MBEDTLS_SHA256_C)
#include "mbedtls/sha256.h"
#define SHA256_VALIDATE_RET(cond) \
  MBEDTLS_INTERNAL_VALIDATE_RET(cond, MBEDTLS_ERR_SHA256_BAD_INPUT_DATA)
#define SHA256_VALIDATE(cond)  MBEDTLS_INTERNAL_VALIDATE(cond)
#endif /* SHA256 acceleration active */

#if defined(MBEDTLS_SHA512_ALT) && defined(MBEDTLS_SHA512_C)
#include "mbedtls/sha512.h"
#define SHA512_VALIDATE_RET(cond) \
  MBEDTLS_INTERNAL_VALIDATE_RET(cond, MBEDTLS_ERR_SHA512_BAD_INPUT_DATA)
#define SHA512_VALIDATE(cond)  MBEDTLS_INTERNAL_VALIDATE(cond)
#endif /* SHA512 acceleration active */

#if defined(HASH_IMPLEMENTATION_PRESENT)
static int psa_status_to_mbedtls(psa_status_t status, psa_algorithm_t alg)
{
  switch ( status ) {
    case PSA_SUCCESS:
      return 0;
    case PSA_ERROR_HARDWARE_FAILURE:
      return MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    case PSA_ERROR_NOT_SUPPORTED:
      return MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED;
    case PSA_ERROR_INVALID_ARGUMENT:
      switch ( alg ) {
#if defined(MBEDTLS_SHA1_ALT) && defined(MBEDTLS_SHA1_C)
        case PSA_ALG_SHA_1:
          return MBEDTLS_ERR_SHA1_BAD_INPUT_DATA;
#endif
#if defined(MBEDTLS_SHA256_ALT) && defined(MBEDTLS_SHA256_C)
        case PSA_ALG_SHA_256:
          return MBEDTLS_ERR_SHA256_BAD_INPUT_DATA;
#endif
#if defined(MBEDTLS_SHA512_ALT) && defined(MBEDTLS_SHA512_C)
        case PSA_ALG_SHA_512:
          return MBEDTLS_ERR_SHA512_BAD_INPUT_DATA;
#endif
        default:
          return MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
      }
    default:
      return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
  }
}

#if defined(MBEDTLS_SHA512_ALT) && (defined(MBEDTLS_SHA384_C) || defined(MBEDTLS_SHA512_C))

void mbedtls_sha512_init(mbedtls_sha512_context *ctx)
{
  SHA512_VALIDATE(ctx != NULL);

  HASH_ABORT_FCT(ctx);
}

void mbedtls_sha512_free(mbedtls_sha512_context *ctx)
{
  HASH_ABORT_FCT(ctx);
}

void mbedtls_sha512_clone(mbedtls_sha512_context *dst,
                          const mbedtls_sha512_context *src)
{
  MBEDTLS_INTERNAL_VALIDATE(dst != NULL);
  MBEDTLS_INTERNAL_VALIDATE(src != NULL);

  *dst = *src;
}

int mbedtls_sha512_starts(mbedtls_sha512_context *ctx, int is384)
{
  return psa_status_to_mbedtls(HASH_SETUP_FCT(ctx, is384 ? PSA_ALG_SHA_384 : PSA_ALG_SHA_512), PSA_ALG_SHA_512);
}

int mbedtls_sha512_update(mbedtls_sha512_context *ctx, const unsigned char *input,
                          size_t ilen)
{
  return psa_status_to_mbedtls(HASH_UPDATE_FCT(ctx, input, ilen), PSA_ALG_SHA_512);
}

int mbedtls_internal_sha512_process(mbedtls_sha512_context *ctx, const unsigned char data[128])
{
  return psa_status_to_mbedtls(HASH_UPDATE_FCT(ctx, data, 128), PSA_ALG_SHA_512);
}

int mbedtls_sha512_finish(mbedtls_sha512_context *ctx, unsigned char output[64])
{
  size_t out_length = 0;
  return psa_status_to_mbedtls(HASH_FINISH_FCT(ctx, output, 64, &out_length), PSA_ALG_SHA_512);
}
#endif /* SHA512 acceleration active */

#if defined(MBEDTLS_SHA256_ALT) && (defined(MBEDTLS_SHA256_C) || defined(MBEDTLS_SHA224_C))
void mbedtls_sha256_init(mbedtls_sha256_context *ctx)
{
  SHA256_VALIDATE(ctx != NULL);

  HASH_ABORT_FCT((void *)ctx);
}

void mbedtls_sha256_free(mbedtls_sha256_context *ctx)
{
  HASH_ABORT_FCT((void *)ctx);
}

void mbedtls_sha256_clone(mbedtls_sha256_context *dst,
                          const mbedtls_sha256_context *src)
{
  MBEDTLS_INTERNAL_VALIDATE(dst != NULL);
  MBEDTLS_INTERNAL_VALIDATE(src != NULL);

  *dst = *src;
}

int mbedtls_sha256_starts(mbedtls_sha256_context *ctx, int is224)
{
  SHA256_VALIDATE_RET(ctx != NULL);
  SHA256_VALIDATE_RET(is224 == 0 || is224 == 1);

  return psa_status_to_mbedtls(HASH_SETUP_FCT((void *)ctx, is224 ? PSA_ALG_SHA_224 : PSA_ALG_SHA_256), PSA_ALG_SHA_256);
}

int mbedtls_sha256_update(mbedtls_sha256_context *ctx, const unsigned char *input,
                          size_t ilen)
{
  SHA256_VALIDATE_RET(ctx != NULL);
  SHA256_VALIDATE_RET(ilen == 0 || input != NULL);

  return psa_status_to_mbedtls(HASH_UPDATE_FCT((void *)ctx, input, ilen), PSA_ALG_SHA_256);
}

int mbedtls_internal_sha256_process(mbedtls_sha256_context *ctx, const unsigned char data[64])
{
  SHA256_VALIDATE_RET(ctx != NULL);
  SHA256_VALIDATE_RET((const unsigned char *)data != NULL);

  return psa_status_to_mbedtls(HASH_UPDATE_FCT((void *)ctx, data, 64), PSA_ALG_SHA_256);
}

int mbedtls_sha256_finish(mbedtls_sha256_context *ctx, unsigned char *output)
{
  SHA256_VALIDATE_RET(ctx != NULL);
  SHA256_VALIDATE_RET((unsigned char *)output != NULL);

  size_t out_length = 0;
  return psa_status_to_mbedtls(HASH_FINISH_FCT((void *)ctx, output, 32, &out_length), PSA_ALG_SHA_256);
}
#endif /* SHA256 acceleration active */

#if defined(MBEDTLS_SHA1_ALT) && defined(MBEDTLS_SHA1_C)

void mbedtls_sha1_init(mbedtls_sha1_context *ctx)
{
  SHA1_VALIDATE(ctx != NULL);

  HASH_ABORT_FCT((void *)ctx);
}

void mbedtls_sha1_free(mbedtls_sha1_context *ctx)
{
  HASH_ABORT_FCT((void *)ctx);
}

void mbedtls_sha1_clone(mbedtls_sha1_context *dst,
                        const mbedtls_sha1_context *src)
{
  MBEDTLS_INTERNAL_VALIDATE(dst != NULL);
  MBEDTLS_INTERNAL_VALIDATE(src != NULL);

  *dst = *src;
}

int mbedtls_sha1_starts(mbedtls_sha1_context *ctx)
{
  SHA1_VALIDATE_RET(ctx != NULL);

  return psa_status_to_mbedtls(HASH_SETUP_FCT((void *)ctx, PSA_ALG_SHA_1), PSA_ALG_SHA_1);
}

int mbedtls_sha1_update(mbedtls_sha1_context *ctx, const unsigned char *input, size_t ilen)
{
  SHA1_VALIDATE_RET(ctx != NULL);
  SHA1_VALIDATE_RET(ilen == 0 || input != NULL);

  return psa_status_to_mbedtls(HASH_UPDATE_FCT((void *)ctx, input, ilen), PSA_ALG_SHA_1);
}

int mbedtls_internal_sha1_process(mbedtls_sha1_context *ctx, const unsigned char data[64])
{
  SHA1_VALIDATE_RET(ctx != NULL);
  SHA1_VALIDATE_RET((const unsigned char *)data != NULL);

  return psa_status_to_mbedtls(HASH_UPDATE_FCT((void *)ctx, data, 64), PSA_ALG_SHA_1);
}

int mbedtls_sha1_finish(mbedtls_sha1_context *ctx, unsigned char output[20])
{
  SHA1_VALIDATE_RET(ctx != NULL);
  SHA1_VALIDATE_RET((unsigned char *)output != NULL);

  size_t out_length = 0;
  return psa_status_to_mbedtls(HASH_FINISH_FCT((void *)ctx, output, 20, &out_length), PSA_ALG_SHA_1);
}
#endif /* SHA1 acceleration active */

#endif /* HASH_IMPLEMENTATION_PRESENT */
#endif /* (SHA1 or SHA256 or SHA512) acceleration active */
