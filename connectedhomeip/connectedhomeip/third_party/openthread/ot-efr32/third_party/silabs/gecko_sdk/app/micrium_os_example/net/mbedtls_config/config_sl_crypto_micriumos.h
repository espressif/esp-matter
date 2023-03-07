/***************************************************************************//**
 * @file
 * @brief mbed TLS configuration for Silicon Labs CRYPTO hardware acceleration
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: APACHE-2.0
 *
 * This software is subject to an open source license and is distributed by
 * Silicon Laboratories Inc. pursuant to the terms of the Apache License,
 * Version 2.0 available at https://www.apache.org/licenses/LICENSE-2.0.
 * Such terms and conditions may be further supplemented by the Silicon Labs
 * Master Software License Agreement (MSLA) available at www.silabs.com and its
 * sections applicable to open source software.
 *
 ******************************************************************************/

/**
 * @defgroup sl_crypto_config Silicon Labs CRYPTO Hardware Acceleration Configuration
 * @addtogroup sl_crypto_config
 *
 * @brief
 *  mbed TLS configuration for Silicon Labs CRYPTO hardware acceleration
 *
 * @details
 *  mbed TLS configuration is composed of settings in this Silicon Labs specific CRYPTO hardware acceleration file located in mbedtls/configs and the mbed TLS configuration file in mbedtls/include/mbedtls/config.h.
 *  This configuration can be used as a starting point to evaluate hardware acceleration available on Silicon Labs devices.
 *
 * @{
 */

#ifndef MBEDTLS_CONFIG_SL_CRYPTO_ALL_ACCELERATION_H
#define MBEDTLS_CONFIG_SL_CRYPTO_ALL_ACCELERATION_H

#include "em_device.h"

#if !defined(NO_CRYPTO_ACCELERATION)
/**
 * @name SECTION: Silicon Labs Acceleration settings
 *
 * This section sets Silicon Labs Acceleration settings.
 * @{

 */

/**
 * \def MBEDTLS_AES_ALT
 *
 * Enable hardware acceleration for the AES block cipher
 *
 * Module:  sl_crypto/src/crypto_aes.c for devices with CRYPTO
 *          sl_crypto/src/aes_aes.c for devices with AES
 *
 * See MBEDTLS_AES_C for more information.
 */
#define MBEDTLS_AES_ALT

/**
 * \def MBEDTLS_ECP_INTERNAL_ALT
 * \def ECP_SHORTWEIERSTRASS
 * \def MBEDTLS_ECP_ADD_MIXED_ALT
 * \def MBEDTLS_ECP_DOUBLE_JAC_ALT
 * \def MBEDTLS_ECP_NORMALIZE_JAC_MANY_ALT
 * \def MBEDTLS_ECP_NORMALIZE_JAC_ALT
 *
 * Enable hardware acceleration for the elliptic curve over GF(p) library.
 *
 * Module:  sl_crypto/src/crypto_ecp.c
 * Caller:  library/ecp.c
 *
 * Requires: MBEDTLS_BIGNUM_C, MBEDTLS_ECP_C and at least one
 * MBEDTLS_ECP_DP_XXX_ENABLED and (CRYPTO_COUNT > 0)
 */
#if defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0)
#define MBEDTLS_ECP_INTERNAL_ALT
#define ECP_SHORTWEIERSTRASS
#define MBEDTLS_ECP_ADD_MIXED_ALT
#define MBEDTLS_ECP_DOUBLE_JAC_ALT
#define MBEDTLS_ECP_NORMALIZE_JAC_MANY_ALT
#define MBEDTLS_ECP_NORMALIZE_JAC_ALT
#define MBEDTLS_ECP_RANDOMIZE_JAC_ALT
#endif

