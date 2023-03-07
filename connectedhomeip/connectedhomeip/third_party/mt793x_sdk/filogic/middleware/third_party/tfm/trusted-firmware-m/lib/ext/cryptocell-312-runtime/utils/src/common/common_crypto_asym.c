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
#include "common_rsa_keypair.h"
#include "common_crypto_asym.h"
#include "common_util_log.h"
#include "common_util_files.h"
#include "cc_crypto_defs.h"
#include "cc_pka_hw_plat_defs.h"



/**
 * @brief The Sign_v15 generates RSA signature using PKCS#1 v1.5 algorithm.
 *
 * The function
 * 1. Create RSA signature
 * 2. Verify the signature correctness
 * @param[in] pRsaPrivKey - the private key
 * @param[in] pDataIn - the data to sign on
 * @param[in] dataInSize - the data size
 * @param[in] Key_ptr - passphrase string
 * @param[out] pSignature - the RSA signature
 *
 */
/*********************************************************/
int32_t Sign_v15(RSA  *pRsaPrivKey,
         int8_t *pDataIn,
         int32_t  dataInSize,
         int8_t *pSignature,
         int8_t *Key_ptr)
{
    RSA *pRsaPubKey    = NULL;
    BIO *bio           = NULL;
    int32_t status         = -1;
    EVP_PKEY *pKey     = NULL;
    EVP_MD_CTX *md_ctx = NULL;
    int32_t SignatureSize  = SB_CERT_RSA_KEY_SIZE_IN_BYTES;

    if ((NULL == pRsaPrivKey) ||
        (NULL == pDataIn) ||
        (NULL == pSignature) ||
        (NULL == Key_ptr)) {
        UTIL_LOG_ERR("ilegal input\n");
        return(status);
    }
    /*EVP_PKEY_new() allocates an empty EVP_PKEY structure which
    is used by OpenSSL to store private keys.*/
    pKey = EVP_PKEY_new();
    if (NULL == pKey) {
        UTIL_LOG_ERR("failed to EVP_PKEY_new\n");
        goto rsaSign_v15_end;
    }
    /*set the referenced key to key*/
    if (!EVP_PKEY_assign_RSA(pKey, pRsaPrivKey)) {
        UTIL_LOG_ERR("failed to EVP_PKEY_assign_RSA\n");
        goto rsaSign_v15_end;
    }

    /* fill and sign on the HASH output */
    md_ctx = EVP_MD_CTX_create();
    if (NULL == md_ctx) {
        UTIL_LOG_ERR("failed to EVP_MD_CTX_create\n");
        goto rsaSign_v15_end;
    }
    /* initializes a signing context to use the default implementation of digest type.*/
    if (!EVP_SignInit(md_ctx, EVP_sha256())) {
        UTIL_LOG_ERR("failed to EVP_SignInit\n");
        goto rsaSign_v15_end;
    }
    /*hash data into the signature */
    if (!EVP_SignUpdate(md_ctx, pDataIn, dataInSize)) {
        UTIL_LOG_ERR("failed to EVP_SignUpdate\n");
        goto rsaSign_v15_end;
    }
    /*signs the data using the private key pkey and places the signature*/
    if (!EVP_SignFinal(md_ctx, pSignature, &SignatureSize, pKey)) {
        UTIL_LOG_ERR("failed to EVP_SignFinal\n");
        goto rsaSign_v15_end;
    }

    // Create public key
    bio = BIO_new(BIO_s_mem());
    if (NULL == bio) {
        UTIL_LOG_ERR("failed to BIO_new\n");
        goto rsaSign_v15_end;
    }
    pRsaPubKey = RSA_new();
    if (NULL == pRsaPubKey) {
        UTIL_LOG_ERR("failed to RSA_new\n");
        goto rsaSign_v15_end;
    }
    if (!PEM_write_bio_RSA_PUBKEY(bio, pRsaPrivKey)) {
        UTIL_LOG_ERR("failed to PEM_write_bio_RSA_PUBKEY\n");
        goto rsaSign_v15_end;
    }

    if (PEM_read_bio_RSA_PUBKEY(bio,&pRsaPubKey,NULL,Key_ptr) == NULL) {
        UTIL_LOG_ERR("failed to PEM_read_bio_RSA_PUBKEY\n");
        goto rsaSign_v15_end;
    }

    if (!EVP_PKEY_assign_RSA(pKey, pRsaPubKey)) {
        UTIL_LOG_ERR("failed to EVP_PKEY_assign_RSA\n");
        goto rsaSign_v15_end;
    }
    /* initializes verification context ctx to use the default implementation of digest type*/
    if (!EVP_VerifyInit(md_ctx, EVP_sha256())) {
        UTIL_LOG_ERR("failed to EVP_VerifyInit\n");
        goto rsaSign_v15_end;
    }
    /*hashes bytes of data into the verification context*/
    if (!EVP_VerifyUpdate(md_ctx, pDataIn, dataInSize)) {
        UTIL_LOG_ERR("failed to EVP_VerifyUpdate\n");
        goto rsaSign_v15_end;
    }
    /*verifies the data in, using the public.*/
    if (!EVP_VerifyFinal(md_ctx, pSignature, SignatureSize, pKey)) {
        UTIL_LOG_ERR("failed to EVP_VerifyFinal\n");
        goto rsaSign_v15_end;
    }

    status = 0;

    rsaSign_v15_end:
    if (pRsaPubKey != NULL) {
        RSA_free(pRsaPubKey);
    }
    if (bio != NULL) {
        BIO_free_all(bio);
    }
    if (pKey != NULL) {
        EVP_PKEY_free(pKey);
    }
    if (md_ctx != NULL) {
        EVP_MD_CTX_destroy(md_ctx);
    }
    return(status);
}

