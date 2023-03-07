/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <openssl/objects.h>
#include <openssl/pem.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/bn.h>
#include <openssl/aes.h>
#include <openssl/err.h>
#include <openssl/sha.h>
#include "common_crypto_sym.h"
#include "common_crypto_asym.h"
#include "common_crypto_encode.h"
#include "common_crypto_x509.h"
#include "common_rsa_keypair.h"
#include "common_rsa_keypair_util.h"
#include "common_util_files.h"
#include "common_util_log.h"
#include "cc_crypto_boot_defs.h"
#include "cc_crypto_x509_common_defs.h"
#include "cc_crypto_x509_defs.h"
#include "secdebug_defs.h"
#include "bootimagesverifier_def.h"
#include "cc_bitops.h"


#define NONCE_LENGTH 8
#define IV_LENGTH  AES_BLOCK_SIZE
#define SBU_TRUE  1
#define SBU_FALSE  0

static uint8_t *gpCertBuff = NULL;
static uint32_t gCertSize = 0;
static CCX509CertType_t  gCertType = 0;
static uint8_t  isLibOpened = 0;

#define NVCOUNTER_EXT_SIZE  28
#define NUM_OF_BITS_IN_BYTE 8
#define MAX_IMAGE_CHUNK (1024)


#define Nullptr (void *)0
#define SIZE_OF_DATA_FOR_DERIVATION 22

#define HASH_ON_PLAIN_TEXT      0
#define HASH_ON_CIPHER_TEXT     1

static CCX509CertHeaderParamsIn_t gCertHeaderParams={0};


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
* @brief internal function to reverse 64 bit word
*
* @param[in] original word
* @param[out] reversed word
*
*/
/*********************************************************/
static void SBU_x509_ReverseWord64Bit(uint64_t word, uint64_t *reversedWord)
{
    uint32_t i = 0;
    uint8_t maxInd = sizeof(uint64_t) - 1;
    uint8_t *tmpOrig = (uint8_t*)&word;
    uint8_t *tmpDst = (uint8_t*)reversedWord;

    for (i = 0; i < sizeof(uint64_t); i++ ){
        *(tmpDst + i) = *(tmpOrig + (maxInd-i));
    }

    return;
}

/**
* @brief initialize openSSL library
*
* @param[in] None
* @param[out] None
*
*/
/*********************************************************/
static void SBU_InitOpenSsl(void)
{
    if (0 == isLibOpened) {
          OpenSSL_add_all_algorithms();
    }
    isLibOpened++;
}


/**
* @brief terminates and cleanup openSSL library
*
* @param[in]  None
* @param[out] None
*
*/
/*********************************************************/
static void SBU_CloseOpenSsl(void)
{
    isLibOpened--;
    if (0 == isLibOpened) {
          EVP_cleanup();
          //CYPTO_cleanup_all_ex_data();  /* cleanup application specific data to avoid memory leaks.*/
    }
}


/**
* @brief clears statit buffer saing config params for certificate header
*
*/
/*********************************************************/
SBUEXPORT_C uint32_t SBU_x509_ClearHeaderParams()
{
    uint32_t rc = 0;

    memset(&gCertHeaderParams, 0, sizeof(gCertHeaderParams));
    return (rc);
}


/**
* @brief Set Issuer Name for Certificate Header
*
*/
/*********************************************************/
SBUEXPORT_C uint32_t SBU_x509_SetHeaderParam_IssuerName(int8_t *pIssuerName)
{
    uint32_t rc = 0;

    if ((pIssuerName == NULL) || (strlen(pIssuerName) <= 0)){
        rc =1;
        return rc;
    }

    if (strlen(pIssuerName) > sizeof(gCertHeaderParams.IssuerName)-1){
        rc =1;
        return rc;
    }

    gCertHeaderParams.setIssuerName = SBU_TRUE;
    strcpy(gCertHeaderParams.IssuerName, pIssuerName);

    return (rc);
}


