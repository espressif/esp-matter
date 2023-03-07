/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#define CC_PAL_LOG_CUR_COMPONENT CC_LOG_MASK_SECURE_BOOT

/************* Include Files ****************/
#include "secureboot_basetypes.h"
#include "secureboot_error.h"
#include "bootimagesverifier_error.h"
#include "bootimagesverifier_def.h"
#include "bootimagesverifier_parser.h"
#include "secureboot_base_func.h"
#include "secureboot_base_swimgverify.h"
#include "cc_pal_log.h"
#include "secureboot_defs.h"
#include "bootimagesverifier_swcomp.h"
#include "common_cert_verify.h"
#include "common_cert_parser.h"

/************************ Defines ******************************/


/************************ Enums ******************************/


/************************ Typedefs ******************************/


/************************ Global Data ******************************/

/************************ Private functions  ******************************/

/************************ Public functions  ******************************/

CCError_t CC_SbCertChainVerificationInit(CCSbCertInfo_t *certPkgInfo)
{
        if (certPkgInfo == NULL) {
                return CC_BOOT_IMG_VERIFIER_INV_INPUT_PARAM;
        }
        /*clear the external hash */
        UTIL_MemSet((uint8_t *)&(certPkgInfo->pubKeyHash), 0x0, sizeof(certPkgInfo->pubKeyHash));
        certPkgInfo->initDataFlag = 0;


        return CC_OK;
}


