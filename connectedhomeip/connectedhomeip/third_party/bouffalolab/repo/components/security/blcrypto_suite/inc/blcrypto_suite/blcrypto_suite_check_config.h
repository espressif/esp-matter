/**
 * \file check_config.h
 *
 * \brief Consistency checks for configuration options
 */
/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 *
 *  This file is provided under the Apache License 2.0, or the
 *  GNU General Public License v2.0 or later.
 *
 *  **********
 *  Apache License 2.0:
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  **********
 *
 *  **********
 *  GNU General Public License v2.0 or later:
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *  **********
 */

/*
 * It is recommended to include this file from your config.h
 * in order to catch dependency issues early.
 */

#ifndef BLCRYPTO_SUITE_CHECK_CONFIG_H
#define BLCRYPTO_SUITE_CHECK_CONFIG_H

/*
 * We assume CHAR_BIT is 8 in many places. In practice, this is true on our
 * target platforms, so not an issue, but let's just be extra sure.
 */
#include <limits.h>
#if CHAR_BIT != 8
#error "mbed TLS requires a platform with 8-bit chars"
#endif

#if defined(_WIN32)
#if !defined(BLCRYPTO_SUITE_PLATFORM_C)
#error "BLCRYPTO_SUITE_PLATFORM_C is required on Windows"
#endif

/* Fix the config here. Not convenient to put an #ifdef _WIN32 in config.h as
 * it would confuse config.pl. */
#if !defined(BLCRYPTO_SUITE_PLATFORM_SNPRINTF_ALT) && \
    !defined(BLCRYPTO_SUITE_PLATFORM_SNPRINTF_MACRO)
#define BLCRYPTO_SUITE_PLATFORM_SNPRINTF_ALT
#endif
#endif /* _WIN32 */

#if defined(TARGET_LIKE_MBED) && \
    ( defined(BLCRYPTO_SUITE_NET_C) || defined(BLCRYPTO_SUITE_TIMING_C) )
#error "The NET and TIMING modules are not available for mbed OS - please use the network and timing functions provided by mbed OS"
#endif

#if defined(BLCRYPTO_SUITE_DEPRECATED_WARNING) && \
    !defined(__GNUC__) && !defined(__clang__)
#error "BLCRYPTO_SUITE_DEPRECATED_WARNING only works with GCC and Clang"
#endif

#if defined(BLCRYPTO_SUITE_HAVE_TIME_DATE) && !defined(BLCRYPTO_SUITE_HAVE_TIME)
#error "BLCRYPTO_SUITE_HAVE_TIME_DATE without BLCRYPTO_SUITE_HAVE_TIME does not make sense"
#endif

#if defined(BLCRYPTO_SUITE_AESNI_C) && !defined(BLCRYPTO_SUITE_HAVE_ASM)
#error "BLCRYPTO_SUITE_AESNI_C defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_CTR_DRBG_C) && !defined(BLCRYPTO_SUITE_AES_C)
#error "BLCRYPTO_SUITE_CTR_DRBG_C defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_DHM_C) && !defined(BLCRYPTO_SUITE_BIGNUM_C)
#error "BLCRYPTO_SUITE_DHM_C defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_SSL_TRUNCATED_HMAC_COMPAT) && !defined(BLCRYPTO_SUITE_SSL_TRUNCATED_HMAC)
#error "BLCRYPTO_SUITE_SSL_TRUNCATED_HMAC_COMPAT defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_CMAC_C) && \
    !defined(BLCRYPTO_SUITE_AES_C) && !defined(BLCRYPTO_SUITE_DES_C)
#error "BLCRYPTO_SUITE_CMAC_C defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_NIST_KW_C) && \
    ( !defined(BLCRYPTO_SUITE_AES_C) || !defined(BLCRYPTO_SUITE_CIPHER_C) )
#error "BLCRYPTO_SUITE_NIST_KW_C defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_ECDH_C) && !defined(BLCRYPTO_SUITE_ECP_C)
#error "BLCRYPTO_SUITE_ECDH_C defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_ECDSA_C) &&            \
    ( !defined(BLCRYPTO_SUITE_ECP_C) ||           \
      !defined(BLCRYPTO_SUITE_ASN1_PARSE_C) ||    \
      !defined(BLCRYPTO_SUITE_ASN1_WRITE_C) )
#error "BLCRYPTO_SUITE_ECDSA_C defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_ECJPAKE_C) &&           \
    ( !defined(BLCRYPTO_SUITE_ECP_C) || !defined(BLCRYPTO_SUITE_MD_C) )
#error "BLCRYPTO_SUITE_ECJPAKE_C defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_ECP_RESTARTABLE)           && \
    ( defined(BLCRYPTO_SUITE_ECDH_COMPUTE_SHARED_ALT) || \
      defined(BLCRYPTO_SUITE_ECDH_GEN_PUBLIC_ALT)     || \
      defined(BLCRYPTO_SUITE_ECDSA_SIGN_ALT)          || \
      defined(BLCRYPTO_SUITE_ECDSA_VERIFY_ALT)        || \
      defined(BLCRYPTO_SUITE_ECDSA_GENKEY_ALT)        || \
      defined(BLCRYPTO_SUITE_ECP_INTERNAL_ALT)        || \
      defined(BLCRYPTO_SUITE_ECP_ALT) )
#error "BLCRYPTO_SUITE_ECP_RESTARTABLE defined, but it cannot coexist with an alternative ECP implementation"
#endif

