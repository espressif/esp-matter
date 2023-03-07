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

#ifndef TEST_PAL_MEM_S_H_
#define TEST_PAL_MEM_S_H_

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/*      Secure API - ONLY WHEN TRUSTZONE-M IS SUPPORTED                       */
/*      Otherwise, please use only test_pal_mem API.                          */
/******************************************************************************/

/******************************************************************************/
/*
 * @brief This function allocates SECURE "size" bytes.
 *
 * @param[in] size in bytes.
 *
 * @param[out]
 *
 * @return pointer to the allocated memory.
 */
void *Test_PalMalloc_s(size_t size);

/******************************************************************************/
/*
 * @brief This function frees SECURE allocated memory pointed by pvAddress.
 *
 * @param[in] pvAddress - pointer to the allocated memory.
 *
 * @param[out]
 *
 * @return
 */
void Test_PalFree_s(void *pvAddress);

/******************************************************************************/
/*
 * @brief This function changes the size of a SECURE memory block pointed by
 * pvAddress.
 * If the function fails to allocate the requested block of memory:
 * 1. a null pointer is returned.
 * 2. The memory block pointed by argument pvAddress is NOT deallocated.
 *
 * @param[in]
 * pvAddress - Pointer to the allocated memory.
 * newSize - New size.
 *
 * @param[out]
 *
 * @return - a pointer to the new allocated memory or NULL in case of failure.
 */
void *Test_PalRealloc_s(void *pvAddress, size_t newSize);

/******************************************************************************/
/*
 * @brief This function allocates a DMA-contiguous buffer in a SECURE memory
 * region and returns its address.
 *
 * @param[in] size - Buffer size in bytes.
 *
 * @param[out]
 *
 * @return an address of the secure allocated buffer.
 */
void *Test_PalDMAContigBufferAlloc_s(size_t size);

/******************************************************************************/
/*
 * @brief This function frees resources in a SECURE region previously allocated
 * by Test_PalDMAContigBufferAlloc_s.
 *
 * @param[in] pvAddress - address of the secure allocated buffer.
 *
 * @param[out]
 *
 * @return
 */
void Test_PalDMAContigBufferFree_s(void *pvAddress);

/******************************************************************************/
/*
 * @brief This function changes the size of the SECURE memory block pointed by
 * pvAddress.
 * If the function fails to allocate the requested block of memory:
 * 1. a null pointer is returned.
 * 2. The memory block pointed by argument pvAddress is NOT deallocated.
 *
 * @param[in]
 * pvAddress - Pointer to the secure allocated memory.
 * newSize - New size in bytes.
 *
 * @param[out]
 *
 * @return - a pointer to the new secure allocated memory.
 */
void *Test_PalDMAContigBufferRealloc_s(void *pvAddress, size_t newSize);

/******************************************************************************/
/*
 * @brief This function returns the SECURE DMA base address, i.e. the start
 * address of the SECURE DMA region.
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return - Secure DMABaseAddr.
 */
unsigned long Test_PalGetDMABaseAddr_s(void);

/******************************************************************************/
/*
 * @brief This function returns the SECURE unmanaged base address.
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return - DMABaseAddr.
 */
unsigned long Test_PalGetUnmanagedBaseAddr_s(void);

/******************************************************************************/
/*
 * @brief This function initializes the SECURE DMA memory management.
 *
 * @param[in]
 * newDMABaseAddr_s - new secure DMA start address.
 * newUnmanagedBaseAddr_s - new secure unmanaged start address.
 * SDMAsize - secure DMA region size.
 *
 * @param[out]
 *
 * @return rc - 0 for success, 1 for failure.
 */
uint32_t Test_PalMemInit_s(unsigned long newDMABaseAddr_s,
               unsigned long newUnmanagedBaseAddr_s,
               size_t SDMAsize);

/******************************************************************************/
/*
 * @brief This function sets the SECURE memory management driver to its initial
 * state.
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return rc - 0 for success, 1 for failure.
 */
uint32_t Test_PalMemFin_s(void);

#ifdef __cplusplus
}
#endif

#endif /* TEST_PAL_MEM_S_H_ */
