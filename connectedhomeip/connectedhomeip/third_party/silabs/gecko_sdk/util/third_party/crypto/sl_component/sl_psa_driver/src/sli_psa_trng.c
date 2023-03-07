/***************************************************************************//**
 * @file
 * @brief Default PSA TRNG hook for Silicon Labs devices.
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

// -------------------------------------
// Includes

#include "psa/crypto_platform.h"

#if defined(MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG) || defined(MBEDTLS_ENTROPY_HARDWARE_ALT)
#include "em_device.h"
#include "psa/crypto.h"
#include "psa/crypto_extra.h"

#if !defined(MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG)
typedef void mbedtls_psa_external_random_context_t;
#endif

#if defined(SEMAILBOX_PRESENT)
  #include "sl_se_manager.h"
  #include "sl_se_manager_entropy.h"
#endif

#if defined(CRYPTOACC_PRESENT)
  #include "sx_trng.h"
  #include "cryptolib_types.h"
  #include "cryptoacc_management.h"
#endif

#if defined(TRNG_PRESENT) \
  && !defined(_SILICON_LABS_GECKO_INTERNAL_SDID_95)
  #define SLI_RNG_TRNG_ENABLED
  #include "sli_crypto_trng_driver.h"
#endif

#if defined(SEMAILBOX_PRESENT)
static psa_status_t se_get_random(unsigned char *output,
                                  size_t len,
                                  size_t *out_len)
{
  sl_status_t ret;
  sl_se_command_context_t cmd_ctx;

  // Initialize the SE manager.
  ret = sl_se_init();
  if (ret != SL_STATUS_OK) {
    return PSA_ERROR_HARDWARE_FAILURE;
  }

  // Initialize command context
  ret = sl_se_init_command_context(&cmd_ctx);
  if (ret != SL_STATUS_OK) {
    return PSA_ERROR_HARDWARE_FAILURE;
  }

  // Get entropy
  ret = sl_se_get_random(&cmd_ctx, output, len);

  if (ret == SL_STATUS_OK) {
    *out_len = len;
    return PSA_SUCCESS;
  }

  *out_len = 0;
  return PSA_ERROR_HARDWARE_FAILURE;
}
#endif // SEMAILBOX_PRESENT

#if defined(CRYPTOACC_PRESENT)
static psa_status_t cryptoacc_get_random(unsigned char *output,
                                         size_t len,
                                         size_t *out_len)
{
  psa_status_t trng_status;
  size_t requested_entropy_len = len;
  block_t data_out;
  size_t surplus_bytes = 0;
  uint32_t surplus_word = 0;

  trng_status = cryptoacc_trng_initialize();
  if (trng_status != PSA_SUCCESS) {
    return trng_status;
  }

  // The sx_rng_get_rand_blk function returns full 32-bit words
  // of random data. If the requested number of bytes (and output buffer)
  // is not a multiple of 4 bytes we need to get the remaining/surplus
  // bytes in an additional call.
  surplus_bytes = requested_entropy_len & 0x3;
  requested_entropy_len &= ~0x3;

  if (requested_entropy_len > 0U) {
    data_out = block_t_convert(output, requested_entropy_len);
    trng_status = cryptoacc_management_acquire();
    if (trng_status != PSA_SUCCESS) {
      return trng_status;
    }
    sx_trng_get_rand_blk(data_out);
    trng_status = cryptoacc_management_release();
  }

  if (surplus_bytes != 0U) {
    data_out = block_t_convert(&surplus_word, 4U);
    trng_status = cryptoacc_management_acquire();
    if (trng_status != PSA_SUCCESS) {
      return trng_status;
    }
    sx_trng_get_rand_blk(data_out);
    trng_status = cryptoacc_management_release();
    for (size_t i = 0; i < surplus_bytes; i++) {
      output[requested_entropy_len + i] = (surplus_word >> (i * 8)) & 0xFF;
    }
  }

  if (trng_status == PSA_SUCCESS) {
    *out_len = len;
  } else {
    *out_len = 0;
  }

  return trng_status;
}
#endif // CRYPTOACC_PRESENT

psa_status_t mbedtls_psa_external_get_random(
  mbedtls_psa_external_random_context_t *context,
  uint8_t *output, size_t output_size, size_t *output_length)
{
  (void)context;

  // Implement chunking support here, as the PSA core doesn't implement it (yet)
#if defined(SEMAILBOX_PRESENT) || defined(CRYPTOACC_PRESENT) || defined(SLI_RNG_TRNG_ENABLED)
  psa_status_t entropy_status = PSA_ERROR_CORRUPTION_DETECTED;
  size_t entropy_max_retries = 5;
  *output_length = 0;

  while (entropy_max_retries > 0 && entropy_status != PSA_SUCCESS) {
    size_t offset = *output_length;
    #if defined(SEMAILBOX_PRESENT)
    entropy_status = se_get_random(&output[offset], output_size - offset, output_length);

    #elif defined(CRYPTOACC_PRESENT)
    entropy_status = cryptoacc_get_random(&output[offset], output_size - offset, output_length);

    #elif defined(SLI_RNG_TRNG_ENABLED)
    entropy_status = sli_crypto_trng_get_random(&output[offset], output_size - offset, output_length);

    #else
      #error "No known entropy source for external random function"
    #endif

    *output_length += offset;

    if (*output_length >= output_size) {
      entropy_status = PSA_SUCCESS;
    }

    // Consume a retry before going through another loop
    entropy_max_retries--;
  }

  return entropy_status;

#else // SE/CRYPTOACC/TRNG_ENABLED
  (void) output;
  (void) output_size;
  (void) output_length;
  return PSA_ERROR_HARDWARE_FAILURE;
#endif
}

#endif // MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG || defined(MBEDTLS_ENTROPY_HARDWARE_ALT)
