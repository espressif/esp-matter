/***************************************************************************//**
 * @file
 * @brief Silicon Labs PSA Crypto Driver Mac functions.
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

#include "em_device.h"

#if defined(SEMAILBOX_PRESENT)

#include "psa/crypto.h"
#include "mbedtls/platform.h"

#include "sli_se_driver_mac.h"
#include "sli_se_manager_internal.h"
#include "sli_se_driver_key_management.h"
#include "sli_psa_driver_common.h"

#include <string.h>

//------------------------------------------------------------------------------
// Static functions

#if defined(PSA_WANT_ALG_HMAC)
sl_se_hash_type_t sli_se_hash_type_from_psa_hmac_alg(psa_algorithm_t alg,
                                                     size_t *length)
{
  if (!PSA_ALG_IS_HMAC(alg)) {
    return SL_SE_HASH_NONE;
  }

  psa_algorithm_t hash_alg = PSA_ALG_HMAC_GET_HASH(alg);
  switch (hash_alg) {
    case PSA_ALG_SHA_1:
      *length = 20;
      return SL_SE_HASH_SHA1;
    case PSA_ALG_SHA_224:
      *length = 28;
      return SL_SE_HASH_SHA224;
    case PSA_ALG_SHA_256:
      *length = 32;
      return SL_SE_HASH_SHA256;
    #if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
    case PSA_ALG_SHA_384:
      *length = 48;
      return SL_SE_HASH_SHA384;
    case PSA_ALG_SHA_512:
      *length = 64;
      return SL_SE_HASH_SHA512;
    #endif
    default:
      return SL_SE_HASH_NONE;
  }
}
#endif // PSA_WANT_ALG_HMAC

//------------------------------------------------------------------------------
// Second stage driver entry points

psa_status_t sli_se_driver_mac_compute(sl_se_key_descriptor_t *key_desc,
                                       psa_algorithm_t alg,
                                       const uint8_t *input,
                                       size_t input_length,
                                       uint8_t *mac,
                                       size_t mac_size,
                                       size_t *mac_length)
{
#if defined(PSA_WANT_ALG_HMAC)  \
  || defined(PSA_WANT_ALG_CMAC) \
  || defined(PSA_WANT_ALG_CBC_MAC)

  if (mac == NULL
      || mac_length == NULL
      || key_desc == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  sl_status_t status;
  psa_status_t psa_status = PSA_ERROR_INVALID_ARGUMENT;
  sl_se_command_context_t cmd_ctx = { 0 };

  status = sl_se_init_command_context(&cmd_ctx);
  if (status != SL_STATUS_OK) {
    return PSA_ERROR_HARDWARE_FAILURE;
  }

  #if defined(PSA_WANT_ALG_HMAC)
  if (PSA_ALG_IS_HMAC(alg)) {
    #if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
    uint8_t tmp_hmac[64];
    #else // VAULT
    uint8_t tmp_hmac[32];
    #endif // VAULT

    size_t requested_length = 0;
    sl_se_hash_type_t hash_type =
      sli_se_hash_type_from_psa_hmac_alg(alg, &requested_length);
    if (hash_type == SL_SE_HASH_NONE) {
      return PSA_ERROR_NOT_SUPPORTED;
    }

    if (PSA_MAC_TRUNCATED_LENGTH(alg) > requested_length) {
      return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (PSA_MAC_TRUNCATED_LENGTH(alg) > 0) {
      requested_length = PSA_MAC_TRUNCATED_LENGTH(alg);
    }

    if (mac_size < requested_length) {
      return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    #if defined(SLI_SE_KEY_PADDING_REQUIRED)
    uint8_t *temp_key_buf = NULL;
    uint32_t key_buffer_size = key_desc->storage.location.buffer.size;
    size_t padding = sli_se_get_padding(key_buffer_size);

    if (padding > 0u) {
      // We can only manipulate the transparent keys.
      if (key_desc->storage.method == SL_SE_KEY_STORAGE_EXTERNAL_PLAINTEXT) {
        size_t word_aligned_buffer_size
          = sli_se_word_align(key_desc->storage.location.buffer.size);
        temp_key_buf = mbedtls_calloc(1, word_aligned_buffer_size);
        if (temp_key_buf == NULL) {
          return PSA_ERROR_INSUFFICIENT_MEMORY;
        }

        // Since we know that this must be a plaintext key, we can freely
        // modify the key descriptor
        memcpy(temp_key_buf,
               key_desc->storage.location.buffer.pointer,
               key_desc->storage.location.buffer.size);
        key_desc->storage.location.buffer.pointer = temp_key_buf;
        key_desc->storage.location.buffer.size = word_aligned_buffer_size;
      }
    }
    #endif // SLI_SE_KEY_PADDING_REQUIRED

    status = sl_se_hmac(&cmd_ctx,
                        key_desc,
                        hash_type,
                        input,
                        input_length,
                        tmp_hmac,
                        sizeof(tmp_hmac));

    #if defined(SLI_SE_KEY_PADDING_REQUIRED)
    if (padding > 0u) {
      mbedtls_free(temp_key_buf);
    }
    #endif // SLI_SE_KEY_PADDING_REQUIRED

    if (status == PSA_SUCCESS) {
      memcpy(mac, tmp_hmac, requested_length);
      *mac_length = requested_length;
    } else {
      *mac_length = 0;
    }

    memset(tmp_hmac, 0, sizeof(tmp_hmac));

    goto exit;
  }
  #endif // PSA_WANT_ALG_HMAC

  #if defined(PSA_WANT_ALG_HMAC) \
  && (defined(PSA_WANT_ALG_CMAC) || defined(PSA_WANT_ALG_CBC_MAC))
  else
  #endif // PSA_WANT_ALG_HMAC && (PSA_WANT_ALG_CMAC || PSA_WANT_ALG_CBC_MAC)

  #if defined(PSA_WANT_ALG_CMAC) || defined(PSA_WANT_ALG_CBC_MAC)
  {
    size_t output_length = PSA_MAC_TRUNCATED_LENGTH(alg);
    if (output_length == 0) {
      output_length = 16;
    } else if (output_length > 16) {
      return PSA_ERROR_INVALID_ARGUMENT;
    }
    if (mac_size < output_length) {
      return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    switch (PSA_ALG_FULL_LENGTH_MAC(alg)) {
      #if defined(PSA_WANT_ALG_CBC_MAC)
      case PSA_ALG_CBC_MAC: {
        uint8_t tmp_buf[16] = { 0 };
        uint8_t tmp_mac[16] = { 0 };

        if (input_length % 16 != 0 || input_length < 16) {
          return PSA_ERROR_INVALID_ARGUMENT;
        }

        // Do an AES-CBC encrypt with zero IV, keeping only the last block.
        while (input_length > 0) {
          status = sl_se_aes_crypt_cbc(&cmd_ctx,
                                       key_desc,
                                       SL_SE_ENCRYPT,
                                       16,
                                       tmp_mac,
                                       input,
                                       tmp_buf);

          input_length -= 16;
          input += 16;
        }

        // Copy the requested number of bytes (max 16) to the user buffer.
        if (status == SL_STATUS_OK) {
          memcpy(mac, tmp_mac, output_length);
          sli_psa_zeroize(tmp_mac, sizeof(tmp_mac));
          *mac_length = output_length;
        }

        goto exit;
        break;
      }
      #endif // PSA_WANT_ALG_CBC_MAC
      #if defined(PSA_WANT_ALG_CMAC)
      case PSA_ALG_CMAC: {
        uint8_t tmp_mac[16] = { 0 };

        status = sl_se_cmac(&cmd_ctx,
                            key_desc,
                            input,
                            input_length,
                            tmp_mac);

        // Copy the requested number of bytes (max 16) to the user buffer.
        if (status == SL_STATUS_OK) {
          memcpy(mac, tmp_mac, output_length);
          sli_psa_zeroize(tmp_mac, sizeof(tmp_mac));
          *mac_length = output_length;
        }

        goto exit;
        break;
      }
      #endif // PSA_WANT_ALG_CMAC
      default:
        return PSA_ERROR_NOT_SUPPORTED;
        break;
    }
  }
  #endif // PSA_WANT_ALG_CMAC || PSA_WANT_ALG_CBC_MAC

  exit:

  if (status == SL_STATUS_INVALID_PARAMETER) {
    psa_status = PSA_ERROR_INVALID_ARGUMENT;
  } else if (status == SL_STATUS_FAIL) {
    psa_status = PSA_ERROR_DOES_NOT_EXIST;
  } else if (status != SL_STATUS_OK) {
    psa_status = PSA_ERROR_HARDWARE_FAILURE;
  } else {
    psa_status = PSA_SUCCESS;
  }

  // Cleanup
  status = sl_se_deinit_command_context(&cmd_ctx);
  if (status != SL_STATUS_OK) {
    return PSA_ERROR_HARDWARE_FAILURE;
  }

  return psa_status;

#else // PSA_WANT_ALG_HMAC || PSA_WANT_ALG_CMAC || PSA_WANT_ALG_CBC_MAC

  (void)key_desc;
  (void)alg;
  (void)input;
  (void)input_length;
  (void)mac;
  (void)mac_size;
  (void)mac_length;

  return PSA_ERROR_NOT_SUPPORTED;
#endif // PSA_WANT_ALG_HMAC || PSA_WANT_ALG_CMAC || PSA_WANT_ALG_CBC_MAC
}

#if defined(PSA_WANT_ALG_CMAC) || defined(PSA_WANT_ALG_CBC_MAC)

psa_status_t sli_se_driver_mac_sign_setup(
  sli_se_driver_mac_operation_t *operation,
  const psa_key_attributes_t *attributes,
  psa_algorithm_t alg)
{
  if (operation == NULL
      || attributes == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // Start by resetting context
  memset(operation, 0, sizeof(*operation));

  switch (PSA_ALG_FULL_LENGTH_MAC(alg)) {
    #if defined(PSA_WANT_ALG_CBC_MAC)
    case PSA_ALG_CBC_MAC:
      if (psa_get_key_type(attributes) != PSA_KEY_TYPE_AES) {
        return PSA_ERROR_NOT_SUPPORTED;
      }
      if (PSA_MAC_TRUNCATED_LENGTH(alg) > 16) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }
      break;
    #endif // PSA_WANT_ALG_CBC_MAC
    #if defined(PSA_WANT_ALG_CMAC)
    case PSA_ALG_CMAC:
      if (psa_get_key_type(attributes) != PSA_KEY_TYPE_AES) {
        return PSA_ERROR_NOT_SUPPORTED;
      }
      if (PSA_MAC_TRUNCATED_LENGTH(alg) > 16) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }
      break;
    #endif // PSA_WANT_ALG_CMAC
    default:
      return PSA_ERROR_NOT_SUPPORTED;
  }

  operation->alg = alg;
  return PSA_SUCCESS;
}

psa_status_t sli_se_driver_mac_update(sli_se_driver_mac_operation_t *operation,
                                      sl_se_key_descriptor_t *key_desc,
                                      const uint8_t *input,
                                      size_t input_length)
{
  if (operation == NULL
      || (input == NULL && input_length > 0)) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // Ephemeral contexts
  sl_se_command_context_t cmd_ctx = { 0 };

  sl_status_t status = sl_se_init_command_context(&cmd_ctx);
  if (status != SL_STATUS_OK) {
    return PSA_ERROR_HARDWARE_FAILURE;
  }

  psa_status_t psa_status = PSA_ERROR_NOT_SUPPORTED;
  switch (PSA_ALG_FULL_LENGTH_MAC(operation->alg)) {
    #if defined(PSA_WANT_ALG_CBC_MAC)
    case PSA_ALG_CBC_MAC:
      if (input_length == 0) {
        psa_status = PSA_SUCCESS;
        goto exit;
      }

      // Add bytes to the streaming buffer up to the next block boundary
      if (operation->ctx.cbcmac.processed_length % 16 != 0) {
        size_t bytes_to_boundary
          = 16 - operation->ctx.cbcmac.processed_length % 16;
        if (input_length < bytes_to_boundary) {
          memcpy(&operation->ctx.cbcmac.streaming_block[16 - bytes_to_boundary],
                 input,
                 input_length);
          operation->ctx.cbcmac.processed_length += input_length;
          psa_status = PSA_SUCCESS;
          goto exit;
        }

        memcpy(&operation->ctx.cbcmac.streaming_block[16 - bytes_to_boundary],
               input,
               bytes_to_boundary);
        input_length -= bytes_to_boundary;
        input += bytes_to_boundary;
        operation->ctx.cbcmac.processed_length += bytes_to_boundary;

        status = sl_se_aes_crypt_cbc(&cmd_ctx,
                                     key_desc,
                                     SL_SE_ENCRYPT,
                                     16,
                                     operation->ctx.cbcmac.iv,
                                     operation->ctx.cbcmac.streaming_block,
                                     operation->ctx.cbcmac.iv);

        if (status == SL_STATUS_FAIL) {
          psa_status = PSA_ERROR_DOES_NOT_EXIST;
          goto exit;
        } else if (status != SL_STATUS_OK) {
          psa_status = PSA_ERROR_HARDWARE_FAILURE;
          goto exit;
        }
      }

      // Draw all full blocks
      while (input_length >= 16) {
        status = sl_se_aes_crypt_cbc(&cmd_ctx,
                                     key_desc,
                                     SL_SE_ENCRYPT,
                                     16,
                                     operation->ctx.cbcmac.iv,
                                     input,
                                     operation->ctx.cbcmac.iv);

        if (status != SL_STATUS_OK) {
          psa_status = PSA_ERROR_HARDWARE_FAILURE;
          goto exit;
        }

        operation->ctx.cbcmac.processed_length += 16;
        input += 16;
        input_length -= 16;
      }

      if (input_length > 0) {
        memcpy(operation->ctx.cbcmac.streaming_block,
               input,
               input_length);
        operation->ctx.cbcmac.processed_length += input_length;
      }

      psa_status = PSA_SUCCESS;
      goto exit;
    #endif // PSA_WANT_ALG_CBC_MAC
    #if defined(PSA_WANT_ALG_CMAC)
    case PSA_ALG_CMAC:
      if (input_length == 0) {
        psa_status = PSA_SUCCESS;
        goto exit;
      }

      status = sl_se_cmac_multipart_update(&operation->ctx.cmac,
                                           &cmd_ctx,
                                           key_desc,
                                           input,
                                           input_length);
      if (status == SL_STATUS_FAIL) {
        psa_status = PSA_ERROR_DOES_NOT_EXIST;
        goto exit;
      } else if (status != SL_STATUS_OK) {
        psa_status = PSA_ERROR_HARDWARE_FAILURE;
        goto exit;
      }
      psa_status = PSA_SUCCESS;
      goto exit;
    #endif // PSA_WANT_ALG_CMAC
    default:
      psa_status = PSA_ERROR_BAD_STATE;
      goto exit;
  }

  exit:
  // Cleanup
  status = sl_se_deinit_command_context(&cmd_ctx);
  if (status != SL_STATUS_OK) {
    return PSA_ERROR_HARDWARE_FAILURE;
  }

  return psa_status;
}

psa_status_t sli_se_driver_mac_sign_finish(
  sli_se_driver_mac_operation_t *operation,
  sl_se_key_descriptor_t *key_desc,
  uint8_t *mac,
  size_t mac_size,
  size_t *mac_length)
{
  if (operation == NULL
      || mac == NULL
      || mac_size == 0
      || mac_length == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // Set maximum output size to 16 or truncated length
  if (mac_size > 16) {
    mac_size = 16;
  }

  size_t truncated_length = PSA_MAC_TRUNCATED_LENGTH(operation->alg);
  if (truncated_length != 0
      && mac_size > truncated_length) {
    mac_size = truncated_length;
  }

  switch (PSA_ALG_FULL_LENGTH_MAC(operation->alg)) {
    #if defined(PSA_WANT_ALG_CBC_MAC)
    case PSA_ALG_CBC_MAC: {
      (void)key_desc;

      if (operation->ctx.cbcmac.processed_length % 16 != 0) {
        return PSA_ERROR_BAD_STATE;
      }

      // Copy the requested number of bytes (max 16) to the user buffer.
      memcpy(mac, operation->ctx.cbcmac.iv, mac_size);
      *mac_length = mac_size;

      return PSA_SUCCESS;
      break;
    }
    #endif // PSA_WANT_ALG_CBC_MAC
    #if defined(PSA_WANT_ALG_CMAC)
    case PSA_ALG_CMAC: {
      // Ephemeral contexts
      sl_se_command_context_t cmd_ctx = { 0 };
      uint8_t tmp_mac[16] = { 0 };
      sl_status_t status = sl_se_init_command_context(&cmd_ctx);
      if (status != SL_STATUS_OK) {
        return PSA_ERROR_HARDWARE_FAILURE;
      }

      status = sl_se_cmac_multipart_finish(&operation->ctx.cmac,
                                           &cmd_ctx,
                                           key_desc,
                                           tmp_mac);
      if (status != SL_STATUS_OK) {
        *mac_length = 0;
        return PSA_ERROR_HARDWARE_FAILURE;
      }

      // Cleanup
      status = sl_se_deinit_command_context(&cmd_ctx);
      if (status != SL_STATUS_OK) {
        *mac_length = 0;
        return PSA_ERROR_HARDWARE_FAILURE;
      }

      // Copy the requested number of bytes (max 16) to the user buffer.
      memcpy(mac, tmp_mac, mac_size);
      *mac_length = mac_size;

      return PSA_SUCCESS;
      break;
    }
    #endif // PSA_WANT_ALG_CMAC
    default:
      return PSA_ERROR_BAD_STATE;
  }
}

#endif // PSA_WANT_ALG_CMAC || PSA_WANT_ALG_CBC_MAC

#endif // defined(SEMAILBOX_PRESENT)
