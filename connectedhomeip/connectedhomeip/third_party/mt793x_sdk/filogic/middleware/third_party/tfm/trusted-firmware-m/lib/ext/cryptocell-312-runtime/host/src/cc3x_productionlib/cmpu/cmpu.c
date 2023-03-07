/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "cc_otp_defs.h"
#include "cc_pal_mem.h"
#include "dx_crys_kernel.h"
#include "prod_hw_defs.h"
#include "cc_pal_log.h"
#include "prod_util.h"
#include "cc_prod_error.h"
#include "cmpu_llf_rnd.h"
#include "prod_crypto_driver.h"
#include "cmpu_derivation.h"
#include "mbedtls_cc_mng_int.h"
#include "mbedtls_cc_mng.h"


static uint32_t CC_PROD_CalcHuk(uint32_t *pBuffForOtp,
                                uint32_t *pManufactorWord,
                                unsigned long workspaceAddr)
{
    uint32_t error = 0;
    uint32_t  zeroCount = 0;
    uint8_t   pKey[CC_PROD_AES_Key256Bits_SIZE_IN_BYTES] = { 0 };
    uint8_t   pIv[CC_PROD_AES_IV_COUNTER_SIZE_IN_BYTES] = { 0 };
    uint32_t *pEntrSrc;
    uint32_t  sourceSize;
    uint32_t *pRndWorkBuff;

    /*Call CC_PROD_LLF_RND_GetTrngSource to get entropy bits and to check entropy size*/
    pRndWorkBuff = (uint32_t *)workspaceAddr;
    error = CC_PROD_LLF_RND_GetTrngSource((uint32_t **)&pEntrSrc, &sourceSize, pRndWorkBuff);
    if (error != CC_OK) {
        CC_PAL_LOG_ERR("failed CC_PROD_LLF_RND_GetTrngSource, error is 0x%X\n", error);
        return error;
    }
    error = CC_PROD_Derivation_Instantiate(pEntrSrc,
                                           sourceSize,
                                           pKey,
                                           pIv);
    if (error != CC_OK) {
        CC_PAL_LOG_ERR("failed to CC_PROD_Derivation_Instantiate, error 0x%x\n", error);
        return error;
    }
    error = CC_PROD_Derivation_Generate(pKey,
                                        pIv,
                                        pBuffForOtp,
                                        CC_OTP_HUK_SIZE_IN_WORDS * sizeof(uint32_t));
    if (error != CC_OK) {
        CC_PAL_LOG_ERR("failed to CC_PROD_Derivation_Generate, error 0x%x\n", error);
        return error;
    }

    error = CC_PROD_LLF_RND_VerifyGeneration((uint8_t *)pBuffForOtp);
    if (error != CC_OK) {
        CC_PAL_LOG_ERR("failed to CC_PROD_LLF_RND_VerifyGeneration, error 0x%x\n", error);
        return error;
    }

    /*Count number of zero bits in HUK OTP fileds*/
    error  = CC_PROD_GetZeroCount(pBuffForOtp, CC_OTP_HUK_SIZE_IN_WORDS, &zeroCount);
    if (error != CC_OK) {
        CC_PAL_LOG_ERR("Invalid Huk zero count\n");
        return error;
    }

    SET_OTP_MANUFACTURE_FLAG(*pManufactorWord, HUK_ZERO_BITS, zeroCount);
    return CC_OK;
}

static uint32_t CC_PROD_HandleHbk0(CCCmpuData_t *pCmpuData,
                                   uint32_t *pHbk0BuffForOtp,
                                   uint32_t *pDcuLockBuffForOtp,
                                   uint32_t *pSwVerBuffForOtp,
                                   uint32_t *pManufactorWord)
{
    uint32_t error = 0;
    uint32_t  zeroCount = 0;
    uint32_t  i = 0;
    uint32_t icvOwnership = 0;
    switch (pCmpuData->uniqueDataType){
    case CMPU_UNIQUE_IS_USER_DATA:
        /* If HBK not in use set the bit in manufactor flag */
        SET_OTP_MANUFACTURE_FLAG(*pManufactorWord, HBK0_NOT_IN_USE, 1);
        break;

    case CMPU_UNIQUE_IS_HBK0:
        /* Calculate HBK0 zeros */
        CC_PalMemCopy(pHbk0BuffForOtp, pCmpuData->uniqueBuff.hbk0, CC_OTP_HBK0_SIZE_IN_WORDS * CC_32BIT_WORD_SIZE);
        error = CC_PROD_GetZeroCount(pHbk0BuffForOtp, PROD_KEY_TMP_CONTEXT_WORD_SIZE, &zeroCount);
        if (error != CC_OK) {
            CC_PAL_LOG_ERR("Invalid Huk zero count\n");
            return error;
        }

        /* Keep only ICV bit in DCU default locking */
        for (i = 0; i < CC_OTP_DCU_SIZE_IN_WORDS; i++) {
            icvOwnership = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, AO_ICV_DCU_RESTRICTION_MASK0) + (i * CC_32BIT_WORD_SIZE));
            pDcuLockBuffForOtp[i] = pCmpuData->icvDcuDefaultLock[i] & icvOwnership;
        }

        /* set ICV minimum SW version */
        error = CC_PROD_BitListFromNum(pSwVerBuffForOtp, CC_OTP_HBK0_MIN_VERSION_SIZE_IN_WORDS, pCmpuData->icvMinVersion);
        if (error != CC_OK) {
            CC_PAL_LOG_ERR("Failed to get bit list from number\n");
            return error;
        }
        break;

    default:
        CC_PAL_LOG_ERR("Invalid unique data type \n");
        return CC_PROD_INVALID_PARAM_ERR;
    }

    SET_OTP_MANUFACTURE_FLAG(*pManufactorWord, HBK0_ZERO_BITS, zeroCount);
    return CC_OK;
}