#if defined(BLCRYPTO_SUITE_ECDSA_DETERMINISTIC) && !defined(BLCRYPTO_SUITE_HMAC_DRBG_C)
#error "BLCRYPTO_SUITE_ECDSA_DETERMINISTIC defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_ECP_C) && ( !defined(BLCRYPTO_SUITE_BIGNUM_C) || (    \
    !defined(BLCRYPTO_SUITE_ECP_DP_SECP192R1_ENABLED) &&                  \
    !defined(BLCRYPTO_SUITE_ECP_DP_SECP224R1_ENABLED) &&                  \
    !defined(BLCRYPTO_SUITE_ECP_DP_SECP256R1_ENABLED) &&                  \
    !defined(BLCRYPTO_SUITE_ECP_DP_SECP384R1_ENABLED) &&                  \
    !defined(BLCRYPTO_SUITE_ECP_DP_SECP521R1_ENABLED) &&                  \
    !defined(BLCRYPTO_SUITE_ECP_DP_BP256R1_ENABLED)   &&                  \
    !defined(BLCRYPTO_SUITE_ECP_DP_BP384R1_ENABLED)   &&                  \
    !defined(BLCRYPTO_SUITE_ECP_DP_BP512R1_ENABLED)   &&                  \
    !defined(BLCRYPTO_SUITE_ECP_DP_SECP192K1_ENABLED) &&                  \
    !defined(BLCRYPTO_SUITE_ECP_DP_SECP224K1_ENABLED) &&                  \
    !defined(BLCRYPTO_SUITE_ECP_DP_SECP256K1_ENABLED) &&                  \
    !defined(BLCRYPTO_SUITE_ECP_DP_CURVE25519_ENABLED) &&                 \
    !defined(BLCRYPTO_SUITE_ECP_DP_CURVE448_ENABLED) ) )
#error "BLCRYPTO_SUITE_ECP_C defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_ECP_C) && !(            \
    defined(BLCRYPTO_SUITE_ECP_ALT) ||             \
    defined(BLCRYPTO_SUITE_CTR_DRBG_C) ||          \
    defined(BLCRYPTO_SUITE_HMAC_DRBG_C) ||         \
    defined(BLCRYPTO_SUITE_SHA512_C) ||            \
    defined(BLCRYPTO_SUITE_SHA256_C) ||            \
    defined(BLCRYPTO_SUITE_ECP_NO_INTERNAL_RNG))
#error "BLCRYPTO_SUITE_ECP_C requires a DRBG or SHA-2 module unless BLCRYPTO_SUITE_ECP_NO_INTERNAL_RNG is defined or an alternative implementation is used"
#endif

#if defined(BLCRYPTO_SUITE_PK_PARSE_C) && !defined(BLCRYPTO_SUITE_ASN1_PARSE_C)
#error "BLCRYPTO_SUITE_PK_PARSE_C defined, but not all prerequesites"
#endif

#if defined(BLCRYPTO_SUITE_ENTROPY_C) && (!defined(BLCRYPTO_SUITE_SHA512_C) &&      \
                                    !defined(BLCRYPTO_SUITE_SHA256_C))
#error "BLCRYPTO_SUITE_ENTROPY_C defined, but not all prerequisites"
#endif
#if defined(BLCRYPTO_SUITE_ENTROPY_C) && defined(BLCRYPTO_SUITE_SHA512_C) &&         \
    defined(BLCRYPTO_SUITE_CTR_DRBG_ENTROPY_LEN) && (BLCRYPTO_SUITE_CTR_DRBG_ENTROPY_LEN > 64)
#error "BLCRYPTO_SUITE_CTR_DRBG_ENTROPY_LEN value too high"
#endif
#if defined(BLCRYPTO_SUITE_ENTROPY_C) &&                                            \
    ( !defined(BLCRYPTO_SUITE_SHA512_C) || defined(BLCRYPTO_SUITE_ENTROPY_FORCE_SHA256) ) \
    && defined(BLCRYPTO_SUITE_CTR_DRBG_ENTROPY_LEN) && (BLCRYPTO_SUITE_CTR_DRBG_ENTROPY_LEN > 32)
#error "BLCRYPTO_SUITE_CTR_DRBG_ENTROPY_LEN value too high"
#endif
#if defined(BLCRYPTO_SUITE_ENTROPY_C) && \
    defined(BLCRYPTO_SUITE_ENTROPY_FORCE_SHA256) && !defined(BLCRYPTO_SUITE_SHA256_C)
#error "BLCRYPTO_SUITE_ENTROPY_FORCE_SHA256 defined, but not all prerequisites"
#endif

#if defined(__has_feature)
#if __has_feature(memory_sanitizer)
#define BLCRYPTO_SUITE_HAS_MEMSAN
#endif
#endif
#if defined(BLCRYPTO_SUITE_TEST_CONSTANT_FLOW_MEMSAN) &&  !defined(BLCRYPTO_SUITE_HAS_MEMSAN)
#error "BLCRYPTO_SUITE_TEST_CONSTANT_FLOW_MEMSAN requires building with MemorySanitizer"
#endif
#undef BLCRYPTO_SUITE_HAS_MEMSAN

#if defined(BLCRYPTO_SUITE_TEST_NULL_ENTROPY) && \
    ( !defined(BLCRYPTO_SUITE_ENTROPY_C) || !defined(BLCRYPTO_SUITE_NO_DEFAULT_ENTROPY_SOURCES) )
#error "BLCRYPTO_SUITE_TEST_NULL_ENTROPY defined, but not all prerequisites"
#endif
#if defined(BLCRYPTO_SUITE_TEST_NULL_ENTROPY) && \
     ( defined(BLCRYPTO_SUITE_ENTROPY_NV_SEED) || defined(BLCRYPTO_SUITE_ENTROPY_HARDWARE_ALT) || \
    defined(BLCRYPTO_SUITE_HAVEGE_C) )
#error "BLCRYPTO_SUITE_TEST_NULL_ENTROPY defined, but entropy sources too"
#endif

#if defined(BLCRYPTO_SUITE_GCM_C) && (                                        \
        !defined(BLCRYPTO_SUITE_AES_C) && !defined(BLCRYPTO_SUITE_CAMELLIA_C) )
