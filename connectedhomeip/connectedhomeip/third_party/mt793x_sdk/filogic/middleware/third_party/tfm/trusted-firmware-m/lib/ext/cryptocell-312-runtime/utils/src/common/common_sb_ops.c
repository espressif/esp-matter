/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include "common_sb_ops.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include <openssl/objects.h>
#include <openssl/pem.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/bn.h>
#include <openssl/aes.h>
#include <openssl/err.h>
#include "common_rsa_keypair.h"
#include "common_rsa_keypair_util.h"
#include "common_crypto_asym.h"
#include "common_util_log.h"
#include "cc_crypto_defs.h"
#include "cc_pka_hw_plat_defs.h"

/**
 * @brief The Sign_v15 generates RSA signature using PKCS#1 v1.5 algorithm.
 *
 * The function
 * 1. Create RSA signature
 * 2. Verify the signature correctness
 * @param[in] pRsaPrivKey - the private key
 * @param[in] DataIn_ptr - the data to sign on
 * @param[in] DataInSize - the data size
 * @param[in] Key_ptr - passphrase string
 * @param[out] Signature_ptr - the RSA signature
 *
 */
 /*********************************************************/
int Sign_v15(RSA *pRsaPrivKey, char *DataIn_ptr,  // IG - merge with common implementation
             int DataInSize, char *Signature_ptr,
             char *Key_ptr)
{
    RSA *pRsaPubKey    = NULL;
    BIO *bio           = NULL;
    int status         = -1;
    EVP_PKEY *pKey     = NULL;
    EVP_MD_CTX *md_ctx = NULL;
    int SignatureSize  = SB_CERT_RSA_KEY_SIZE_IN_BYTES;

    /*EVP_PKEY_new() allocates an empty EVP_PKEY structure which
        is used by OpenSSL to store private keys.*/
    pKey = EVP_PKEY_new();
    /*set the referenced key to key*/
    if (!EVP_PKEY_assign_RSA(pKey, pRsaPrivKey))
        SIGN_RELEASE("EVP_PKEY_assign_RSA Private key")

    /* fill and sign on the HASH output */
    md_ctx = EVP_MD_CTX_create();
    /* initializes a signing context to use the default implementation of digest type.*/
    if (!EVP_SignInit(md_ctx, EVP_sha256()))
        SIGN_RELEASE("EVP_SignInit")
    /*hash data into the signature */
    if (!EVP_SignUpdate(md_ctx, DataIn_ptr, DataInSize))
        SIGN_RELEASE("EVP_SignUpdate")
    /*signs the data using the private key pkey and places the signature*/
    if (!EVP_SignFinal(md_ctx, Signature_ptr, &SignatureSize, pKey))
        SIGN_RELEASE("EVP_SignFinal")

    // Create public key
    bio = BIO_new(BIO_s_mem());
    pRsaPubKey = RSA_new();
    if (!PEM_write_bio_RSA_PUBKEY(bio, pRsaPrivKey))
        SIGN_RELEASE("PEM_write_bio_RSA_PUBKEY")

    if (PEM_read_bio_RSA_PUBKEY(bio,&pRsaPubKey,NULL,Key_ptr) == NULL)
        SIGN_RELEASE("PEM_read_bio_RSA_PUBKEY")

    if (!EVP_PKEY_assign_RSA(pKey, pRsaPubKey))
        SIGN_RELEASE("EVP_PKEY_assign_RSA Public key")
    /* initializes verification context ctx to use the default implementation of digest type*/
    if (!EVP_VerifyInit(md_ctx, EVP_sha256()))
        SIGN_RELEASE("EVP_VerifyInit")
    /*hashes bytes of data into the verification context*/
    if (!EVP_VerifyUpdate(md_ctx, DataIn_ptr, DataInSize))
        SIGN_RELEASE("EVP_VerifyUpdate")
    /*verifies the data in, using the public.*/
    if (!EVP_VerifyFinal(md_ctx, Signature_ptr, SignatureSize, pKey))
        SIGN_RELEASE("EVP_VerifyFinal")

    status = 0;

    SIGN_RELEASE("")
}


