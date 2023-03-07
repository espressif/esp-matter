/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef _AESCCM_DRIVER_H
#define _AESCCM_DRIVER_H

/*
 * All the includes that are needed for code using this file to
 * compile correctly should be #included here.
 */
#include "driver_defs.h"
#include "cc_aes_defs.h"

#ifdef __cplusplus
extern "C"
{
#endif

/************************ Defines ******************************/

/************************ Enums ********************************/

/************************ Typedefs  ****************************/

/*  NOTE: make sure struct size equals to CC_AESCCM_USER_CTX_SIZE_IN_WORDS */
typedef struct  AesCcmContext_t {
    /* IV buffer */
    uint32_t ivBuf[AES_IV_SIZE_WORDS];
    /* AES max key size supported is 256 bit */
    uint32_t keyBuf[AES_256_BIT_KEY_SIZE_WORDS];
    /* AES counter for CTR mode */
    uint32_t ctrStateBuf[AES_IV_SIZE_WORDS];
    /* scratch buffer for internal use */
    uint8_t tempBuff[CC_AES_BLOCK_SIZE_IN_BYTES];
    /* mode: CBC_MAC, CTR, CCMAPD, CCMAPE */
    aesMode_t mode;
    /* keySize: 128, 192, 256 */
    keySizeId_t keySizeId;
    /* Decrypt / Encrypt */
    cryptoDirection_t dir;
    /* nonce size */
    uint8_t sizeOfN;
    /* T mac size */
    uint8_t sizeOfT;
}AesCcmContext_t;


/******************************************************************************
*               FUNCTION PROTOTYPES
******************************************************************************/

/*!
 * This function is used to process block of data using the AES machine.
 * It is optimized to support only CBC_MAC, CTR, CCMPE, CCMPD.
 *
 * \param pAesCcmCtx A pointer to the AEAD context buffer
 * \param pInputBuffInfo A structure which represents the data input buffer.
 * \param pOutputBuffInfo A structure which represents the data output buffer.
 * \param blockSize - number of bytes to copy
 *
 * \return drvError_t defined in driver_defs.h
 */
drvError_t ProcessAesCcmDrv(AesCcmContext_t   *pAesCcmCtx, CCBuffInfo_t *pInputBuffInfo, CCBuffInfo_t *pOutputBuffInfo, uint32_t blockSize);


#ifdef __cplusplus
}
#endif

#endif


