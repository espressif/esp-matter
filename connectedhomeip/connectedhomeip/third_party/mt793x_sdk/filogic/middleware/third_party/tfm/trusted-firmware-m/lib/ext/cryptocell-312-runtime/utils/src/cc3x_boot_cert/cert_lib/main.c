/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


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
#include "common_sb_ops.h"
#include "common_crypto_asym.h"
#include "common_util_log.h"
#include "common_util_files.h"
#include "cc_crypto_defs.h"
#include "cc_pka_hw_plat_defs.h"


#define HASH_ON_PLAIN_TEXT      0
#define HASH_ON_CIPHER_TEXT     1


/**
 * @brief The SBU_RSA_Sign generates RSA signature and returns it.
 *
 * The function follows the steps:
 * 1. Read RSA private key structure
 * 2. Call function according to PKCS version to create RSA signature
 *
 * @param[in] pkcsVersion - the version used (according to global definitions of available versions)
 * @param[in] DataIn_ptr - the data to sign on
 * @param[in] DataInSize - the data size
 * @param[in] PemEncryptedFileName_ptr - the private key file
 * @param[in] pwdFileName - file name of the password
 * @param[out] Signature_ptr - the RSA signature
 *
 */
 /*********************************************************/
SBUEXPORT_C int SBU_RSA_Sign(int pkcsVersion,
                             char* DataIn_ptr,
                             unsigned int DataInSize,
                             char* PemEncryptedFileName_ptr,
                             char* pwdFileName,
                             char* Signature_ptr)
{
    RSA  *pRsaPrivKey = NULL;
    unsigned char *pwd = NULL;
    int  ret_code;

    OpenSSL_add_all_algorithms ();

    /* parse the passphrase for a given file */
    if( strlen(pwdFileName) ) {
        if(CC_CommonGetPassphrase(pwdFileName, &pwd) != 0){
            printf("Failed to retrieve pwd\n");
            if (pwd != NULL)
                free(pwd);
            return (-1);
        }
    }
    else {
        pwd = Nullptr;
    }

    if (CC_CommonGetKeyPair (&pRsaPrivKey, PemEncryptedFileName_ptr, pwd) < 0)
    {
        printf ("\nCC_CommonGetKeyPair Cannot read RSA private key\n");
        return (-1);
    }

    if (pkcsVersion == RSA_USE_PKCS_15_VERSION) {
        ret_code = Sign_v15(pRsaPrivKey, DataIn_ptr, DataInSize, Signature_ptr, pwd);
    } else {
        ret_code = Sign_v21(pRsaPrivKey, DataIn_ptr, DataInSize, Signature_ptr);
    }

    return (ret_code);
}


