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

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Located in kernel directory */
#include "board_addrs.h"

#include "board_configs.h"
#include "test_pal_mem.h"
#include "test_pal_map_addrs.h"
#include "test_pal_log.h"

#ifdef TZM
#include "test_pal_mem_s.h"
#endif

/******************************************************************************/
uint32_t Test_HalBoardInit(void)
{
    unsigned long DMABaseAddr;
    unsigned long unmanagedBaseAddr;
#ifdef TZM
    unsigned long DMABaseAddr_s;
#endif

    /* maps the unmanaged memory base address */
    unmanagedBaseAddr = (unsigned long)Test_PalMapAddr(
                (void *)MPS2_GetUnmanagedbaseAddr(),
                NULL, NULL, 0, 0);

    if (!VALID_MAPPED_ADDR(unmanagedBaseAddr)) {
        TEST_PRINTF_ERROR("Error: mmap failed for unmanagedBaseAddr\n");
        goto end_with_error;
    }
    TEST_PRINTF("unmanagedBaseAddr = 0x%lx\n", unmanagedBaseAddr);

#ifndef TZM
    /* maps the DMA base address */
    DMABaseAddr = (unsigned long)Test_PalMapAddr(
                (void *)MPS2_GetDMAbaseAddr(),
                NULL, NULL, 0, 0);

    if (!VALID_MAPPED_ADDR(DMABaseAddr)) {
        TEST_PRINTF_ERROR("Error: mmap failed for DMABaseAddr\n");
        goto end_with_error;
    }

    TEST_PRINTF("DMABaseAddr = 0x%lx\n", DMABaseAddr);

    if (Test_PalMemInit(DMABaseAddr, unmanagedBaseAddr,
                MPS2_GetDMAAreaLen())) {
        TEST_PRINTF_ERROR("Error: MemInit failed\n");
        goto end_with_error;
    }

#else
    /* maps secure DMA base address */
    DMABaseAddr_s = (unsigned long)Test_PalMapAddr(
            (void *)MPS2_Get_S_DMAbaseAddr(),
            NULL, NULL, 0, 0);

    TEST_PRINTF("DMABaseAddr_s = 0x%lx\n", DMABaseAddr_s);

    if (Test_PalMemInit_s(DMABaseAddr_s, unmanagedBaseAddr,
                MPS2_Get_S_DMAAreaLen())) {
        TEST_PRINTF_ERROR("Error: Secure MemInit failed\n");
        goto end_with_error;
    }

    /* maps non-secure DMA base address */
    DMABaseAddr = (unsigned long)Test_PalMapAddr(
            (void *)MPS2_Get_NS_DMAbaseAddr(),
            NULL, NULL, 0, 0);

    TEST_PRINTF("DMABaseAddr_ns = 0x%lx\n", DMABaseAddr);

    if (Test_PalMemInit(DMABaseAddr, 0,
                MPS2_Get_NS_DMAAreaLen())) {
        TEST_PRINTF_ERROR("Error: non-secure MemInit failed\n");
        goto end_with_error;
    }
#endif

    return 0;

    end_with_error:
    Test_HalBoardFree();
    return 1;
}

/******************************************************************************/
void Test_HalBoardFree(void)
{
#ifndef TZM
    /* Unmaps DMA base address and unmanaged memory base address */
    if (Test_PalGetUnmanagedBaseAddr() != 0) {
        Test_PalUnmapAddr((void *)Test_PalGetUnmanagedBaseAddr(),
                MPS2_GetUnmanagedAreaLen());
    }

    if (Test_PalGetDMABaseAddr() != 0) {
        Test_PalUnmapAddr((void *)Test_PalGetDMABaseAddr(),
                MPS2_GetDMAAreaLen());
    }

#else
    /* Unmaps secure DMA base address and unmanaged memory base address */
    if (Test_PalGetUnmanagedBaseAddr_s() != 0) {
        Test_PalUnmapAddr((void *)Test_PalGetUnmanagedBaseAddr_s(),
                MPS2_GetUnmanagedAreaLen());
    }

    if (Test_PalGetDMABaseAddr_s() != 0) {
        Test_PalUnmapAddr((void *)Test_PalGetDMABaseAddr_s(),
                MPS2_Get_S_DMAAreaLen());
    }

    /* Unmaps non-secure DMA base address */
    if (Test_PalGetDMABaseAddr() != 0) {
        Test_PalUnmapAddr((void *)Test_PalGetDMABaseAddr(),
                MPS2_Get_NS_DMAAreaLen());
    }

#endif
}
