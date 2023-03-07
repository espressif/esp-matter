/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#define CC_PAL_LOG_CUR_COMPONENT CC_LOG_MASK_SECURE_BOOT

/************* Include Files ****************/


#include "secureboot_error.h"
#include "secureboot_basetypes.h"
#include "secureboot_stage_defs.h"
#include "secureboot_gen_defs.h"
#include "secureboot_defs.h"
#include "bootimagesverifier_error.h"
#include "nvm_otp.h"
#include "cc_pal_log.h"
#include "secureboot_stage_defs.h"
#include "secdebug_defs.h"

/************************ Defines ******************************/

/************************ Enums ******************************/

/************************ Typedefs ******************************/

/************************ Global Data ******************************/

/************************ Internal Functions ******************************/

/************************ Public Functions ******************************/

CCError_t CCSbImageLoadAndVerify(CCSbFlashReadFunc preHashflashRead_func,
                       void *preHashUserContext,
                       unsigned long hwBaseAddress,
                       uint8_t isLoadFromFlash,
                       uint8_t isVerifyImage,
                       bsvCryptoMode_t cryptoMode,
                       CCBsvKeyType_t  keyType,
                       AES_Iv_t AESIv,
                       uint8_t *pSwRecSignedData,
                       uint32_t *pSwRecNonSignedData,
                       uint32_t *workspace_ptr,
                       uint32_t workspaceSize)
{
    /* error variable */
    CCError_t error = CC_OK;

        ContentCertImageRecord_t cntImageRec;
        CCSbSwImgAddData_t  cntNonSignedImageRec;
        CCHashResult_t      actImageHash;
    CCAddr_t currLoadStartAddress = 0;
    uint32_t chunkSizeInBytes = 0;
    uint32_t actualImageSize = 0;

    /* Use user workspace in double buffer manner */
    uint32_t *workRam1=NULL, *workRam2=NULL;
    uint8_t isToggle = CC_FALSE, isLoadIV = CC_TRUE;

    /*------------------
        CODE
    -------------------*/

    /* In order to improve performance the Loading from Flash will be done simultaneously wit Hash calculation */

    /* Initialize parameters */
        UTIL_MemCopy((uint8_t *)&cntImageRec, (uint8_t *)pSwRecSignedData, sizeof(ContentCertImageRecord_t));
    /* The non-signed is word aligned, so we can cast the pointer */
        UTIL_MemCopy((uint8_t *)&cntNonSignedImageRec, (uint8_t *)pSwRecNonSignedData, sizeof(CCSbSwImgAddData_t));

        actualImageSize = cntNonSignedImageRec.Len;
    currLoadStartAddress = cntImageRec.loadAddr;

        if (cntImageRec.isAesCodeEncUsed == 0){
        /* overwrite crypto mode to hash only */
        cryptoMode = BSV_CRYPTO_HASH;
        keyType = CC_BSV_END_OF_KEY_TYPE;
    } else {
        /* verify crypto mode and key are set for aes */
        if( (cryptoMode == BSV_CRYPTO_HASH) || (keyType == CC_BSV_END_OF_KEY_TYPE) ){
            CC_PAL_LOG_ERR("AES operation is not configuraed correctly\n");
            return CC_BOOT_IMG_VERIFIER_CERT_DECODING_ILLEGAL;
        }
    }

    /* Validate image size */
    if ((cntNonSignedImageRec.Len == 0) ||
            (cntNonSignedImageRec.Len > cntImageRec.imageMaxSize)) {
        CC_PAL_LOG_ERR("SW image size is illegal !\n");
        return CC_BOOT_IMG_VERIFIER_SW_COMP_SIZE_IS_NULL;
    }

    /* Set chunk size for read and process data (fixed according to user workspace or HW limitation) */
    chunkSizeInBytes = CC_DOUBLE_BUFFER_MAX_SIZE_IN_BYTES / 2;

    /* In case of no loading address, we use user workspace in double buffer mode */
    if (cntImageRec.loadAddr == CC_SW_COMP_NO_MEM_LOAD_INDICATION) {

        isToggle = CC_TRUE;

        /* The workspace minimum size must be at least CC_DOUBLE_BUFFER_MAX_SIZE_IN_BYTES,
           if its not the function will return error (if temp memory should be used) */
        if (workspaceSize < CC_DOUBLE_BUFFER_MAX_SIZE_IN_BYTES){

            CC_PAL_LOG_ERR("workspace size too small\n");
            return CC_BOOT_IMG_VERIFIER_WORKSPACE_SIZE_TOO_SMALL;
        }

        /* Divide the workspace into 2 buffers, in order to allow reading and calculating HASH
         simultaneously , each buffer size is CC_DOUBLE_BUFFER_MAX_SIZE_IN_BYTES/2 */
        workRam1 = workspace_ptr; /* Size of this section is CC_DOUBLE_BUFFER_MAX_SIZE_IN_BYTES/2 */
        workRam2 = workspace_ptr + (CC_DOUBLE_BUFFER_MAX_SIZE_IN_BYTES/2)/sizeof(uint32_t);

        /* Starting with the first buffer */
        cntImageRec.loadAddr = CONVERT_TO_ADDR(workRam1);
    }

    if (isVerifyImage == CC_TRUE){

        /* initialize the AES and HASH */
        error = BsvCryptoImageInit(hwBaseAddress, cryptoMode, keyType);
        if (error != CC_OK) {
                            CC_PAL_LOG_ERR("BsvCryptoImageInit failed 0x%xl !\n", error);
                return error;
        }
    }

    /* Load and/or Verify image in chunks */
    /*------------------------------------*/
    while (cntNonSignedImageRec.Len > 0) {

        /* Set number of bytes to load and/or verify */
        chunkSizeInBytes = min(chunkSizeInBytes, cntNonSignedImageRec.Len);

        /* Copy data from the flash to memory with user callback */
        if ( isLoadFromFlash == CC_TRUE) {
            error = preHashflashRead_func(cntNonSignedImageRec.StoreAddr, (uint8_t*)(CONVERT_TO_ADDR(cntImageRec.loadAddr)),
                                            chunkSizeInBytes, preHashUserContext);
            if (error != CC_OK) {
                                CC_PAL_LOG_ERR("preHashflashRead_func failed 0x%xl !\n", error);
                return error;
            }
        }

        if (isVerifyImage == CC_TRUE){
            /* Per chunk, run CC operation (hash + AES) in-place.
            To improve performance, do not wait for completion operation */
            error = BsvCryptoImageUpdate( hwBaseAddress, cryptoMode, keyType, (uint32_t *)AESIv,
                            (uint8_t *)(CONVERT_TO_ADDR(cntImageRec.loadAddr)),
                            (uint8_t *)(CONVERT_TO_ADDR(cntImageRec.loadAddr)),
                                                     chunkSizeInBytes, actImageHash, isLoadIV);
            if (error != CC_OK) {
                                CC_PAL_LOG_ERR("BsvCryptoImageUpdate failed 0x%xl !\n", error);
                return error;
            }
            isLoadIV = CC_FALSE;
        }

        /* Update for next chunk */
               cntNonSignedImageRec.StoreAddr = (CCAddr_t)((unsigned long)cntNonSignedImageRec.StoreAddr + chunkSizeInBytes);
        cntImageRec.loadAddr = (CCAddr_t)((unsigned long)cntImageRec.loadAddr + chunkSizeInBytes);
        cntNonSignedImageRec.Len = cntNonSignedImageRec.Len - chunkSizeInBytes;

        if (isToggle == CC_TRUE) {

            /* Toggle on user's workspace (double buffer) */
            if ( cntImageRec.loadAddr == CONVERT_TO_ADDR(workRam1) ){
                cntImageRec.loadAddr = CONVERT_TO_ADDR(workRam2);
            } else {
                cntImageRec.loadAddr = CONVERT_TO_ADDR(workRam1);
            }
        }
    }

    if (isVerifyImage == CC_TRUE){

        /* get Hash result  and compare  */
                error = BsvCryptoImageFinish(hwBaseAddress, cryptoMode, actImageHash);
        if (error != CC_OK){
                        CC_PAL_LOG_ERR("BsvCryptoImageFinish failed 0x%xl !\n", error);
            return error;
        }

                error = UTIL_MemCmp((uint8_t *)cntImageRec.imageHash, (uint8_t *)actImageHash, HASH_RESULT_SIZE_IN_BYTES);
        if (error != CC_TRUE){
            CC_PAL_LOG_ERR("SW comp failed verification\n");
            /* clear image in RAM in case of CC_SB_LOAD_AND_VERIFY scheme */
            if ((isLoadFromFlash == CC_TRUE) && (isVerifyImage == CC_TRUE)
                && (currLoadStartAddress != CC_SW_COMP_NO_MEM_LOAD_INDICATION)){
                    UTIL_MemSet((uint8_t*)currLoadStartAddress, 0, actualImageSize);
            }
            return CC_BOOT_IMG_VERIFIER_SW_COMP_FAILED_VERIFICATION;
        }
    }

    return CC_OK;
}


