/*
 * Copyright (c) 2020, Qorvo Inc
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
 */

 /** @file "lzma_gpHal_flash.c"
 *
 *  Shim layer between LZMA decompression (Ubisys' assembly implementation) and gpHal_flash
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include "gpHal.h"

#ifndef LZMA_PROGRAM_PAGE
#define LZMA_PROGRAM_PAGE(addr, len, data)   gpHal_FlashWrite(addr, len, data)
#else
    extern gpHal_FlashError_t LZMA_PROGRAM_PAGE(FlashPtr address, UInt16 length, UInt32* data);
#endif

/*****************************************************************************
 *                    global Data Definitions
 *****************************************************************************/
static gpHal_FlashError_t lzma_programPage_result;

void lzma_gpHal_Flash_ResetStatus( void )
{
    lzma_programPage_result = gpHal_FlashError_Success;
}

gpHal_FlashError_t lzma_gpHal_Flash_GetStatus( void )
{
    return lzma_programPage_result;
}

/**
 * @brief Function called by lzma-decompression-v4-output.s
          On entry:
          r1 is the source address (typically a page buffer in SRAM)
          r5 is the start address of a flash page
          r6 contains the number of 32 bit words to write to the page buffer
 * @param[in] length (r0) number of 32bit words to be written (0x40 in 1 page)
 * @param[in] data   (r1) "page" in RAM memory that should be copied to @p address (in FLASH)
 * @param[in] address(r2) address to write the page to
 * @Note: The caller of this function is an assemble file that will restore
 *        registers r0 - r7, so these do not need to be restored upon exit
 */
void program_page( void )
{
    UInt16 length;
    UInt32* data;
    FlashPtr address;
    gpHal_FlashError_t result;

    asm (   "push     {r8-r12}"   ); // Only registers r0-r7 can be freely used here, back up all others
    asm (   "push     {r1,r5-r6}" ); // Back-up input registers to stack prior to passing them to LZMA_PROGRAM_PAGE, as the passing itself can taint them
    asm (   "LDR %0,  [SP, #4 * 0]"  // Load r1 into "data"
         : "=r"(data)
    );
    asm (   "LDR %0,  [SP, #4 * 1]"  // Load r5 into "address"
         : "=r"(address)
    );
    asm (   "LDR %0,  [SP, #4 * 2]"  // Load r6 into "length"
         : "=r"(length)
    );

    result = LZMA_PROGRAM_PAGE(address, length, data);

    if (result != gpHal_FlashError_Success)
    {
        // Error during flash write
        // Should be checked after lzma_Decode() has finished!
        lzma_programPage_result = result;
    }

    asm (   "pop     {r1,r5-r6}" ); // Restore when data address and length variables are no longer used
    asm (   "pop     {r8-r12}"   ); // Restore registers possibly tainted by LZMA_PROGRAM_PAGE or this function
}
