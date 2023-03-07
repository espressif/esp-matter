/***************************************************************************//**
 * @file
 * @brief Silicon Labs PSA CRYPTO utility functions
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
#include "sli_psa_crypto.h"
#include "psa/crypto.h"

size_t sli_psa_context_get_size(sli_psa_context_name_t ctx_type)
{
  switch (ctx_type) {
    case SLI_PSA_CONTEXT_ENUM_NAME(psa_hash_operation_t):
      return sizeof(psa_hash_operation_t);
    case SLI_PSA_CONTEXT_ENUM_NAME(psa_cipher_operation_t):
      return sizeof(psa_cipher_operation_t);
    case SLI_PSA_CONTEXT_ENUM_NAME(psa_mac_operation_t):
      return sizeof(psa_mac_operation_t);
    case SLI_PSA_CONTEXT_ENUM_NAME(psa_aead_operation_t):
      return sizeof(psa_aead_operation_t);
    case SLI_PSA_CONTEXT_ENUM_NAME(psa_key_derivation_operation_t):
      return sizeof(psa_key_derivation_operation_t);
    case SLI_PSA_CONTEXT_ENUM_NAME(psa_key_attributes_t):
      return sizeof(psa_key_attributes_t);
    default:
      return 0;
  }
}
