/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#define CC_PAL_LOG_CUR_COMPONENT CC_LOG_MASK_CC_API

#include "cc_pal_types.h"
#include "cc_pal_mem.h"
#include "cc_pal_abort.h"
#include "chacha_driver.h"
#include "mbedtls_cc_chacha.h"
#include "mbedtls_cc_chacha_error.h"
#include "driver_defs.h"
#include "cc_sym_error.h"

static CCError_t Driver2CCChachaErr(drvError_t drvRc)
{
        switch (drvRc) {
        case CHACHA_DRV_OK:
                return CC_OK;
        case CHACHA_DRV_INVALID_USER_CONTEXT_POINTER_ERROR:
                return CC_CHACHA_INVALID_USER_CONTEXT_POINTER_ERROR;
        case CHACHA_DRV_ILLEGAL_OPERATION_DIRECTION_ERROR:
                return CC_CHACHA_INVALID_ENCRYPT_MODE_ERROR;
        case CHACHA_DRV_ILLEGAL_INPUT_ADDR_MEM_ERROR:
                return CC_CHACHA_DATA_IN_POINTER_INVALID_ERROR;
        case CHACHA_DRV_ILLEGAL_OUTPUT_ADDR_MEM_ERROR:
                return CC_CHACHA_DATA_OUT_POINTER_INVALID_ERROR;
        case CHACHA_DRV_ILLEGAL_NONCE_SIZE_ERROR:
                return CC_CHACHA_INVALID_NONCE_ERROR;
        default:
                return CC_FATAL_ERROR;
        }
}

static CCError_t ChachaBlock(mbedtls_chacha_user_context    *pContextID,
        uint8_t                     *pDataIn,
        size_t                      dataInSize,
        uint8_t                     *pDataOut)
{
        drvError_t drvRc = CHACHA_DRV_OK;
        ChachaContext_t *chachaCtx = NULL;
        uintptr_t upDataOut = 0;
        uintptr_t upDataIn = 0;
        CCBuffInfo_t inBuffInfo;
        CCBuffInfo_t outBuffInfo;

        /* if the users context ID pointer is NULL return an error */
        if (pContextID == NULL) {
                return CC_CHACHA_INVALID_USER_CONTEXT_POINTER_ERROR;
        }

        /* if the users Data In pointer is illegal return an error */
        if (pDataIn == NULL) {
                return CC_CHACHA_DATA_IN_POINTER_INVALID_ERROR;
        }

        /* Size zero is not a valid block operation */
        if (dataInSize == 0) {
                return CC_CHACHA_DATA_IN_SIZE_ILLEGAL;
        }

        /* if the users Data Out pointer is illegal return an error */
        if (pDataOut == NULL) {
                return CC_CHACHA_DATA_OUT_POINTER_INVALID_ERROR;
        }

        upDataOut = (uintptr_t)pDataOut;
        upDataIn = (uintptr_t)pDataIn;
        if ((((upDataOut > upDataIn) && (upDataOut - upDataIn < dataInSize))) ||
                (((upDataIn > upDataOut) && (upDataIn - upDataOut < dataInSize)))) {
                return CC_CHACHA_DATA_OUT_POINTER_INVALID_ERROR;
        }

        chachaCtx = (ChachaContext_t *)pContextID;

        /* set data buffers structures */
        drvRc = SetDataBuffersInfo(pDataIn, dataInSize, &inBuffInfo,
                                   pDataOut, dataInSize, &outBuffInfo);
        if (drvRc != 0) {
             CC_PAL_LOG_ERR("illegal data buffers\n");
             return CC_CHACHA_DATA_IN_POINTER_INVALID_ERROR;
        }

        drvRc = ProcessChacha(chachaCtx, &inBuffInfo, &outBuffInfo, dataInSize);

        return Driver2CCChachaErr(drvRc);
}

