/***************************************************************************//**
 * @file
 * @brief Silicon Labs PSA Crypto Opaque Driver API Types for VSE.
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

#ifndef SLI_CRYPTOACC_OPAQUE_TYPES_H
#define SLI_CRYPTOACC_OPAQUE_TYPES_H

#include "em_device.h"

#if defined(CRYPTOACC_PRESENT) && defined(SEPUF_PRESENT)

#include "sl_psa_values.h"

#if defined(MBEDTLS_PSA_CRYPTO_BUILTIN_KEYS)

/// Context struct for opaque registered keys
typedef struct {
  /// Version field for the struct
  uint8_t struct_version;
  /// Builtin key ID
  uint8_t builtin_key_id;
  /// Reserved space (initialise to all-zero)
  uint8_t reserved[2];
} sli_cryptoacc_opaque_key_context_t;

#endif // MBEDTLS_PSA_CRYPTO_BUILTIN_KEYS

#endif // CRYPTOACC_PRESENT && SEPUF_PRESENT

#endif // SLI_CRYPTOACC_OPAQUE_TYPES_H
