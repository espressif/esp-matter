/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


/************* Include Files ****************/
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "cc_pal_log.h"
#include "cc_prod_error.h"
#include "prod_hw_defs.h"
#include "cc_regs.h"
#include "dx_host.h"
#include "dx_crys_kernel.h"
#include "prod_crypto_driver.h"
#include "cc_pal_mem.h"
#include "aes_driver.h"
#include "driver_defs.h"
#include "cc_production_asset.h"
#include "cc_aes_defs.h"
#include "ccm.h"

/**
 * This function is used to perform AES  operation.
 *
 * @param[in] pInternalKey  - a pointer to internal key
 * @param[in] pDataIn       - a pointer to input buffer - address must be 32bit aligned
 * @param[in] dataInSize    - size of data in bytes
 * @param[in] pOutbuff      - a pointer to output buffer- address must be 32bit aligned
 * @param[in] outbuffSize   - size of data out bytes
 *
 */

uint32_t CC_PROD_AesCcmDecrypt(uint8_t *pKey,
                               uint32_t  keySizeInBytes,
                               uint8_t *pNonce,
                               uint32_t  nonceSizeInBytes,
                               uint8_t *pAddData,
                               uint32_t  addDataSizeInBytes,
                               uint8_t *pCipherData,
                               uint32_t  dataSize,
                               uint8_t *pPlainBuff,
                               uint32_t  tagSize,
                               uint8_t *pTagBuff,
                               unsigned long workspaceAddr,
                            uint32_t     workspaceSize)
{

        uint32_t rc = 0;
        mbedtls_ccm_context *pCtx;

        if (workspaceSize < sizeof(mbedtls_ccm_context) || (workspaceAddr % CC_32BIT_WORD_SIZE)) {
                CC_PAL_LOG_ERR("invalid workspace\n");
                return CC_PROD_INVALID_PARAM_ERR;
        }
        pCtx = (mbedtls_ccm_context *)workspaceAddr;

        mbedtls_ccm_init(pCtx);

        rc = mbedtls_ccm_setkey(pCtx, MBEDTLS_CIPHER_ID_AES, pKey, keySizeInBytes * CC_BITS_IN_BYTE);
        if (rc != 0) {
                CC_PAL_LOG_ERR("Failed to mbedtls_ccm_setkey 0x%x\n", rc);
                return rc;
        }

        rc = mbedtls_ccm_auth_decrypt(pCtx, dataSize,
                                      pNonce, nonceSizeInBytes,
                                      pAddData, addDataSizeInBytes,
                                      pCipherData, pPlainBuff,
                                      pTagBuff, tagSize);
        if (rc != 0) {
                CC_PAL_LOG_ERR("Failed to mbedtls_ccm_auth_decrypt 0x%x\n", rc);
        }

        return rc;
}


/**
 * This function is used to perform AES  operation.
 *
 * @param[in] pInternalKey  - a pointer to internal key
 * @param[in] pDataIn       - a pointer to input buffer - address must be 32bit aligned
 * @param[in] dataInSize    - size of data in bytes
 * @param[in] pOutbuff      - a pointer to output buffer- address must be 32bit aligned
 * @param[in] outbuffSize   - size of data out bytes
 *
 */
uint32_t CC_PROD_Aes(aesMode_t cipherMode,
                     cryptoDirection_t encDecDir,
                     cryptoKeyType_t keyType,
                     uint8_t *pKey,
                     uint32_t  keySizeInBytes,
                     uint8_t *pIv,
                     uint32_t  ivSizeInBytes,
                     uint32_t *pDataIn,
                     uint32_t  dataInSize,
                     uint32_t *pOutbuff)
{
        uint32_t error;
        AesContext_t aesCtx;

        error = CC_PROD_AesInit(&aesCtx,
                                cipherMode,
                                encDecDir,
                                keyType, pKey, keySizeInBytes,
                                pIv, ivSizeInBytes);
        if (error != CC_OK) {
                return error;
        }
        error = CC_PROD_AesProcess(&aesCtx,
                                   pDataIn,  dataInSize,
                                   pOutbuff);
        if (error != CC_OK) {
                return error;
        }
        if ((cipherMode == CIPHER_CBC_MAC) && (pOutbuff != NULL)){
            CC_PalMemCopy((uint8_t *)pOutbuff, (uint8_t *)aesCtx.ivBuf, CC_AES_BLOCK_SIZE_IN_BYTES);
        }
        return CC_OK;
}

/**
 * This function is used to perform AES CBC MAC operation.
 *
 * @param[in] pKey  - a pointer to internal key
 * @param[in] pDataIn       - a pointer to input buffer
 * @param[in] blockSize     - size of data in bytes
 * @param[in] pCmacResult   - a pointer to output buffer
 *
 */
