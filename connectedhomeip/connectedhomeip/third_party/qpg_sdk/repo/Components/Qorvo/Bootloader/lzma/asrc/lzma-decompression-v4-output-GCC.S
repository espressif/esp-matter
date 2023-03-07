// lzma-decompression-v4-output.s : implementation file
//
// Copyright(C) 2022 Qorvo Inc.
// All rights reserved.
//
// Copyright(C) 2020 ubisys technologies GmbH, Duesseldorf, Germany.
// All rights reserved.
//
// www.ubisys.de
// support@ubisys.de
//
// This is an implementation of LZMA decompression written by Igor Pavlov and
// hand-optimized by John F. Reiser, adapted by ubisys to the IAR assembler
// and C/C++ EABI addressing some issues with parameter passing. This version
// is also using a thumb entry point. Modifications by Qorvo 
// to be compatible with the GCC assembler were done in 2022.
//
// This module provides generic functions for writing to a paged flash device
// i.e. implements a page buffer and triggers a platform-specific page program
// function to actually write the data page

#include "qorvo_internals.h"

    .syntax unified
    .thumb
    .align 1

    .extern program_page

#if   defined(GP_DIVERSITY_GPHAL_K8E)
pageSizeShift = 9
#else
#error "Device not supported!"
#endif
granularityShift = 2

    .bss
    .align          granularityShift

pageBuffer:     .skip 1<<pageSizeShift
pageAddress:    .skip 4

    .text

/////////////////////////////////////////////////////////////////////////////
// write_output: write to the page buffer and emit the page when it is full,
// erase the sector when writing to the first page in the sector. For SAM7S
// parts a single page comprises an erase sector, and there is a command to
// erase and program in one operation, so no special erase step is required
//
// On entry:
//  r2 contains the target address in the output buffer, which is located
//  in flash memory
//  r4 contains a byte to be written to above specified address

    .section .text.write_output, "ax", %progbits
    .thumb_func
    .type   write_output, %function
    .global write_output

write_output:   push    { r0 - r7, lr }

                    movs    r0, #1
                    lsls    r0, r0, #pageSizeShift      // r0 = number of bytes
                    lsrs    r6, r0, #2                  // r6 = number of 32-bit words
                    subs    r0, r0, #1                  // r0 = (1 << pageSizeShift) - 1
                    lsrs    r5, r2, #pageSizeShift
                    lsls    r5, r5, #pageSizeShift      // r5 = @first byte on page
                    movs    r3, r2
                    ands    r3, r3, r0

                    bne skip_store
                    ldr r2, =pageAddress
                    str r5, [r2]                    //Write to pageAddress variable at each page boundary - requires output buffer to be page aligned!!

skip_store:     ldr r1, =pageBuffer
                    adds    r2, r1, r3
                    strb    r4, [r2]

                    cmp r3, r0                      // is this the last byte on the page?
                    bne skip_program

                    // At this point R5 points to a page in flash memory to
                    // program, R1 holds the location of the page buffer with the
                    // data to be written, and R6 holds the number of 32-bit words
                    // to write
                    bl  program_page

skip_program:   pop { r0 - r7, pc }


/////////////////////////////////////////////////////////////////////////////
// read_output: read from the output buffer in flash, or the page buffer if
// the address belongs to the page that is currently being assembled. We
// assume that all read operations prior to the current page will be from
// flash ROM; we further assume all read operations beyond the page address
// are still within the page buffer boundary (no sanity checks will be made
// here - if they failed they were indicative of a major flaw in the LZMA
// decompressor, anyways)
//
// On entry:
//  r3 contains the source address in the output buffer
//
// On exit
//      r4 contains a byte read from above specified address

    .section .text.read_output, "ax", %progbits
    .thumb_func
    .type   read_output, %function
    .global read_output

read_output:    push    { r0 - r3, r5 - r7, lr }

                    ldr r0, =pageAddress
                    ldr r5, [r0]            //Get the page address (@0x200099F8 - uninit: 0x00000000)
                    subs    r2, r3, r5      //get offset between the pageAddress and the output buffer (@0x20008815 - lower_ram_noretain)
                    bmi read_rom            //offset < 0, so read start of output buffer

                    ldr r3, =pageBuffer
                    adds    r3, r3, r2      //Apply the offset

read_rom:       ldrb    r4, [r3]            // Return the output buffer value at the offset
                    pop { r0 - r3, r5 - r7, pc }