#error "BLCRYPTO_SUITE_GCM_C defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_ECP_RANDOMIZE_JAC_ALT) && !defined(BLCRYPTO_SUITE_ECP_INTERNAL_ALT)
#error "BLCRYPTO_SUITE_ECP_RANDOMIZE_JAC_ALT defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_ECP_ADD_MIXED_ALT) && !defined(BLCRYPTO_SUITE_ECP_INTERNAL_ALT)
#error "BLCRYPTO_SUITE_ECP_ADD_MIXED_ALT defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_ECP_DOUBLE_JAC_ALT) && !defined(BLCRYPTO_SUITE_ECP_INTERNAL_ALT)
#error "BLCRYPTO_SUITE_ECP_DOUBLE_JAC_ALT defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_ECP_NORMALIZE_JAC_MANY_ALT) && !defined(BLCRYPTO_SUITE_ECP_INTERNAL_ALT)
#error "BLCRYPTO_SUITE_ECP_NORMALIZE_JAC_MANY_ALT defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_ECP_NORMALIZE_JAC_ALT) && !defined(BLCRYPTO_SUITE_ECP_INTERNAL_ALT)
#error "BLCRYPTO_SUITE_ECP_NORMALIZE_JAC_ALT defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_ECP_DOUBLE_ADD_MXZ_ALT) && !defined(BLCRYPTO_SUITE_ECP_INTERNAL_ALT)
#error "BLCRYPTO_SUITE_ECP_DOUBLE_ADD_MXZ_ALT defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_ECP_RANDOMIZE_MXZ_ALT) && !defined(BLCRYPTO_SUITE_ECP_INTERNAL_ALT)
#error "BLCRYPTO_SUITE_ECP_RANDOMIZE_MXZ_ALT defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_ECP_NORMALIZE_MXZ_ALT) && !defined(BLCRYPTO_SUITE_ECP_INTERNAL_ALT)
#error "BLCRYPTO_SUITE_ECP_NORMALIZE_MXZ_ALT defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_HAVEGE_C) && !defined(BLCRYPTO_SUITE_TIMING_C)
#error "BLCRYPTO_SUITE_HAVEGE_C defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_HKDF_C) && !defined(BLCRYPTO_SUITE_MD_C)
#error "BLCRYPTO_SUITE_HKDF_C defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_HMAC_DRBG_C) && !defined(BLCRYPTO_SUITE_MD_C)
#error "BLCRYPTO_SUITE_HMAC_DRBG_C defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_KEY_EXCHANGE_ECDH_ECDSA_ENABLED) &&                 \
    ( !defined(BLCRYPTO_SUITE_ECDH_C) || !defined(BLCRYPTO_SUITE_X509_CRT_PARSE_C) )
#error "BLCRYPTO_SUITE_KEY_EXCHANGE_ECDH_ECDSA_ENABLED defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_KEY_EXCHANGE_ECDH_RSA_ENABLED) &&                 \
    ( !defined(BLCRYPTO_SUITE_ECDH_C) || !defined(BLCRYPTO_SUITE_X509_CRT_PARSE_C) )
#error "BLCRYPTO_SUITE_KEY_EXCHANGE_ECDH_RSA_ENABLED defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_KEY_EXCHANGE_DHE_PSK_ENABLED) && !defined(BLCRYPTO_SUITE_DHM_C)
#error "BLCRYPTO_SUITE_KEY_EXCHANGE_DHE_PSK_ENABLED defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_KEY_EXCHANGE_ECDHE_PSK_ENABLED) &&                     \
    !defined(BLCRYPTO_SUITE_ECDH_C)
#error "BLCRYPTO_SUITE_KEY_EXCHANGE_ECDHE_PSK_ENABLED defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_KEY_EXCHANGE_DHE_RSA_ENABLED) &&                   \
    ( !defined(BLCRYPTO_SUITE_DHM_C) || !defined(BLCRYPTO_SUITE_RSA_C) ||           \
      !defined(BLCRYPTO_SUITE_X509_CRT_PARSE_C) || !defined(BLCRYPTO_SUITE_PKCS1_V15) )
#error "BLCRYPTO_SUITE_KEY_EXCHANGE_DHE_RSA_ENABLED defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_KEY_EXCHANGE_ECDHE_RSA_ENABLED) &&                 \
    ( !defined(BLCRYPTO_SUITE_ECDH_C) || !defined(BLCRYPTO_SUITE_RSA_C) ||          \
      !defined(BLCRYPTO_SUITE_X509_CRT_PARSE_C) || !defined(BLCRYPTO_SUITE_PKCS1_V15) )
#error "BLCRYPTO_SUITE_KEY_EXCHANGE_ECDHE_RSA_ENABLED defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED) &&                 \
    ( !defined(BLCRYPTO_SUITE_ECDH_C) || !defined(BLCRYPTO_SUITE_ECDSA_C) ||          \
      !defined(BLCRYPTO_SUITE_X509_CRT_PARSE_C) )
#error "BLCRYPTO_SUITE_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_KEY_EXCHANGE_RSA_PSK_ENABLED) &&                   \
    ( !defined(BLCRYPTO_SUITE_RSA_C) || !defined(BLCRYPTO_SUITE_X509_CRT_PARSE_C) || \
      !defined(BLCRYPTO_SUITE_PKCS1_V15) )
#error "BLCRYPTO_SUITE_KEY_EXCHANGE_RSA_PSK_ENABLED defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_KEY_EXCHANGE_RSA_ENABLED) &&                       \
    ( !defined(BLCRYPTO_SUITE_RSA_C) || !defined(BLCRYPTO_SUITE_X509_CRT_PARSE_C) || \
      !defined(BLCRYPTO_SUITE_PKCS1_V15) )
