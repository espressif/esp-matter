@
@  Copyright (c) 2013-2019, Texas Instruments Incorporated
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
@ ======== Hwi_asm_gnu.sv7M ========
@
@

#define TRUE 1
#define FALSE 0

        .syntax unified
        .thumb

        .global ti_sysbios_family_arm_m3_Hwi_doSwiRestore
        .global ti_sysbios_family_arm_m3_Hwi_doTaskRestore
        .global ti_sysbios_family_arm_m3_Hwi_dispatchC
        .global ti_sysbios_family_arm_m3_Hwi_dispatch
        .global ti_sysbios_family_arm_m3_Hwi_pendSV
        .global ti_sysbios_family_arm_m3_Hwi_excHandlerAsm
        .global ti_sysbios_family_arm_m3_Hwi_excHandler
        .global ti_sysbios_family_arm_m3_Hwi_return
        .global ti_sysbios_family_arm_m3_Hwi_interruptsAreDisabledButShouldNotBe

        .global ti_sysbios_family_arm_m3_Hwi_swiTaskKeyAddress
        .global ti_sysbios_family_arm_ducati_Core_unlock__E


@
@ Here on all dispatched interrupts
@
@ Warning! Do not overwrite r3 prior to jumping to dispatcher.
@ When in sparse dispatch table mode, r3 contains the address of
@ the Hwi object.
@

        .section .text.ti_sysbios_family_arm_m3_Hwi_dispatch
        .thumb_func
ti_sysbios_family_arm_m3_Hwi_dispatch:

@
@ get IRP
@ If this hwi switched to MSP then IRP is on PSP stack
@ else if this is a nested interrupt then IRP is on current MSP stack
@
        tst     lr, #4          @ context on PSP?
        ite     NE
        mrsne   r1, psp         @ if yes, then use PSP
        moveq   r1, sp          @ else use MSP
        ldr     r0, [r1, #24]   @ get IRP (2nd of 8 items to be pushed)

#if (defined(__VFP_FP__) && !defined(__SOFTFP__))
        sub     r2, r1, #72     @ make room for 9 FP regs
        ite     NE              @ CCs are still valid from tst lr above
        msrne   psp, r2         @ update appropriate SP
        moveq   sp, r2
        vstmdb  r1!, {d0-d7}    @ push vfp scratch regs on appropriate stack
        fmrx    r2, fpscr       @ push fpscr too
        str     r2, [r1, #-8]!  @ (keep even align)
#endif

        mrs     r1, basepri
        push    {r1}            @ save old basepri
        push    {lr}            @ save EXC_RETURN

        ldr     lr, dispatchAddr
        blx     lr              @ pass IRP to ti_sysbios_family_arm_m3_Hwi_dispatchC()
                                @ dispatcher returns swi and task keys in r0

@ Here on return from dispatcher, r0 = tskKey, interrupts disabled
        ldr     r2, swiTaskKeyAddr
        ldr     r2, [r2]
        ldr     r1, [r2]
        and     r0, r1, r0
        str     r0, [r2]

        pop     {lr}            @ restore EXC_RETURN
        tst     lr, #8          @ returning to thread mode?
        bne     lab$2           @ branch if lowest order interrupt

@
@ return now, no further processing on nested interrupts
@
@ This is normal when:
@  1) this is a nested interrupt,
@  2) the Task scheduler has been disabled by the interrupted task thread
@  3) this is a Swi-only system
@
        pop     {r0}            @ restore old basepri
        msr     basepri, r0     @
#if (defined(__VFP_FP__) && !defined(__SOFTFP__))
        ldr     r0, [r13], #8   @ pop fpscr (keep even align)
        fmxr    fpscr, r0
        vldmia  r13!, {d0-d7}   @ pop vfp scratch regs
#endif
        bx      lr              @ return from interrupt

