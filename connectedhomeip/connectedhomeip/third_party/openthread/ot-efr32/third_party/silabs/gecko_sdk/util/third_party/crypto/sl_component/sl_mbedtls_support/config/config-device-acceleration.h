/***************************************************************************//**
 * @brief Configuration for enabling hardware acceleration for mbedtls features.
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

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN

/**
 * @defgroup sl_config_device_acceleration Silicon Labs Hardware Acceleration Configuration
 * @addtogroup sl_config_device_acceleration
 *
 * @brief
 *  mbed TLS configuration for Silicon Labs device specific hardware acceleration
 *
 * @details
 *  mbed TLS configuration is composed of settings in this Silicon Labs device specific hardware acceleration
 *  file located in mbedtls/configs that will enable hardware acceleration of all features where this is
 *  supported. This file should be included from an application specific configuration file that
 *  configures what mbedtls features should be included.
 *
 * @{
 */

#ifndef MBEDTLS_CONFIG_DEVICE_ACCELERATION_H
#define MBEDTLS_CONFIG_DEVICE_ACCELERATION_H

#if !defined(NO_CRYPTO_ACCELERATION)
#include "em_device.h"
#if defined(SEMAILBOX_PRESENT) || defined(DOXY_DOC_ONLY)
#include "em_se.h"
#endif

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif

/**
 * @name SECTION: Silicon Labs Acceleration settings
 *
 * This section sets Silicon Labs Acceleration settings.
 * @{

 */

/** \def MBEDTLS_PSA_CRYPTO_DRIVERS
 *
 * Enable support for the experimental PSA crypto driver interface.
 *
 * Requires: \ref MBEDTLS_PSA_CRYPTO_C.
 *
 * \warning This interface is experimental and may change or be removed
 * without notice.
 */
#if defined(MBEDTLS_PSA_CRYPTO_C)
#define MBEDTLS_PSA_CRYPTO_DRIVERS
#endif

/**
 * \def MBEDTLS_AES_ALT
 *
 * Enable hardware acceleration for the AES block cipher modes through
 * the mbed TLS APIs.
 *
 * Module:  sl_mbedtls_support/src/crypto_aes.c for devices with CRYPTO,
 *          sl_mbedtls_support/src/se_aes.c for devices with HSE,
 *          sl_mbedtls_support/src/cryptoacc_aes.c for devices with CRYPTOACC,
 *          sl_mbedtls_support/src/aes_aes.c for devices with AES
 *
 * See \ref MBEDTLS_AES_C for more information.
 */
#if defined(_SILICON_LABS_32B_SERIES)
#define MBEDTLS_AES_ALT
#endif
#if defined(CRYPTOACC_PRESENT) || defined(SEMAILBOX_PRESENT) || defined(DOXY_DOC_ONLY)
#define AES_192_SUPPORTED
#endif

/**
 * \def MBEDTLS_CCM_ALT
 *
 * Enable hardware acceleration of CCM through mbed TLS APIs.
 * Not enabled when PSA Crypto is present in the build together with the PSA driver for CCM,
 * as that would preclude software fallback for cases where the hardware capabilites do not
 * cover the full potential usage of the PSA Driver API
 *
 * Module:  sl_mbedtls_support/src/mbedtls_ccm.c for all devices, plus:
 *          - sl_psa_driver/src/sli_se_transparent_driver_aead.c and sl_psa_driver/src/sli_se_driver_aead.c for devices with HSE,
 *          - sl_psa_driver/src/sli_cryptoacc_transparent_driver_aead.c for devices with CRYPTOACC
 *          - sl_psa_driver/src/sli_crypto_transparent_driver_aead.c for devices with CRYPTO
 *
 * Requires: \ref MBEDTLS_AES_C and \ref MBEDTLS_CCM_C (CRYPTOACC_PRESENT or SEMAILBOX_PRESENT)
 *
 * See MBEDTLS_CCM_C for more information.
 */
#if defined(CRYPTO_PRESENT) || defined(CRYPTOACC_PRESENT) || defined(DOXY_DOC_ONLY) \
  || (defined(SEMAILBOX_PRESENT) && defined(SE_COMMAND_AES_CCM_ENCRYPT) && defined(SE_COMMAND_AES_CCM_DECRYPT) )
