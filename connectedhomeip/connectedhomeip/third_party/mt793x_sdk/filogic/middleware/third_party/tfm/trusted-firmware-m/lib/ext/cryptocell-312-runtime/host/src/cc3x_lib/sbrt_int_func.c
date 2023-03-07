/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


/************* Include Files ****************/
#include "cc_pal_types.h"
#include "cc_pal_mem.h"
#include "cc_pal_mutex.h"
#include "cc_pal_abort.h"

#include "secureboot_defs.h"
#include "secureboot_stage_defs.h"
#include "secureboot_gen_defs.h"
#include "bootimagesverifier_error.h"
#include "bsv_error.h"
#include "secdebug_defs.h"
#include "secureboot_base_swimgverify.h"
#include "cc_hal.h"
#include "mbedtls_cc_mng_int.h"
#include "mbedtls_cc_mng_error.h"
#include "cc_util_pm.h"
#include "cc_int_general_defs.h"

/************************ Defines ******************************/

/************************ Enums ******************************/

/************************ Typedefs ******************************/

/************************ Global Data ******************************/
extern CC_PalMutex CCAsymCryptoMutex;
extern CC_PalMutex CCSymCryptoMutex;

/************************ Private functions  ******************************/

static CCError_t GetMngKeyIndex(CCSbPubKeyIndexType_t keyIndex, mbedtls_mng_pubKeyType_t *mngKeyIndex)
{
    switch (keyIndex) {
        case CC_SB_HASH_BOOT_KEY_0_128B:
            *mngKeyIndex = CC_MNG_HASH_BOOT_KEY_0_128B;
            break;
        case CC_SB_HASH_BOOT_KEY_1_128B:
            *mngKeyIndex = CC_MNG_HASH_BOOT_KEY_1_128B;
            break;
        case CC_SB_HASH_BOOT_KEY_256B:
            *mngKeyIndex = CC_MNG_HASH_BOOT_KEY_256B;
            break;
        case CC_SB_HASH_BOOT_NOT_USED:
            *mngKeyIndex = CC_MNG_HASH_BOOT_NOT_USED;
            break;
        case CC_SB_HASH_MAX_NUM:
            *mngKeyIndex = CC_MNG_HASH_MAX_NUM;
            break;
        default:
            return CC_MNG_ILLEGAL_INPUT_PARAM_ERR;
    }

    return CC_OK;
}

/************************ Public functions  ******************************/

CCError_t SBRT_ImageLoadAndVerify(CCSbFlashReadFunc preHashflashRead_func,
                                  void *preHashUserContext,
                                  unsigned long hwBaseAddress,
                                  uint8_t isLoadFromFlash,
                                  uint8_t isVerifyImage,
                                  bsvCryptoMode_t cryptoMode,
                                  CCBsvKeyType_t keyType,
                                  AES_Iv_t AESIv,
                                  uint8_t *pSwRecSignedData,
                                  uint32_t *pSwRecNoneSignedData,
                                  uint32_t *workspace_ptr,
                                  uint32_t workspaceSize)
{
    CCError_t error = CC_OK;
    ContentCertImageRecord_t cntImageRec;

    /* Loading only is not supported for SBRT */
    if (isVerifyImage == CC_FALSE){
            return CC_BOOT_IMG_VERIFIER_INV_INPUT_PARAM;
    }

    /* In any case, loading operation shall be ignored (and not failed) and the image will be only verified. */
    if (isLoadFromFlash == CC_TRUE){

            /* Initialize parameters */
            CC_PalMemCopy((uint8_t *)&cntImageRec, pSwRecSignedData, sizeof(ContentCertImageRecord_t));

            /* overwrite load address to ignore loading */
            cntImageRec.loadAddr = CC_SW_COMP_NO_MEM_LOAD_INDICATION;
            CC_PalMemCopy(pSwRecSignedData, (uint8_t*)&cntImageRec, sizeof(ContentCertImageRecord_t));
    }

    /* lock mutex for more CC operations */
    error = CC_PalMutexLock(&CCSymCryptoMutex, CC_INFINITE);
    if (error != 0) {
            CC_PalAbort("Fail to acquire mutex\n");
    }

    /* increase CC counter at the beginning of each operation */
    error = CC_IS_WAKE;
    if (error != 0) {
            CC_PalAbort("Fail to increase PM counter\n");
    }

    error = CCSbImageLoadAndVerify(preHashflashRead_func, preHashUserContext,
                                   hwBaseAddress, isLoadFromFlash, isVerifyImage,
                                   cryptoMode, keyType, AESIv, pSwRecSignedData,
                                   pSwRecNoneSignedData, workspace_ptr, workspaceSize);

    /* decrease CC counter at the end of each operation */
    if (CC_IS_IDLE != CC_SUCCESS) {
            CC_PalAbort("Fail to decrease PM counter\n");
    }

    /* release mutex */
    if (CC_PalMutexUnlock(&CCSymCryptoMutex) != 0) {
            CC_PalAbort("Fail to release mutex\n");
    }

    return error;
}

