/***************************************************************************//**
 * @file
 * @brief AES-CMAC abstraction based on PSA accelerators
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
 * This file includes an alternative implementation of various functions in
 * cmac.c, using the accelerators incorporated in devices from Silicon Labs.
 *
 * This alternative implementation calls the PSA Crypto drivers provided
 * by Silicon Labs. For details on these drivers, see \ref sl_psa_drivers.
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined (MBEDTLS_CMAC_C) && defined(MBEDTLS_CMAC_ALT)

#include "mbedtls/cmac.h"
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
#define SLI_DEVICE_HAS_AES_192
#define MAC_IMPLEMENTATION_PRESENT
#define MAC_SETUP_EN_FCT    sli_se_transparent_mac_sign_setup
#define MAC_SETUP_DE_FCT    sli_se_transparent_mac_verify_setup
#define MAC_UPDATE_FCT      sli_se_transparent_mac_update
#define MAC_FINISH_EN_FCT   sli_se_transparent_mac_sign_finish
#define MAC_FINISH_DE_FCT   sli_se_transparent_mac_verify_finish
#define MAC_ABORT_FCT       sli_se_transparent_mac_abort
#define MAC_ONESHOT_EN_FCT  sli_se_transparent_mac_compute
#define MAC_ONESHOT_DE_FCT  sli_se_transparent_mac_verify

#if defined(RADIOAES_PRESENT)
#include "sli_protocol_crypto.h"
#endif
#elif defined(CRYPTO_PRESENT)
#include "sli_crypto_transparent_functions.h"
#define MAC_IMPLEMENTATION_PRESENT
#define MAC_SETUP_EN_FCT    sli_crypto_transparent_mac_sign_setup
#define MAC_SETUP_DE_FCT    sli_crypto_transparent_mac_verify_setup
#define MAC_UPDATE_FCT      sli_crypto_transparent_mac_update
#define MAC_FINISH_EN_FCT   sli_crypto_transparent_mac_sign_finish
#define MAC_FINISH_DE_FCT   sli_crypto_transparent_mac_verify_finish
#define MAC_ABORT_FCT       sli_crypto_transparent_mac_abort
#define MAC_ONESHOT_EN_FCT  sli_crypto_transparent_mac_compute
#define MAC_ONESHOT_DE_FCT  sli_crypto_transparent_mac_verify
#elif defined(CRYPTOACC_PRESENT)
#include "sli_cryptoacc_transparent_functions.h"
#define SLI_DEVICE_HAS_AES_192
#define MAC_IMPLEMENTATION_PRESENT
#define MAC_SETUP_EN_FCT    sli_cryptoacc_transparent_mac_sign_setup
#define MAC_SETUP_DE_FCT    sli_cryptoacc_transparent_mac_verify_setup
#define MAC_UPDATE_FCT      sli_cryptoacc_transparent_mac_update
#define MAC_FINISH_EN_FCT   sli_cryptoacc_transparent_mac_sign_finish
#define MAC_FINISH_DE_FCT   sli_cryptoacc_transparent_mac_verify_finish
#define MAC_ABORT_FCT       sli_cryptoacc_transparent_mac_abort
#define MAC_ONESHOT_EN_FCT  sli_cryptoacc_transparent_mac_compute
#define MAC_ONESHOT_DE_FCT  sli_cryptoacc_transparent_mac_verify
#endif

#if defined(MAC_IMPLEMENTATION_PRESENT)

#include <string.h>

static int psa_status_to_mbedtls(psa_status_t status)
{
  switch ( status ) {
    case PSA_SUCCESS:
      return 0;
    case PSA_ERROR_HARDWARE_FAILURE:
      return MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    case PSA_ERROR_NOT_SUPPORTED:
      return MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED;
    default:
      return MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA;
  }
}

static inline void sl_psa_set_key_type(psa_key_attributes_t *attributes,
                                       psa_key_type_t type)
{
  /* Common case: quick path */
  attributes->MBEDTLS_PRIVATE(core).MBEDTLS_PRIVATE(type) = type;
}

