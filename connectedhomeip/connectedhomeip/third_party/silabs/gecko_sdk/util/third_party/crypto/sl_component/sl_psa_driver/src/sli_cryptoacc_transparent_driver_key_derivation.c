/***************************************************************************//**
 * @file
 * @brief Silicon Labs PSA Crypto Transparent Driver Key Derivation functions.
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
#include "cryptoacc_management.h"
// Replace inclusion of psa/crypto_xxx.h with the new psa driver commong
// interface header file when it becomes available.
#include "psa/crypto_platform.h"
#include "psa/crypto_sizes.h"
#include "psa/crypto_struct.h"

#if defined(PSA_WANT_ALG_ECDH) && defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR)
#include "sx_dh_alg.h"
#include "sx_ecc_curves.h"
#include "sx_errors.h"
#include "cryptolib_types.h"
#include <string.h>
#endif /* #if defined(PSA_WANT_ALG_ECDH)
           && defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR) */

psa_status_t sli_cryptoacc_transparent_key_agreement(psa_algorithm_t alg,
                                                     const psa_key_attributes_t *attributes,
                                                     const uint8_t *key_buffer,
                                                     size_t key_buffer_size,
                                                     const uint8_t *peer_key,
                                                     size_t peer_key_length,
                                                     uint8_t *output,
                                                     size_t output_size,
                                                     size_t *output_length)
{
#if defined(PSA_WANT_ALG_ECDH) && defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR) \
  && (defined(PSA_WANT_ECC_SECP_R1_192)                                   \
  || defined(PSA_WANT_ECC_SECP_R1_224)                                    \
  || defined(PSA_WANT_ECC_SECP_R1_256)                                    \
  || defined(PSA_WANT_ECC_SECP_K1_256))

  // Argument check.
  if (attributes == NULL
      || key_buffer == NULL
      || peer_key == NULL
      || output == NULL
      || output_length == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
  uint32_t sx_ret = CRYPTOLIB_CRYPTO_ERR;
  uint32_t curve_flags = 0;
  block_t domain = NULL_blk;
  uint8_t tmp_output_buf[64] = { 0 };
  size_t key_bits = psa_get_key_bits(attributes);
  psa_key_type_t key_type = psa_get_key_type(attributes);

  // Check that key_buffer contains private key.
  if (PSA_KEY_TYPE_IS_PUBLIC_KEY(key_type)) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // Only accelerate ECDH over secp{192, 224, 256}r1 and secp256k1 curves.
  if (!PSA_ALG_IS_ECDH(alg)) {
    return PSA_ERROR_NOT_SUPPORTED;
  }

  switch (key_bits) {
#if defined(PSA_WANT_ECC_SECP_R1_192)
    case 192:
      if (key_type != PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1)) {
        return PSA_ERROR_NOT_SUPPORTED;
      }
      curve_flags = sx_ecc_curve_p192.pk_flags;
      domain = block_t_convert(sx_ecc_curve_p192.params.addr,
                               6 * sx_ecc_curve_p192.bytesize);
      break;
#endif // PSA_WANT_ECC_SECP_R1_192
#if defined(PSA_WANT_ECC_SECP_R1_224)
    case 224:
      if (key_type != PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1)) {
        return PSA_ERROR_NOT_SUPPORTED;
      }
      curve_flags = sx_ecc_curve_p224.pk_flags;
      domain = block_t_convert(sx_ecc_curve_p224.params.addr,
                               6 * sx_ecc_curve_p224.bytesize);
      break;
#endif // PSA_WANT_ECC_SECP_R1_224
    case 256:
#if defined(PSA_WANT_ECC_SECP_R1_256)
      if (key_type == PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1)) {
        curve_flags = sx_ecc_curve_p256.pk_flags;
        domain = block_t_convert(sx_ecc_curve_p256.params.addr,
                                 6 * sx_ecc_curve_p256.bytesize);
      } else
#endif // PSA_WANT_ECC_SECP_R1_256
#if defined(PSA_WANT_ECC_SECP_K1_256)
      if (key_type == PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_K1)) {
        curve_flags = sx_ecc_curve_p256k1.pk_flags;
        domain = block_t_convert(sx_ecc_curve_p256k1.params.addr,
                                 6 * sx_ecc_curve_p256k1.bytesize);
      } else
#endif // PSA_WANT_ECC_SECP_K1_256
      {
        return PSA_ERROR_NOT_SUPPORTED;
      }
      break;
    default:
      return PSA_ERROR_NOT_SUPPORTED;
  }

  // Check input-keys sizes.
  if (key_buffer_size < PSA_BITS_TO_BYTES(key_bits)
      || peer_key_length < PSA_BITS_TO_BYTES(key_bits) * 2 + 1) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // Check sufficient output buffer size.
  if (output_size < PSA_BITS_TO_BYTES(key_bits)) {
    return PSA_ERROR_BUFFER_TOO_SMALL;
  }

  block_t priv = block_t_convert(key_buffer, PSA_BITS_TO_BYTES(key_bits));
  block_t pub = block_t_convert(peer_key + 1, PSA_BITS_TO_BYTES(key_bits) * 2);
  block_t shared_key = block_t_convert(tmp_output_buf, PSA_BITS_TO_BYTES(key_bits) * 2);

  // Check peer_key is a public key of correct format.
  if (peer_key[0] != 0x04) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // Compute shared key.
  status = cryptoacc_management_acquire();
  if (status != PSA_SUCCESS) {
    return status;
  }
  sx_ret = dh_shared_key_ecdh(domain, priv, pub, shared_key, PSA_BITS_TO_BYTES(key_bits), curve_flags);
  status = cryptoacc_management_release();
  if (sx_ret != CRYPTOLIB_SUCCESS
      || status != PSA_SUCCESS) {
    // If the ECDH libcryptosoc operation failed, this is most likely due to
    // the peer key being an invalid elliptic curve point. Other sources for
    // failure should hopefully have been caught during parameter validation.
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  memcpy(output, tmp_output_buf, PSA_BITS_TO_BYTES(key_bits));
  memset(tmp_output_buf, 0, sizeof(tmp_output_buf));
  *output_length = PSA_BITS_TO_BYTES(key_bits);

  return PSA_SUCCESS;

#else // PSA_WANT_ALG_ECDH && PSA_WANT_KEY_TYPE_ECC_KEY_PAIR && PSA_WANT_ECC_*

  (void) alg;
  (void) attributes;
  (void) key_buffer;
  (void) key_buffer_size;
  (void) peer_key;
  (void) peer_key_length;
  (void) output;
  (void) output_size;
  (void) output_length;

  return PSA_ERROR_NOT_SUPPORTED;

#endif  // PSA_WANT_ALG_ECDH && PSA_WANT_KEY_TYPE_ECC_KEY_PAIR && PSA_WANT_ECC_*
}

#endif // defined(CRYPTOACC_PRESENT)