CIMPORT_C CCError_t  mbedtls_chacha_init(mbedtls_chacha_user_context    *pContextID,
        mbedtls_chacha_nonce     pNonce,
                                               mbedtls_chacha_nonce_size_t        nonceSize,
                                               mbedtls_chacha_key           pKey,
                                               uint32_t                    initialCounter,
                                               mbedtls_chacha_encrypt_mode_t   EncryptDecryptFlag)
{
        ChachaContext_t *chachaCtx = NULL;

        /* Chacha key size bytes */
        uint32_t keySizeBytes = CC_CHACHA_KEY_MAX_SIZE_IN_BYTES;
        /* Chacha nonce size bytes */
        uint32_t nonceSizeBytes = 0;

        /* FUNCTION LOGIC */

        /* ............... local initializations .............................. */
        /* -------------------------------------------------------------------- */

        /* ............... checking the parameters validity ................... */
        /* -------------------------------------------------------------------- */

        /* if the users context ID pointer is NULL return an error */
        if (pContextID == NULL) {
                return CC_CHACHA_INVALID_USER_CONTEXT_POINTER_ERROR;
        }

        if (sizeof(mbedtls_chacha_user_context) != sizeof(ChachaContext_t)) {
                return CC_CHACHA_CTX_SIZES_ERROR;
        }

        /* if the pNonce is NULL return an error */
        if (pNonce == NULL) {
                return CC_CHACHA_INVALID_NONCE_PTR_ERROR;
        }

        /* check the nonce size */
        if (nonceSize == CC_CHACHA_Nonce64BitSize) {
        nonceSizeBytes = 8;
    } else if (nonceSize == CC_CHACHA_Nonce96BitSize) {
        nonceSizeBytes = CC_CHACHA_NONCE_MAX_SIZE_IN_BYTES;
    } else {
                return  CC_CHACHA_INVALID_NONCE_ERROR;
        }

        /* check the Encrypt / Decrypt flag validity */
        if (EncryptDecryptFlag >= CC_CHACHA_EncryptNumOfOptions) {
                return  CC_CHACHA_INVALID_ENCRYPT_MODE_ERROR;
        }

        /*  check the validity of the key pointer */
        if (pKey == NULL) {
                return  CC_CHACHA_INVALID_KEY_POINTER_ERROR;
        }

        chachaCtx = (ChachaContext_t *)pContextID;
        chachaCtx->dir = (enum cryptoDirection)EncryptDecryptFlag;
        chachaCtx->nonceSize = (chachaNonceSize_t)nonceSize;
        chachaCtx->inputDataAddrType = DLLI_ADDR;
        chachaCtx->outputDataAddrType = DLLI_ADDR;

        /* Copy the key to the context */
        CC_PalMemCopy(chachaCtx->keyBuf, pKey, keySizeBytes);

        /* Copy the nonce to the context */
        CC_PalMemCopy(chachaCtx->nonceBuf, pNonce, nonceSizeBytes);

        /* init the block counter */
        chachaCtx->blockCounterLsb = initialCounter;
        chachaCtx->blockCounterMsb = 0;

        return CC_OK;
}

CIMPORT_C CCError_t  mbedtls_chacha_block(mbedtls_chacha_user_context    *pContextID,
                                         uint8_t                     *pDataIn,
                                         size_t                      dataInSize,
                                         uint8_t                     *pDataOut )
{
        if ((dataInSize % CHACHA_BLOCK_SIZE_BYTES) != 0) {
                return CC_CHACHA_DATA_IN_SIZE_ILLEGAL;
        }
        if (dataInSize == 0) {
                return CC_OK;
        }

        return ChachaBlock(pContextID, pDataIn, dataInSize, pDataOut);
}

CIMPORT_C CCError_t  mbedtls_chacha_finish(mbedtls_chacha_user_context    *pContextID,
        uint8_t                     *pDataIn,
        size_t                      dataInSize,
        uint8_t                     *pDataOut)
{
        if (dataInSize == 0) {
                return CC_OK;
        }

        return ChachaBlock(pContextID, pDataIn, dataInSize, pDataOut);
}

CIMPORT_C CCError_t  mbedtls_chacha_free(mbedtls_chacha_user_context *pContextID)
{
        ChachaContext_t *chachaCtx = (ChachaContext_t*)pContextID;

        if (pContextID == NULL) {
                return CC_CHACHA_INVALID_USER_CONTEXT_POINTER_ERROR;
        }

        /* Zero the context */
        CC_PalMemSetZero(chachaCtx, sizeof(ChachaContext_t));

        return CC_OK;
}

CIMPORT_C CCError_t  mbedtls_chacha(mbedtls_chacha_nonce       pNonce,
                                   mbedtls_chacha_nonce_size_t          nonceSize,
                                   mbedtls_chacha_key             pKey,
                                   uint32_t                      initialCounter,
                                   mbedtls_chacha_encrypt_mode_t     encryptDecryptFlag,
                                   uint8_t                       *pDataIn,
                                   size_t                        dataInSize,
                                   uint8_t                       *pDataOut )
{
    mbedtls_chacha_user_context UserContext;
    CCError_t Error = CC_OK;


    /* if the users Data In pointer is illegal return an error */
    if ( (pDataIn == NULL) ^ (dataInSize == 0) ) {
        return CC_CHACHA_DATA_IN_POINTER_INVALID_ERROR;
    }

    /* limit the input size. in IoT no reason for such a big size */
    if ( dataInSize > CC_MAX_UINT32_VAL ) {
        return CC_CHACHA_DATA_IN_SIZE_ILLEGAL;
    }

    /* Size zero is valid - do nothing and return with CC_OK */
    if (dataInSize == 0) {
        return CC_OK;
    }

    Error = mbedtls_chacha_init(&UserContext, pNonce, nonceSize, pKey, initialCounter, encryptDecryptFlag);
    if (Error != CC_OK) {
        goto end;
    }

    Error = mbedtls_chacha_finish(&UserContext, pDataIn, dataInSize, pDataOut);
    if (Error != CC_OK) {
            goto end;
    }

    Error = mbedtls_chacha_free(&UserContext);
    if (Error != CC_OK) {
            goto end;
    }

end:
    return Error;
}


