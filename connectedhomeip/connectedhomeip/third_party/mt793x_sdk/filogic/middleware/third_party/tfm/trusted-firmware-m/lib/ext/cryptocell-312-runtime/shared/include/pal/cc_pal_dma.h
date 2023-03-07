/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */



#ifndef _CC_PAL_DMA_H
#define _CC_PAL_DMA_H

/*!
@file
@brief This file contains definitions that are used for DMA-related APIs. The implementation of these functions
need to be replaced according to the platform and OS.
@defgroup ssi_pal_dma CryptoCell PAL DMA related APIs
@{
@ingroup ssi_pal

*/

#ifdef __cplusplus
extern "C"
{
#endif

#include "cc_pal_types.h"
#include "cc_pal_dma_plat.h"
#include "cc_pal_dma_defs.h"

/*! User buffer scatter information. */
typedef struct {
    CCDmaAddr_t     blockPhysAddr; /*!< The physical address of the user buffer.*/
    uint32_t        blockSize;     /*!< The block size of the user buffer.*/
}CCPalDmaBlockInfo_t;

#ifdef BIG__ENDIAN
/*! Definition for big to little endian. */
#define  SET_WORD_LE(val) cpu_to_le32(val)
#else
/*! Definition for big to little endian. */
#define  SET_WORD_LE
#endif

/**
 * @brief   This function is called by the CryptoCell runtime library before the HW is used.
 *      It maps a given data buffer (virtual address) for CryptoCell HW DMA use (physical address), and returns the list of
 *      one or more DMA-able (physical) blocks. Once it is called,
 *      only CryptoCell HW access to the buffer is allowed, until it is unmapped.
 *      \note If the data buffer was already mapped by the secure OS prior to calling the CryptoCell runtime library,
 *      this API does not have to perform any actual mapping operation, but only return the list of DMA-able blocks.
 *
 * @return A non-zero value in case of failure.
 */
uint32_t CC_PalDmaBufferMap(
                uint8_t                      *pDataBuffer,     /*!< [in] The address of the buffer to map. */
                uint32_t                      buffSize,         /*!< [in] The buffer size in Bytes. */
                CCPalDmaBufferDirection_t     copyDirection,    /*!< [in] The copy direction of the buffer, according to ::CCPalDmaBufferDirection_t:
                                              <ul><li>TO_DEVICE - the original buffer is the input to the operation,
                                              and this function should copy it to the temporary buffer,
                                              prior to the activating the HW on the temporary buffer.</li>
                                              <li>FROM_DEVICE - not relevant for this API.</li>
                                              <li>BI_DIRECTION - used when the cryptographic operation is "in-place", that is,
                                              the result of encryption or decryption is written over the original data
                                              at the same address. Should be treated by this API same as
                                              TO_DEVICE. </li></ul> */
                 uint32_t                     *pNumOfBlocks,    /*!< [in/out] <ul><li> In - The maximal number of blocks to fill.</li><li>Out - the actual number of blocks.</li></ul> */
                 CCPalDmaBlockInfo_t          *pDmaBlockList,   /*!< [out] The list of DMA-able blocks that the buffer maps to. */
                 CC_PalDmaBufferHandle         *dmaBuffHandle   /*!< [out] A handle to the private resources of the mapped buffer.*/ );


/**
 * @brief   This function is called by the CryptoCell runtime library after the HW is used.
 *      It unmaps a given buffer and frees its associated resources, if needed. It may unlock the buffer and flush it for CPU use.
 *      Once it is called, CryptoCell HW does not require any further access to this buffer.
 *      \note If the data buffer was already unmapped by the secure OS prior to calling the CryptoCell runtime library,
 *      this API does not have to perform any unmapping operation, and the actual unmapping can be done by the secure OS
 *      outside the context of the CryptoCell runtime library.
 * @return A non-zero value in case of failure.
 */
uint32_t CC_PalDmaBufferUnmap(uint8_t                       *pDataBuffer,   /*!< [in] The address of the buffer to unmap. */
                  uint32_t                       buffSize,      /*!< [in] The buffer size in Bytes. */
                  CCPalDmaBufferDirection_t      copyDirection, /*!< [in] The copy direction of the buffer, according to ::CCPalDmaBufferDirection_t:
                                              <ul><li>TO_DEVICE - not relevant for this API. </li>
                                              <li>FROM_DEVICE - the temporary buffer holds the output of the HW, and this
                                              API should copy it to the actual output buffer.</li>
                                              <li>BI_DIRECTION - used when the cryptographic operation is "in-place", that is,
                                              the result of encryption or decryption is written over the original data
                                              at the same address. Should be treated by this API same as
                                              FROM_DEVICE.</li></ul> */
                  uint32_t                       numOfBlocks,           /*!< [in] The number of DMA-able blocks that the buffer maps to. */
                  CCPalDmaBlockInfo_t            *pDmaBlockList,    /*!< [in] The list of DMA-able blocks that the buffer maps to. */
                  CC_PalDmaBufferHandle          dmaBuffHandle  /*!< [in] A handle to the private resources of the mapped buffer. */);


/**
 * @brief Allocates a DMA-contiguous buffer for CPU use, and returns its virtual address.
 *  Before passing the buffer to the CryptoCell HW, ::CC_PalDmaBufferMap should be called.
 *  \note The returned address must be aligned to 32bits.
 *
 *
 * @return A non-zero value in case of failure.
 */
uint32_t CC_PalDmaContigBufferAllocate(uint32_t          buffSize, /*!< [in] The buffer size in Bytes.*/
                       uint8_t           **ppVirtBuffAddr /*!< [out]  The virtual address of the allocated buffer.*/);



/**
 * @brief Frees resources previously allocated by ::CC_PalDmaContigBufferAllocate.
 *
 *
 * @return A non-zero value in case of failure.
 */
uint32_t CC_PalDmaContigBufferFree(uint32_t          buffSize, /*!< [in] The buffer size in Bytes. */
                   uint8_t           *pVirtBuffAddr /*!< [in] The virtual address of the buffer to free. */);



/**
 * @brief Checks whether the buffer is guaranteed to be a single contiguous DMA block.
 *
 *
 * @return TRUE if the buffer is guaranteed to be a single contiguous DMA block.
 * @return FALSE otherwise.
 */
uint32_t CC_PalIsDmaBufferContiguous(uint8_t                     *pDataBuffer, /*!< [in] The address of the user buffer. */
                     uint32_t                    buffSize   /*!< [in] The size of the user buffer. */);


#ifdef __cplusplus
}
#endif
/**
@}
 */
#endif