/**
 * @brief The Sign_v21 generates RSA signature using PKCS#1 v2.1 algorithm.
 *
 * The function
 * 1. Create RSA signature
 * 2. Verify the signature correctness
 * @param[in] pRsaPrivKey - the private key
 * @param[in] pDataIn - the data to sign on
 * @param[in] dataInSize - the data size
 * @param[out] pSignature - the RSA signature
 *
 */
/*********************************************************/
int32_t Sign_v21(RSA   *pRsaPrivKey,
         int8_t  *pDataIn,
         uint32_t   dataInSize,
         int8_t  *pSignature)
{
    uint8_t pDigest[HASH_SHA256_DIGEST_SIZE_IN_BYTES] = {0};
    uint32_t uDigestLen = HASH_SHA256_DIGEST_SIZE_IN_BYTES;
    EVP_MD_CTX md_ctx;
    uint8_t EM[SB_CERT_RSA_KEY_SIZE_IN_BYTES] = {0};
    uint8_t pDecrypted[SB_CERT_RSA_KEY_SIZE_IN_BYTES] = {0};
    int32_t status = -1;

    if ((NULL == pRsaPrivKey) ||
        (NULL == pDataIn) ||
        (NULL == pSignature)) {
        UTIL_LOG_ERR("ilegal input\n");
        return(status);
    }
    /* hash the message */
    EVP_MD_CTX_init(&md_ctx);
    EVP_DigestInit(&md_ctx, EVP_sha256());
    EVP_DigestUpdate(&md_ctx, (const void*) pDataIn, dataInSize);
    EVP_DigestFinal(&md_ctx, pDigest, &uDigestLen);
    EVP_MD_CTX_cleanup(&md_ctx);

    /* compute the PSS padded data */
    if (!RSA_padding_add_PKCS1_PSS(pRsaPrivKey, EM, pDigest, EVP_sha256(), RSA_SALT_LEN)) {
        return(status);
    }

    /* perform digital signature */
    if (RSA_private_encrypt(SB_CERT_RSA_KEY_SIZE_IN_BYTES, EM, pSignature, pRsaPrivKey, RSA_NO_PADDING) == -1) {
        return(status);
    }

    /* verify the data */
    if (RSA_public_decrypt(SB_CERT_RSA_KEY_SIZE_IN_BYTES, pSignature, pDecrypted, pRsaPrivKey, RSA_NO_PADDING) == -1) {
        return(status);
    }

    if (RSA_verify_PKCS1_PSS(pRsaPrivKey, pDigest, EVP_sha256(), pDecrypted, RSA_SALT_LEN) != 1) {
        return(status);
    }


    status = 0;

    return(status);
}

