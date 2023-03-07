/***************************************************************************//**
 * @file
 * @brief Silicon Labs PSA Crypto Secure Engine Driver AEAD functions.
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
#ifndef SLI_SE_DRIVER_AEAD_H
#define SLI_SE_DRIVER_AEAD_H

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

#ifdef __cplusplus
extern "C" {
#endif

#if defined(SEMAILBOX_PRESENT)

// Replace inclusion of crypto_driver_common.h with the new psa driver interface
// header file when it becomes available.
#include "psa/crypto_driver_common.h"

// -----------------------------------------------------------------------------
// Types

typedef struct {
  sl_se_cipher_operation_t direction;
  size_t ad_length;
  size_t pt_length;
  uint8_t nonce[16];
  size_t nonce_length;
} sli_se_driver_aead_preinit_t;

typedef struct {
  psa_algorithm_t alg;
  sl_se_key_descriptor_t key_desc;
  size_t ad_len;
  size_t pt_len;
  union {
    sl_se_gcm_multipart_context_t gcm;
    sl_se_ccm_multipart_context_t ccm;
    sli_se_driver_aead_preinit_t preinit;
  } ctx;
} sli_se_driver_aead_operation_t;

// -----------------------------------------------------------------------------
// Functions

psa_status_t sli_se_driver_aead_encrypt(const psa_key_attributes_t *attributes,
                                        const uint8_t *key_buffer,
                                        size_t key_buffer_size,
                                        psa_algorithm_t alg,
                                        const uint8_t *nonce,
                                        size_t nonce_length,
                                        const uint8_t *additional_data,
                                        size_t additional_data_length,
                                        const uint8_t *plaintext,
                                        size_t plaintext_length,
                                        uint8_t *ciphertext,
                                        size_t ciphertext_size,
                                        size_t *ciphertext_length);

psa_status_t sli_se_driver_aead_decrypt(const psa_key_attributes_t *attributes,
                                        const uint8_t *key_buffer,
                                        size_t key_buffer_size,
                                        psa_algorithm_t alg,
                                        const uint8_t *nonce,
                                        size_t nonce_length,
                                        const uint8_t *additional_data,
                                        size_t additional_data_length,
                                        const uint8_t *ciphertext,
                                        size_t ciphertext_length,
                                        uint8_t *plaintext,
                                        size_t plaintext_size,
                                        size_t *plaintext_length);

psa_status_t sli_se_driver_aead_encrypt_tag(const psa_key_attributes_t *attributes,
                                            const uint8_t *key_buffer,
                                            size_t key_buffer_size,
                                            psa_algorithm_t alg,
                                            const uint8_t *nonce,
                                            size_t nonce_length,
                                            const uint8_t *additional_data,
                                            size_t additional_data_length,
                                            const uint8_t *plaintext,
                                            size_t plaintext_length,
                                            uint8_t *ciphertext,
                                            size_t ciphertext_size,
                                            size_t *ciphertext_length,
                                            uint8_t *tag,
                                            size_t tag_size,
                                            size_t *tag_length);

psa_status_t sli_se_driver_aead_decrypt_tag(const psa_key_attributes_t *attributes,
                                            const uint8_t *key_buffer,
                                            size_t key_buffer_size,
                                            psa_algorithm_t alg,
                                            const uint8_t *nonce,
                                            size_t nonce_length,
                                            const uint8_t *additional_data,
                                            size_t additional_data_length,
                                            const uint8_t *ciphertext,
                                            size_t ciphertext_length,
                                            const uint8_t* tag,
                                            size_t tag_length,
                                            uint8_t *plaintext,
                                            size_t plaintext_size,
                                            size_t *plaintext_length);

psa_status_t sli_se_driver_aead_encrypt_decrypt_setup(sli_se_driver_aead_operation_t *operation,
                                                      const psa_key_attributes_t *attributes,
                                                      const uint8_t *key_buffer,
                                                      size_t key_buffer_size,
                                                      psa_algorithm_t alg,
                                                      sl_se_cipher_operation_t operation_direction,
                                                      uint8_t *key_storage_buffer,
                                                      size_t key_storage_buffer_size,
                                                      size_t key_storage_overhead);

psa_status_t sli_se_driver_aead_set_nonce(sli_se_driver_aead_operation_t *operation,
                                          const uint8_t *nonce,
                                          size_t nonce_size);

psa_status_t sli_se_driver_aead_set_lengths(sli_se_driver_aead_operation_t *operation,
                                            size_t ad_length,
                                            size_t plaintext_length);

psa_status_t sli_se_driver_aead_update_ad(sli_se_driver_aead_operation_t *operation,
                                          uint8_t *key_buffer,
                                          const uint8_t *input,
                                          size_t input_length);

psa_status_t sli_se_driver_aead_update(sli_se_driver_aead_operation_t *operation,
                                       uint8_t *key_buffer,
                                       const uint8_t *input,
                                       size_t input_length,
                                       uint8_t *output,
                                       size_t output_size,
                                       size_t *output_length);

psa_status_t sli_se_driver_aead_finish(sli_se_driver_aead_operation_t *operation,
                                       uint8_t *key_buffer,
                                       uint8_t *ciphertext,
                                       size_t ciphertext_size,
                                       size_t *ciphertext_length,
                                       uint8_t *tag,
                                       size_t tag_size,
                                       size_t *tag_length);

psa_status_t sli_se_driver_aead_verify(sli_se_driver_aead_operation_t *operation,
                                       uint8_t *key_buffer,
                                       uint8_t *plaintext,
                                       size_t plaintext_size,
                                       size_t *plaintext_length,
                                       const uint8_t *tag,
                                       size_t tag_length);

#endif // SEMAILBOX_PRESENT

#ifdef __cplusplus
}
#endif

/** \} (end addtogroup sl_psa_drivers_se) */
/** \} (end addtogroup sl_psa_drivers) */

/// @endcond

#endif // SLI_SE_DRIVER_AEAD_H