CCError_t SBRT_RSA_PSS_Verify(unsigned long hwBaseAddress,      /* [in] HW base address of registers. */
                              CCHashResult_t mHash,         /* [in] Pointer to the SHA256 hash of the message. */
                              uint32_t      *pN,                /* [in] Pointer to the RSA modulus (LE words array). */
                              uint32_t      *pNp,           /* [in] Pointer to the Barrett tag of the RSA modulus (LE words array). */
                              uint32_t      *pSign              /* [out] Pointer to the signature output (it is placed as BE bytes
                                                                array into words buffer for alignments goal). */)
{

    CCError_t error = CC_OK;

    error = CC_PalMutexLock(&CCAsymCryptoMutex, CC_INFINITE);
    if (error != CC_SUCCESS) {
            CC_PalAbort("Fail to acquire mutex\n");
    }

    /* verify that the device is not in fatal error state before activating the PKA engine */
    CC_IS_FATAL_ERR_ON(error);
    if (error == CC_TRUE) {
            error = CC_BSV_FATAL_ERR_IS_LOCKED_ERR;
            goto _EndUnlockMutex;
    }

    /* increase CC counter at the beginning of each operation */
    error = CC_IS_WAKE;
    if (error != CC_SUCCESS) {
            CC_PalAbort("Fail to increase PM counter\n");
    }

    error = RSA_PSS_Verify(hwBaseAddress, mHash, pN, pNp, pSign);

    /* decrease CC counter at the end of each operation */
    if (CC_IS_IDLE != CC_SUCCESS) {
            CC_PalAbort("Fail to decrease PM counter\n");
    }

_EndUnlockMutex:
    if (CC_PalMutexUnlock(&CCAsymCryptoMutex) != 0) {
            CC_PalAbort("Fail to release mutex\n");
    }

    return error;

}

void SBRT_HalClearInterruptBit(unsigned long hwBaseAddress, uint32_t data)
{
    CC_UNUSED_PARAM(hwBaseAddress);
    CC_HalClearInterruptBit(data);
    return;
}

void SBRT_HalMaskInterrupt(unsigned long hwBaseAddress, uint32_t data)
{
    CC_UNUSED_PARAM(hwBaseAddress);
    CC_HalMaskInterrupt(data);
    return;
}

CCError_t SBRT_HalWaitInterrupt(unsigned long hwBaseAddress, uint32_t data)
{
    CC_UNUSED_PARAM(hwBaseAddress);
    return CC_HalWaitInterrupt(data);
}

CCError_t SBRT_LcsGet(unsigned long hwBaseAddress, uint32_t *pLcs)
{

    CC_UNUSED_PARAM(hwBaseAddress);
    return mbedtls_mng_lcsGet(pLcs);
}

CCError_t SBRT_OTPWordRead(unsigned long hwBaseAddress, uint32_t otpAddress, uint32_t *pOtpWord)
{
    CC_UNUSED_PARAM(hwBaseAddress);
    return mbedtls_mng_otpWordRead(otpAddress, pOtpWord);
}

