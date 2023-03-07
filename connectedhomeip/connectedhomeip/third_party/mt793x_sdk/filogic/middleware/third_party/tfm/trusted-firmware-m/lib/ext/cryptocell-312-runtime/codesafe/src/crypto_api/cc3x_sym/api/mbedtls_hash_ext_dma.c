/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#define CC_PAL_LOG_CUR_COMPONENT CC_LOG_MASK_CC_API

#include "cc_pal_types.h"
#include "cc_pal_mem.h"
#include "cc_pal_abort.h"
#include "cc_hash_defs.h"
#include "hash_driver_ext_dma.h"
#include "mbedtls_hash_ext_dma.h"
#include "cc_common.h"
#include "cc_common_math.h"
#include "cc_sym_error.h"
#include "cc_pal_perf.h"
#include "cc_pal_compiler.h"
#include "mbedtls_ext_dma_error.h"

static int Driver2ExtDmaHashErr(drvError_t drvRc)
{
    switch (drvRc) {
    case HASH_DRV_OK:
        return 0;
    case HASH_DRV_ILLEGAL_OPERATION_MODE_ERROR:
        return EXT_DMA_HASH_ILLEGAL_OPERATION_MODE_ERROR;
    default:
        return CC_FATAL_ERROR;
    }
}


int mbedtls_hash_ext_dma_init(CCHashOperationMode_t  operationMode, uint32_t dataSize)
{
    drvError_t drvRc = HASH_DRV_OK;
    int error = CC_OK;
    hashMode_t mode;

    /* check Hash mode */
    switch (operationMode) {
    case CC_HASH_SHA1_mode:
        mode = HASH_SHA1;
        break;
    case CC_HASH_SHA224_mode:
        mode = HASH_SHA224;
        break;
    case CC_HASH_SHA256_mode:
        mode = HASH_SHA256;
        break;
    default:
        error = EXT_DMA_HASH_ILLEGAL_OPERATION_MODE_ERROR;
        goto endInit;
    }

    if (dataSize > CPU_DIN_MAX_SIZE){
        error = EXT_DMA_ILLEGAL_INPUT_SIZE_ERROR;
        goto endInit;
    }
    drvRc = InitHashExtDma(mode, dataSize);
    error = Driver2ExtDmaHashErr(drvRc);

endInit:
    return error;
}



int mbedtls_hash_ext_dma_finish(CCHashOperationMode_t  operationMode, uint32_t digestBufferSize, uint32_t *digestBuffer)
{
    CCError_t error = CC_OK;
    drvError_t rc = HASH_DRV_OK;
    hashMode_t mode;

    if ( digestBuffer == NULL ) {
        error = EXT_DMA_HASH_INVALID_RESULT_BUFFER_POINTER_ERROR;
        goto endFinish;
    }
    if (digestBufferSize != CC_HASH_SHA1_DIGEST_SIZE_IN_BYTES &&
        digestBufferSize != CC_HASH_SHA224_DIGEST_SIZE_IN_BYTES &&
        digestBufferSize != CC_HASH_SHA256_DIGEST_SIZE_IN_BYTES) {
        error = EXT_DMA_HASH_ILLEGAL_PARAMS_ERROR;
        goto endFinish;
    }
    /* check Hash mode */
    switch (operationMode) {
    case CC_HASH_SHA1_mode:
        mode = HASH_SHA1;
        break;
    case CC_HASH_SHA224_mode:
        mode = HASH_SHA224;
        break;
    case CC_HASH_SHA256_mode:
        mode = HASH_SHA256;
        break;
    default:
        error = EXT_DMA_HASH_ILLEGAL_OPERATION_MODE_ERROR;
        goto endFinish;
    }

    error = FinishHashExtDma(mode, digestBuffer);
    error = Driver2ExtDmaHashErr(error);
    return error;

endFinish:
    rc = terminateHashExtDma();
    if (rc != 0) {
        CC_PalAbort("Failed to terminateAesExtDma \n");
    }
    return error;
}
