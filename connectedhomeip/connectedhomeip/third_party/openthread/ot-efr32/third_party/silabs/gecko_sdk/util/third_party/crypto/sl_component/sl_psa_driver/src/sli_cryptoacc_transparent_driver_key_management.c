/***************************************************************************//**
 * @file
 * @brief Silicon Labs PSA Crypto Transparent Driver Key Management functions.
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

#if defined(CRYPTOACC_PRESENT)

#include "sli_cryptoacc_transparent_types.h"
#include "sli_cryptoacc_transparent_functions.h"
#include "sli_psa_driver_common.h"
#include "cryptoacc_management.h"
// Replace inclusion of psa/crypto_xxx.h with the new psa driver commong
// interface header file when it becomes available.
#include "psa/crypto_platform.h"
#include "psa/crypto_sizes.h"
#include "psa/crypto_struct.h"
#include "ba414ep_config.h"
#include "sx_ecc_keygen_alg.h"
#include "sx_ecc_curves.h"
#include "sx_primitives.h"
#include "sx_memcmp.h"
#include "sx_trng.h"
#include "sx_errors.h"
#include "cryptolib_types.h"
#include <string.h>

psa_status_t sli_cryptoacc_transparent_generate_key(const psa_key_attributes_t *attributes,
                                                    uint8_t *key_buffer,
                                                    size_t key_buffer_size,
                                                    size_t *key_length)
{
#if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR) \
  && (defined(PSA_WANT_ECC_SECP_R1_192)     \
  || defined(PSA_WANT_ECC_SECP_R1_224)      \
  || defined(PSA_WANT_ECC_SECP_R1_256)      \
  || defined(PSA_WANT_ECC_SECP_K1_256))

  // Argument check.
  if (attributes == NULL
      || key_buffer == NULL
      || key_buffer_size == 0
      || key_length == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  psa_key_type_t key_type = psa_get_key_type(attributes);
  psa_ecc_family_t curve_type = PSA_KEY_TYPE_ECC_GET_FAMILY(key_type);
  size_t key_bits = psa_get_key_bits(attributes);

  // Check key type. PSA Crypto defines generate_key to be an invalid call with a key type
  // of public key.
  if (!PSA_KEY_TYPE_IS_ECC_KEY_PAIR(key_type)) {
    return PSA_ERROR_NOT_SUPPORTED;
  }

  // We currently only support R1 or K1
  if (curve_type != PSA_ECC_FAMILY_SECP_R1 && curve_type != PSA_ECC_FAMILY_SECP_K1) {
    return PSA_ERROR_NOT_SUPPORTED;
  }

  // Check sufficient buffer size.
  if (key_buffer_size < PSA_BITS_TO_BYTES(key_bits)) {
    return PSA_ERROR_BUFFER_TOO_SMALL;
  }

  // Grab the correct order for the requested curve
  block_t n = NULL_blk;
  switch (key_bits) {
#if defined(PSA_WANT_ECC_SECP_R1_192)
    case 192:
      if (curve_type == PSA_ECC_FAMILY_SECP_R1) {
        // The order n is stored as the second element in the curve-parameter tuple
        // consisting of (q, n, Gx, Gy, a, b). The length of the parameters is
        // dependent on the length of the corresponding key.
        n = block_t_convert(sx_ecc_curve_p192.params.addr + (1 * sx_ecc_curve_p192.bytesize),
                            sx_ecc_curve_p192.bytesize);
      } else {
        return PSA_ERROR_NOT_SUPPORTED;
      }
      break;
#endif // PSA_WANT_ECC_SECP_R1_192
#if defined(PSA_WANT_ECC_SECP_R1_224)
    case 224:
      if (curve_type == PSA_ECC_FAMILY_SECP_R1) {
        n = block_t_convert(sx_ecc_curve_p224.params.addr + (1 * sx_ecc_curve_p224.bytesize),
                            sx_ecc_curve_p224.bytesize);
      } else {
        return PSA_ERROR_NOT_SUPPORTED;
      }
      break;
#endif // PSA_WANT_ECC_SECP_R1_224
    case 256:
      switch (curve_type) {
#if defined(PSA_WANT_ECC_SECP_R1_256)
        case PSA_ECC_FAMILY_SECP_R1:
          n = block_t_convert(sx_ecc_curve_p256.params.addr + (1 * sx_ecc_curve_p256.bytesize),
                              sx_ecc_curve_p256.bytesize);
          break;
#endif // PSA_WANT_ECC_SECP_R1_256
#if defined(PSA_WANT_ECC_SECP_K1_256)
        case PSA_ECC_FAMILY_SECP_K1:
          n = block_t_convert(sx_ecc_curve_p256k1.params.addr + (1 * sx_ecc_curve_p256k1.bytesize),
                              sx_ecc_curve_p256k1.bytesize);
          break;
#endif // PSA_WANT_ECC_SECP_R1_256
      }
      break;
    default:
      return PSA_ERROR_NOT_SUPPORTED;
  }

  block_t priv = block_t_convert(key_buffer, PSA_BITS_TO_BYTES(key_bits));

  psa_status_t status = cryptoacc_trng_initialize();
  if (status != PSA_SUCCESS) {
    return status;
  }

  // Get random number < n -> private key.
  status = cryptoacc_management_acquire();
  if (status != PSA_SUCCESS) {
    return status;
  }

  struct sx_rng trng = { NULL, sx_trng_fill_blk };
  uint32_t sx_ret = ecc_generate_private_key(n, priv, trng);
  status = cryptoacc_management_release();
  if (sx_ret != CRYPTOLIB_SUCCESS
      || status != PSA_SUCCESS) {
    return PSA_ERROR_HARDWARE_FAILURE;
  }

  *key_length = PSA_BITS_TO_BYTES(key_bits);

  return PSA_SUCCESS;

#else // PSA_WANT_KEY_TYPE_ECC_KEY_PAIR  && PSA_WANT_ECC_*

  (void) attributes;
  (void) key_buffer;
  (void) key_buffer_size;
  (void) key_length;

  return PSA_ERROR_NOT_SUPPORTED;

#endif  // PSA_WANT_KEY_TYPE_ECC_KEY_PAIR  && PSA_WANT_ECC_*
}

psa_status_t sli_cryptoacc_transparent_export_public_key(const psa_key_attributes_t *attributes,
                                                         const uint8_t *key_buffer,
                                                         size_t key_buffer_size,
                                                         uint8_t *data,
                                                         size_t data_size,
                                                         size_t *data_length)
{
#if (defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR)    \
  || defined(PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY)) \
  && (defined(PSA_WANT_ECC_SECP_R1_192)         \
  || defined(PSA_WANT_ECC_SECP_R1_224)          \
  || defined(PSA_WANT_ECC_SECP_R1_256)          \
  || defined(PSA_WANT_ECC_SECP_K1_256))

  // Argument check.
  if (attributes == NULL
      || key_buffer == NULL
      || key_buffer_size == 0
      || data == NULL
      || data_size == 0
      || data_length == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  psa_key_type_t key_type = psa_get_key_type(attributes);
  psa_ecc_family_t curve_type = PSA_KEY_TYPE_ECC_GET_FAMILY(key_type);
  size_t key_bits = psa_get_key_bits(attributes);

  // If the key is stored transparently and is already a public key,
  // let the core handle it.
  if (PSA_KEY_TYPE_IS_ECC_PUBLIC_KEY(key_type)) {
    return PSA_ERROR_NOT_SUPPORTED;
  }

  if (!PSA_KEY_TYPE_IS_ECC(key_type)) {
    return PSA_ERROR_NOT_SUPPORTED;
  }

  // We currently only support R1 or K1
  if (curve_type != PSA_ECC_FAMILY_SECP_R1 && curve_type != PSA_ECC_FAMILY_SECP_K1) {
    return PSA_ERROR_NOT_SUPPORTED;
  }

  if (key_buffer_size < PSA_BITS_TO_BYTES(key_bits)) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // Check sufficient output buffer size.
  if (data_size < PSA_BITS_TO_BYTES(key_bits) * 2 + 1) {
    return PSA_ERROR_BUFFER_TOO_SMALL;
  }

  block_t *domain_ptr = NULL;
  uint32_t curve_flags = 0;
  switch (key_bits) {
#if defined(PSA_WANT_ECC_SECP_R1_192)
    case 192:
      if (curve_type == PSA_ECC_FAMILY_SECP_R1) {
        curve_flags = sx_ecc_curve_p192.pk_flags;
        domain_ptr = (block_t*)&sx_ecc_curve_p192.params;
      } else {
        return PSA_ERROR_NOT_SUPPORTED;
      }
      break;
#endif // PSA_WANT_ECC_SECP_R1_192
#if defined(PSA_WANT_ECC_SECP_R1_224)
    case 224:
      if (curve_type == PSA_ECC_FAMILY_SECP_R1) {
        curve_flags = sx_ecc_curve_p224.pk_flags;
        domain_ptr = (block_t*)&sx_ecc_curve_p224.params;
      } else {
        return PSA_ERROR_NOT_SUPPORTED;
      }
      break;
#endif // PSA_WANT_ECC_SECP_R1_224
    case 256:
      switch (curve_type) {
#if defined(PSA_WANT_ECC_SECP_R1_256)
        case PSA_ECC_FAMILY_SECP_R1:
          curve_flags = sx_ecc_curve_p256.pk_flags;
          domain_ptr = (block_t*)&sx_ecc_curve_p256.params;
          break;
#endif // PSA_WANT_ECC_SECP_R1_256
#if defined(PSA_WANT_ECC_SECP_K1_256)
        case PSA_ECC_FAMILY_SECP_K1:
          curve_flags = sx_ecc_curve_p256k1.pk_flags;
          domain_ptr = (block_t*)&sx_ecc_curve_p256k1.params;
          break;
#endif // PSA_WANT_ECC_SECP_K1_256
      }
      break;
    default:
      return PSA_ERROR_NOT_SUPPORTED;
  }

  block_t priv = block_t_convert(key_buffer, PSA_BITS_TO_BYTES(key_bits));
  block_t pub = block_t_convert(data + 1, PSA_BITS_TO_BYTES(key_bits) * 2);

  psa_status_t status = cryptoacc_management_acquire();
  if (status != PSA_SUCCESS) {
    return status;
  }
  uint32_t sx_ret = ecc_generate_public_key(*domain_ptr,
                                            pub,
                                            priv,
                                            PSA_BITS_TO_BYTES(key_bits),
                                            curve_flags);
  status = cryptoacc_management_release();
  if (sx_ret != CRYPTOLIB_SUCCESS
      || status != PSA_SUCCESS) {
    return PSA_ERROR_HARDWARE_FAILURE;
  }

  data[0] = 0x04;
  *data_length = PSA_BITS_TO_BYTES(key_bits) * 2 + 1;

  return PSA_SUCCESS;

#else // PSA_WANT_KEY_TYPE_ECC_* && PSA_WANT_ECC_*

  (void) attributes;
  (void) key_buffer;
  (void) key_buffer_size;
  (void) data;
  (void) data_size;
  (void) data_length;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_KEY_TYPE_ECC_* && PSA_WANT_ECC_*
}

psa_status_t sli_cryptoacc_transparent_import_key(const psa_key_attributes_t *attributes,
                                                  const uint8_t *data,
                                                  size_t data_length,
                                                  uint8_t *key_buffer,
                                                  size_t key_buffer_size,
                                                  size_t *key_buffer_length,
                                                  size_t *bits)
{
#if (defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR)    \
  || defined(PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY)) \
  && (defined(PSA_WANT_ECC_SECP_R1_192)         \
  || defined(PSA_WANT_ECC_SECP_R1_224)          \
  || defined(PSA_WANT_ECC_SECP_R1_256)          \
  || defined(PSA_WANT_ECC_SECP_K1_256))

  // Argument check.
  if (attributes == NULL
      || data == NULL
      || data_length == 0
      || bits == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  psa_status_t status;
  psa_key_type_t key_type = psa_get_key_type(attributes);
  psa_ecc_family_t curve_type = PSA_KEY_TYPE_ECC_GET_FAMILY(key_type);

  // Transparent driver is not involved in validation of symmetric keys.
  if (!PSA_KEY_TYPE_IS_ECC(key_type)) {
    return PSA_ERROR_NOT_SUPPORTED;
  }

  // We currently only support R1 or K1
  if (curve_type != PSA_ECC_FAMILY_SECP_R1 && curve_type != PSA_ECC_FAMILY_SECP_K1) {
    return PSA_ERROR_NOT_SUPPORTED;
  }

  if (PSA_KEY_TYPE_IS_ECC_KEY_PAIR(key_type)) { // Private key.
    void *modulus_ptr = NULL;
    *bits = psa_get_key_bits(attributes);

    // Determine key bit-size
    if (*bits == 0) {
      *bits = data_length * 8;
    } else {
      if (PSA_BITS_TO_BYTES(*bits) != data_length) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }
    }

    switch (*bits) {
#if defined(PSA_WANT_ECC_SECP_R1_192)
      case 192:
        if (curve_type == PSA_ECC_FAMILY_SECP_R1) {
          // The order n is stored as the second element in the curve-parameter tuple
          // consisting of (q, n, Gx, Gy, a, b). The length of the parameters is
          // dependent on the length of the corresponding key.
          modulus_ptr = sx_ecc_curve_p192.params.addr + (1 * sx_ecc_curve_p192.bytesize);
        } else {
          return PSA_ERROR_NOT_SUPPORTED;
        }
        break;
#endif // PSA_WANT_ECC_SECP_R1_192
#if defined(PSA_WANT_ECC_SECP_R1_224)
      case 224:
        if (curve_type == PSA_ECC_FAMILY_SECP_R1) {
          modulus_ptr = sx_ecc_curve_p224.params.addr + (1 * sx_ecc_curve_p224.bytesize);
        } else {
          return PSA_ERROR_NOT_SUPPORTED;
        }
        break;
#endif // PSA_WANT_ECC_SECP_R1_224
      case 256:
        switch (curve_type) {
#if defined(PSA_WANT_ECC_SECP_R1_256)
          case PSA_ECC_FAMILY_SECP_R1:
            modulus_ptr = sx_ecc_curve_p256.params.addr + (1 * sx_ecc_curve_p256.bytesize);
            break;
#endif // PSA_WANT_ECC_SECP_R1_256
#if defined(PSA_WANT_ECC_SECP_K1_256)
          case PSA_ECC_FAMILY_SECP_K1:
            modulus_ptr = sx_ecc_curve_p256k1.params.addr + (1 * sx_ecc_curve_p256k1.bytesize);
            break;
#endif // PSA_WANT_ECC_SECP_K1_256
        }
        break;
      default:
        return PSA_ERROR_NOT_SUPPORTED;
    }

    status = sli_psa_validate_ecc_weierstrass_privkey(data,
                                                      modulus_ptr,
                                                      data_length);
  } else { // Public key.
    block_t *domain_ptr = NULL;
    uint32_t curve_flags = 0;

    // Check that uncompressed representation is given.
    if (data[0] != 0x04) {
      return PSA_ERROR_INVALID_ARGUMENT;
    }

    // Determine key bit size.
    *bits = (data_length - 1) * 8 / 2;

    switch (*bits) {
#if defined(PSA_WANT_ECC_SECP_R1_192)
      case 192:
        if (curve_type == PSA_ECC_FAMILY_SECP_R1) {
          curve_flags = sx_ecc_curve_p192.pk_flags;
          domain_ptr = (block_t*)&sx_ecc_curve_p192.params;
        } else {
          return PSA_ERROR_NOT_SUPPORTED;
        }
        break;
#endif // PSA_WANT_ECC_SECP_R1_192
#if defined(PSA_WANT_ECC_SECP_R1_224)
      case 224:
        if (curve_type == PSA_ECC_FAMILY_SECP_R1) {
          curve_flags = sx_ecc_curve_p224.pk_flags;
          domain_ptr = (block_t*)&sx_ecc_curve_p224.params;
        } else {
          return PSA_ERROR_NOT_SUPPORTED;
        }
        break;
#endif // PSA_WANT_ECC_SECP_R1_224
      case 256:
        switch (curve_type) {
#if defined(PSA_WANT_ECC_SECP_R1_256)
          case PSA_ECC_FAMILY_SECP_R1:
            curve_flags = sx_ecc_curve_p256.pk_flags;
            domain_ptr = (block_t*)&sx_ecc_curve_p256.params;
            break;
#endif // PSA_WANT_ECC_SECP_R1_256
#if defined(PSA_WANT_ECC_SECP_K1_256)
          case PSA_ECC_FAMILY_SECP_K1:
            curve_flags = sx_ecc_curve_p256k1.pk_flags;
            domain_ptr = (block_t*)&sx_ecc_curve_p256k1.params;
            break;
#endif // PSA_WANT_ECC_SECP_K1_256
        }
        break;
      default:
        return PSA_ERROR_NOT_SUPPORTED;
    }

    block_t point = block_t_convert(data + 1, PSA_BITS_TO_BYTES(*bits) * 2);

    status = cryptoacc_management_acquire();
    if (status != PSA_SUCCESS) {
      return status;
    }
    uint32_t sx_ret = ecc_is_point_on_curve(*domain_ptr,
                                            point,
                                            PSA_BITS_TO_BYTES(*bits),
                                            curve_flags);
    status = cryptoacc_management_release();
    if (status != PSA_SUCCESS) {
      return status;
    }
    if (sx_ret != CRYPTOLIB_SUCCESS) {
      return PSA_ERROR_INVALID_ARGUMENT;
    } else {
      status = PSA_SUCCESS;
    }
  }

  if ( status == PSA_SUCCESS ) {
    if ( key_buffer_size >= data_length ) {
      memcpy(key_buffer, data, data_length);
      *key_buffer_length = data_length;
    } else {
      status = PSA_ERROR_BUFFER_TOO_SMALL;
    }
  }

  return status;

#else // PSA_WANT_KEY_TYPE_ECC_* && PSA_WANT_ECC_*

  (void) attributes;
  (void) data;
  (void) data_length;
  (void) key_buffer;
  (void) key_buffer_size;
  (void) key_buffer_length;
  (void) bits;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_KEY_TYPE_ECC_* && PSA_WANT_ECC_*
}

#endif // defined(CRYPTOACC_PRESENT)
