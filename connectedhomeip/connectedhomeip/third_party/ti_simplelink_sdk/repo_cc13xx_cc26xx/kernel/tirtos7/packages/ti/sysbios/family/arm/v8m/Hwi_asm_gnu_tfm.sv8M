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
@ ======== v8m_Hwi_asm_gnu_tfm.sv8M ========
@
@

#define TRUE 1
#define FALSE 0

        .syntax unified
        .thumb

        .global ti_sysbios_family_arm_v8m_Hwi_doSwiRestore
        .global ti_sysbios_family_arm_v8m_Hwi_doTaskRestore
        .global ti_sysbios_family_arm_v8m_Hwi_dispatchC
        .global ti_sysbios_family_arm_v8m_Hwi_dispatch
        .global ti_sysbios_family_arm_v8m_Hwi_pendSV
        .global ti_sysbios_family_arm_v8m_Hwi_excHandlerAsm
        .global ti_sysbios_family_arm_v8m_Hwi_excHandler
        .global ti_sysbios_family_arm_v8m_Hwi_svcHandlerAsm
        .global ti_sysbios_family_arm_v8m_Hwi_svcHandler
        .global ti_sysbios_family_arm_v8m_Hwi_return
        .global ti_sysbios_family_arm_v8m_Hwi_interruptsAreDisabledButShouldNotBe

        .global ti_sysbios_family_arm_v8m_Hwi_swiTaskKeyAddress

@
@ Here on all dispatched interrupts
@
@ Warning! Do not overwrite r3 prior to jumping to dispatcher.
@ When in sparse dispatch table mode, r3 contains the address of
@ the Hwi object.
@

        .section .text.ti_sysbios_family_arm_v8m_Hwi_dispatch
        .thumb_func
