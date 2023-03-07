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
#include "common_util_log.h"
#include "common_crypto_asym.h"
#include "common_crypto_sym.h"
#include "common_rsa_keypair_util.h"
#include "common_util_files.h"
#include "cc_util_asset_prov_int.h"

static uint8_t  isLibOpened = 0;

#define KPICV_KEY_SIZE      16
#define KPROV_KEY_SIZE      16
#define KPROV_DATA_IN_SIZE  8


/**
* @brief initialize openSSL library
*
* @param[in] None
* @param[out] None
*
*/
/*********************************************************/
static void InitOpenSsl(void)
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
static void CloseOpenSsl(void)
{
    isLibOpened--;
    if (0 == isLibOpened) {
          EVP_cleanup();
          //CYPTO_cleanup_all_ex_data();  /* cleanup application specific data to avoid memory leaks.*/
    }
}


/**
* @brief performs CMAC key derivation for Kprov using openSSL library
*
* @param[in]  pKey & keySize - Kpicv key and its size
*       lable & pContext & contextSize used to build the dataIn for derivation
* @param[out] pOutKey - Kprov
*
*/
/*********************************************************/
static int AesCmacKeyDerivation(char *pKey, uint32_t keySize,
                char lable,
                uint8_t *pContext, uint32_t contextSize,
                char *pOutKey, uint32_t outKeySize)
{
    int rc = 0;
    int i = 0;
    int8_t dataIn[KPROV_DATA_IN_SIZE] = {0x0};

    /* Create the input to the CMAC derivation */
    dataIn[i++] = 0x1;
    dataIn[i++] = lable;
    dataIn[i++] = 0x0;
    memcpy(&dataIn[i], pContext, contextSize);
    i += contextSize;
    dataIn[i] = outKeySize*CC_BITS_IN_BYTE; // size of the key in bits

    UTIL_LOG_BYTE_BUFF("dataIn", dataIn, sizeof(dataIn));
    UTIL_LOG_BYTE_BUFF("pKey", pKey, keySize);
    rc = CC_CommonAesCmacEncrypt(dataIn, sizeof(dataIn),
                     pKey, keySize, pOutKey);
    if (rc != 0) {
        UTIL_LOG_ERR( "failed to CC_CommonAesCmacEncrypt(), rc %d\n", rc);
        return (-1);
    }
    UTIL_LOG_BYTE_BUFF("pOutKey", pOutKey, outKeySize);
    return rc;
}


/**
* @brief Build the ICV asset BLOB using openSSL library
*
* @param[in]  encKeyBuff & encKeyBuffSize & pKeyPwdFileName - the encryptes Kpicv key
*           assetId - Asset ID, used for Kprov derivation
*           asset & assetSize - The Asset to generate the BLOB for
*           pBlobFileName - OUT - the asset BLOB binary file name
* @param[out] None
*
*/
/*********************************************************/
int build_asset_blob(char *encKeyBuff, uint32_t encKeyBuffSize,
    char *pKeyPwdFileName,
    uint32_t assetId,
    uint8_t *asset, uint32_t assetSize,
    char *pBlobFileName)
{
    int rc = 0;
    uint8_t keyPicv[(KPICV_KEY_SIZE + CC_ASSET_PROV_BLOCK_SIZE)] = {0};
    uint8_t keyProv[KPROV_KEY_SIZE] = {0};
    uint8_t i = 0;
    CCAssetProvPkg_t assetBlob = {0};
    uint32_t assetBlobSize;

    // Verify Inputs
    UTIL_LOG_ERR( "encKeyBuffSize %d\n", encKeyBuffSize);
    if ((encKeyBuff == NULL) ||
        (encKeyBuffSize != (KPICV_KEY_SIZE + CC_ASSET_PROV_BLOCK_SIZE)) ||
        (asset == NULL) ||
        (assetSize > CC_ASSET_PROV_MAX_ASSET_SIZE) || (assetSize % CC_ASSET_PROV_BLOCK_SIZE) ||
        (pBlobFileName == NULL)) {
        UTIL_LOG_ERR( "Invalid inputs\n");
        return (-1);
    }
        assetBlobSize = CC_ASSET_PROV_ADATA_SIZE + CC_ASSET_PROV_NONCE_SIZE + assetSize + CC_ASSET_PROV_TAG_SIZE;

    InitOpenSsl();

    // Build the BLOB header
    assetBlob.token = CC_ASSET_PROV_TOKEN;
    assetBlob.version = CC_ASSET_PROV_VERSION;
    assetBlob.assetSize = assetSize;
    assetBlob.reserved[0] = 0;
    assetBlob.reserved[1] = 0; // reserved
    rc = CC_CommonRandBytes(CC_ASSET_PROV_NONCE_SIZE, (uint8_t *)assetBlob.nonce);
    if (rc != 0) {
        UTIL_LOG_ERR( "failed to CC_CommonRandBytes() for nonce, rc %d\n", rc);
        rc = (-1);
        goto end_func;
    }


    // Decrypt Kpicv
    UTIL_LOG_ERR( "calling CC_CommonAesCbcDecrypt() pKeyPwdFileName %s\n", pKeyPwdFileName);
    rc = CC_CommonAesCbcDecrypt(pKeyPwdFileName, encKeyBuff, encKeyBuffSize, keyPicv);
    if (rc != 0) {
        UTIL_LOG_ERR( "failed to CC_CommonAesCbcDecrypt() for Kpicv, rc %d\n", rc);
        rc = (-1);
        goto end_func;
    }


        // Calculate Kprov = cmac(Kpicv, 0x01 || 0x50 || 0x00 || asset id || 0x80)
    UTIL_LOG_ERR( "calling AesCmacKeyDerivation\n");
    rc = AesCmacKeyDerivation(keyPicv, KPICV_KEY_SIZE,
                'P',
                (uint8_t *)&assetId, sizeof(assetId),
                keyProv, sizeof(keyProv));
    if (rc != 0) {
        UTIL_LOG_ERR( "failed to AesCmacKeyDerivation() for Kprov, rc %d\n", rc);
        rc = (-1);
        goto end_func;
    }

    // Encrypt and authenticate the asset
    UTIL_LOG_ERR( "calling CC_CommonAesCcmEncrypt\n");
    rc = CC_CommonAesCcmEncrypt(keyProv,
                (uint8_t *)assetBlob.nonce, CC_ASSET_PROV_NONCE_SIZE,
                (uint8_t *)&assetBlob, CC_ASSET_PROV_ADATA_SIZE,
                asset, assetSize,
                (uint8_t *)assetBlob.encAsset, &assetSize,
                                ((uint8_t *)assetBlob.encAsset)+assetSize, CC_ASSET_PROV_TAG_SIZE);
    if (rc != 0) {
        UTIL_LOG_ERR( "failed to CC_CommonAesCmacEncrypt() for Kprov, rc %d\n", rc);
        rc = (-1);
        goto end_func;
    }

    UTIL_LOG_BYTE_BUFF("assetBlob", (uint8_t *)&assetBlob, assetBlobSize);
    // Writing the asset BLOB into bin file
    rc = CC_CommonUtilCopyBuffToBinFile(pBlobFileName, (uint8_t *)&assetBlob, assetBlobSize);
    if (rc != 0) {
        UTIL_LOG_ERR( "failed to CC_CommonUtilCopyBuffToBinFile(), rc %d\n", rc);
        rc = (-1);
        goto end_func;
    }
    rc = 0;
end_func:
    CloseOpenSsl();
    UTIL_LOG_ERR( "End rc %d\n", rc);
    return rc;
}
