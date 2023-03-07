/***************************************************************************//**
 * @file
 * @brief Silicon Labs PSA Crypto Opaque Driver Mac functions.
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

#include "em_device.h"

#if defined(CRYPTOACC_PRESENT) && defined(SEPUF_PRESENT)

#include <string.h>

#include "sli_cryptoacc_opaque_types.h"
#include "sli_psa_driver_common.h"
#include "cryptoacc_management.h"
// Replace inclusion of psa/crypto_xxx.h with the new psa driver common
// interface header file when it becomes available.
#include "psa/crypto_platform.h"
#include "psa/crypto_sizes.h"
#include "psa/crypto_struct.h"
#include "psa/crypto_extra.h"
#include "cryptolib_def.h"
#include "sx_errors.h"
#include "sx_aes.h"

psa_status_t sli_cryptoacc_opaque_mac_compute(const psa_key_attributes_t *attributes,
                                              const uint8_t *key_buffer,
                                              size_t key_buffer_size,
                                              psa_algorithm_t alg,
                                              const uint8_t *input,
                                              size_t input_length,
                                              uint8_t *mac,
                                              size_t mac_size,
                                              size_t *mac_length)
{
#if defined(MBEDTLS_PSA_CRYPTO_BUILTIN_KEYS)
  if (key_buffer == NULL
      || attributes == NULL
      || mac == NULL
      || mac_length == NULL
      || ((input == NULL) && (input_length > 0))) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }
  if (key_buffer_size < sizeof(sli_cryptoacc_opaque_key_context_t)) {
    return PSA_ERROR_BUFFER_TOO_SMALL;
  }

  // The only opaque key that is currently supported is the PUF key
  sli_cryptoacc_opaque_key_context_t *key_context =
    (sli_cryptoacc_opaque_key_context_t *)key_buffer;
  block_t key_block = NULL_blk;
  switch (key_context->builtin_key_id) {
    case SLI_CRYPTOACC_BUILTIN_KEY_PUF_SLOT:
      // Using this key block as input will make the AES engine use the PUF-
      // derived key for the operation.
      // Make sure that the attributes and so on match our expectations
      if (psa_get_key_bits(attributes) != 256) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }
      key_block = AES_KEY1_256;
      break;
    default:
      return PSA_ERROR_INVALID_ARGUMENT;
  }

  psa_status_t status;

  switch (alg) {
    case PSA_ALG_CMAC:
    {
      // The builting key specifies PSA_ALG_CMAC without a truncated length.
      // Therefore, we only support full size MAC output.
      if (mac_size < BLK_CIPHER_MAC_SIZE) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
      }
      uint8_t sx_mac_buf[BLK_CIPHER_MAC_SIZE];
      block_t input_block = block_t_convert(input, input_length);
      block_t mac_block = block_t_convert(sx_mac_buf, sizeof(sx_mac_buf));

      // Acquire hardware lock and execute CMAC operation
      status = cryptoacc_management_acquire();
      if (status != PSA_SUCCESS) {
        return status;
      }
      uint32_t sx_ret = sx_aes_cmac_generate(&key_block,
                                             &input_block,
                                             &mac_block);
      status = cryptoacc_management_release();
      if (sx_ret != CRYPTOLIB_SUCCESS) {
        status = PSA_ERROR_HARDWARE_FAILURE;
      }

      // Output mac if operation is successful
      if (status == PSA_SUCCESS) {
        memcpy(mac, sx_mac_buf, BLK_CIPHER_MAC_SIZE);
        *mac_length = BLK_CIPHER_MAC_SIZE;
      } else {
        *mac_length = 0;
      }
      memset(sx_mac_buf, 0, BLK_CIPHER_MAC_SIZE);
      break;
    }
    default:
      status = PSA_ERROR_NOT_SUPPORTED;
  }

  return status;

#else // MBEDTLS_PSA_CRYPTO_BUILTIN_KEYS

  (void)attributes;
  (void)key_buffer;
  (void)key_buffer_size;
  (void)alg;
  (void)input;
  (void)input_length;
  (void)mac;
  (void)mac_size;
  (void)mac_length;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // MBEDTLS_PSA_CRYPTO_BUILTIN_KEYS
}

#endif // defined(CRYPTOACC_PRESENT) || defined(SEPUF_PRESENT)