ti_sysbios_family_arm_v8m_Hwi_dispatch:

        tst     lr, #0x40       @ did we pre-empt Secure mode?
        itt     NE
        movne   r0, lr          @ let ROV's IRP value signal this case
        bne     lab$0
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
lab$0:
        mrs     r1, basepri
        push    {r1}            @ save old basepri
        push    {lr}            @ save EXC_RETURN

        ldr     lr, dispatchAddr
        blx     lr              @ pass IRP to ti_sysbios_family_arm_v8m_Hwi_dispatchC()
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

        tst     lr, #0x10       @ FP saved?
        ite     NE
        subne   r1, #(8*4)      @ make room for dummy
        subeq   r1, #(26*4)

        tst     lr, #4          @ context on PSP?
        ite     NE              @ Z bit still valid from above tst
        msrne   psp, r1         @ update appropriate SP
        moveq   sp, r1

        str     lr, [r1, #8]    @ root LR pops into r2
        bic     lr, #0x40       @ clear secure mode bit

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
@       r2 = root LR
@
@ Call Swi and Task schedulers
@
lab$1:
        push    {r2}            @ push root LR
        push    {r1}            @ push old basepri
        push    {r0}            @ push copy of swiTaskKey

        mrs     r1, control     @ on ISR stack already?
        tst     r1, #2
        itt     NE
        bicne   r2, r1, #2      @ no,
        msrne   control, r2     @ switch to ISR stack

        push    {r1}            @ push old control reg onto ISR stack (MSP)

        ldr     lr, doSwiRestoreAddr
        blx     lr              @ pass swiTaskKey (r0) to ti_sysbios_family_arm_v8m_Hwi_doSwiRestore()

        pop     {r0}            @ pop old control register

        tst     r0, #2          @ lowest order ISR?
        it      NE
        msrne   control, r0     @ switch back ti Task stack (PSP)


        pop     {r0}            @ pop copy of swiTaskKey
        ldr     lr, doTaskRestoreAddr
        blx     lr              @ pass returned tskkey to ti_sysbios_family_arm_v8m_Hwi_doTaskRestore()
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
@ which is configured with the same priority level as ti_sysbios_family_arm_v8m_Hwi_disable().
@ Then we re-enable interrupts. This will cause an immediate pendSV
@ exception to be taken which is still at ti_sysbios_family_arm_v8m_Hwi_disable() priority.
@ We then blow off the pendSV stack contents and return to the
@ background thread at the normal priority level.
@
        ldr     r0, nvic_icsr   @ point to ti_sysbios_family_arm_v8m_Hwi_nvic.ICSR
        mov     r1, #0x10000000
        str     r1, [r0]        @ post a pendSV interrupt

        pop     {r0}            @ retrieve old basepri
        pop     {r2}            @ pop root LR

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

ti_sysbios_family_arm_v8m_Hwi_interruptsAreDisabledButShouldNotBe:
        b   ti_sysbios_family_arm_v8m_Hwi_interruptsAreDisabledButShouldNotBe

        .thumb_func
ti_sysbios_family_arm_v8m_Hwi_pendSV:
        tst     lr, #4          @ context on PSP?
        ite     NE
        mrsne   r1, psp         @ if yes, then use PSP
        moveq   r1, sp          @ else use MSP
        ldr     r2, [r1, #8]    @ fetch root LR

        tst     lr, #0x10       @ FP saved?
        ite     NE
        addne   r1, #(8*4)      @ blow off pendSV stack
        addeq   r1, #(26*4)     @ blow off pendSV stack

        tst     lr, #4          @ context on PSP?
        ite     NE              @ Z bit still valid from above tst
        msrne   psp, r1         @ update appropriate SP
        moveq   sp, r1

ti_sysbios_family_arm_v8m_Hwi_return:

        mov     lr, r2          @restore root LR
        bx      lr              @ return into task, re-enables ints

        .align 2
nvic_icsr:
        .word       0xe000ed04
dispatchAddr:
        .word       ti_sysbios_family_arm_v8m_Hwi_dispatchC
doSwiRestoreAddr:
        .word       ti_sysbios_family_arm_v8m_Hwi_doSwiRestore
doTaskRestoreAddr:
        .word       ti_sysbios_family_arm_v8m_Hwi_doTaskRestore
swiTaskKeyAddr:
        .word       ti_sysbios_family_arm_v8m_Hwi_swiTaskKeyAddress
lab$1Addr:
        .word       lab$1

        .section .text.ti_sysbios_family_arm_v8m_Hwi_excHandlerAsm
        .thumb_func
ti_sysbios_family_arm_v8m_Hwi_excHandlerAsm:
        tst     lr, #4          @ context on PSP?
        ite     NE
        mrsne   r0, psp         @ if yes, then use PSP
        moveq   r0, sp          @ else use MSP

#if defined(__ARM_ARCH_8M_MAIN__)
        mov     r1, #0          @ exception may occur on psp stack and it
        msr     msplim, r1      @ may be at a lower address than msp stack.
                                @ therefore, set msplim to 0 to disable
                                @ stack limit checking
#endif
        mov     sp, r0          @ use this stack
        tst     lr, #0x20       @ DCRS=1 = r4-r11 stacked too?
        bne     lab$4
        pop     {r2-r11}        @ restore r4-r11, ignore magic/reserved
                                @ popped into r2,r3
lab$4:
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
        .word          ti_sysbios_family_arm_v8m_Hwi_excHandler
	
        .section .text.ti_sysbios_family_arm_v8m_Hwi_svcHandlerAsm
        .thumb_func
ti_sysbios_family_arm_v8m_Hwi_svcHandlerAsm:
        mrs     r2,msp
        tst     lr, #4
        ite     eq
        moveq   r0, r2
        mrsne   r0, psp
        mov     r1, lr
        ldr     r2, svcHandlerAddr
        blx     r2
        bx      r0

        .align 2
svcHandlerAddr:
        .word       ti_sysbios_family_arm_v8m_Hwi_svcHandler

        .end