// Remove this when full multipart support is present in the CCM ALT driver
// Todo: remove guard when [PSEC-1954][PSEC-2109][PSEC-3133] are done
#if !(defined(MBEDTLS_PSA_CRYPTO_DRIVERS))
#define MBEDTLS_CCM_ALT
#endif

#endif

/**
 * \def MBEDTLS_CMAC_ALT
 *
 * Enable hardware acceleration CMAC through mbed TLS APIs.
 *
 * Module:  sl_mbedtls_support/src/mbedtls_cmac.c for all devices, plus:
 *          - sl_psa_driver/src/sli_se_transparent_driver_mac.c and sl_psa_driver/src/sli_se_driver_mac.c for devices with HSE,
 *          - sl_psa_driver/src/sli_cryptoacc_transparent_driver_mac.c for devices with CRYPTOACC
 *
 * Requires: \ref MBEDTLS_AES_C and \ref MBEDTLS_CMAC_C (CRYPTOACC_PRESENT or SEMAILBOX_PRESENT)
 *
 * See MBEDTLS_CMAC_C for more information.
 */
#if defined(CRYPTO_PRESENT) || defined(CRYPTOACC_PRESENT) || defined(DOXY_DOC_ONLY) \
  || (defined(SEMAILBOX_PRESENT) && defined(SE_COMMAND_AES_CMAC))
#define MBEDTLS_CMAC_ALT
#endif

/**
 * \def MBEDTLS_GCM_ALT
 *
 * Enable hardware acceleration GCM.
 *
 * Module:  sl_mbedtls_support/src/se_gcm.c for devices with HSE,
 *          sl_mbedtls_support/src/cryptoacc_gcm.c for devices with CRYPTOACC
 *
 * Requires: \ref MBEDTLS_GCM_C (CRYPTOACC_PRESENT or SEMAILBOX_PRESENT)
 *
 * See MBEDTLS_GCM_C for more information.
 */
#if defined(CRYPTO_PRESENT) || defined(CRYPTOACC_PRESENT) || defined(DOXY_DOC_ONLY) \
  || (defined(SEMAILBOX_PRESENT) && defined(SE_COMMAND_AES_GCM_ENCRYPT) && defined(SE_COMMAND_AES_GCM_ENCRYPT) )
#define MBEDTLS_GCM_ALT
#endif

/**
 * \def MBEDTLS_SHA1_ALT
 *
 * Enable hardware acceleration for the SHA1 cryptographic hash algorithm
 * through the mbed TLS APIs.
 *
 * Module:  sl_mbedtls_support/src/mbedtls_sha.c for all devices, plus:
 *          - sl_psa_driver/src/sli_crypto_transparent_driver_hash.c for devices with CRYPTO,
 *          - sl_psa_driver/src/sli_se_transparent_driver_hash.c for devices with HSE,
 *          - sl_psa_driver/src/sli_cryptoacc_transparent_driver_hash.c for devices with CRYPTOACC
 *
 * Caller:  library/mbedtls_md.c
 *          library/ssl_cli.c
 *          library/ssl_srv.c
 *          library/ssl_tls.c
 *          library/x509write_crt.c
 *
 * Requires: \ref MBEDTLS_SHA1_C and (CRYPTO_PRESENT or CRYPTOACC_PRESENT or SEMAILBOX_PRESENT)
 *
 * See MBEDTLS_SHA1_C for more information.
 */
#if defined(CRYPTO_PRESENT) || defined(CRYPTOACC_PRESENT) || defined(DOXY_DOC_ONLY) \
  || (defined(SEMAILBOX_PRESENT) && defined(SE_COMMAND_OPTION_HASH_SHA1))
#define MBEDTLS_SHA1_ALT
#endif

/**
 * \def MBEDTLS_SHA256_ALT
 *
 * Enable hardware acceleration for the SHA-224 and SHA-256 cryptographic
 * hash algorithms through the mbed TLS APIs.
 *
 * Module:  sl_mbedtls_support/src/mbedtls_sha.c for all devices, plus:
 *          - sl_psa_driver/src/sli_crypto_transparent_driver_hash.c for devices with CRYPTO,
 *          - sl_psa_driver/src/sli_se_transparent_driver_hash.c for devices with HSE,
 *          - sl_psa_driver/src/sli_cryptoacc_transparent_driver_hash.c for devices with CRYPTOACC
 *
 * Caller:  library/entropy.c
 *          library/mbedtls_md.c
 *          library/ssl_cli.c
 *          library/ssl_srv.c
 *          library/ssl_tls.c
 *
 * Requires: \ref MBEDTLS_SHA256_C and (CRYPTO_PRESENT or CRYPTOACC_PRESENT or SEMAILBOX_PRESENT)
 *
 * See MBEDTLS_SHA256_C for more information.
 */
