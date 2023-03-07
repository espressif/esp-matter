/***************************************************************************//**
 * @file
 * @brief Silicon Labs PSA Crypto Values.
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_PSA_VALUES_H
#define SL_PSA_VALUES_H

#include "em_device.h"

#if defined(SL_TRUSTZONE_NONSECURE)
#include "psa/crypto.h"  // include path: trusted-firmware-m/interface/include
#else
#include "psa/crypto_driver_common.h" // include path: mbedtls/include
#endif

//------------------------------------------------------------------------------
// Hardware Secure Engine (SEMAILBOX_PRESENT)

#if defined(SEMAILBOX_PRESENT)

/***************************************************************************//**
 * \addtogroup sl_psa_drivers
 * \{
 ******************************************************************************/

/***************************************************************************//**
 * \addtogroup sl_psa_drivers_se Values for devices with Secure Engine
 * \{
 ******************************************************************************/

/// Location value for keys to be stored encrypted with the device-unique secret,
/// or for accessing the built-in keys on Vault devices.
#define PSA_KEY_LOCATION_SL_SE_OPAQUE   ((psa_key_location_t)0x000001UL)

#if defined(MBEDTLS_PSA_CRYPTO_BUILTIN_KEYS) || defined(SL_TRUSTZONE_NONSECURE)
/***************************************************************************//**
 * \addtogroup sl_psa_drivers_se_builtin_keys Builtin keys on devices with Secure Engine
 * \brief These key ID values belong to the builtin range of PSA, and allow access
 *        to the keys which respectively are and can be preprovisioned in Secure Engine
 *        devices. Refer to \ref psa_builtin_keys or AN1311 for more information on the
 *        usage of builtin keys through PSA Crypto.
 * \{
 ******************************************************************************/

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
  #ifndef SL_SE_BUILTIN_KEY_APPLICATION_ATTESTATION_ID
/// Vendor Key ID for the built-in application identity key on Vault High devices
    #define SL_SE_BUILTIN_KEY_APPLICATION_ATTESTATION_ID  (MBEDTLS_PSA_KEY_ID_BUILTIN_MIN + 5)
  #endif

  #ifndef SL_SE_BUILTIN_KEY_SYSTEM_ATTESTATION_ID
/// Vendor Key ID for the built-in SE identity key on Vault High devices
    #define SL_SE_BUILTIN_KEY_SYSTEM_ATTESTATION_ID       (MBEDTLS_PSA_KEY_ID_BUILTIN_MIN + 4)
  #endif
#endif // VAULT

#ifndef SL_SE_BUILTIN_KEY_SECUREBOOT_ID
/// Vendor Key ID for the Secure Boot verifying key provisioned to the Secure Engine.
  #define SL_SE_BUILTIN_KEY_SECUREBOOT_ID               (MBEDTLS_PSA_KEY_ID_BUILTIN_MIN + 1)
#endif

#ifndef SL_SE_BUILTIN_KEY_SECUREDEBUG_ID
/// Vendor Key ID for the Secure Debug verifying key provisioned to the Secure Engine.
  #define SL_SE_BUILTIN_KEY_SECUREDEBUG_ID              (MBEDTLS_PSA_KEY_ID_BUILTIN_MIN + 2)
#endif

#ifndef SL_SE_BUILTIN_KEY_AES128_ID
/// Vendor Key ID for AES-128 key provisioned to the Secure Engine.
  #define SL_SE_BUILTIN_KEY_AES128_ID                   (MBEDTLS_PSA_KEY_ID_BUILTIN_MIN + 3)
#endif

#ifndef SL_SE_BUILTIN_KEY_TRUSTZONE_ID
/// Vendor Key ID for the TrustZone root key
  #define SL_SE_BUILTIN_KEY_TRUSTZONE_ID                (MBEDTLS_PSA_KEY_ID_BUILTIN_MIN + 6)
#endif

#ifndef SL_SE_BUILTIN_KEY_AES128_ALG
/// Algorithm with which the #SL_SE_BUILTIN_KEY_AES128_ID key will be used.
// PSA Crypto only allows one specific usage algorithm per built-in key ID.
  #define SL_SE_BUILTIN_KEY_AES128_ALG                  (PSA_ALG_CTR)
#endif

/** \} (end addtogroup sl_psa_drivers_se_builtin_keys) */
#endif // MBEDTLS_PSA_CRYPTO_BUILTIN_KEYS || SL_TRUSTZONE_NONSECURE

/** \} (end addtogroup sl_psa_drivers_se) */
/** \} (end addtogroup sl_psa_drivers) */

#endif // SEMALBOX_PRESENT

//------------------------------------------------------------------------------
// Virtual Secure Engine (CRYPTOACC_PRESENT)

#if defined(CRYPTOACC_PRESENT) && defined(SEPUF_PRESENT)

/***************************************************************************//**
 * \addtogroup sl_psa_drivers
 * \{
 ******************************************************************************/

/***************************************************************************//**
 * \addtogroup sl_psa_drivers_cryptoacc Values for devices with a VSE
 * \{
 ******************************************************************************/

/// Location value for builtin keys on VSE archtectures
#define PSA_KEY_LOCATION_SL_CRYPTOACC_OPAQUE     ((psa_key_location_t)0x000002UL)

#if defined(MBEDTLS_PSA_CRYPTO_BUILTIN_KEYS) || defined(SL_TRUSTZONE_NONSECURE)
/***************************************************************************//**
 * \addtogroup sl_psa_drivers_cryptoacc_builtin_keys Builtin keys on devices with a VSE
 * \brief These key ID values belong to the builtin range of PSA, and allow access
 *        to the keys which respectively are and can be preprovisioned in (Virtual)
 *        Secure Engine devices. Refer to \ref psa_builtin_keys or AN1311 for more
 *        information on the usage of builtin keys through PSA Crypto.
 * \{
 ******************************************************************************/

#ifndef SL_CRYPTOACC_BUILTIN_KEY_TRUSTZONE_ID
/// Vendor Key ID for the PUF-derived hardware unique key
  #define SL_CRYPTOACC_BUILTIN_KEY_PUF_ID          (MBEDTLS_PSA_KEY_ID_BUILTIN_MIN + 1)
#endif

/// Internal ID for PUF-derived key
#define SLI_CRYPTOACC_BUILTIN_KEY_PUF_SLOT       (SL_CRYPTOACC_BUILTIN_KEY_PUF_ID && 0xFF)

/** \} (end addtogroup sl_psa_drivers_se_builtin_keys) */

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN
/// Version of opaque header struct
#define SLI_CRYPTOACC_OPAQUE_KEY_CONTEXT_VERSION (0x00)
/// @endcond

#endif // MBEDTLS_PSA_CRYPTO_BUILTIN_KEYS || SL_TRUSTZONE_NONSECURE

/** \} (end addtogroup sl_psa_drivers_cryptoacc) */
/** \} (end addtogroup sl_psa_drivers) */

#endif // CRYPTOACC_PRESENT && SEPUF_PRESENT

#endif // SL_PSA_VALUES_H
