/******************************************************************************

 @file  cc26xx_app_oad_onchip.cmd

 @brief CC2650F128 linker configuration file for TI-RTOS with Code Composer
        Studio.  For use with OAD on chip applications (Image A or B).

        Imported Symbols
        Note: Linker defines are located in the CCS IDE project by placing them
        in
        Properties->Build->Linker->Advanced Options->Command File Preprocessing.

        CACHE_AS_RAM:       Disable system cache to be used as GPRAM for
                            additional volatile memory storage.
        CCxxxxROM:          Device Name (e.g. CC2650). In order to define this
                            symbol, the tool chain requires that it be set to
                            a specific value, but in fact, the actual value does
                            not matter as it is not used in the linker control
                            file. The only way this symbol is used is based on
                            whether it is defined or not, not its actual value.
                            There are other linker symbols that do specifically
                            set their value to 1 to indicate R1, and 2 to
                            indicate R2, and these values are checked and do make
                            a difference. However, it would appear confusing if
                            the device name's value did not correspond to the
                            value set in other linker symbols. In order to avoid
                            this confusion, when the symbol is defined, it should
                            be set to the value of the device's ROM that it
                            corresponds so as to look and feel consistent. Please
                            note that a device name symbol should always be
                            defined to avoid side effects from default values
                            that may not be correct for the device being used.
        IMAGE_A:            Fixed Image A build.  When not present, it is
                            assumed to be an Image B build.
        ICALL_RAM0_START:   RAM start of BLE stack.
        ICALL_STACK0_START: Flash start of BLE stack.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2013-2022, Texas Instruments Incorporated
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

#define FLASH_BASE   0x00000000
#define GPRAM_BASE   0x11000000
#define RAM_BASE     0x20000000
#define ROM_BASE     0x10000000

#ifdef CC26X0ROM
  #define FLASH_SIZE 0x00020000
  #define GPRAM_SIZE 0x00002000
  #define RAM_SIZE   0x00005000
  #define ROM_SIZE   0x0001C000
#endif /* CC26X0ROM */

/*******************************************************************************
 * Memory Definitions
 ******************************************************************************/

/*******************************************************************************
 * RAM
 */

#ifdef CC26X0ROM
  #if CC26X0ROM == 2
    #define RESERVED_RAM_SIZE 0x00000C00
  #elif CC26X0ROM == 1
    #define RESERVED_RAM_SIZE 0x00000718
  #endif /* CC26X0ROM */
#endif /* CC26X0ROM */

#define RAM_START             RAM_BASE


#ifdef ICALL_RAM0_START
  #define RAM_END             (ICALL_RAM0_START - 1)
#else
  #define RAM_END             (RAM_START + RAM_SIZE - RESERVED_RAM_SIZE - 1)
#endif /* ICALL_RAM0_START */

/*******************************************************************************
 * Flash
 */

#define FLASH_START              FLASH_BASE

#ifdef CC26X0ROM
  #define PAGE_SIZE              0x1000
#endif /* CC26X0ROM */

#ifdef CC26X0ROM
  #define PAGE_MASK              0xFFFFF000
#endif /* CC26X0ROM */

/* OAD Image Header is 16 bytes */
#define FLASH_OAD_IMG_HDR_SIZE   0x00000010

/* OAD does not support same page word aligned images. */
#ifdef ICALL_STACK0_START
#define ADJ_ICALL_STACK0_START   (ICALL_STACK0_START & PAGE_MASK)
#endif /* ICALL_STACK0_START */

/*
 * Image A links in RTOS in ROM symbols to reduce this image's length.
 * Image A is a fixed image and its ending address is the lowest page aligned
 * address in which it can link.
 * Image B begins on the next highest adjacent page.
 * The first 16 bytes of Image B and the header which is used to store OAD meta
 * information and BIM assumes this information is stored here.  Image B follows
 * its header on the adjacent flash word.
 */
#define FLASH_IMG_A_START        FLASH_START
#define FLASH_IMG_A_END          (FLASH_START + (PAGE_SIZE * 7))
#define FLASH_IMG_A_SIZE         ((FLASH_IMG_A_END) - (FLASH_IMG_A_START))
#define FLASH_IMG_B_HDR_START    FLASH_IMG_A_END
#define FLASH_IMG_B_START        ((FLASH_IMG_B_HDR_START) + (FLASH_OAD_IMG_HDR_SIZE))

