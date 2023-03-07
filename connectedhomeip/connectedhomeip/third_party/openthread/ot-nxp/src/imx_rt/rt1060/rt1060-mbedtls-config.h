/*
 *  Copyright (c) 2021, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef RT1060_MBEDTLS_CONFIG_H
#define RT1060_MBEDTLS_CONFIG_H

/**************************** KSDK ********************************************/

#include "fsl_debug_console.h"
#include "fsl_device_registers.h"

/* Enable LTC use in library if there is LTC on chip. */
#if defined(FSL_FEATURE_SOC_LTC_COUNT) && (FSL_FEATURE_SOC_LTC_COUNT > 0)
#include "fsl_ltc.h"

#define LTC_INSTANCE LTC0 /* LTC base register.*/

#if FSL_FEATURE_LTC_HAS_SHA
#define MBEDTLS_FREESCALE_LTC_SHA1   /* Enable use of LTC SHA.*/
#define MBEDTLS_FREESCALE_LTC_SHA256 /* Enable use of LTC SHA256.*/
#endif
#if defined(FSL_FEATURE_LTC_HAS_DES) && FSL_FEATURE_LTC_HAS_DES
#define MBEDTLS_FREESCALE_LTC_DES /* Enable use of LTC DES.*/
#endif
#define MBEDTLS_FREESCALE_LTC_AES /* Enable use of LTC AES.*/
#if defined(FSL_FEATURE_LTC_HAS_GCM) && FSL_FEATURE_LTC_HAS_GCM
#define MBEDTLS_FREESCALE_LTC_AES_GCM /* Enable use of LTC AES GCM.*/
#endif
#if defined(FSL_FEATURE_LTC_HAS_PKHA) && FSL_FEATURE_LTC_HAS_PKHA
#define MBEDTLS_FREESCALE_LTC_PKHA /* Enable use of LTC PKHA.*/
#define FREESCALE_PKHA_INT_MAX_BYTES 256
#endif
#endif

/* Enable MMCAU use in library if there is MMCAU on chip. */
#if defined(FSL_FEATURE_SOC_MMCAU_COUNT) && (FSL_FEATURE_SOC_MMCAU_COUNT > 0)
#include "fsl_mmcau.h"

#define MBEDTLS_FREESCALE_MMCAU_MD5    /* Enable use of MMCAU MD5.*/
#define MBEDTLS_FREESCALE_MMCAU_SHA1   /* Enable use of MMCAU SHA1.*/
#define MBEDTLS_FREESCALE_MMCAU_SHA256 /* Enable use of MMCAU SHA256.*/
#define MBEDTLS_FREESCALE_MMCAU_DES    /* Enable use of MMCAU DES, when LTC is disabled.*/
#define MBEDTLS_FREESCALE_MMCAU_AES    /* Enable use of MMCAU AES, when LTC is disabled.*/
#endif

/* Enable CAU3 use in library if there is CAU3 on chip. */
#if defined(FSL_FEATURE_SOC_CAU3_COUNT) && (FSL_FEATURE_SOC_CAU3_COUNT > 0)
#include "fsl_cau3.h"

#define MBEDTLS_CAU3_COMPLETION_SIGNAL kCAU3_TaskDonePoll

#define MBEDTLS_FREESCALE_CAU3_AES    /* Enable use of CAU3 AES.*/
#define MBEDTLS_FREESCALE_CAU3_DES    /* Enable use of CAU3 DES.*/
#define MBEDTLS_FREESCALE_CAU3_SHA1   /* Enable use of CAU3 SHA1.*/
#define MBEDTLS_FREESCALE_CAU3_SHA256 /* Enable use of CAU3 SHA256.*/
#define MBEDTLS_FREESCALE_CAU3_PKHA   /* Enable use of CAU3 PKHA.*/
#define FREESCALE_PKHA_INT_MAX_BYTES 512

#define MBEDTLS_FREESCALE_CAU3_CIPHER_CMAC
#endif

/* Enable CAAM use in library if there is CAAM on chip. */
#if defined(FSL_FEATURE_SOC_CAAM_COUNT) && (FSL_FEATURE_SOC_CAAM_COUNT > 0) && defined(CRYPTO_USE_DRIVER_CAAM)
#include "fsl_caam.h"

#define CAAM_INSTANCE CAAM

#define MBEDTLS_FREESCALE_CAAM_AES     /* Enable use of CAAM AES.*/
#define MBEDTLS_FREESCALE_CAAM_AES_GCM /* Enable use of CAAM AES GCM.*/