/**
* @brief Set Subject Name for Certificate Header
*
*/
/*********************************************************/
SBUEXPORT_C uint32_t SBU_x509_SetHeaderParam_SubjectName(int8_t *pSubjectName)
{
    uint32_t rc = 0;

    if ((pSubjectName == NULL) || (strlen(pSubjectName) <= 0)){
        rc =1;
        return rc;
    }

    if (strlen(pSubjectName) > sizeof(gCertHeaderParams.SubjectName)-1){
        rc =1;
        return rc;
    }

    gCertHeaderParams.setSubjectName = SBU_TRUE;
    strcpy(gCertHeaderParams.SubjectName, pSubjectName);

    return (rc);
}

/**
* @brief Set Subject Name for Certificate Header
*
*/
/*********************************************************/
SBUEXPORT_C uint32_t SBU_x509_SetHeaderParam_NotBefore(long NotBefore)
{
    uint32_t rc = 0;

    gCertHeaderParams.setNotBefore= SBU_TRUE;
    gCertHeaderParams.notBefore= NotBefore;

    return (rc);
}

/**
* @brief Set Subject Name for Certificate Header
*
*/
/*********************************************************/
SBUEXPORT_C uint32_t SBU_x509_SetHeaderParam_NotAfter(long NotAfter)
{
    uint32_t rc = 0;

    gCertHeaderParams.setNotAfter= SBU_TRUE;
    gCertHeaderParams.notAfter= NotAfter;

    return (rc);
}

/**
* @brief Set Subject Name for Certificate Header
*
*/
/*********************************************************/
SBUEXPORT_C uint32_t SBU_x509_SetHeaderParam_SerialNumber(uint32_t SerialNumber)
{
    uint32_t rc = 0;

    gCertHeaderParams.setSerialNum= SBU_TRUE;
    gCertHeaderParams.serialNum= SerialNumber;

    return (rc);
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

    return rc;

}







/**
* @brief allocate x509 certifiacte and set its header
*
* @param[in] certType - certifiacte type
*/
/*********************************************************/
SBUEXPORT_C uint32_t SBU_x509_CreateAndSetHeader(CCX509CertType_t  certType)
{
    uint32_t rc = 0;

    SBU_InitOpenSsl();
    rc = CC_CommonX509CreateAndSetHeader(&gpCertBuff, certType,&gCertHeaderParams);
    if ((rc != 0) ||
        (NULL == gpCertBuff)) {
        UTIL_LOG_ERR("failed to CC_CommonX509CreateAndSetHeader \n");
        goto END;
    }
    /* save certificate in global variable */
    gCertType = certType;
    rc = 0;

    END:
    if (rc != 0) {
        SBU_CloseOpenSsl();
    }
    return (rc);
}


/**
* @brief calculate Np from key pair file and add it to x509 certificate extension
*
* @param[in] pKeyPairFileName - key pair PEM format file name
* @param[in] pwdFileName - key pair passphrase
*/
/*********************************************************/
SBUEXPORT_C uint32_t SBU_x509_AddPubKeyNpBuffExtension(int8_t *pKeyPairFileName, int8_t *pwdFileName)
{
    uint32_t rc = 0;
    DxRsaKeyNandNp_t pNAndNp = {0};
    uint32_t pNAndNpSize = sizeof(pNAndNp);

    /* validate inputs */
    if ((NULL == gpCertBuff) ||
        (NULL == pKeyPairFileName)) {
        UTIL_LOG_ERR("invalid input\n");
        rc = 1;
        goto END;
    }
    /* calculate N and Np */
    rc =  CC_CommonGetNAndNpFromKeyPair(pKeyPairFileName, pwdFileName, (uint8_t *)&pNAndNp, &pNAndNpSize);
    if (rc != 0) {
        UTIL_LOG_ERR("failed to CC_CommonGetNAndNpFromKeyPair %s pwd %s\n", pKeyPairFileName, pwdFileName);
        goto END;
    }
    UTIL_LOG_BYTE_BUFF("pNAndNp", (uint8_t *)pNAndNp.pNpBuff, NP_SIZE_IN_BYTES);
    /* Add Np to the cerifiacte extension */
    rc = CC_CommonX509AddStringExtension(gpCertBuff, gCertType, CC_X509_ID_EXT_PUB_KEY_NP, pNAndNp.pNpBuff, NP_SIZE_IN_BYTES);
    if (rc != 0) {
        UTIL_LOG_ERR("failed to CC_CommonX509AddStringExtension \n");
        goto END;
    }
    rc = 0;

    END:
    if (rc != 0) {
        if (gpCertBuff != NULL) {
            CC_CommonX509Free(&gpCertBuff);
        }
        SBU_CloseOpenSsl();
    }
    return rc;
}



