/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "cc_dmpu.h"
#include "cc_otp_defs.h"
#include "cc_pal_mem.h"
#include "dx_crys_kernel.h"
#include "prod_hw_defs.h"
#include "cc_pal_log.h"
#include "prod_util.h"
#include "cc_prod_error.h"
#include "prod_crypto_driver.h"
#include "mbedtls_cc_mng_int.h"
#include "mbedtls_cc_mng.h"

static uint32_t CC_PROD_HandleKcp(CCDmpuData_t *pDmpuData,
                                    uint32_t *pBuffForOtp,
                                    uint32_t *pManufactorWord,
                               unsigned long workspaceAddr,
                            uint32_t     workspaceSize)
{
        uint32_t error = 0;
        uint32_t  zeroCount = 0;

        switch (pDmpuData->kcpDataType) {
        case  ASSET_NO_KEY:
                SET_OTP_OEM_FLAG(*pManufactorWord, KCP_NOT_IN_USE, 1);
                goto end_kcp;
        case  ASSET_PKG_KEY:
                error = CC_PROD_PkgVerify((CCProdAssetPkg_t *)&pDmpuData->kcp.pkgAsset,
                                           (const uint8_t *)PROD_OEM_PROV_CONTEXT, PROD_KPROV_CONTEXT_SIZE,
                                           (const uint8_t *)PROD_OEM_KEY_TMP_LABEL, PROD_KEY_TMP_LABEL_SIZE,
                                          (uint8_t *)pDmpuData->hbkBuff.hbk, CC_OTP_HBK1_SIZE_IN_WORDS * CC_32BIT_WORD_SIZE,
                                           (uint8_t *  )pBuffForOtp,
                                           workspaceAddr,
                                           workspaceSize);
                if (error != CC_OK) {
                        CC_PAL_LOG_ERR("failed to CC_PROD_PkgVerify fro Kcp\n");
                        return error;
                }
                break;
        case ASSET_PLAIN_KEY:
                CC_PalMemCopy(pBuffForOtp, pDmpuData->kcp.plainAsset, PROD_ASSET_SIZE);
                break;
        default:
                CC_PAL_LOG_ERR("Inavlid key type for  Kcp\n");
                return CC_PROD_INVALID_PARAM_ERR;
        }
        error = CC_PROD_GetZeroCount(pBuffForOtp, PROD_KEY_TMP_CONTEXT_WORD_SIZE, &zeroCount);
        if (error != CC_OK) {
                CC_PAL_LOG_ERR("Invalid zero count for Kcp\n");
                return error;
        }
end_kcp:
        SET_OTP_OEM_FLAG(*pManufactorWord, KCP_ZERO_BITS, zeroCount);
        return CC_OK;
}


static uint32_t CC_PROD_HandleKce(CCDmpuData_t *pDmpuData,
                                     uint32_t *pBuffForOtp,
                                     uint32_t *pManufactorWord,
                               unsigned long workspaceAddr,
                            uint32_t     workspaceSize)
{
        uint32_t error = 0;
        uint32_t  zeroCount = 0;

        switch (pDmpuData->kceDataType) {
        case  ASSET_NO_KEY:
                SET_OTP_OEM_FLAG(*pManufactorWord, KCE_NOT_IN_USE, 1);
                goto end_kce;
        case  ASSET_PKG_KEY:
                error = CC_PROD_PkgVerify((CCProdAssetPkg_t *)&pDmpuData->kce.pkgAsset,
                                           (const uint8_t *)PROD_OEM_ENC_CONTEXT, PROD_KPROV_CONTEXT_SIZE,
                                           (const uint8_t *)PROD_OEM_KEY_TMP_LABEL, PROD_KEY_TMP_LABEL_SIZE,
                                          (uint8_t *)pDmpuData->hbkBuff.hbk, CC_OTP_HBK1_SIZE_IN_WORDS * CC_32BIT_WORD_SIZE,
                                           (uint8_t *  )pBuffForOtp,
                                           workspaceAddr,
                                           workspaceSize);
                if (error != CC_OK) {
                        CC_PAL_LOG_ERR("failed to CC_PROD_PkgVerify fro Kce\n");
                        return error;
                }
                break;
        case ASSET_PLAIN_KEY:
                CC_PalMemCopy(pBuffForOtp, pDmpuData->kce.plainAsset, PROD_ASSET_SIZE);
                break;
        default:
                CC_PAL_LOG_ERR("Invalid key type for Kce\n");
                return CC_PROD_INVALID_PARAM_ERR;
        }

        error = CC_PROD_GetZeroCount(pBuffForOtp, PROD_KEY_TMP_CONTEXT_WORD_SIZE, &zeroCount);
        if (error != CC_OK) {
                CC_PAL_LOG_ERR("Invalid zerocount for Kce\n");
                return error;
        }

end_kce:
        SET_OTP_OEM_FLAG(*pManufactorWord, KCE_ZERO_BITS, zeroCount);
        return CC_OK;
}



