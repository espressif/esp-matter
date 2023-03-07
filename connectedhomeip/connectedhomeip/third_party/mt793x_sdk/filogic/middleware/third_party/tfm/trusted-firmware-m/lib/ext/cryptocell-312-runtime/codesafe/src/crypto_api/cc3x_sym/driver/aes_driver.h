/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef  _AES_DRIVER_H
#define  _AES_DRIVER_H

#include "driver_defs.h"

/******************************************************************************
*               TYPE DEFINITIONS
******************************************************************************/

/* The context data-base used by the AES functions on the low level */
typedef struct AesContext {
        /* IV buffer contains: on CTR mode - Counter, on other modes - IV */
        uint32_t    ivBuf[AES_IV_SIZE_WORDS];
        /* AES Key: fixed size is 128 bit = 512/2*/
        uint32_t    keyBuf[AES_256_BIT_KEY_SIZE_WORDS];
        /* keySize: 128, 192, 256 */
        keySizeId_t   keySizeId;
        /* mode: ECB, CBC, CTR, CBC-MAC, CMAC */
        aesMode_t mode;
        /* Decrypt / Encrypt */
        cryptoDirection_t dir;
        /* key type: user or hw(RKEK/PROV) */
        cryptoKeyType_t cryptoKey;
        /* padding type */
        cryptoPaddingType_t padType;
        /* first/middel/last */
        DataBlockType_t dataBlockType;
        /* data input addr type */
        dataAddrType_t inputDataAddrType;
        /* data output addr type */
        dataAddrType_t outputDataAddrType;
        uint8_t     tempBuff[AES_BLOCK_SIZE];
} AesContext_t;


/******************************************************************************
*               FUNCTION PROTOTYPES
******************************************************************************/

/*!
 * This function is used to process block(s) of data using the AES machine.
 *
 * \param aesCtx A pointer to the AES context buffer.
 * \param pInputBuffInfo A structure which represents the data input buffer.
 * \param pOutputBuffInfo A structure which represents the data output buffer.
 * \param blockSize - number of bytes to copy.
 *
 * \return drvError_t defined in driver_defs.h.
 */
drvError_t ProcessAesDrv(AesContext_t *aesCtx, CCBuffInfo_t *pInputBuffInfo, CCBuffInfo_t *pOutputBuffInfo, uint32_t blockSize);

/*!
 * This function is used as finish operation of AES on XCBC, CMAC, CBC
 * and other modes besides XTS mode.
 * The function may either be called after "InitCipher" or "ProcessCipher".
 *
 * \param aesCtx A pointer to the AES context buffer.
 * \param pInputBuffInfo A structure which represents the data input buffer.
 * \param pOutputBuffInfo A structure which represents the data output buffer.
 * \param blockSize - number of bytes to copy.
 *
 * \return drvError_t defined in driver_defs.h.
 */
drvError_t FinishAesDrv(AesContext_t *aesCtx, CCBuffInfo_t *pInputBuffInfo, CCBuffInfo_t *pOutputBuffInfo, uint32_t blockSize);


#endif /* _AES_DRIVER_H */