/**
* @brief Add proprietary header as X509 extension
*
* @param[in] pHeaderInfo - proprietary header
*/
/*********************************************************/
SBUEXPORT_C uint32_t SBU_x509_AddPropHeaderExtension(uint8_t *pHeaderInfo)
{
    uint32_t rc = 0;

    /* validate inputs */
    if ((gpCertBuff == NULL) ||
        (pHeaderInfo == NULL)) {
        UTIL_LOG_ERR("invalid input\n");
        rc = 1;
        goto END;
    }
    UTIL_LOG_BYTE_BUFF("pHeaderInfo", pHeaderInfo, sizeof(CCSbCertHeader_t));
    rc = CC_CommonX509AddStringExtension(gpCertBuff, gCertType, CC_X509_ID_EXT_PROPRIETARY_HEADER, pHeaderInfo, sizeof(CCSbCertHeader_t));
    if (rc != 0) {
        UTIL_LOG_ERR("failed to CC_CommonX509AddStringExtension \n");
        goto END;
    }
    return rc;
    END:
    if (rc != 0) {
        if (gpCertBuff != NULL) {
            CC_CommonX509Free(&gpCertBuff);
        }
        SBU_CloseOpenSsl();
    }
    return rc;
}
/**
* @brief Add key certificate body to X509 extension
*
* @param[in] nvCounter - SW version of the certificate, must be at least as defined in OTP
* @param[in] pPubKeyFileName - Next certificate public key file name
*/
/*********************************************************/
SBUEXPORT_C uint32_t SBU_x509_AddKeyCertBodyExtension(uint32_t nvCounter,
        uint8_t *pPubKeyFileName)
{
    uint32_t rc = 0;
    KeyCertMain_t keyCertBody;

    /* validate inputs */
    if ((gpCertBuff == NULL) ||
        (pPubKeyFileName == NULL)) {
        UTIL_LOG_ERR("invalid input\n");
        rc = 1;
        goto END;
    }

    /* calculate SHA256 on public key file*/
    rc = CC_CommonCalcHBKFromFile(pPubKeyFileName, keyCertBody.nextPubKeyHash, HASH_SHA256_DIGEST_SIZE_IN_BYTES);
    if (rc != 0) {
        UTIL_LOG_ERR("failed to CC_CommonCalcHBKFromFile \n");
        goto END;
    }
    keyCertBody.swVer = nvCounter;

    rc = CC_CommonX509AddStringExtension(gpCertBuff, gCertType, CC_X509_ID_EXT_KEY_CERT_MAIN_VAL, (uint8_t *)&keyCertBody, sizeof(KeyCertMain_t));
    if (rc != 0) {
        UTIL_LOG_ERR("failed to CC_CommonX509AddStringExtension \n");
        goto END;
    }
    rc = 0;

    END:
    if (rc != 0) {
        if (gpCertBuff != NULL) {
            CC_CommonX509Free(&gpCertBuff);
        }
        SBU_CloseOpenSsl();
    }
    return rc;
}