#error "BLCRYPTO_SUITE_KEY_EXCHANGE_RSA_ENABLED defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_KEY_EXCHANGE_ECJPAKE_ENABLED) &&                    \
    ( !defined(BLCRYPTO_SUITE_ECJPAKE_C) || !defined(BLCRYPTO_SUITE_SHA256_C) ||      \
      !defined(BLCRYPTO_SUITE_ECP_DP_SECP256R1_ENABLED) )
#error "BLCRYPTO_SUITE_KEY_EXCHANGE_ECJPAKE_ENABLED defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_MEMORY_BUFFER_ALLOC_C) &&                          \
    ( !defined(BLCRYPTO_SUITE_PLATFORM_C) || !defined(BLCRYPTO_SUITE_PLATFORM_MEMORY) )
#error "BLCRYPTO_SUITE_MEMORY_BUFFER_ALLOC_C defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_MEMORY_BACKTRACE) && !defined(BLCRYPTO_SUITE_MEMORY_BUFFER_ALLOC_C)
#error "BLCRYPTO_SUITE_MEMORY_BACKTRACE defined, but not all prerequesites"
#endif

#if defined(BLCRYPTO_SUITE_MEMORY_DEBUG) && !defined(BLCRYPTO_SUITE_MEMORY_BUFFER_ALLOC_C)
#error "BLCRYPTO_SUITE_MEMORY_DEBUG defined, but not all prerequesites"
#endif

#if defined(BLCRYPTO_SUITE_PADLOCK_C) && !defined(BLCRYPTO_SUITE_HAVE_ASM)
#error "BLCRYPTO_SUITE_PADLOCK_C defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_PEM_PARSE_C) && !defined(BLCRYPTO_SUITE_BASE64_C)
#error "BLCRYPTO_SUITE_PEM_PARSE_C defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_PEM_WRITE_C) && !defined(BLCRYPTO_SUITE_BASE64_C)
#error "BLCRYPTO_SUITE_PEM_WRITE_C defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_PK_C) && \
    ( !defined(BLCRYPTO_SUITE_RSA_C) && !defined(BLCRYPTO_SUITE_ECP_C) )
#error "BLCRYPTO_SUITE_PK_C defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_PK_PARSE_C) && !defined(BLCRYPTO_SUITE_PK_C)
#error "BLCRYPTO_SUITE_PK_PARSE_C defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_PK_WRITE_C) && !defined(BLCRYPTO_SUITE_PK_C)
#error "BLCRYPTO_SUITE_PK_WRITE_C defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_PKCS11_C) && !defined(BLCRYPTO_SUITE_PK_C)
#error "BLCRYPTO_SUITE_PKCS11_C defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_PLATFORM_EXIT_ALT) && !defined(BLCRYPTO_SUITE_PLATFORM_C)
#error "BLCRYPTO_SUITE_PLATFORM_EXIT_ALT defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_PLATFORM_EXIT_MACRO) && !defined(BLCRYPTO_SUITE_PLATFORM_C)
#error "BLCRYPTO_SUITE_PLATFORM_EXIT_MACRO defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_PLATFORM_EXIT_MACRO) &&\
    ( defined(BLCRYPTO_SUITE_PLATFORM_STD_EXIT) ||\
        defined(BLCRYPTO_SUITE_PLATFORM_EXIT_ALT) )
#error "BLCRYPTO_SUITE_PLATFORM_EXIT_MACRO and BLCRYPTO_SUITE_PLATFORM_STD_EXIT/BLCRYPTO_SUITE_PLATFORM_EXIT_ALT cannot be defined simultaneously"
#endif

#if defined(BLCRYPTO_SUITE_PLATFORM_TIME_ALT) &&\
    ( !defined(BLCRYPTO_SUITE_PLATFORM_C) ||\
        !defined(BLCRYPTO_SUITE_HAVE_TIME) )
#error "BLCRYPTO_SUITE_PLATFORM_TIME_ALT defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_PLATFORM_TIME_MACRO) &&\
    ( !defined(BLCRYPTO_SUITE_PLATFORM_C) ||\
        !defined(BLCRYPTO_SUITE_HAVE_TIME) )
#error "BLCRYPTO_SUITE_PLATFORM_TIME_MACRO defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_PLATFORM_TIME_TYPE_MACRO) &&\
    ( !defined(BLCRYPTO_SUITE_PLATFORM_C) ||\
        !defined(BLCRYPTO_SUITE_HAVE_TIME) )
#error "BLCRYPTO_SUITE_PLATFORM_TIME_TYPE_MACRO defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_PLATFORM_TIME_MACRO) &&\
    ( defined(BLCRYPTO_SUITE_PLATFORM_STD_TIME) ||\
        defined(BLCRYPTO_SUITE_PLATFORM_TIME_ALT) )
#error "BLCRYPTO_SUITE_PLATFORM_TIME_MACRO and BLCRYPTO_SUITE_PLATFORM_STD_TIME/BLCRYPTO_SUITE_PLATFORM_TIME_ALT cannot be defined simultaneously"
#endif

#if defined(BLCRYPTO_SUITE_PLATFORM_TIME_TYPE_MACRO) &&\
    ( defined(BLCRYPTO_SUITE_PLATFORM_STD_TIME) ||\
        defined(BLCRYPTO_SUITE_PLATFORM_TIME_ALT) )
#error "BLCRYPTO_SUITE_PLATFORM_TIME_TYPE_MACRO and BLCRYPTO_SUITE_PLATFORM_STD_TIME/BLCRYPTO_SUITE_PLATFORM_TIME_ALT cannot be defined simultaneously"
#endif

#if defined(BLCRYPTO_SUITE_PLATFORM_FPRINTF_ALT) && !defined(BLCRYPTO_SUITE_PLATFORM_C)
#error "BLCRYPTO_SUITE_PLATFORM_FPRINTF_ALT defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_PLATFORM_FPRINTF_MACRO) && !defined(BLCRYPTO_SUITE_PLATFORM_C)
#error "BLCRYPTO_SUITE_PLATFORM_FPRINTF_MACRO defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_PLATFORM_FPRINTF_MACRO) &&\
    ( defined(BLCRYPTO_SUITE_PLATFORM_STD_FPRINTF) ||\
        defined(BLCRYPTO_SUITE_PLATFORM_FPRINTF_ALT) )