/**
 * @brief The Sign_v21 generates RSA signature using PKCS#1 v2.1 algorithm.
 *
 * The function
 * 1. Verify RSA signature
 * 2. Verify the signature correctness
 * @param[in] pRsaPrivKey - the private key
 * @param[in] pDataIn - the data to sign on
 * @param[in] dataInSize - the data size
 * @param[out] pSignature - the RSA signature
 *
 */
/*********************************************************/
int32_t Verify_v21(RSA *pRsaPubKey,
           int8_t  *pDataIn,
           int32_t   dataInSize,
           int8_t  *pSignature)
{
    uint8_t pDigest[HASH_SHA256_DIGEST_SIZE_IN_BYTES] = {0};
    uint32_t uDigestLen = HASH_SHA256_DIGEST_SIZE_IN_BYTES;
    EVP_MD_CTX md_ctx;
    uint8_t EM[SB_CERT_RSA_KEY_SIZE_IN_BYTES] = {0};
    uint8_t pDecrypted[SB_CERT_RSA_KEY_SIZE_IN_BYTES] = {0};
    int32_t status = -1;

    if ((NULL == pRsaPubKey) ||
        (NULL == pDataIn) ||
        (NULL == pSignature)) {
        UTIL_LOG_ERR("ilegal input\n");
        return(status);
    }

    /* hash the message */
    EVP_MD_CTX_init(&md_ctx);
    EVP_DigestInit(&md_ctx, EVP_sha256());
    EVP_DigestUpdate(&md_ctx, (const void*) pDataIn, dataInSize);
    EVP_DigestFinal(&md_ctx, pDigest, &uDigestLen);
    EVP_MD_CTX_cleanup(&md_ctx);

    /* decrypt the signature to get the hash */
    if (RSA_public_decrypt(SB_CERT_RSA_KEY_SIZE_IN_BYTES, pSignature, pDecrypted, pRsaPubKey, RSA_NO_PADDING) == -1) {
        return(status);
    }

    if (RSA_verify_PKCS1_PSS(pRsaPubKey,  pDigest, EVP_sha256(), pDecrypted,RSA_SALT_LEN) != 1) {
        return(status);
    }

    UTIL_LOG_INFO("\nVerify_v21: OK\n");
    status = 0;

    return(status);
}

/**
 * @brief Verifies RSA signature.
 *
 * The function follows the steps:
 * 1. Read RSA private key structure
 * 2. Call function according to PKCS version to create RSA signature
 *
 * @param[in] pkcsVersion - the version used (according to global definitions of available versions)
 * @param[in] pDataIn - the data to sign on
 * @param[in] dataInSize - the data size
 * @param[in] pPrivKeyFileName - the private key file
 * @param[in] pPrivKeyPwd - the passphrase string
 * @param[out] pSignature - the RSA signature
 *
 */
