/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#define CC_PAL_LOG_CUR_COMPONENT CC_LOG_MASK_SECURE_BOOT

/************* Include Files ****************/
#include "secureboot_basetypes.h"
#include "rsa_bsv.h"
#include "bootimagesverifier_def.h"
#include "secureboot_error.h"
#include "bootimagesverifier_error.h"
#include "bootimagesverifier_parser.h"
#include "secureboot_stage_defs.h"
#include "nvm.h"
#include "bootimagesverifier_swcomp.h"
#include "secureboot_base_swimgverify.h"
#include "cc_pal_log.h"


/************************ Defines ******************************/

/************************ Enums ******************************/

/************************ Typedefs ******************************/

/************************ Global Data ******************************/

/************************ Internal Functions ******************************/

/************************ Public Functions ******************************/

CCError_t CCCertValidateSWComps(CCSbFlashReadFunc flashRead_func,
                  void *userContext,
                  unsigned long hwBaseAddress,
                  CCSbPubKeyIndexType_t keyIndex,
                  CCSbCertParserSwCompsInfo_t *pSwImagesData,
                  uint32_t *pSwImagesAddData,
                  uint32_t *workspace_ptr,
                  uint32_t workspaceSize)
{
    /* error variable */
    CCError_t error = CC_OK;

    /* internal index */
    uint32_t i = 0;

    /* internal pointer for the certificate main body, might not be word aligned */
    uint8_t *pSwRecSignedData = NULL;
    /* the non-signed part is always word aligned */
    uint32_t *pSwRecNoneSignedData = NULL;

    /* AES IV buffer */
    AES_Iv_t AESIv;
    uint8_t *nonce;
    CCswCodeEncType_t swCodeEncType;
    CCBsvKeyType_t  keyType;
    bsvCryptoMode_t cryptoMode;
    CCswCryptoType_t swCryptoType;
    CCswLoadVerifyScheme_t swLoadVerifyScheme;

    uint32_t lcs;

    uint8_t isLoadFromFlash;
    uint8_t isVerifyImage;
        ContentCertImageRecord_t cntImageRec;

    /*------------------
        CODE
    -------------------*/

    /* Point to the s/w record signed data: hash, load address, max size, code enc */
    pSwRecSignedData = pSwImagesData->pSwCompsData;

    /* Point to the s/w record non-signed data: storage address, actual size */
    pSwRecNoneSignedData = pSwImagesAddData;

    nonce = pSwImagesData->nonce;
    swCodeEncType = pSwImagesData->swCodeEncType;
    swCryptoType = pSwImagesData->swCryptoType;
    swLoadVerifyScheme = pSwImagesData->swLoadVerifyScheme;

    /* Set default CC mode to Hash only (no encrypted images) */
    cryptoMode = BSV_CRYPTO_HASH;
    keyType = CC_BSV_END_OF_KEY_TYPE;

    /* Get LCS */
    error = NVM_GetLCS(hwBaseAddress, &lcs);
    if (error != CC_OK){
        return error;
    }

    switch(swLoadVerifyScheme){
    case CC_SB_LOAD_AND_VERIFY:
        isLoadFromFlash = CC_TRUE;
        isVerifyImage = CC_TRUE;
        break;
    case CC_SB_VERIFY_ONLY_IN_FLASH:
        isLoadFromFlash = CC_TRUE;
        isVerifyImage = CC_TRUE;
        break;
    case CC_SB_VERIFY_ONLY_IN_MEM:
        isLoadFromFlash = CC_FALSE;
        isVerifyImage = CC_TRUE;
        break;
    case CC_SB_LOAD_ONLY:
        isLoadFromFlash = CC_TRUE;
        isVerifyImage = CC_FALSE;
        /* Loading only is validate only in none secure lifecycle */
        if (lcs == CC_BSV_SECURE_LCS) {
            return CC_BOOT_IMG_VERIFIER_ILLEGAL_LCS_FOR_OPERATION_ERR;
        }
        break;
    default:
        return CC_BOOT_IMG_VERIFIER_INV_INPUT_PARAM;
    }

    /* Set AES key type */
    switch (swCodeEncType){
    case CC_SB_NO_IMAGE_ENCRYPTION:
        break;
    case CC_SB_ICV_CODE_ENCRYPTION:
        keyType = CC_BSV_ICV_CE_KEY;
        if ((keyIndex!=CC_SB_HASH_BOOT_KEY_0_128B) && (keyIndex!=CC_SB_HASH_BOOT_KEY_256B)){
            return CC_BOOT_IMG_VERIFIER_INV_INPUT_PARAM;
        }
        break;
    case CC_SB_OEM_CODE_ENCRYPTION:
        keyType = CC_BSV_CE_KEY;
        if ((keyIndex!=CC_SB_HASH_BOOT_KEY_1_128B) && (keyIndex!=CC_SB_HASH_BOOT_KEY_256B)){
            return CC_BOOT_IMG_VERIFIER_INV_INPUT_PARAM;
        }
        break;
    default:
        return CC_BOOT_IMG_VERIFIER_INV_INPUT_PARAM;
    }

    /* Case of encrypted SW image */
    if (swCodeEncType != CC_SB_NO_IMAGE_ENCRYPTION) {

        /* SB should fail if CE is needed in RMA lcs */
        if (lcs == CC_BSV_RMA_LCS) {
            return CC_BOOT_IMG_VERIFIER_ILLEGAL_LCS_FOR_OPERATION_ERR;
        }

        /* image can not be encrypted in case of "load only" or "verify in flash" */
        if ( (swLoadVerifyScheme == CC_SB_LOAD_ONLY) || (swLoadVerifyScheme == CC_SB_VERIFY_ONLY_IN_FLASH) ) {
            return CC_BOOT_IMG_VERIFIER_INV_INPUT_PARAM;
        }

        /* Set crypto mode */
        switch (swCryptoType){
        case CC_SB_HASH_ON_DECRYPTED_IMAGE:
            /* do AES decrypt on cipher image, and then do hash is done on plain image */
            cryptoMode = BSV_CRYPTO_AES_TO_HASH_AND_DOUT;
            break;
        case CC_SB_HASH_ON_ENCRYPTED_IMAGE:
            /* do AES decrypt and Hash on cipher image */
            cryptoMode = BSV_CRYPTO_AES_AND_HASH;
            break;
        default:
            return CC_BOOT_IMG_VERIFIER_INV_INPUT_PARAM;
        }

        /* Initiate AES IV with nonce data */
        UTIL_MemSet((uint8_t*)AESIv, 0, AES_IV_COUNTER_SIZE_IN_BYTES);
        UTIL_MemCopy((uint8_t*)&AESIv[0], nonce, CC_SB_MAX_SIZE_NONCE_BYTES);
    }

    /* Load and verify all images in the certificate */
    /*-----------------------------------------------*/
    for (i = 0; i < pSwImagesData->numOfSwComps; i++ ) {

        /* In case of encrypted image, set AES IV CTR */
        if ((isVerifyImage == CC_TRUE) && (keyType != CC_BSV_END_OF_KEY_TYPE)) {
            UTIL_MemCopy((uint8_t *)&cntImageRec, (uint8_t *)pSwRecSignedData, sizeof(ContentCertImageRecord_t));
#ifdef BIG__ENDIAN
            UTIL_MemCopy((uint8_t*)&AESIv[2], (uint8_t*)&cntImageRec.loadAddr, sizeof(CCAddr_t));
#else
            UTIL_ReverseMemCopy((uint8_t*)&AESIv[2], (uint8_t*)&cntImageRec.loadAddr, sizeof(CCAddr_t));
#endif
        }

        /* Load and/or verify image as needed */
        error = _CCSbImageLoadAndVerify(flashRead_func, userContext,/* Flash Read function */
                             hwBaseAddress,         /* CC base address */
                             isLoadFromFlash,       /* should image be copied from Flash with user callback */
                             isVerifyImage,         /* should image be verified with hash (and Aes if needed) */
                             cryptoMode,            /* crypto mode type */
                             keyType,           /* code encryption type definition */
                             AESIv,             /* AES IV buffer */
                             pSwRecSignedData,      /* pointer to SW component signed data - not word aligned for x.509 */
                             pSwRecNoneSignedData,  /* pointer to SW components non-signed data. always word aligned */
                             workspace_ptr, workspaceSize); /* workspace & workspaceSize to load the SW component into */

        if (error != CC_OK){
            return error;
        }

        /* Point to the next SW record */
        pSwRecSignedData = (uint8_t *)((unsigned long)pSwRecSignedData + SW_REC_SIGNED_DATA_SIZE_IN_BYTES);
        pSwRecNoneSignedData = (uint32_t *)((unsigned long)pSwRecNoneSignedData + SW_REC_NONE_SIGNED_DATA_SIZE_IN_BYTES);

    }

    return CC_OK;
}

