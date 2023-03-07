/*******************************************************************************
* The confidential and proprietary information contained in this file may      *
* only be used by a person authorised under and to the extent permitted        *
* by a subsisting licensing agreement from ARM Limited or its affiliates.      *
*   (C) COPYRIGHT [2001-2017] ARM Limited or its affiliates.                   *
*       ALL RIGHTS RESERVED                                                    *
* This entire notice must be reproduced on all copies of this file             *
* and copies of this file may only be made by a person if such person is       *
* permitted to do so under the terms of a subsisting license agreement         *
* from ARM Limited or its affiliates.                                          *
*******************************************************************************/

#ifndef TEST_PAL_MEM_H_
#define TEST_PAL_MEM_H_

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* Note: When TrustZone-M is supported, the following functions use NON SECURE*/
/* memory by default.                                                         */
/******************************************************************************/

/******************************************************************************/
/*
 * @brief This function allocates "size" bytes.
 * When TZM is supported, it is used only for NON SECURE memory allocations.
 *
 * @param[in] size in bytes.
 *
 * @param[out]
 *
 * @return pointer to the allocated memory.
 */
void *Test_PalMalloc(size_t size);

/******************************************************************************/
/*
 * @brief This function frees allocated memory pointed by pvAddress.
 * When TZM is supported, it is used only for NON SECURE memory blocks.
 *
 * @param[in] pvAddress - pointer to the allocated memory.
 *
 * @param[out]
 *
 * @return
 */
void Test_PalFree(void *pvAddress);

/******************************************************************************/
/*
 * @brief This function changes the size of the memory block pointed by
 * pvAddress.
 * If the function fails to allocate the requested block of memory:
 * 1. a null pointer is returned.
 * 2. The memory block pointed by argument pvAddress is NOT deallocated.
 * When TZM is supported, it is used only for NON SECURE memory blocks.
 *
 * @param[in]
 * pvAddress - Pointer to the allocated memory.
 * newSize - New size.
 *
 * @param[out]
 *
 * @return - a pointer to the new allocated memory or NULL in case of failure.
 */
void *Test_PalRealloc(void *pvAddress, size_t newSize);

/******************************************************************************/
/*
 * @brief This function allocates a DMA-contiguous buffer and returns its
 * address.
 * When TZM is supported, it is used only for NON SECURE buffer allocations.
 *
 * @param[in] size - Buffer size in bytes.
 *
 * @param[out]
 *
 * @return an address of the allocated buffer.
 */
void *Test_PalDMAContigBufferAlloc(size_t size);

/******************************************************************************/
/*
 * @brief This function frees resources previously allocated by
 * Test_PalDMAContigBufferAlloc.
 *
 * When TZM is supported, it is used only for NON SECURE buffers.
 *
 * @param[in] pvAddress - address of the allocated buffer.
 *
 * @param[out]
 *
 * @return
 */
void Test_PalDMAContigBufferFree(void *pvAddress);

/******************************************************************************/
/*
 * @brief This function changes the size of the memory block pointed by
 * pvAddress.
 * If the function fails to allocate the requested block of memory:
 * 1. a null pointer is returned.
 * 2. The memory block pointed by argument pvAddress is NOT deallocated.
 * When TZM is supported, it is used only for NON SECURE buffers.
 *
 * @param[in]
 * pvAddress - Pointer to the allocated memory.
 * newSize - New size in bytes.
 *
 * @param[out]
 *
 * @return - a pointer to the new allocated memory.
 */
void *Test_PalDMAContigBufferRealloc(void *pvAddress, size_t newSize);

/******************************************************************************/
/*
 * @brief This function returns DMA base address, i.e. the start address
 * of the DMA region.
 * When TZM is supported, it returns the NON SECURE DMA base address.
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return - DMABaseAddr.
 */
unsigned long Test_PalGetDMABaseAddr(void);

/******************************************************************************/
/*
 * @brief This function returns the unmanaged base address.
 * When TZM is supported, it returns the NON SECURE unmanaged base address.
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return - UnmanagedBaseAddr.
 */
unsigned long Test_PalGetUnmanagedBaseAddr(void);

/******************************************************************************/
/*
 * @brief This function initializes DMA memory management.
 * When TZM is supported, it initializes the NON SECURE DMA memory management.
 *
 * @param[in]
 * newDMABaseAddr - new DMA start address.
 * newUnmanagedBaseAddr - new unmanaged start address.
 * DMAsize - DMA region size.
 *
 * @param[out]
 *
 * @return rc - 0 for success, 1 for failure.
 */
uint32_t Test_PalMemInit(unsigned long newDMABaseAddr,
             unsigned long newUnmanagedBaseAddr,
             size_t DMAsize);

/******************************************************************************/
/*
 * @brief This function sets this driver to its initial state.
 * When TZM is supported, it sets the NON SECURE management to its initial
 * state.
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return rc - 0 for success, 1 for failure.
 */
uint32_t Test_PalMemFin(void);

#ifdef __cplusplus
}
#endif

#endif /* TEST_PAL_MEM_H_ */
