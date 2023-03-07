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
#include "aes_driver.h"
#include "driver_defs.h"
#include "cc_util_cmac.h"
#include "dx_crys_kernel.h"

/************************************************************************************/
/****************         CMAC key derivation    ************************************/
/************************************************************************************/


CCUtilError_t UtilCmacBuildDataForDerivation( const uint8_t               *pLabel,
                                             size_t                      labelSize,
                                             const uint8_t               *pContextData,
                                             size_t                      contextSize,
                                             uint8_t            *pDataIn,
                                             size_t                 *pDataInSize,
                                             size_t                      derivedKeySize)
{
        uint32_t      length = 0;
        uint32_t      lengthReverse = 0;
        uint32_t      i = 0;

        /* Check Label, Context, DerivedKey sizes */
        if (derivedKeySize > CC_UTIL_MAX_DERIVED_KEY_SIZE_IN_BYTES) {
                return CC_UTIL_ILLEGAL_PARAMS_ERROR;
        }

        if (derivedKeySize * CC_BITS_IN_BYTE > 0xFF) {
                length = CC_UTIL_FIX_DATA_MAX_SIZE_IN_BYTES;
        } else {
                length = CC_UTIL_FIX_DATA_MIN_SIZE_IN_BYTES;
        }

        if ( ((labelSize != 0) && (pLabel == NULL)) ||
             (labelSize == 0) ||
             (labelSize > CC_UTIL_MAX_LABEL_LENGTH_IN_BYTES) ) {
                return CC_UTIL_ILLEGAL_PARAMS_ERROR;
        }

        if ( ((contextSize != 0) && (pContextData == NULL)) ||
             (contextSize == 0) ||
             (contextSize > CC_UTIL_MAX_CONTEXT_LENGTH_IN_BYTES) ) {
                return CC_UTIL_ILLEGAL_PARAMS_ERROR;
        }
        if ((pDataIn == NULL) ||
             (*pDataInSize == 0) ||
             (*pDataInSize < (contextSize+labelSize+length)) ) {
                return CC_UTIL_ILLEGAL_PARAMS_ERROR;
        }

        i = 1;
        if (labelSize!=0) {
                CC_PalMemCopy((pDataIn+i), pLabel, labelSize);
                i+=labelSize;
        }

        pDataIn[i++] = 0x00;

        if (contextSize!=0) {
                CC_PalMemCopy((pDataIn+i), pContextData, contextSize);
                i+=contextSize;
        }

        length = derivedKeySize * CC_BITS_IN_BYTE;
        if (length > 0xFF) {
                /* Reverse words order and bytes in each word */
                lengthReverse = ((length & 0xFF00)>>8) | ((length & 0xFF)<<8);
                CC_PalMemCopy((pDataIn+i), (uint8_t*)&lengthReverse, 2);
                i += 2;
        } else {
                CC_PalMemCopy((pDataIn+i), (uint8_t*)&length, 1);
                i += 1;
        }
        *pDataInSize = i;

        return CC_OK;
}
CCUtilError_t UtilCmacDeriveKey(UtilKeyType_t       keyType,
                CCAesUserKeyData_t      *pUserKey,
                uint8_t         *pDataIn,
                size_t                  dataInSize,
                CCUtilAesCmacResult_t   pCmacResult)
{
        uint32_t      rc;
        AesContext_t    aesCtxBuf;
        CCBuffInfo_t inBuffInfo;
        CCBuffInfo_t outBuffInfo;

        /* Check inputs */
        if (NULL == pDataIn) {
                return CC_UTIL_DATA_IN_POINTER_INVALID_ERROR;
        }
        if (NULL == pCmacResult) {
                return CC_UTIL_DATA_OUT_POINTER_INVALID_ERROR;
        }
        if ((dataInSize < CC_UTIL_CMAC_DERV_MIN_DATA_IN_SIZE) ||
            (dataInSize > CC_UTIL_CMAC_DERV_MAX_DATA_IN_SIZE)) {
                return CC_UTIL_DATA_IN_SIZE_INVALID_ERROR;
        }

        switch(keyType){
        case UTIL_ROOT_KEY:
                /* Set AES key to ROOT KEY */
                aesCtxBuf.cryptoKey = RKEK_KEY;
                aesCtxBuf.keySizeId = KEY_SIZE_256_BIT;
                break;
        case UTIL_KCP_KEY:
                /* Set AES key to ROOT KEY */
                aesCtxBuf.cryptoKey = KCP_KEY;
                aesCtxBuf.keySizeId = KEY_SIZE_128_BIT;
                break;
        case UTIL_KPICV_KEY:
                /* Set AES key to ROOT KEY */
                aesCtxBuf.cryptoKey = KPICV_KEY;
                aesCtxBuf.keySizeId = KEY_SIZE_128_BIT;
                break;
        case UTIL_USER_KEY:
                if ((pUserKey->keySize != CC_UTIL_AES_128BIT_SIZE) &&
                    (pUserKey->keySize != CC_UTIL_AES_192BIT_SIZE) &&
                    (pUserKey->keySize != CC_UTIL_AES_256BIT_SIZE)) {
                        return CC_UTIL_INVALID_USER_KEY_SIZE;
                }
                /* Set AES key to USER KEY, and copy the key to the context */
                aesCtxBuf.cryptoKey = USER_KEY;
                switch (pUserKey->keySize) {
                case CC_UTIL_AES_128BIT_SIZE:
                        aesCtxBuf.keySizeId = KEY_SIZE_128_BIT;
                        break;
                case CC_UTIL_AES_192BIT_SIZE:
                        aesCtxBuf.keySizeId = KEY_SIZE_192_BIT;
                        break;
                case CC_UTIL_AES_256BIT_SIZE:
                        aesCtxBuf.keySizeId = KEY_SIZE_256_BIT;
                        break;
                default:
                        break;
                }
                CC_PalMemCopy(aesCtxBuf.keyBuf, pUserKey->pKey, pUserKey->keySize);
                break;
        default:
                return CC_UTIL_INVALID_KEY_TYPE;
        }


        /* call CC_AES_Init with CMAC */
        aesCtxBuf.mode               = CIPHER_CMAC;
        aesCtxBuf.dir                = CRYPTO_DIRECTION_ENCRYPT;
        aesCtxBuf.dataBlockType      = FIRST_BLOCK;
        aesCtxBuf.inputDataAddrType  = DLLI_ADDR;
        aesCtxBuf.outputDataAddrType = DLLI_ADDR;
        CC_PalMemSetZero(aesCtxBuf.ivBuf, AES_IV_SIZE);

        /* set data buffers structures */
        rc = SetDataBuffersInfo(pDataIn, dataInSize, &inBuffInfo,
                                pCmacResult, CC_UTIL_AES_CMAC_RESULT_SIZE_IN_BYTES, &outBuffInfo);
        if (rc != 0) {
             CC_PAL_LOG_ERR("illegal data buffers\n");
             return CC_UTIL_FATAL_ERROR;
        }

        rc = FinishAesDrv(&aesCtxBuf, &inBuffInfo, &outBuffInfo, dataInSize);
        if (rc != 0) {
                return CC_UTIL_FATAL_ERROR;
        }

        CC_PalMemCopy(pCmacResult, aesCtxBuf.ivBuf, CC_AES_BLOCK_SIZE_IN_BYTES);

        return CC_UTIL_OK;
}




