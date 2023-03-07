/***************************************************************************//**
 * @file
 * @brief Silicon Labs PSA Crypto common driver functions.
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
#ifndef SLI_PSA_DRIVER_COMMON_H
#define SLI_PSA_DRIVER_COMMON_H

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN

#include "em_device.h"
#include "psa/crypto.h"
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

/*******************************************************************************
 * @brief
 *   Validate the PKCS#7 padding contained in the final block of plaintext
 *   in certain block cipher modes of operation. Based on the get_pkcs_padding()
 *   implementation in Mbed TLS.
 *
 * @param[in] padded_data
 *   A buffer of (at least) size 16 containing the padded final block.
 *
 * @param padded_data_length
 *   The length of the paddad data (should be 16). Parameter is mainly kept used
 *   in order to make it harder for the compiler to optimize out some of the
 *   "time-constantness".
 *
 * @param padding_bytes
 *   The expected padding bytes (likely derived from padded_block[15]).
 *
 * @return
 *   PSA_SUCCESS if the padding is valid, PSA_ERROR_INVALID_PADDING otherwise.
 ******************************************************************************/
psa_status_t sli_psa_validate_pkcs7_padding(uint8_t *padded_data,
                                            size_t padded_data_length,
                                            uint8_t padding_bytes);

__STATIC_INLINE
/*******************************************************************************
 * @brief
 *   Validate that a elliptic curve (in Weierstrass form) private key is valid.
 *   This fuction attempts to operate in constant time.
 *
 * @param[in] privkey
 *   A buffer containing the private key.
 *
 * @param padding_bytes
 *   A buffer containing the modulus (n) to compare the private key against.
 *
 * @return
 *   PSA_SUCCESS if the key is in [1, n-1], PSA_ERROR_INVALID_ARGUMENT otherwise.
 ******************************************************************************/
psa_status_t sli_psa_validate_ecc_weierstrass_privkey(const void *privkey,
                                                      const void *modulus,
                                                      size_t privkey_size)
{
  // Compare private key to maximum allowed value, n - 1,
  // and also check that it is non-zero.

  // Initial values.
  uint8_t non_zero_accumulator = 0;
  int32_t memcmp_res = 0;
  int32_t diff = 0;

  // Loop over every byte in the private key. We start from the end so that
  // the final result we store reflects the first byte which differs between the
  // two numbers (privkey and modulus).
  for (size_t i = 0; i < privkey_size; ++i) {
    // Partial non-zero check operation.
    non_zero_accumulator |= ((uint8_t *)privkey)[privkey_size - 1 - i];

    // Compute the difference between the current bytes being compared.
    diff = ((uint8_t *)privkey)[privkey_size - 1 - i]
           - ((uint8_t *)modulus)[privkey_size - 1 - i];

    // This will only update memcmp_res if the difference is non-zero.
    memcmp_res = (memcmp_res & - !diff) | diff;
  }

  if ((non_zero_accumulator == 0) || (memcmp_res >= 0)) {
    // We have either failed because the private key turned out to be empty,
    // or because the result of the memcmp indicated that the privkey was not
    // smaller than the modulus.
    return PSA_ERROR_INVALID_ARGUMENT;
  } else {
    return PSA_SUCCESS;
  }
}

/***************************************************************************//**
 * @brief
 *   Clear a memory location in a way that is guaranteed not be optimized away
 *   by the compiler.
 *
 * @param[in]  v
 *   Pointer to memory location.
 *
 * @param[in] n
 *   Number of bytes to clear.
 ******************************************************************************/
__STATIC_INLINE
void sli_psa_zeroize(void *v, size_t n)
{
  if (n == 0) {
    return;
  }

  volatile unsigned char *p = v;
  while (n--) {
    *p++ = 0;
  }
}

/***************************************************************************//**
 * @brief
 *   Perform a memcmp() in 'constant time'.
 *
 * @param[in]  a
 *   Pointer to the first memory location.
 *
 * @param[in]  a
 *   Pointer to the second memory location.
 *
 * @param[in] n
 *   Number of bytes to compare between the two memory locations.
 *
 * @return
 *   Zero if the buffer contents are equal, non-zero otherwise.
 ******************************************************************************/
__STATIC_INLINE
uint8_t sli_psa_safer_memcmp(const uint8_t *a,
                             const uint8_t *b,
                             size_t n)
{
  uint8_t diff = 0u;

  for (size_t i = 0; i < n; i++) {
    diff |= a[i] ^ b[i];
  }

  return diff;
}

#if defined(SLI_PSA_SUPPORT_GCM_IV_CALCULATION)
// See function documentation in sli_psa_driver_ghash.c
void sli_psa_software_ghash_setup(const uint8_t Ek[16],
                                  uint64_t HL[16],
                                  uint64_t HH[16]);

void sli_psa_software_ghash_multiply(const uint64_t HL[16],
                                     const uint64_t HH[16],
                                     uint8_t output[16],
                                     const uint8_t input[16]);

#endif /* SLI_PSA_SUPPORT_GCM_IV_CALCULATION */

// Declare the TRNG function prototype if it's not already declared by PSA
#if defined(MBEDTLS_ENTROPY_HARDWARE_ALT) && !defined(MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG)
psa_status_t mbedtls_psa_external_get_random(
  void *context,
  uint8_t *output, size_t output_size, size_t *output_length);
#endif

#ifdef __cplusplus
}
#endif

/// @endcond

#endif // SLI_PSA_DRIVER_COMMON_H
