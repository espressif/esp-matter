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
#include "cc_production_asset.h"

static uint8_t  isLibOpened = 0;

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
                uint8_t *pLabel, uint32_t labelSize,
                uint8_t *pContext, uint32_t contextSize,
                char *pOutKey, uint32_t outKeySize)
{
        #define MAX_DATA_IN_SIZE  (PROD_KEY_TMP_LABEL_SIZE + PROD_KEY_TMP_CONTEXT_SIZE + 3)  // +3 for: iteration, key size and 0x0
    int rc = 0;
    int index = 0;
    int8_t dataIn[MAX_DATA_IN_SIZE] = {0x0};

        if ((pKey == NULL) ||
            (keySize != PROD_KEY_TMP_KEY_SIZE) ||
            (pLabel == NULL) ||
            (labelSize > PROD_KEY_TMP_LABEL_SIZE) ||
            (pContext == NULL) ||
            (contextSize > PROD_KEY_TMP_CONTEXT_SIZE) ||
            (pOutKey == NULL) ||
            (outKeySize != PROD_KEY_TMP_KEY_SIZE)) {
        UTIL_LOG_ERR( "Invalid inputs\n");
        return (-1);
        }

        /* Create the input to the CMAC derivation
           since key size is 16 bytes, we have 1 iteration for cmac  derivation*
           the data or the derivation:
           0x1 || label || 0x0 || context || size of derived key in bits */
    dataIn[index++] = 0x1;
    memcpy(&dataIn[index], pLabel, labelSize);
    index += labelSize;
    dataIn[index++] = 0x0;
    memcpy(&dataIn[index], pContext, contextSize);
    index += contextSize;
    dataIn[index++] = outKeySize*CC_BITS_IN_BYTE; // size of the key in bits

    UTIL_LOG_BYTE_BUFF("dataIn", dataIn, index);
    UTIL_LOG_BYTE_BUFF("pKey", pKey, keySize);
    rc = CC_CommonAesCmacEncrypt(dataIn, index,
                     pKey, keySize, pOutKey);
    if (rc != 0) {
        UTIL_LOG_ERR( "failed to CC_CommonAesCmacEncrypt(), rc %d\n", rc);
        return (-1);
    }
    UTIL_LOG_BYTE_BUFF("pOutKey", pOutKey, outKeySize);
    return rc;
}