int mbedtls_cipher_cmac_starts(mbedtls_cipher_context_t *ctx,
                               const unsigned char *key, size_t keybits)
{
  mbedtls_cipher_type_t type;
  mbedtls_cmac_context_t *cmac_ctx;
  psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
  sl_psa_set_key_type(&attr, PSA_KEY_TYPE_AES);

  if ( ctx == NULL || ctx->MBEDTLS_PRIVATE(cipher_info) == NULL || key == NULL ) {
    return MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA;
  }

  type = ctx->MBEDTLS_PRIVATE(cipher_info)->MBEDTLS_PRIVATE(type);

  switch ( type ) {
    case MBEDTLS_CIPHER_AES_128_ECB:
      psa_set_key_bits(&attr, 128);
      break;
    case MBEDTLS_CIPHER_AES_192_ECB:
      #if defined(SLI_DEVICE_HAS_AES_192)
      psa_set_key_bits(&attr, 192);
      #else
      return MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED;
      #endif
      break;
    case MBEDTLS_CIPHER_AES_256_ECB:
      psa_set_key_bits(&attr, 256);
      break;
    default:
      return MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED;
  }

  if ( ctx->MBEDTLS_PRIVATE(cmac_ctx) == NULL ) {
    /* Allocate CMAC context memory if it hasn't already been allocated */
    cmac_ctx = mbedtls_calloc(1, sizeof(struct mbedtls_cmac_context_t) );
    if ( cmac_ctx == NULL ) {
      return(MBEDTLS_ERR_CIPHER_ALLOC_FAILED);
    }

    ctx->MBEDTLS_PRIVATE(cmac_ctx) = cmac_ctx;
  } else {
    mbedtls_platform_zeroize(ctx->MBEDTLS_PRIVATE(cmac_ctx), sizeof(*ctx->MBEDTLS_PRIVATE(cmac_ctx)) );
  }

  return psa_status_to_mbedtls(
    MAC_SETUP_EN_FCT(&ctx->MBEDTLS_PRIVATE(cmac_ctx)->ctx,
                     &attr,
                     key,
                     keybits / 8U,
                     PSA_ALG_CMAC) );
}

int mbedtls_cipher_cmac_update(mbedtls_cipher_context_t *ctx,
                               const unsigned char *input, size_t ilen)
{
  if ( ctx == NULL || ctx->MBEDTLS_PRIVATE(cipher_info) == NULL || input == NULL
       || ctx->MBEDTLS_PRIVATE(cmac_ctx) == NULL ) {
    return(MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA);
  }

  return psa_status_to_mbedtls(
    MAC_UPDATE_FCT(&ctx->MBEDTLS_PRIVATE(cmac_ctx)->ctx,
                   input,
                   ilen) );
}

int mbedtls_cipher_cmac_finish(mbedtls_cipher_context_t *ctx,
                               unsigned char *output)
{
  if ( ctx == NULL || ctx->MBEDTLS_PRIVATE(cipher_info) == NULL || ctx->MBEDTLS_PRIVATE(cmac_ctx) == NULL
       || output == NULL ) {
    return(MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA);
  }

  size_t olen = 0;

  return psa_status_to_mbedtls(
    MAC_FINISH_EN_FCT(&ctx->MBEDTLS_PRIVATE(cmac_ctx)->ctx,
                      output,
                      MBEDTLS_AES_BLOCK_SIZE,
                      &olen) );
}

int mbedtls_cipher_cmac_reset(mbedtls_cipher_context_t *ctx)
{
  if ( ctx == NULL || ctx->MBEDTLS_PRIVATE(cipher_info) == NULL || ctx->MBEDTLS_PRIVATE(cmac_ctx) == NULL ) {
    return(MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA);
  }

  uint8_t key[32];
  size_t key_len;
  psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
  sl_psa_set_key_type(&attr, PSA_KEY_TYPE_AES);

  if ( ctx->MBEDTLS_PRIVATE(cmac_ctx)->ctx.cipher_mac.key_len > sizeof(key) ) {
    return(MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA);
  }

  /* Save the key to be able to restart the operation */
  memcpy(key,
         ctx->MBEDTLS_PRIVATE(cmac_ctx)->ctx.cipher_mac.key,
         ctx->MBEDTLS_PRIVATE(cmac_ctx)->ctx.cipher_mac.key_len);
  key_len = ctx->MBEDTLS_PRIVATE(cmac_ctx)->ctx.cipher_mac.key_len;
  psa_set_key_bits(&attr, key_len * 8);

  /* Abort and restart with the same key */
  MAC_ABORT_FCT(&ctx->MBEDTLS_PRIVATE(cmac_ctx)->ctx);
  return psa_status_to_mbedtls(
    MAC_SETUP_EN_FCT(&ctx->MBEDTLS_PRIVATE(cmac_ctx)->ctx,
                     &attr,
                     key,
                     key_len,
                     PSA_ALG_CMAC) );
}