#if defined(CRYPTO_PRESENT) || defined(CRYPTOACC_PRESENT) || defined(DOXY_DOC_ONLY) \
  || (defined(SEMAILBOX_PRESENT)                                                    \
  && (defined(SE_COMMAND_OPTION_HASH_SHA256) || defined(SE_COMMAND_OPTION_HASH_SHA224) ) )
#define MBEDTLS_SHA256_ALT
#endif

/**
 * \def MBEDTLS_SHA512_ALT
 *
 * Enable hardware acceleration for the SHA-384 and SHA-512 cryptographic
 * hash algorithms through the mbed TLS APIs.
 *
 * Module:  sl_mbedtls_support/src/mbedtls_sha.c
 *          sl_psa_driver/src/sli_se_transparent_driver_hash.c
 *
 * Requires: \ref MBEDTLS_SHA512_C
 *
 * See MBEDTLS_SHA512_C for more information.
 */
#if defined(SEMAILBOX_PRESENT) \
  && (defined(SE_COMMAND_OPTION_HASH_SHA512) || defined(SE_COMMAND_OPTION_HASH_SHA384) )
#define MBEDTLS_SHA512_ALT
#endif

/**
 * \def MBEDTLS_ECP_INTERNAL_ALT
 * \def ECP_SHORTWEIERSTRASS
 * \def MBEDTLS_ECP_ADD_MIXED_ALT
 * \def MBEDTLS_ECP_DOUBLE_JAC_ALT
 * \def MBEDTLS_ECP_NORMALIZE_JAC_MANY_ALT
 * \def MBEDTLS_ECP_NORMALIZE_JAC_ALT
 *
 * Enable hardware acceleration for the elliptic curve over GF(p) library
 * in mbed TLS. This accelerates the raw arithmetic operations.
 *
 * Module:  sl_mbedtls_support/src/crypto_ecp.c
 *
 * Caller:  library/ecp.c
 *
 * Requires: \ref MBEDTLS_BIGNUM_C, \ref MBEDTLS_ECP_C and at least one
 * MBEDTLS_ECP_DP_XXX_ENABLED and CRYPTO_PRESENT
 */
#if defined(CRYPTO_PRESENT)                     \
  && (defined(MBEDTLS_ECP_DP_SECP192R1_ENABLED) \
  || defined(MBEDTLS_ECP_DP_SECP224R1_ENABLED)  \
  || defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED))
#define MBEDTLS_ECP_INTERNAL_ALT
#define ECP_SHORTWEIERSTRASS
#define MBEDTLS_ECP_ADD_MIXED_ALT
#define MBEDTLS_ECP_DOUBLE_JAC_ALT
#define MBEDTLS_ECP_NORMALIZE_JAC_MANY_ALT
#define MBEDTLS_ECP_NORMALIZE_JAC_ALT
#define MBEDTLS_ECP_RANDOMIZE_JAC_ALT
#endif

/**
 * \def MBEDTLS_ECDH_COMPUTE_SHARED_ALT
 * \def MBEDTLS_ECDH_GEN_PUBLIC_ALT
 * \def MBEDTLS_ECDSA_GENKEY_ALT
 * \def MBEDTLS_ECDSA_SIGN_ALT
 * \def MBEDTLS_ECDSA_VERIFY_ALT
 *
 * Enable hardware acceleration for certain ECC operations.
 *
 * Module:  sl_mbedtls_support/src/mbedtls_ecdsa_ecdh.c for all devices, plus:
 *          - sl_psa_driver/src/sli_se_driver_signature.c and sl_psa_driver/src/sli_se_driver_key_management.c for devices with HSE,
 *          - sl_psa_driver/src/sli_cryptoacc_transparent_driver_signature.c and sl_psa_driver/src/sli_cryptoacc_transparent_driver_key_management.c for devices with CRYPTOACC
 *
 * Requires: \ref MBEDTLS_ECP_C (CRYPTOACC_PRESENT or SEMAILBOX_PRESENT)
 *
 * See \ref MBEDTLS_ECP_C for more information.
 */
