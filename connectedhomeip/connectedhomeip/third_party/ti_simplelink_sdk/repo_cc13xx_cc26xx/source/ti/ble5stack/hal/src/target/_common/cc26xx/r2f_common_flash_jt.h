/******************************************************************************

 @file  r2f_common_flash_jt.h

 @brief This file contains the defines for every flash function call using the
        ROM-to-Flash Flash Jump Table.

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2014-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

#ifndef R2F_COMMON_FLASH_JT_H
#define R2F_COMMON_FLASH_JT_H

#if defined( COMMON_ROM_BUILD )

/*******************************************************************************
 * INCLUDES
 */

#include "hal_types.h"

/*******************************************************************************
 * EXTERNS
 */

// Common ROM's RAM table for pointers to ICall functions and flash jump tables.
// Note: This linker imported symbol is treated as a variable by the compiler.
// 0: iCall Dispatch Function Pointer
// 1: iCall Enter Critical Section Function Pointer
// 2: iCall Leave Critical Section Function Pointer
// 3: R2F Flash Jump Table Pointer
// 4: R2R Flash Jump Table Pointer
extern uint32 COMMON_RAM_BASE_ADDR;

/*******************************************************************************
 * CONSTANTS
 */

// Common ROM's RAM table offset to R2F flash jump table pointer.
#define COMMON_ROM_RAM_TABLE_R2F   3

// Defines used for the flash jump table routines that are not part of build.
// Note: Any change to this table must accompany a change to Flash_JT[]!
#define R2F_JT_LOCATION            (uint32 *)(&COMMON_RAM_BASE_ADDR+COMMON_ROM_RAM_TABLE_R2F)

#define R2F_JT_BASE                (*((uint32 **)R2F_JT_LOCATION))
#define R2F_JT_OFFSET(index)       (*(R2F_JT_BASE+(index)))

// ROM-to-Flash Functions
#define osal_mem_alloc             ((void *(*) (uint16))                       R2F_JT_OFFSET(0))
#define osal_mem_free              ((void  (*) (void *))                       R2F_JT_OFFSET(1))
#define osal_bm_free               ((void  (*) (void *))                       R2F_JT_OFFSET(2))
#define osal_memcpy                ((void *(*) (void *, const void *, uint32)) R2F_JT_OFFSET(3))
#define osal_memset                ((void *(*) (void *, uint8, int))           R2F_JT_OFFSET(4))
#define halAssertHandler           ((void  (*) (void))                         R2F_JT_OFFSET(5))
#endif // COMMON_ROM_BUILD

#endif /* R2F_COMMON_FLASH_JT_H */



