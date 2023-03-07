/***************************************************************************//**
 * @file
 * @brief Silicon Labs PSA Crypto Opaque Driver API Internal Types for SE.
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
#ifndef SLI_SE_OPAQUE_TYPES_H
#define SLI_SE_OPAQUE_TYPES_H

#include "em_device.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(SEMAILBOX_PRESENT)

#include "sl_se_manager_defines.h"
#include "sl_se_manager_types.h"
#include "sl_psa_values.h"
#include "sli_se_driver_aead.h"
#include "sli_se_driver_mac.h"
#include "sli_se_driver_key_derivation.h"
#include "sli_se_driver_cipher.h"
// Replace inclusion of crypto_driver_common.h with the new psa driver interface
// header file when it becomes available.
#include "psa/crypto_driver_common.h"
#include "psa/crypto_platform.h"

/***************************************************************************//**
 * \addtogroup sl_psa_drivers
 * \{
 ******************************************************************************/

/***************************************************************************//**
 * \addtogroup sl_psa_drivers_se PSA drivers for devices with Secure Engine
 * \{
 ******************************************************************************/

/*******************************************************************************
 * Defines *
 ******************************************************************************/

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN

/// Location value for keys to be stored encrypted with the device-unique secret,
/// or for accessing the built-in keys on Vault devices.
#define PSA_KEY_LOCATION_SLI_SE_OPAQUE  PSA_KEY_LOCATION_SL_SE_OPAQUE

/// Version of opaque header struct
#define SLI_SE_OPAQUE_KEY_CONTEXT_VERSION (0x01)

/*******************************************************************************
 * Structs *
 ******************************************************************************/
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT) \
  || defined(MBEDTLS_PSA_CRYPTO_BUILTIN_KEYS)
/// Key header for context struct of opaque registered keys
typedef struct {
  /// Version field for the struct
  uint8_t struct_version;
  /// Builtin key ID. Set to zero for a key header which is part of a \ref
  /// sli_se_opaque_wrapped_key_context_t, otherwise set to an SE Manager builtin
  /// key ID.
  uint8_t builtin_key_id;
  /// Reserved space (initialise to all-zero)
  uint8_t reserved[2];
} sli_se_opaque_key_context_header_t;

/// Key context for wrapped keys
typedef struct {
  /// Key context header
  sli_se_opaque_key_context_header_t header;

  /// Key information required to construct an SE manager key descriptor
  // sl_se_key_descriptor_t key_desc;
  uint32_t key_type;
  uint32_t key_size;
  uint32_t key_flags;

  /// wrapped_buffer is set to a distinctive size to make sizeof() result
  /// in the overhead for storing a wrapped key.
  /// A wrapped key will in reality consume more space than
  /// SLI_SE_WRAPPED_KEY_OVERHEAD
  uint8_t wrapped_buffer[SLI_SE_WRAPPED_KEY_OVERHEAD];
  /// Variable member, accounting for the extra space
  uint8_t fill[];
} sli_se_opaque_wrapped_key_context_t;

// Notes for JSON entry for wrapped driver:
// "base_size": "sizeof(sli_se_opaque_wrapped_key_context_t)",
// "symmetric_factor": 1,
// "key_pair_size": 66,
// "public_key_size" 133
// Is 66/133 the largest keys we accept? What about custom curves?

// ----------------------------------
// Potential format for internal volatile keys
// typedef struct {
//   sl_se_key_descriptor_t key_desc;
// } sli_se_opaque_volatile_key_context;

// Notes for JSON entry for internal volatile driver:
// "base_size": "sizeof(sli_se_opaque_volatile_key_context)",
// For the remaining entries, the defaults are fine.

typedef struct {
  sl_se_key_descriptor_t key_desc;
  #if defined(PSA_WANT_ALG_HMAC)
  uint8_t key[SLI_SE_WRAPPED_KEY_OVERHEAD + 64];
  #else
  uint8_t key[SLI_SE_WRAPPED_KEY_OVERHEAD + 32];
  #endif
  size_t key_len;
  sli_se_driver_mac_operation_t operation;
} sli_se_opaque_mac_operation_t;

typedef struct {
  uint8_t key[SLI_SE_WRAPPED_KEY_OVERHEAD + 32];
  size_t key_len;
  sli_se_driver_aead_operation_t operation;
} sli_se_opaque_aead_operation_t;

typedef struct {
  uint8_t key[SLI_SE_WRAPPED_KEY_OVERHEAD + 32];
  size_t key_len;
  sli_se_driver_cipher_operation_t operation;
} sli_se_opaque_cipher_operation_t;

#endif // _SILICON_LABS_SECURITY_FEATURE_VAULT || MBEDTLS_PSA_CRYPTO_BUILTIN_KEYS
#endif // SEMALBOX_PRESENT

#ifdef __cplusplus
}
#endif

/// @endcond
/** \} (end addtogroup sl_psa_drivers_se) */
/** \} (end addtogroup sl_psa_drivers) */

#endif // SLI_SE_OPAQUE_TYPES_H