int mbedtls_cipher_cmac(const mbedtls_cipher_info_t *cipher_info,
                        const unsigned char *key, size_t keylen,
                        const unsigned char *input, size_t ilen,
                        unsigned char *output)
{
  if ( cipher_info == NULL || key == NULL || input == NULL || output == NULL ) {
    return MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA;
  }

  switch ( cipher_info->MBEDTLS_PRIVATE(type) ) {
    case MBEDTLS_CIPHER_AES_128_ECB:
      if ( keylen != 128UL ) {
        return MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA;
      }
      break;
    case MBEDTLS_CIPHER_AES_192_ECB:
      #if defined(SLI_DEVICE_HAS_AES_192)
      if ( keylen != 192UL ) {
        return MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA;
      }
      break;
      #else
      return MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED;
      #endif
    case MBEDTLS_CIPHER_AES_256_ECB:
      if ( keylen != 256UL ) {
        return MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA;
      }
      break;
    default:
      return MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED;
  }

#if defined(RADIOAES_PRESENT) && defined(SEMAILBOX_PRESENT)
  /* For speeding up PBKDF2-CMAC, which needs a lot of iterations with small-size
   * CMAC operations, we can dispatch these to the RADIOAES instance if there is
   * one available. */
  if ( (keylen == 128UL || keylen == 256UL) && (ilen <= 2 * MBEDTLS_AES_BLOCK_SIZE) ) {
    sl_status_t status = sli_aes_cmac_radio(key,
                                            keylen,
                                            input,
                                            ilen,
                                            output);
    if (status == SL_STATUS_OK) {
      return 0;
    } else {
      return MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    }
  }
#endif

  size_t olen = 0;
  psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
  sl_psa_set_key_type(&attr, PSA_KEY_TYPE_AES);

  switch ( cipher_info->MBEDTLS_PRIVATE(type) ) {
    case MBEDTLS_CIPHER_AES_128_ECB:
      psa_set_key_bits(&attr, 128);
      break;
    case MBEDTLS_CIPHER_AES_192_ECB:
      psa_set_key_bits(&attr, 192);
      break;
    case MBEDTLS_CIPHER_AES_256_ECB:
      psa_set_key_bits(&attr, 256);
      break;
    default:
      return MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA;
  }

  return psa_status_to_mbedtls(
    MAC_ONESHOT_EN_FCT(&attr,
                       key, keylen / 8U,
                       PSA_ALG_CMAC,
                       input, ilen,
                       output, MBEDTLS_AES_BLOCK_SIZE, &olen) );
}

/*
 * Implementation of AES-CMAC-PRF-128 defined in RFC 4615
 */
int mbedtls_aes_cmac_prf_128(const unsigned char *key, size_t key_length,
                             const unsigned char *input, size_t in_len,
                             unsigned char output[16])
{
  int ret;
  unsigned char zero_key[MBEDTLS_AES_BLOCK_SIZE];
  unsigned char int_key[MBEDTLS_AES_BLOCK_SIZE];

  if ( key == NULL || input == NULL || output == NULL ) {
    return(MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA);
  }

  size_t olen = 0;
  psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
  sl_psa_set_key_type(&attr, PSA_KEY_TYPE_AES);
  psa_set_key_bits(&attr, 128);

  if ( key_length == MBEDTLS_AES_BLOCK_SIZE ) {
    /* Use key as is */
    memcpy(int_key, key, MBEDTLS_AES_BLOCK_SIZE);
  } else {
    memset(zero_key, 0, MBEDTLS_AES_BLOCK_SIZE);

    ret = psa_status_to_mbedtls(
      MAC_ONESHOT_EN_FCT(&attr,
                         zero_key, MBEDTLS_AES_BLOCK_SIZE,
                         PSA_ALG_CMAC,
                         key, key_length,
                         int_key, MBEDTLS_AES_BLOCK_SIZE, &olen) );
    if ( ret != 0 ) {
      goto exit;
    }
  }

  ret = psa_status_to_mbedtls(
    MAC_ONESHOT_EN_FCT(&attr,
                       int_key, MBEDTLS_AES_BLOCK_SIZE,
                       PSA_ALG_CMAC,
                       input, in_len,
                       (uint8_t*)output, in_len, &olen) );

  exit:
  mbedtls_platform_zeroize(int_key, sizeof(int_key) );

  return(ret);
}

#endif /* MAC_IMPLEMENTATION_PRESENT */

#endif /* MBEDTLS_CMAC_C && MBEDTLS_CMAC_ALT */
