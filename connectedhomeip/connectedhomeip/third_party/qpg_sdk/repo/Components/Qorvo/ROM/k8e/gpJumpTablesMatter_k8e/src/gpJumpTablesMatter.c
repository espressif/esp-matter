/*
 * Copyright (c) 2021, Qorvo Inc
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include "gpHal_kx_mm.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#define MATTER_V1_VERSION_LOCATION 0x4814000
#define MATTER_V1_VERSION_PATTERN \
    {                             \
        0x10, 0xB5, 0x04, 0x46    \
    }

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

UInt8 gpJumpTablesMatter_GetVersion(void)
{
    // Check v1
    const UInt8 versionPattern1[4] = MATTER_V1_VERSION_PATTERN;
    if(!MEMCMP((UInt8*)MATTER_V1_VERSION_LOCATION, versionPattern1, 4))
    {
        return 1;
    }

    // Unknown version/no ROM present
    return 0xFF;
}

extern void matter_flash_jump_memset(void* s, size_t c, size_t n);
typedef void (*memset_ptr_t)(void* s, size_t c, size_t n);

void gpJumpTablesMatter_Init(void)
{
    // Dummy call to prevent optimization of flash jump table
    // Use reference from Flash jump table, but take virtual window offset into account
    // Values in the table are expected to be triggered from the virtual window out
    UInt8 dummy;
    memset_ptr_t memsetDummy;

    memsetDummy = (memset_ptr_t)((UIntPtr)matter_flash_jump_memset + (GP_MM_FLASH_VIRT_WINDOWS_START - GP_MM_FLASH_START));

    memsetDummy(&dummy, 0, sizeof(dummy));
}