#if defined(CRYPTOACC_PRESENT) || defined(DOXY_DOC_ONLY)
#if !(defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED) \
  || defined(MBEDTLS_ECP_DP_SECP521R1_ENABLED)  \
  || defined(MBEDTLS_ECP_DP_SECP192K1_ENABLED)  \
  || defined(MBEDTLS_ECP_DP_SECP224K1_ENABLED)  \
  || defined(MBEDTLS_ECP_DP_BP256R1_ENABLED)    \
  || defined(MBEDTLS_ECP_DP_BP384R1_ENABLED)    \
  || defined(MBEDTLS_ECP_DP_BP512R1_ENABLED)    \
  || defined(MBEDTLS_ECP_DP_CURVE25519_ENABLED) \
  || defined(MBEDTLS_ECP_DP_CURVE448_ENABLED) )
  #define MBEDTLS_ECDH_COMPUTE_SHARED_ALT
  #define MBEDTLS_ECDH_GEN_PUBLIC_ALT
#endif // #if !(   defined(MBEDTLS_ECP_DP_XXX_ENABLED) ...

#if !(defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED) \
  || defined(MBEDTLS_ECP_DP_SECP521R1_ENABLED)  \
  || defined(MBEDTLS_ECP_DP_SECP192K1_ENABLED)  \
  || defined(MBEDTLS_ECP_DP_SECP224K1_ENABLED)  \
  || defined(MBEDTLS_ECP_DP_BP256R1_ENABLED)    \
  || defined(MBEDTLS_ECP_DP_BP384R1_ENABLED)    \
  || defined(MBEDTLS_ECP_DP_BP512R1_ENABLED) )
  #define MBEDTLS_ECDSA_GENKEY_ALT
  #define MBEDTLS_ECDSA_VERIFY_ALT
  #if !defined(MBEDTLS_ECDSA_DETERMINISTIC)
    #define MBEDTLS_ECDSA_SIGN_ALT
  #endif
#endif // #if !(   defined(MBEDTLS_ECP_DP_XXX_ENABLED) ...

#endif /* CRYPTOACC */

#if defined(SEMAILBOX_PRESENT) || defined(DOXY_DOC_ONLY)

#if defined(DOXY_DOC_ONLY)
/**
 * \def SL_SE_SUPPORT_FW_PRIOR_TO_1_2_2
 *
 * Enable software fallback for ECDH and ECC public key validation on xG21 devices
 * running SE firmware versions lower than 1.2.2.
 *
 * Due to other stability concerns, it is strongly recommended to upgrade these devices to
 * the latest firmware revision instead of turning on software fallback support.
 *
 * Not having fallback support will make ECDH operations, as well as PSA Crypto public key
 * import, return an error code on affected devices.
 */
#define SL_SE_SUPPORT_FW_PRIOR_TO_1_2_2
/**
 * \def SL_SE_ASSUME_FW_AT_LEAST_1_2_2
 *
 * For enhanced performance: if it is guaranteed that all devices on which this library will
 * run are updated to at least SE FW 1.2.2, then turning on this option will remove certain
 * fallback checks, thereby reducing the amount of processing required for ECDH and public
 * key verification operations.
 */
#define SL_SE_ASSUME_FW_AT_LEAST_1_2_2
/**
 * \def SL_SE_ASSUME_FW_AT_LEAST_1_2_10
 *
 * For code size savings: if it is guaranteed that all devices on which this
 * library will run are updated to at least SE FW 1.2.10 (on the 1.x.x branch),
 * then enabling this option will disable the software fallback implementation
 * for key exchange using X25519 (and related key handling operations). This
 * option is targeted at xG21A devices, which did not support hardware
 * acceleration of Curve25519 operations before this SE version.
 */
