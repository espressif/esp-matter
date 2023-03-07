/***************************************************************************//**
 * @file
 * @brief AES-CMAC abstraction based on CRYPTOACC
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
 * This file includes alternative plugin implementations of various
 * functions in gmac.c using the cryptographic accelerator incorporated
 * in Series-2 devices with CRYPTOACC from Silicon Laboratories.
 */

/*
 * http://csrc.nist.gov/publications/nistpubs/800-38D/SP-800-38D.pdf
 *
 * See also:
 * [MGV] http://csrc.nist.gov/groups/ST/toolkit/BCM/documents/proposedmodes/gcm/gcm-revised-spec.pdf
 *
 * We use the algorithm described as Shoup's method with 4-bit tables in
 * [MGV] 4.1, pp. 12-13, to enhance speed without using too much memory.
 */

#include "em_device.h"

#if defined(CRYPTOACC_PRESENT)

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_GCM_ALT) && defined(MBEDTLS_GCM_C)
#include "cryptoacc_management.h"
#include "mbedtls/gcm.h"
#include "mbedtls/aes.h"
#include "mbedtls/platform.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/error.h"
#include "sx_aes.h"
#include "sx_math.h"
#include "sx_errors.h"
#include "cryptolib_def.h"
#include <string.h>

/* Parameter validation macros */
#define GCM_VALIDATE_RET(cond) \
  MBEDTLS_INTERNAL_VALIDATE_RET(cond, MBEDTLS_ERR_GCM_BAD_INPUT)
#define GCM_VALIDATE(cond) \
  MBEDTLS_INTERNAL_VALIDATE(cond)

/* Implementation that should never be optimized out by the compiler */
static void mbedtls_zeroize(void *v, size_t n)
{
  volatile unsigned char *p = v; while ( n-- ) *p++ = 0;
}

static int sli_validate_gcm_params(size_t tag_len,
                                   size_t iv_len,
                                   size_t add_len)
{
  // NOTE: tag lengths != 16 byte are only supported as of SE FW v1.2.0.
  //   Earlier firmware versions will return an error trying to verify non-16-byte
  //   tags using this function.
  if ( tag_len < 4 || tag_len > 16 || iv_len == 0 ) {
    return (MBEDTLS_ERR_GCM_BAD_INPUT);
  }

  /* AD are limited to 2^64 bits, so 2^61 bytes. Since the length of AAD is
   * limited by the mbedtls API to a size_t, length checking only needs to be
   * done on 64-bit platforms. */
#if SIZE_MAX > 0xFFFFFFFFUL
  if (add_len >> 61 != 0) {
    return MBEDTLS_ERR_GCM_BAD_INPUT;
  }
#else
  (void) add_len;
#endif /* 64-bit size_t */

  /* Library does not support non-12-byte IVs */
  if (iv_len != AES_IV_GCM_SIZE) {
    return MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED;
  }

  return 0;
}

/*
 * Initialize a context
 */
void mbedtls_gcm_init(mbedtls_gcm_context *ctx)
{
  GCM_VALIDATE(ctx != NULL);

  memset(ctx, 0, sizeof(mbedtls_gcm_context) );
}

// Set key
int mbedtls_gcm_setkey(mbedtls_gcm_context *ctx,
                       mbedtls_cipher_id_t cipher,
                       const unsigned char *key,
                       unsigned int keybits)
{
  (void) cipher;

  GCM_VALIDATE_RET(ctx != NULL);
  GCM_VALIDATE_RET(key != NULL);
  GCM_VALIDATE_RET(cipher == MBEDTLS_CIPHER_ID_AES);
  GCM_VALIDATE_RET(keybits == 128 || keybits == 192 || keybits == 256);

  /* Store key in gcm context */
  ctx->keybits = keybits;
  memcpy(ctx->key, key, keybits / 8);

  return 0;
}

int mbedtls_gcm_starts(mbedtls_gcm_context *ctx,
                       int mode,
                       const unsigned char *iv,
                       size_t iv_len)
{
  GCM_VALIDATE_RET(ctx != NULL);
  GCM_VALIDATE_RET(iv != NULL);

  int status = sli_validate_gcm_params(16, iv_len, 0);
  if (status) {
    return status;
  }

  /* Store input in context data structure. */
  ctx->dir = mode == MBEDTLS_AES_ENCRYPT ? SLI_GCM_ENC : SLI_GCM_DEC;
  ctx->add_len    = 0;
  ctx->len        = 0;

  memcpy(ctx->sx_ctx, iv, AES_IV_GCM_SIZE);
  return 0;
}