CCError_t SBRT_SwVersionGet(unsigned long hwBaseAddress, CCSbPubKeyIndexType_t keyIndex, uint32_t *swVersion)
{
    CCError_t error = CC_OK;
    mbedtls_mng_pubKeyType_t mngKeyIndex;

    CC_UNUSED_PARAM(hwBaseAddress);

    error = GetMngKeyIndex(keyIndex, &mngKeyIndex);
    if (error != CC_OK)
        return error;

    return mbedtls_mng_swVersionGet(mngKeyIndex, swVersion);
}

CCError_t SBRT_PubKeyHashGet(unsigned long hwBaseAddress, CCSbPubKeyIndexType_t keyIndex, uint32_t *hashedPubKey, uint32_t hashResultSizeWords)
{
    CCError_t error = CC_OK;
    mbedtls_mng_pubKeyType_t mngKeyIndex;

    CC_UNUSED_PARAM(hwBaseAddress);

    error = GetMngKeyIndex(keyIndex, &mngKeyIndex);
    if (error != CC_OK)
        return error;

    return mbedtls_mng_pubKeyHashGet(mngKeyIndex, hashedPubKey, hashResultSizeWords);
}

CCError_t SBRT_SHA256( unsigned long        hwBaseAddress,
                       uint8_t          *pDataIn,
                       size_t                   dataSize,
                       CCHashResult_t       hashBuff)
{
    CCError_t error = CC_OK;

    /* verify that data is limited to 64KB */
    if ( dataSize >= CC_BSV_SHA256_MAX_DATA_SIZE_IN_BYTES) {
            return CC_BSV_INVALID_DATA_SIZE_ERROR;
    }

    /* verify valid buffer pointer */
    if ( (pDataIn == NULL) && (dataSize!=0) ) {
            return CC_BSV_INVALID_DATA_IN_POINTER_ERROR;
    }

    /* verify valid buffer pointer */
    if (hashBuff == NULL) {
            return CC_BSV_INVALID_RESULT_BUFFER_POINTER_ERROR;
    }

    /* lock mutex for more CC operations */
    error = CC_PalMutexLock(&CCSymCryptoMutex, CC_INFINITE);
    if (error != 0) {
            CC_PalAbort("Fail to acquire mutex\n");
    }

    /* increase CC counter at the beginning of each operation */
    error = CC_IS_WAKE;
    if (error != 0) {
            CC_PalAbort("Fail to increase PM counter\n");
    }

    InitBsvHash(hwBaseAddress);

    error = ProcessBsvHash(hwBaseAddress, (uint32_t)pDataIn, dataSize);
    if (error != CC_OK){
            // in case of error, ProcessBsvHash responsible to call FreeBsvHash
            UTIL_MemSet((uint8_t*)hashBuff, 0, CC_BSV_SHA256_DIGEST_SIZE_IN_BYTES);
            goto _END_SHA256;
    }

    FinishBsvHash(hwBaseAddress, hashBuff);

_END_SHA256:
    /* decrease CC counter at the end of each operation */
    if (CC_IS_IDLE != CC_SUCCESS) {
            CC_PalAbort("Fail to decrease PM counter\n");
    }

    /* release mutex */
    if (CC_PalMutexUnlock(&CCSymCryptoMutex) != 0) {
            CC_PalAbort("Fail to release mutex\n");
    }

    return error;
}

