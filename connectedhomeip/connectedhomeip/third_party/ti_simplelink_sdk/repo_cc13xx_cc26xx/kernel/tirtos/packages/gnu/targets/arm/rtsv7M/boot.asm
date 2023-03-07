@ --COPYRIGHT--,EPL
@   Copyright (c) 2011-2020 Texas Instruments and others.
@   All rights reserved. This program and the accompanying materials
@   are made available under the terms of the Eclipse Public License v1.0
@   which accompanies this distribution, and is available at
@   http://www.eclipse.org/legal/epl-v10.html
@
@   Contributors:
@       Texas Instruments - initial implementation
@
@ --/COPYRIGHT--
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@  boot.asm
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#define M3  1
#define M4  2
#define M4F 3

    .syntax unified

    .global	_c_int00
    .global	_reset_fxns_done_

    .sect .bootVecs, "ax"
    .long 0         @ real stack pointer will be initialized in
    .long _c_int00  @ startup routine

    .sect .c_int00, "ax"
    .func _c_int00
    .thumb_func

_c_int00: 
    @ initialize stack pointer
    ldr r0, =__TI_STACK_BASE
    mov sp, r0
    ldr r0, =__TI_STACK_SIZE
    add sp, r0

#if (xdc_target_name__ == M4F)
    @ CPACR is located at address 0xE000ED88
    ldr.w r0, =0xE000ED88
    @ Read CPACR
    ldr r1, [r0]
    @ Set bits 20-23 to enable CP10 and CP11 coprocessors
    orr r1, r1, #(0xF << 20)
    @ Write back the modified value to the CPACR
    str r1, [r0]
#endif

    @ align to 64-bits for EABI
    mov r7, sp
#if (xdc_target_name__ == M0)
    movs r0, #0x07
    bics r7, r0
#else
    mov r0, #0x07
    bic r7, r0
#endif
    mov sp, r7

    @ run any reset functions
    ldr r0, =xdc_runtime_Startup_reset__I
    cmp r0, #0
    beq _reset_fxns_done_
    blx r0

_reset_fxns_done_:

    @ do more initialization in C, go to main()
    ldr r0, =startupC
    blx r0

    .endfunc
    .end