/**
   @brief This function
   loadSbCert() loads the certificate from Flash to RAM
        if called first time, expected certificate is key certificate
        else, if second time, expected is key or content certificate
        else, if third, expected is content.
   Call CCCommonCertVerify(expected types) to verify common certificate fields,
        and returns pointers to certificate proprietary header, and body.
   If certificate type in proprietary header is key, call CCCommonKeyCertVerify(), to verify key certificate fields.
   Otherwise, call CCCommonContentCertVerify(), to verify Content certificate fields.
*/
CCError_t CC_SbCertVerifySingle(CCSbFlashReadFunc flashReadFunc,
                                void *userContext,
                                unsigned long hwBaseAddress,
                                CCAddr_t certStoreAddress,
                                CCSbCertInfo_t *pCertPkgInfo,
                                uint32_t *pHeader,     // used for X509 header
                                uint32_t  headerSize,
                                uint32_t *pWorkspace,
                                uint32_t workspaceSize)
{
        CCError_t   rc = CC_OK;
        uint32_t    certLoadWordSize;
        CertFieldsInfo_t  certFields;
        BufferInfo32_t  workspaceInfo;
        BufferInfo32_t  certInfo;
        BufferInfo32_t  x509HeaderInfo;
        BufferInfo32_t *pX509HeaderInfo = NULL;


        /* 1. Verify input parameters */
        /*----------------------------*/
        if ((flashReadFunc == NULL) ||
            (pCertPkgInfo == NULL) ||
            (pWorkspace == NULL) ||
            (workspaceSize == 0) ||
            ((unsigned long)pWorkspace + workspaceSize < (unsigned long)pWorkspace) ||   /* Verify no overflow in workspace */
            (workspaceSize < CC_SB_MIN_WORKSPACE_SIZE_IN_BYTES) ||
            (!IS_ALIGNED(workspaceSize, sizeof(uint32_t))) ||
            (!IS_ALIGNED(pWorkspace, sizeof(uint32_t)))) {
                CC_PAL_LOG_ERR("illegal params \n");
                return CC_BOOT_IMG_VERIFIER_INV_INPUT_PARAM;
        }
        if ((pHeader != NULL) && (headerSize != 0)) {
                x509HeaderInfo.pBuffer = pHeader;
                x509HeaderInfo.bufferSize = headerSize;
                pX509HeaderInfo = &x509HeaderInfo;
        }

        if (CC_SB_MAX_CERT_SIZE_IN_BYTES < CC_SB_MAX_CONTENT_PKG_SIZE_IN_BYTES) {
                CC_PAL_LOG_ERR("CC_SB_MAX_CERT_SIZE_IN_BYTES \n");
                return CC_BOOT_IMG_VERIFIER_INV_INPUT_PARAM;
        }

        UTIL_MemSet((uint8_t *)&certFields, 0, sizeof(CertFieldsInfo_t));
        /* Clearing the RAM just to verify that there is no secret data on it, before starting to process certificate */
        UTIL_MemSet((uint8_t *)pWorkspace, 0, workspaceSize);


        /* 2. Load the certificate from the Flash */
        /*----------------------------------------*/
        /* Set the maximum certificate size, and get back the current certificate size.
           The certificate to load is 32 bit aligned */
        certLoadWordSize = (CC_SB_MAX_CERT_SIZE_IN_BYTES / CC_32BIT_WORD_SIZE);
        rc = CCCertLoadCertificate(flashReadFunc,
                                   userContext,
                                   certStoreAddress,
                                   pWorkspace,
                                   &certLoadWordSize);
        if (rc != CC_OK) {
                CC_PAL_LOG_ERR("CCCertParserLoadCertificate  returned 0x%X\n", (unsigned int)rc);
                return rc;
        }

        /* workspace order:
           [0]  certificate
            [certificate size]   if content certificate - additional data for images and addresses
           [end of workspace]   N+Np+Signature  OR images information */
        certInfo.pBuffer =  pWorkspace;

        /* Set expected certificate type according to the certificate place in chain - first key ,
           second key or content, third content. Maximal size in content certificate is calculated according to
           MAX number of possible SW images.*/
        switch (pCertPkgInfo->initDataFlag) {
        case CC_SB_FIRST_CERT_IN_CHAIN:
                certFields.certType = CC_SB_KEY_CERT;
                certFields.certBodySize = sizeof(KeyCertMain_t);
                certInfo.bufferSize = CC_SB_MAX_KEY_CERT_SIZE_IN_BYTES;
                break;
        case CC_SB_SECOND_CERT_IN_CHAIN:
                certFields.certType = CC_SB_KEY_OR_CONTENT_CERT;
                certFields.certBodySize = sizeof(ContentCertMain_t);
                certInfo.bufferSize = CC_SB_MAX_CONTENT_CERT_SIZE_IN_BYTES;
                break;
        case CC_SB_THIRD_CERT_IN_CHAIN:
                certFields.certType = CC_SB_CONTENT_CERT;
                certFields.certBodySize = sizeof(ContentCertMain_t);
                certInfo.bufferSize = CC_SB_MAX_CONTENT_CERT_SIZE_IN_BYTES;
                break;
        default:
                CC_PAL_LOG_ERR("Not expecting any certificate in the chain \n");
                return CC_BOOT_IMG_VERIFIER_INV_INPUT_PARAM;
        }

        // set the workspace for N, Np and signature
        workspaceInfo.bufferSize = sizeof(workspaceInt_t);
        workspaceInfo.pBuffer = (uint32_t *)((unsigned long)pWorkspace + workspaceSize - sizeof(workspaceInt_t));

        /* 3. Verify the certificate (Verify the RSA signature and the public key hash) . */
        rc = CCCommonCertVerify(hwBaseAddress,
                                &certInfo,
                                &certFields,
                                pCertPkgInfo,
                                &workspaceInfo,
                                pX509HeaderInfo);
        if (rc != CC_OK) {
                CC_PAL_LOG_ERR("CCCommonCertVerify failed 0x%X\n", rc);
                return rc;
        }


        /* 4. In case of content certificate - verify the SW images */
        /*----------------------------------------------------------*/
        switch (certFields.certType) {
        case CC_SB_KEY_CERT:
                /* Verify the key certificate sw version. */
                rc = CCCommonKeyCertVerify(hwBaseAddress,
                                           certFields.certHeader.certFlags,
                                           certFields.pCertBody,
                                           pCertPkgInfo);
                /* Update the certificate number in the chain.*/
                pCertPkgInfo->initDataFlag++;
                break;
        case CC_SB_CONTENT_CERT:
                /* Verify the content certificate sw version and verify the SW images. If needed update the sw version.
                  workspaceInfo may overlap with N+Np+Signature, since N, Np and signature were already verified */
                workspaceInfo.pBuffer = pWorkspace + certLoadWordSize;
                workspaceInfo.bufferSize = workspaceSize - certLoadWordSize * CC_32BIT_WORD_SIZE;
                rc = CCCommonContentCertVerify(flashReadFunc,
                                               userContext,
                                               hwBaseAddress,
                                               certStoreAddress + certLoadWordSize * CC_32BIT_WORD_SIZE,
                                               pCertPkgInfo,
                                               certFields.certHeader.certFlags,
                                               certFields.pCertBody,
                                               &workspaceInfo);
                /* the content certificate is always the last. */
                pCertPkgInfo->initDataFlag = CC_SB_LAST_CERT_IN_CHAIN;
                break;
        default:
                CC_PAL_LOG_ERR("Illegal certificate type for secure boot flow.\n");
                return CC_BOOT_IMG_VERIFIER_INV_INPUT_PARAM;
        }

        return rc;

} /* End of CC_SbCertVerifySingle */