/**
 * @brief The Sign_v21 generates RSA signature using PKCS#1 v2.1 algorithm.
 *
 * The function
 * 1. Create RSA signature
 * 2. Verify the signature correctness
 * @param[in] pRsaPrivKey - the private key
 * @param[in] DataIn_ptr - the data to sign on
 * @param[in] DataInSize - the data size
 * @param[out] Signature_ptr - the RSA signature
 *
 */
 /*********************************************************/
int Sign_v21(RSA *pRsaPrivKey, char *DataIn_ptr,
             int DataInSize, char *Signature_ptr)
{
    unsigned char pDigest[HASH_SHA256_DIGEST_SIZE_IN_BYTES];
    unsigned int uDigestLen = HASH_SHA256_DIGEST_SIZE_IN_BYTES;
    EVP_MD_CTX md_ctx;
    unsigned char EM[SB_CERT_RSA_KEY_SIZE_IN_BYTES];
    unsigned char pDecrypted[SB_CERT_RSA_KEY_SIZE_IN_BYTES];
    int status = -1;

   /* hash the message */
    EVP_MD_CTX_init(&md_ctx);
    EVP_DigestInit(&md_ctx, EVP_sha256());
    EVP_DigestUpdate(&md_ctx, (const void*) DataIn_ptr, DataInSize);
    EVP_DigestFinal(&md_ctx, pDigest, &uDigestLen);
    EVP_MD_CTX_cleanup(&md_ctx);

    /* compute the PSS padded data */
    if (!RSA_padding_add_PKCS1_PSS(pRsaPrivKey, EM, pDigest, EVP_sha256(), RSA_SALT_LEN))
        SIGN21_RELEASE("RSA_padding_add_PKCS1_PSS")

    /* perform digital signature */
    if (RSA_private_encrypt(SB_CERT_RSA_KEY_SIZE_IN_BYTES, EM, Signature_ptr, pRsaPrivKey, RSA_NO_PADDING) == -1)
        SIGN21_RELEASE("RSA_private_encrypt")

    /* verify the data */
    if (RSA_public_decrypt(SB_CERT_RSA_KEY_SIZE_IN_BYTES, Signature_ptr, pDecrypted, pRsaPrivKey, RSA_NO_PADDING) == -1)
        SIGN21_RELEASE("RSA_public_decrypt")

    if (RSA_verify_PKCS1_PSS(pRsaPrivKey, pDigest, EVP_sha256(), pDecrypted, RSA_SALT_LEN) != 1)
        SIGN21_RELEASE("RSA_verify_PKCS1_PSS")

    status = 0;

    SIGN21_RELEASE("")
}


void reverseBuff( uint8_t *pBuff , uint32_t size )
{
    uint32_t i;
    uint32_t temp;

    for( i = 0 ; i < (size / 2) ; i++ )
    {
        temp = pBuff[i];
        pBuff[i] = pBuff[size - i - 1];
        pBuff[size - i - 1] = temp;
    }

    return;

}/* END OF UTIL_ReverseBuff */