/*********************************************************/
int32_t CC_CommonRsaVerify(int32_t  pkcsVersion,
             int8_t *pPubKey,
             int8_t *pDataIn,
             int32_t  dataInSize,
             int8_t *pSignature)
{
    RSA  *pRsaPubKey = NULL;
    int32_t status = -1;
    uint8_t pubKeyExp[] = {0x01, 0x00, 0x01};

    if ((NULL == pPubKey) ||
        (NULL == pDataIn) ||
        (NULL == pSignature)) {
        UTIL_LOG_ERR("ilegal input\n");
        return(status);
    }

    pRsaPubKey = RSA_new();
    if (NULL == pRsaPubKey) {
        UTIL_LOG_ERR("Failed RSA_new\n");
        goto END;
    }

    /* set the modulus  and exponent from int8_t * into RSA key as BIGNUM */
    pRsaPubKey->n = BN_bin2bn(pPubKey, SB_CERT_RSA_KEY_SIZE_IN_BYTES, NULL);
    if (NULL == pRsaPubKey->n) {
        UTIL_LOG_ERR("Failed BN_bin2bn for n\n");
        goto END;
    }
    pRsaPubKey->e = BN_bin2bn(pubKeyExp, sizeof(pubKeyExp),NULL);
    if (NULL == pRsaPubKey->e) {
        UTIL_LOG_ERR("Failed BN_bin2bn for e\n");
        goto END;
    }

    if (RSA_USE_PKCS_21_VERSION == pkcsVersion) {
        status = Verify_v21(pRsaPubKey, pDataIn, dataInSize, pSignature);
    } else {
        UTIL_LOG_ERR("\nCC_CommonRsaVerify: Invalid pkcs version\n");
        goto END;
    }

    END:
    if (pRsaPubKey != NULL) {
        RSA_free(pRsaPubKey);
    }
    return status;
}


/**
 * @brief Generates RSA signature and returns it.
 *
 * The function follows the steps:
 * 1. Read RSA private key structure
 * 2. Call function according to PKCS version to create RSA signature
 *
 * @param[in] pkcsVersion - the version used (according to global definitions of available versions)
 * @param[in] pDataIn - the data to sign on
 * @param[in] dataInSize - the data size
 * @param[in] pPrivKeyFileName - the private key file
 * @param[in] pPrivKeyPwd - the passphrase string
 * @param[out] pSignature - the RSA signature
 *
 */
/*********************************************************/
int32_t CC_CommonRsaSign(int32_t pkcsVersion,
               int8_t *pDataIn,
               uint32_t dataInSize,
               int8_t *pPrivKeyFileName,
               int8_t *pPrivKeyPwd,
               int8_t *pSignature)
{
    RSA  *pRsaPrivKey = NULL;
    uint8_t *pwd = NULL;
    int32_t status = -1;

    if ((NULL == pDataIn) ||
        (NULL == pPrivKeyFileName) ||
        (NULL == pSignature)) {
        UTIL_LOG_ERR("ilegal input\n");
        return(status);
    }

    /* parse the passphrase for a given file */
    if ((NULL != pPrivKeyPwd)) {
        if (CC_CommonGetPassphrase(pPrivKeyPwd, &pwd)) {
        UTIL_LOG_ERR("Failed to retrieve pwd\n");
        goto END;
        }
    }

    pRsaPrivKey = RSA_new();

    if (NULL == pRsaPrivKey) {
        UTIL_LOG_ERR("Failed RSA_new\n");
        goto END;
    }
    if (CC_CommonGetKeyPair (&pRsaPrivKey, pPrivKeyFileName, pwd) < 0) {
        UTIL_LOG_ERR("CC_CommonGetKeyPair Cannot read RSA private key\n");
        goto END;
    }

    if (RSA_USE_PKCS_21_VERSION == pkcsVersion) {
        status = Sign_v21(pRsaPrivKey, pDataIn, dataInSize, pSignature);
    } else if (RSA_USE_PKCS_15_VERSION == pkcsVersion) {
        status = Sign_v15(pRsaPrivKey, pDataIn, dataInSize, pSignature, pwd);
    } else {
        UTIL_LOG_ERR("\nCC_CommonRsaSign: Invalid pkcs version\n");
        goto END;
    }

    END:
    if (pRsaPrivKey != NULL) {
        RSA_free(pRsaPrivKey);
    }
    if (pwd != NULL) {
        free(pwd);
    }
    return status;
}



