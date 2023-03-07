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
#include "dmpu_utils.h"
#include "cc_production_asset.h"


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
int build_oem_asset_pkg(char *pKeyFileName, char *pKeyPwdFileName,
                    char *encKeyBuff, uint32_t encKeyBuffSize,
        uint32_t assetType,
        uint8_t *asset, uint32_t assetSize,
    char *pPkgFileName)
{
    int rc = 0;
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
        (encKeyBuffSize != SB_CERT_RSA_KEY_SIZE_IN_BYTES) ||
        ((assetType != PROD_ASSET_TYPE_KCE)  && (assetType != PROD_ASSET_TYPE_KCP))||
        (pKeyFileName == NULL) ||
        (pKeyPwdFileName == NULL) ||
        (asset == NULL) ||
        (assetSize != PROD_ASSET_SIZE) ||
        (pPkgFileName == NULL)) {
        UTIL_LOG_ERR( "Invalid inputs\n");
        return 1;
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
        rc = 1;
        goto end_func;
    }

    // Decrypt temporary key created by ICV
    rc = CC_CommonRsaDecrypt(RSA_USE_PKCS_21_VERSION,
              pKeyFileName, pKeyPwdFileName,
              encKeyBuff, encKeyBuffSize,
              keyTmp) ;
    if (rc != 0) {
        UTIL_LOG_ERR( "failed to CC_CommonRsaDecrypt() for key temp, rc %d\n", rc);
        rc = 1;
        goto end_func;
    }


        if (assetType == PROD_ASSET_TYPE_KCE) {
                memcpy(keyProvContext, PROD_OEM_ENC_CONTEXT, PROD_KPROV_CONTEXT_SIZE);
        } else { // PROD_ASSET_TYPE_KCP
                memcpy(keyProvContext, PROD_OEM_PROV_CONTEXT, PROD_KPROV_CONTEXT_SIZE);
        }

        // Calculate Kprov= cmac(Ktmp, 0x01 || "P"  || 0x0 || asset_id || 0x80)
    rc = AesCmacKeyDerivation(keyTmp, PROD_KEY_TMP_KEY_SIZE,
                label,  PROD_KPROV_LABEL_SIZE,
                keyProvContext, PROD_KPROV_CONTEXT_SIZE,
                keyProv, PROD_KPROV_KEY_SIZE);
    if (rc != 0) {
        UTIL_LOG_ERR( "failed to AesCmacKeyDerivation() for Kprov, rc %d\n", rc);
        rc = 1;
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
        rc = 1;
        goto end_func;
    }

    // Writing the asset package into bin file
    rc = CC_CommonUtilCopyBuffToBinFile(pPkgFileName, (uint8_t *)&assetPackage, assetPkgSize);
    if (rc != 0) {
        UTIL_LOG_ERR( "failed to CC_CommonUtilCopyBuffToBinFile(), rc %d\n", rc);
        rc = 1;
        goto end_func;
    }

end_func:
    CloseOpenSsl();
    UTIL_LOG_ERR( "End rc %d\n", rc);
    return rc;
}