#error "BLCRYPTO_SUITE_PLATFORM_FPRINTF_MACRO and BLCRYPTO_SUITE_PLATFORM_STD_FPRINTF/BLCRYPTO_SUITE_PLATFORM_FPRINTF_ALT cannot be defined simultaneously"
#endif

#if defined(BLCRYPTO_SUITE_PLATFORM_FREE_MACRO) &&\
    ( !defined(BLCRYPTO_SUITE_PLATFORM_C) || !defined(BLCRYPTO_SUITE_PLATFORM_MEMORY) )
#error "BLCRYPTO_SUITE_PLATFORM_FREE_MACRO defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_PLATFORM_FREE_MACRO) &&\
    defined(BLCRYPTO_SUITE_PLATFORM_STD_FREE)
#error "BLCRYPTO_SUITE_PLATFORM_FREE_MACRO and BLCRYPTO_SUITE_PLATFORM_STD_FREE cannot be defined simultaneously"
#endif

#if defined(BLCRYPTO_SUITE_PLATFORM_FREE_MACRO) && !defined(BLCRYPTO_SUITE_PLATFORM_CALLOC_MACRO)
#error "BLCRYPTO_SUITE_PLATFORM_CALLOC_MACRO must be defined if BLCRYPTO_SUITE_PLATFORM_FREE_MACRO is"
#endif

#if defined(BLCRYPTO_SUITE_PLATFORM_CALLOC_MACRO) &&\
    ( !defined(BLCRYPTO_SUITE_PLATFORM_C) || !defined(BLCRYPTO_SUITE_PLATFORM_MEMORY) )
#error "BLCRYPTO_SUITE_PLATFORM_CALLOC_MACRO defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_PLATFORM_CALLOC_MACRO) &&\
    defined(BLCRYPTO_SUITE_PLATFORM_STD_CALLOC)
#error "BLCRYPTO_SUITE_PLATFORM_CALLOC_MACRO and BLCRYPTO_SUITE_PLATFORM_STD_CALLOC cannot be defined simultaneously"
#endif

#if defined(BLCRYPTO_SUITE_PLATFORM_CALLOC_MACRO) && !defined(BLCRYPTO_SUITE_PLATFORM_FREE_MACRO)
#error "BLCRYPTO_SUITE_PLATFORM_FREE_MACRO must be defined if BLCRYPTO_SUITE_PLATFORM_CALLOC_MACRO is"
#endif

#if defined(BLCRYPTO_SUITE_PLATFORM_MEMORY) && !defined(BLCRYPTO_SUITE_PLATFORM_C)
#error "BLCRYPTO_SUITE_PLATFORM_MEMORY defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_PLATFORM_PRINTF_ALT) && !defined(BLCRYPTO_SUITE_PLATFORM_C)
#error "BLCRYPTO_SUITE_PLATFORM_PRINTF_ALT defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_PLATFORM_PRINTF_MACRO) && !defined(BLCRYPTO_SUITE_PLATFORM_C)
#error "BLCRYPTO_SUITE_PLATFORM_PRINTF_MACRO defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_PLATFORM_PRINTF_MACRO) &&\
    ( defined(BLCRYPTO_SUITE_PLATFORM_STD_PRINTF) ||\
        defined(BLCRYPTO_SUITE_PLATFORM_PRINTF_ALT) )
#error "BLCRYPTO_SUITE_PLATFORM_PRINTF_MACRO and BLCRYPTO_SUITE_PLATFORM_STD_PRINTF/BLCRYPTO_SUITE_PLATFORM_PRINTF_ALT cannot be defined simultaneously"
#endif

#if defined(BLCRYPTO_SUITE_PLATFORM_SNPRINTF_ALT) && !defined(BLCRYPTO_SUITE_PLATFORM_C)
#error "BLCRYPTO_SUITE_PLATFORM_SNPRINTF_ALT defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_PLATFORM_SNPRINTF_MACRO) && !defined(BLCRYPTO_SUITE_PLATFORM_C)
#error "BLCRYPTO_SUITE_PLATFORM_SNPRINTF_MACRO defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_PLATFORM_SNPRINTF_MACRO) &&\
    ( defined(BLCRYPTO_SUITE_PLATFORM_STD_SNPRINTF) ||\
        defined(BLCRYPTO_SUITE_PLATFORM_SNPRINTF_ALT) )
#error "BLCRYPTO_SUITE_PLATFORM_SNPRINTF_MACRO and BLCRYPTO_SUITE_PLATFORM_STD_SNPRINTF/BLCRYPTO_SUITE_PLATFORM_SNPRINTF_ALT cannot be defined simultaneously"
#endif

#if defined(BLCRYPTO_SUITE_PLATFORM_STD_MEM_HDR) &&\
    !defined(BLCRYPTO_SUITE_PLATFORM_NO_STD_FUNCTIONS)
#error "BLCRYPTO_SUITE_PLATFORM_STD_MEM_HDR defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_PLATFORM_STD_CALLOC) && !defined(BLCRYPTO_SUITE_PLATFORM_MEMORY)
#error "BLCRYPTO_SUITE_PLATFORM_STD_CALLOC defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_PLATFORM_STD_CALLOC) && !defined(BLCRYPTO_SUITE_PLATFORM_MEMORY)
#error "BLCRYPTO_SUITE_PLATFORM_STD_CALLOC defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_PLATFORM_STD_FREE) && !defined(BLCRYPTO_SUITE_PLATFORM_MEMORY)
#error "BLCRYPTO_SUITE_PLATFORM_STD_FREE defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_PLATFORM_STD_EXIT) &&\
    !defined(BLCRYPTO_SUITE_PLATFORM_EXIT_ALT)
