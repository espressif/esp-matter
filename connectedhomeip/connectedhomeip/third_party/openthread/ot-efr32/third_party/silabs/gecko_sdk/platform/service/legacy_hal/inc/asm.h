/***************************************************************************//**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef ASM_H
#define ASM_H

#if defined(__IAR_SYSTEMS_ASM__)
#define __END__                     END
#define __EQU__(a, b)               a EQU b
#define __EXPORT__                  EXPORT
#define __IMPORT__                  IMPORT
#define __SPACE__                   DS8
#define __THUMB__                   THUMB
#define __WEAK__                    PUBWEAK

          RSEG .stack:DATA
#define __CODE__                    SECTION .text:CODE:REORDER:NOROOT(2)
#define __BSS__                     SECTION .bss:DATA:NOROOT(2)
#define __BEGIN_STACK__(offset)     SFB(.stack + offset)
#define __END_STACK__(offset)       SFE(.stack + offset)
#define __CFI__(x)
#endif

#if defined(__GNUC__) || defined(__clang__)
.syntax unified
.thumb
#define __BSS__                     .bss
#define __CODE__                    .text
#define __END__                     .end
#define __EQU__(a, b)               .equ a, b
#define __EXPORT__                  .global
#define __IMPORT__                  .extern
#define __SPACE__                   .space
#define __THUMB__                   .thumb_func
#define __WEAK__                    .weak
#define __BEGIN_STACK__(offset)     (__StackLimit + offset)
#define __END_STACK__(offset)       (__StackTop + offset)
#endif

#endif // ASM_H
