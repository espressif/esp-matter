/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _COMMON_RSA_KEYPAIR_UTIL_H
#define _COMMON_RSA_KEYPAIR_UTIL_H

#include <stdint.h>

#include "common_rsa_keypair.h"
#include "cc_pka_hw_plat_defs.h"

typedef struct {
    uint8_t pNBuff[SB_CERT_RSA_KEY_SIZE_IN_BYTES];
    uint8_t pNpBuff[NP_SIZE_IN_BYTES];
}DxRsaKeyNandNp_t;

/**
* @brief The function reads RSA key from the file and returns its N and Np.
*
* @param[in] PemEncryptedFileName_ptr - file name of the key pair
* @param[in] pwdFileName - file name of the password
* @param[out] pNbuff - N  buffer
* @param[in/out] pNbuffSize - as input - max size of pNbuff
*                              as output - actual size of pNbuff
*/
/*********************************************************/
int32_t CC_CommonGetNbuffFromKeyPair(int8_t *PemEncryptedFileName_ptr, int8_t *pwdFileName, uint8_t *pNbuff, uint32_t *pNbuffSize);

/**
* @brief The function reads RSA key from the file and returns its N and Np.
*
* @param[in] PemEncryptedFileName_ptr - file name of the key pair
* @param[in] pwdFileName - file name of the password
* @param[out] pNAndNp - N and Np buffer
* @param[in/out] pNAndNpSize - as input - max size of pNAndNp
*                              as output - actual size of pNAndNp
*/
/*********************************************************/
int32_t CC_CommonGetNAndNpFromKeyPair(int8_t *PemEncryptedFileName_ptr, int8_t *pwdFileName, uint8_t *pNAndNp, uint32_t *pNAndNpSize);


/**
* @brief The function reads RSA key from the file and returns its N and Np.
*
* @param[in] pubKeyFileName_ptr - file name of the key pair
* @param[out] pNAndNp - N and Np buffer
* @param[in/out] pNAndNpSize - as input - max size of pNAndNp
*                              as output - actual size of pNAndNp
*/
/*********************************************************/
int32_t CC_CommonGetNAndNpFromPubKey(int8_t *pubKeyFileName_ptr, uint8_t *pNAndNp, uint32_t *pNAndNpSize);


/**
* @brief The CC_CommonCalcHBKFromFile reads RSA key from the file using passphrase
*        and returns its decrypted value.
*
* @param[in] pubKeyFileName_ptr - file name of the public key
* @param[out] pHash - hash output
* @param[in] hashSize - hash output size
*/
/*********************************************************/
int32_t CC_CommonCalcHBKFromFile(int8_t* pubKeyFileName_ptr, uint8_t *pHash, int32_t hashSize);


#endif