/**
* @brief Add content certificate body to X509 extension
*
* @param[in] nvCounter - SW version of the certificate, must be at least as defined in OTP
* @param[in] pNonce - Key nonce - 8 bytes
* @param[in] pImageList - images data list
* @param[in] imageListSize -size of pImageList
*/
/*********************************************************/
SBUEXPORT_C uint32_t SBU_x509_AddContentCertBodyExtension(uint32_t nvCounter,
        uint8_t *pNonce,
        uint8_t *pImageList,
        uint32_t numOfImages)
{
    uint32_t rc = 0;
    ContentCertMain_t certBody;
    uint32_t    certBodySize;

    /* validate inputs */
    if ((gpCertBuff == NULL) ||
        (pImageList == NULL) ||
        (numOfImages == 0) || (numOfImages > CC_SB_MAX_NUM_OF_IMAGES) ||
        (pNonce == NULL)) {
        UTIL_LOG_ERR("invalid input\n");
        rc = 1;
        goto END;
    }

    certBody.swVer = nvCounter;
    memcpy((uint8_t *)certBody.nonce, pNonce, sizeof(CCSbNonce_t));
    memcpy((uint8_t *)certBody.imageRec, pImageList, sizeof(ContentCertImageRecord_t)*numOfImages);
    certBodySize = sizeof(ContentCertMain_t) - (sizeof(ContentCertImageRecord_t)*(CC_SB_MAX_NUM_OF_IMAGES - numOfImages));
    rc = CC_CommonX509AddStringExtension(gpCertBuff, gCertType, CC_X509_ID_EXT_CONTENT_CERT_MAIN_VAL, (uint8_t *)&certBody, certBodySize);
    if (rc != 0) {
        UTIL_LOG_ERR("failed to CC_CommonX509AddStringExtension \n");
        goto END;
    }

    END:
    if (rc != 0) {
        if (gpCertBuff != NULL) {
            CC_CommonX509Free(&gpCertBuff);
        }
        SBU_CloseOpenSsl();
    }
    return rc;
}



/**
* @brief Add Enabler certificate body to X509 extension
*
* @param[in] pDebugMask - Enabler certificate debug mask
* @param[in] pDebugLock - Enabler certificate debug lock
* @param[in] pPubKeyFileName - Developer certificate public key file name
*/
/*********************************************************/
SBUEXPORT_C uint32_t SBU_x509_AddEnablerCertBodyExtension(uint8_t *pDebugMask,
        uint8_t *pDebugLock,
        uint8_t *pPubKeyFileName)
{
    uint32_t rc = 0;
    EnablerCertMain_t enablerCertBody;

    /* validate inputs */
    if ((gpCertBuff == NULL) ||
        (pDebugMask == NULL) ||
        (pDebugLock == NULL) ||
        (pPubKeyFileName == NULL)) {
        UTIL_LOG_ERR("invalid input\n");
        rc = 1;
        goto END;
    }

    /* calculate SHA256 on public key file*/
    rc = CC_CommonCalcHBKFromFile(pPubKeyFileName, enablerCertBody.nextPubKeyHash, HASH_SHA256_DIGEST_SIZE_IN_BYTES);
    if (rc != 0) {
        UTIL_LOG_ERR("failed to CC_CommonCalcHBKFromFile \n");
        goto END;
    }
    memcpy((uint8_t *)enablerCertBody.debugMask, pDebugMask, CC_BSV_SEC_DEBUG_DCU_SIZE_IN_BYTES);
    memcpy((uint8_t *)enablerCertBody.debugLock, pDebugLock, CC_BSV_SEC_DEBUG_DCU_SIZE_IN_BYTES);

    rc = CC_CommonX509AddStringExtension(gpCertBuff, gCertType, CC_X509_ID_EXT_ENABLER_CERT_MAIN_VAL, (uint8_t *)&enablerCertBody, sizeof(EnablerCertMain_t));
    if (rc != 0) {
        UTIL_LOG_ERR("failed to CC_CommonX509AddStringExtension \n");
        goto END;
    }
    rc = 0;

    END:
    if (rc != 0) {
        if (gpCertBuff != NULL) {
            CC_CommonX509Free(&gpCertBuff);
        }
        SBU_CloseOpenSsl();
    }
    return rc;
}


