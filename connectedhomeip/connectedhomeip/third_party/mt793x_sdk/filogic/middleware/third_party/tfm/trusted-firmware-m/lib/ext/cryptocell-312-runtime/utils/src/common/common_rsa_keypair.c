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
#include "common_rsa_keypair.h"
#include "common_util_log.h"
#include "cc_pka_hw_plat_defs.h"

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
int32_t CC_CommonGetKeyPair(RSA **pRsaKeyPair, int8_t *PemEncryptedFileName_ptr, int8_t *Key_ptr)
{
    FILE *fp = NULL;

    if (PemEncryptedFileName_ptr == NULL) {
        UTIL_LOG_ERR("Illegal RSA key pair or pwd file name\n");
        return -1;
    }

    fp = fopen (PemEncryptedFileName_ptr, "r");
    if (fp == NULL) {
        UTIL_LOG_ERR("Cannot open RSA file %s\n", PemEncryptedFileName_ptr);
        return -1;
    }


    if ((PEM_read_RSAPrivateKey (fp, pRsaKeyPair, NULL, Key_ptr)) == NULL) {
        UTIL_LOG_ERR("Cannot read RSA private key\n");
        ERR_print_errors_fp(stderr);
        fclose (fp);
        return -1;
    }

    fclose (fp);
    return 0;
}

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
int32_t CC_CommonGetPubKey(RSA **pRsaKeyPair, int8_t *PemEncryptedFileName_ptr)
{
    FILE *fp = NULL;

    if (PemEncryptedFileName_ptr == NULL) {
        UTIL_LOG_ERR("Illegal RSA file name\n");
        return -1;
    }

    fp = fopen (PemEncryptedFileName_ptr, "r");
    if (fp == NULL) {
        UTIL_LOG_ERR("Cannot open RSA file %s\n", PemEncryptedFileName_ptr);
        return -1;
    }


    if ((PEM_read_RSA_PUBKEY(fp, pRsaKeyPair, NULL, NULL)) == NULL) {
        UTIL_LOG_ERR("Cannot read RSA public key\n");
        ERR_print_errors_fp(stderr);
        fclose (fp);
        return -1;
    }

    fclose (fp);
    return 0;
}

