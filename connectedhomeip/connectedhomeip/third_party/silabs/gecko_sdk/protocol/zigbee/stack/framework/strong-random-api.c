/***************************************************************************//**
 * @file
 * @brief EmberZNet APIs for pseudo and true random number generation.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifdef PLATFORM_HEADER
  #include PLATFORM_HEADER
#endif

#include "include/ember.h"
#include "hal/hal.h"
#include "include/error.h"

#ifdef UC_BUILD
#include "sl_component_catalog.h"
#endif

#ifdef UC_BUILD
#if (defined SL_CATALOG_ZIGBEE_STRONG_RANDOM_API_RADIO_PRESENT)
#define USE_RADIO_API_FOR_TRNG
#elif (defined SL_CATALOG_ZIGBEE_STRONG_RANDOM_API_PSA_PRESENT)
#define USE_PSA_API_FOR_TRNG
#else // None of the components present, then error
#error Must select one of the strong random api with radio or PSA Crypto
#endif // SL_CATALOG_ZIGBEE_STRONG_RANDOM_API_
#endif // UC_BUILD

#if defined(MBEDTLS_CONFIG_FILE)
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(USE_PSA_API_FOR_TRNG)
#include "psa/crypto.h"
#else // defined(USE_PSA_API_FOR_TRNG)
extern bool emRadioGetRandomNumbers(uint16_t *randomNumber, uint8_t count);
#endif // defined(USE_PSA_API_FOR_TRNG)

EmberStatus emberGetStrongRandomNumberArray(uint16_t *randomNumber, uint8_t count)
{
#if defined(USE_PSA_API_FOR_TRNG)
  // Get random numbers, lenght is in unsigned char blocks, making it uint16_t
  psa_status_t status;
  status = psa_generate_random((uint8_t*)randomNumber, 2 * count);

  if (status != PSA_SUCCESS) {
#else // defined(USE_MBEDTLS_API_FOR_TRNG)
  // Get random numbers from the radio API
  if (!emRadioGetRandomNumbers(randomNumber, count)) {
#endif // defined(USE_MBEDTLS_API_FOR_TRNG)
    return EMBER_INSUFFICIENT_RANDOM_DATA;
  }

  return EMBER_SUCCESS;
}

EmberEntropySource emberGetStrongRandomEntropySource()
{
#if defined(USE_PSA_API_FOR_TRNG)
#if defined(MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG) || defined(MBEDTLS_ENTROPY_HARDWARE_ALT)
  return EMBER_ENTROPY_SOURCE_MBEDTLS_TRNG;
#else
  return EMBER_ENTROPY_SOURCE_MBEDTLS;
#endif
#else // defined(USE_PSA_API_FOR_TRNG)
  return EMBER_ENTROPY_SOURCE_RADIO;
#endif // defined(USE_PSA_API_FOR_TRNG)
}
