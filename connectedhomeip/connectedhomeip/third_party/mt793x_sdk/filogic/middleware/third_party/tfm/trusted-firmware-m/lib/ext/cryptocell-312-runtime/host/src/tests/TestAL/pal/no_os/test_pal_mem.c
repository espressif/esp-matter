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

#include <stdio.h>
#include <stdlib.h>
#include "test_pal_log.h"
#include "test_pal_mem.h"

static unsigned long dmableBaseAddr = 0;
static unsigned long unmanagedBaseAddr = 0;
static unsigned long dmableEndAddr = 0;
static unsigned long nextToAlloc = 0;

static uint8_t memInitialised = 0;

/******************************************************************************/
void *Test_PalMalloc(size_t size)
{
    if (!size)
        return NULL;

    return (void *)malloc(size);
}

/******************************************************************************/
void Test_PalFree(void *pvAddress)
{
    if (pvAddress == NULL)
        return;

    free(pvAddress);
}

/******************************************************************************/
void *Test_PalRealloc(void *pvAddress, size_t newSize)
{
    if (pvAddress == NULL)
        return NULL;

    return realloc(pvAddress, newSize);
}

/******************************************************************************/
void *Test_PalDMAContigBufferAlloc(size_t size)
{
    unsigned long currAddr;

    if(!size) {
        TEST_PRINTF_ERROR("Size cannot be zero");
        goto end_with_error;
    }

    /* increase size with the requested alignment in our case 0x10 */
    size += 0x10;

    if((dmableBaseAddr == 0) || (dmableEndAddr == 0)) {
        TEST_PRINTF_ERROR("DMA memory allocator wasn't initialized");
        goto end_with_error;
    }

    /* Exceeds the maximum memory size */
    if(nextToAlloc + size > dmableEndAddr) {
        nextToAlloc = dmableBaseAddr;

        if(nextToAlloc + size > dmableEndAddr) {
            TEST_PRINTF_ERROR("End of range! size needs to be less "
                "than %d",(dmableEndAddr - nextToAlloc));
            goto end_with_error;
        }
    }

    currAddr = nextToAlloc;
    nextToAlloc += size;

    return (void *)((currAddr + 0x10) & 0xfffffff0);

end_with_error:
    return NULL;
}

/******************************************************************************/
void Test_PalDMAContigBufferFree(void *pvAddress)
{
    (void)pvAddress;
}

/******************************************************************************/
void *Test_PalDMAContigBufferRealloc(void *pvAddress, size_t newSize)
{
    (void)pvAddress;
    (void)newSize;
    return NULL;
}

/******************************************************************************/
unsigned long Test_PalGetDMABaseAddr(void)
{
    return dmableBaseAddr;
}

/******************************************************************************/
unsigned long Test_PalGetUnmanagedBaseAddr(void)
{
    return unmanagedBaseAddr;
}

/******************************************************************************/
uint32_t Test_PalMemInit(unsigned long newDMABaseAddr,
             unsigned long newUnmanagedBaseAddr,
             size_t DMAsize)
{
    if(memInitialised) {
        TEST_PRINTF_ERROR("Memory is already initialised");
        return 1;
    }

    dmableBaseAddr = newDMABaseAddr;
    dmableEndAddr = newDMABaseAddr + DMAsize;
    nextToAlloc = newDMABaseAddr;
    unmanagedBaseAddr = newUnmanagedBaseAddr;
    memInitialised = 1;

    return 0;
}

/******************************************************************************/
uint32_t Test_PalMemFin(void)
{
    dmableBaseAddr = 0;
    unmanagedBaseAddr = 0;
    dmableEndAddr = 0;
    nextToAlloc = 0;
    memInitialised = 0;

    return 0;
}