/**
* @brief The SBU_AES_CTR_Encrypt encrypts (AES CTR) a given data and returns it.
* Also, it hash either the plain or the cipher text according to cryptoType.
*
* @param[in] pFileName - input plain text
* @param[in] pOutputFileName - output cipher text
* @param[in] Key_ptr - the AES key
* @param[in] KeySize - AES key size (must be one of the allowed AES key sizes)
* @param[in] IV_ptr - IV (AES IV size is constant)
* @param[in] Output_ptr - Output hash buffer
* @param[in] imageSize - size of input data
* @param[in] cryptoType - 0 - hash the plain text; 1 = hash the cipher text
*/
/*********************************************************/
SBUEXPORT_C int SBU_AES_CTR_EncryptFile(char *pFileName,
                    char *pOutputFileName,
                    char* Key_ptr, int KeySize,
                    char* IV_ptr,
                    char* Output_ptr,
                    uint32_t *imageSize,
                    int8_t cryptoType)
{
    int rc = 0;
    FILE *fd = NULL;
    FILE *encFd = NULL;
    int actualFileLen = 0;
    bool encFlag = false;
    uint32_t totalRead = 0;
    uint32_t actualRead = 0;
    unsigned char m_iv[AES_BLOCK_SIZE];
    unsigned char m_key[AES_BLOCK_SIZE*2];
    uint8_t origBuff[MAX_IMAGE_CHUNK] = {0};
    uint8_t encryptedBuff[MAX_IMAGE_CHUNK+AES_BLOCK_SIZE] = {0};
    uint32_t actualWriten = 0;
    uint8_t *pBuff2Hash = NULL;
    uint8_t imageHash[HASH_SHA256_DIGEST_SIZE_IN_BYTES] = {0};
    EVP_CIPHER_CTX ctrCtx;
    SHA256_CTX sha256Ctx;
    uint32_t encryptedBuffSize = 0;
    uint32_t prevBuffLen = 0;

    if (pFileName == NULL || Output_ptr == NULL || imageSize == NULL){
        printf("illegal parameters !\n");
        return 1;
    }
    if (IV_ptr != NULL){ /* if there is IV than key is required as well */
        memcpy (m_iv, IV_ptr, sizeof (m_iv));
        memcpy (m_key, Key_ptr, KeySize);
        encFlag = true;
    }

    /* Open image file for reading */
    fd = fopen(pFileName, "rb");
    if (NULL == fd) {
        printf( "failed to open file %s for reading\n", pFileName);
        return 1;
    }
    /* Get file length */
    fseek(fd, 0, SEEK_END);
    actualFileLen=ftell(fd);
    if (actualFileLen == -1){
        printf("ftell failed\n");
        goto END;
    }
    fseek(fd, 0, SEEK_SET);
    // validate size legal and word aligned
    if ((0 == actualFileLen) ||
        (actualFileLen % 0x4)) {
        printf( "ilegal actualFileLen == 0\n");
        rc = 3;
        goto END;
    }

    /* init SHA256 open-ssl context */
    rc = SHA256_Init(&sha256Ctx);
    if (rc == 0) {
        printf( "failed to SHA256_Init 0x%x\n", rc);
        rc = 1;
        goto END;
    }

    /* init variables in case encryption is needed */
    if (encFlag == true) {
        /* open encrypted file for writing encrypted image */
        encFd = fopen(pOutputFileName, "wb");
        if (NULL == encFd) {
            printf( "failed to open file %s for writing\n", pOutputFileName);
            rc = 1;
            goto END;
        }

        /* init AES CTR open-ssl context */
        EVP_CIPHER_CTX_init(&ctrCtx);
        rc = EVP_EncryptInit(&ctrCtx, EVP_aes_128_ctr(), m_key, m_iv);
        if (rc == 0) {
            printf( "failed to EVP_EncryptInit_ex 0x%x\n", rc);
            rc = 1;
            goto END;
        }
    }

    /* read max 1M bytes from image file,
      if encryption is required, encrype first and write into pOutputFileName;
      then perform SHA256. */
    totalRead = 0;
    prevBuffLen = 0;
    encryptedBuffSize = 0;

    while(totalRead < actualFileLen) {
        /* read file content */
        actualRead = fread(origBuff, 1, MAX_IMAGE_CHUNK, fd);
        if (actualRead == 0)
        {
            printf( "fread returned 0\n");
            goto END;
        }
        totalRead += actualRead;
        pBuff2Hash = origBuff;
        /* handle encryption */
        if (encFlag == true) {
            encryptedBuffSize = 0;
            pBuff2Hash = &encryptedBuff[0];
            rc = EVP_EncryptUpdate(&ctrCtx, encryptedBuff, &prevBuffLen, origBuff, actualRead);
            if (rc == 0) {
                printf( "failed to EVP_EncryptUpdate, rc 0x%x\n", rc);
                rc = 1;
                goto END;
            }
            /* update total encrypted byte count */
            encryptedBuffSize += prevBuffLen;
            if (totalRead >= actualFileLen) {
                    rc = EVP_EncryptFinal_ex(&ctrCtx, encryptedBuff+encryptedBuffSize, &prevBuffLen);
                if (rc == 0) {
                    printf( "failed to EVP_EncryptFinal_ex, rc 0x%x\n", rc);
                    rc = 1;
                    goto END;
                }
                encryptedBuffSize += prevBuffLen;
            }

            /* write encrypted data to binary file  */
            actualWriten = fwrite(pBuff2Hash, 1, actualRead, encFd);
            if (actualWriten != actualRead) {
                printf( "failed to write data to file actual written %d, expected %d\n", actualWriten, actualRead);
                rc = 1;
                goto END;
            }
        }

        /* calculate SHA256 on plain/cipher image */
        if (cryptoType == HASH_ON_PLAIN_TEXT) {
            rc = SHA256_Update(&sha256Ctx, origBuff, actualRead);
        } else {
            rc = SHA256_Update(&sha256Ctx, encryptedBuff, actualRead);
        }
        if (rc == 0) {
            printf( "failed to SHA256_Update, rc 0x%x\n", rc);
            rc = 1;
            goto END;
        }
        if (totalRead >= actualFileLen) {
            rc = SHA256_Final(imageHash, &sha256Ctx);
            if (rc == 0) {
                printf( "failed to SHA256_Final, rc 0x%x\n", rc);
                rc = 1;
                goto END;
            }
            OPENSSL_cleanse(&sha256Ctx,sizeof(sha256Ctx));
        }

    }
    // if we reached here rc is OK
    rc = 0;
    // copy the output HASH back to the caller
    memcpy(Output_ptr , imageHash, HASH_SHA256_DIGEST_SIZE_IN_BYTES);
    *imageSize = actualFileLen;

END:
    if (fd != NULL) {
        fclose(fd);
    }
    if (encFd != NULL) {
        fclose(encFd);
    }
    EVP_cleanup();

    return rc;

}