#define MBEDTLS_FREESCALE_CAAM_DES /* Enable use of CAAM DES.*/

#define MBEDTLS_FREESCALE_CAAM_SHA1   /* Enable use of CAAM SHA1.*/
#define MBEDTLS_FREESCALE_CAAM_SHA256 /* Enable use of CAAM SHA256.*/

#define MBEDTLS_FREESCALE_CAAM_PKHA /* Enable use of CAAM PKHA.*/
#define FREESCALE_PKHA_INT_MAX_BYTES 512
#define MBEDTLS_ECP_DP_SECP192R1_ENABLED
#define MBEDTLS_ECP_DP_SECP224R1_ENABLED
#define MBEDTLS_ECP_DP_SECP256R1_ENABLED
#define MBEDTLS_ECP_DP_SECP384R1_ENABLED
#define MBEDTLS_ECP_DP_SECP521R1_ENABLED
#endif

/* Enable DCP use in library if there is DCP on chip. */
#if defined(FSL_FEATURE_SOC_DCP_COUNT) && (FSL_FEATURE_SOC_DCP_COUNT > 0)
#include "fsl_dcp.h"

#define MBEDTLS_FREESCALE_DCP_AES    /* Enable use of DCP AES.*/
#define MBEDTLS_FREESCALE_DCP_SHA1   /* Enable use of DCP SHA1.*/
#define MBEDTLS_FREESCALE_DCP_SHA256 /* Enable use of DCP SHA256.*/

#endif

/* Enable HASHCRYPT use in library if there is HASHCRYPT on chip. */
#if defined(FSL_FEATURE_SOC_HASHCRYPT_COUNT) && (FSL_FEATURE_SOC_HASHCRYPT_COUNT > 0)
#include "fsl_hashcrypt.h"

#define MBEDTLS_FREESCALE_HASHCRYPT_AES /* Enable use of HASHCRYPT AES.*/
/* Hashcrypt without context switch is not able to calculate SHA in parallel with AES.
 * HW acceleration of SHA is disabled by default in MbedTLS integration.
 * HW acceleration of SHA is enabled on chip with context switch.
 */
#if defined(FSL_FEATURE_HASHCRYPT_HAS_RELOAD_FEATURE)
#define MBEDTLS_FREESCALE_HASHCRYPT_SHA1   /* Enable use of HASHCRYPT SHA1.*/
#define MBEDTLS_FREESCALE_HASHCRYPT_SHA256 /* Enable use of HASHCRYPT SHA256.*/
#endif
#endif

#if defined(MBEDTLS_FREESCALE_LTC_PKHA) || defined(MBEDTLS_FREESCALE_CAU3_PKHA) || defined(MBEDTLS_FREESCALE_CAAM_PKHA)
/*
 * This FREESCALE_PKHA_LONG_OPERANDS_ENABLE macro can be defined.
 * In such a case both software and hardware algorithm for TFM is linked in.
 * The decision for which algorithm is used is determined at runtime
 * from size of inputs. If inputs and result can fit into FREESCALE_PKHA_INT_MAX_BYTES
 * then we call hardware algorithm, otherwise we call software algorithm.
 *
 * Note that mbedTLS algorithms break modular operations unefficiently into two steps.
 * First is normal operation, for example non-modular multiply, which can produce number
 * with greater size than operands. Second is modular reduction.
 * The implication of this is that if for example FREESCALE_PKHA_INT_MAX_BYTES is 256 (2048 bits),
 * RSA-2048 still requires the FREESCALE_PKHA_LONG_OPERANDS_ENABLE macro to be defined,
 * otherwise it fails at runtime.
 */
//#define FREESCALE_PKHA_LONG_OPERANDS_ENABLE
#endif

/* Enable AES use in library if there is AES on chip. */
#if defined(FSL_FEATURE_SOC_AES_COUNT) && (FSL_FEATURE_SOC_AES_COUNT > 0)
#include "fsl_aes.h"

#define AES_INSTANCE AES0             /* AES base register.*/
#define MBEDTLS_FREESCALE_LPC_AES     /* Enable use of LPC AES.*/
#define MBEDTLS_FREESCALE_LPC_AES_GCM /* Enable use of LPC AES GCM.*/

#endif

/* Enable SHA use in library if there is SHA on chip. */
#if defined(FSL_FEATURE_SOC_SHA_COUNT) && (FSL_FEATURE_SOC_SHA_COUNT > 0)
#include "fsl_sha.h"

