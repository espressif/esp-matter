/******************************************************************************

 @file  cc26xx_stack.cmd

 @brief CC2650F128 linker configuration file for TI-RTOS with
        Code Composer Studio.

        Imported Symbols
        Note: Linker defines are located in the CCS IDE project by placing them
        in
        Properties->Build->Linker->Advanced Options->Command File Preprocessing.

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
        ICALL_RAM0_START:   RAM start of BLE stack.
        ICALL_STACK0_START: Flash start of BLE stack.
        PAGE_AlIGN:         Align BLE stack boundary to a page boundary.
                            Aligns to Flash word boundary by default.

 Group: WCS, BTS
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

/*******************************************************************************
 * CCS Linker configuration
 */

/* Retain interrupt vector table variable                                    */
--retain=g_pfnVectors
/* Override default entry point.                                             */
//--entry_point ResetISR
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
#define RAM_BASE     0x20000000
#define ROM_BASE     0x10000000

#ifdef CC26X0ROM
  #define FLASH_SIZE 0x00020000
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
#define RAM_END               (RAM_START + RAM_SIZE - RESERVED_RAM_SIZE - 1)

/*******************************************************************************
 * Flash
 */

#define WORD_SIZE                 4

#ifdef CC26X0ROM
 #define PAGE_SIZE                0x1000
#endif /* CC26X0ROM */

#ifdef PAGE_ALIGN
  #define FLASH_MEM_ALIGN         PAGE_SIZE
#else
  #define FLASH_MEM_ALIGN         WORD_SIZE
#endif /* PAGE_ALIGN */

/* The last Flash page is reserved for the application. */
#define NUM_RESERVED_FLASH_PAGES  1
#define RESERVED_FLASH_SIZE       (NUM_RESERVED_FLASH_PAGES * PAGE_SIZE)

#define FLASH_START               FLASH_BASE
#define FLASH_END                 (FLASH_START + FLASH_SIZE - RESERVED_FLASH_SIZE - 1)

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
 * System Memory Map
 ******************************************************************************/
MEMORY
{
  /* EDITOR'S NOTE:
   * the FLASH and SRAM lengths can be changed by defining
   * ICALL_STACK0_START or ICALL_RAM0_START in
   * Properties->ARM Linker->Advanced Options->Command File Preprocessing.
   */

  /* Application stored in and executes from internal flash */
  FLASH (RX) : origin = FLASH_START, length = FLASH_END - FLASH_START + 1

  /* Application uses internal RAM for data */
  SRAM (RWX) : origin = RAM_START, length = RAM_END - RAM_START + 1
}

/*******************************************************************************
 * Section Allocation in Memory
 ******************************************************************************/
SECTIONS
{
	GROUP > FLASH(HIGH)  align FLASH_MEM_ALIGN
	{
		EntrySection
	  .text
	  .const
	  .constdata
	  .rodata
	  .cinit
	  .pinit
	  .init_array
	  .emb_text
	}

  .vtable         :   > SRAM(HIGH)
  .vtable_ram     :   > SRAM(HIGH)
  vtable_ram      :   > SRAM(HIGH)
  .data           :   > SRAM(HIGH)
  .bss            :   > SRAM(HIGH)
  .sysmem         :   > SRAM(HIGH)
  .nonretenvar    :   > SRAM(HIGH)
  .noinit         :   > SRAM(HIGH) TYPE=NOINIT
}