static uint32_t CC_PROD_HandleKpicv(CCCmpuData_t *pCmpuData,
                                    uint32_t *pBuffForOtp,
                                    uint32_t *pManufactorWord,
                                    unsigned long workspaceAddr,
                                    uint32_t     workspaceSize)
{
    uint32_t error = 0;
    uint32_t  zeroCount = 0;

    switch (pCmpuData->kpicvDataType) {
    case  ASSET_NO_KEY:
        SET_OTP_MANUFACTURE_FLAG(*pManufactorWord, KPICV_NOT_IN_USE, 1);
        goto end_kpicv;
    case  ASSET_PKG_KEY:
        error = CC_PROD_PkgVerify((CCProdAssetPkg_t *)&pCmpuData->kpicv.pkgAsset,
                                  (const uint8_t *)PROD_ICV_PROV_CONTEXT, PROD_KPROV_CONTEXT_SIZE,
                                  (const uint8_t *)PROD_ICV_KEY_TMP_LABEL, PROD_KEY_TMP_LABEL_SIZE,
                                  pCmpuData->uniqueBuff.hbk0, CC_OTP_HBK0_SIZE_IN_WORDS * CC_32BIT_WORD_SIZE,
                                  (uint8_t *  )pBuffForOtp,
                                  workspaceAddr,
                                  workspaceSize);
        if (error != CC_OK) {
            CC_PAL_LOG_ERR("failed to CC_PROD_PkgVerify fro Kpicv\n");
            return error;
        }
        break;
    case ASSET_PLAIN_KEY:
        CC_PalMemCopy(pBuffForOtp, pCmpuData->kpicv.plainAsset, PROD_ASSET_SIZE);
        break;
    default:
        CC_PAL_LOG_ERR("Inavlid key type for  Kpicv\n");
        return CC_PROD_INVALID_PARAM_ERR;
    }
    error = CC_PROD_GetZeroCount(pBuffForOtp, PROD_KEY_TMP_CONTEXT_WORD_SIZE, &zeroCount);
    if (error != CC_OK) {
        CC_PAL_LOG_ERR("Invalid zero count for Kpicv\n");
        return error;
    }
end_kpicv:
    SET_OTP_MANUFACTURE_FLAG(*pManufactorWord, KPICV_ZERO_BITS, zeroCount);
    return CC_OK;
}


static uint32_t CC_PROD_HandleKceicv(CCCmpuData_t *pCmpuData,
                                     uint32_t *pBuffForOtp,
                                     uint32_t *pManufactorWord,
                                     unsigned long workspaceAddr,
                                     uint32_t     workspaceSize)
{
    uint32_t error = 0;
    uint32_t  zeroCount = 0;

    switch (pCmpuData->kceicvDataType) {
    case  ASSET_NO_KEY:
        SET_OTP_MANUFACTURE_FLAG(*pManufactorWord, KCEICV_NOT_IN_USE, 1);
        goto end_kceicv;
    case  ASSET_PKG_KEY:
        error = CC_PROD_PkgVerify((CCProdAssetPkg_t *)&pCmpuData->kceicv.pkgAsset,
                                  (const uint8_t *)PROD_ICV_ENC_CONTEXT, PROD_KPROV_CONTEXT_SIZE,
                                  (const uint8_t *)PROD_ICV_KEY_TMP_LABEL, PROD_KEY_TMP_LABEL_SIZE,
                                  pCmpuData->uniqueBuff.hbk0, CC_OTP_HBK0_SIZE_IN_WORDS * CC_32BIT_WORD_SIZE,
                                  (uint8_t *  )pBuffForOtp,
                                  workspaceAddr,
                                  workspaceSize);
        if (error != CC_OK) {
            CC_PAL_LOG_ERR("failed to CC_PROD_PkgVerify fro Kceicv\n");
            return error;
        }
        break;
    case ASSET_PLAIN_KEY:
        CC_PalMemCopy(pBuffForOtp, pCmpuData->kceicv.plainAsset, PROD_ASSET_SIZE);
        break;
    default:
        CC_PAL_LOG_ERR("Invalid key type for Kceicv\n");
        return CC_PROD_INVALID_PARAM_ERR;
    }

    error = CC_PROD_GetZeroCount(pBuffForOtp, PROD_KEY_TMP_CONTEXT_WORD_SIZE, &zeroCount);
    if (error != CC_OK) {
        CC_PAL_LOG_ERR("Invalid zerocount for Kceicv\n");
        return error;
    }

end_kceicv:
    SET_OTP_MANUFACTURE_FLAG(*pManufactorWord, KCEICV_ZERO_BITS, zeroCount);
    return CC_OK;
}



