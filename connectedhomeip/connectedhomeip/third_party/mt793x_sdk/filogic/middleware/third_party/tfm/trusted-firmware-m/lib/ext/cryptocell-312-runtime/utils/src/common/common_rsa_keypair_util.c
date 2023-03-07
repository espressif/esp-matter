/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <openssl/objects.h>
#include <openssl/pem.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/bn.h>
#include <openssl/aes.h>
#include <openssl/err.h>
#include <openssl/rsa.h>
#include "common_rsa_keypair_util.h"
#include "common_util_files.h"
#include "common_util_log.h"
#include "common_crypto_sym.h"
#include "cc_pka_hw_plat_defs.h"
#define RSA_OAEP_KEY_SIZE_IN_BITS 2048UL //temp -RC
#define RSA_OAEP_KEY_SIZE_IN_BYTES (RSA_OAEP_KEY_SIZE_IN_BITS/8) //temp -RC

/************************************* Globals  *************************************/

uint8_t gNp[NP_SIZE_IN_BYTES] = {0};
DxRsaKeyNandNp_t gNAndNp = {0};
uint8_t gN[SB_CERT_RSA_KEY_SIZE_IN_BYTES +1] = {0};


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
int32_t CC_CommonGetNbuffFromKeyPair(int8_t *PemEncryptedFileName_ptr, int8_t *pwdFileName, uint8_t *pNbuff, uint32_t *pNbuffSize)
{

    int32_t status = -1;
    uint8_t *pwd = NULL;
    RSA *rsa_pkey = NULL;
    int32_t i;

    if ((NULL == pNbuff) ||
        (NULL == pNbuffSize) ||
        (NULL == PemEncryptedFileName_ptr)) {
        return status;
    }
    if (*pNbuffSize != SB_CERT_RSA_KEY_SIZE_IN_BYTES) {
        return status;
    }

    /* parse the passphrase for a given file */
    if ((NULL != pwdFileName)) {
        if (CC_CommonGetPassphrase(pwdFileName, &pwd) != 0) {
            UTIL_LOG_ERR("Failed to retrieve pwd\n");
            goto END;
        }
        }

    rsa_pkey = RSA_new();
    if (NULL == rsa_pkey) {
        UTIL_LOG_ERR("Failed RSA_new\n");
        goto END;
    }
    if (CC_CommonGetKeyPair (&rsa_pkey, PemEncryptedFileName_ptr, pwd) != 0) {
        UTIL_LOG_ERR("Cannot read RSA public key.\n");
        goto END;
    }

    /* get the modulus from BIGNUM to uint8_t* */
    BN_bn2bin(rsa_pkey->n, (uint8_t *)gN);
    UTIL_LOG_BYTE_BUFF("gN", gN, SB_CERT_RSA_KEY_SIZE_IN_BYTES);

    /* copy the Np to the end of N */
    memcpy(pNbuff, gN, SB_CERT_RSA_KEY_SIZE_IN_BYTES);
    *pNbuffSize = SB_CERT_RSA_KEY_SIZE_IN_BYTES;
    status = 0;


    END:
    if (rsa_pkey != NULL) {
        RSA_free(rsa_pkey);
    }
    if (pwd != NULL) {
        free(pwd);
    }
    return status;
}



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
int32_t CC_CommonGetNAndNpFromKeyPair(int8_t *PemEncryptedFileName_ptr, int8_t *pwdFileName, uint8_t *pNAndNp, uint32_t *pNAndNpSize)
{

    int32_t status = -1;
    uint8_t *pwd = NULL;
    RSA *rsa_pkey = NULL;
    DxRsaKeyNandNp_t *pNandNpBuff = (DxRsaKeyNandNp_t *)pNAndNp;
    int32_t i;

    if ((NULL == pNAndNp) ||
        (NULL == pNAndNpSize) ||
        (NULL == PemEncryptedFileName_ptr)) {
        return status;
    }
    if (*pNAndNpSize != sizeof(DxRsaKeyNandNp_t)) {
        return status;
    }

    /* parse the passphrase for a given file */
    if ((NULL != pwdFileName)) {
        if (CC_CommonGetPassphrase(pwdFileName, &pwd) != 0) {
            UTIL_LOG_ERR("Failed to retrieve pwd %s\n", pwdFileName);
            goto END;
        }
    }

    rsa_pkey = RSA_new();

    if (NULL == rsa_pkey) {
        UTIL_LOG_ERR("Failed RSA_new\n");
        goto END;
    }
    if (CC_CommonGetKeyPair (&rsa_pkey, PemEncryptedFileName_ptr, pwd)  != 0) {
        UTIL_LOG_ERR("Cannot read RSA public key.\n");
        goto END;
    }

    /* get the modulus from BIGNUM to uint8_t* */
    BN_bn2bin(rsa_pkey->n, (uint8_t *)gN);

    /* calculate the Np, and get the output as BIGNUM*/
    if (CC_CommonRSACalculateNpInt(rsa_pkey->n, gNp, NP_BIN)) {
        UTIL_LOG_ERR("Failed creating Np\n");
        goto END;
    }

    /* copy the Np to the end of N */
    memcpy(pNandNpBuff->pNBuff, gN, SB_CERT_RSA_KEY_SIZE_IN_BYTES);
    memcpy(pNandNpBuff->pNpBuff, gNp, NP_SIZE_IN_BYTES);
    *pNAndNpSize = (SB_CERT_RSA_KEY_SIZE_IN_BYTES+NP_SIZE_IN_BYTES);
    UTIL_LOG_BYTE_BUFF("gN", gN, SB_CERT_RSA_KEY_SIZE_IN_BYTES);
    UTIL_LOG_BYTE_BUFF("gNp", gNp, NP_SIZE_IN_BYTES);
    status = 0;


    END:
    if (rsa_pkey != NULL) {
        RSA_free(rsa_pkey);
    }
    if (pwd != NULL) {
        free(pwd);
    }
    return status;
}