CCError_t CCSbSetNvCounter(unsigned long hwBaseAddress, CCSbCertInfo_t *certPkgInfo)
{
    CCError_t error = CC_OK;

    if (NULL == certPkgInfo) {
        return CC_BOOT_IMG_VERIFIER_INV_INPUT_PARAM;
    }

    /* if version is bigger, then set the new version in the otp */
    if (certPkgInfo->activeMinSwVersionVal > certPkgInfo->otpVersion) {
        error =  NVM_SetSwVersion(hwBaseAddress, certPkgInfo->keyIndex, certPkgInfo->activeMinSwVersionVal);
        if (CC_OK != error) {
            return error;
        }
    }

    return error;
}

CCError_t CCSbVerifyNvCounter(unsigned long hwBaseAddress, uint32_t swVersion, CCSbCertInfo_t *certPkgInfo)
{
    CCError_t error = CC_OK;
    uint32_t otpVersion;
    uint8_t initFlag;
    CCSbPubKeyIndexType_t keyIndex;
    uint32_t activeNvCounterVal;

    /* Validate input parameters */
    if (NULL == certPkgInfo) {
        CC_PAL_LOG_ERR("invalid inputs\n");
        return CC_BOOT_IMG_VERIFIER_INV_INPUT_PARAM;
    }

    initFlag = certPkgInfo->initDataFlag;

    keyIndex = certPkgInfo->keyIndex;

    if (initFlag == 0) {
        /* Get sw version from OTP */
        error = NVM_GetSwVersion(hwBaseAddress, keyIndex, &otpVersion);
        if (CC_OK != error) {
            return error;
        }

        certPkgInfo->otpVersion = otpVersion;
    } else {

        otpVersion = certPkgInfo->otpVersion;
        //keyIndex = certPkgInfo->keyIndex;
        activeNvCounterVal = certPkgInfo->activeMinSwVersionVal;

        if ( activeNvCounterVal != swVersion ){
            CC_PAL_LOG_ERR("active counter version is different from the current\n");
            return CC_BOOT_IMG_VERIFIER_CERT_SW_VER_ILLEGAL;
        }

    }

    /* Verify the certificate version against the otp */
    if (swVersion < otpVersion){
        CC_PAL_LOG_ERR("currSwVersion1 < minVersion\n");
        return CC_BOOT_IMG_VERIFIER_SW_VER_SMALLER_THAN_MIN_VER;
    }

    return CC_OK;
}


