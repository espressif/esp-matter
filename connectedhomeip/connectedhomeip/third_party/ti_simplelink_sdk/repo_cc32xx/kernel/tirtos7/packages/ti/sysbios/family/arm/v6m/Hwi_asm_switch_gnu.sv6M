@
@  Copyright (c) 2020, Texas Instruments Incorporated
@  All rights reserved.
@
@  Redistribution and use in source and binary forms, with or without
@  modification, are permitted provided that the following conditions
@  are met:
@
@  *  Redistributions of source code must retain the above copyright
@     notice, this list of conditions and the following disclaimer.
@
@  *  Redistributions in binary form must reproduce the above copyright
@     notice, this list of conditions and the following disclaimer in the
@     documentation and/or other materials provided with the distribution.
@
@  *  Neither the name of Texas Instruments Incorporated nor the names of
@     its contributors may be used to endorse or promote products derived
@     from this software without specific prior written permission.
@
@  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
@  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
@  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
@  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
@  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
@  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
@  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES@ LOSS OF USE, DATA, OR PROFITS@
@  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
@  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
@  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
@  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
@
@
@ ======== Hwi_asm_switch_gnu.sv6M ========
@

        .syntax unified

        .thumb

        .global ti_sysbios_family_arm_v6m_Hwi_switchAndRunFunc
        .global ti_sysbios_family_arm_v6m_Hwi_initStacks
        .global ti_sysbios_family_arm_v6m_Hwi_getTaskSP

@
@  ======== ti_sysbios_family_arm_v6m_Hwi_switchAndRunFunc ========
@  ti_sysbios_family_arm_v6m_Hwi_switchAndRunFunc(Void (*func)());
@
@  Switch to ISR stack, call the function Func() and then switch
@  back to Task stack.
@

        .section .text.ti_sysbios_family_arm_v6m_Hwi_switchAndRunFunc
        .thumb_func

ti_sysbios_family_arm_v6m_Hwi_switchAndRunFunc:
        push    {r4, lr}
        mrs     r4, control     @ on ISR stack already?
        cmp     r4, #0
        beq     lab$1
        movs    r1, #0          @ no,
        msr     control, r1     @ force thread mode to use MSP
        blx     r0              @ Call func
        movs    r1, #2          @ yes
        msr     control, r1     @ force thread mode to use PSP
        pop     {r4, pc}        @ restore r4, lr into pc.

lab$1:
        blx     r0
        pop     {r4, pc}        @ restore r4, lr into pc.

        .section .text.ti_sysbios_family_arm_v6m_Hwi_getTaskSP
        .thumb_func

ti_sysbios_family_arm_v6m_Hwi_getTaskSP:
        mrs     r0, psp         @ psp = interrupt task stack
        bx      lr

@
@  ======== ti_sysbios_family_arm_v6m_Hwi_initStacks ========
@  set up dual stacks
@  only called if tasking is enabled
@  otherwise, msp = the only stack.
@  msp = handlers (isr Stack)
@  psp = threads (task Stacks)
@

        .section .text.ti_sysbios_family_arm_v6m_Hwi_initStacks
        .thumb_func

ti_sysbios_family_arm_v6m_Hwi_initStacks:
        push    {lr}
        mrs     r2, xpsr
@       tst     r2, #0xff       @ check if we're in handler mode
        movs    r1, #0xff
        tst     r2, r1
        beq     lab$2
@
@ Here if CPU was not reset before starting the program
@ and the last
@ program halted while in handler mode.
@ must unwind from handler mode before
@ configuring control register
@
        mov     r2, sp
        subs    r2, #(8*4)      @ make room for dummy stack frame
        msr     psp, r2         @ update PSP
@       mvn     lr, #2
        movs    r1, #0xfd
        mov     lr, r1
        str     r0, [r2, #0]    @ save passed R0 on ISR stack
@       mov     r1, #0x01000000 @ dummy xPSR with T bit set
        movs    r1, #0x01
        lsls    r1, r1, #24
        str     r1, [r2, #28]
        ldr     r1, lab$2Addr
        str     r1, [r2, #24]   @ return PC to $1 below
        bx      lr              @ "return" from ISR

lab$2:
        pop     {r2}
        mov     r1, sp
        msr     psp, r1         @ psp = task stacks
        msr     msp, r0         @ msp = isrStack
        movs    r0, #2
        mrs     r1, control
        msr     control, r0     @ split stacks now.
        bx      r2              @ return on psp

        .align 2
lab$2Addr:
        .word   lab$2

        .end
