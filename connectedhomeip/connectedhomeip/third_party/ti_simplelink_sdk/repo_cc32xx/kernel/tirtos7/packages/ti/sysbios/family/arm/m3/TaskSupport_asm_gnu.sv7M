@
@  Copyright (c) 2013-2020, Texas Instruments Incorporated
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
@ ======== TaskSupport_asm_gnu.sv7M ========
@

        .syntax unified
        .thumb

        .global ti_sysbios_knl_TaskSupport_swap
        .global ti_sysbios_knl_TaskSupport_glue
        .global ti_sysbios_knl_TaskSupport_glueUnpriv
        .global ti_sysbios_knl_TaskSupport_swapReturn

        .align 2

@
@  ======== ti_sysbios_knl_TaskSupport_swap ========
@
@

        .section .text.ti_sysbios_knl_TaskSupport_swap
        .thumb_func
ti_sysbios_knl_TaskSupport_swap:

#if defined(ti_sysbios_knl_TaskSupport_usesMonitors__D)
        clrex
#endif
        push    {r4-r11, lr}
#if (defined(__VFP_FP__) && !defined(__SOFTFP__))
        vpush   {d8-d15}
#endif
#if defined(__ARM_ARCH_8M_MAIN__)
        mrs     r2, psplim
        push    {r2, r3}
        mov     r2, #0
        msr     psplim, r2
#endif
        str     sp, [r0]

ti_sysbios_knl_TaskSupport_swapReturn:
        ldr     sp, [r1]
#if defined(__ARM_ARCH_8M_MAIN__)
        pop     {r2, r3}
        msr     psplim, r3
#endif
#if (defined(__VFP_FP__) && !defined(__SOFTFP__))
        vpop    {d8-d15}        @ pop vfp caller regs
#endif
        pop     {r4-r11, pc}
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop

@
@  ======== ti_sysbios_knl_TaskSupport_glue ========
@
@  This glue function is executed immediately before executing the "fxn"
@  passed to Task_start.  This function loads the parameters to "fxn" into
@  registers - necessary for the m3 register passing model.
@

        .section .text.ti_sysbios_knl_TaskSupport_glue
        .thumb_func
ti_sysbios_knl_TaskSupport_glue:

        nop
        nop
        nop
        nop
        ldr     r0, [sp, #16]   @ Task enter function
        blx     r0              @ call Task enter function

@ return from Task enter function here
        ldr     r0, [sp, #0]    @ task arg0
        ldr     r1, [sp, #4]    @ task arg1
        ldr     r2, [sp, #8]    @ task fxn

        ldr     lr, [sp, #12]   @ Task exit function
        bx      r2              @ call task fxn,  ("returns" to exit func)

@
@  ======== ti_sysbios_knl_TaskSupport_glueUnpriv ========
@
@  This glue function is executed immediately before executing the "fxn"
@  passed to Task_start.  This function loads the parameters to "fxn" into
@  registers - necessary for the m3 register passing model. This function
@  alse switches execution state to unprivileged before jumping to "fxn" and
@  switches execution state back to privileged when "fxn" returns.
@

        .section .text.ti_sysbios_knl_TaskSupport_glueUnpriv
        .thumb_func
ti_sysbios_knl_TaskSupport_glueUnpriv:

        ldr     r0, [sp, #16]   @ Task enter function
        blx     r0              @ call Task enter function

@ return from Task enter function here
        svc     #1              @ Enter unprivileged mode

        ldr     r0, [sp, #0]    @ task arg0
        ldr     r1, [sp, #4]    @ task arg1
        ldr     r2, [sp, #8]    @ task fxn
        mov     r3, #0          @ clear register to prevent data leak
        mov     r4, #0          @ clear register to prevent data leak
        mov     r5, #0          @ clear register to prevent data leak
        mov     r6, #0          @ clear register to prevent data leak
        mov     r7, #0          @ clear register to prevent data leak
        mov     r8, #0          @ clear register to prevent data leak
        mov     r9, #0          @ clear register to prevent data leak
        mov     r10, #0         @ clear register to prevent data leak
        mov     r11, #0         @ clear register to prevent data leak
        mov     r12, #0         @ clear register to prevent data leak
        blx     r2              @ call task fxn

@ Below code reads Task_exit() address from code memory. This is
@ unlike the Task_glue() function which reads the Task_exit() address
@ from the stack. This is to prevent an attacker from directly
@ jumping to the svc instruction below and inserting a custom
@ function address on the stack so the attacker's code is
@ executed in privileged mode
        svc     #0              @ Enter priv mode
        ldr     r0, ti_sysbios_knl_Task_exit
        bx      r0
ti_sysbios_knl_Task_exit:
        .word          ti_sysbios_knl_Task_exit__E


        .end
