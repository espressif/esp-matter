/***************************************************************************//**
 * @file
 * @brief mbed TLS configuration file for application enabled device acceleration.
 *
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

#ifndef MBEDTLS_CONFIG_DEVICE_ACCELERATION_APP_H
#define MBEDTLS_CONFIG_DEVICE_ACCELERATION_APP_H

#if defined(MBEDTLS_ENTROPY_C)

#include "em_device.h"

// Include if ADC entropy should be available for application to use
#if defined(MBEDTLS_ENTROPY_ADC_PRESENT) && defined(MBEDTLS_ENTROPY_ADC_APP)
#define MBEDTLS_ENTROPY_ADC_C
#define MBEDTLS_ENTROPY_ADC_INSTANCE (0)
#endif

/**
 * \def MBEDTLS_ENTROPY_HARDWARE_ALT
 *
 * Integrate the provided default entropy source into the mbed
 * TLS entropy infrastructure.
 *
 * Requires MBEDTLS_TRNG_PRESENT || MBEDTLS_ENTROPY_RAIL_PRESENT || SEMAILBOX_PRESENT || CRYPTOACC_PRESENT
 */
#if defined(MBEDTLS_TRNG_PRESENT)          \
  || defined(MBEDTLS_ENTROPY_RAIL_PRESENT) \
  || defined(SEMAILBOX_PRESENT)            \
  || defined(CRYPTOACC_PRESENT)
#define MBEDTLS_ENTROPY_HARDWARE_ALT
#endif

#endif // defined(MBEDTLS_ENTROPY_C)

#endif /* MBEDTLS_CONFIG_DEVICE_ACCELERATION_APP_H */