/* SHA HW accelerator does not support to compute multiple interleaved hashes,
 * it doesn't support context switch.
 * HW acceleration of SHA is disabled by default in MbedTLS integration.
 */
//#define SHA_INSTANCE SHA0            /* SHA base register.*/
//#define MBEDTLS_FREESCALE_LPC_SHA1   /* Enable use of LPC SHA.*/
//#define MBEDTLS_FREESCALE_LPC_SHA256 /* Enable use of LPC SHA256.*/
//#define MANUAL_LOAD_SHA_INPUT 1      /* 0 - use MEMADDR, MEMCRL (pseudo-DMA), 1 - manual load */
#endif

/* Enable CASPER use in library if there is CASPER on chip. */
#if defined(FSL_FEATURE_SOC_CASPER_COUNT) && (FSL_FEATURE_SOC_CASPER_COUNT > 0)
#include "fsl_casper.h"

#define CASPER_INSTANCE CASPER        /* CASPER base register.*/
#define MBEDTLS_FREESCALE_CASPER_PKHA /* Enable use of CASPER PKHA.*/
#define FREESCALE_PKHA_INT_MAX_BYTES (512)

#define MBEDTLS_ECP_MUL_COMB_ALT /* Alternate implementation of ecp_mul_comb() */
#define MBEDTLS_ECP_MULADD_ALT   /* Alternate implementation of mbedtls_ecp_muladd() */
#define MBEDTLS_MCUX_CASPER_ECC  /* CASPER implementation */

#define MBEDTLS_ECP_DP_SECP256R1_ENABLED /* Enable ECP_DP_SECP256R1 curve */
#define MBEDTLS_ECP_DP_SECP384R1_ENABLED /* Enable ECP_DP_SECP384R1 curve */
#define MBEDTLS_ECP_DP_SECP521R1_ENABLED /* Enable ECP_DP_SECP521R1 curve */

#endif

/**
 * \def MBEDTLS_FREESCALE_FREERTOS_CALLOC_ALT
 *
 * Enable implementation for FreeRTOS's pvPortCalloc() in ksdk_mbedtls.c module.
 * You can comment this macro if you provide your own alternate implementation.
 *
 */
#if defined(USE_RTOS) && defined(SDK_OS_FREE_RTOS)
#define MBEDTLS_FREESCALE_FREERTOS_CALLOC_ALT
#endif

/* Define ALT functions. */
#define MBEDTLS_ECP_ALT

#if defined(MBEDTLS_FREESCALE_MMCAU_DES) || defined(MBEDTLS_FREESCALE_LTC_DES) || \
    defined(MBEDTLS_FREESCALE_CAAM_DES) || defined(MBEDTLS_FREESCALE_CAU3_DES)
#define MBEDTLS_DES_ALT
#define MBEDTLS_DES3_SETKEY_ENC_ALT
#define MBEDTLS_DES3_SETKEY_DEC_ALT
#define MBEDTLS_DES3_CRYPT_ECB_ALT
#if defined(MBEDTLS_FREESCALE_LTC_DES) || defined(MBEDTLS_FREESCALE_MMCAU_DES) || defined(MBEDTLS_FREESCALE_CAAM_DES)
#define MBEDTLS_DES_SETKEY_ENC_ALT
#define MBEDTLS_DES_SETKEY_DEC_ALT
#define MBEDTLS_DES_CRYPT_ECB_ALT
#endif
#endif
#if defined(MBEDTLS_FREESCALE_LTC_DES) || defined(MBEDTLS_FREESCALE_CAAM_DES)
#define MBEDTLS_DES_CRYPT_CBC_ALT
#define MBEDTLS_DES3_CRYPT_CBC_ALT
#endif
#if defined(MBEDTLS_FREESCALE_CAU3_AES) || defined(MBEDTLS_FREESCALE_DCP_AES)
#define MBEDTLS_AES_ALT
#define MBEDTLS_AES_SETKEY_ENC_ALT
#define MBEDTLS_AES_SETKEY_DEC_ALT
#define MBEDTLS_AES_ENCRYPT_ALT
#define MBEDTLS_AES_DECRYPT_ALT
#define MBEDTLS_AES_ALT_NO_192
#endif
#if defined(MBEDTLS_FREESCALE_DCP_AES)
#define MBEDTLS_AES_CRYPT_CBC_ALT
#define MBEDTLS_AES_ALT_NO_256
//#define MBEDTLS_AES192_ALT_SW
//#define MBEDTLS_AES256_ALT_SW
//#define MBEDTLS_AES_CBC_ALT_SW
#endif
#if defined(MBEDTLS_FREESCALE_LTC_AES) || defined(MBEDTLS_FREESCALE_MMCAU_AES) || \
    defined(MBEDTLS_FREESCALE_LPC_AES) || defined(MBEDTLS_FREESCALE_CAAM_AES) ||  \
    defined(MBEDTLS_FREESCALE_HASHCRYPT_AES)