/**
* @brief The function CC_CommonRsaCalculateNp calculates the Np it returns it as array of ascii's
*
* @param[in] N_ptr - public key N, represented as array of ascii's (0xbc is translated
*                    to 0x62 0x63)
* @param[out] NP_ptr - The NP result. NP size is NP_SIZE_IN_BYTES*2 + 1
*
*/
/*********************************************************/
SBUEXPORT_C int32_t CC_CommonRsaCalculateNp(const int8_t *N_ptr,
                      int8_t *NP_ptr)
{
    int8_t *N_Temp = NULL;
    int32_t  status  = -1;
    BIGNUM *bn_n = BN_new();

    if ((NULL == N_ptr) || (NULL == NP_ptr)) {
        UTIL_LOG_ERR("Illegal input\n");
        goto calcNp_end;
    }

    /* Copy the N to temporary N, allocate temporary N in N size + 2 */
    N_Temp= (int8_t *)malloc ((SB_CERT_RSA_KEY_SIZE_IN_BYTES * 2 + 2) * sizeof (int8_t));
    if (NULL == N_Temp) {
        UTIL_LOG_ERR("failed to malloc.\n");
        goto calcNp_end;
    }

    if (NULL == bn_n) {
        UTIL_LOG_ERR("failed to BN_new.\n");
        goto calcNp_end;
    }

    /* set the temporary N to 0 */
    memset(N_Temp, 0, (SB_CERT_RSA_KEY_SIZE_IN_BYTES * 2 + 2));

    /* Copy the N to temp N */
    memcpy (N_Temp, N_ptr, SB_CERT_RSA_KEY_SIZE_IN_BYTES * 2);

    if (!BN_hex2bn (&bn_n, N_Temp)) {
        UTIL_LOG_ERR("BN_hex2bn failed.\n");
        goto calcNp_end;
    }

    if (CC_CommonRSACalculateNpInt(bn_n, NP_ptr, NP_HEX) != 0) {
        UTIL_LOG_ERR("CC_CommonRSACalculateNpInt failed.\n");
        goto calcNp_end;
    }

    status = 0;

    calcNp_end:
    if (N_Temp != NULL) {
        free(N_Temp);
    }
    if (bn_n != NULL) {
        BN_free (bn_n);
    }
    return(status);
}

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
                         NP_RESULT_TYPE_t resultType)
{
    int32_t len;
    uint8_t *NP_res = NULL, *NP_resTemp = NULL;
    int32_t  status  = -1;
    BN_CTX *bn_ctx = BN_CTX_new();

    BIGNUM *bn_r   = BN_new();
    BIGNUM *bn_a   = BN_new();
    BIGNUM *bn_p   = BN_new();
    BIGNUM *bn_n   = BN_new();
    BIGNUM *bn_quo = BN_new();
    BIGNUM *bn_rem = BN_new();

    if ((NULL == n) || (NULL == NP_ptr)) {
        UTIL_LOG_ERR("Illegal input parameters.\n");
        goto calcNpInt_end;
    }

    NP_res = (int8_t*)malloc(NP_SIZE_IN_BYTES);
    if (NP_res == NULL) {
        UTIL_LOG_ERR("failed to malloc.\n");
        goto calcNpInt_end;
    }
    if ((NULL == bn_r) ||
        (NULL == bn_a) ||
        (NULL == bn_p) ||
        (NULL == bn_n) ||
        (NULL == bn_quo) ||
        (NULL == bn_rem) ||
        (NULL == bn_ctx)) {
        UTIL_LOG_ERR("failed to BN_new or BN_CTX_new.\n");
        goto calcNpInt_end;
    }

    /* computes a = 2^SNP */
    BN_set_word (bn_a, 2);
    BN_set_word (bn_p, SNP);
    if (!BN_exp (bn_r, bn_a, bn_p, bn_ctx)) {
        UTIL_LOG_ERR("failed to BN_exp.\n");
        goto calcNpInt_end;
    }
    if (!BN_div (bn_quo, bn_rem, bn_r, n, bn_ctx)) {
        UTIL_LOG_ERR("failed to BN_div.\n");
        goto calcNpInt_end;
    }

    if (resultType == NP_BIN) {
        len = BN_bn2bin (bn_quo, NP_res);

        /* Set the output with 0 and than copy the result */
        memset (NP_ptr, 0, NP_SIZE_IN_BYTES);
        memcpy ((uint8_t *)(NP_ptr + (NP_SIZE_IN_BYTES - len)), (int8_t *)NP_res, len);
    } else { /* resultType == HEX*/
        NP_resTemp = BN_bn2hex (bn_quo);
        if (NP_resTemp == NULL) {
            UTIL_LOG_ERR("BN_bn2hex failed\n");
            goto calcNpInt_end;
        }
        if (NP_resTemp[0] == '-'){
            UTIL_LOG_ERR("BN_bn2hex returned negative values\n");
            goto calcNpInt_end;
        }
        len = (int32_t)strlen (NP_resTemp);
        memcpy(NP_res, NP_resTemp, len);

        /* Set the output with 0 and than copy the result */
        memset (NP_ptr, 0, (NP_SIZE_IN_BYTES * 2 + 2));
        memcpy ((int8_t *)(NP_ptr + (NP_SIZE_IN_BYTES * 2 + 2 - len)), (int8_t *)NP_res, len);
    }

    status = 0;

    calcNpInt_end:
    if (NP_res != NULL) {
        free(NP_res);
    }
    if (bn_r != NULL) {
        BN_free (bn_r);
    }
    if (bn_a != NULL) {
        BN_free (bn_a);
    }
    if (bn_p != NULL) {
        BN_free (bn_p);
    }
    if (bn_n != NULL) {
        BN_free (bn_n);
    }
    if (bn_quo != NULL) {
        BN_free (bn_quo);
    }
    if (bn_rem != NULL) {
        BN_free (bn_rem);
    }
    if (bn_ctx != NULL) {
        BN_CTX_free(bn_ctx);
    }
    if (NP_resTemp != NULL){
        OPENSSL_free(NP_resTemp);
    }
    return(status);
}


