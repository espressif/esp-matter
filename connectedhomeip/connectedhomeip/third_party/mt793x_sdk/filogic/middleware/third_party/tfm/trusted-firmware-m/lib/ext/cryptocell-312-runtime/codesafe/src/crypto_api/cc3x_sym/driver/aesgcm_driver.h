/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef _AESGCM_DRIVER_H
#define _AESGCM_DRIVER_H

#if defined(MBEDTLS_CONFIG_FILE)
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_GCM_C)

/*
 * All the includes that are needed for code using this file to
 * compile correctly should be #included here.
 */
#include "driver_defs.h"
#include "cc_aes_defs.h"
#include "cc_pal_types.h"
#include "cc_bitops.h"

#ifdef __cplusplus
extern "C"
{
#endif

/************************ Defines ******************************/

/* 128 bits */
#define CC_AESGCM_GHASH_DIGEST_SIZE_BYTES           16
#define CC_AESGCM_GHASH_DIGEST_SIZE_WORDS           CC_AESGCM_GHASH_DIGEST_SIZE_BYTES>>2

/************************ Enums ********************************/

/*! AES GCM driver process modes. */
typedef enum {
    /*! Calculate H. */
    DRV_AESGCM_Process_CalcH                = 0,
    /*! Calculate J0 - First phase. */
    DRV_AESGCM_Process_CalcJ0_FirstPhase    = 1,
    /*! Calculate J0 - Second phase. */
    DRV_AESGCM_Process_CalcJ0_SecondPhase   = 2,
    /*! GHASH AAD. */
    DRV_AESGCM_Process_A                    = 3,
    /*! GCTR and GHASH Data In. */
    DRV_AESGCM_Process_DataIn               = 4,
    /*! GHASH Len(A) || Len(C). */
    DRV_AESGCM_Process_LenA_LenC            = 5,
    /*! GCTR Final. */
    DRV_AESGCM_Process_GctrFinal            = 6,

    /*! Number of optional key sizes. */
    DRV_AESGCM_ProcessesTotal,
    /*! Reserved. */
    DRV_AESGCM_ProcessLast    = 0x7FFFFFFF,
}drvAesGcmProcessModes_t;


/************************ Typedefs  ****************************/

/*  NOTE: make sure that structure size equals to CC_AESGCM_USER_CTX_SIZE_IN_WORDS */
typedef struct  AesGcmContext_t {
    /* AES max key size supported is 256 bit */
    uint32_t keyBuf[AES_256_BIT_KEY_SIZE_WORDS];
    /* H buffer size is 128 bit */
    uint32_t H[AES_128_BIT_KEY_SIZE_WORDS];
    /* J0 buffer size is 128 bit */
    uint32_t J0[CC_AESGCM_GHASH_DIGEST_SIZE_WORDS];
    /* Temp. buffer size is 128 bit */
    uint32_t tempBuf[CC_AESGCM_GHASH_DIGEST_SIZE_WORDS];
    /* AES Counter buffer size is 128 bit */
    uint32_t aesCntrBuf[CC_AESGCM_GHASH_DIGEST_SIZE_WORDS];
    /* GHASH Result buffer size is 128 bit */
    uint32_t ghashResBuf[CC_AESGCM_GHASH_DIGEST_SIZE_WORDS];
    /* Pre-Tag buffer size is 128 bit */
    uint8_t preTagBuf[CC_AESGCM_GHASH_DIGEST_SIZE_BYTES];
    /* J0 Inc32 flag */
    CCBool J0Inc32DoneFlg;
    /* keySize: 128, 192, 256 */
    keySizeId_t keySizeId;
    /* Encrypt / Decrypt */
    cryptoDirection_t dir;
    /* Tag size */
    uint8_t tagSize;
    /* Alignment */
    uint8_t RFU[3];
    /* Driver process mode */
    drvAesGcmProcessModes_t processMode;
    /* Data size (Plain/Cipher text) */
    uint32_t dataSize;
    /* IV size */
    uint32_t ivSize;
    /* AAD size */
    uint32_t aadSize;
}AesGcmContext_t;


/******************************************************************************
*               FUNCTION PROTOTYPES
******************************************************************************/

/*!
 * This function is used to process block of data using the AES and / or Hash machines.
 *
 * \param pAesGcmCtx A pointer to the AES-GCM context buffer.
 * \param pInputBuffInfo A structure which represents the data input buffer.
 * \param pOutputBuffInfo A structure which represents the data output buffer.
 * \param blockSize - number of bytes to copy.
 *
 * \return drvError_t defined in driver_defs.h.
 */
drvError_t ProcessAesGcm(AesGcmContext_t *pAesGcmCtx, CCBuffInfo_t *pInputBuffInfo, CCBuffInfo_t *pOutputBuffInfo, uint32_t blockSize);


#ifdef __cplusplus
}
#endif


























#endif //  MBEDTLS_GCM_C
#endif // _AESGCM_DRIVER_H