int mbedtls_gcm_update_ad(mbedtls_gcm_context *ctx,
                          const unsigned char *add,
                          size_t add_len)
{
  uint32_t sx_ret;
  block_t  key;
  block_t  aad;
  block_t  nonce;
  block_t  hw_ctx;
  block_t  dummy = NULL_blk;

  GCM_VALIDATE_RET(ctx != NULL);
  GCM_VALIDATE_RET(add_len == 0 || add != NULL);
  int status = sli_validate_gcm_params(16, 12, add_len);
  if (status) {
    return status;
  }

  if (add_len == 0) {
    return 0;
  }

  if (ctx->add_len > 0) {
    // This accelerator does not support adding AD in chunks
    return MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED;
  }

  ctx->add_len = add_len;

  key = block_t_convert(ctx->key, ctx->keybits / 8);
  nonce = block_t_convert(ctx->sx_ctx, AES_IV_GCM_SIZE);
  aad = block_t_convert(add, add_len);
  hw_ctx = block_t_convert(ctx->sx_ctx, AES_CTX_xCM_SIZE);

  status = cryptoacc_management_acquire();
  if (status != 0) {
    return status;
  }
  /* Execute GCM operation */
  if (ctx->dir == SLI_GCM_ENC) {
    sx_ret = sx_aes_gcm_encrypt_init((const block_t *)&key, (const block_t *)&dummy, &dummy,
                                     (const block_t *)&nonce, &hw_ctx, (const block_t *)&aad);
  } else {
    sx_ret = sx_aes_gcm_decrypt_init((const block_t *)&key, (const block_t *)&dummy, &dummy,
                                     (const block_t *)&nonce, &hw_ctx, (const block_t *)&aad);
  }
  status = cryptoacc_management_release();

  if (sx_ret == CRYPTOLIB_SUCCESS) {
    return status;
  } else {
    return MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
  }
}

int mbedtls_gcm_update(mbedtls_gcm_context *ctx,
                       const unsigned char *input, size_t input_length,
                       unsigned char *output, size_t output_size,
                       size_t *output_length)
{
  int status;
  uint32_t sx_ret;
  block_t  data_in;
  block_t  data_out;
  block_t  key;
  block_t  nonce;
  block_t  hw_ctx;
  block_t  dummy = NULL_blk;

  GCM_VALIDATE_RET(ctx != NULL);
  GCM_VALIDATE_RET(input_length == 0 || input != NULL);
  GCM_VALIDATE_RET(input_length == 0 || output != NULL);

  *output_length = 0;

  if (input_length > output_size) {
    return MBEDTLS_ERR_GCM_BAD_INPUT;
  }

  if (input_length == 0) {
    return 0;
  }

  /* Total length is restricted to 2^39 - 256 bits, ie 2^36 - 2^5 bytes
   * Also check for possible overflow */
  if ( ctx->len + input_length < ctx->len
       || (uint64_t) ctx->len + input_length > 0xFFFFFFFE0ull ) {
    return(MBEDTLS_ERR_GCM_BAD_INPUT);
  }

  key = block_t_convert(ctx->key, ctx->keybits / 8);
  data_in = block_t_convert(input, input_length);
  data_out = block_t_convert(output, input_length);
  hw_ctx = block_t_convert(ctx->sx_ctx, AES_CTX_xCM_SIZE);

  if (ctx->add_len == 0 && ctx->len == 0) {
    /* If there were no additional authentcation data then
       mbedtls_gcm_starts did not 'CTX_BEGIN' the GCM operation
       in the CRYPTOACC, so we need to 'CTX_BEGIN' now. */
    nonce = block_t_convert(ctx->sx_ctx, AES_IV_GCM_SIZE);

    status = cryptoacc_management_acquire();
    if (status != 0) {
      return status;
    }
    /* Execute GCM operation */
    if (ctx->dir == SLI_GCM_ENC) {
      sx_ret = sx_aes_gcm_encrypt_init((const block_t *)&key, (const block_t *)&data_in, &data_out,
                                       (const block_t *)&nonce, &hw_ctx, (const block_t *)&dummy);
    } else {
      sx_ret = sx_aes_gcm_decrypt_init((const block_t *)&key, (const block_t *)&data_in, &data_out,
                                       (const block_t *)&nonce, &hw_ctx, (const block_t *)&dummy);
    }
    status = cryptoacc_management_release();
  } else {
    status = cryptoacc_management_acquire();
    if (status != 0) {
      return status;
    }
    /* Execute GCM operation */
    if (ctx->dir == SLI_GCM_ENC) {
      sx_ret = sx_aes_gcm_encrypt_update((const block_t *)&key, (const block_t *)&data_in, &data_out,
                                         (const block_t *)&hw_ctx, &hw_ctx);
    } else {
      sx_ret = sx_aes_gcm_decrypt_update((const block_t *)&key, (const block_t *)&data_in, &data_out,
                                         (const block_t *)&hw_ctx, &hw_ctx);
    }
    status = cryptoacc_management_release();
  }

  ctx->len += input_length;

  if (sx_ret == CRYPTOLIB_SUCCESS) {
    *output_length = input_length;
    return status;
  } else {
    memset(output, 0, output_size);
    return MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
  }
}