@
@ Here when no other interrupt is pending
@ perform a dummy EXC_RETURN to return to thread mode before
@ running the Swi and Task schedulers
@
lab$2:
        pop     {r2}            @ restore old basepri
        tst     lr, #4          @ context on PSP?
        ite     NE
        mrsne   r1, psp         @ if yes, then use PSP
        moveq   r1, sp          @ else use MSP
        sub     r1, #(8*4)      @ make room for dummy
        ite     NE              @ Z bit still valid from above tst
        msrne   psp, r1         @ update appropriate SP
        moveq   sp, r1

        str     r2, [r1, #4]    @ old basepri pops into r1

        mov     r0, #0x01000000 @ dummy xPSR with T bit set
        str     r0, [r1, #28]
        ldr     r0, lab$1Addr
        str     r0, [r1, #24]   @ last str with imm offset
                                @ 5 more inst before bx lr
                                @ (See ARM errata #838869)

@ copy saved swi and task keys into interrupt context

        ldr     r3, swiTaskKeyAddr
        ldr     r3, [r3]
        ldr     r0, [r3]
        str     r0, [r1]        @ tskkey pops into r0
        movw    r0, #0x00000101
        str     r0, [r3]        @ re-init swiTaskKey

        bx      lr              @ EXC_RETURN to lab$1 below

@
@ Now we're back in thread mode an could be running on the stack
@ or the ISR stack.
@
@ If an ISR hit while a Swi posted from an ISR was running, we'll be on the ISR stack.
@ Otherwise we'll be on the pre-empted Task stack.
@
@       r0 = swiTaskkey
@       r1 = old basepri
@
@ Call Swi and Task schedulers
@
lab$1:
        push    {r1}            @ push old basepri
        push    {r0}            @ push copy of swiTaskKey

        mrs     r1, control     @ on ISR stack already?
        cmp     r1, #0
        itt     NE
        movne   r2, #0          @ no,
        msrne   control, r2     @ switch to ISR stack

        push    {r1}            @ push old control reg onto ISR stack (MSP)
        push    {r1}            @ dummy to force 8 byte stack align

        ldr     lr, doSwiRestoreAddr
        blx     lr              @ pass swiTaskKey (r0) to ti_sysbios_family_arm_m3_Hwi_doSwiRestore()

        pop     {r0}            @ pop dummy
        pop     {r0}            @ pop old control register

        cmp     r0, #0          @ lowest order ISR?
        itt     NE
        movne   r1, #2          @ if yes
        msrne   control, r1     @ switch back ti Task stack (PSP)


        pop     {r0}            @ pop copy of swiTaskKey
        ldr     lr, doTaskRestoreAddr
        blx     lr              @ pass returned tskkey to ti_sysbios_family_arm_m3_Hwi_doTaskRestore()
                                @ current task may be pre-empted here
@
@ Here on return from doTaskRestoreHwi() with ints disabled.
@
@ Extra care must be taken now to ensure that we completely unwind
@ to the originally interrupted background thread without allowing
@ any more dispatched interrupts. Otherwise the BIOS promise of no
@ more than 2 (dispatched) interrupt contexts on a Task stack can be
@ broken.
@
@ In order to achieve this feat, we first post the pendSV interrupt
@ which is configured with the same priority level as ti_sysbios_family_arm_m3_Hwi_disable().
@ Then we re-enable interrupts. This will cause an immediate pendSV
@ exception to be taken which is still at ti_sysbios_family_arm_m3_Hwi_disable() priority.
@ We then blow off the pendSV stack contents and return to the
@ background thread at the normal priority level.
@
        ldr     r0, nvic_icsr   @ point to ti_sysbios_family_arm_m3_Hwi_nvic.ICSR
        mov     r1, #0x10000000
        str     r1, [r0]        @ post a pendSV interrupt

        pop     {r0}            @ retrieve old basepri

@ CAUTION!!! Do NOT single step thru the next instruction
@ else, the processor won't arrive at pendSV thru the
@ exception mechanism  and catastrophic things will happen
@ afterwards.

        msr     basepri, r0     @ causes pendSV to happen
        nop                     @ 2 nops required for prefetch
        nop                     @

@ Arriving here can only happen if interrupts were globally disabled
@ when the pendSV interrupt was posted above. This can happen if
@ user code has set the PRIMASK and not cleared it, or when single
@ stepping with interrupts disabled.

ti_sysbios_family_arm_m3_Hwi_interruptsAreDisabledButShouldNotBe:
        b   ti_sysbios_family_arm_m3_Hwi_interruptsAreDisabledButShouldNotBe

        .thumb_func
ti_sysbios_family_arm_m3_Hwi_pendSV:
        tst     lr, #4          @ context on PSP?
        ite     NE
        mrsne   r1, psp         @ if yes, then use PSP
        moveq   r1, sp          @ else use MSP
        add     r1, #(8*4)      @ blow off pendSV stack
#if (defined(__VFP_FP__) && !defined(__SOFTFP__))
        ldr     r0, [r1], #8    @ pop fpscr (keep even align)
        fmxr    fpscr, r0
        vldmia  r1!, {d0-d7}    @ pop vfp scratch regs
#endif
        ite     NE              @ Z bit still valid from above tst
        msrne   psp, r1         @ update appropriate SP
        moveq   sp, r1

ti_sysbios_family_arm_m3_Hwi_return:
        bx      lr              @ return into task, re-enables ints

        .align 2
nvic_icsr:
        .word       0xe000ed04
dispatchAddr:
        .word       ti_sysbios_family_arm_m3_Hwi_dispatchC
doSwiRestoreAddr:
        .word       ti_sysbios_family_arm_m3_Hwi_doSwiRestore
doTaskRestoreAddr:
        .word       ti_sysbios_family_arm_m3_Hwi_doTaskRestore
swiTaskKeyAddr:
        .word       ti_sysbios_family_arm_m3_Hwi_swiTaskKeyAddress
lab$1Addr:
        .word       lab$1

        .section .text.ti_sysbios_family_arm_m3_Hwi_excHandlerAsm
        .thumb_func
ti_sysbios_family_arm_m3_Hwi_excHandlerAsm:
        tst     lr, #4          @ context on PSP?
        ite     NE
        mrsne   r0, psp         @ if yes, then use PSP
        moveq   r0, sp          @ else use MSP
        mov     sp, r0          @ use this stack
        stmfd   sp!, {r4-r11}   @ save r4-r11 while we're at it
        mov     r0, sp          @ pass sp to exception handler
        mov     r1, lr          @ pass lr too
        mov     r4, lr          @ preserve LR in r4

        ldr     r2, excHandlerAddr
        blx     r2

        mov     r0, sp          @ for ROV
        mov     r1, r4          @ for ROV
lab$3:
        b       lab$3           @ spin here indefinitely

        .align 2
excHandlerAddr:
        .word          ti_sysbios_family_arm_m3_Hwi_excHandler

        .end