#error "BLCRYPTO_SUITE_PLATFORM_STD_EXIT defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_PLATFORM_STD_TIME) &&\
    ( !defined(BLCRYPTO_SUITE_PLATFORM_TIME_ALT) ||\
        !defined(BLCRYPTO_SUITE_HAVE_TIME) )
#error "BLCRYPTO_SUITE_PLATFORM_STD_TIME defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_PLATFORM_STD_FPRINTF) &&\
    !defined(BLCRYPTO_SUITE_PLATFORM_FPRINTF_ALT)
#error "BLCRYPTO_SUITE_PLATFORM_STD_FPRINTF defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_PLATFORM_STD_PRINTF) &&\
    !defined(BLCRYPTO_SUITE_PLATFORM_PRINTF_ALT)
#error "BLCRYPTO_SUITE_PLATFORM_STD_PRINTF defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_PLATFORM_STD_SNPRINTF) &&\
    !defined(BLCRYPTO_SUITE_PLATFORM_SNPRINTF_ALT)
#error "BLCRYPTO_SUITE_PLATFORM_STD_SNPRINTF defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_ENTROPY_NV_SEED) &&\
    ( !defined(BLCRYPTO_SUITE_PLATFORM_C) || !defined(BLCRYPTO_SUITE_ENTROPY_C) )
#error "BLCRYPTO_SUITE_ENTROPY_NV_SEED defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_PLATFORM_NV_SEED_ALT) &&\
    !defined(BLCRYPTO_SUITE_ENTROPY_NV_SEED)
#error "BLCRYPTO_SUITE_PLATFORM_NV_SEED_ALT defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_PLATFORM_STD_NV_SEED_READ) &&\
    !defined(BLCRYPTO_SUITE_PLATFORM_NV_SEED_ALT)
#error "BLCRYPTO_SUITE_PLATFORM_STD_NV_SEED_READ defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_PLATFORM_STD_NV_SEED_WRITE) &&\
    !defined(BLCRYPTO_SUITE_PLATFORM_NV_SEED_ALT)
#error "BLCRYPTO_SUITE_PLATFORM_STD_NV_SEED_WRITE defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_PLATFORM_NV_SEED_READ_MACRO) &&\
    ( defined(BLCRYPTO_SUITE_PLATFORM_STD_NV_SEED_READ) ||\
      defined(BLCRYPTO_SUITE_PLATFORM_NV_SEED_ALT) )
#error "BLCRYPTO_SUITE_PLATFORM_NV_SEED_READ_MACRO and BLCRYPTO_SUITE_PLATFORM_STD_NV_SEED_READ cannot be defined simultaneously"
#endif

#if defined(BLCRYPTO_SUITE_PLATFORM_NV_SEED_WRITE_MACRO) &&\
    ( defined(BLCRYPTO_SUITE_PLATFORM_STD_NV_SEED_WRITE) ||\
      defined(BLCRYPTO_SUITE_PLATFORM_NV_SEED_ALT) )
#error "BLCRYPTO_SUITE_PLATFORM_NV_SEED_WRITE_MACRO and BLCRYPTO_SUITE_PLATFORM_STD_NV_SEED_WRITE cannot be defined simultaneously"
#endif

#if defined(BLCRYPTO_SUITE_RSA_C) && ( !defined(BLCRYPTO_SUITE_BIGNUM_C) ||         \
    !defined(BLCRYPTO_SUITE_OID_C) )
#error "BLCRYPTO_SUITE_RSA_C defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_RSA_C) && ( !defined(BLCRYPTO_SUITE_PKCS1_V21) &&         \
    !defined(BLCRYPTO_SUITE_PKCS1_V15) )
#error "BLCRYPTO_SUITE_RSA_C defined, but none of the PKCS1 versions enabled"
#endif

#if defined(BLCRYPTO_SUITE_X509_RSASSA_PSS_SUPPORT) &&                        \
    ( !defined(BLCRYPTO_SUITE_RSA_C) || !defined(BLCRYPTO_SUITE_PKCS1_V21) )
#error "BLCRYPTO_SUITE_X509_RSASSA_PSS_SUPPORT defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_SSL_PROTO_SSL3) && ( !defined(BLCRYPTO_SUITE_MD5_C) ||     \
    !defined(BLCRYPTO_SUITE_SHA1_C) )
#error "BLCRYPTO_SUITE_SSL_PROTO_SSL3 defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_SSL_PROTO_TLS1) && ( !defined(BLCRYPTO_SUITE_MD5_C) ||     \
    !defined(BLCRYPTO_SUITE_SHA1_C) )
#error "BLCRYPTO_SUITE_SSL_PROTO_TLS1 defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_SSL_PROTO_TLS1_1) && ( !defined(BLCRYPTO_SUITE_MD5_C) ||     \
    !defined(BLCRYPTO_SUITE_SHA1_C) )
#error "BLCRYPTO_SUITE_SSL_PROTO_TLS1_1 defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_SSL_PROTO_TLS1_2) && ( !defined(BLCRYPTO_SUITE_SHA1_C) &&     \
    !defined(BLCRYPTO_SUITE_SHA256_C) && !defined(BLCRYPTO_SUITE_SHA512_C) )
#error "BLCRYPTO_SUITE_SSL_PROTO_TLS1_2 defined, but not all prerequisites"
#endif

