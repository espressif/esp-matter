/*
 * Copyright (c) 2017, Texas Instruments Incorporated
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
/*
 *  ======== boot.asm ========
 */

    .syntax unified

    .global	_c_int00
    .global	_reset_fxns_done_

    .sect .c_int00, "ax"
    .func _c_int00
    .thumb_func

_c_int00: 
    /* initialize stack pointer */
    ldr r0, =__TI_STACK_BASE
    mov sp, r0
    ldr r0, =__TI_STACK_SIZE
    add sp, r0

    /* CPACR is located at address 0xE000ED88 */
    ldr.w r0, =0xE000ED88
    /* Read CPACR */
    ldr r1, [r0]
    /* Set bits 20-23 to enable CP10 and CP11 coprocessors */
    orr r1, r1, #(0xF << 20)
    /* Write back the modified value to the CPACR */
    str r1, [r0]

    /* align to 64-bits for EABI */
    mov r7, sp
    mov r0, #0x07
    bic r7, r0
    mov sp, r7

    /* run any reset functions */
    ldr r0, =xdc_runtime_Startup_reset__I
    cmp r0, #0
    beq _reset_fxns_done_
    blx r0

_reset_fxns_done_:

    /* do more initialization in C, go to main() */
    ldr r0, =startupC
    blx r0

    .endfunc
    .end