/**
 * \def MBEDTLS_SHA1_ALT
 *
 * Enable hardware acceleration for the SHA1 cryptographic hash algorithm.
 *
 * Module:  sl_crypto/src/crypto_sha.c
 * Caller:  library/mbedtls_md.c
 *          library/ssl_cli.c
 *          library/ssl_srv.c
 *          library/ssl_tls.c
 *          library/x509write_crt.c
 *
 * Requires: MBEDTLS_SHA1_C and (CRYPTO_COUNT > 0)
 * See MBEDTLS_SHA1_C for more information.
 */
#if defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0)
#define MBEDTLS_SHA1_ALT
#endif

/**
 * \def MBEDTLS_SHA256_ALT
 *
 * Enable hardware acceleration for the SHA-224 and SHA-256 cryptographic
 * hash algorithms.
 *
 * Module:  sl_crypto/src/crypto_sha.c
 * Caller:  library/entropy.c
 *          library/mbedtls_md.c
 *          library/ssl_cli.c
 *          library/ssl_srv.c
 *          library/ssl_tls.c
 *
 * Requires: MBEDTLS_SHA256_C and (CRYPTO_COUNT > 0)
 * See MBEDTLS_SHA256_C for more information.
 */
#if defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0)
#define MBEDTLS_SHA256_ALT
#endif

#if defined(SEMAILBOX_PRESENT)
#define MBEDTLS_SHA1_ALT
#define MBEDTLS_SHA1_PROCESS_ALT
#define MBEDTLS_SHA256_ALT
#define MBEDTLS_SHA256_PROCESS_ALT
#define MBEDTLS_SHA512_ALT
#define MBEDTLS_SHA512_PROCESS_ALT

#define MBEDTLS_ECDH_GEN_PUBLIC_ALT
#define MBEDTLS_ECDH_COMPUTE_SHARED_ALT
#define MBEDTLS_ECDSA_GENKEY_ALT
#define MBEDTLS_ECDSA_SIGN_ALT
#define MBEDTLS_ECDSA_VERIFY_ALT

#define MBEDTLS_CCM_ALT
#define MBEDTLS_CMAC_ALT
#endif

#endif /* #if !defined(NO_CRYPTO_ACCELERATION) */

/**
 * \def MBEDTLS_TRNG_C
 *
 * Enable software support for the True Random Number Generator (TRNG)
 * incorporated from Series 1 Configuration 2 devices (EFR32MG12, etc.)
 * from Silicon Labs.
 *
 * TRNG is not supported by software for EFR32XG13 (SDID_89) and
 * EFR32XG14 (SDID_95).
 *
 * Requires TRNG_PRESENT &&
 *  !(_SILICON_LABS_GECKO_INTERNAL_SDID_89 ||
 *   _SILICON_LABS_GECKO_INTERNAL_SDID_95)
 */
#if defined(TRNG_PRESENT)                            \
  && !(defined(_SILICON_LABS_GECKO_INTERNAL_SDID_89) \
  || defined(_SILICON_LABS_GECKO_INTERNAL_SDID_95))
#define MBEDTLS_TRNG_C
#endif

/**
 * \def MBEDTLS_ENTROPY_ADC_C
 *
 * Enable software support for the retrieving entropy data from the ADC
 * incorporated on devices from Silicon Labs.
 *
 * Requires ADC_PRESENT && _ADC_SINGLECTRLX_VREFSEL_VENTROPY
 */
#if defined(ADC_PRESENT) && defined(_ADC_SINGLECTRLX_VREFSEL_VENTROPY)
#define MBEDTLS_ENTROPY_ADC_C
#endif

/**
 * \def MBEDTLS_ENTROPY_ADC_INSTANCE
 *
 * Specify which ADC instance shall be used as entropy source.
 *
 * Requires MBEDTLS_ENTROPY_ADC_C
 */
#if defined(MBEDTLS_ENTROPY_ADC_C)
#define MBEDTLS_ENTROPY_ADC_INSTANCE  (0)
#endif

