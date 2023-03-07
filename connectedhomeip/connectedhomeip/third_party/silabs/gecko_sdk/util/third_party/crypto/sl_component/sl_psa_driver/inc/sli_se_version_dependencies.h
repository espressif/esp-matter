/***************************************************************************//**
 * @file
 * @brief Silicon Labs PSA Crypto Driver SE Version Dependencies.
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

#ifndef SLI_SE_VERSION_DEPENDENCIES_H
#define SLI_SE_VERSION_DEPENDENCIES_H

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN

#include "em_device.h"

#if defined(SEMAILBOX_PRESENT)
#include "sli_se_driver_key_management.h"
#endif // defined(SEMAILBOX_PRESENT)

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// Version Constants

// HSE specific constants
#if defined(SEMAILBOX_PRESENT)
// The oldest firmware revision with support for checking the validity
// of public ECC keys. Also see SL_SE_SUPPORT_FW_PRIOR_TO_1_2_2 and
// SL_SE_ASSUME_FW_AT_LEAST_1_2_2.
  #if !defined(SLI_SE_OLDEST_VERSION_WITH_PUBLIC_KEY_VALIDATION)
  #define SLI_SE_OLDEST_VERSION_WITH_PUBLIC_KEY_VALIDATION (0x00010202U)
  #endif

// The SE version that first introduced a regression related to Ed25519. See
// SL_SE_ASSUME_FW_UNAFFECTED_BY_ED25519_ERRATA.
  #if !defined(SLI_SE_FIRST_VERSION_WITH_BROKEN_ED25519)
  #define SLI_SE_FIRST_VERSION_WITH_BROKEN_ED25519 (0x00010202U)
  #endif

// The final SE version containing a bug causing Ed25519 to be broken. See
// SL_SE_ASSUME_FW_UNAFFECTED_BY_ED25519_ERRATA.
  #if !defined(SLI_SE_LAST_VERSION_WITH_BROKEN_ED25519)
  #define SLI_SE_LAST_VERSION_WITH_BROKEN_ED25519 (0x00010208U)
  #endif
#endif // defined(SEMAILBOX_PRESENT)

// Common HSE/VSE constants

// The first SE version that supports TrustZone Storage Root Key (SRK)
#if !defined(SLI_SE_FIRST_VERSION_WITH_SRK_SUPPORT)
  #if (_SILICON_LABS_32B_SERIES_2_CONFIG >= 3)
    #define SLI_SE_FIRST_VERSION_WITH_SRK_SUPPORT (0x00020200)
  #else
    #define SLI_SE_FIRST_VERSION_WITH_SRK_SUPPORT (0x0001020c)
  #endif
#endif

// -----------------------------------------------------------------------------
// Preprocessor Guard Helper Defines

// -------------------------------
// ECDH

#if defined(SEMAILBOX_PRESENT)
  #if !defined(SL_SE_ASSUME_FW_AT_LEAST_1_2_2) \
  && defined(_SILICON_LABS_32B_SERIES_2_CONFIG_1)
    #define SLI_SE_VERSION_ECDH_PUBKEY_VALIDATION_UNCERTAIN
  #endif
#endif // defined(SEMAILBOX_PRESENT)

// -------------------------------
// EdDSA

#if defined(SEMAILBOX_PRESENT)
  #if !defined(SL_SE_ASSUME_FW_UNAFFECTED_BY_ED25519_ERRATA) \
  && defined(_SILICON_LABS_32B_SERIES_2_CONFIG_1)
    #define SLI_SE_VERSION_ED25519_ERRATA_UNCERTAIN
  #endif

  #if defined(SLI_SE_VERSION_ED25519_ERRATA_UNCERTAIN) \
  && defined(SLI_PSA_WANT_ECC_TWISTED_EDWARDS)
    #define SLI_SE_VERSION_ED25519_ERRATA_CHECK_REQUIRED
  #endif
#endif // defined(SEMAILBOX_PRESENT)

// -----------------------------------------------------------------------------
// Version Macros

// HSE specific macros
#if defined(SEMAILBOX_PRESENT)
  #define SLI_SE_VERSION_PUBKEY_VALIDATION_REQUIRED(se_version) \
  (se_version < SLI_SE_OLDEST_VERSION_WITH_PUBLIC_KEY_VALIDATION)

  #define SLI_SE_VERSION_ED25519_BROKEN(se_version)          \
  (!((se_version < SLI_SE_FIRST_VERSION_WITH_BROKEN_ED25519) \
     || (se_version > SLI_SE_LAST_VERSION_WITH_BROKEN_ED25519)))
#endif // defined(SEMAILBOX_PRESENT)

// Common HSE/VSE macros
#define SLI_VERSION_REMOVE_DIE_ID(version)  ((version) & 0x00FFFFFFU)

#define SLI_SE_VERSION_SUPPORTS_SRK(se_version) \
  (SLI_VERSION_REMOVE_DIE_ID(se_version) >= SLI_SE_FIRST_VERSION_WITH_SRK_SUPPORT)

// -----------------------------------------------------------------------------
// Function Declarations

#if defined(SEMAILBOX_PRESENT)
psa_status_t sli_se_check_eddsa_errata(const psa_key_attributes_t* attributes,
                                       sl_se_command_context_t* cmd_ctx);
#endif // defined(SEMAILBOX_PRESENT)

#ifdef __cplusplus
}
#endif

/// @endcond

#endif // SLI_SE_VERSION_DEPENDENCIES_H
