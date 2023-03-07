/*
 * Copyright (c) 2001-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board_addrs.h"

/******************************************************************************/
unsigned long MPS2_GetDMAbaseAddr(void)
{
    return (unsigned long)MPS2_PLUS_MEM_DMA_BASE_ADDR;
}

/******************************************************************************/
unsigned long MPS2_GetDMAAreaLen(void)
{
    return (unsigned long)MPS2_PLUS_MEM_DMA_AREA_LEN;
}

/******************************************************************************/
unsigned long MPS2_GetUnmanagedbaseAddr(void)
{
    return (unsigned long)MPS2_PLUS_MEM_UNMANAGED_BASE_ADDR;
}

/******************************************************************************/
unsigned long MPS2_GetUnmanagedAreaLen(void)
{
    return (unsigned long)MPS2_PLUS_MEM_UNMANAGED_AREA_LEN;
}
