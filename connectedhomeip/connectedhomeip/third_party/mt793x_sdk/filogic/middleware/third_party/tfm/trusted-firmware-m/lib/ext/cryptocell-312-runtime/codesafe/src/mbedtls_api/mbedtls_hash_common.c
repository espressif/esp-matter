/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "hash_driver.h"
#include "cc_pal_mem.h"
#include "cc_pal_log.h"
#include "sha1_alt.h"
#include "sha256_alt.h"

/*
In CC312, DMA (i.e. for hash module input ) needs access to physical and continues memory.
In order to assure the DMA can access the residue data saved in the hashCtx, it is being to copied to a local
stack variable.
In case memory is guaranteed to be DMAable, this copy can be removed, and hashCtx->prevData can be used.
*/
static uint32_t   mbedtls_hashUpdate(void    *pHashUserCtx,
                        uint8_t     *pDataIn,
                        size_t      dataInSize)
{
    uint32_t rc = 0;
    HashContext_t *pHashCtx = NULL;
    size_t   bytesToAdd = 0;
    uint32_t localPrevDataIn[HASH_SHA512_BLOCK_SIZE_IN_WORDS];
    CCBuffInfo_t inBuffInfo;

    pHashCtx = (HashContext_t *)pHashUserCtx;
    // If pHashCtx->prevDataInSize > 0, fill it with with the current data
    bytesToAdd = CC_MIN(((pHashCtx->blockSizeInBytes - pHashCtx->prevDataInSize) % pHashCtx->blockSizeInBytes), dataInSize);
    if (bytesToAdd > 0) {
        /* add the data to the remaining buffer */
        CC_PalMemCopy(&(((uint8_t *)(pHashCtx->prevDataIn))[pHashCtx->prevDataInSize]), pDataIn, bytesToAdd);
        pHashCtx->prevDataInSize += bytesToAdd;
        pDataIn += bytesToAdd;
        dataInSize -= bytesToAdd;
    }

    // If the remaining buffer is full, process the block (else, the remaining buffer will be processed in the next update or finish) */
    if (pHashCtx->prevDataInSize == pHashCtx->blockSizeInBytes) {
        /* Copy prevDataIn to stack, in order to ensure continues and physical memory access.
        That way, DMA will be able to access the data on any platform.*/
        CC_PalMemCopy(localPrevDataIn, pHashCtx->prevDataIn, CC_MIN(HASH_SHA512_BLOCK_SIZE_IN_WORDS*sizeof(uint32_t), pHashCtx->prevDataInSize));

        rc = SetDataBuffersInfo((uint8_t*)localPrevDataIn, pHashCtx->blockSizeInBytes, &inBuffInfo,
                                   NULL, 0, NULL);
        if (rc != 0) {
             CC_PAL_LOG_ERR("illegal data buffers\n");
             return rc;
        }

        rc = ProcessHashDrv(pHashCtx, &inBuffInfo, pHashCtx->blockSizeInBytes);
        if (rc != CC_OK) {
            CC_PAL_LOG_ERR( "ProcessHashDrv failed, ret = %d\n", rc );
            return rc;
        }
        pHashCtx->prevDataInSize = 0;
    }

    // Process all the blocks that remain in the data
    bytesToAdd = (dataInSize / pHashCtx->blockSizeInBytes) * pHashCtx->blockSizeInBytes;
    if (bytesToAdd > 0) {

        rc = SetDataBuffersInfo(pDataIn, bytesToAdd, &inBuffInfo,
                                   NULL, 0, NULL);
        if (rc != 0) {
             CC_PAL_LOG_ERR("illegal data buffers\n");
             return rc;
        }

        rc = ProcessHashDrv(pHashCtx, &inBuffInfo, bytesToAdd);
        if (rc != CC_OK) {
            CC_PAL_LOG_ERR( "ProcessHashDrv failed, ret = %d\n", rc );
            return rc;
        }
        pDataIn += bytesToAdd;
        dataInSize -= bytesToAdd;
    }

    // Copy the remaining partial block to prevDataIn */
    bytesToAdd = dataInSize;
    if (bytesToAdd > 0) {
        CC_PalMemCopy((uint8_t *)&((pHashCtx->prevDataIn)[pHashCtx->prevDataInSize]), pDataIn, bytesToAdd);
        pHashCtx->prevDataInSize += bytesToAdd;
    }
    return CC_OK;
}

