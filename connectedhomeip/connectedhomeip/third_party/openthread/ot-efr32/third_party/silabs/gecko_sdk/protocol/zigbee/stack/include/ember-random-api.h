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

#ifndef EMBER_RANDOM_API_H
#define EMBER_RANDOM_API_H

#if defined(MBEDTLS_CONFIG_FILE) && !defined(ZIGBEE_STACK_ON_HOST)
#include MBEDTLS_CONFIG_FILE
#endif

/**
 * @addtogroup security
 *
 * This file describes routines for getting pseudo- and strong-random numbers.
 *
 * @{
 */

/** @brief Wrapper for the HAL API to generate pseudo-random numbers.
 *
 * @return uint16_t pseudo random number
 */
#define emberGetPseudoRandomNumber() halCommonGetRandom()

/** @brief Used to get ::count number of true random numbers. The method of obtaining
 *        and quality of the random varies by chip family and the configuration used.
 *        For high security applications, please verify that that the generator's
 *        properties match your needs.
 *
 * @return An ::EmberStatus value.
 * - ::EMBER_SUCCESS If success.
 * - ::EMBER_INSUFFICIENT_RANDOM_DATA In case of any error.
 */
EmberStatus emberGetStrongRandomNumberArray(uint16_t *randomNumber, uint8_t count);

#define emberGetTrueRandomNumberArray(randomNumber, count) \
  (emberGetStrongRandomNumberArray((randomNumber), (count)))

/** @brief Used to get one true random number.
 *         For more info check ::emberGetStrongRandomNumberArray.
 */
#define emberGetStrongRandomNumber(randomNumber) \
  (emberGetStrongRandomNumberArray((randomNumber), 1))

#define emberGetTrueRandomNumber(randomNumber) \
  (emberGetStrongRandomNumber((randomNumber)))

/** @brief Returns the entropy source used for true random number generation.
 *
 * @return An ::EmberEntropySource value.
 * - ::EMBER_ENTROPY_SOURCE_ERROR If there is any error identifying the entroypy source.
 * - ::EMBER_ENTROPY_SOURCE_RADIO Entropy source is the chips that support this. The method of obtaining
 *                                and quality of the random varies by chip family. For high security
 *                                applications, please verify that that the generator's properties match
 *                                the requirements.
 * - ::EMBER_ENTROPY_SOURCE_MBEDTLS_TRNG Entropy source is the TRNG hardware module (if applicable)
 *                                       and the interface driver is powered by mbed TLS.
 * - ::EMBER_ENTROPY_SOURCE_MBEDTLS Entropy source is not the TRNG hardware module
 *                                  but the interface driver is powered by mbed TLS.
 */
EmberEntropySource emberGetStrongRandomEntropySource(void);

// @} END addtogroup

#ifndef EZSP_HOST
#define emberGetTrueRandomEntropySource() \
  (emberGetStrongRandomEntropySource())
#endif

#endif // EMBER_RANDOM_API_H
