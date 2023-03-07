/***************************************************************************//**
 * @file
 * @brief Entropy driver for Silicon Labs devices.
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

#if !defined(MBEDTLS_CONFIG_FILE)
#include "config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_ENTROPY_HARDWARE_ALT)
#include "entropy_poll.h"
#include "psa/crypto.h"

#include "em_device.h"

#if defined(MBEDTLS_TRNG_PRESENT) \
  || defined(SEMAILBOX_PRESENT)   \
  || defined(CRYPTOACC_PRESENT)
#define SLI_ENTROPY_HAVE_TRNG
#if !defined(MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG)
/* If PSA is not configured with external RNG, do a forward declaration of the
 * external RNG function here to allow us to call it for entropy as well. */
psa_status_t mbedtls_psa_external_get_random(
  void *context,
  uint8_t *output, size_t output_size, size_t *output_length);
#endif
#endif

#if !defined(MBEDTLS_ERR_ENTROPY_SOURCE_FAILED)
/* Repeat declaration of MBEDTLS_ERR_ENTROPY_SOURCE_FAILED since the full entropy.h
 * header is not always a clean include. I.e. when mbedtls_hardware_poll is used
 * without having the full entropy module (with collector) present, the header will
 * potentially complain about missing a SHA256/SHA512 context structure definition. */
#define MBEDTLS_ERR_ENTROPY_SOURCE_FAILED -0x003C
#endif

/* For devices with an active TRNG errata, we need to rely on a different
 * source of entropy. */
#if defined(_SILICON_LABS_GECKO_INTERNAL_SDID_89) \
  || defined(_SILICON_LABS_GECKO_INTERNAL_SDID_95)
  #define SLI_ENTROPY_REQUIRE_FALLBACK
#endif

#if defined(_SILICON_LABS_GECKO_INTERNAL_SDID_89)
#include "em_system.h"    // SYSTEM_ChipRevisionGet()
#endif

// -------------------------------------
// Local function definitions

// Include radio entropy fallback if present & required
#if defined(MBEDTLS_ENTROPY_RAIL_PRESENT) \
  && (!defined(SLI_ENTROPY_HAVE_TRNG) || defined(SLI_ENTROPY_REQUIRE_FALLBACK))
#include "rail.h"
static int rail_get_random(unsigned char *output,
                           size_t len,
                           size_t *out_len)
{
  uint16_t rail_entropy_request_len;
  if (len > UINT16_MAX) {
    rail_entropy_request_len = UINT16_MAX;
  } else {
    rail_entropy_request_len = (uint16_t)len;
  }

  *out_len = (size_t)RAIL_GetRadioEntropy(RAIL_EFR32_HANDLE,
                                          (uint8_t *)output,
                                          rail_entropy_request_len);
  return 0;
}
#endif // radio fallback

#if defined(MBEDTLS_ENTROPY_ADC_C) \
  && (!defined(SLI_ENTROPY_HAVE_TRNG) || defined(SLI_ENTROPY_REQUIRE_FALLBACK))
#if !defined(MBEDTLS_ENTROPY_ADC_INSTANCE)
#define MBEDTLS_ENTROPY_ADC_INSTANCE 0
#endif

#include "sl_entropy_adc.h"
static int adc_get_random(unsigned char *output,
                          size_t len,
                          size_t *out_len)
{
  mbedtls_entropy_adc_context adc_ctx;
  int ret = -1;

  mbedtls_entropy_adc_init(&adc_ctx);
  ret = mbedtls_entropy_adc_set_instance(&adc_ctx, MBEDTLS_ENTROPY_ADC_INSTANCE);
  if (ret < 0) {
    goto exit;
  }

  ret = mbedtls_entropy_adc_poll(&adc_ctx, output, len, out_len);

  exit:
  mbedtls_entropy_adc_free(&adc_ctx);
  return ret;
}
#endif // ADC fallback

#if (defined(MBEDTLS_ENTROPY_RAIL_PRESENT) || defined(MBEDTLS_ENTROPY_ADC_C)) \
  && (!defined(SLI_ENTROPY_HAVE_TRNG) || defined(SLI_ENTROPY_REQUIRE_FALLBACK))
static int rail_adc_entropy(unsigned char *output,
                            size_t len,
                            size_t *olen)
{
  (void) output;
  (void) len;
  (void) olen;

  *olen = 0;
  int ret = MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;
  #if defined(MBEDTLS_ENTROPY_RAIL_PRESENT)
  ret = rail_get_random(output, len, olen);
  if (*olen > 0 && ret == 0) {
    // Return if we actually gathered something
    // Otherwise, fallback to the ADC source if it is available.
    return ret;
  }
  #endif // MBEDTLS_ENTROPY_RAIL_PRESENT
  #if defined(MBEDTLS_ENTROPY_ADC_C)
  ret = adc_get_random(output, len, olen);
  #endif // MBEDTLS_ENTROPY_ADC_C
  return ret;
}
#endif // RAIL and ADC entropy

// -------------------------------------
// Global function definitions

int mbedtls_hardware_poll(void *data,
                          unsigned char *output,
                          size_t len,
                          size_t *olen)
{
  (void)data;

#if defined(_SILICON_LABS_GECKO_INTERNAL_SDID_89)
  // TRNG entropy on EFR32xG13 is under errata on revisions < A3
  SYSTEM_ChipRevision_TypeDef rev;
  SYSTEM_ChipRevisionGet(&rev);

  if ((rev.major == 1) && (rev.minor < 3)) {
    // On affected revisions, fall back to radio (prefered) or ADC entropy
    return rail_adc_entropy(output, len, olen);
  }
#elif defined(SLI_ENTROPY_REQUIRE_FALLBACK)
  // Other devices for which this symbol is defined have TRNG erratas requiring
  // fallback to other sources for all revisions.
  return rail_adc_entropy(output, len, olen);
#endif

#if !defined(SLI_ENTROPY_REQUIRE_FALLBACK) \
  || defined(_SILICON_LABS_GECKO_INTERNAL_SDID_89)
  // Devices not requiring fallback (or fell through here because the active
  // errata does not apply to the ICs revision) use a TRNG when available, but
  // can also use the radio or ADC when no TRNG is present.
  #if defined(SLI_ENTROPY_HAVE_TRNG)
  psa_status_t status = mbedtls_psa_external_get_random(data, output, len, olen);
  if (status == PSA_SUCCESS) {
    return 0;
  } else {
    return MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;
  }
  #else // SLI_ENTROPY_HAVE_TRNG
  return rail_adc_entropy(output, len, olen);
  #endif // SLI_ENTROPY_HAVE_TRNG
#endif
}

#endif // MBEDTLS_ENTROPY_HARDWARE_ALT
