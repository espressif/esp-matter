/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _SBRT_INT_FUNC_H
#define _SBRT_INT_FUNC_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "secureboot_gen_defs.h"

/* The AES block size in words and in bytes */
#define AES_BLOCK_SIZE_IN_WORDS 4
/* The size of the AES KEY in words and bytes */
#define AES_KEY_SIZE_IN_WORDS AES_BLOCK_SIZE_IN_WORDS
/* The size of the IV or counter buffer */
#define AES_IV_COUNTER_SIZE_IN_WORDS   AES_BLOCK_SIZE_IN_WORDS
#define AES_IV_COUNTER_SIZE_IN_BYTES  (AES_IV_COUNTER_SIZE_IN_WORDS * sizeof(uint32_t))

/* Defines the AES key buffer */
typedef uint32_t AES_Key_t[AES_KEY_SIZE_IN_WORDS];
/* Defines the IV counter buffer  - 16 bytes array */
typedef uint32_t AES_Iv_t[AES_IV_COUNTER_SIZE_IN_WORDS];

CCError_t SBRT_ImageLoadAndVerify(CCSbFlashReadFunc preHashflashRead_func,
                           void *preHashUserContext,
                           unsigned long hwBaseAddress,
                           uint8_t isLoadFromFlash,
                           uint8_t isVerifyImage,
                           bsvCryptoMode_t cryptoMode,
                           CCBsvKeyType_t  keyType,
                           AES_Iv_t AESIv,
                           uint8_t *pSwRecSignedData,
                           uint32_t *pSwRecNoneSignedData,
                           uint32_t *workspace_ptr,
                           uint32_t workspaceSize);

CCError_t SBRT_RSA_PSS_Verify(unsigned long hwBaseAddress,      /* [in] HW base address of registers. */
                  CCHashResult_t mHash,         /* [in] Pointer to the SHA256 hash of the message. */
                  uint32_t      *pN,                /* [in] Pointer to the RSA modulus (LE words array). */
                  uint32_t      *pNp,           /* [in] Pointer to the Barrett tag of the RSA modulus (LE words array). */
                  uint32_t      *pSign              /* [out] Pointer to the signature output (it is placed as BE bytes
                                                                array into words buffer for alignments goal). */);

void SBRT_HalClearInterruptBit(unsigned long hwBaseAddress, uint32_t data);

void SBRT_HalMaskInterrupt(unsigned long hwBaseAddress, uint32_t data);

CCError_t SBRT_HalWaitInterrupt(unsigned long hwBaseAddress, uint32_t data);

CCError_t SBRT_LcsGet(unsigned long hwBaseAddress, uint32_t *pLcs);

CCError_t SBRT_OTPWordRead(unsigned long hwBaseAddress, uint32_t otpAddress, uint32_t *pOtpWord);

CCError_t SBRT_SwVersionGet(unsigned long hwBaseAddress, CCSbPubKeyIndexType_t keyIndex, uint32_t *swVersion);

CCError_t SBRT_PubKeyHashGet(unsigned long hwBaseAddress, CCSbPubKeyIndexType_t keyIndex, uint32_t *hashedPubKey, uint32_t hashResultSizeWords);

CCError_t SBRT_SHA256( unsigned long        hwBaseAddress,
            uint8_t         *pDataIn,
                        size_t                  dataSize,
                        CCHashResult_t          hashBuff);

CCError_t SBRT_CryptoImageInit( unsigned long   hwBaseAddress,
                bsvCryptoMode_t mode,
                CCBsvKeyType_t  keyType);

CCError_t SBRT_CryptoImageUpdate( unsigned long     hwBaseAddress,
                  bsvCryptoMode_t   mode,
                  CCBsvKeyType_t        keyType,
                  uint32_t      *pCtrStateBuf,
                  uint8_t       *pDataIn,
                  uint8_t       *pDataOut,
                  size_t                dataSize,
                  CCHashResult_t        hashBuff,
                  uint8_t       isLoadIV);

CCError_t SBRT_CryptoImageFinish( unsigned long     hwBaseAddress,
                  bsvCryptoMode_t   mode,
                  CCHashResult_t        hashBuff);

uint32_t SBRT_MemCmp( uint8_t *pBuff1 , uint8_t *pBuff2 , uint32_t size);

void SBRT_ReverseMemCopy( uint8_t *pDst, uint8_t *pSrc, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif



