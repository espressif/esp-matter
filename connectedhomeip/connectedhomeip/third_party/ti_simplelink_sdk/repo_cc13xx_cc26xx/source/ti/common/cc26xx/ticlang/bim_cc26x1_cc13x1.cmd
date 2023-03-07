/******************************************************************************

 @file  bim_cc26x1_cc13x1.cmd

 @brief linker configuration file for TI-RTOS with Code Composer
        Studio.  Used for the Boot Image Manager (BIM) for on-chip over the air
        downloads.

        Imported Symbols
        Note: Linker defines are located in the CCS IDE project by placing them
        in
        Properties->Build->Linker->Advanced Options->Command File Preprocessing.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2012-2022, Texas Instruments Incorporated
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

/*******************************************************************************
 * CCS Linker configuration
 */

/* Retain interrupt vector table variable                                    */
--retain=g_pfnVectors
/* Override default entry point.                                             */
--entry_point ResetISR
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

/*******************************************************************************
 * Memory Sizes
 */

#define FLASH_BASE            0x00000000
#define GPRAM_BASE            0x11000000
#define RAM_BASE              0x20000000
#define ROM_BASE              0x10000000

#define FLASH_SIZE            0x00058000
#define RAM_SIZE              0x00008000

/*******************************************************************************
 * RAM
 */
/* Reserved RAM is that section of the RAM used by the ROM code.
 * For Secure BIM, the ECC library from ROM uses 0x80 bytes of
 * RAM space at the beginning of the RAM. Even though the ECC library
 * from ROM is not used by the non-secure BIM, to keep the linker
 * command file simple and uniform across the two variants, the
 * reserved section is defined here commonly for both variants.
 * ROM code actually uses slightly <0x300 bytes; Leaving this
 * section of the RAM untouched by BIM application
 */
#define RESERVED_RAM_SIZE           (0x300)

/*
 * Defines for BIM variable
 */
#define RAM_START             (RAM_BASE + RESERVED_RAM_SIZE)



#define RAM_END             (RAM_START + RAM_SIZE - RESERVED_RAM_SIZE - 1)

/* App is given full RAM range (minus ROM reserved offset) when Stack image is not present. */
#define RAM_BIM_SIZE          (RAM_END - RAM_START + 1)

/*******************************************************************************
 * Flash
 */

#define FLASH_START           FLASH_BASE

#define PAGE_SIZE             0x2000

/*
 * Bim controls the CCFG in page 31 as well as linking it's application here.
 * Any left over space could be claimed by another image.
 */
#define FLASH_BIM_START       0x56000 //FLASH_SIZE - PAGE_SIZE
#define FLASH_BIM_END         CERT_START - 1 //FLASH_CCFG_START - 1
#define FLASH_BIM_SIZE        ((FLASH_BIM_END) - (FLASH_BIM_START) + 1)

#define FLASH_CCFG_START      0x00057FA8
#define FLASH_CCFG_END        (FLASH_START + FLASH_SIZE - 1)
#define FLASH_CCFG_SIZE       ((FLASH_CCFG_END) - (FLASH_CCFG_START) + 1)

#define FLASH_FNPTR_START     FLASH_CCFG_START - 8 // Adding 4 bytes buffer between CCFG and function pointer 0x0001ffa0
#define FLASH_FNPTR_END       FLASH_FNPTR_START + 3 // 4 bytes function pointer 0x0001ffa3
#define FLASH_FNPTR_SIZE      4 // ((FLASH_FNPTR_END) - (FLASH_FNPTR_START) + 1)

//storing cert element in fixed flash region,
//length of region is size of cert_element_t
#define CERT_END              FLASH_FNPTR_START - 1
#define CERT_SIZE             0x4C        // For version 1 ECDSA-P256
#define CERT_START            CERT_END - CERT_SIZE + 1

/*******************************************************************************
 * Stack
 */

/* Create global constant that points to top of stack */
/* CCS: Change stack size under Project Properties    */
__STACK_TOP = __stack + __STACK_SIZE;

/*******************************************************************************
 * Main arguments
 */

/* Allow main() to take args */
/* --args 0x8 */

/*******************************************************************************
 * System Memory Map
 ******************************************************************************/
MEMORY
{
  /* Flash */
  FLASH_BIM  (RX) : origin = FLASH_BIM_START,  length = FLASH_BIM_SIZE
  FLASH_CCFG (RX) : origin = FLASH_CCFG_START, length = FLASH_CCFG_SIZE
  FLASH_FNPTR (RX) : origin = FLASH_FNPTR_START, length = FLASH_FNPTR_SIZE
  FLASH_CERT (RX) : origin = CERT_START, length = CERT_SIZE

  /* RAM */
  SRAM (RWX) : origin = RAM_START, length = RAM_BIM_SIZE
}

/*******************************************************************************
 * Section Allocation in Memory
 ******************************************************************************/
SECTIONS
{
  .intvecs        :   > FLASH_BIM_START
  .text           :   > FLASH_BIM
  .const          :   > FLASH_BIM
  .constdata      :   > FLASH_BIM
  .rodata         :   > FLASH_BIM
  .cinit          :   > FLASH_BIM
  .pinit          :   > FLASH_BIM
  .init_array     :   > FLASH_BIM
  .emb_text       :   > FLASH_BIM
  .cert_element   :   > FLASH_CERT
  .fnPtr          :   > FLASH_FNPTR
  .ccfg           :   > FLASH_CCFG (HIGH)

  .vtable         :   > SRAM
  .vtable_ram     :   > SRAM
  vtable_ram      :   > SRAM
  .data           :   > SRAM
  .bss            :   > SRAM
  .sysmem         :   > SRAM
  .stack          :   > SRAM (HIGH)
  .nonretenvar    :   > SRAM
}