/**
 * @brief Encrypts data using RSA.
 *
 * The function follows the steps:
 * 1. Read RSA private key structure
 * 2. Call function according to PKCS version to create RSA signature
 *
 * @param[in] pkcsVersion - the version used (according to global definitions of available versions)
 * @param[in] pPrivKeyFileName - the private key file
 * @param[in] pPrivKeyPwd - the passphrase string
 * @param[in] pDataIn - the data to encrypt
 * @param[in] dataInSize - the data size
 * @param[out] pEncData - the encrypted data
 *
 */
/*********************************************************/
int32_t CC_CommonRsaEncrypt(int32_t pkcsVersion,
              int8_t *pPubKey,
              int8_t *pDataIn,
              int32_t  dataInSize,
              int8_t *pEncData)
{

    RSA  *pRsaPubKey = NULL;
    int32_t status = -1;
    uint8_t pubKeyExp[] = {0x01, 0x00, 0x01};

    if ((NULL == pPubKey) ||
        (NULL == pDataIn) ||
        (NULL == pEncData)) {
        UTIL_LOG_ERR("ilegal input\n");
        return(status);
    }
    /* get RSA public key from provided buffer */
    pRsaPubKey = RSA_new();
    if (NULL == pRsaPubKey) {
        UTIL_LOG_ERR("Failed RSA_new\n");
        goto rsaEncryptEnd;
    }

    /* set the modulus  and exponent from int8_t * into RSA key as BIGNUM */
    pRsaPubKey->n = BN_bin2bn(pPubKey, SB_CERT_RSA_KEY_SIZE_IN_BYTES, NULL);
    if (NULL == pRsaPubKey->n) {
        UTIL_LOG_ERR("Failed BN_bin2bn for n\n");
        goto rsaEncryptEnd;
    }
    pRsaPubKey->e = BN_bin2bn(pubKeyExp, sizeof(pubKeyExp),NULL);
    if (NULL == pRsaPubKey->e) {
        UTIL_LOG_ERR("Failed BN_bin2bn for e\n");
        goto rsaEncryptEnd;
    }

    /* now encrypt the data */
    status = RSA_public_encrypt(dataInSize, pDataIn, pEncData, pRsaPubKey, RSA_PKCS1_OAEP_PADDING); /* returns the size of the encrypted data, On error, -1 is returned*/
    if (status != SB_CERT_RSA_KEY_SIZE_IN_BYTES) { /* expected encryped size is RSA modulus sieze */
        UTIL_LOG_ERR("Failed RSA_public_encrypt\n");
        status = -1;
        goto rsaEncryptEnd;
    }
    status = 0;

    rsaEncryptEnd:
    if (pRsaPubKey != NULL) {
        RSA_free(pRsaPubKey);
    }
    return status;
}


/**
 * @brief Decrypts data using RSA.
 *
 * The function follows the steps:
 * 1. Read RSA private key structure
 * 2. Call function according to PKCS version to create RSA signature
 *
 * @param[in] pkcsVersion - the version used (according to global definitions of available versions)
 * @param[in] pPrivKeyFileName - the private key file
 * @param[in] pPrivKeyPwd - the passphrase string
 * @param[in] pEnDataIn - the data to decrypt
 * @param[in] enDataInSize - the encrypted data size
 * @param[out] pData - the decrypted data
 *
 */
