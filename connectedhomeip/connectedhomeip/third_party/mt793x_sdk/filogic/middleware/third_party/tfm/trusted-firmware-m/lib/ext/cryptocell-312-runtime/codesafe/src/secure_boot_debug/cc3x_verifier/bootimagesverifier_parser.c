/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#define CC_PAL_LOG_CUR_COMPONENT CC_LOG_MASK_SECURE_BOOT

/************* Include Files ****************/

#include "secureboot_error.h"
#include "secureboot_basetypes.h"
#include "secureboot_defs.h"
#include "rsa_bsv.h"
#include "bootimagesverifier_def.h"
#include "bootimagesverifier_error.h"
#include "bootimagesverifier_parser.h"
#include "cc_pal_log.h"


/************************ Defines ******************************/

/************************ Enums ******************************/

/************************ Typedefs ******************************/

/************************ Global Data ******************************/

/************************ Internal Functions ******************************/

/************************ Public Functions ******************************/


/**
   @brief This function load sizeof(CCSbCertHeader_t ) from flash and get the
   certificate size from it. Make sure size is within range (smaller than workspace
   size not including the required space for N, Np and signature).
   read the certificate according to size from header and copy the certificate content from Flash to RAM.
 */
uint32_t CCCertLoadCertificate(CCSbFlashReadFunc flashRead_func,
                               void *userContext,
                               CCAddr_t certAddress,
                               uint32_t *pCert,
                               uint32_t *pCertBufferWordSize)
{
        CCError_t error = CC_OK;
        CCSbCertHeader_t *pCertHeader = (CCSbCertHeader_t *)pCert;

        /* Verify that the certificate buffer size is big enough to contain the header */
        if (*pCertBufferWordSize < (sizeof(CCSbCertHeader_t) / CC_32BIT_WORD_SIZE)) {
                CC_PAL_LOG_ERR("certificate buff size too small to contain certificate header\n");
                return CC_BOOT_IMG_VERIFIER_WORKSPACE_SIZE_TOO_SMALL;
        }

        /* Read the certificate header from the Flash */
        error = flashRead_func(certAddress, (uint8_t *)pCertHeader, sizeof(CCSbCertHeader_t), userContext);
        if (error != CC_OK) {
                CC_PAL_LOG_ERR("failed flashRead_func for certificate header\n");
                return error;
        }

        /* Verify there is no wrap around in the certificate size*/
        if ((pCertHeader->certSize + SB_CERT_RSA_KEY_SIZE_IN_WORDS) < pCertHeader->certSize){
            CC_PAL_LOG_ERR("Certificate size too big\n");
            return CC_BOOT_IMG_VERIFIER_INV_INPUT_PARAM;
        }

        /* Make sure certificate size is within range (certificate size + signature size) */
        if ((pCertHeader->certSize + SB_CERT_RSA_KEY_SIZE_IN_WORDS) > *pCertBufferWordSize) {
                CC_PAL_LOG_ERR("Certificate size too big\n");
                return CC_BOOT_IMG_VERIFIER_INV_INPUT_PARAM;
        }

        /* according to the header read the additional certificate buffer -
           not including the non-signed part in case of content certificate */
        error = flashRead_func(certAddress + sizeof(CCSbCertHeader_t),
                               (uint8_t *)pCert + sizeof(CCSbCertHeader_t),
                               ((pCertHeader->certSize + SB_CERT_RSA_KEY_SIZE_IN_WORDS) * CC_32BIT_WORD_SIZE) - sizeof(CCSbCertHeader_t),
                               userContext);
        if (error != CC_OK) {
                CC_PAL_LOG_ERR("failed flashRead_func for certificate\n");
                return error;
        }

        *pCertBufferWordSize = (pCertHeader->certSize + SB_CERT_RSA_KEY_SIZE_IN_WORDS);

        return CC_OK;

} /* End of CCCertLoadCertificate */


/****************************************************************************************************/
