/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef  _CHACHA_DRIVER_H
#define  _CHACHA_DRIVER_H

#include "driver_defs.h"

/******************************************************************************
*               TYPE DEFINITIONS
******************************************************************************/

/* The context data-base used by the CHACHA functions on the low level */
typedef struct ChachaContext {
    /* IV buffer */
    uint32_t    nonceBuf[CHACHA_IV_96_SIZE_WORDS];
    /* CHACHA block counter */
    uint32_t    blockCounterLsb;
    uint32_t    blockCounterMsb;
    /* CHACHA Key: fixed size is 256 bit */
    uint32_t    keyBuf[CHACHA_256_BIT_KEY_SIZE_WORDS];
    /* Decrypt / Encrypt */
    cryptoDirection_t dir;
    /* data input addr type */
    dataAddrType_t inputDataAddrType;
    /* data output addr type */
    dataAddrType_t outputDataAddrType;
    /* CHACHA iv size */
    chachaNonceSize_t nonceSize;
} ChachaContext_t;


/******************************************************************************
*               FUNCTION PROTOTYPES
******************************************************************************/
/*!
 * This function is used to process block(s) of data using the CHACHA machine.
 *
 * \param chachaCtx - A pointer to the CHACHA context buffer.
 * \param pInputBuffInfo A structure which represents the data input buffer.
 * \param pOutputBuffInfo A structure which represents the data output buffer.
 * \param inDataSize - number of bytes to process.
 *
 * \return drvError_t defined in driver_defs.h.
 */
drvError_t ProcessChacha(ChachaContext_t *chachaCtx, CCBuffInfo_t *pInputBuffInfo, CCBuffInfo_t *pOutputBuffInfo, uint32_t inDataSize);


#endif /* _CHACHA_DRIVER_H */