/**
* @brief The function reads RSA key from the file and returns its N and Np.
*
* @param[in] pubKeyFileName_ptr - file name of the key pair
* @param[out] pNAndNp - N and Np buffer
* @param[in/out] pNAndNpSize - as input - max size of pNAndNp
*                              as output - actual size of pNAndNp
*/
/*********************************************************/
int32_t CC_CommonGetNAndNpFromPubKey(int8_t *pubKeyFileName_ptr, uint8_t *pNAndNp, uint32_t *pNAndNpSize)
{
    int32_t status = -1;
    int32_t i;
    RSA *rsa_pkey = NULL;
    DxRsaKeyNandNp_t *pNandNpBuff = (DxRsaKeyNandNp_t *)pNAndNp;

    if ((NULL == pNAndNp) ||
        (NULL == pNAndNpSize) ||
        (NULL == pubKeyFileName_ptr)) {
        return status;
    }
    if (*pNAndNpSize != sizeof(DxRsaKeyNandNp_t)) {
        return status;
    }
    rsa_pkey = RSA_new();
    if (NULL == rsa_pkey) {
        UTIL_LOG_ERR("Failed RSA_new\n");
        goto END;
    }
    if (CC_CommonGetPubKey (&rsa_pkey, pubKeyFileName_ptr) < 0) {
        UTIL_LOG_ERR("Cannot read RSA public key.\n");
        goto END;
    }

    /* get the modulus from BIGNUM to uint8_t* */
    BN_bn2bin(rsa_pkey->n, (uint8_t *)gN);

    /* calculate the Np, and get the output as BIGNUM*/
    if (CC_CommonRSACalculateNpInt(rsa_pkey->n, gNp, NP_BIN) != 0) {
        UTIL_LOG_ERR("Failed creating Np\n");
        goto END;
    }

    /* copy the Np to the end of N */
    memcpy(pNandNpBuff->pNBuff, gN, SB_CERT_RSA_KEY_SIZE_IN_BYTES);
    memcpy(pNandNpBuff->pNpBuff, gNp, NP_SIZE_IN_BYTES);
    *pNAndNpSize = (SB_CERT_RSA_KEY_SIZE_IN_BYTES+NP_SIZE_IN_BYTES);
    UTIL_LOG_BYTE_BUFF("gN", gN, SB_CERT_RSA_KEY_SIZE_IN_BYTES);
    UTIL_LOG_BYTE_BUFF("gNp", gNp, NP_SIZE_IN_BYTES);
    status = 0;

    END:
    if (rsa_pkey != NULL) {
        RSA_free(rsa_pkey);
    }
    return status;
}