#define MBEDTLS_AES_ALT
#define MBEDTLS_AES_SETKEY_ENC_ALT
#define MBEDTLS_AES_SETKEY_DEC_ALT
#define MBEDTLS_AES_ENCRYPT_ALT
#define MBEDTLS_AES_DECRYPT_ALT
#endif
#if defined(MBEDTLS_FREESCALE_LTC_AES) || defined(MBEDTLS_FREESCALE_CAAM_AES)
#define MBEDTLS_AES_CRYPT_CBC_ALT
#define MBEDTLS_AES_CRYPT_CTR_ALT
#define MBEDTLS_CCM_CRYPT_ALT
#endif
#if defined(MBEDTLS_FREESCALE_HASHCRYPT_AES)
#define MBEDTLS_AES_CRYPT_CBC_ALT
#define MBEDTLS_AES_CRYPT_CTR_ALT
#endif
#if defined(MBEDTLS_FREESCALE_LTC_AES_GCM) || defined(MBEDTLS_FREESCALE_LPC_AES_GCM) || \
    defined(MBEDTLS_FREESCALE_CAAM_AES_GCM)
#define MBEDTLS_GCM_CRYPT_ALT
#endif
#if defined(MBEDTLS_FREESCALE_LTC_PKHA) || defined(MBEDTLS_FREESCALE_CAU3_PKHA) || defined(MBEDTLS_FREESCALE_CAAM_PKHA)
#define MBEDTLS_MPI_ADD_ABS_ALT
#define MBEDTLS_MPI_SUB_ABS_ALT
#define MBEDTLS_MPI_MUL_MPI_ALT
#define MBEDTLS_MPI_MOD_MPI_ALT
#define MBEDTLS_MPI_EXP_MOD_ALT
#define MBEDTLS_MPI_GCD_ALT
#define MBEDTLS_MPI_INV_MOD_ALT
#define MBEDTLS_MPI_IS_PRIME_ALT
#define MBEDTLS_ECP_MUL_COMB_ALT
#define MBEDTLS_ECP_ADD_ALT
#endif
#if defined(MBEDTLS_FREESCALE_CAU3_PKHA)
#define MBEDTLS_ECP_MUL_MXZ_ALT
#endif
#if defined(MBEDTLS_FREESCALE_CASPER_PKHA)
#define MBEDTLS_RSA_PUBLIC_ALT
#endif
#if defined(MBEDTLS_FREESCALE_LTC_SHA1) || defined(MBEDTLS_FREESCALE_LPC_SHA1) ||   \
    defined(MBEDTLS_FREESCALE_CAAM_SHA1) || defined(MBEDTLS_FREESCALE_CAU3_SHA1) || \
    defined(MBEDTLS_FREESCALE_DCP_SHA1) || defined(MBEDTLS_FREESCALE_HASHCRYPT_SHA1)
#define MBEDTLS_SHA1_ALT
#endif
#if defined(MBEDTLS_FREESCALE_LTC_SHA256) || defined(MBEDTLS_FREESCALE_LPC_SHA256) ||   \
    defined(MBEDTLS_FREESCALE_CAAM_SHA256) || defined(MBEDTLS_FREESCALE_CAU3_SHA256) || \
    defined(MBEDTLS_FREESCALE_DCP_SHA256) || defined(MBEDTLS_FREESCALE_HASHCRYPT_SHA256)
#define MBEDTLS_SHA256_ALT
/*
 * LPC SHA module does not support SHA-224.
 *
 * Since mbed TLS does not provide separate APIs for SHA-224 and SHA-256
 * and SHA-224 is not widely used, this implementation provides HW accelerated SHA-256 only
 * and SHA-224 is not available at all (calls will fail).
 *
 * To use SHA-224 on LPC, do not define MBEDTLS_SHA256_ALT and both SHA-224 and SHA-256 will use
 * original mbed TLS software implementation.
 */