/**
* @brief Add Developer certificate body to X509 extension
*
* @param[in] pDebugMask - Developer certificate debug mask
* @param[in] pSocId - Developer certificate SoC ID
**/
/*********************************************************/
SBUEXPORT_C uint32_t SBU_x509_AddDeveloperCertBodyExtension(uint8_t *pDebugMask,
        uint8_t *pSocId)
{
    uint32_t rc = 0;
    DeveloperCertMain_t developerCertBody;

    /* validate inputs */
    if ((gpCertBuff == NULL) ||
        (pDebugMask == NULL) ||
        (pSocId == NULL)) {
        UTIL_LOG_ERR("invalid input\n");
        rc = 1;
        goto END;
    }

    memcpy((uint8_t *)developerCertBody.debugMask, pDebugMask, CC_BSV_SEC_DEBUG_DCU_SIZE_IN_BYTES);
    memcpy((uint8_t *)developerCertBody.socId, pSocId, HASH_SHA256_DIGEST_SIZE_IN_BYTES);

    rc = CC_CommonX509AddStringExtension(gpCertBuff, gCertType, CC_X509_ID_EXT_DEVELOPER_CERT_MAIN_VAL, (uint8_t *)&developerCertBody, sizeof(DeveloperCertMain_t));
    if (rc != 0) {
        UTIL_LOG_ERR("failed to CC_CommonX509AddStringExtension \n");
        goto END;
    }
    rc = 0;

    END:
    if (rc != 0) {
        if (gpCertBuff != NULL) {
            CC_CommonX509Free(&gpCertBuff);
        }
        SBU_CloseOpenSsl();
    }
    return rc;
}


/**
* @brief Add subject public key to the X509 certificate
*   and sign the certificate
*
* @param[in] pKeyPairFileName   - key pair file name in PEM format
* @param[in] pKeyPairPwd    - passphrase of key pair
*/
/*********************************************************/
SBUEXPORT_C uint32_t SBU_x509_SetKeyAndSign(int8_t *pKeyPairFileName,
                     int8_t *pPrivKeyPwd)
{
    uint32_t rc = 0;

    /* verify inputs */
    if ((NULL == gpCertBuff) ||
        (NULL == pKeyPairFileName)) {
        UTIL_LOG_ERR("invalid input\n");
        rc = 1;
        goto END;
    }
    UTIL_LOG_INFO("calling CC_CommonX509SetKeyAndSign pKeyPairFileName %s, pPrivKeyPwd %s\n", pKeyPairFileName, pPrivKeyPwd);
    rc = CC_CommonX509SetKeyAndSign(gpCertBuff, pKeyPairFileName, pPrivKeyPwd);
    if (rc != 0) {
        UTIL_LOG_ERR("failed to CC_CommonX509SetKeyAndSign \n");
        goto END;
    }

    rc = CC_CommonX509ToDer(&gpCertBuff, &gCertSize);
    if (rc != 0) {
        UTIL_LOG_ERR("failed to CC_CommonX509ToDer \n");
        goto END;
    }
    rc = 0;

    END:
    if (rc != 0) {
        SBU_CloseOpenSsl();
    }
    return rc;
}


