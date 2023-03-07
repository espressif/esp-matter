/***************************************************************************//**
 * @file
 * @brief AES-CCM abstraction based on PSA accelerators
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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
 * This file includes an alternative implementation of various functions in
 * ccm.c, using the accelerators incorporated in devices from Silicon Labs.
 *
 * This alternative implementation calls the PSA Crypto drivers provided
 * by Silicon Labs. For details on these drivers, see \ref sl_psa_drivers.
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_AES_C) && defined(MBEDTLS_CCM_C) && defined(MBEDTLS_CCM_ALT)

#include "mbedtls/ccm.h"
#include "mbedtls/error.h"

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdlib.h>
#define mbedtls_calloc     calloc
#define mbedtls_free       free
#if defined(MBEDTLS_SELF_TEST)
#include <stdio.h>
#define mbedtls_printf     printf
#endif /* MBEDTLS_SELF_TEST */
#endif /* MBEDTLS_PLATFORM_C */

#include "psa/crypto.h"

#include "em_device.h"

#if defined(SEMAILBOX_PRESENT)
#include "sli_se_transparent_functions.h"
#define AEAD_IMPLEMENTATION_PRESENT
#define SLI_DEVICE_HAS_AES_192
#define AEAD_ENCRYPT_TAG_FCT    sli_se_driver_aead_encrypt_tag
#define AEAD_DECRYPT_TAG_FCT    sli_se_driver_aead_decrypt_tag
#elif defined(CRYPTO_PRESENT)
#include "sli_crypto_transparent_functions.h"
#define AEAD_IMPLEMENTATION_PRESENT
#define AEAD_ENCRYPT_TAG_FCT    sli_crypto_transparent_aead_encrypt_tag
#define AEAD_DECRYPT_TAG_FCT    sli_crypto_transparent_aead_decrypt_tag
#elif defined(CRYPTOACC_PRESENT)
#include "sli_cryptoacc_transparent_functions.h"
#define AEAD_IMPLEMENTATION_PRESENT
#define SLI_DEVICE_HAS_AES_192
#define AEAD_ENCRYPT_TAG_FCT    sli_cryptoacc_transparent_aead_encrypt_tag
#define AEAD_DECRYPT_TAG_FCT    sli_cryptoacc_transparent_aead_decrypt_tag
#endif

#if defined(AEAD_IMPLEMENTATION_PRESENT)

#include <string.h>

static int psa_status_to_mbedtls(psa_status_t status)
{
  switch ( status ) {
    case PSA_SUCCESS:
      return 0;
    case PSA_ERROR_INVALID_SIGNATURE:
      return MBEDTLS_ERR_CCM_AUTH_FAILED;
    case PSA_ERROR_HARDWARE_FAILURE:
      return MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    case PSA_ERROR_NOT_SUPPORTED:
      return MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED;
    default:
      return MBEDTLS_ERR_CCM_BAD_INPUT;
  }
}

/*
 * Initialize CCM context
 */
void mbedtls_ccm_init(mbedtls_ccm_context *ctx)
{
  if ( ctx == NULL ) {
    return;
  }

  memset(ctx, 0, sizeof(mbedtls_ccm_context) );
}

/*
 * Clear CCM context
 */
void mbedtls_ccm_free(mbedtls_ccm_context *ctx)
{
  if ( ctx == NULL ) {
    return;
  }

  memset(ctx, 0, sizeof(mbedtls_ccm_context) );
}

/*
 * CCM key schedule
 */
int mbedtls_ccm_setkey(mbedtls_ccm_context *ctx,
                       mbedtls_cipher_id_t cipher,
                       const unsigned char *key,
                       unsigned int keybits)
{
  if (ctx == NULL || key == NULL) {
    return MBEDTLS_ERR_CCM_BAD_INPUT;
  }

  memset(ctx, 0, sizeof(mbedtls_ccm_context) );

  if ( cipher != MBEDTLS_CIPHER_ID_AES ) {
    return MBEDTLS_ERR_CCM_BAD_INPUT;
  }

  if ( (128UL != keybits) && (192UL != keybits) && (256UL != keybits) ) {
    /* Unsupported key size */
    return MBEDTLS_ERR_CCM_BAD_INPUT;
  }

  #if !defined(SLI_DEVICE_HAS_AES_192)
  if (192UL == keybits) {
    return MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED;
  }
  #endif

  ctx->keybits = keybits;
  memcpy(ctx->key, key, keybits / 8);

  return 0;
}

int mbedtls_ccm_encrypt_and_tag(mbedtls_ccm_context *ctx, size_t length,
                                const unsigned char *iv, size_t iv_len,
                                const unsigned char *add, size_t add_len,
                                const unsigned char *input, unsigned char *output,
                                unsigned char *tag, size_t tag_len)
{
  // 'Regular' CCM always outputs a tag of at least 4 bytes
  if (tag_len < 4) {
    return MBEDTLS_ERR_CCM_BAD_INPUT;
  }

  return mbedtls_ccm_star_encrypt_and_tag(ctx, length, iv, iv_len, add, add_len,
                                          input, output, tag, tag_len);
}

int mbedtls_ccm_auth_decrypt(mbedtls_ccm_context *ctx, size_t length,
                             const unsigned char *iv, size_t iv_len,
                             const unsigned char *add, size_t add_len,
                             const unsigned char *input, unsigned char *output,
                             const unsigned char *tag, size_t tag_len)
{
  // 'Regular' CCM always verifies a tag of at least 4 bytes
  if (tag_len < 4) {
    return MBEDTLS_ERR_CCM_BAD_INPUT;
  }

  return mbedtls_ccm_star_auth_decrypt(ctx, length, iv, iv_len, add, add_len,
                                       input, output, tag, tag_len);
}

