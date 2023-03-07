/***************************************************************************//**
 * @file
 * @brief PSA Driver common utility functions
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "sli_psa_driver_common.h"
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

psa_status_t sli_psa_validate_pkcs7_padding(uint8_t *padded_data,
                                            size_t padded_data_length,
                                            uint8_t padding_bytes)
{
  // Check that the last padding byte is valid.
  // Note that the below checks are valid for both partial block padding
  // and complete padding blocks.
  size_t invalid_padding = 0;
  invalid_padding |= (padding_bytes > padded_data_length);
  invalid_padding |= (padding_bytes == 0);

  // Check that every padding byte is correct (equal to padding_bytes)
  size_t pad_index = padded_data_length - padding_bytes;
  for (size_t i = 0; i < padded_data_length; ++i) {
    // The number of checks should be independent of padding_bytes,
    // so use pad_index instead to make the result zero for non-padding
    // bytes in out_buf.
    invalid_padding |= (padded_data[i] ^ padding_bytes) * (i >= pad_index);
  }

  return invalid_padding ? PSA_ERROR_INVALID_PADDING : PSA_SUCCESS;
}
