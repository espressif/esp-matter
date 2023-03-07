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
#include <stdint.h>
#include <stdlib.h>
#include "test_pal_log.h"
#include "test_pal_mem.h"

static unsigned long unmanagedBaseAddr = 0;
static uint8_t memInitialised = 0;

/******************************************************************************/
void *Test_PalMalloc(size_t size)
{
    if (!size)
        return NULL;

    return malloc(size);
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
    return Test_PalMalloc(size);
}

/******************************************************************************/
void Test_PalDMAContigBufferFree(void *pvAddress)
{
    Test_PalFree(pvAddress);
}

/******************************************************************************/
void *Test_PalDMAContigBufferRealloc(void *pvAddress, size_t newSize)
{
    return Test_PalRealloc(pvAddress, newSize);
}

/******************************************************************************/
unsigned long Test_PalGetDMABaseAddr(void)
{
    return 0;
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

    (void)newDMABaseAddr;
    (void)DMAsize;
    unmanagedBaseAddr = newUnmanagedBaseAddr;
    memInitialised = 1;

    return 0;
}

/******************************************************************************/
uint32_t Test_PalMemFin(void)
{
    unmanagedBaseAddr = 0;
    memInitialised = 0;

    return 0;
}