/*********************************************************/
int32_t CC_CommonRsaDecrypt(int32_t pkcsVersion,
              int8_t *pPrivKeyFileName,
              int8_t *pPrivKeyPwd,
              int8_t *pEnDataIn,
              int32_t  enDataInSize,
              int8_t *pData)
{
    RSA  *pRsaPrivKey = NULL;
    uint8_t *pwd = NULL;
    int32_t status = -1;

    if ((NULL == pPrivKeyFileName) ||
        (NULL == pEnDataIn) ||
        (NULL == pData)) {
        UTIL_LOG_ERR("ilegal input\n");
        return(status);
    }

    /* parse the passphrase for a given file */
    if ((NULL != pPrivKeyPwd)) {
        if (CC_CommonGetPassphrase(pPrivKeyPwd, &pwd)) {
        UTIL_LOG_ERR("Failed to retrieve pwd\n");
        goto rsaDecryptEnd;
        }
    }

    /* build RSA key from pem private key file */
    pRsaPrivKey = RSA_new();
    if (NULL == pRsaPrivKey) {
        UTIL_LOG_ERR("Failed RSA_new\n");
        goto rsaDecryptEnd;
    }
    if (CC_CommonGetKeyPair (&pRsaPrivKey, pPrivKeyFileName, pwd) < 0) {
        UTIL_LOG_ERR("Cannot read RSA key pair\n");
        goto rsaDecryptEnd;
    }

    /* now decrypt the data */
    status = RSA_private_decrypt(enDataInSize, pEnDataIn, pData, pRsaPrivKey, RSA_PKCS1_OAEP_PADDING);
    if (status != (-1)) {  /* RSA_private_decrypt returns the size of the recovered plaintext. On error, -1 is returned */
        status = 0;
    }

    rsaDecryptEnd:
    if (pRsaPrivKey != NULL) {
        RSA_free(pRsaPrivKey);
    }
    if (pwd != NULL) {
        free(pwd);
    }
    return status;
}





/**
* @brief Calculates the H it returns it as binary string
*
* @param[in] N_ptr - public key N, represented as array of ascii's (0xbc is translated
*                    to 0x62 0x63)
* @param[out] H_ptr - The H result. H size is N_SIZE_IN_BYTES*2 + 1
*
*/
/*********************************************************/
int32_t CC_CommonRsaCalculateH(const int8_t *N_ptr, int8_t *H_ptr)
{
    int8_t *H_res = NULL, *N_Temp = NULL, *H_resTemp = NULL;
    int32_t len, i;
    int32_t status = -1;
    uint32_t s = SB_CERT_RSA_KEY_SIZE_IN_BITS + 2;

    BN_CTX *bn_ctx = BN_CTX_new();

    BIGNUM *bn_two   = BN_new();
    BIGNUM *bn_twos  = BN_new();
    BIGNUM *bn_n     = BN_new();
    BIGNUM *bn_h     = BN_new();

    if ((NULL == N_ptr) || ( NULL == H_ptr)) {
        UTIL_LOG_ERR("illegal input\n");
        goto calcH_end;
    }

    /* Copy the N to temporary N, allocate temporary N in N size + 2 */
    N_Temp= (int8_t *)malloc ((SB_CERT_RSA_KEY_SIZE_IN_BYTES * 2 + 2) * sizeof (int8_t));
    if (NULL == N_Temp) {
        UTIL_LOG_ERR("malloc N_Temp failed\n");
        goto calcH_end;
    }

    /* set the temporary N to 0 */
    memset (N_Temp, 0, (SB_CERT_RSA_KEY_SIZE_IN_BYTES * 2 + 2));

    /* Copy the N to temp N */
    memcpy (N_Temp, N_ptr, SB_CERT_RSA_KEY_SIZE_IN_BYTES * 2);

    /* Allocate the output buffer */
    H_res = (int8_t *)malloc ((SB_CERT_RSA_KEY_SIZE_IN_BYTES * 2 + 2) * sizeof (int8_t));
    if (NULL == H_res) {
        UTIL_LOG_ERR("malloc H_res failed\n");
        goto calcH_end;
    }

    BN_set_word (bn_two, 2);
    BN_set_word (bn_twos, 2 * s);

    if (!BN_hex2bn(&bn_n, N_Temp)) {
        UTIL_LOG_ERR("BN_hex2bn failed.");
        goto calcH_end;
    }

    if (!BN_mod_exp(bn_h, bn_two, bn_twos, bn_n, bn_ctx)) {
        UTIL_LOG_ERR("BN_mod_exp failed\n");
        goto calcH_end;
    }

    H_resTemp = BN_bn2hex(bn_h);
    if (H_resTemp == NULL) {
        UTIL_LOG_ERR("BN_bn2hex failed\n");
        goto calcH_end;
    }

    if (H_resTemp[0] == '-'){ // in case the output is negative
        UTIL_LOG_ERR("BN_bn2hex returned negative value\n");
        goto calcH_end;
    }
    len = (int32_t)strlen (H_resTemp);
    memcpy(H_res, H_resTemp, len);

    if (len < SB_CERT_RSA_KEY_SIZE_IN_BYTES * 2) {
        memmove (H_res + (SB_CERT_RSA_KEY_SIZE_IN_BYTES * 2 - len), H_res, len + 1);
        for (i = 0; i < (int32_t)(SB_CERT_RSA_KEY_SIZE_IN_BYTES * 2 - len); i++) {
            H_res[i] = '0';
        }
    }

    /* Set the output with 0 and than copy the result */
    memset (H_ptr, 0, (SB_CERT_RSA_KEY_SIZE_IN_BYTES * 2 + 2)); //VERIFY THAT USER SENDS THE SAME SIZE
    memcpy ((int8_t *)H_ptr, (int8_t *)H_res, SB_CERT_RSA_KEY_SIZE_IN_BYTES * 2 + 2);

    status = 0;

    calcH_end:
    if (N_Temp != NULL) {
        free(N_Temp);
    }
    if (H_res != NULL) {
        free(H_res);
    }
    if (bn_two != NULL) {
        BN_free(bn_two);
    }
    if (bn_twos != NULL) {
        BN_free(bn_twos);
    }
    if (bn_n != NULL) {
        BN_free(bn_n);
    }
    if (bn_h != NULL) {
        BN_free(bn_h);
    }
    if (bn_ctx != NULL) {
        BN_CTX_free(bn_ctx);
    }
    if (H_resTemp != NULL) {
        OPENSSL_free(H_resTemp);
    }
    return(status);
}