int mbedtls_sha_process_internal( void *ctx, const unsigned char *data )
{
    int ret;

    if ( NULL == ctx )
    {
        CC_PAL_LOG_ERR( "ctx is NULL\n" );
        return( 1 );
    }
    if ( NULL == data )
    {
        CC_PAL_LOG_ERR( "data is NULL\n" );
        return( 1 );
    }
    ret = mbedtls_hashUpdate(ctx, (uint8_t *)data, HASH_BLOCK_SIZE_IN_BYTES);
    if( CC_OK != ret)
    {
        CC_PAL_LOG_ERR("mbedtls_hashUpdate failed, ret = %d\n", ret);
        return( 1 );
    }

    return( 0 );
}

int mbedtls_sha_starts_internal( void *ctx, hashMode_t mode )
{
    int ret;
    HashContext_t *pHashCtx = NULL;

    if( NULL == ctx )
    {
        CC_PAL_LOG_ERR( "ctx is NULL\n" );
        return( 1 );
    }

    pHashCtx = (HashContext_t *)ctx;
    CC_PalMemSetZero(ctx, sizeof( HashContext_t ) );
    pHashCtx->mode = mode;
    pHashCtx->blockSizeInBytes = HASH_BLOCK_SIZE_IN_BYTES;
    ret = InitHashDrv(pHashCtx);
    if(ret != 0)
    {
        return( 1 );
    }
    else
    {
        return( 0 );
    }
}

int mbedtls_sha_finish_internal( void *ctx )
{
    uint32_t localPrevDataIn[HASH_SHA512_BLOCK_SIZE_IN_WORDS];
    size_t      dataInSize = 0;
    drvError_t drvRc = HASH_DRV_OK;
    HashContext_t *pHashCtx = NULL;
    CCBuffInfo_t inBuffInfo;

    pHashCtx = (HashContext_t *)ctx;
    if (pHashCtx->prevDataInSize != 0) {
        /* Copy prevDataIn to stack, in order to ensure continues and physical memory access.
        That way, DMA will be able to access the data on any platform.*/
        CC_PalMemCopy(localPrevDataIn, pHashCtx->prevDataIn, CC_MIN(HASH_SHA512_BLOCK_SIZE_IN_WORDS*sizeof(uint32_t), pHashCtx->prevDataInSize));
        dataInSize = pHashCtx->prevDataInSize;
    }
    pHashCtx->isLastBlockProcessed = 1;

    drvRc = SetDataBuffersInfo((uint8_t*)localPrevDataIn, dataInSize, &inBuffInfo,
                               NULL, 0, NULL);
    if (drvRc != 0) {
         CC_PAL_LOG_ERR("illegal data buffers\n");
         return( 1 );
    }

    drvRc = ProcessHashDrv(pHashCtx, &inBuffInfo, dataInSize);
    if (drvRc != HASH_DRV_OK){
        CC_PAL_LOG_ERR( "ProcessHashDrv failed, ret = %d\n", drvRc );
        return( 1 );
    }
    drvRc = FinishHashDrv(pHashCtx);
    if (drvRc != HASH_DRV_OK) {
        CC_PAL_LOG_ERR( "FinishHashDrv failed, ret = %d\n", drvRc );
        return( 1 );
    }
    pHashCtx->prevDataInSize = 0;

    return( 0 );
}

int mbedtls_sha_update_internal( void *ctx, const unsigned char *input, size_t ilen )
{
    int ret = 1;

    if (NULL == ctx){
        CC_PAL_LOG_ERR( "ctx is NULL\n" );
        return( 1 );
    }

    if (0 == ilen){
        /* This is a valid situation, no need to call hashUpdate.
        HashFinish will produce the result. */
        return( 0 );
    }

    //if len not zero, but pointer is NULL
    if (NULL == input){
        CC_PAL_LOG_ERR( "input is NULL\n" );
        //printf("TEST12, hash err = 1\n");
        return( 1 );
    }
    ret = mbedtls_hashUpdate(ctx, (uint8_t *)input, ilen);
    if( CC_OK != ret)
    {
        CC_PAL_LOG_ERR("mbedtls_hashUpdate failed, ret = %d\n", ret);
        return( 1 );
    }

    return( 0 );
}