/**
 * \def MBEDTLS_ENTROPY_RAIL_C
 *
 * Enable software support for the retrieving entropy data from the RAIL
 * incorporated on devices from Silicon Labs.
 *
 * Requires _EFR_DEVICE
 */
#if defined(_EFR_DEVICE)
#define MBEDTLS_ENTROPY_RAIL_C
#endif

/**
 * \def MBEDTLS_ENTROPY_HARDWARE_ALT_RAIL
 *
 * Use the radio (RAIL) as default hardware entropy source.
 *
 * Requires MBEDTLS_ENTROPY_RAIL_C && _SILICON_LABS_32B_SERIES_1 &&
 *         !MBEDTLS_TRNG_C
 */
#if defined(MBEDTLS_ENTROPY_RAIL_C) \
  && defined(_SILICON_LABS_32B_SERIES_1) && !defined(MBEDTLS_TRNG_C)
#define MBEDTLS_ENTROPY_HARDWARE_ALT_RAIL
#endif

/**
 * \def MBEDTLS_ENTROPY_HARDWARE_ALT
 *
 * Integrate the provided default entropy source into the mbed
 * TLS entropy infrastructure.
 *
 * Requires MBEDTLS_TRNG_C || MBEDTLS_ENTROPY_HARDWARE_ALT_RAIL || SEMAILBOX
 */
#if defined(MBEDTLS_TRNG_C) || defined(MBEDTLS_ENTROPY_HARDWARE_ALT_RAIL) || defined(SEMAILBOX_PRESENT)
#define MBEDTLS_ENTROPY_HARDWARE_ALT
#endif

/* Default ECC configuration for Silicon Labs devices: */

/* ECC curves supported by CRYPTO hardware module: */
#define MBEDTLS_ECP_DP_SECP192R1_ENABLED
#define MBEDTLS_ECP_DP_SECP224R1_ENABLED
#define MBEDTLS_ECP_DP_SECP256R1_ENABLED

/* Save RAM by adjusting to our exact needs */
#define MBEDTLS_ECP_MAX_BITS   512
#ifndef MBEDTLS_MPI_MAX_SIZE
#define MBEDTLS_MPI_MAX_SIZE   1024 // 384 bits is 48 bytes
#endif

/*
   Set MBEDTLS_ECP_WINDOW_SIZE to configure
   ECC point multiplication window size, see ecp.h:
   2 = Save RAM at the expense of speed
   3 = Improve speed at the expense of RAM
   4 = Optimize speed at the expense of RAM
 */
#define MBEDTLS_ECP_WINDOW_SIZE        6
#define MBEDTLS_ECP_FIXED_POINT_OPTIM  1

/* Significant speed benefit at the expense of some ROM */
#define MBEDTLS_ECP_NIST_OPTIM

/* Include the default mbed TLS config file */
#include "mbedtls/config.h"

#undef MBEDTLS_TIMING_C
#undef MBEDTLS_FS_IO
#define MBEDTLS_NO_PLATFORM_ENTROPY
#undef MBEDTLS_HAVE_TIME_DATE

/* Hardware entropy source is not yet supported. Uncomment this macro to
   provide your own implementation of an entropy collector. */
//#define MBEDTLS_ENTROPY_HARDWARE_ALT

/* Exclude and/or change default config here. E.g.: */
//#undef MBEDTLS_ECP_DP_SECP384R1_ENABLED
//#undef MBEDTLS_ECP_DP_SECP521R1_ENABLED
//#undef MBEDTLS_ECP_DP_BP384R1_ENABLED
//#undef MBEDTLS_ECP_DP_BP512R1_ENABLED
//#undef MBEDTLS_SHA512_C

#include "mbedtls/check_config.h"

/** @} (end section sl_crypto_config) */
/** @} (end addtogroup sl_crypto_config) */

#endif /* MBEDTLS_CONFIG_SL_CRYPTO_ALL_ACCELERATION_H */