#define SL_SE_ASSUME_FW_AT_LEAST_1_2_10
/**
 * \def SL_SE_ASSUME_FW_AT_LEAST_2_1_7
 *
 * For code size savings: if it is guaranteed that all devices on which this
 * library will run are updated to at least SE FW 2.1.7 (on the 2.x.x branch),
 * then enabling this option will disable the software fallback implementation
 * for key exchange using X25519 (and related key handling operations). This
 * option is targeted at xG23A devices, which did not support hardware
 * acceleration of Curve25519 operations before this SE version.
 */
#define SL_SE_ASSUME_FW_AT_LEAST_2_1_7
/**
 * \def SL_SE_ASSUME_FW_UNAFFECTED_BY_ED25519_ERRATA
 *
 * For minimal code size and performance savings: if it is guaranteed that none
 * of the devices running this library has SE FWs in the range [1.2.2, 1.2.8],
 * then enabling this option will disable runtime version checks.
 */
#define SL_SE_ASSUME_FW_UNAFFECTED_BY_ED25519_ERRATA
#else
/* Default configuration for pubkey validation: check for incompatible firmware
 * revisions at runtime, but don't include fallback code unless specifically
 * requested. */
// #define SL_SE_SUPPORT_FW_PRIOR_TO_1_2_2
// #define SL_SE_ASSUME_FW_AT_LEAST_1_2_2

/* Default configuration for X25519/Ed25519 support on Vault mid devices:
 * assume that SE firmware has not been updated. */
// #define SL_SE_ASSUME_FW_AT_LEAST_1_2_10
// #define SL_SE_ASSUME_FW_AT_LEAST_2_1_7

/* Default configuration for Ed25519 errata runtime checking: assume that the SE
 * firmware is among the affected versions. */
// #define SL_SE_ASSUME_FW_UNAFFECTED_BY_ED25519_ERRATA
#endif

/* Additional SE FW version assumption handling. */
#if defined(SL_SE_ASSUME_FW_AT_LEAST_1_2_10)
  #define SL_SE_ASSUME_FW_AT_LEAST_1_2_2
  #define SL_SE_ASSUME_FW_UNAFFECTED_BY_ED25519_ERRATA
#endif
#if !defined(_SILICON_LABS_32B_SERIES_2_CONFIG_1)
  #undef SL_SE_ASSUME_FW_AT_LEAST_1_2_2
  #undef SL_SE_ASSUME_FW_AT_LEAST_1_2_10
  #undef SL_SE_ASSUME_FW_UNAFFECTED_BY_ED25519_ERRATA
#endif
#if !defined(_SILICON_LABS_32B_SERIES_2_CONFIG_3)
  #undef SL_SE_ASSUME_FW_AT_LEAST_2_1_7
#endif

#if  !defined(MBEDTLS_ECP_DP_SECP224R1_ENABLED) \
  && !defined(MBEDTLS_ECP_DP_SECP192K1_ENABLED) \
  && !defined(MBEDTLS_ECP_DP_SECP224K1_ENABLED) \
  && !defined(MBEDTLS_ECP_DP_SECP256K1_ENABLED) \
  && !defined(MBEDTLS_ECP_DP_BP256R1_ENABLED)   \
  && !defined(MBEDTLS_ECP_DP_BP384R1_ENABLED)   \
  && !defined(MBEDTLS_ECP_DP_BP512R1_ENABLED)   \
  && !defined(MBEDTLS_ECP_DP_CURVE448_ENABLED)

/* Do not enable the ECDH and/or ECDSA ALT implementations when one or more
 * non-accelerated curves are included, then the application needs to
 * use the standard mbedTLS library. */

  #if !( (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_SE) \
  && ((defined(MBEDTLS_ECP_DP_CURVE25519_ENABLED)                              \
  && !(defined(SL_SE_ASSUME_FW_AT_LEAST_1_2_10)                                \
  || defined(SL_SE_ASSUME_FW_AT_LEAST_2_1_7)))                                 \
  || defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED)                                 \
  || defined(MBEDTLS_ECP_DP_SECP521R1_ENABLED)  ) )
    #if defined(SE_COMMAND_CREATE_KEY)
      #define MBEDTLS_ECDH_GEN_PUBLIC_ALT
    #endif
    #if defined(SE_COMMAND_DH)
      #define MBEDTLS_ECDH_COMPUTE_SHARED_ALT
    #endif
  #endif

  #if !( (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_SE) \
  && (defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED)                                \
  || defined(MBEDTLS_ECP_DP_SECP521R1_ENABLED)  ) )
    #if defined(SE_COMMAND_CREATE_KEY)
      #define MBEDTLS_ECDSA_GENKEY_ALT
    #endif
    #if defined(SE_COMMAND_SIGNATURE_SIGN)
      #if !defined(MBEDTLS_ECDSA_DETERMINISTIC)
        #define MBEDTLS_ECDSA_SIGN_ALT
      #endif
    #endif
    #if defined(SE_COMMAND_SIGNATURE_VERIFY)
      #define MBEDTLS_ECDSA_VERIFY_ALT
    #endif
  #endif

