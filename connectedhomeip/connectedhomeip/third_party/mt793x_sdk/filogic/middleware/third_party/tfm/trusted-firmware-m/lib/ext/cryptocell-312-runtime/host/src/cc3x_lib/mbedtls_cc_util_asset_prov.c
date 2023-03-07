/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


/************* Include Files ****************/
#include "cc_hal_plat.h"
#include "cc_regs.h"
#include "cc_pal_mem.h"
#include "cc_pal_mutex.h"
#include "cc_pal_abort.h"
#include "cc_util_int_defs.h"
#include "mbedtls_cc_util_defs.h"
#include "cc_util_error.h"
#include "cc_aes_defs.h"
#include "ccm.h"
#include "aes_driver.h"
#include "driver_defs.h"
#include "cc_util_cmac.h"
#include "mbedtls_cc_util_asset_prov.h"
#include "cc_util_asset_prov_int.h"


CCError_t mbedtls_util_asset_pkg_unpack(CCAssetProvKeyType_t        keyType,
            uint32_t        assetId,
            uint32_t        *pAssetPkgBuff,
            size_t          assetPackageLen,
            uint32_t        *pAssetData,
            size_t          *pAssetDataLen)
{
    uint32_t  rc = CC_OK;
    CCUtilAesCmacResult_t         keyProv = { 0 };
    uint8_t     dataIn[CC_UTIL_MAX_KDF_SIZE_IN_BYTES] = { 0 };
    size_t    dataInSize = CC_UTIL_MAX_KDF_SIZE_IN_BYTES;
    uint8_t     provLabel = 'P';
    CCAssetProvPkg_t   *pAssetPackage = NULL;
    mbedtls_ccm_context ccmCtx;
    uint8_t    ccmAddData[CC_ASSET_PROV_ADATA_SIZE] = { 0 };
    uint8_t    ccmNonceData[CC_ASSET_PROV_NONCE_SIZE] = { 0 };
    uint32_t    assetDataSize = 0;
    uint32_t    constPkgSize = CC_ASSET_PROV_ADATA_SIZE+CC_ASSET_PROV_NONCE_SIZE+CC_ASSET_PROV_TAG_SIZE;
    uint32_t    minPkgSize = constPkgSize+CC_ASSET_PROV_BLOCK_SIZE;

    /* Validate Inputs */
    if ((pAssetPkgBuff == NULL) ||
        (pAssetData == NULL) ||
        (pAssetDataLen == NULL) ||
        (assetPackageLen > CC_ASSET_PROV_MAX_ASSET_PKG_SIZE) ||
        (assetPackageLen < minPkgSize) ||
        /* Overlapping verification */
        (((unsigned long)pAssetPkgBuff + assetPackageLen) < (unsigned long)pAssetPkgBuff) ||
        (assetPackageLen  % CC_32BIT_WORD_SIZE) ||
        ((keyType != ASSET_PROV_KEY_TYPE_KPICV) && (keyType != ASSET_PROV_KEY_TYPE_KCP))) {
            CC_PAL_LOG_ERR("Invalid params");
            return CC_UTIL_ILLEGAL_PARAMS_ERROR;
    }
    pAssetPackage = (CCAssetProvPkg_t *)pAssetPkgBuff;

    /* Validate asset size, must be multiply of 16 bytes */
    if ((pAssetPackage->assetSize > CC_ASSET_PROV_MAX_ASSET_SIZE) ||
        (pAssetPackage->assetSize == 0) ||
        (pAssetPackage->assetSize % CC_ASSET_PROV_BLOCK_SIZE)) {
            CC_PAL_LOG_ERR("Invalid asset size 0x%x", pAssetPackage->assetSize);
            return CC_UTIL_ILLEGAL_PARAMS_ERROR;
    }
    /* Verify assetDataSize against assetPkgSize */
    if ((assetPackageLen < (constPkgSize+pAssetPackage->assetSize)) ||
        (*pAssetDataLen  < pAssetPackage->assetSize)) {
            CC_PAL_LOG_ERR("Invalid asset size 0x%x", pAssetPackage->assetSize);
            return CC_UTIL_ILLEGAL_PARAMS_ERROR;
    }
    /* Verify package token and version */
    if ((pAssetPackage->token != CC_ASSET_PROV_TOKEN) ||
        (pAssetPackage->version != CC_ASSET_PROV_VERSION)) {
            CC_PAL_LOG_ERR("Invalid token or version");
            return CC_UTIL_ILLEGAL_PARAMS_ERROR;
    }

    /* Generate dataIn buffer for CMAC: iteration || 'P' || 0x00 || asset Id || 0x80
           since deruved key is 128 bits we have only 1 iteration */
    rc = UtilCmacBuildDataForDerivation(&provLabel,sizeof(provLabel),
                                          (uint8_t *)&assetId, sizeof(assetId),
                                         dataIn, &dataInSize,
                                         (size_t)CC_UTIL_AES_CMAC_RESULT_SIZE_IN_BYTES);
    if (rc != 0) {
            CC_PAL_LOG_ERR("Failed UtilCmacBuildDataForDerivation 0x%x", rc);
            return rc;
    }
    dataIn[0] = 1;  // only 1 iteration
    rc = UtilCmacDeriveKey((keyType == ASSET_PROV_KEY_TYPE_KPICV)?UTIL_KPICV_KEY:UTIL_KCP_KEY,
                            NULL,
                            dataIn, dataInSize,
                            keyProv);
    if (rc != 0) {
            CC_PAL_LOG_ERR("Failed UtilCmacDeriveKey 0x%x", rc);
            return rc;
    }

    /* Decrypt and authenticate the BLOB */
    mbedtls_ccm_init(&ccmCtx);

    rc = mbedtls_ccm_setkey(&ccmCtx, MBEDTLS_CIPHER_ID_AES, keyProv, CC_UTIL_AES_CMAC_RESULT_SIZE_IN_BYTES * CC_BITS_IN_BYTE);
    if (rc != 0) {
            CC_PAL_LOG_ERR("Failed to mbedtls_ccm_setkey 0x%x\n", rc);
            return rc;
    }

    assetDataSize = pAssetPackage->assetSize;
    CC_PalMemCopy(ccmNonceData, pAssetPackage->nonce, CC_ASSET_PROV_NONCE_SIZE);
    CC_PalMemCopy(ccmAddData, (uint8_t *)pAssetPackage, CC_ASSET_PROV_ADATA_SIZE);
    rc = mbedtls_ccm_auth_decrypt(&ccmCtx, pAssetPackage->assetSize,
                                  ccmNonceData, CC_ASSET_PROV_NONCE_SIZE,
                                  ccmAddData, CC_ASSET_PROV_ADATA_SIZE,
                                  pAssetPackage->encAsset, (uint8_t *)pAssetData,
                                  pAssetPackage->encAsset + pAssetPackage->assetSize, CC_ASSET_PROV_TAG_SIZE);
    if (rc != 0) {
            CC_PAL_LOG_ERR("Failed to mbedtls_ccm_auth_decrypt 0x%x\n", rc);
            return rc;
    }

    // Set output data
    *pAssetDataLen = assetDataSize;
    return CC_OK;
}

