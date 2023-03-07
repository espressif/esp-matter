/***************************************************************************//**
 * @file
 * @brief Silicon Labs PSA Crypto Secure Engine Driver cipher functions.
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
#ifndef SLI_SE_DRIVER_CIPHER_H
#define SLI_SE_DRIVER_CIPHER_H

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

#include "sl_se_manager_types.h"
// Replace inclusion of crypto_driver_common.h with the new psa driver interface
// header file when it becomes available.
#include "psa/crypto_driver_common.h"

// -----------------------------------------------------------------------------
// Types

typedef struct {
  sl_se_key_descriptor_t key_desc;
  sl_se_cipher_operation_t direction;
  psa_algorithm_t alg;
  uint8_t iv[16];
  size_t iv_len;
  uint8_t streaming_block[16];
  size_t processed_length;
} sli_se_driver_cipher_operation_t;

// -----------------------------------------------------------------------------
// Functions

psa_status_t sli_se_driver_cipher_encrypt(const psa_key_attributes_t *attributes,
                                          const uint8_t *key_buffer,
                                          size_t key_buffer_size,
                                          psa_algorithm_t alg,
                                          const uint8_t *iv,
                                          size_t iv_length,
                                          const uint8_t *input,
                                          size_t input_length,
                                          uint8_t *output,
                                          size_t output_size,
                                          size_t *output_length);

psa_status_t sli_se_driver_cipher_decrypt(const psa_key_attributes_t *attributes,
                                          const uint8_t *key_buffer,
                                          size_t key_buffer_size,
                                          psa_algorithm_t alg,
                                          const uint8_t *input,
                                          size_t input_length,
                                          uint8_t *output,
                                          size_t output_size,
                                          size_t *output_length);

psa_status_t sli_se_driver_cipher_encrypt_setup(sli_se_driver_cipher_operation_t *operation,
                                                const psa_key_attributes_t *attributes,
                                                psa_algorithm_t alg);

psa_status_t sli_se_driver_cipher_decrypt_setup(sli_se_driver_cipher_operation_t *operation,
                                                const psa_key_attributes_t *attributes,
                                                psa_algorithm_t alg);

psa_status_t sli_se_driver_cipher_set_iv(sli_se_driver_cipher_operation_t *operation,
                                         const uint8_t *iv,
                                         size_t iv_length);

psa_status_t sli_se_driver_cipher_update(sli_se_driver_cipher_operation_t *operation,
                                         const uint8_t *input,
                                         size_t input_length,
                                         uint8_t *output,
                                         size_t output_size,
                                         size_t *output_length);

psa_status_t sli_se_driver_cipher_finish(sli_se_driver_cipher_operation_t *operation,
                                         uint8_t *output,
                                         size_t output_size,
                                         size_t *output_length);

psa_status_t sli_se_driver_cipher_abort(sli_se_driver_cipher_operation_t *operation);

#endif // SEMAILBOX_PRESENT

#ifdef __cplusplus
}
#endif

/** \} (end addtogroup sl_psa_drivers_se) */
/** \} (end addtogroup sl_psa_drivers) */

/// @endcond

#endif // SLI_SE_DRIVER_CIPHER_H
