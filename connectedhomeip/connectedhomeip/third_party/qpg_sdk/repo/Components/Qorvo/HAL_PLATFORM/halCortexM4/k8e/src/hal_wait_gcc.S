/*
 * Copyright (c) 2018, Qorvo Inc
 *
 *   Wait loop
 *
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
 * $Header: $
 * $Change: $
 * $DateTime: $
 *
 */

    .syntax unified
    .arch armv7
    .thumb

/*
 *  void hal_wait_loop(UInt32 loops);
 */

    .section .text.hal_wait_loop, "ax", %progbits
    .type    hal_wait_loop, %function
    .global  hal_wait_loop

    .align 3    /* Force 8-byte alignment. Alignment of this function is essential. It affects the wait duration. */

hal_wait_loop:
    nop
    subs    r0, #1
    bcs.n   hal_wait_loop
    bx      lr