int mbedtls_ccm_star_encrypt_and_tag(mbedtls_ccm_context *ctx, size_t length,
                                     const unsigned char *iv, size_t iv_len,
                                     const unsigned char *add, size_t add_len,
                                     const unsigned char *input, unsigned char *output,
                                     unsigned char *tag, size_t tag_len)
{
  if ( ctx == NULL || iv == NULL || iv_len == 0
       || (add_len > 0 && add == NULL) || add_len >= 0xFF00
       || (length > 0 && input == NULL) || length >= 0xFF00
       || (length > 0 && output == NULL)
       || (tag_len > 0 && tag == NULL) ) {
    return MBEDTLS_ERR_CCM_BAD_INPUT;
  }

  psa_status_t psa_status;
  psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
  psa_set_key_type(&attr, PSA_KEY_TYPE_AES);
  psa_set_key_bits(&attr, ctx->keybits);

  if (tag_len > 0) {
    psa_status = AEAD_ENCRYPT_TAG_FCT(
      &attr, ctx->key, ctx->keybits / 8,
      PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, tag_len),
      iv, iv_len,
      add, add_len,
      input, length,
      output, length, &length,
      tag, tag_len, &tag_len);
  } else {
    (void) tag;
    uint8_t dummy_tag[4];
    psa_status = AEAD_ENCRYPT_TAG_FCT(
      &attr, ctx->key, ctx->keybits / 8,
      PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, sizeof(dummy_tag)),
      iv, iv_len,
      add, add_len,
      input, length,
      output, length, &length,
      dummy_tag, sizeof(dummy_tag), &tag_len);
  }

  psa_reset_key_attributes(&attr);
  return psa_status_to_mbedtls(psa_status);
}

int mbedtls_ccm_star_auth_decrypt(mbedtls_ccm_context *ctx, size_t length,
                                  const unsigned char *iv, size_t iv_len,
                                  const unsigned char *add, size_t add_len,
                                  const unsigned char *input, unsigned char *output,
                                  const unsigned char *tag, size_t tag_len)
{
  if ( ctx == NULL || iv == NULL || iv_len == 0
       || (add_len > 0 && add == NULL) || add_len >= 0xFF00
       || (length > 0 && input == NULL) || length >= 0xFF00
       || (length > 0 && output == NULL)
       || (tag_len > 0 && tag == NULL) ) {
    return MBEDTLS_ERR_CCM_BAD_INPUT;
  }

  psa_status_t psa_status;
  psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
  psa_set_key_type(&attr, PSA_KEY_TYPE_AES);
  psa_set_key_bits(&attr, ctx->keybits);

  if (tag_len > 0) {
    psa_status = AEAD_DECRYPT_TAG_FCT(
      &attr, ctx->key, ctx->keybits / 8,
      PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, tag_len),
      iv, iv_len,
      add, add_len,
      input, length,
      tag, tag_len,
      output, length, &length);
  } else {
    // CCM(*) is symmetric in encryption/decryption of the data, so if we don't have
    // to verify a tag we can transform ciphertext to plaintext by running an
    // 'encrypt' operation and throwing away the tag.
    (void) tag;
    uint8_t dummy_tag[4];
    psa_status = AEAD_ENCRYPT_TAG_FCT(
      &attr, ctx->key, ctx->keybits / 8,
      PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, sizeof(dummy_tag)),
      iv, iv_len,
      add, add_len,
      input, length,
      output, length, &length,
      dummy_tag, sizeof(dummy_tag), &tag_len);
  }

  psa_reset_key_attributes(&attr);
  return psa_status_to_mbedtls(psa_status);
}

/* Provide stubs for linkage purposes. To be implemented when implementing
 * support for multipart AEAD in the PSA drivers, see [PSEC-3221] */
int mbedtls_ccm_starts(mbedtls_ccm_context *ctx,
                       int mode,
                       const unsigned char *iv,
                       size_t iv_len)
{
  (void) ctx;
  (void) mode;
  (void) iv;
  (void) iv_len;
  return MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED;
}

int mbedtls_ccm_set_lengths(mbedtls_ccm_context *ctx,
                            size_t total_ad_len,
                            size_t plaintext_len,
                            size_t tag_len)
{
  (void) ctx;
  (void) total_ad_len;
  (void) plaintext_len;
  (void) tag_len;
  return MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED;
}

int mbedtls_ccm_update_ad(mbedtls_ccm_context *ctx,
                          const unsigned char *ad,
                          size_t ad_len)
{
  (void) ctx;
  (void) ad;
  (void) ad_len;
  return MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED;
}

int mbedtls_ccm_update(mbedtls_ccm_context *ctx,
                       const unsigned char *input, size_t input_len,
                       unsigned char *output, size_t output_size,
                       size_t *output_len)
{
  (void) ctx;
  (void) input;
  (void) input_len;
  (void) output;
  (void) output_size;
  (void) output_len;
  return MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED;
}

int mbedtls_ccm_finish(mbedtls_ccm_context *ctx,
                       unsigned char *tag, size_t tag_len)
{
  (void) ctx;
  (void) tag;
  (void) tag_len;
  return MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED;
}

#endif /* AEAD_IMPLEMENTATION_PRESENT */

#endif /* MBEDTLS_AES_C && MBEDTLS_CCM_C && MBEDTLS_CCM_ALT */
