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
@ ======== Hwi_asm_switch_gnu.sv8M ========
@

        .syntax unified

        .thumb

        .global ti_sysbios_family_arm_v8m_Hwi_switchAndRunFunc
        .global ti_sysbios_family_arm_v8m_Hwi_initStacks
        .global ti_sysbios_family_arm_v8m_Hwi_getTaskSP
        .global ti_sysbios_family_arm_v8m_Hwi_setStackLimit

@
@  ======== ti_sysbios_family_arm_v8m_Hwi_switchAndRunFunc ========
@  ti_sysbios_family_arm_v8m_Hwi_switchAndRunFunc(Void (*func)());
@
@  Switch to ISR stack, call the function Func() and then switch
@  back to Task stack.
@

        .section .text.ti_sysbios_family_arm_v8m_Hwi_switchAndRunFunc
        .thumb_func

ti_sysbios_family_arm_v8m_Hwi_switchAndRunFunc:
        push    {r4, lr}
        mrs     r4, control     @ on ISR stack already?
        tst     r4, #2
        itt     NE
        bicne   r1, r4, #2      @ no,
        msrne   control, r1     @ force thread mode to use MSP

        blx     r0              @ Call func

        tst     r4, #2          @ lowest order ISR?
        it      NE
        msrne   control, r4     @ force thread mode to use PSP

        pop     {r4, pc}        @ return

        .section .text.ti_sysbios_family_arm_v8m_Hwi_getTaskSP
        .thumb_func

ti_sysbios_family_arm_v8m_Hwi_getTaskSP:
        mrs     r0, psp         @ psp = interrupt task stack
        bx      lr

@
@  ======== ti_sysbios_family_arm_v8m_Hwi_initStacks ========
@  set up dual stacks
@  only called if tasking is enabled
@  otherwise, msp = the only stack.
@  msp = handlers (isr Stack)
@  psp = threads (task Stacks)
@

        .section .text.ti_sysbios_family_arm_v8m_Hwi_initStacks
        .thumb_func

ti_sysbios_family_arm_v8m_Hwi_initStacks:
        push    {lr}
        mrs     r2, xpsr
        tst     r2, #0xff       @ check if we're in handler mode
        beq     lab$1
@
@ Here if CPU was not reset before starting the program
@ and the last
@ program halted while in handler mode.
@ must unwind from handler mode before
@ configuring control register
@
        mov     r2, sp
        sub     r2, #(8*4)      @ make room for dummy stack frame
        msr     psp, r2         @ update PSP
        mvn     lr, #2
        str     r0, [r2, #0]    @ save passed R0 on ISR stack
        mov     r1, #0x01000000 @ dummy xPSR with T bit set
        str     r1, [r2, #28]
        ldr     r1, lab$1Addr
        str     r1, [r2, #24]   @ return PC to $1 below
        bx      lr              @ "return" from ISR

lab$1:
        pop     {lr}
        mov     r1, sp
        msr     psp, r1         @ psp = task stacks
        msr     msp, r0         @ msp = isrStack
        mov     r0, #2
        mrs     r1, control
        msr     control, r0     @ split stacks now.

#if (defined(__VFP_FP__) && !defined(__SOFTFP__))
        ldr     r0, fpccr       @ clear APSEN and LSPEN
        ldr     r1, [r0]        @ get current fpccr
        bic     r1, r1, #0xc0000000 @ clear upper 2 bits
        orr     r1, r1, #0x80000000 @ set ASPEN, always stack FP regs
        str     r1, [r0]        @ save/restore manually
#endif

        bx      lr              @ return on psp

        .align 2
lab$1Addr:
        .word   lab$1

#if (defined(__VFP_FP__) && !defined(__SOFTFP__))
        .align 2
fpccr:
        .word   0xe000ef34
#endif

@
@  ======== ti_sysbios_family_arm_v8m_Hwi_setStackLimit ========
@

        .section .text.ti_sysbios_family_arm_v8m_Hwi_setStackLimit
        .thumb_func

ti_sysbios_family_arm_v8m_Hwi_setStackLimit:
#if defined(__ARM_ARCH_8M_MAIN__)
        msr    msplim, r0
#endif
        bx     lr

        .end