/**
* @brief Reads RSA key from the file using passphrase and returns its decrypted value.
*
* @param[in] PemEncryptedFileName_ptr - file name
* @param[in] Key_ptr - passphrase
*/
/*********************************************************/
int32_t CC_CommonRsaLoadKey(int8_t *PemEncryptedFileName_ptr, int8_t *Key_ptr, int8_t *PemDecryted)
{
    RSA *rsa_pkey = NULL;
    int8_t buffer [256];
    BIO *out=NULL;
    int32_t status = -1;

    *PemDecryted = '\0';
    if (CC_CommonGetKeyPair (&rsa_pkey, PemEncryptedFileName_ptr, Key_ptr) < 0) {
        UTIL_LOG_ERR ("failed to CC_CommonGetKeyPair\n");
        goto rsaLoadKey_end;
    }

    out = BIO_new (BIO_s_mem());
    if (PEM_write_bio_RSAPrivateKey (out, rsa_pkey, NULL, NULL, 0, 0, NULL) <= 0) {
        UTIL_LOG_ERR ("failed to PEM_write_bio_RSAPrivateKey\n");
        goto rsaLoadKey_end;
    }

    while (BIO_gets (out, buffer, sizeof(buffer)) > 0) {
        if (strlen (PemDecryted) + strlen (buffer) + 2 > RSA_PRIVATE_KEY_SIZE) {
            UTIL_LOG_ERR ("Internal error: The output buffer is too few\n");
            break;
        }
        strncat (PemDecryted, buffer, strlen (buffer));
    }
    status = 0;

    rsaLoadKey_end:
    if (rsa_pkey != NULL) {
        RSA_free (rsa_pkey);
    }
    if (out != NULL) {
        BIO_free_all(out);
    }
    return status;
}

/**
* @brief Generates random byte buffer
*
* @param[in] numBytes - nuber of bytes to random
* @param[out] buf - buffer
*/
/*********************************************************/
int32_t CC_CommonRandBytes(int32_t numBytes, int8_t *buf)
{
    int32_t result = -1;

    if (numBytes > 0) {
        result = RAND_bytes (buf, numBytes);
        if (result <= 0) {
            UTIL_LOG_ERR("\nCC_CommonRandBytes - Internal error: Function RAND_bytes failed\n");
            return 1;
        }
        return 0;
    }
    return result;
}



