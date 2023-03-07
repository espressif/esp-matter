/***************************************************************************//**
 * @file
 * @brief Silicon Labs PSA Crypto Driver Key Derivation functions.
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

#include <string.h>

#include "psa/crypto.h"
#include "sli_se_opaque_functions.h"
#include "sli_se_driver_key_management.h"
#include "sli_se_driver_key_derivation.h"
#include "sli_se_version_dependencies.h"

#include "sl_se_manager.h"
#include "sl_se_manager_key_derivation.h"
#include "sl_se_manager_util.h"
#include "sli_se_manager_internal.h"

// -----------------------------------------------------------------------------
// Function Definitions

#if defined(PSA_WANT_ALG_HKDF) \
  && (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)

psa_status_t sli_se_driver_single_shot_hkdf(
  psa_algorithm_t alg,
  const psa_key_attributes_t *key_in_attributes,
  const uint8_t *key_in_buffer,
  size_t key_in_buffer_size,
  const uint8_t* info,
  size_t info_length,
  const uint8_t* salt,
  size_t salt_length,
  const psa_key_attributes_t *key_out_attributes,
  uint8_t *key_out_buffer,
  size_t key_out_buffer_size)
{
  // This driver function will not be called unless alg is of HKDF type.
  sl_se_hash_type_t sl_hash_alg = SL_SE_HASH_NONE;
  psa_algorithm_t psa_hash_alg = PSA_ALG_HKDF_GET_HASH(alg);
  switch (psa_hash_alg) {
    case PSA_ALG_SHA_1:
      sl_hash_alg = SL_SE_HASH_SHA1;
      break;
    case PSA_ALG_SHA_224:
      sl_hash_alg = SL_SE_HASH_SHA224;
      break;
    case PSA_ALG_SHA_256:
      sl_hash_alg = SL_SE_HASH_SHA256;
      break;
    case PSA_ALG_SHA_384:
      sl_hash_alg = SL_SE_HASH_SHA384;
      break;
    case PSA_ALG_SHA_512:
      sl_hash_alg = SL_SE_HASH_SHA512;
      break;
    default:
      return PSA_ERROR_NOT_SUPPORTED;
  }

  // Create input key descriptor.
  sl_se_key_descriptor_t key_in_desc = { 0 };
  psa_status_t psa_status = sli_se_key_desc_from_input(key_in_attributes,
                                                       key_in_buffer,
                                                       key_in_buffer_size,
                                                       &key_in_desc);
  if (psa_status != PSA_SUCCESS) {
    return psa_status;
  }

  size_t key_out_size = PSA_BITS_TO_BYTES(psa_get_key_bits(key_out_attributes));

  // Check that we don't request more than 255 times the hash digest size.
  // This limitation comes from RFC-5869.
  if (key_out_size > 255 * PSA_HASH_LENGTH(psa_hash_alg)) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // Create output key descriptor.
  sl_se_key_descriptor_t key_out_desc = { 0 };
  psa_status = sli_se_key_desc_from_psa_attributes(
    key_out_attributes,
    key_out_size,
    &key_out_desc);
  if (psa_status != PSA_SUCCESS) {
    return psa_status;
  }

  psa_status = sli_se_set_key_desc_output(key_out_attributes,
                                          key_out_buffer,
                                          key_out_buffer_size,
                                          key_out_size,
                                          &key_out_desc);
  if (psa_status != PSA_SUCCESS) {
    return psa_status;
  }

  // Prepare SE command context.
  sl_se_command_context_t cmd_ctx = { 0 };
  sl_status_t sl_status = sl_se_init_command_context(&cmd_ctx);
  if (sl_status != SL_STATUS_OK) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // Execute the SE command.
  sl_status = sl_se_derive_key_hkdf(&cmd_ctx,
                                    &key_in_desc,
                                    sl_hash_alg,
                                    salt,
                                    salt_length,
                                    info,
                                    info_length,
                                    &key_out_desc);
  if (sl_status != SL_STATUS_OK) {
    return PSA_ERROR_HARDWARE_FAILURE;
  } else {
    psa_status = PSA_SUCCESS;
  }

  if (PSA_KEY_LIFETIME_GET_LOCATION(psa_get_key_lifetime(key_out_attributes))
      == PSA_KEY_LOCATION_SLI_SE_OPAQUE) {
    // Add the key desc to the output array for opaque keys.
    psa_status = store_key_desc_in_context(&key_out_desc,
                                           key_out_buffer,
                                           key_out_buffer_size);
  }

  return psa_status;
}

#endif // PSA_WANT_ALG_HKDF && VAULT

#if (defined(PSA_WANT_ALG_PBKDF2_AES_CMAC_PRF_128) || defined(PSA_WANT_ALG_PBKDF2_HMAC)) \
  && (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)

psa_status_t sli_se_driver_single_shot_pbkdf2(
  psa_algorithm_t alg,
  const psa_key_attributes_t *key_in_attributes,
  const uint8_t *key_in_buffer,
  size_t key_in_buffer_size,
  const uint8_t* salt,
  size_t salt_length,
  const psa_key_attributes_t *key_out_attributes,
  uint32_t iterations,
  uint8_t *key_out_buffer,
  size_t key_out_buffer_size)
{
  sl_se_hash_type_t sl_prf = SL_SE_HASH_NONE;
  psa_algorithm_t psa_hash_alg = PSA_ALG_GET_HASH(alg);

  switch (psa_hash_alg) {
    case PSA_ALG_SHA_1:
      sl_prf = SL_SE_PRF_HMAC_SHA1;
      break;
    case PSA_ALG_SHA_224:
      sl_prf = SL_SE_PRF_HMAC_SHA224;
      break;
    case PSA_ALG_SHA_256:
      sl_prf = SL_SE_PRF_HMAC_SHA256;
      break;
    case PSA_ALG_SHA_384:
      sl_prf = SL_SE_PRF_HMAC_SHA384;
      break;
    case PSA_ALG_SHA_512:
      sl_prf = SL_SE_PRF_HMAC_SHA512;
      break;
    default:
      if (alg == PSA_ALG_PBKDF2_AES_CMAC_PRF_128) {
        sl_prf = SL_SE_PRF_AES_CMAC_128;
        break;
      }
      return PSA_ERROR_NOT_SUPPORTED;
  }

  // Create input key descriptor.
  sl_se_key_descriptor_t key_in_desc = { 0 };
  psa_status_t psa_status = sli_se_key_desc_from_input(key_in_attributes,
                                                       key_in_buffer,
                                                       key_in_buffer_size,
                                                       &key_in_desc);
  if (psa_status != PSA_SUCCESS) {
    return psa_status;
  }

  size_t key_out_size = PSA_BITS_TO_BYTES(psa_get_key_bits(key_out_attributes));

  if ( alg == PSA_ALG_PBKDF2_AES_CMAC_PRF_128 ) {
    #define AES_CMAC_PRF_128_BLOCK_SIZE 128
    // The out key length can atmost be 128 bits long.
    if ( !key_out_size || (key_out_size > PSA_BITS_TO_BYTES(AES_CMAC_PRF_128_BLOCK_SIZE)) ) {
      return PSA_ERROR_INVALID_ARGUMENT;
    }
  } else { // HMAC based
    // In conformance with rfc 8018 (sec 5.2), max output length should not exceed
    // 2 ^ 32 -1 * hlen.
    // Our max key size is limited by type of key bits in attributes, so no further
    // validation is necessary.Our key out size is narrower than the rfc specification.
    if ( !key_out_size ) {
      return PSA_ERROR_INVALID_ARGUMENT;
    }
  }

  if ( !iterations ) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // Create output key descriptor.
  sl_se_key_descriptor_t key_out_desc = { 0 };
  psa_status = sli_se_key_desc_from_psa_attributes(
    key_out_attributes,
    key_out_size,
    &key_out_desc);
  if (psa_status != PSA_SUCCESS) {
    return psa_status;
  }

  psa_status = sli_se_set_key_desc_output(key_out_attributes,
                                          key_out_buffer,
                                          key_out_buffer_size,
                                          key_out_size,
                                          &key_out_desc);
  if (psa_status != PSA_SUCCESS) {
    return psa_status;
  }

  // Prepare SE command context.
  sl_se_command_context_t cmd_ctx = { 0 };
  sl_status_t sl_status = sl_se_init_command_context(&cmd_ctx);
  if (sl_status != SL_STATUS_OK) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // Execute the SE command.
  sl_status = sl_se_derive_key_pbkdf2(&cmd_ctx,
                                      &key_in_desc,
                                      sl_prf,
                                      salt,
                                      salt_length,
                                      iterations,
                                      &key_out_desc);
  if (sl_status != SL_STATUS_OK) {
    return PSA_ERROR_HARDWARE_FAILURE;
  } else {
    psa_status = PSA_SUCCESS;
  }

  if (PSA_KEY_LIFETIME_GET_LOCATION(psa_get_key_lifetime(key_out_attributes))
      == PSA_KEY_LOCATION_SLI_SE_OPAQUE) {
    // Add the key desc to the output array for opaque keys.
    psa_status = store_key_desc_in_context(&key_out_desc,
                                           key_out_buffer,
                                           key_out_buffer_size);
  }

  return psa_status;
}

#endif // (PSA_WANT_ALG_PBKDF2_AES_CMAC_PRF_128 || PSA_WANT_ALG_PBKDF2_HMAC) && VAULT

psa_status_t sli_se_driver_key_agreement(psa_algorithm_t alg,
                                         const psa_key_attributes_t *attributes,
                                         const uint8_t *key_buffer,
                                         size_t key_buffer_size,
                                         const uint8_t *peer_key,
                                         size_t peer_key_length,
                                         uint8_t *output,
                                         size_t output_size,
                                         size_t *output_length)
{
#if defined(SLI_PSA_WANT_ALG_ECDH)

  sl_se_key_descriptor_t priv_desc = { 0 };
  sl_se_key_descriptor_t pub_desc = { 0 };
  sl_se_key_descriptor_t shared_desc = { 0 };
  sl_se_command_context_t cmd_ctx = SL_SE_COMMAND_CONTEXT_INIT;
  sl_status_t sl_status = SL_STATUS_FAIL;
  psa_status_t psa_status = PSA_ERROR_CORRUPTION_DETECTED;

  #if defined(SLI_SE_KEY_PADDING_REQUIRED)
  #if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
  size_t padding_bytes = 0;
  #endif // Vault High
  uint8_t tmp_output_buf[SLI_SE_MAX_PADDED_ECP_PUBLIC_KEY_SIZE] = { 0 };
  #else
  uint8_t tmp_output_buf[SLI_SE_MAX_ECP_PUBLIC_KEY_SIZE] = { 0 };
  #endif // defined(SLI_SE_KEY_PADDING_REQUIRED)

  // Argument check.
  if (attributes == NULL
      || key_buffer == NULL
      || peer_key == NULL
      || output == NULL
      || output_length == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  psa_key_type_t key_type = psa_get_key_type(attributes);
  size_t key_bits = psa_get_key_bits(attributes);

  // Check that key_buffer contains private key.
  if (PSA_KEY_TYPE_IS_PUBLIC_KEY(key_type)) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // Only accelerate ECDH.
  if (!PSA_ALG_IS_ECDH(alg)) {
    return PSA_ERROR_NOT_SUPPORTED;
  }

  // Check private key buffer.
  if (key_buffer_size < PSA_BITS_TO_BYTES(key_bits)) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // Check sufficient output buffer size.
  if (output_size < PSA_BITS_TO_BYTES(key_bits)) {
    return PSA_ERROR_BUFFER_TOO_SMALL;
  }

  #if defined(SLI_SE_VERSION_ECDH_PUBKEY_VALIDATION_UNCERTAIN)
  sl_status = sl_se_init_command_context(&cmd_ctx);
  if (sl_status != SL_STATUS_OK) {
    return PSA_ERROR_HARDWARE_FAILURE;
  }
  uint32_t se_version = 0;
  sl_status = sl_se_get_se_version(&cmd_ctx, &se_version);
  if (sl_status != SL_STATUS_OK) {
    return PSA_ERROR_HARDWARE_FAILURE;
  }
  se_version = SLI_VERSION_REMOVE_DIE_ID(se_version);

  // External public key validation is required for older versions of SE FW.
  if (SLI_SE_VERSION_PUBKEY_VALIDATION_REQUIRED(se_version)) {
      #if defined(MBEDTLS_ECP_C)     \
    && defined(MBEDTLS_PSA_CRYPTO_C) \
    && defined(SL_SE_SUPPORT_FW_PRIOR_TO_1_2_2)
    psa_status = sli_se_driver_validate_pubkey_with_fallback(key_type,
                                                             key_bits,
                                                             peer_key,
                                                             peer_key_length);
    if (psa_status != PSA_SUCCESS) {
      return psa_status;
    }
      #else
    // No fallback code is compiled in, cannot do public key validation.
    return PSA_ERROR_NOT_SUPPORTED;
      #endif
  }
  #endif // SLI_SE_VERSION_ECDH_PUBKEY_VALIDATION_UNCERTAIN

  switch (key_type) {
    #if defined(SLI_PSA_WANT_ECC_SECP)
    case PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1):
      switch (key_bits) {
        #if defined(PSA_WANT_ECC_SECP_R1_192)
        case 192:
          pub_desc.type = SL_SE_KEY_TYPE_ECC_P192;
          break;
        #endif // PSA_WANT_ECC_SECP_R1_192
        #if defined(PSA_WANT_ECC_SECP_R1_224)
        case 224:
          #if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_1)
          // Series-2-config-1 devices do not support SECP224R1.
          return PSA_ERROR_NOT_SUPPORTED;
          #else
          pub_desc.type = SL_SE_KEY_TYPE_ECC_P224;
          #endif
          break;
        #endif // PSA_WANT_ECC_SECP_R1_224
        #if defined(PSA_WANT_ECC_SECP_R1_256)
        case 256:
          pub_desc.type = SL_SE_KEY_TYPE_ECC_P256;
          break;
        #endif // PSA_WANT_ECC_SECP_R1_256

        #if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
        #if defined(PSA_WANT_ECC_SECP_R1_384)
        case 384:
          pub_desc.type = SL_SE_KEY_TYPE_ECC_P384;
          break;
        #endif // PSA_WANT_ECC_SECP_R1_384
        #if defined(PSA_WANT_ECC_SECP_R1_521)
        case 521:
          pub_desc.type = SL_SE_KEY_TYPE_ECC_P521;
          #if defined(SLI_SE_KEY_PADDING_REQUIRED)
          padding_bytes = SLI_SE_P521_PADDING_BYTES;
          #endif
          break;
        #endif // PSA_WANT_ECC_SECP_R1_521
        #endif // (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
        default:
          return PSA_ERROR_NOT_SUPPORTED;
      }
      // Set key descriptor attributes.
      // If padding is required, the descriptor will be set later as part of the padding.
      // If padding is not required, set the descriptor here.
      if (pub_desc.type != 0
          #if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT) \
          && defined(SLI_SE_KEY_PADDING_REQUIRED)
          && padding_bytes == 0
          #endif
          ) {
        sli_se_key_descriptor_set_plaintext(&pub_desc,
                                            peer_key + 1,
                                            peer_key_length - 1);
        sli_se_key_descriptor_set_plaintext(&shared_desc,
                                            tmp_output_buf,
                                            sizeof(tmp_output_buf));
        shared_desc.size = PSA_BITS_TO_BYTES(key_bits) * 2;
      }
      break;
    #endif // SLI_PSA_WANT_ECC_SECP
    #if defined(SLI_PSA_WANT_ECC_MONTGOMERY)
    case PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_MONTGOMERY):

      // Check peer_key is of sufficient size.
      if (peer_key_length < PSA_BITS_TO_BYTES(key_bits)) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }

      switch (key_bits) {
        #if defined(SLI_PSA_WANT_ECC_MONTGOMERY_255)
        case 255:
          pub_desc.type = SL_SE_KEY_TYPE_ECC_X25519;
          break;
        #endif // SLI_PSA_WANT_ECC_MONTGOMERY_255
        #if defined(SLI_PSA_WANT_ECC_MONTGOMERY_448)
        case 448:
          pub_desc.type = SL_SE_KEY_TYPE_ECC_X448;
          break;
        #endif // SLI_PSA_WANT_ECC_MONTGOMERY_448
        default:
          return PSA_ERROR_NOT_SUPPORTED;
      }

      // Set key descriptor attributes.
      sli_se_key_descriptor_set_plaintext(&pub_desc,
                                          peer_key,
                                          peer_key_length);
      sli_se_key_descriptor_set_plaintext(&shared_desc,
                                          output,
                                          output_size);
      shared_desc.size = PSA_BITS_TO_BYTES(key_bits);
      break;
    #endif // SLI_PSA_WANT_ECC_MONTGOMERY
    default:
      return PSA_ERROR_NOT_SUPPORTED;
  }

  #if defined(SLI_PSA_WANT_ECC_SECP) || defined(SLI_PSA_WANT_ECC_MONTGOMERY)
  // Generate a key descriptor for private key.
  psa_status = sli_se_key_desc_from_input(attributes,
                                          key_buffer,
                                          key_buffer_size,
                                          &priv_desc);
  if (psa_status != PSA_SUCCESS) {
    return psa_status;
  }

  // Panther crypto engine requires alignment on word boundries instead of byte
  // boundaries which is used in the PSA crypto API.
  #if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT) \
  && defined(SLI_SE_KEY_PADDING_REQUIRED)
  uint8_t tmp_priv_padded_buf[SLI_SE_MAX_PADDED_ECP_PRIVATE_KEY_SIZE] = { 0 };
  uint8_t tmp_pub_padded_buf[SLI_SE_MAX_PADDED_ECP_PUBLIC_KEY_SIZE] = { 0 };

  // Should currently only happen for curve P521.
  if (padding_bytes > 0) {
    // Can only do padding on non-wrapped keys.
    if (PSA_KEY_LIFETIME_GET_LOCATION(psa_get_key_lifetime(attributes))
        == PSA_KEY_LOCATION_LOCAL_STORAGE) {
      // Pad private key.
      sli_se_pad_big_endian(tmp_priv_padded_buf, key_buffer,
                            PSA_BITS_TO_BYTES(key_bits));

      // Re-set key descriptor attributes.
      sli_se_key_descriptor_set_plaintext(&priv_desc,
                                          tmp_priv_padded_buf,
                                          sizeof(tmp_priv_padded_buf));
    }

    // Pad public key.
    sli_se_pad_curve_point(tmp_pub_padded_buf, peer_key + 1,
                           PSA_BITS_TO_BYTES(key_bits));

    // Set key descriptor attributes.
    sli_se_key_descriptor_set_plaintext(&pub_desc,
                                        tmp_pub_padded_buf,
                                        sizeof(tmp_pub_padded_buf));
    sli_se_key_descriptor_set_plaintext(&shared_desc,
                                        tmp_output_buf,
                                        sizeof(tmp_output_buf));
    shared_desc.size = (PSA_BITS_TO_BYTES(key_bits) + padding_bytes) * 2;
  }
  #endif // VAULT padding

  // Set key descriptor attributes that are common to all supported curves.
  pub_desc.flags |= SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PUBLIC_KEY;
  shared_desc.type = SL_SE_KEY_TYPE_SYMMETRIC;

  // Re-init SE command context.
  sl_status = sl_se_init_command_context(&cmd_ctx);
  if (sl_status != SL_STATUS_OK) {
    return PSA_ERROR_HARDWARE_FAILURE;
  }

  // Perform key agreement algorithm (ECDH).
  sl_status = sl_se_ecdh_compute_shared_secret(&cmd_ctx,
                                               &priv_desc,
                                               &pub_desc,
                                               &shared_desc);
  if (sl_status != SL_STATUS_OK) {
    if (sl_status == SL_STATUS_COMMAND_IS_INVALID) {
      // This error will be returned if the key type isn't supported.
      return PSA_ERROR_NOT_SUPPORTED;
    } else {
      // If the ECDH operation failed, this is most likely due to the peer key
      // being an invalid elliptic curve point. Other sources for failure should
      // hopefully have been caught during parameter validation.
      return PSA_ERROR_INVALID_ARGUMENT;
    }
  }

  #if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT) \
  && defined(SLI_SE_KEY_PADDING_REQUIRED)
  // Remove padding bytes and clean up temporary key storage.
  if (padding_bytes > 0) {
    sli_se_unpad_curve_point(tmp_output_buf,
                             tmp_output_buf,
                             PSA_BITS_TO_BYTES(key_bits));
    memset(tmp_priv_padded_buf, 0, sizeof(tmp_priv_padded_buf));
  }
  #endif // VAULT padding

  // Montgomery curve computations do not require the temporary buffer to store the y-coord.
  if (key_type == PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1)) {
    memcpy(output, tmp_output_buf, PSA_BITS_TO_BYTES(key_bits));
    memset(tmp_output_buf, 0, sizeof(tmp_output_buf));
  }

  *output_length = PSA_BITS_TO_BYTES(key_bits);

  return PSA_SUCCESS;
  #endif // SLI_PSA_WANT_ECC_SECP || SLI_PSA_WANT_ECC_MONTGOMERY
#else // SLI_PSA_WANT_ALG_ECDH

  (void) attributes;
  (void) key_buffer;
  (void) peer_key;
  (void) output;
  (void) output_length;
  (void) alg;
  (void) key_buffer_size;
  (void) peer_key_length;
  (void) output_size;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // SLI_PSA_WANT_ALG_ECDH
}

#endif // SEMAILBOX_PRESENT