/**
* @brief The function SBU_GetNFromKeyPairAndCalcH Reads RSA key from the file using passphrase,
*       and returns its decrypted value and its calculated Hash
*
* @param[in] PemEncryptedFileName_ptr - file name
* @param[in] pwdFileName - file name of the password
* @param[out] PemDecryted - N buffer
* @param[out] H_ptr - The H result. H size is N_SIZE_IN_BYTES*2 + 1
*
*/
/*********************************************************/
SBUEXPORT_C int SBU_GetNFromKeyPairAndCalcH(char* PemEncryptedFileName_ptr, char *pwdFileName, char *N_ptr, char *H_ptr)
{
    char *H_res = NULL, *N_Temp = NULL, *H_resTemp = NULL;
    int len, i;
    int status = -1;
    unsigned long s = SB_CERT_RSA_KEY_SIZE_IN_BITS + 2;
    int rc = 0;
    int nSize = SB_CERT_RSA_KEY_SIZE_IN_BYTES;
    unsigned char *pwdPtr = NULL;

    if( strlen(pwdFileName) )
        pwdPtr = pwdFileName;
    else
        pwdPtr = Nullptr;

    OpenSSL_add_all_algorithms ();

    /* get N  buffer */
    rc = CC_CommonGetNbuffFromKeyPair(PemEncryptedFileName_ptr, pwdPtr, N_ptr, &nSize);
    EVP_cleanup();
    CRYPTO_cleanup_all_ex_data();  /* cleanup application specific data to avoid memory leaks.*/
    if (rc != 0) {
        printf( "failed to CC_CommonGetNbuffFromKeyPair %d \n", rc);
        return rc;
    }

    BN_CTX *bn_ctx = BN_CTX_new();

    BIGNUM *bn_two   = BN_new();
    BIGNUM *bn_twos  = BN_new();
    BIGNUM *bn_n     = BN_new();
    BIGNUM *bn_h     = BN_new();

    if ((NULL == N_ptr) || ( NULL == H_ptr))
        CALCULATE_H_RELEASE("SBU_RSA_CalculateH: Illegal input parameters.")

    /* Copy the N to temporary N, allocate temporary N in N size + 2 */
    N_Temp= (char *)malloc ((SB_CERT_RSA_KEY_SIZE_IN_BYTES * 2 + 2) * sizeof (char));

    /* If malloc failed return error */
    if (NULL == N_Temp)
        CALCULATE_H_RELEASE("Error during memory allocation.")

    /* set the temporary N to 0 */
    memset (N_Temp, 0, (SB_CERT_RSA_KEY_SIZE_IN_BYTES * 2 + 2));

    /* Copy the N to temp N */
    memcpy (N_Temp, N_ptr, SB_CERT_RSA_KEY_SIZE_IN_BYTES * 2);

    /* Allocate the output buffer */
    if (NULL == (H_res = (char *)malloc ((SB_CERT_RSA_KEY_SIZE_IN_BYTES * 2 + 2) * sizeof (char))))
        CALCULATE_H_RELEASE("Error during memory allocation.")

    BN_set_word (bn_two, 2);
    BN_set_word (bn_twos, 2 * s);

    bn_n = BN_bin2bn(N_ptr, SB_CERT_RSA_KEY_SIZE_IN_BYTES, bn_n);
    if (NULL == bn_n)
        CALCULATE_H_RELEASE("BN_bin2bn failed.")

    if (!BN_mod_exp (bn_h, bn_two, bn_twos, bn_n, bn_ctx))
        CALCULATE_H_RELEASE("BN_mod_exp failed.")

    H_resTemp = BN_bn2hex (bn_h);
    if (H_resTemp == NULL)
        CALCULATE_H_RELEASE("BN_bn2hex failed.")
    if (H_resTemp[0] == '-'){
        CALCULATE_H_RELEASE("BN_bn2hex failed.")
    }

    len = (int)strlen (H_resTemp);
    memcpy(H_res, H_resTemp, len);

    if (len < SB_CERT_RSA_KEY_SIZE_IN_BYTES * 2)
    {
        memmove (H_res + (SB_CERT_RSA_KEY_SIZE_IN_BYTES * 2 - len), H_res, len + 1);
        for (i = 0; i < (int)(SB_CERT_RSA_KEY_SIZE_IN_BYTES * 2 - len); i++)
            H_res[i] = '0';
    }

    /* Set the output with 0 and than copy the result */
    memset (H_ptr, 0, (SB_CERT_RSA_KEY_SIZE_IN_BYTES * 2 + 2));
    memcpy ((char *)H_ptr, (char *)H_res, SB_CERT_RSA_KEY_SIZE_IN_BYTES * 2 + 2);

    status = 0;

    CALCULATE_H_RELEASE("")
}

