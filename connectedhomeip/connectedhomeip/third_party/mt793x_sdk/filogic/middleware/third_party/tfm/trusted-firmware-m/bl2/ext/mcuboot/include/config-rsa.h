/*
 *  Minimal configuration for using TLS in the bootloader
 *
 *  Copyright (C) 2006-2019, Arm Limited. All rights reserved.
 *  Copyright (C) 2016, Linaro Ltd
 *
 *  SPDX-License-Identifier: Apache-2.0
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
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */

/*
 * Original code taken from mcuboot project at:
 * https://github.com/JuulLabs-OSS/mcuboot
 * Git SHA of the original version: ac55554059147fff718015be9f4bd3108123f50a
 */

/*
 * Minimal configuration for using TLS in the bootloader
 *
 * - RSA signature verification
 */

#ifndef MCUBOOT_MBEDTLS_CONFIG_RSA
#define MCUBOOT_MBEDTLS_CONFIG_RSA

/* System support */
#define MBEDTLS_PLATFORM_C
#define MBEDTLS_PLATFORM_MEMORY
#define MBEDTLS_MEMORY_BUFFER_ALLOC_C
#define MBEDTLS_NO_PLATFORM_ENTROPY
#define MBEDTLS_NO_DEFAULT_ENTROPY_SOURCES

#define MBEDTLS_PLATFORM_EXIT_ALT
#define MBEDTLS_PLATFORM_PRINTF_ALT

#define MBEDTLS_RSA_C
#define MBEDTLS_PKCS1_V15

/* mbed TLS modules */
#define MBEDTLS_ASN1_PARSE_C
#define MBEDTLS_ASN1_WRITE_C
#define MBEDTLS_BIGNUM_C
#define MBEDTLS_MD_C
#define MBEDTLS_OID_C
#define MBEDTLS_SHA256_C

/* Save RAM by adjusting to our exact needs */
#if MCUBOOT_SIGN_RSA_LEN == 3072
#define MBEDTLS_MPI_MAX_SIZE 384
#else /* RSA2048 */
#define MBEDTLS_MPI_MAX_SIZE 256
#endif

#define MBEDTLS_SSL_MAX_CONTENT_LEN 1024

/* Save ROM and a few bytes of RAM by specifying our own ciphersuite list */
#define MBEDTLS_SSL_CIPHERSUITES MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8

#ifdef CRYPTO_HW_ACCELERATOR_OTP_PROVISIONING
#define MBEDTLS_CIPHER_C
#define MBEDTLS_AES_C
#define MBEDTLS_CCM_C
#define MBEDTLS_ECDSA_C
#define MBEDTLS_ECP_C
#define MBEDTLS_ECP_DP_SECP256R1_ENABLED
#define MBEDTLS_ECP_DP_CURVE25519_ENABLED
#endif /* CRYPTO_HW_ACCELERATOR_OTP_PROVISIONING */

#ifdef CRYPTO_HW_ACCELERATOR
#include "mbedtls_accelerator_config.h"
#endif

#include "mbedtls/check_config.h"

#endif /* MCUBOOT_MBEDTLS_CONFIG_RSA */
