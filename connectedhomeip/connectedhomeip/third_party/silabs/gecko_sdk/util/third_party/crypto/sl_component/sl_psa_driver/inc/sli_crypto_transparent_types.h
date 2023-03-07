/***************************************************************************//**
 * @file
 * @brief Silicon Labs PSA Crypto Transparent Driver API Types for CRYPTO.
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
#ifndef SLI_CRYPTO_TRANSPARENT_TYPES_H
#define SLI_CRYPTO_TRANSPARENT_TYPES_H

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN

/***************************************************************************//**
 * \addtogroup sl_psa_drivers
 * \{
 ******************************************************************************/

/***************************************************************************//**
 * \addtogroup sl_psa_drivers_crypto
 * \{
 ******************************************************************************/

#include "em_device.h"

#if defined(CRYPTO_PRESENT)

#include "em_crypto.h"
// Replace inclusion of crypto_driver_common.h with the new psa driver interface
// header file when it becomes available.
#include "psa/crypto_driver_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/// PSA transparent accelerator driver compatible context structure
typedef struct {
  psa_algorithm_t hash_type;          ///< Hash type
  uint32_t total[2];                  ///< number of bytes processed
  uint32_t state[8];                  ///< intermediate digest state
  uint32_t buffer[16];                ///< data block being processed
} sli_crypto_transparent_hash_operation_t;

typedef union {
  struct {
    psa_algorithm_t alg;              ///< Algorithm
    uint8_t key[32];                  ///< Key buffer
    size_t key_len;                   ///< Key length
    uint8_t iv[16];                   ///< IV buffer
    uint8_t unprocessed_block[16];    ///< Unprocessed data buffer
    size_t unprocessed_len;           ///< Length of unprocessed data buffer
  } cipher_mac;
} sli_crypto_transparent_mac_operation_t;

typedef struct {
  psa_encrypt_or_decrypt_t direction;    ///< Encrypt or Decrypt
  psa_algorithm_t alg;                   ///< Algorithm
  uint8_t key[32];                       ///< Key buffer
  size_t key_len;                        ///< Key length
  uint8_t iv[16];                        ///< IV buffer
  size_t iv_len;                         ///< IV length
  uint8_t streaming_block[16];           ///< Streaming length
  size_t processed_length;               ///< Current length of the encrypted/decrypted data
} sli_crypto_transparent_cipher_operation_t;

#if defined(PSA_WANT_ALG_GCM)
typedef struct {
  CRYPTO_Data_TypeDef  ghash_state;      ///< GHASH state
  CRYPTO_Data_TypeDef  gctr_state;       ///< GCTR counter value
  CRYPTO_Data_TypeDef  ghash_key;        ///< GHASH key (is a constant value
                                         ///  which is faster to restore than
                                         ///  to reconstruct each time).
  CRYPTO_Data_TypeDef  iv;               ///< IV value
} sli_crypto_gcm_ctx;
#endif // PSA_WANT_ALG_GCM

#if defined(PSA_WANT_ALG_CCM)
typedef struct {
  CRYPTO_Data_TypeDef cctr_state;       ///< CTR counter value
  CRYPTO_Data_TypeDef iv;               ///< IV buffer
  uint8_t iv_len;                       ///< IV length
} sli_crypto_ccm_ctx;
#endif// PSA_WANT_ALG_CCM

typedef struct {
  uint8_t nonce[16];                     ///< Nonce buffer
  uint8_t nonce_length;                  ///< Nonce length
#if defined(PSA_WANT_ALG_CCM)
  size_t add_len;                        ///< Lenght of additional data
#endif
} sli_crypto_transparent_aead_preinit_t;

typedef struct {
  psa_algorithm_t alg;                   ///< Algorithm
  psa_encrypt_or_decrypt_t direction;    ///< Encrypt or decrypt
  uint8_t key[32];                       ///< Key buffer
  size_t key_len;                        ///< Key length
  size_t add_len;                        ///< Length of additional data
  uint8_t unprocessed_block[16];         ///< Input data saved for finish operation
  uint8_t unprocessed_aad_length;        ///< Length of additional data saved for finish operation
  uint8_t unprocessed_length;            ///< Length of input data saved for finish operation
  bool first_update_ad_op;               ///< Bool indicating first update authenication data call. Only used for CCM.
  size_t processed_len;                  ///< Current length of the encrypted/decrypted data
  size_t total_length;                   ///< Total length of data to encrypt/decrypt
  union {
#if defined(PSA_WANT_ALG_CCM)
    sli_crypto_ccm_ctx ccm_ctx;          ///< CCM context
#endif // PSA_WANT_ALG_CCM
#if defined(PSA_WANT_ALG_GCM)
    sli_crypto_gcm_ctx gcm_ctx;          ///< GCM context
#endif // PSA_WANT_ALG_GCM
    sli_crypto_transparent_aead_preinit_t preinit; ///< Used for initialization of multipart operation
  } ctx;
} sli_crypto_transparent_aead_operation_t;

#ifdef __cplusplus
}
#endif
#endif // CRYPTO_PRESENT

/** \} (end addtogroup sl_psa_drivers_crypto) */
/** \} (end addtogroup sl_psa_drivers) */

/// @endcond

#endif // SLI_CRYPTO_TRANSPARENT_TYPES_H