#endif // #if !defined(MBEDTLS_ECP_DP_XXXX_ENABLED) && ...

#if defined(SE_COMMAND_JPAKE_GEN_SESSIONKEY) || defined(DOXY_DOC_ONLY)
/**
 * \def MBEDTLS_ECJPAKE_ALT
 *
 * Enable hardware acceleration JPAKE.
 *
 * Module:  sl_mbedtls_support/src/se_jpake.c
 *
 * Requires: \ref MBEDTLS_ECJPAKE_C (SEMAILBOX_PRESENT)
 *
 * See \ref MBEDTLS_ECJPAKE_C for more information.
 */
#define MBEDTLS_ECJPAKE_ALT
#endif

#endif /* SEMAILBOX_PRESENT */

/**
 * \def MBEDTLS_ENTROPY_ADC_PRESENT
 *
 * Decode if device supports retrieving entropy data from the ADC
 * incorporated on devices from Silicon Labs.
 *
 * Requires ADC_PRESENT && _ADC_SINGLECTRLX_VREFSEL_VENTROPY &&
 *          _SILICON_LABS_32B_SERIES_1
 */
#if defined(ADC_PRESENT)                        \
  && defined(_ADC_SINGLECTRLX_VREFSEL_VENTROPY) \
  && defined(_SILICON_LABS_32B_SERIES_1)
#define MBEDTLS_ENTROPY_ADC_PRESENT
#endif

/**
 * \def MBEDTLS_TRNG_PRESENT
 *
 * Determine whether mbedTLS supports the TRNG (if present) on the device.
 *
 * Requires TRNG_PRESENT and not _SILICON_LABS_GECKO_INTERNAL_SDID_95 (xg14)
 */
#if defined(TRNG_PRESENT) \
  && !defined(_SILICON_LABS_GECKO_INTERNAL_SDID_95)
#undef MBEDTLS_TRNG_PRESENT
#define MBEDTLS_TRNG_PRESENT
#endif

/**
 * \def MBEDTLS_ENTROPY_RAIL_PRESENT
 *
 * Determine whether mbedTLS supports RAIL entropy on the device.
 * This is currently only available on a few series-1 devices
 * where there is no functional TRNG.
 *
 * Requires _EFR_DEVICE and one of
 * _SILICON_LABS_GECKO_INTERNAL_SDID_80
 * _SILICON_LABS_GECKO_INTERNAL_SDID_89
 * _SILICON_LABS_GECKO_INTERNAL_SDID_95
 */
#if defined(_EFR_DEVICE)                            \
  && (defined(_SILICON_LABS_GECKO_INTERNAL_SDID_80) \
  || defined(_SILICON_LABS_GECKO_INTERNAL_SDID_89)  \
  || defined(_SILICON_LABS_GECKO_INTERNAL_SDID_95) )
#if defined(SL_CATALOG_RAIL_LIB_PRESENT)
#undef MBEDTLS_ENTROPY_RAIL_PRESENT
#define MBEDTLS_ENTROPY_RAIL_PRESENT
#endif
#endif

/* Default ECC configuration for Silicon Labs devices: */

/* Save RAM by adjusting to our exact needs */
#ifndef MBEDTLS_MPI_MAX_SIZE
#define MBEDTLS_MPI_MAX_SIZE    32 // 384 bits is 48 bytes
#endif

/*
   Set MBEDTLS_ECP_WINDOW_SIZE to configure
   ECC point multiplication window size, see ecp.h:
   2 = Save RAM at the expense of speed
   3 = Improve speed at the expense of RAM
   4 = Optimize speed at the expense of RAM
 */