/**
* @brief build package for the certificate
*
* @param[in] pAddData       - additional data to add to package
* @param[in] addDataSize        - length of additional data
* @param[out] outPkgFile        - package file name to write the package to
*/
/*********************************************************/
SBUEXPORT_C uint32_t SBU_x509_BuildCertPkg(uint8_t *pAddData,
                    uint32_t addDataSize,
                    uint32_t  isAddDataBefore,
                    uint8_t *outPkgFile)
{
    uint32_t rc = 0;
    FILE *fp = NULL;
    uint8_t *pCertPkg = NULL;
    uint32_t pkgBytesSize = 0;
    uint32_t pkgLocation = 0;

    /* verify inputs */
    if ((NULL == gpCertBuff) ||
        (NULL == outPkgFile) ||
        ((pAddData != NULL) && (addDataSize == 0)) ||
        ((pAddData == NULL) && (addDataSize != 0))) {
        UTIL_LOG_ERR("invalid input\n");
        rc = 1;
        goto END;
    }

    UTIL_LOG_INFO("outPkgFile %s, addDataSize %d, gCertSize %d\n", outPkgFile, addDataSize, gCertSize);
    UTIL_LOG_BYTE_BUFF("pAddData", pAddData, addDataSize);
    // Offset within package must be aligned
    if (isAddDataBefore == 1) {
        addDataSize = ALIGN_TO_4BYTES(addDataSize);
    } else {
        gCertSize = ALIGN_TO_4BYTES(gCertSize);
    }

    /* Calculate package size */
    pkgBytesSize = gCertSize + addDataSize;

    UTIL_LOG_INFO("Opening certificate pkg file for writing\n");
    fp = fopen(outPkgFile, "w");
    if (NULL == fp) {
        UTIL_LOG_ERR("failed to open %s\n", outPkgFile);
        rc = (-1);
        goto END;
    }

    /* create the package buffer */
    pCertPkg = (uint8_t *) malloc(pkgBytesSize);
    if (pCertPkg == NULL){
        UTIL_LOG_ERR("failed to allocate pkg\n");
        rc = (-1);
        goto END;
    }
    pkgLocation = 0;
    if ((pAddData != NULL) &&
        (isAddDataBefore == 1)){
        memcpy(&pCertPkg[pkgLocation], pAddData, addDataSize);
        pkgLocation += addDataSize;
    }
      /* copy certificate PEM  to package */
    memcpy(&pCertPkg[pkgLocation], gpCertBuff, gCertSize);
    pkgLocation += gCertSize;
    /* copy additional data to package */
    if ((pAddData != NULL) &&
        (isAddDataBefore == 0)){
        memcpy(&pCertPkg[pkgLocation], pAddData, addDataSize);
        pkgLocation += addDataSize;
    }
    if (pkgLocation != (pkgBytesSize)) {
        UTIL_LOG_ERR("Invalid pkgLocation %d, addDataSize %d, gCertSize %d\n", pkgLocation, addDataSize, gCertSize);
        rc = 1;
        goto END;
    }

    /* write out the package in binary format  */
    UTIL_LOG_INFO("writing pkg to file\n");
    rc = CC_CommonUtilCopyBuffToBinFile(outPkgFile, pCertPkg, pkgBytesSize);
    if (rc != 0) {
        UTIL_LOG_ERR("failed to CC_CommonUtilCopyBuffToBinFile\n");
        rc = 1;
        goto END;
    }
    rc = 0;
    UTIL_LOG_INFO("OK\n");

END:
    if (fp != NULL){
        fclose(fp);
    }
    if (pCertPkg != NULL) {
        free(pCertPkg);
    }
    if (gpCertBuff != NULL) {
        free(gpCertBuff);
    }
    SBU_CloseOpenSsl();
    return rc;



}

/**
* @brief Reads RSA public key from the file and returns its raw value and its Np
*
* @param[in] pPemFileName_ptr - file name
* @param[out] pNAndNp - N and Np buffer
*/
/*********************************************************/
SBUEXPORT_C int SBU_x509_GetHashOfNAndNpFromPubKey(char* pPemFileName_ptr, char *pHash, int hashSize)
{
    int rc = 0;

    SBU_InitOpenSsl();

    rc = CC_CommonCalcHBKFromFile(pPemFileName_ptr, pHash, hashSize);
    if (rc != 0) {
        printf( "failed to CC_CommonCalcHBKFromFile %d or ilegal size %d\n", rc, hashSize);
    }

    SBU_CloseOpenSsl();  /* cleanup application specific data to avoid memory leaks.*/

    return rc;
}