int mbedtls_gcm_finish(mbedtls_gcm_context *ctx,
                       unsigned char *output, size_t output_size,
                       size_t *output_length,
                       unsigned char *tag,
                       size_t tag_len)
{
  // Voiding these because our implementation does not support
  // partial-block input (i.e. passing a partial block to
  // update() will have caused the operation to finish already)
  (void) output;
  (void) output_size;
  *output_length = 0;

  int status;
  uint32_t sx_ret;
  block_t key;
  block_t _tag;
  uint8_t tagbuf[16];
  uint8_t lena_lenc[16];
  block_t lena_lenc_blk = NULL_blk;
  block_t dummy = NULL_blk;
  block_t hw_ctx;

  GCM_VALIDATE_RET(ctx != NULL);
  GCM_VALIDATE_RET(tag != NULL);

  status = sli_validate_gcm_params(tag_len, 12, 16);
  if (status) {
    return status;
  }

  if (ctx->add_len == 0 && ctx->len == 0) {
    /* If there were no data and additional authentcation data then
       mbedtls_gcm_starts and update did not start the GCM operation,
       so we need to run the whole GCM now. */
    return mbedtls_gcm_crypt_and_tag(ctx,
                                     ctx->dir == SLI_GCM_ENC ? MBEDTLS_GCM_ENCRYPT
                                     : MBEDTLS_GCM_DECRYPT,
                                     0, ctx->sx_ctx, AES_IV_GCM_SIZE, 0, 0, 0, 0,
                                     tag_len, tag);
  } else {
    key = block_t_convert(ctx->key, ctx->keybits / 8);
    _tag = block_t_convert(tagbuf, 16);     // CRYPTOACC supports only 128bits tags
    hw_ctx = block_t_convert(ctx->sx_ctx, AES_CTX_xCM_SIZE);

    // build lena_lenc block as big endian byte array
    sx_math_u64_to_u8array(ctx->add_len << 3, &lena_lenc[0], sx_big_endian);
    sx_math_u64_to_u8array(ctx->len << 3, &lena_lenc[8], sx_big_endian);
    lena_lenc_blk = block_t_convert(lena_lenc, 16);
    status = cryptoacc_management_acquire();
    if (status != 0) {
      return status;
    }
    if (ctx->dir == SLI_GCM_ENC) {
      sx_ret = sx_aes_gcm_encrypt_final((const block_t *)&key, (const block_t *)&dummy, &dummy,
                                        (const block_t *)&hw_ctx, &_tag, (const block_t *)&lena_lenc_blk);
    } else {
      sx_ret = sx_aes_gcm_decrypt_final((const block_t *)&key, (const block_t *)&dummy, &dummy,
                                        (const block_t *)&hw_ctx, &_tag, (const block_t *)&lena_lenc_blk);
    }
    status = cryptoacc_management_release();

    if (sx_ret != CRYPTOLIB_SUCCESS) {
      return(MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED);
    }

    memcpy(tag, tagbuf, tag_len);
    return(status);
  }
}