#define MBEDTLS_ECP_WINDOW_SIZE        2
#define MBEDTLS_ECP_FIXED_POINT_OPTIM  0

#if defined(MBEDTLS_ECP_C)
/* First section: devices with ECP hardware acceleration enabled */
#if defined(MBEDTLS_ECP_INTERNAL_ALT)
/* When the internal ECP implementation is overridden, apply optimisation
 * only when it benefits us for curves we can't accelerate. */
#if defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED) \
  || defined(MBEDTLS_ECP_DP_SECP521R1_ENABLED)
#define MBEDTLS_ECP_NIST_OPTIM
#endif /* Non-accelerated SECP R1 curves requested */
/* If only accelerated curves are requested, and no non-accelerated ones,
 * we can turn on the NO_FALLBACK flag to dead-strip a whole lot of ECC
 * math software implementation. */
#if (defined(MBEDTLS_ECP_DP_SECP192R1_ENABLED)   \
  || defined(MBEDTLS_ECP_DP_SECP224R1_ENABLED)   \
  || defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED) ) \
  && !(defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED) \
  || defined(MBEDTLS_ECP_DP_SECP521R1_ENABLED)   \
  || defined(MBEDTLS_ECP_DP_BP256R1_ENABLED)     \
  || defined(MBEDTLS_ECP_DP_BP384R1_ENABLED)     \
  || defined(MBEDTLS_ECP_DP_BP512R1_ENABLED)     \
  || defined(MBEDTLS_ECP_DP_SECP192K1_ENABLED)   \
  || defined(MBEDTLS_ECP_DP_SECP224K1_ENABLED)   \
  || defined(MBEDTLS_ECP_DP_SECP256K1_ENABLED)   \
  || defined(MBEDTLS_ECP_DP_CURVE25519_ENABLED)  \
  || defined(MBEDTLS_ECP_DP_CURVE448_ENABLED))
#define MBEDTLS_ECP_NO_FALLBACK
#endif /* Only ECP-hardware-accelerated curves requested */
/* Second section: devices with ECDSA / ECDH hardware acceleration (without ECP) */
#elif defined(MBEDTLS_ECDH_COMPUTE_SHARED_ALT) \
  || defined(MBEDTLS_ECDH_GEN_PUBLIC_ALT)      \
  || defined(MBEDTLS_ECDSA_GENKEY_ALT)         \
  || defined(MBEDTLS_ECDSA_SIGN_ALT)           \
  || defined(MBEDTLS_ECDSA_VERIFY_ALT)         \
/* When the upper layers calling into ECP_C are overridden, apply optimisation
 * only when it benefits us for curves we can't accelerate. */
#if (defined(SEMAILBOX_PRESENT) && (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_SE) ) \
  || defined(CRYPTOACC_PRESENT)
#if defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED)   \
  || defined(MBEDTLS_ECP_DP_SECP521R1_ENABLED)  \
  || (defined(MBEDTLS_ECDSA_DETERMINISTIC)      \
  && (defined(MBEDTLS_ECP_DP_SECP192R1_ENABLED) \
  || defined(MBEDTLS_ECP_DP_SECP224R1_ENABLED)  \
  || defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED)  \
  || defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED)  \
  || defined(MBEDTLS_ECP_DP_SECP521R1_ENABLED)))
#define MBEDTLS_ECP_NIST_OPTIM
#endif /* Non-accelerated SECP R1 curves requested */
#endif /* Devices not implementing the full suite of SECP R1 curves */
/* Third section: configurations without any ECP/ECC acceleration at all */
#else
/* When there's no ECC acceleration at all, apply optimisation always when
 * applicable curves are present. */
#if defined(MBEDTLS_ECP_DP_SECP192R1_ENABLED)  \
  || defined(MBEDTLS_ECP_DP_SECP224R1_ENABLED) \
  || defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED) \
  || defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED) \
  || defined(MBEDTLS_ECP_DP_SECP521R1_ENABLED)
#define MBEDTLS_ECP_NIST_OPTIM
#endif /* Software-optimisable curve requested */
#endif /* Different acceleration constellations */
#endif /* MBEDTLS_ECP_C */