uint32_t CC_PROD_AesInit(AesContext_t *pAesCtx,
                         aesMode_t cipherMode,
                         cryptoDirection_t encDecDir,
                         cryptoKeyType_t keyType,
                         uint8_t *pKey,
                         uint32_t keySizeInBytes,
                         uint8_t *pIv,
                         uint32_t ivSizeInBytes)
{
        switch (keySizeInBytes) {
        case 16:
                pAesCtx->keySizeId = KEY_SIZE_128_BIT;
                break;
        case 24:
                pAesCtx->keySizeId = KEY_SIZE_192_BIT;
                break;
        case 32:
                pAesCtx->keySizeId = KEY_SIZE_256_BIT;
                break;
        default:
                CC_PAL_LOG_ERR("NOT OK\n");
                return 1;
        }
        pAesCtx->mode = cipherMode;
        pAesCtx->dir = encDecDir;
        pAesCtx->cryptoKey = keyType;
        pAesCtx->padType = CRYPTO_PADDING_NONE;
        pAesCtx->dataBlockType = FIRST_BLOCK;
        pAesCtx->inputDataAddrType = DLLI_ADDR;
        pAesCtx->outputDataAddrType = DLLI_ADDR;
        CC_PalMemCopy((uint8_t *)pAesCtx->ivBuf, pIv, ivSizeInBytes);
        if (keyType == USER_KEY) {
                CC_PalMemCopy((uint8_t *)pAesCtx->keyBuf, pKey, keySizeInBytes);
        }

        return CC_OK;
}


/**
 * This function is used to perform AES CBC MAC operation.
 *
 * @param[in] cipherMode    - the aes mode
 * @param[in] pDataIn       - a pointer to input buffer - address must be 32bit aligned
 * @param[in] dataInSize    - size of data in bytes
 * @param[out] pOutbuff     - a pointer to output buffer - address must be 32bit aligned
 * @param[in] outbuffSize   -  the expectedsize of output buffer in bytes
 *
 */
uint32_t CC_PROD_AesProcess(AesContext_t *pAesCtx,
                            uint32_t *pDataIn,
                            uint32_t  dataInSize,
                     uint32_t *pOutbuff)
{
    uint32_t error;
    CCBuffInfo_t inBuffInfo;
    CCBuffInfo_t outBuffInfo;

    /* set data buffers structures */
    error = SetDataBuffersInfo((uint8_t*)pDataIn, dataInSize, &inBuffInfo,
                               (uint8_t*)pOutbuff, dataInSize, &outBuffInfo);
    if (error != 0) {
         CC_PAL_LOG_ERR("illegal data buffers\n");
         return error;
    }

    return ProcessAesDrv(pAesCtx, &inBuffInfo, &outBuffInfo, dataInSize);
}



uint32_t  CC_PROD_KeyDerivation(cryptoKeyType_t keyType,
                                uint8_t *pUserKey,
                                const uint8_t               *pLabel,
                                size_t                      labelSize,
                                const uint8_t               *pContextData,
                                size_t                      contextSize,
                                uint8_t                     *pDerivedKey)
{
        uint32_t                dataInSize = 0;
        drvError_t      drvRc;
        AesContext_t    aesCtx;
        uint8_t         dataIn[PROD_KEY_TMP_LABEL_SIZE + PROD_KEY_TMP_CONTEXT_SIZE + 3] = { 0 };
        CCBuffInfo_t inBuffInfo;
        CCBuffInfo_t outBuffInfo;

        /* Check inputs */
        if (((keyType != USER_KEY) && (keyType != RTL_KEY)) ||
            ((keyType == USER_KEY) && (pUserKey == NULL)) ||
            (labelSize > PROD_KEY_TMP_LABEL_SIZE) ||
            (pLabel == NULL) ||
            (contextSize > PROD_KEY_TMP_CONTEXT_SIZE) ||
            (pContextData == NULL) ||
            (pDerivedKey == NULL)) {
                return CC_PROD_INVALID_PARAM_ERR;
        }

        /* Generate dataIn buffer for CMAC: iteration || Label || 0x00 || context || length
               since deruved key is 128 bits we have only 1 iteration */
        dataInSize = 0;
        dataIn[dataInSize++] = 1;
        CC_PalMemCopy((uint8_t *)&dataIn[dataInSize], pLabel, labelSize);
        dataInSize += labelSize;

        dataIn[dataInSize++] = 0x00;
        CC_PalMemCopy((uint8_t *)&dataIn[dataInSize], pContextData, contextSize);
        dataInSize += contextSize;
        dataIn[dataInSize++] =  CC_AES_BLOCK_SIZE_IN_BYTES * CC_BITS_IN_BYTE;  // all derived keys are 128 bits

        aesCtx.cryptoKey = keyType;
        aesCtx.keySizeId = KEY_SIZE_128_BIT;
        if (keyType == USER_KEY) {
                CC_PalMemCopy(aesCtx.keyBuf, pUserKey, CC_AES_BLOCK_SIZE_IN_BYTES);
        }

        aesCtx.mode               = CIPHER_CMAC;
        aesCtx.dir                = CRYPTO_DIRECTION_ENCRYPT;
        aesCtx.dataBlockType      = FIRST_BLOCK;
        aesCtx.inputDataAddrType  = DLLI_ADDR;
        aesCtx.outputDataAddrType = DLLI_ADDR;
        CC_PalMemSetZero(aesCtx.ivBuf, AES_IV_SIZE);

        /* set data buffers structures */
        drvRc = SetDataBuffersInfo((uint8_t*)dataIn, dataInSize, &inBuffInfo,
                                   NULL, 0, &outBuffInfo);
        if (drvRc != 0) {
             CC_PAL_LOG_ERR("illegal data buffers\n");
             return drvRc;
        }
        drvRc = FinishAesDrv(&aesCtx, &inBuffInfo, &outBuffInfo, dataInSize);
        if (drvRc != 0) {
                return drvRc;
        }
        CC_PalMemCopy(pDerivedKey, aesCtx.ivBuf, CC_AES_BLOCK_SIZE_IN_BYTES);
        return CC_OK;
}


