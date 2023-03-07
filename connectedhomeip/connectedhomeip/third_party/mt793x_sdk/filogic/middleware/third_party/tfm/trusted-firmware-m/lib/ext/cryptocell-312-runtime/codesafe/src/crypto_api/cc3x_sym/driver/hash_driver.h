/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef  _HASH_DRIVER_H
#define  _HASH_DRIVER_H

#include "driver_defs.h"

/******************************************************************************
*               TYPE DEFINITIONS
******************************************************************************/

/* The context data-base used by the Hash functions on the low level */
typedef struct HashContext {
    /* mode: SHA1, SHA224, SHA256 */
    hashMode_t mode;

    /* last block ack */
    uint32_t isLastBlockProcessed;

    /* hash data */
    uint32_t digest[MAX_DIGEST_SIZE_WORDS];

    /* The buffer that contains the size of so far digested message in the hardware.
       For SHA1, sha-224 and SHA-256 it is 64 bits,
       For SHA-384 and SHA-512 it is 128 bits */
    uint32_t totalDataSizeProcessed[4];

/* the follwing ia used only by soft sha512 */

    uint32_t valid_tag;

    /* the size of the Block size in bytes according to the currently running SHA algorithm (64 or 108) */
    uint32_t blockSizeInBytes;

    /* The number of bytes in the previous update */
    uint32_t prevDataInSize;

    /* A block buffer used for all cases where the update data size
    is not aligned to a block size - we cannot perform the block,
    therefore the first block is always loaded from this buffer  */
    uint32_t prevDataIn[HASH_SHA512_BLOCK_SIZE_IN_WORDS];

    /* function pointers for llf hash operations */
    llf_hash_init_operation_func  llfHashInitFuncP;
    llf_hash_update_operation_func  llfHashUpdateFuncP;
    llf_hash_finish_operation_func  llfHashFinishFuncP;

} HashContext_t;


/* HW hash for sha1, sha224 and sha256 */
drvError_t InitHashDrv(void  *pCtx);
drvError_t ProcessHashDrv(void *pCtx, CCBuffInfo_t *pInputBuffInfo, uint32_t dataInSize);
drvError_t FinishHashDrv(void  *pCtx);

/* soft hash for sha384 and sha512 */
drvError_t InitSwHash512(void  *pCtx);
drvError_t ProcessSwHash512(void  *pCtx, CCBuffInfo_t *pInputBuffInfo, uint32_t dataInSize );
drvError_t FinishSwHash512(void  *pCtx);

#endif /* _HASH_DRIVER_H */

