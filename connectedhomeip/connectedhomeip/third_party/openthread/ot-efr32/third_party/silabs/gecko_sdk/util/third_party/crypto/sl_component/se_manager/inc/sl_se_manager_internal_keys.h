/***************************************************************************//**
 * @file
 * @brief Silicon Labs Secure Engine Manager Internal key defines
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
#ifndef SL_SE_MANAGER_INTERNAL_KEYS
#define SL_SE_MANAGER_INTERNAL_KEYS

#include "em_device.h"
#include "sl_se_manager_defines.h"

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT) || defined(DOXYGEN)

#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_1)
/// Key descriptor for internal application attestation key
  #define SL_SE_APPLICATION_ATTESTATION_KEY                     \
  {                                                             \
    .type = SL_SE_KEY_TYPE_ECC_P256,                            \
    .flags = SL_SE_KEY_FLAG_NON_EXPORTABLE                      \
             | SL_SE_KEY_FLAG_IS_DEVICE_GENERATED               \
             | SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PRIVATE_KEY \
             | SL_SE_KEY_FLAG_ASYMMETRIC_SIGNING_ONLY,          \
    .storage = {                                                \
      .method = SL_SE_KEY_STORAGE_INTERNAL_IMMUTABLE,           \
      .location = {                                             \
        .slot = SL_SE_KEY_SLOT_APPLICATION_ATTESTATION_KEY,     \
      },                                                        \
    },                                                          \
  }
#else
/// Key descriptor for internal application attestation key
  #define SL_SE_APPLICATION_ATTESTATION_KEY                     \
  {                                                             \
    .type = SL_SE_KEY_TYPE_ECC_P256,                            \
    .flags = SL_SE_KEY_FLAG_IS_DEVICE_GENERATED                 \
             | SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PRIVATE_KEY \
             | SL_SE_KEY_FLAG_ASYMMETRIC_SIGNING_ONLY,          \
    .storage = {                                                \
      .method = SL_SE_KEY_STORAGE_INTERNAL_IMMUTABLE,           \
      .location = {                                             \
        .slot = SL_SE_KEY_SLOT_APPLICATION_ATTESTATION_KEY,     \
      },                                                        \
    },                                                          \
  }
#endif

/// Key descriptor for internal SE attestation key
/// @note: Can only be used to get the public part
  #define SL_SE_SYSTEM_ATTESTATION_KEY                          \
  {                                                             \
    .type = SL_SE_KEY_TYPE_ECC_P256,                            \
    .flags = SL_SE_KEY_FLAG_NON_EXPORTABLE                      \
             | SL_SE_KEY_FLAG_IS_DEVICE_GENERATED               \
             | SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PRIVATE_KEY \
             | SL_SE_KEY_FLAG_ASYMMETRIC_SIGNING_ONLY,          \
    .storage = {                                                \
      .method = SL_SE_KEY_STORAGE_INTERNAL_IMMUTABLE,           \
      .location = {                                             \
        .slot = SL_SE_KEY_SLOT_SE_ATTESTATION_KEY,              \
      },                                                        \
    },                                                          \
  }

#endif // _SILICON_LABS_SECURITY_FEATURE_VAULT

/// Key descriptor for secure boot public key
#define SL_SE_APPLICATION_SECURE_BOOT_KEY                    \
  {                                                          \
    .type = SL_SE_KEY_TYPE_ECC_P256,                         \
    .flags = SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PUBLIC_KEY \
             | SL_SE_KEY_FLAG_ASYMMETRIC_SIGNING_ONLY,       \
    .storage = {                                             \
      .method = SL_SE_KEY_STORAGE_INTERNAL_IMMUTABLE,        \
      .location = {                                          \
        .slot = SL_SE_KEY_SLOT_APPLICATION_SECURE_BOOT_KEY,  \
      },                                                     \
    },                                                       \
  }

/// Key descriptor for secure debug public key
#define SL_SE_APPLICATION_SECURE_DEBUG_KEY                   \
  {                                                          \
    .type = SL_SE_KEY_TYPE_ECC_P256,                         \
    .flags = SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PUBLIC_KEY \
             | SL_SE_KEY_FLAG_ASYMMETRIC_SIGNING_ONLY,       \
    .storage = {                                             \
      .method = SL_SE_KEY_STORAGE_INTERNAL_IMMUTABLE,        \
      .location = {                                          \
        .slot = SL_SE_KEY_SLOT_APPLICATION_SECURE_DEBUG_KEY, \
      },                                                     \
    },                                                       \
  }

/// Key descriptor for application AES-128 key
#define SL_SE_APPLICATION_AES_128_KEY                   \
  {                                                     \
    .type = SL_SE_KEY_TYPE_AES_128,                     \
    .flags = SL_SE_KEY_FLAG_NON_EXPORTABLE,             \
    .storage = {                                        \
      .method = SL_SE_KEY_STORAGE_INTERNAL_IMMUTABLE,   \
      .location = {                                     \
        .slot = SL_SE_KEY_SLOT_APPLICATION_AES_128_KEY, \
      },                                                \
    },                                                  \
  }

/// Key descriptor for TrustZone root key
#define SL_SE_TRUSTZONE_ROOT_KEY                      \
  {                                                   \
    .type = SL_SE_KEY_TYPE_AES_256,                   \
    .flags = SL_SE_KEY_FLAG_IS_DEVICE_GENERATED,      \
    .storage = {                                      \
      .method = SL_SE_KEY_STORAGE_INTERNAL_IMMUTABLE, \
      .location = {                                   \
        .slot = SL_SE_KEY_SLOT_TRUSTZONE_ROOT_KEY,    \
      },                                              \
    },                                                \
  }
#endif // SL_SE_MANAGER_INTERNAL_KEYS