int mbedtls_gcm_crypt_and_tag(mbedtls_gcm_context *ctx,
                              int mode,
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
  int status;
  uint32_t sx_ret;
  sli_gcm_mode_t dir = mode == MBEDTLS_AES_ENCRYPT ? SLI_GCM_ENC : SLI_GCM_DEC;
  block_t key;
  block_t aad;
  block_t _tag;
  block_t nonce;
  block_t data_in;
  block_t data_out;
  uint8_t tagbuf[16];

  GCM_VALIDATE_RET(ctx != NULL);
  GCM_VALIDATE_RET(iv != NULL);
  GCM_VALIDATE_RET(add_len == 0 || add != NULL);
  GCM_VALIDATE_RET(length == 0 || input != NULL);
  GCM_VALIDATE_RET(length == 0 || output != NULL);
  GCM_VALIDATE_RET(tag != NULL);

  status = sli_validate_gcm_params(tag_len, iv_len, add_len);
  if (status) {
    return status;
  }

  key = block_t_convert(ctx->key, ctx->keybits / 8);
  nonce = block_t_convert(iv, iv_len);
  aad = block_t_convert(add, add_len);
  _tag = block_t_convert(tagbuf, sizeof(tagbuf));   // CRYPTOACC supports only 128bits tags
  data_in = block_t_convert(input, length);
  data_out = block_t_convert(output, length);

  status = cryptoacc_management_acquire();
  if (status != 0) {
    return status;
  }
  /* Execute GCM operation */
  if (dir == SLI_GCM_ENC) {
    sx_ret = sx_aes_gcm_encrypt((const block_t *)&key, (const block_t *)&data_in, &data_out,
                                (const block_t *)&nonce, &_tag, (const block_t *)&aad);
  } else {
    sx_ret = sx_aes_gcm_decrypt((const block_t *)&key, (const block_t *)&data_in, &data_out,
                                (const block_t *)&nonce, &_tag, (const block_t *)&aad);
  }
  status = cryptoacc_management_release();

  if (sx_ret != CRYPTOLIB_SUCCESS) {
    mbedtls_zeroize(output, length);
    return(MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED);
  }

  memcpy(tag, tagbuf, tag_len);
  return(status);
}

int mbedtls_gcm_auth_decrypt(mbedtls_gcm_context *ctx,
                             size_t length,
                             const unsigned char *iv,
                             size_t iv_len,
                             const unsigned char *add,
                             size_t add_len,
                             const unsigned char *tag,
                             size_t tag_len,
                             const unsigned char *input,
                             unsigned char *output)
{
  int status;
  uint32_t sx_ret;
  block_t key;
  block_t aad;
  block_t _tag;
  block_t nonce;
  block_t data_in;
  block_t data_out;
  uint8_t tagbuf[16];

  /* Check input parameters. */
  GCM_VALIDATE_RET(ctx != NULL);
  GCM_VALIDATE_RET(iv != NULL);
  GCM_VALIDATE_RET(add_len == 0 || add != NULL);
  GCM_VALIDATE_RET(tag != NULL);
  GCM_VALIDATE_RET(length == 0 || input != NULL);
  GCM_VALIDATE_RET(length == 0 || output != NULL);

  status = sli_validate_gcm_params(tag_len, iv_len, add_len);
  if (status) {
    return status;
  }

  key = block_t_convert(ctx->key, ctx->keybits / 8);
  nonce = block_t_convert(iv, iv_len);
  aad = block_t_convert(add, add_len);
  _tag = block_t_convert(tagbuf, sizeof(tagbuf));   // CRYPTOACC supports only 128bits tags
  data_in = block_t_convert(input, length);
  data_out = block_t_convert(output, length);

  status = cryptoacc_management_acquire();
  if (status != 0) {
    return status;
  }
  /* Execute GCM operation */
  sx_ret = sx_aes_gcm_decrypt((const block_t *)&key, (const block_t *)&data_in, &data_out,
                              (const block_t *)&nonce, &_tag, (const block_t *)&aad);
  status = cryptoacc_management_release();

  if (sx_ret == CRYPTOLIB_SUCCESS) {
    if (memcmp_time_cst((uint8_t*)tag, tagbuf, tag_len) == 0) {
      return(status);
    } else {
      mbedtls_zeroize(output, length);
      return(MBEDTLS_ERR_GCM_AUTH_FAILED);
    }
  } else {
    mbedtls_zeroize(output, length);
    return(MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED);
  }
}

void mbedtls_gcm_free(mbedtls_gcm_context *ctx)
{
  if ( ctx == NULL ) {
    return;
  }
  mbedtls_zeroize(ctx, sizeof(mbedtls_gcm_context) );
}

#endif /* MBEDTLS_GCM_ALT && MBEDTLS_GCM_C */

#endif /* CRYPTOACC_PRESENT */
