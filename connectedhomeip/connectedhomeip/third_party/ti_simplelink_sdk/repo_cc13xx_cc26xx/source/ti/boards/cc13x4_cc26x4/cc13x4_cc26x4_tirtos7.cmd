/*
 * Copyright (c) 2020-2021, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

--stack_size=1024   /* C stack is also used for ISR stack */

HEAPSIZE = 0x4000;  /* Size of heap buffer used by HeapMem */

/* Retain interrupt vector table variable                                    */
--retain=g_pfnVectors
/* Allow main() to take args                                                 */
--args 0x8
/* Suppress warnings and errors:                                             */
/* - 10063: Warning about entry point not being _c_int00                     */
/* - 16011, 16012: 8-byte alignment errors. Observed when linking in object  */
/*   files compiled using Keil (ARM compiler)                                */
--diag_suppress=10063,16011,16012

/* The following command line options are set as part of the CCS project.    */
/* If you are building using the command line, or for some reason want to    */
/* define them here, you can uncomment and modify these lines as needed.     */
/* If you are using CCS for building, it is probably better to make any such */
/* modifications in your CCS project and leave this file alone.              */
/*                                                                           */
/* --heap_size=0                                                             */
/* --stack_size=256                                                          */
/* --library=rtsv7M3_T_le_eabi.lib                                           */

/* The starting address of the application.  Normally the interrupt vectors  */
/* must be located at the beginning of the application.                      */
#define FLASH_BASE              0x0
#define FLASH_SIZE              0x100000
#define RAM_BASE                0x20000000
#define RAM_SIZE                0x40000
#define GPRAM_BASE              0x11000000
#define GPRAM_SIZE              0x2000
#define CCFG_BASE               0x50000000
#define CCFG_SIZE               0x800


/* System memory map */

MEMORY
{
    /* Application stored in and executes from internal flash
     * Note there are some holes in this memory due to issues with
     * first-sampling silicon.
     */
    FLASH_1 (RX) : origin = 0x0,     length = 0x1C000
    FLASH_2 (RX) : origin = 0x24000, length = 0x38000
    FLASH_3 (RX) : origin = 0x64000, length = 0x38000
    FLASH_4 (RX) : origin = 0xA4000, length = 0x38000
    FLASH_5 (RX) : origin = 0xE4000, length = 0x1C000

    /* Application uses internal RAM for data */
    SRAM (RWX) : origin = RAM_BASE, length = RAM_SIZE
    /* Application can use GPRAM region as RAM if cache is disabled in the CCFG
    (DEFAULT_CCFG_SIZE_AND_DIS_FLAGS.SET_CCFG_SIZE_AND_DIS_FLAGS_DIS_GPRAM = 0) */
    GPRAM (RWX): origin = GPRAM_BASE, length = GPRAM_SIZE
    /* Configuration region */
    CCFG (RW) : origin = CCFG_BASE, length = CCFG_SIZE
    /* Explicitly placed off target for the storage of logging data.
     * The ARM memory map allocates 1 GB of external memory from 0x60000000 - 0x9FFFFFFF.
     * Unlikely that all of this will be used, so we are using the upper parts of the region.
     * ARM memory map: https://developer.arm.com/documentation/ddi0337/e/memory-map/about-the-memory-map*/
    LOG_DATA (R) : origin = 0x90000000, length = 0x40000        /* 256 KB */
}

/* Section allocation in memory */

SECTIONS
{
    .intvecs        :   > FLASH_BASE
    .text           :   >> FLASH_1|FLASH_2|FLASH_3|FLASH_4|FLASH_5
    .TI.ramfunc     : {} load=FLASH_2, run=SRAM, table(BINIT)
    .const          :   >> FLASH_1|FLASH_2|FLASH_3|FLASH_4|FLASH_5
    .constdata      :   >> FLASH_1|FLASH_2|FLASH_3|FLASH_4|FLASH_5
    .rodata         :   >> FLASH_1|FLASH_2|FLASH_3|FLASH_4|FLASH_5
    .binit          :   > FLASH_1|FLASH_2|FLASH_3|FLASH_4|FLASH_5
    .cinit          :   > FLASH_1|FLASH_2|FLASH_3|FLASH_4|FLASH_5
    .pinit          :   > FLASH_1|FLASH_2|FLASH_3|FLASH_4|FLASH_5
    .init_array     :   > FLASH_1|FLASH_2|FLASH_3|FLASH_4|FLASH_5
    .emb_text       :   >> FLASH_1|FLASH_2|FLASH_3|FLASH_4|FLASH_5
    .ccfg           :   > CCFG

    .vtable         :   > SRAM
    .vtable_ram     :   > SRAM
     vtable_ram     :   > SRAM
    .data           :   > SRAM
    .bss            :   > SRAM
    .sysmem         :   > SRAM
    .stack          :   > SRAM (HIGH)
    .nonretenvar    :   > SRAM
    /* Heap buffer used by HeapMem */
    .priheap   : {
        __primary_heap_start__ = .;
        . += HEAPSIZE;
        __primary_heap_end__ = .;
    } > SRAM align 8
    .gpram          :   > GPRAM

    .log_data       :   > LOG_DATA, type = COPY
}

--symbol_map __TI_STACK_SIZE=__STACK_SIZE
--symbol_map __TI_STACK_BASE=__stack

-u_c_int00
--retain "*(.resetVecs)"
--retain "*(.vecs)"

SECTIONS
{
    .resetVecs: load > 0
    .vecs: load > 0x20000000, type = NOLOAD
}