#if defined(MBEDTLS_FREESCALE_LPC_SHA256) || defined(MBEDTLS_FREESCALE_DCP_SHA256) || \
    defined(MBEDTLS_FREESCALE_HASHCRYPT_SHA256)
#define MBEDTLS_SHA256_ALT_NO_224
#endif
#endif
#if defined(MBEDTLS_FREESCALE_MMCAU_MD5)
#define MBEDTLS_MD5_PROCESS_ALT
#endif
#if defined(MBEDTLS_FREESCALE_MMCAU_SHA1)
#define MBEDTLS_SHA1_PROCESS_ALT
#endif
#if defined(MBEDTLS_FREESCALE_MMCAU_SHA256)
#define MBEDTLS_SHA256_PROCESS_ALT
#endif
#if defined(MBEDTLS_FREESCALE_CAU3_SHA1)
#define MBEDTLS_SHA1_PROCESS_ALT
#endif
#if defined(MBEDTLS_FREESCALE_CAU3_SHA256)
#define MBEDTLS_SHA256_ALT_NO_224
#define MBEDTLS_SHA256_PROCESS_ALT
#endif
#if defined(MBEDTLS_FREESCALE_CAU3_CIPHER_CMAC)
#define MBEDTLS_CIPHER_CMAC_ALT

/* this macros can be used to enable or disable AES-CMAC-256 and/or TDES-CMAC with CMAC_ALT hw acceleration */
#define MBEDTLS_CIPHER_CMAC_AES_256_ENABLED
#define MBEDTLS_CIPHER_CMAC_TDES_ENABLED
#endif
#if defined(MBEDTLS_FREESCALE_LTC_AES)
#if !defined(FSL_FEATURE_LTC_HAS_AES192) || !FSL_FEATURE_LTC_HAS_AES192
#define MBEDTLS_AES_ALT_NO_192
#endif
#if !defined(FSL_FEATURE_LTC_HAS_AES256) || !FSL_FEATURE_LTC_HAS_AES256
#define MBEDTLS_AES_ALT_NO_256
#endif
#endif
#if defined(MBEDTLS_FREESCALE_LPC_AES)
#define MBEDTLS_AES_CRYPT_CBC_ALT
#define MBEDTLS_AES_CRYPT_CFB_ALT
#define MBEDTLS_AES_CRYPT_CTR_ALT
#endif
#if defined(MBEDTLS_FREESCALE_LPC_SHA1)
#define MBEDTLS_SHA1_PROCESS_ALT
#endif
#if defined(MBEDTLS_FREESCALE_LPC_SHA256)
#define MBEDTLS_SHA256_PROCESS_ALT
#endif

/* Use SHA-256 HW acceleration for MD COOKIE when SHA-224 mode is not available */
#ifdef MBEDTLS_SHA256_ALT_NO_224
#define FSL_MD_COOKIE_USE_SHA256
#endif

#if defined(USE_RTOS) && defined(SDK_OS_FREE_RTOS)
#include "FreeRTOS.h"

void *pvPortCalloc(size_t num, size_t size); /*Calloc for HEAP3.*/

#define MBEDTLS_PLATFORM_MEMORY
#undef MBEDTLS_PLATFORM_STD_CALLOC
#undef MBEDTLS_PLATFORM_STD_FREE
#define MBEDTLS_PLATFORM_STD_CALLOC pvPortCalloc
#define MBEDTLS_PLATFORM_STD_FREE vPortFree

#endif /* USE_RTOS*/

/* Reduce RAM usage.*/
/* More info: https://tls.mbed.org/kb/how-to/reduce-mbedtls-memory-and-storage-footprint */
#define MBEDTLS_ECP_FIXED_POINT_OPTIM 0 /* To reduce peak memory usage */
#define MBEDTLS_AES_ROM_TABLES
#define MBEDTLS_MPI_WINDOW_SIZE 1
#define MBEDTLS_ECP_WINDOW_SIZE 2

#if defined(MBEDTLS_AES_ALT_NO_256)
/* This macros will add support for CTR_DRBG using AES-128 for crypto engines
 * without AES-256 capability.  */
#define MBEDTLS_CTR_DRBG_USE_128_BIT_KEY
#endif

/**************************** KSDK end ****************************************/

#define MBEDTLS_THREADING_C
#define MBEDTLS_THREADING_ALT

#endif // RT1060_MBEDTLS_CONFIG_H