#if (defined(BLCRYPTO_SUITE_SSL_PROTO_SSL3) || defined(BLCRYPTO_SUITE_SSL_PROTO_TLS1) ||  \
     defined(BLCRYPTO_SUITE_SSL_PROTO_TLS1_1) || defined(BLCRYPTO_SUITE_SSL_PROTO_TLS1_2)) && \
    !(defined(BLCRYPTO_SUITE_KEY_EXCHANGE_RSA_ENABLED) ||                          \
      defined(BLCRYPTO_SUITE_KEY_EXCHANGE_DHE_RSA_ENABLED) ||                      \
      defined(BLCRYPTO_SUITE_KEY_EXCHANGE_ECDHE_RSA_ENABLED) ||                    \
      defined(BLCRYPTO_SUITE_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED) ||                  \
      defined(BLCRYPTO_SUITE_KEY_EXCHANGE_ECDH_RSA_ENABLED) ||                     \
      defined(BLCRYPTO_SUITE_KEY_EXCHANGE_ECDH_ECDSA_ENABLED) ||                   \
      defined(BLCRYPTO_SUITE_KEY_EXCHANGE_PSK_ENABLED) ||                          \
      defined(BLCRYPTO_SUITE_KEY_EXCHANGE_DHE_PSK_ENABLED) ||                      \
      defined(BLCRYPTO_SUITE_KEY_EXCHANGE_RSA_PSK_ENABLED) ||                      \
      defined(BLCRYPTO_SUITE_KEY_EXCHANGE_ECDHE_PSK_ENABLED) ||                    \
      defined(BLCRYPTO_SUITE_KEY_EXCHANGE_ECJPAKE_ENABLED) )
#error "One or more versions of the TLS protocol are enabled " \
        "but no key exchange methods defined with BLCRYPTO_SUITE_KEY_EXCHANGE_xxxx"
#endif

#if defined(BLCRYPTO_SUITE_SSL_PROTO_DTLS)     && \
    !defined(BLCRYPTO_SUITE_SSL_PROTO_TLS1_1)  && \
    !defined(BLCRYPTO_SUITE_SSL_PROTO_TLS1_2)
#error "BLCRYPTO_SUITE_SSL_PROTO_DTLS defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_SSL_CLI_C) && !defined(BLCRYPTO_SUITE_SSL_TLS_C)
#error "BLCRYPTO_SUITE_SSL_CLI_C defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_SSL_TLS_C) && ( !defined(BLCRYPTO_SUITE_CIPHER_C) ||     \
    !defined(BLCRYPTO_SUITE_MD_C) )
#error "BLCRYPTO_SUITE_SSL_TLS_C defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_SSL_SRV_C) && !defined(BLCRYPTO_SUITE_SSL_TLS_C)
#error "BLCRYPTO_SUITE_SSL_SRV_C defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_SSL_TLS_C) && (!defined(BLCRYPTO_SUITE_SSL_PROTO_SSL3) && \
    !defined(BLCRYPTO_SUITE_SSL_PROTO_TLS1) && !defined(BLCRYPTO_SUITE_SSL_PROTO_TLS1_1) && \
    !defined(BLCRYPTO_SUITE_SSL_PROTO_TLS1_2))
#error "BLCRYPTO_SUITE_SSL_TLS_C defined, but no protocols are active"
#endif

#if defined(BLCRYPTO_SUITE_SSL_TLS_C) && (defined(BLCRYPTO_SUITE_SSL_PROTO_SSL3) && \
    defined(BLCRYPTO_SUITE_SSL_PROTO_TLS1_1) && !defined(BLCRYPTO_SUITE_SSL_PROTO_TLS1))
#error "Illegal protocol selection"
#endif

#if defined(BLCRYPTO_SUITE_SSL_TLS_C) && (defined(BLCRYPTO_SUITE_SSL_PROTO_TLS1) && \
    defined(BLCRYPTO_SUITE_SSL_PROTO_TLS1_2) && !defined(BLCRYPTO_SUITE_SSL_PROTO_TLS1_1))
#error "Illegal protocol selection"
#endif

#if defined(BLCRYPTO_SUITE_SSL_TLS_C) && (defined(BLCRYPTO_SUITE_SSL_PROTO_SSL3) && \
    defined(BLCRYPTO_SUITE_SSL_PROTO_TLS1_2) && (!defined(BLCRYPTO_SUITE_SSL_PROTO_TLS1) || \
    !defined(BLCRYPTO_SUITE_SSL_PROTO_TLS1_1)))
#error "Illegal protocol selection"
#endif

#if defined(BLCRYPTO_SUITE_SSL_DTLS_HELLO_VERIFY) && !defined(BLCRYPTO_SUITE_SSL_PROTO_DTLS)
#error "BLCRYPTO_SUITE_SSL_DTLS_HELLO_VERIFY  defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_SSL_DTLS_CLIENT_PORT_REUSE) && \
    !defined(BLCRYPTO_SUITE_SSL_DTLS_HELLO_VERIFY)
#error "BLCRYPTO_SUITE_SSL_DTLS_CLIENT_PORT_REUSE  defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_SSL_DTLS_ANTI_REPLAY) &&                              \
    ( !defined(BLCRYPTO_SUITE_SSL_TLS_C) || !defined(BLCRYPTO_SUITE_SSL_PROTO_DTLS) )
#error "BLCRYPTO_SUITE_SSL_DTLS_ANTI_REPLAY  defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_SSL_DTLS_BADMAC_LIMIT) &&                              \
    ( !defined(BLCRYPTO_SUITE_SSL_TLS_C) || !defined(BLCRYPTO_SUITE_SSL_PROTO_DTLS) )
#error "BLCRYPTO_SUITE_SSL_DTLS_BADMAC_LIMIT  defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_SSL_ENCRYPT_THEN_MAC) &&   \
    !defined(BLCRYPTO_SUITE_SSL_PROTO_TLS1)   &&      \
    !defined(BLCRYPTO_SUITE_SSL_PROTO_TLS1_1) &&      \
    !defined(BLCRYPTO_SUITE_SSL_PROTO_TLS1_2)
#error "BLCRYPTO_SUITE_SSL_ENCRYPT_THEN_MAC defined, but not all prerequsites"
#endif

