/***************************************************************************//**
 * @file
 * @brief Silicon Labs PSA Crypto Secure Engine Driver MAC functions.
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
#ifndef SLI_SE_DRIVER_MAC_H
#define SLI_SE_DRIVER_MAC_H

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

#include "sl_se_manager.h"
#include "sl_se_manager_cipher.h"

// -----------------------------------------------------------------------------
// Types

typedef struct {
  psa_algorithm_t alg;
  union {
    sl_se_cmac_multipart_context_t cmac;
    struct {
      uint8_t iv[16];
      size_t iv_len;
      uint8_t streaming_block[16];
      size_t processed_length;
    } cbcmac;
    #if defined(PSA_WANT_ALG_HMAC)
    struct {
      #if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
      uint8_t hmac_result[64];
      #else
      uint8_t hmac_result[32];
      #endif
      size_t hmac_len;
    } hmac;
    #endif
  } ctx;
} sli_se_driver_mac_operation_t;

// -----------------------------------------------------------------------------
// Functions

#if defined(PSA_WANT_ALG_HMAC)
sl_se_hash_type_t sli_se_hash_type_from_psa_hmac_alg(psa_algorithm_t alg,
                                                     size_t *length);
#endif

psa_status_t sli_se_driver_mac_compute(sl_se_key_descriptor_t *key_desc,
                                       psa_algorithm_t alg,
                                       const uint8_t *input,
                                       size_t input_length,
                                       uint8_t *mac,
                                       size_t mac_size,
                                       size_t *mac_length);

psa_status_t sli_se_driver_mac_sign_setup(sli_se_driver_mac_operation_t *operation,
                                          const psa_key_attributes_t *attributes,
                                          psa_algorithm_t alg);

psa_status_t sli_se_driver_mac_update(sli_se_driver_mac_operation_t *operation,
                                      sl_se_key_descriptor_t *key_desc,
                                      const uint8_t *input,
                                      size_t input_length);

psa_status_t sli_se_driver_mac_sign_finish(sli_se_driver_mac_operation_t *operation,
                                           sl_se_key_descriptor_t *key_desc,
                                           uint8_t *mac,
                                           size_t mac_size,
                                           size_t *mac_length);

#endif // SEMAILBOX_PRESENT

#ifdef __cplusplus
}
#endif

/** \} (end addtogroup sl_psa_drivers_se) */
/** \} (end addtogroup sl_psa_drivers) */

/// @endcond

#endif // SLI_SE_DRIVER_MAC_H
