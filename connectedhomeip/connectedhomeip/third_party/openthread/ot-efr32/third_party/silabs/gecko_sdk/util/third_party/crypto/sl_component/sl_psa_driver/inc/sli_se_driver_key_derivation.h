/***************************************************************************//**
 * @file
 * @brief SE Driver for Silicon Labs devices with an embedded SE, for use with
 *        PSA Crypto and Mbed TLS
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

#ifndef SLI_SE_DRIVER_KEY_DERIVATION
#define SLI_SE_DRIVER_KEY_DERIVATION

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

#include "sl_se_manager.h"
#include "sl_se_manager_defines.h"
// Replace inclusion of crypto_driver_common.h with the new psa driver interface
// header file when it becomes available.
#include "psa/crypto_driver_common.h"
#include "psa/crypto_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// Structs and typedefs

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
  #define SLI_SE_MAX_ECP_PRIVATE_KEY_SIZE (PSA_BITS_TO_BYTES(521))
#else
  #define SLI_SE_MAX_ECP_PRIVATE_KEY_SIZE (PSA_BITS_TO_BYTES(256))
#endif

#define SLI_SE_MAX_ECP_PUBLIC_KEY_SIZE    (SLI_SE_MAX_ECP_PRIVATE_KEY_SIZE * 2)

#define SLI_SE_MAX_PADDED_ECP_PRIVATE_KEY_SIZE \
  (SLI_SE_MAX_ECP_PRIVATE_KEY_SIZE             \
   + sli_se_get_padding(SLI_SE_MAX_ECP_PRIVATE_KEY_SIZE))
#define SLI_SE_MAX_PADDED_ECP_PUBLIC_KEY_SIZE \
  (SLI_SE_MAX_PADDED_ECP_PRIVATE_KEY_SIZE * 2)

// -----------------------------------------------------------------------------
// Function declarations

psa_status_t sli_se_driver_key_agreement(
  psa_algorithm_t alg,
  const psa_key_attributes_t *attributes,
  const uint8_t *key_buffer,
  size_t key_buffer_size,
  const uint8_t *peer_key,
  size_t peer_key_length,
  uint8_t *output,
  size_t output_size,
  size_t *output_length);

#ifdef __cplusplus
}
#endif

#endif // SEMAILBOX_PRESENT

/** \} (end addtogroup sl_psa_drivers_se) */
/** \} (end addtogroup sl_psa_drivers) */

/// @endcond

#endif // SLI_SE_DRIVER_KEY_DERIVATION
