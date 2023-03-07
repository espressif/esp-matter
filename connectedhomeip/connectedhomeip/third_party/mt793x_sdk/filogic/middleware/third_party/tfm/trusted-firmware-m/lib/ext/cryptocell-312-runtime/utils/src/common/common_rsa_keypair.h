/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _COMMON_RSA_KEYPAIR_H
#define _COMMON_RSA_KEYPAIR_H

#include <stdint.h>

#include <openssl/objects.h>
#include <openssl/pem.h>
#include "cc_pka_hw_plat_defs.h"

#ifdef WIN32
#define SBUEXPORT_C __declspec(dllexport)
#else
#define SBUEXPORT_C
#endif

typedef enum NP_RESULT_TYPE {
    NP_BIN = 0,
    NP_HEX = 1
}NP_RESULT_TYPE_t;


/* Global defines */
#define RSA_PRIVATE_KEY_SIZE     SB_CERT_RSA_KEY_SIZE_IN_BITS
#define NP_SIZE_IN_BYTES         20
#define NEW_PKA_WORD_SIZE_BITS   CC_PKA_WORD_SIZE_IN_BITS
#define NEW_PAK_ADDITIONAL_BITS  8
#define SNP                      SB_CERT_RSA_KEY_SIZE_IN_BITS + NEW_PKA_WORD_SIZE_BITS + NEW_PAK_ADDITIONAL_BITS -1


/**
 * @brief The CC_CommonGetKeyPair reads RSA private key from the file, along with retrieving the private key,
 *    it also retrieves the public key.
 *
 * The function
 * 1. Build RSA public key structure
 * @param[out] pRsaPrivKey - the private key
 * @param[in] PemEncryptedFileName_ptr - private key file
 * @param[in] Key_ptr - passphrase string
 *
 */
/*********************************************************/
int32_t CC_CommonGetKeyPair (RSA **pRsaKeyPair, int8_t *PemEncryptedFileName_ptr, int8_t *Key_ptr);

/**
 * @brief The CC_CommonGetPubKey reads RSA public key from the file.
 *
 * The function
 * 1. Build RSA public key structure
 * @param[out] pRsaPrivKey - the rsa key
 * @param[in] PemEncryptedFileName_ptr - public key file name
 *
 */
/*********************************************************/
int32_t CC_CommonGetPubKey (RSA **pRsaKeyPair, int8_t *PemEncryptedFileName_ptr);

/**
 * @brief The function calculates Np when given N as hex data.
 *
 * @param[in] n - modulus as hex data
 * @param[out] NP_ptr - the Np
 *
 */
/*********************************************************/
SBUEXPORT_C int32_t CC_CommonRsaCalculateNp(const int8_t* N_ptr,
                      int8_t *NP_ptr);


/**
 * @brief The function calculates Np when given N as BIGNUM.
 *
 * @param[in] n - modulus as BIGNUM ptr
 * @param[out] NP_ptr - the Np
 *
 */
/*********************************************************/
SBUEXPORT_C int32_t CC_CommonRSACalculateNpInt(BIGNUM *n,
                         uint8_t *NP_ptr,
                         NP_RESULT_TYPE_t resultType);

#endif
