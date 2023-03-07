/*
 * Copyright (c) 2019, Qorvo Inc
 *
 *   mbedtls_user_config.h
 *   This file decides which components in mbedtls are enabled.
 *
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * $Header:
 * $Change:
 * $DateTime:
 *
 */

#ifndef MBEDTLS_USER_CONFIG_H
#define MBEDTLS_USER_CONFIG_H

/* #define MBEDTLS_SELF_TEST */
#if defined(GP_EXTSTORAGE_DIVERSITY_SECURE)
#define MBEDTLS_AES_C
#define MBEDTLS_CIPHER_MODE_CTR
#endif
/* #define MBEDTLS_AES_ROM_TABLES */
/* #define MBEDTLS_ASN1_PARSE_C */
/* #define MBEDTLS_ASN1_WRITE_C */
/* #define MBEDTLS_BIGNUM_C */
/* #define MBEDTLS_CCM_C */
/* #define MBEDTLS_CIPHER_C */
/* #define MBEDTLS_CMAC_C */
/* #define MBEDTLS_CTR_DRBG_C */
/* #define MBEDTLS_DEBUG_C */
/* #define MBEDTLS_ECJPAKE_C */
/* #define MBEDTLS_ECP_C */
/* #define MBEDTLS_ECP_DP_SECP256R1_ENABLED */
/* #define MBEDTLS_ECP_NIST_OPTIM */
/* #define MBEDTLS_ENTROPY_C */
/* #define MBEDTLS_HAVE_ASM */
/* #define MBEDTLS_HMAC_DRBG_C */
/* #define MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED */
/* #define MBEDTLS_MD_C */
/* #define MBEDTLS_NO_PLATFORM_ENTROPY */
/* #define MBEDTLS_PK_C */
/* #define MBEDTLS_PK_PARSE_C */
#define MBEDTLS_PLATFORM_C
/* #define MBEDTLS_PLATFORM_MEMORY */
#define MBEDTLS_SHA256_C
/* #define MBEDTLS_SHA256_SMALLER */
/* #define MBEDTLS_SSL_CLI_C */
/* #define MBEDTLS_SSL_DTLS_ANTI_REPLAY */
/* #define MBEDTLS_SSL_DTLS_HELLO_VERIFY */
/* #define MBEDTLS_SSL_EXPORT_KEYS */
/* #define MBEDTLS_SSL_MAX_FRAGMENT_LENGTH */
/* #define MBEDTLS_SSL_PROTO_TLS1_2 */
/* #define MBEDTLS_SSL_PROTO_DTLS */
/* #define MBEDTLS_SSL_TLS_C */
/* #define MBEDTLS_MD2_C */
/* #define MBEDTLS_MD4_C */
/* #define MBEDTLS_MD5_C */
/* #define MBEDTLS_RIPEMD160_C */
/* #define MBEDTLS_SHA1_C */
/* #define MBEDTLS_SHA512_C */
/* #define MBEDTLS_ARC4_C */
/* #define MBEDTLS_DES_C */
/* #define MBEDTLS_GCM_C */
/* #define MBEDTLS_NIST_KW_C */
/* #define MBEDTLS_CHACHA20_C */
/* #define MBEDTLS_POLY1305_C */
/* #define MBEDTLS_CHACHAPOLY_C */
/* #define MBEDTLS_RSA_C */
/* #define MBEDTLS_X509_USE_C */
/* #define MBEDTLS_XTEA_C */
/* #define MBEDTLS_CAMELLIA_C */
/* #define MBEDTLS_ARIA_C */
/* #define MBEDTLS_CTR_DRBG_C */
/* #define MBEDTLS_DHM_C */
/* #define MBEDTLS_PKCS5_C */
/* #define MBEDTLS_PKCS1_V21 */
/* #define MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED 1 */
/* #define MBEDTLS_BASE64_C */
/* #define MBEDTLS_ECDH_C */
/* #define MBEDTLS_ECDSA_C */
/* #define MBEDTLS_OID_C */
/* #define MBEDTLS_PEM_PARSE_C */
/* #define MBEDTLS_X509_USE_C */
/* #define MBEDTLS_X509_CRT_PARSE_C */
#define MBEDTLS_PLATFORM_NO_STD_FUNCTIONS
#define MBEDTLS_PLATFORM_SNPRINTF_MACRO    snprintf /**< Default snprint macro to use, can be undefined */

#ifdef GP_TLS_DIVERSITY_USE_MBEDTLS_ALT
/* #undef MBEDTLS_ECP_WINDOW_SIZE */
/* #undef MBEDTLS_ECP_FIXED_POINT_OPTIM */
#define MBEDTLS_AES_ALT
/* #define MBEDTLS_CCM_ALT */
/* #define MBEDTLS_ECP_ALT */
/* #define MBEDTLS_ECJPAKE_ALT */
#define MBEDTLS_SHA256_ALT
/* #define MBEDTLS_ECDSA_VERIFY_ALT */
/* #define MBEDTLS_ECDSA_SIGN_ALT */
/* #define MBEDTLS_SHA512_ALT */
/*
 * Use hardware (Silex Crypto TRNG) entropy source.
 */
#define MBEDTLS_ENTROPY_HARDWARE_ALT
/* For testing purpose, atleast two sources have to be
 * defined, one is the silex crypto TRNG and other is a
 * dummy source defined by mbedtls entropy self test
 */
#define MBEDTLS_ENTROPY_MAX_SOURCES        2
#endif /* GP_TLS_DIVERSITY_USE_MBEDTLS_ALT */

#include "mbedtls/check_config.h"

#endif /* MBEDTLS_CONFIG_H */