#if defined(BLCRYPTO_SUITE_SSL_EXTENDED_MASTER_SECRET) && \
    !defined(BLCRYPTO_SUITE_SSL_PROTO_TLS1)   &&          \
    !defined(BLCRYPTO_SUITE_SSL_PROTO_TLS1_1) &&          \
    !defined(BLCRYPTO_SUITE_SSL_PROTO_TLS1_2)
#error "BLCRYPTO_SUITE_SSL_EXTENDED_MASTER_SECRET defined, but not all prerequsites"
#endif

#if defined(BLCRYPTO_SUITE_SSL_TICKET_C) && !defined(BLCRYPTO_SUITE_CIPHER_C)
#error "BLCRYPTO_SUITE_SSL_TICKET_C defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_SSL_CBC_RECORD_SPLITTING) && \
    !defined(BLCRYPTO_SUITE_SSL_PROTO_SSL3) && !defined(BLCRYPTO_SUITE_SSL_PROTO_TLS1)
#error "BLCRYPTO_SUITE_SSL_CBC_RECORD_SPLITTING defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_SSL_SERVER_NAME_INDICATION) && \
        !defined(BLCRYPTO_SUITE_X509_CRT_PARSE_C)
#error "BLCRYPTO_SUITE_SSL_SERVER_NAME_INDICATION defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_THREADING_PTHREAD)
#if !defined(BLCRYPTO_SUITE_THREADING_C) || defined(BLCRYPTO_SUITE_THREADING_IMPL)
#error "BLCRYPTO_SUITE_THREADING_PTHREAD defined, but not all prerequisites"
#endif
#define BLCRYPTO_SUITE_THREADING_IMPL
#endif

#if defined(BLCRYPTO_SUITE_THREADING_ALT)
#if !defined(BLCRYPTO_SUITE_THREADING_C) || defined(BLCRYPTO_SUITE_THREADING_IMPL)
#error "BLCRYPTO_SUITE_THREADING_ALT defined, but not all prerequisites"
#endif
#define BLCRYPTO_SUITE_THREADING_IMPL
#endif

#if defined(BLCRYPTO_SUITE_THREADING_C) && !defined(BLCRYPTO_SUITE_THREADING_IMPL)
#error "BLCRYPTO_SUITE_THREADING_C defined, single threading implementation required"
#endif
#undef BLCRYPTO_SUITE_THREADING_IMPL

#if defined(BLCRYPTO_SUITE_VERSION_FEATURES) && !defined(BLCRYPTO_SUITE_VERSION_C)
#error "BLCRYPTO_SUITE_VERSION_FEATURES defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_X509_USE_C) && ( !defined(BLCRYPTO_SUITE_BIGNUM_C) ||  \
    !defined(BLCRYPTO_SUITE_OID_C) || !defined(BLCRYPTO_SUITE_ASN1_PARSE_C) ||      \
    !defined(BLCRYPTO_SUITE_PK_PARSE_C) )
#error "BLCRYPTO_SUITE_X509_USE_C defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_X509_CREATE_C) && ( !defined(BLCRYPTO_SUITE_BIGNUM_C) ||  \
    !defined(BLCRYPTO_SUITE_OID_C) || !defined(BLCRYPTO_SUITE_ASN1_WRITE_C) ||       \
    !defined(BLCRYPTO_SUITE_PK_WRITE_C) )
#error "BLCRYPTO_SUITE_X509_CREATE_C defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_CERTS_C) && !defined(BLCRYPTO_SUITE_X509_USE_C)
#error "BLCRYPTO_SUITE_CERTS_C defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_X509_CRT_PARSE_C) && ( !defined(BLCRYPTO_SUITE_X509_USE_C) )
#error "BLCRYPTO_SUITE_X509_CRT_PARSE_C defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_X509_CRL_PARSE_C) && ( !defined(BLCRYPTO_SUITE_X509_USE_C) )
#error "BLCRYPTO_SUITE_X509_CRL_PARSE_C defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_X509_CSR_PARSE_C) && ( !defined(BLCRYPTO_SUITE_X509_USE_C) )
#error "BLCRYPTO_SUITE_X509_CSR_PARSE_C defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_X509_CRT_WRITE_C) && ( !defined(BLCRYPTO_SUITE_X509_CREATE_C) )
#error "BLCRYPTO_SUITE_X509_CRT_WRITE_C defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_X509_CSR_WRITE_C) && ( !defined(BLCRYPTO_SUITE_X509_CREATE_C) )
#error "BLCRYPTO_SUITE_X509_CSR_WRITE_C defined, but not all prerequisites"
#endif

#if defined(BLCRYPTO_SUITE_HAVE_INT32) && defined(BLCRYPTO_SUITE_HAVE_INT64)
#error "BLCRYPTO_SUITE_HAVE_INT32 and BLCRYPTO_SUITE_HAVE_INT64 cannot be defined simultaneously"
#endif /* BLCRYPTO_SUITE_HAVE_INT32 && BLCRYPTO_SUITE_HAVE_INT64 */

#if ( defined(BLCRYPTO_SUITE_HAVE_INT32) || defined(BLCRYPTO_SUITE_HAVE_INT64) ) && \
    defined(BLCRYPTO_SUITE_HAVE_ASM)
#error "BLCRYPTO_SUITE_HAVE_INT32/BLCRYPTO_SUITE_HAVE_INT64 and BLCRYPTO_SUITE_HAVE_ASM cannot be defined simultaneously"
#endif /* (BLCRYPTO_SUITE_HAVE_INT32 || BLCRYPTO_SUITE_HAVE_INT64) && BLCRYPTO_SUITE_HAVE_ASM */

/*
 * Avoid warning from -pedantic. This is a convenient place for this
 * workaround since this is included by every single file before the
 * #if defined(BLCRYPTO_SUITE_xxx_C) that results in empty translation units.
 */
typedef int blcrypto_suite_iso_c_forbids_empty_translation_units;

#endif /* BLCRYPTO_SUITE_CHECK_CONFIG_H */