CCError_t CCProd_Cmpu( unsigned long  ccHwRegBaseAddr,
                       CCCmpuData_t   *pCmpuData,
                       unsigned long  workspaceBaseAddr,
                       uint32_t       workspaceSize)
{
    uint32_t error = 0;
    uint32_t lcs = 0;
    uint32_t manufactorWord = 0;
    uint32_t hukBuffForOtp[CC_OTP_HUK_SIZE_IN_WORDS] = { 0 };
    uint32_t hbk0BuffForOtp[CC_OTP_HBK0_SIZE_IN_WORDS] = { 0 };
    uint32_t kpicvBuffForOtp[CC_OTP_KPICV_SIZE_IN_WORDS] = { 0 };
    uint32_t kceicvBuffForOtp[CC_OTP_KCEICV_SIZE_IN_WORDS] = { 0 };
    uint32_t icvLockBitsBuffForOtp[CC_OTP_DCU_SIZE_IN_WORDS] = { 0 };
    uint32_t   icvMinSwVersion[CC_OTP_HBK0_MIN_VERSION_SIZE_IN_WORDS] = { 0 };

    if (sizeof(CCProdAssetPkg_t) != PROD_ASSET_PKG_SIZE) {
        CC_PAL_LOG_ERR("invalid Pkg size\n");
        return CC_PROD_INVALID_PARAM_ERR;
    }
    if ((pCmpuData == NULL) ||
            (workspaceBaseAddr == 0) ||  //can not be zero because it is cast to a pointer later
            (workspaceBaseAddr % CC_32BIT_WORD_SIZE) ||   // workspace address must be word aligned
            (workspaceSize < CMPU_WORKSPACE_MINIMUM_SIZE) ) {
        CC_PAL_LOG_ERR("invalid params\n");
        return CC_PROD_INVALID_PARAM_ERR;
    }

    gCcRegBase = ccHwRegBaseAddr;

    error = CCProd_Init();
    if (error != CC_OK) {
        CC_PAL_LOG_ERR("Failed to CCProd_Init 0x%x\n", error);
        goto cmpuEnd;
    }

    /* Check LCS - CM only */
    error = mbedtls_mng_lcsGet(&lcs);
    if (error != CC_OK) {
        CC_PAL_LOG_ERR("Failed to get LCS 0x%x \n", error);
        goto cmpuEnd;
    }

    /* Verify LCS is CM */
    if (lcs != CC_MNG_LCS_CM) {
        CC_PAL_LOG_ERR("LCS is %d not valid\n", lcs);
        error = CC_PROD_ILLEGAL_LCS_ERR;
        goto cmpuEnd;
    }

    /* Genertae HUK  */
    error = CC_PROD_CalcHuk(hukBuffForOtp, &manufactorWord, workspaceBaseAddr);
    if (error != CC_OK) {
        CC_PAL_LOG_ERR("failed to CC_PROD_CalcHuk 0x%x \n", error);
        goto cmpuEnd;
    }

    /* Set HBK0 if exists, and calculate its zero count. or set HBK0 not in use
           Handle HBK0  dependencies: DCU lock bits and ICV minimum version */
    error = CC_PROD_HandleHbk0(pCmpuData, hbk0BuffForOtp, icvLockBitsBuffForOtp, icvMinSwVersion, &manufactorWord);
    if (error != CC_OK) {
        CC_PAL_LOG_ERR("failed to CC_PROD_HandleHbk0 0x%x \n", error);
        goto cmpuEnd;
    }

    /* Handle Kpicv, or set Kpicv not in use */
    error = CC_PROD_HandleKpicv(pCmpuData, kpicvBuffForOtp, &manufactorWord, workspaceBaseAddr, workspaceSize);
    if (error != CC_OK) {
        CC_PAL_LOG_ERR("failed to CC_PROD_HandleKpicv 0x%x \n", error);
        goto cmpuEnd;
    }


    /* Handle Kceicv, , or set Kceicv not in use */
    error = CC_PROD_HandleKceicv(pCmpuData, kceicvBuffForOtp, &manufactorWord, workspaceBaseAddr, workspaceSize);
    if (error != CC_OK) {
        CC_PAL_LOG_ERR("failed to CC_PROD_HandleKceicv 0x%x \n", error);
        goto cmpuEnd;
    }

    /* First, Burn Manufactor flag, including not in use flags set by ahndle functions above  */
    CC_PROD_OTP_WRITE_VERIFY_WORD(CC_OTP_MANUFACTURE_FLAG_OFFSET, manufactorWord, error);
    if (error != CC_OK) {
        CC_PAL_LOG_ERR("Failed to verify OTP write 0x%x \n", error);
        goto cmpuEnd;
    }

    /* Burn Huk */
    CC_PROD_OTP_WRITE_VERIFY_WORD_BUFF(CC_OTP_HUK_OFFSET, hukBuffForOtp, CC_OTP_HUK_SIZE_IN_WORDS, error);
    if (error != CC_OK) {
        CC_PAL_LOG_ERR("Failed to verify OTP write 0x%x \n", error);
        goto cmpuEnd;
    }

    /* Burn HBK0 and its dependencies: DCU lock bits and SW version */
    if (pCmpuData->uniqueDataType == CMPU_UNIQUE_IS_HBK0) {
        /* HBK0 */
        CC_PROD_OTP_WRITE_VERIFY_WORD_BUFF(CC_OTP_HBK0_OFFSET, hbk0BuffForOtp, CC_OTP_HBK0_SIZE_IN_WORDS, error);
        if (error != CC_OK) {
            CC_PAL_LOG_ERR("Failed to verify OTP write 0x%x \n", error);
            goto cmpuEnd;
        }
        /* DCU lock bits */
        CC_PROD_OTP_WRITE_VERIFY_WORD_BUFF(CC_OTP_DCU_OFFSET, icvLockBitsBuffForOtp, CC_OTP_DCU_SIZE_IN_WORDS, error);
        if (error != CC_OK) {
            CC_PAL_LOG_ERR("Failed to verify OTP write 0x%x \n", error);
            goto cmpuEnd;
        }
        /* SW version */
        CC_PROD_OTP_WRITE_VERIFY_WORD_BUFF(CC_OTP_HBK0_MIN_VERSION_OFFSET, icvMinSwVersion, CC_OTP_HBK0_MIN_VERSION_SIZE_IN_WORDS, error);
        if (error != CC_OK) {
            CC_PAL_LOG_ERR("Failed to verify OTP write 0x%x \n", error);
            goto cmpuEnd;
        }
    }
    /* Burn Kpicv */
    if (pCmpuData->kpicvDataType  != ASSET_NO_KEY)  {
        CC_PROD_OTP_WRITE_VERIFY_WORD_BUFF(CC_OTP_KPICV_OFFSET, kpicvBuffForOtp, CC_OTP_KPICV_SIZE_IN_WORDS, error);
        if (error != CC_OK) {
            CC_PAL_LOG_ERR("Failed to verify OTP write 0x%x \n", error);
            goto cmpuEnd;
        }
    }
    /* Burn Kceicv*/
    if (pCmpuData->kceicvDataType  != ASSET_NO_KEY)  {
        CC_PROD_OTP_WRITE_VERIFY_WORD_BUFF(CC_OTP_KCEICV_OFFSET, kceicvBuffForOtp, CC_OTP_KCEICV_SIZE_IN_WORDS, error);
        if (error != CC_OK) {
            CC_PAL_LOG_ERR("Failed to verify OTP write 0x%x \n", error);
            goto cmpuEnd;
        }
    }
    /* Burn icv configuration word */
    CC_PROD_OTP_WRITE_VERIFY_WORD(CC_OTP_ICV_GENERAL_PURPOSE_FLAG_OFFSET,
                                  pCmpuData->icvConfigWord,
                                  error);
    if (error != CC_OK) {
        CC_PAL_LOG_ERR("Failed to verify OTP write 0x%x \n", error);
        goto cmpuEnd;
    }

cmpuEnd:
    CC_PalMemSetZero((uint8_t *)workspaceBaseAddr, workspaceSize);
    if (error != CC_OK) {
        CC_PAL_LOG_ERR("CMPU failed, error = 0x%x \n", error);
    }
    CCPROD_Fini();
    return error;
}



