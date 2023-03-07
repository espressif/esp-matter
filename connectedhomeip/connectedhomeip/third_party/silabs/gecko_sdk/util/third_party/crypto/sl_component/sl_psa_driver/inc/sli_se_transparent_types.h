/***************************************************************************//**
 * @file
 * @brief Silicon Labs PSA Crypto Transparent Driver API Types for SE.
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
#ifndef SLI_SE_TRANSPARENT_TYPES_H
#define SLI_SE_TRANSPARENT_TYPES_H

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN

/***************************************************************************//**
 * \addtogroup sl_psa_drivers
 * \{
 ******************************************************************************/

/***************************************************************************//**
 * \addtogroup sl_psa_drivers_se
 * \{
 ******************************************************************************/

#include "em_device.h"

#if defined(SEMAILBOX_PRESENT)

#include "sl_se_manager_types.h"
#include "sli_se_driver_aead.h"
#include "sli_se_driver_mac.h"
#include "sli_se_driver_cipher.h"
// Replace inclusion of crypto_driver_common.h with the new psa driver interface
// header file when it becomes available.
#include "psa/crypto_driver_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/// PSA transparent accelerator driver compatible context structure
typedef struct {
  sl_se_hash_type_t       hash_type;    ///< Hash type
  union {
    sl_se_sha1_multipart_context_t      sha1_context;
    sl_se_sha224_multipart_context_t    sha224_context;
    sl_se_sha256_multipart_context_t    sha256_context;
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT) || defined(DOXYGEN)
    sl_se_sha384_multipart_context_t    sha384_context;
    sl_se_sha512_multipart_context_t    sha512_context;
#endif
  } streaming_contexts;
} sli_se_transparent_hash_operation_t;

typedef struct {
  uint8_t key[32];
  size_t key_len;
  sli_se_driver_cipher_operation_t operation;
} sli_se_transparent_cipher_operation_t;

typedef union {
  struct {
    sli_se_driver_mac_operation_t operation;
    uint8_t key[32];
    size_t key_len;
  } cipher_mac;
  #if defined(PSA_WANT_ALG_HMAC)
  struct {
    psa_algorithm_t alg;
    sli_se_transparent_hash_operation_t hash_ctx;
    #if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
    uint8_t opad[128];
    #else
    uint8_t opad[64];
    #endif
  } hmac;
  #endif /* PSA_WANT_ALG_HMAC */
} sli_se_transparent_mac_operation_t;

typedef struct {
  uint8_t key[32];
  size_t key_len;
  sli_se_driver_aead_operation_t operation;
} sli_se_transparent_aead_operation_t;

#ifdef __cplusplus
}
#endif

#endif // SEMAILBOX_PRESENT

/** \} (end addtogroup sl_psa_drivers_se) */
/** \} (end addtogroup sl_psa_drivers) */

/// @endcond

#endif // SLI_SE_TRANSPARENT_TYPES_H