CCError_t CCProd_Dmpu( unsigned long             ccHwRegBaseAddr,
    CCDmpuData_t *pDmpuData,
                     unsigned long             workspaceBaseAddr,
                     uint32_t                 workspaceSize)
{
        uint32_t error = 0;
        uint32_t lcs = 0;
        uint32_t manufactorWord = 0;
        uint32_t icvWord = 0;
        uint32_t  hbkSizeInWords = 0;
        uint32_t  hbkOtpWordOffset = 0;
        uint32_t  swVerSizeInWords = 0;
        uint32_t  swVerOtpWordOffset = 0;
        uint32_t  zeroCount = 0;
        uint32_t  i = 0;
        uint32_t icvOwnership = 0;
        uint32_t hbkBuffForOtp[CC_OTP_HBK_SIZE_IN_WORDS] = { 0 };
        uint32_t kcpBuffForOtp[CC_OTP_KCP_SIZE_IN_WORDS] = { 0 };
        uint32_t kceBuffForOtp[CC_OTP_KCE_SIZE_IN_WORDS] = { 0 };
        uint32_t dcuLockBitsBuffForOtp[CC_OTP_DCU_SIZE_IN_WORDS] = { 0 };
        uint32_t   oemMinSwVersion[CC_OTP_HBK_MIN_VERSION_SIZE_IN_WORDS] = { 0 };

        if (sizeof(CCProdAssetPkg_t) != PROD_ASSET_PKG_SIZE) {
                CC_PAL_LOG_ERR("invalid Pkg size\n");
                return CC_PROD_INVALID_PARAM_ERR;
        }
        if ((pDmpuData == NULL) ||
            (workspaceBaseAddr == 0) ||  //can not be zero because it is cast to a pointer later
            (workspaceBaseAddr % CC_32BIT_WORD_SIZE) ||   // workspace address must be word aligned
            (workspaceSize < DMPU_WORKSPACE_MINIMUM_SIZE)) {
                CC_PAL_LOG_ERR("invalid params\n");
                return CC_PROD_INVALID_PARAM_ERR;
        }

        gCcRegBase = ccHwRegBaseAddr;

        error = CCProd_Init();
        if (error != CC_OK) {
                CC_PAL_LOG_ERR("Failed to CCProd_Init 0x%x\n", error);
                goto dmpuEnd;
        }

        /* Check LCS - DM only */
        error = mbedtls_mng_lcsGet(&lcs);
        if (error != CC_OK) {
                CC_PAL_LOG_ERR("Failed to get LCS 0x%x \n", error);
                goto dmpuEnd;
        }

        /* Verify LCS is DM */
        if (lcs != CC_MNG_LCS_DM) {
                CC_PAL_LOG_ERR("LCS is %d not valid\n", lcs);
                error = CC_PROD_ILLEGAL_LCS_ERR;
                goto dmpuEnd;
        }

        /**************** Preparing and validating teh OEM assets  ******************/
        /* First, read ICV  manufactor word to see if HBK0 exists */
       CC_PROD_OTP_READ(icvWord, CC_OTP_MANUFACTURE_FLAG_OFFSET);
        if (error != CC_OK) {
                CC_PAL_LOG_ERR("Failed to verify OTP write 0x%x \n", error);
                goto dmpuEnd;
        }

        switch (pDmpuData->hbkType) {
        case DMPU_HBK_TYPE_HBK1:
                if (!IS_HBK0_USED(icvWord)) {
                        CC_PAL_LOG_ERR("HBK type is not valid\n");
                        error = CC_PROD_ASSET_PKG_PARAM_ERR;
                        goto dmpuEnd;
                }
                hbkSizeInWords = CC_OTP_HBK1_SIZE_IN_WORDS;
                hbkOtpWordOffset = CC_OTP_HBK1_OFFSET;
                swVerOtpWordOffset = CC_OTP_HBK1_MIN_VERSION_OFFSET;
                swVerSizeInWords = CC_OTP_HBK1_MIN_VERSION_SIZE_IN_WORDS;
                break;
        case DMPU_HBK_TYPE_HBK:
                if (IS_HBK0_USED(icvWord)) {
                        CC_PAL_LOG_ERR("HBK type is not valid\n");
                        error = CC_PROD_ASSET_PKG_PARAM_ERR;
                        goto dmpuEnd;
                }
                hbkSizeInWords = CC_OTP_HBK_SIZE_IN_WORDS;
                hbkOtpWordOffset = CC_OTP_HBK_OFFSET;
                swVerOtpWordOffset = CC_OTP_HBK_MIN_VERSION_OFFSET;
                swVerSizeInWords = CC_OTP_HBK_MIN_VERSION_SIZE_IN_WORDS;
                break;
        default:
                CC_PAL_LOG_ERR("Invalid HBK type\n");
                error = CC_PROD_ASSET_PKG_PARAM_ERR;
                goto dmpuEnd;
        }

        /* Set HBK/1 buffer */
        CC_PalMemCopy(hbkBuffForOtp, pDmpuData->hbkBuff.hbk, hbkSizeInWords * CC_32BIT_WORD_SIZE);
        error = CC_PROD_GetZeroCount(hbkBuffForOtp, hbkSizeInWords, &zeroCount);
        if (error != CC_OK) {
                CC_PAL_LOG_ERR("Invalid Huk zero count\n");
                goto dmpuEnd;
        }

        if (pDmpuData->hbkType == DMPU_HBK_TYPE_HBK) {
                SET_OTP_OEM_FLAG(manufactorWord, HBK_ZERO_BITS, zeroCount);
        } else {  // type is DMPU_HBK_TYPE_HBK1
                SET_OTP_OEM_FLAG(manufactorWord, HBK1_ZERO_BITS, zeroCount);
        }
        /* Set OEM minimum SW version according to HBK/1*/
        error = CC_PROD_BitListFromNum(oemMinSwVersion, swVerSizeInWords, pDmpuData->oemMinVersion);
        if (error != CC_OK) {
                CC_PAL_LOG_ERR("Failed to get bit list from number\n");
                goto dmpuEnd;
        }

        /* Handle Kcp, or set Kcp not in use */
        error = CC_PROD_HandleKcp(pDmpuData, kcpBuffForOtp, &manufactorWord, workspaceBaseAddr, workspaceSize);
        if (error != CC_OK) {
                CC_PAL_LOG_ERR("failed to CC_PROD_HandleKcp 0x%x \n", error);
                goto dmpuEnd;
        }


        /* Handle Kce, , or set Kce not in use */
        error = CC_PROD_HandleKce(pDmpuData, kceBuffForOtp, &manufactorWord, workspaceBaseAddr, workspaceSize);
        if (error != CC_OK) {
                CC_PAL_LOG_ERR("failed to CC_PROD_HandleKce 0x%x \n", error);
                goto dmpuEnd;
        }

        /* Keep only OEM bit in DCU default locking */
        for (i = 0; i < CC_OTP_DCU_SIZE_IN_WORDS; i++) {
                icvOwnership = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, AO_ICV_DCU_RESTRICTION_MASK0) + (i * CC_32BIT_WORD_SIZE));
                dcuLockBitsBuffForOtp[i] = pDmpuData->oemDcuDefaultLock[i] & (~icvOwnership);
        }

        /**************** Burning OTP  ******************/
        /* First, Burn Manufactor flag, including not in use flags set by ahndle functions above  */
        CC_PROD_OTP_WRITE_VERIFY_WORD(CC_OTP_OEM_FLAG_OFFSET, manufactorWord, error);
        if (error != CC_OK) {
                CC_PAL_LOG_ERR("Failed to verify OTP write 0x%x \n", error);
                goto dmpuEnd;
        }

        /* Burn HBK or HBK1 */
        CC_PROD_OTP_WRITE_VERIFY_WORD_BUFF(hbkOtpWordOffset, hbkBuffForOtp, hbkSizeInWords, error);
        if (error != CC_OK) {
                CC_PAL_LOG_ERR("Failed to verify OTP write 0x%x \n", error);
                goto dmpuEnd;
        }
        /* DCU lock bits */
        CC_PROD_OTP_WRITE_VERIFY_WORD_BUFF(CC_OTP_DCU_OFFSET, dcuLockBitsBuffForOtp, CC_OTP_DCU_SIZE_IN_WORDS, error);
        if (error != CC_OK) {
                CC_PAL_LOG_ERR("Failed to verify OTP write 0x%x \n", error);
                goto dmpuEnd;
        }
        /* SW version */
        CC_PROD_OTP_WRITE_VERIFY_WORD_BUFF(swVerOtpWordOffset, oemMinSwVersion, swVerSizeInWords, error);
        if (error != CC_OK) {
                CC_PAL_LOG_ERR("Failed to verify OTP write 0x%x \n", error);
                goto dmpuEnd;
        }

        /* Burn Kcp */
        if (pDmpuData->kcpDataType  != ASSET_NO_KEY)  {
                CC_PROD_OTP_WRITE_VERIFY_WORD_BUFF(CC_OTP_KCP_OFFSET, kcpBuffForOtp, CC_OTP_KCP_SIZE_IN_WORDS, error);
                if (error != CC_OK) {
                        CC_PAL_LOG_ERR("Failed to verify OTP write 0x%x \n", error);
                        goto dmpuEnd;
                }
        }
        /* Burn Kce*/
        if (pDmpuData->kceDataType  != ASSET_NO_KEY)  {
                CC_PROD_OTP_WRITE_VERIFY_WORD_BUFF(CC_OTP_KCE_OFFSET, kceBuffForOtp, CC_OTP_KCE_SIZE_IN_WORDS, error);
                if (error != CC_OK) {
                        CC_PAL_LOG_ERR("Failed to verify OTP write 0x%x \n", error);
                        goto dmpuEnd;
                }
        }

dmpuEnd:
        if (error != CC_OK) {
                CC_PAL_LOG_ERR("DMPU failed, error = 0x%x \n", error);
        }
        CCPROD_Fini();
        return error;
}