/*
   Set max CTR-DRBG seed input size to reasonable default in order to reduce
   stack usage when using CTR-DRBG.
   NOTE:
   Due to existing dependencies we need to keep the setting of
   MBEDTLS_CTR_DRBG_MAX_SEED_INPUT here. However this is subject to be moved
   later, to mbedtls_config.h or mbedtls_config_autogen.h in order to be more
   practical for configuration.
 */
#if !defined(MBEDTLS_CTR_DRBG_MAX_SEED_INPUT)
#if !(defined(MBEDTLS_ECDH_COMPUTE_SHARED_ALT) \
  && defined(MBEDTLS_ECDH_GEN_PUBLIC_ALT)      \
  && defined(MBEDTLS_ECDSA_GENKEY_ALT)         \
  && defined(MBEDTLS_ECDSA_SIGN_ALT)           \
  && defined(MBEDTLS_ECDSA_VERIFY_ALT))
/*
   If any of ECDH and/or ECDSA ALT is/are not enabled, then the ecp_mul_xxx()
   functions will seed the internal drbg (for randomization of projective
   coordinates) with the private key of size corresponding to the curve
   hence we will need to adjust:
 */
#if defined(MBEDTLS_ECP_DP_SECP521R1_ENABLED)
// For key size 521 bits (=66 bytes) add 66 - 32 (256bits default) = 34 bytes
#define MBEDTLS_CTR_DRBG_MAX_SEED_INPUT (MBEDTLS_CTR_DRBG_ENTROPY_LEN + MBEDTLS_CTR_DRBG_KEYSIZE  * 3 / 2 + 66 - 32)
#elif defined(MBEDTLS_ECP_DP_BP512R1_ENABLED)
// For key size 512 bits (=64 bytes) add 64 - 32 (256bits default) = 32 bytes
#define MBEDTLS_CTR_DRBG_MAX_SEED_INPUT (MBEDTLS_CTR_DRBG_ENTROPY_LEN + MBEDTLS_CTR_DRBG_KEYSIZE  * 3 / 2 + 64 - 32)
#elif defined(MBEDTLS_ECP_DP_CURVE448_ENABLED)
// For key size 448 bits (=56 bytes) add 56 - 32 (256bits default) = 24 bytes
#define MBEDTLS_CTR_DRBG_MAX_SEED_INPUT (MBEDTLS_CTR_DRBG_ENTROPY_LEN + MBEDTLS_CTR_DRBG_KEYSIZE  * 3 / 2 + 56 - 32)
#elif defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED)
// For key size 384 bits (=48 bytes) add 48 - 32 (256bits default) = 16 bytes
#define MBEDTLS_CTR_DRBG_MAX_SEED_INPUT (MBEDTLS_CTR_DRBG_ENTROPY_LEN + MBEDTLS_CTR_DRBG_KEYSIZE  * 3 / 2 + 48 - 32)
#elif defined(MBEDTLS_ECP_DP_BP384R1_ENABLED)
// For key size 384 bits (=48 bytes) add 48 - 32 (256bits default) = 16 bytes
#define MBEDTLS_CTR_DRBG_MAX_SEED_INPUT (MBEDTLS_CTR_DRBG_ENTROPY_LEN + MBEDTLS_CTR_DRBG_KEYSIZE  * 3 / 2 + 48 - 32)
#else
// Default value to support curve sizes up to 256 bits ( 32 bytes )
#define MBEDTLS_CTR_DRBG_MAX_SEED_INPUT (MBEDTLS_CTR_DRBG_ENTROPY_LEN + MBEDTLS_CTR_DRBG_KEYSIZE  * 3 / 2)
#endif
#endif
#endif

#endif /* !NO_CRYPTO_ACCELERATION */

/** @} (end section sl_config_device_acceleration) */
/** @} (end addtogroup sl_config_device_acceleration) */

#if defined(MBEDTLS_PLATFORM_NV_SEED_ALT)
// Provide the NV seed function signatures since we have no specific header for them
#include <stddef.h>
int sli_nv_seed_read(unsigned char *buf, size_t buf_len);
int sli_nv_seed_write(unsigned char *buf, size_t buf_len);
#endif /* MBEDTLS_PLATFORM_NV_SEED_ALT */

#endif /* MBEDTLS_CONFIG_DEVICE_ACCELERATION_H */
/// @endcond
