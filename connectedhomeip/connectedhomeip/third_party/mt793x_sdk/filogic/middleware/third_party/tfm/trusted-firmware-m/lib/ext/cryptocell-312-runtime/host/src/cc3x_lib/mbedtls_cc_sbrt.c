/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#define CC_PAL_LOG_CUR_COMPONENT CC_LOG_MASK_SECURE_BOOT

/************* Include Files ****************/
#include "secureboot_stage_defs.h"
#include "bootimagesverifier_api.h"
#include "bootimagesverifier_error.h"
#include "bootimagesverifier_parser.h"
#include "secdebug_defs.h"


/************************ Defines ******************************/


/************************ Enums ******************************/


/************************ Typedefs ******************************/


/************************ Global Data ******************************/

/************************ Private functions  ******************************/

/************************ Public functions  ******************************/

CCError_t mbedtls_sb_cert_chain_cerification_init(CCSbCertInfo_t *certPkgInfo)
{
    return CC_SbCertChainVerificationInit(certPkgInfo);
}


CCError_t mbedtls_sb_cert_verify_single(CCSbFlashReadFunc flashReadFunc,
                                void *userContext,
                                CCAddr_t certStoreAddress,
                                CCSbCertInfo_t *pCertPkgInfo,
                                uint32_t *pHeader,     // used for X509 header
                                uint32_t  headerSize,
                                uint32_t *pWorkspace,
                                uint32_t workspaceSize)
{
        return CC_SbCertVerifySingle(flashReadFunc,
                      userContext,
                      0, /* hwBaseAddress NA for RT */
                      certStoreAddress,
                      pCertPkgInfo,
                      pHeader,
                      headerSize,
                      pWorkspace,
                      workspaceSize);
}


CCError_t mbedtls_sb_sw_image_store_address_change(uint32_t *pCert, uint32_t maxCertSizeWords, CCAddr_t address, uint32_t indexOfAddress)
{

    CCError_t error = CC_OK;
    uint32_t unsignedDataOffsetWords;
    uint32_t *pCurrRecAddInfo = NULL;

    /* Check inputs */
    if (pCert == NULL){
         CC_PAL_LOG_DEBUG("pCert is NULL\n");
         return CC_BOOT_IMG_VERIFIER_INV_INPUT_PARAM;
    }
    if (maxCertSizeWords == 0){
         CC_PAL_LOG_DEBUG("maxCertSizeWords is zero\n");
         return CC_BOOT_IMG_VERIFIER_INV_INPUT_PARAM;
    }

        /* Get certificate offset(in words)to unsigned data part */
        error = CCCertGetUnsignedDataOffset(pCert, &unsignedDataOffsetWords);
        if (error != CC_OK) {
            CC_PAL_LOG_ERR("Failed CCCertGetUnsignedDataOffset 0x%x\n", error);
            return CC_BOOT_IMG_VERIFIER_INV_INPUT_PARAM;
        }

    /* Check the buffer size boundaries (up to referred SW index) */
    if ( ((unsignedDataOffsetWords + (indexOfAddress+1)*SW_REC_NONE_SIGNED_DATA_SIZE_IN_WORDS) > maxCertSizeWords ) ||
         ((unsignedDataOffsetWords + (indexOfAddress+1)*SW_REC_NONE_SIGNED_DATA_SIZE_IN_WORDS) < unsignedDataOffsetWords) ) {
        return CC_BOOT_IMG_VERIFIER_INV_INPUT_PARAM;
    }

    /* Point to the relevant address and verify there is no wrap around in the memory */
    pCurrRecAddInfo = pCert + unsignedDataOffsetWords + indexOfAddress*SW_REC_NONE_SIGNED_DATA_SIZE_IN_WORDS;
    if (pCurrRecAddInfo < pCert){
        return CC_BOOT_IMG_VERIFIER_INV_INPUT_PARAM;
    }

    CC_PAL_LOG_DEBUG("current address is 0x%x, new address is 0x%x\n", (CCAddr_t)(*pCurrRecAddInfo), address);

    UTIL_MemCopy((uint8_t*)pCurrRecAddInfo, (uint8_t*)&address, sizeof(CCAddr_t));

    return CC_OK;
}