/*
 * When the Stack image is not present, Image B assume it contains all of the
 * remaining Flash pages above the contiguous portion of Image A's highest page
 * with the exception of the last 2 pages.  This assumes the application wishes
 * to retain a 1 page SNV storage area on the second to last page.
 */
#ifdef ICALL_STACK0_START
#define FLASH_IMG_B_END          ADJ_ICALL_STACK0_START
#else
#define FLASH_IMG_B_END          (FLASH_START + (FLASH_SIZE - (PAGE_SIZE*2))
#endif /* ICALL_STACK0_START */

#define FLASH_IMG_B_SIZE         (FLASH_IMG_B_END - FLASH_IMG_B_START)

/*******************************************************************************
 * Stack
 */

/* Create global constant that points to top of stack */
/* CCS: Change stack size under Project Properties    */
__STACK_TOP = __stack + __STACK_SIZE;

/*******************************************************************************
 * GPRAM
 */

#ifdef CACHE_AS_RAM
  #define GPRAM_START GPRAM_BASE
  #define GPRAM_END   (GPRAM_START + GPRAM_SIZE - 1)
#endif /* CACHE_AS_RAM */

/*******************************************************************************
 * ROV
 * These symbols are used by ROV2 to extend the valid memory regions on device.
 * Without these defines, ROV will encounter a Java exception when using an
 * autosized heap. This is a posted workaround for a known limitation of
 * RTSC/rta. See: https://bugs.eclipse.org/bugs/show_bug.cgi?id=487894
 *
 * Note: these do not affect placement in RAM or FLASH, they are only used
 * by ROV2, see the BLE Stack User's Guide for more info on a workaround
 * for ROV Classic
 *
 */
__UNUSED_SRAM_start__ = RAM_BASE;
__UNUSED_SRAM_end__ = RAM_BASE + RAM_SIZE;

__UNUSED_FLASH_start__ = FLASH_BASE;
__UNUSED_FLASH_end__ = FLASH_BASE + FLASH_SIZE;

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
  /* EDITOR'S NOTE:
   * the FLASH and SRAM lengths can be changed by defining
   * ICALL_STACK0_START or ICALL_RAM0_START in
   * Properties->ARM Linker->Advanced Options->Command File Preprocessing.
   *
   * IMAGE_A definition is defined at
   * Properties->ARM Linker->Advanced Options->Command File Preprocessing.
   */

  /* Flash */
  #ifdef IMAGE_A
      /* Image A is fixed, it does not matter where the header is linked. */
      #define IMG_HDR FLASH_IMG_A_START
      FLASH (RX) : origin = FLASH_IMG_A_START, length = FLASH_IMG_A_SIZE
  #else /* IMAGE B */
      FLASH (RX) : origin = FLASH_IMG_B_START, length = FLASH_IMG_B_SIZE
  #endif /* IMAGE_A */

  /* RAM */
  SRAM (RWX) : origin = RAM_START, length = RAM_END - RAM_START + 1

  #ifdef CACHE_AS_RAM
      GPRAM(RWX) : origin = GPRAM_START, length = GPRAM_SIZE
  #endif /* CACHE_AS_RAM */
}

/*******************************************************************************
 * Section Allocation in Memory
 ******************************************************************************/
SECTIONS
{
  .intvecs        :   > FLASH
  .text           :   > FLASH
  .const          :   > FLASH
  .constdata      :   > FLASH
  .rodata         :   > FLASH
  .cinit          :   > FLASH
  .pinit          :   > FLASH
  .init_array     :   > FLASH
  .emb_text       :   > FLASH

  GROUP > SRAM
  {
    .data
    #ifndef CACHE_AS_RAM
    .bss
    #endif /* CACHE_AS_RAM */
    .vtable
    .vtable_ram
    vtable_ram
    .sysmem
    .nonretenvar
    /*This keeps ll.o objects out of GPRAM, if no ll.o would be placed here
      the warning #10068 is supressed.*/
    #ifdef CACHE_AS_RAM
    ll_bss
    {
      --library=*ll_*.a<ll.o> (.bss)
      --library=*ll_*.a<ll_ae.o> (.bss)
    }
    #endif /* CACHE_AS_RAM */
  } LOAD_END(heapStart)

  .stack            :   >  SRAM (HIGH) LOAD_START(heapEnd)

  #ifdef CACHE_AS_RAM

  .bss :
  {
    *(.bss)
  } > GPRAM
  #endif /* CACHE_AS_RAM */
}