/**
* @brief The SBU_RAND_Bytes reads RSA key from the file using passphrase
*        and returns its decrypted value.
*
* @param[in] PemEncryptedFileName_ptr - file name
* @param[in] Key_ptr - passphrase
*/
/*********************************************************/
SBUEXPORT_C int SBU_RAND_Bytes(int numBytes, char *buf)
{
    int result = -1;

    if (numBytes > 0)
    {
        result = RAND_bytes (buf, numBytes);
        if (result <= 0)
            printf ("\nSBU_RAND_Bytes - Internal error: Function RAND_bytes failed\n");
    }
    return result;
}


/**
* @brief Reads RSA key from the file using passphrase, and returns its decrypted value and its Np
*
* @param[in] PemEncryptedFileName_ptr - file name
* @param[in] pwdFileName - file name of the password
* @param[out] PemDecryted - N and Np buffer
*/
/*********************************************************/
SBUEXPORT_C int SBU_GetNAndNpFromKeyPair(char* PemEncryptedFileName_ptr, char *pwdFileName, char *PemDecryted)
{
    int rc = 0;
    int nAndNpSize = SB_CERT_RSA_KEY_SIZE_IN_BYTES+NP_SIZE_IN_BYTES;
    unsigned char *pwdPtr = NULL;

    if( strlen(pwdFileName) )
        pwdPtr = pwdFileName;
    else
        pwdPtr = Nullptr;

    OpenSSL_add_all_algorithms ();

    /* get N and Np buffer */
    rc = CC_CommonGetNAndNpFromKeyPair(PemEncryptedFileName_ptr, pwdPtr, PemDecryted, &nAndNpSize);
    if (rc != 0) {
        printf( "failed to CC_CommonGetNAndNpFromKeyPair %d or ilegal size %d\n", rc, nAndNpSize);
    }

    EVP_cleanup();
    CRYPTO_cleanup_all_ex_data();  /* cleanup application specific data to avoid memory leaks.*/

    return rc;
}


/**
* @brief Reads RSA key from the file using passphrase, and returns its decrypted value and its Np
*
* @param[in] PemEncryptedFileName_ptr - public key file name
* @param[out] PemDecryted - N and Np buffer
*/
/*********************************************************/
SBUEXPORT_C int SBU_GetNAndNpFromPubKey(char* PemEncryptedFileName_ptr, char *PemDecryted)
{
    int rc = 0;
    int nAndNpSize = SB_CERT_RSA_KEY_SIZE_IN_BYTES+NP_SIZE_IN_BYTES;

    OpenSSL_add_all_algorithms ();

    /* get N and Np buffer */
    rc = CC_CommonGetNAndNpFromPubKey(PemEncryptedFileName_ptr, PemDecryted, &nAndNpSize);
    if (rc != 0) {
        printf( "failed to CC_CommonGetNAndNpFromPubKey %d or ilegal size %d\n", rc, nAndNpSize);
    }

    EVP_cleanup();
    CRYPTO_cleanup_all_ex_data();  /* cleanup application specific data to avoid memory leaks.*/

    return rc;
}


/**
* @brief Reads RSA public key from the file and returns its raw value and its Np
*
* @param[in] pPemFileName_ptr - file name
* @param[out] pNAndNp - N and Np buffer
*/
/*********************************************************/
SBUEXPORT_C int SBU_GetHashOfNAndNpFromPubKey(char* pPemFileName_ptr, char *pHash, int hashSize)
{
    int rc = 0;

    OpenSSL_add_all_algorithms ();

    rc = CC_CommonCalcHBKFromFile(pPemFileName_ptr, pHash, hashSize);
    if (rc != 0) {
        printf( "failed to CC_CommonCalcHBKFromFile %d or ilegal size %d\n", rc, hashSize);
    }

    EVP_cleanup();
    CRYPTO_cleanup_all_ex_data();  /* cleanup application specific data to avoid memory leaks.*/

    return rc;
}

