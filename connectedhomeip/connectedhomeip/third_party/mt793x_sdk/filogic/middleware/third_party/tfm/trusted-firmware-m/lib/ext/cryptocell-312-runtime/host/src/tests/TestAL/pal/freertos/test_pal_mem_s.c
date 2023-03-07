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

#ifdef TZM
#include "FreeRTOS.h"
#include "test_pal_log.h"
#include "test_pal_mem.h"
#include "test_pal_mem_s.h"
#include <arm_cmse.h>
#endif

#ifndef TZM
/******************************************************************************/
void *Test_PalMalloc_s(size_t size)
{
    (void)size;
    return NULL;
}

/******************************************************************************/
void Test_PalFree_s(void *pvAddress)
{
    (void)pvAddress;
}

/******************************************************************************/
void *Test_PalRealloc_s(void *pvAddress, size_t newSize)
{
    (void)pvAddress;
    (void)newSize;
    return NULL;
}

/******************************************************************************/
void *Test_PalDMAContigBufferAlloc_s(size_t size)
{
    (void)size;
    return NULL;
}

/******************************************************************************/
void Test_PalDMAContigBufferFree_s(void *pvAddress)
{
    (void)pvAddress;
}

/******************************************************************************/
void *Test_PalDMAContigBufferRealloc_s(void *pvAddress, size_t newSize)
{
    (void)pvAddress;
    (void)newSize;
    return NULL;
}

/******************************************************************************/
unsigned long Test_PalGetDMABaseAddr_s(void)
{
    return 0;
}

/******************************************************************************/
unsigned long Test_PalGetUnmanagedBaseAddr_s(void)
{
    return 0;
}

/******************************************************************************/
uint32_t Test_PalMemInit_s(unsigned long newDMABaseAddr,
               unsigned long newUnmanagedBaseAddr,
               size_t DMAsize)
{
    (void)newDMABaseAddr;
    (void)newUnmanagedBaseAddr;
    (void)DMAsize;
    return 1;
}

/******************************************************************************/
uint32_t Test_PalMemFin_s(void)
{
    return 1;
}

#else

static unsigned long unmanagedBaseAddr_s = 0;
static uint8_t memInitialised_s = 0;

/******************************************************************************/
void *Test_PalMalloc_s(size_t size)
{
    /* A non-secure caller */
    if (cmse_is_nsfptr(&Test_PalMalloc_s))
        return NULL;

    /* A secure caller */
    if (!size)
        return NULL;

    return pvPortMalloc(size);
}

/******************************************************************************/
void Test_PalFree_s(void *pvAddress)
{
    /* A non-secure caller */
    if (cmse_is_nsfptr(&Test_PalFree_s))
        return;

    /* A secure caller */
    if (pvAddress == NULL)
        return;

    vPortFree(pvAddress);
}

/******************************************************************************/
void *Test_PalRealloc_s(void *pvAddress, size_t newSize)
{
    /* A non-secure caller */
    if (cmse_is_nsfptr(&Test_PalRealloc_s))
        return NULL;

    /* A secure caller */
    if (pvAddress == NULL)
        return NULL;

    return pvPortRealloc(pvAddress, newSize);
}

/******************************************************************************/
void *Test_PalDMAContigBufferAlloc_s(size_t size)
{
    return Test_PalMalloc_s(size);
}

/******************************************************************************/
void Test_PalDMAContigBufferFree_s(void *pvAddress)
{
    Test_PalFree_s(pvAddress);
}

/******************************************************************************/
void *Test_PalDMAContigBufferRealloc_s(void *pvAddress, size_t newSize)
{
    return Test_PalRealloc_s(pvAddress, newSize);
}

/******************************************************************************/
unsigned long Test_PalGetDMABaseAddr_s(void)
{
    return 0;
}

/******************************************************************************/
unsigned long Test_PalGetUnmanagedBaseAddr_s(void)
{
    return unmanagedBaseAddr_s;
}

/******************************************************************************/
uint32_t Test_PalMemInit_s(unsigned long newDMABaseAddr_s,
               unsigned long newUnmanagedBaseAddr_s,
               size_t SDMAsize)
{
    /* A non-secure caller */
    if (cmse_is_nsfptr(&Test_PalMemInit_s))
        return 1;

    /* A secure caller */
    if(memInitialised_s) {
        TEST_PRINTF_ERROR("Secure memory is already initialised");
        return 1;
    }

    (void)newDMABaseAddr_s;
    (void)SDMAsize;
    unmanagedBaseAddr_s = newUnmanagedBaseAddr_s;
    memInitialised_s = 1;

    return 0;
}

/******************************************************************************/
uint32_t Test_PalMemFin_s(void)
{
    /* A non-secure caller */
    if (cmse_is_nsfptr(&Test_PalMemFin_s))
        return 1;

    /* A secure caller */
    unmanagedBaseAddr_s = 0;
    memInitialised_s = 0;

    return 0;
}
#endif