/**
* @brief The CC_CommonCalcHBKFromBuff calculates Np from given pNbuff.
*        Then calculates HASH both N and Np
*
* @param[in] pNBuff - the N - modulus buff
* @param[out] pHash - hash output
* @param[in] hashSize - hash output size
*/
/*********************************************************/
int32_t CC_CommonCalcHBKFromBuff(int8_t* pNBuff, uint8_t *pHash, int32_t hashSize)
{

    int32_t status = -1;
    int32_t i;
    BIGNUM *bn_n = NULL;

    memcpy((uint8_t *)&gNAndNp, pNBuff, RSA_OAEP_KEY_SIZE_IN_BYTES);
    UTIL_LOG_BYTE_BUFF("gN", (uint8_t *)&gNAndNp, SB_CERT_RSA_KEY_SIZE_IN_BYTES);

    /* calculate the Np */
    bn_n = BN_bin2bn(pNBuff, SB_CERT_RSA_KEY_SIZE_IN_BYTES, bn_n);
    if (NULL == bn_n) {
        UTIL_LOG_ERR ("BN_bin2bn failed\n");
        return -1;
    }

    if (CC_CommonRSACalculateNpInt(bn_n, gNp, NP_BIN) != 0) {
        UTIL_LOG_ERR ("BN_bin2bn failed\n");
        goto END;
    }
    UTIL_LOG_BYTE_BUFF("gNp", gNp, NP_SIZE_IN_BYTES);

    /* copy the Np to the end of N and calc hash on both */
    memcpy(gNAndNp.pNpBuff, gNp, NP_SIZE_IN_BYTES);

    /* write hash*/
    /* calculate hash and write to */
    if (CC_CommonCalcHash((uint8_t *)&gNAndNp, sizeof(gNAndNp), pHash, hashSize) != 0) {
        UTIL_LOG_ERR ("Common_CalcHashOnPubKey failed\n");
        goto END;
    }

    /* write hash*/
    status = 0;

    END:
    if (bn_n != NULL) {
        BN_free(bn_n);
    }
    return status;
}

/**
* @brief The CC_CommonCalcHBKFromFile reads RSA key from the file using passphrase
*        and returns its decrypted value.
*
* @param[in] pubKeyFileName_ptr - file name of the public key
* @param[out] pHash - hash output
* @param[in] hashSize - hash output size
*/
/*********************************************************/
int32_t CC_CommonCalcHBKFromFile(int8_t* pubKeyFileName_ptr, uint8_t *pHash, int32_t hashSize)
{

    int32_t status = -1;
    int32_t i;
    RSA *rsa_pkey = NULL;

    rsa_pkey = RSA_new();
    if (NULL == rsa_pkey) {
        UTIL_LOG_ERR("Failed RSA_new\n");
        goto END;
    }
    if (CC_CommonGetPubKey (&rsa_pkey, pubKeyFileName_ptr) != 0) {
        UTIL_LOG_ERR("Cannot read RSA public key\n");
        goto END;
    }

    /* get the modulus from BIGNUM to uint8_t* */
    BN_bn2bin(rsa_pkey->n, (uint8_t *)gN);
    UTIL_LOG_BYTE_BUFF("gN", gN, SB_CERT_RSA_KEY_SIZE_IN_BYTES);

    /* calculate the Np, and get the output as BIGNUM*/
    if (CC_CommonRSACalculateNpInt(rsa_pkey->n, gNp, NP_BIN) != 0) {
        UTIL_LOG_ERR("Failed creating Np\n");
        goto END;
    }
    UTIL_LOG_BYTE_BUFF("gNp", gNp, NP_SIZE_IN_BYTES);

    /* copy the Np to the end of N and calc hash on both */
    memcpy(gNAndNp.pNBuff, gN, SB_CERT_RSA_KEY_SIZE_IN_BYTES);
    memcpy(gNAndNp.pNpBuff, gNp, NP_SIZE_IN_BYTES);
    /* write hash*/

    /* calculate hash and write to */
    if (CC_CommonCalcHash((uint8_t *)&gNAndNp, SB_CERT_RSA_KEY_SIZE_IN_BYTES+NP_SIZE_IN_BYTES, pHash, hashSize) != 0) {
        UTIL_LOG_ERR ("Common_CalcHashOnPubKey failed\n");
        goto END;
    }

    /* write hash*/
    status = 0;

    END:
    if (rsa_pkey != NULL) {
        RSA_free(rsa_pkey);
    }
    return status;
}