/**
* @brief Build the ICV asset package using openSSL library
*
* @param[in]  encKeyBuff & encKeyBuffSize & pKeyPwdFileName - the encryptes Kpicv key
*           assetId - Asset ID, used for Kprov derivation
*           asset & assetSize - The Asset to generate the package for
*               pPkgFileName - OUT - the asset package binary
*               file name
* @param[out] None
*
*/
/*********************************************************/
int build_asset_pkg(char *encKeyBuff, uint32_t encKeyBuffSize,
    char *pKeyPwdFileName,
        uint32_t assetType,
    uint8_t *pUserData, uint32_t userDataSize,
        uint8_t *asset, uint32_t assetSize,
    char *pPkgFileName)
{
    int rc = 0;
    uint8_t keyRtl[PROD_KEY_RTL_KEY_SIZE + AES_BLOCK_SIZE] = {0};  // Adding AES_BLOCK_SIZE to avoid over-run
    uint8_t keyTmpLabel[PROD_KEY_TMP_LABEL_SIZE] = {0};
    uint8_t keyTmp[PROD_KEY_TMP_KEY_SIZE] = {0};
    uint8_t keyProv[PROD_KPROV_KEY_SIZE] = {0};
    uint8_t keyProvContext[PROD_KPROV_CONTEXT_SIZE] = {0};
    uint8_t i = 0;
    CCProdAssetPkg_t  assetPackage = {0};
    uint32_t assetPkgSize = sizeof(CCProdAssetPkg_t);
        uint8_t label[] = PROD_LABEL;

    // Verify Inputs
    UTIL_LOG_ERR( "encKeyBuffSize %d\n", encKeyBuffSize);
    if ((encKeyBuff == NULL) ||
        (encKeyBuffSize != (PROD_KEY_RTL_KEY_SIZE+ AES_BLOCK_SIZE)) ||
        ((assetType != PROD_ASSET_TYPE_KCE)  && (assetType != PROD_ASSET_TYPE_KCP))||
        (pUserData == NULL) ||
        (userDataSize != PROD_KEY_TMP_CONTEXT_SIZE) ||
        (asset == NULL) ||
        (assetSize != PROD_ASSET_SIZE) ||
        (pPkgFileName == NULL)) {
        UTIL_LOG_ERR( "Invalid inputs\n");
        return (-1);
    }

    InitOpenSsl();

    // Build the package header
    assetPackage.token = PROD_ASSET_PROV_TOKEN;
    assetPackage.version = PROD_ASSET_PROV_VERSION;
    assetPackage.assetSize = assetSize;
        assetPackage.reserved[0] = PROD_ASSET_RESERVED1_VAL;
        assetPackage.reserved[1] = PROD_ASSET_RESERVED2_VAL;
        rc = CC_CommonRandBytes(PROD_ASSET_NONCE_SIZE, (uint8_t *)assetPackage.nonce);
    if (rc != 0) {
        UTIL_LOG_ERR( "failed to CC_CommonRandBytes() for nonce, rc %d\n", rc);
        rc = (-1);
        goto end_func;
    }


    // Decrypt Krtl
    rc = CC_CommonAesCbcDecrypt(pKeyPwdFileName, encKeyBuff, encKeyBuffSize, keyRtl);
    if (rc != 0) {
        UTIL_LOG_ERR( "failed to CC_CommonAesCbcDecrypt() for Krtl, rc %d\n", rc);
        rc = (-1);
        goto end_func;
    }


        memcpy(keyTmpLabel, PROD_ICV_KEY_TMP_LABEL, PROD_KEY_TMP_LABEL_SIZE);
        if (assetType == PROD_ASSET_TYPE_KCE) {
                memcpy(keyProvContext, PROD_ICV_ENC_CONTEXT, PROD_KPROV_CONTEXT_SIZE);
        } else { // PROD_ASSET_TYPE_KCP
                memcpy(keyProvContext, PROD_ICV_PROV_CONTEXT, PROD_KPROV_CONTEXT_SIZE);
        }

        // Calculate Ktmp = cmac(Krtl, 0x01 || ICV/OEM_label  || 0x0 || user context || 0x80)
    rc = AesCmacKeyDerivation(keyRtl, PROD_KEY_RTL_KEY_SIZE,
                keyTmpLabel,  PROD_KEY_TMP_LABEL_SIZE,
                (uint8_t *)pUserData, PROD_KEY_TMP_CONTEXT_SIZE,
                keyTmp, PROD_KEY_TMP_KEY_SIZE);
    if (rc != 0) {
        UTIL_LOG_ERR( "failed to AesCmacKeyDerivation() for Ktmp, rc %d\n", rc);
        rc = (-1);
        goto end_func;
    }

        // Calculate Kprov= cmac(Ktmp, 0x01 || "P"  || 0x0 || asset_id || 0x80)
    rc = AesCmacKeyDerivation(keyTmp, PROD_KEY_TMP_KEY_SIZE,
                label,  PROD_KPROV_LABEL_SIZE,
                keyProvContext, PROD_KPROV_CONTEXT_SIZE,
                keyProv, PROD_KPROV_KEY_SIZE);
    if (rc != 0) {
        UTIL_LOG_ERR( "failed to AesCmacKeyDerivation() for Ktmp, rc %d\n", rc);
        rc = (-1);
        goto end_func;
    }

    // Encrypt and authenticate the asset
    rc = CC_CommonAesCcmEncrypt(keyProv,
                (uint8_t *)assetPackage.nonce, PROD_ASSET_NONCE_SIZE,
                (uint8_t *)&assetPackage, PROD_ASSET_ADATA_SIZE,
                asset, assetSize,
                (uint8_t *)assetPackage.encAsset, &assetSize,
                                ((uint8_t *)assetPackage.encAsset)+assetSize, PROD_ASSET_TAG_SIZE);
    if (rc != 0) {
        UTIL_LOG_ERR( "failed to CC_CommonAesCmacEncrypt() for Kprov, rc %d\n", rc);
        rc = (-1);
        goto end_func;
    }

    // Writing the asset package into bin file
    rc = CC_CommonUtilCopyBuffToBinFile(pPkgFileName, (uint8_t *)&assetPackage, assetPkgSize);
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
