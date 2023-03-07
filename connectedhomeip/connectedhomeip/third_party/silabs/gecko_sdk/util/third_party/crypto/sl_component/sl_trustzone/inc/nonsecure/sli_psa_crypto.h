/***************************************************************************//**
 * @file
 * @brief Silicon Labs PSA Crypto utility functions.
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
#ifndef SLI_PSA_CRYPTO_H
#define SLI_PSA_CRYPTO_H

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Convert a type name into an enum entry name, since enum entries and type
// names share the same C namespace.
#define SLI_PSA_CONTEXT_ENUM_NAME(NAME) \
  NAME ## _e
// Convenience macro for getting the size of a PSA context structure type
#define SLI_PSA_CONTEXT_GET_RUNTIME_SIZE(NAME) \
  (sli_psa_context_get_size(SLI_PSA_CONTEXT_ENUM_NAME(NAME)))

// Type names supported by sli_psa_context_get_size
typedef enum {
  SLI_PSA_CONTEXT_ENUM_NAME(psa_hash_operation_t),
  SLI_PSA_CONTEXT_ENUM_NAME(psa_cipher_operation_t),
  SLI_PSA_CONTEXT_ENUM_NAME(psa_mac_operation_t),
  SLI_PSA_CONTEXT_ENUM_NAME(psa_aead_operation_t),
  SLI_PSA_CONTEXT_ENUM_NAME(psa_key_derivation_operation_t),
  SLI_PSA_CONTEXT_ENUM_NAME(psa_key_attributes_t)
} sli_psa_context_name_t;

/***************************************************************************//**
 * @brief
 *   Get the size of a named PSA context structure. This is valuable for code
 *   shipping as precompiled libraries and needing to link with a source version
 *   of PSA Crypto, since the context structures can change in size based on
 *   configuration options which might not have been present at library
 *   compilation time.
 *
 * @param ctx_type
 *   Which context structure to get the size of. Use
 *   #SLI_PSA_CONTEXT_ENUM_NAME(psa_xxx_operation_t) as argument.
 *
 * @return
 *   Size (in bytes) of the context structure as expected by the current build.
 ******************************************************************************/
size_t sli_psa_context_get_size(sli_psa_context_name_t ctx_type);

#ifdef __cplusplus
}
#endif

/// @endcond

#endif // SLI_PSA_CRYPTO_H