CCError_t SBRT_CryptoImageInit( unsigned long   hwBaseAddress,
                                bsvCryptoMode_t mode,
                                CCBsvKeyType_t  keyType)
{
    /* verify tunneling mode */
    if ((mode!=BSV_CRYPTO_HASH) &&
            (mode!=BSV_CRYPTO_AES_AND_HASH) &&
            (mode!=BSV_CRYPTO_AES_TO_HASH_AND_DOUT)){
            return CC_BSV_INVALID_CRYPTO_MODE_ERROR;
    }

    if(mode != BSV_CRYPTO_HASH){
            /* for image decryption only KCE and KCEICV are supported */
            if ((keyType!=CC_BSV_CE_KEY) && (keyType!=CC_BSV_ICV_CE_KEY)){
                    return CC_BSV_INVALID_KEY_TYPE_ERROR;
            }
    }

    /* initiate HW engines */
    InitBsvHash(hwBaseAddress);

    if (mode != BSV_CRYPTO_HASH){
        InitBsvAes(hwBaseAddress);

        /* overwrite crypto mode */
        SB_HAL_WRITE_REGISTER(SB_REG_ADDR(hwBaseAddress, CRYPTO_CTL) ,mode);
    }

    return CC_OK;
}

CCError_t SBRT_CryptoImageUpdate( unsigned long     hwBaseAddress,
                                  bsvCryptoMode_t   mode,
                                  CCBsvKeyType_t        keyType,
                                  uint32_t      *pCtrStateBuf,
                                  uint8_t       *pDataIn,
                                  uint8_t       *pDataOut,
                                  size_t                dataSize,
                                  CCHashResult_t        hashBuff,
                                  uint8_t       isLoadIV)
{
    CCError_t error = CC_OK;

    /* data in processing */
    if (mode == BSV_CRYPTO_HASH){
            error = ProcessBsvHash(hwBaseAddress, (uint32_t)pDataIn, dataSize);
    } else {
            error = ProcessBsvAes(hwBaseAddress, BSV_AES_CIPHER_CTR, keyType, NULL, CC_BSV_128BITS_KEY_SIZE_IN_BYTES,
                                  pCtrStateBuf, (uint32_t)pDataIn, (uint32_t)pDataOut, dataSize, isLoadIV);
    }

    if (error != CC_OK){
        FreeBsvHash(hwBaseAddress);
        UTIL_MemSet((uint8_t*)hashBuff, 0, CC_BSV_SHA256_DIGEST_SIZE_IN_BYTES);
    }

    return error;
}


CCError_t SBRT_CryptoImageFinish( unsigned long     hwBaseAddress,
                                  bsvCryptoMode_t   mode,
                                  CCHashResult_t        hashBuff)
{

    /* close HW engines */

    FinishBsvHash(hwBaseAddress, hashBuff);
    if (mode != BSV_CRYPTO_HASH){
            FinishBsvAes(hwBaseAddress, BSV_AES_CIPHER_CTR, NULL);
    }

    return CC_OK;
}

uint32_t SBRT_MemCmp( uint8_t *pBuff1 , uint8_t *pBuff2 , uint32_t size)
{
    /* same as ROM  code for security issues */

    /* loop variable */
    uint32_t i;
    uint32_t stat = 0;

    /* FUNCTION LOGIC */

    for( i = 0; i < size; i++ ) {
            stat |= (pBuff1[i] ^ pBuff2[i]);
    }

    if(stat == 0)
        return CC_TRUE;
    else
        return CC_FALSE;

}

void SBRT_ReverseMemCopy( uint8_t *pDst, uint8_t *pSrc, uint32_t size)
{
    /* FUNCTION DECLARATIONS */

    /* loop variable */
    uint32_t i;
    uint8_t tmp;

    /* buffers position identifiers */
    uint32_t dstPos, srcPos;

    /* FUNCTION LOGIC */

    /* initialize the source and the destination position */
    dstPos = size - 1;
    srcPos = 0;

    /* execute the reverse copy in case of different buffers */
    if (pDst != pSrc) {
            for( i = 0 ; i < size ; i++ )
                pDst[dstPos--] = pSrc[srcPos++];
    } else {
            /* execute the reverse copy in case of in-place reversing */
            for( i = 0 ; i < size/2 ; i++ ) {
                    tmp = pDst[dstPos];
                    pDst[dstPos--] = pSrc[srcPos];
                    pSrc[srcPos++] = tmp;
            }
    }

    return;

}



