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
@ ======== Hwi_asm_gnu.sv6M ========
@
@

#define TRUE 1
#define FALSE 0

        .syntax unified
        .thumb

        .global ti_sysbios_family_arm_v6m_Hwi_doSwiRestore
        .global ti_sysbios_family_arm_v6m_Hwi_doTaskRestore
        .global ti_sysbios_family_arm_v6m_Hwi_dispatchC
        .global ti_sysbios_family_arm_v6m_Hwi_dispatch
        .global ti_sysbios_family_arm_v6m_Hwi_pendSV
        .global ti_sysbios_family_arm_v6m_Hwi_excHandlerAsm
        .global ti_sysbios_family_arm_v6m_Hwi_excHandler
        .global ti_sysbios_family_arm_v6m_Hwi_return

        .global NOROM_CPUcpsid
        .global NOROM_CPUcpsie
        .global NOROM_SetupTrimDevice

@
@ Here on all dispatched interrupts
@
@ Warning! Do not overwrite r3 prior to jumping to dispatcher.
@ When in sparse dispatch table mode, r3 contains the address of
@ the Hwi object.
@

        .section .text.ti_sysbios_family_arm_v6m_Hwi_dispatch
        .thumb_func
ti_sysbios_family_arm_v6m_Hwi_dispatch:

@
@ get IRP
@ If this hwi switched to MSP then IRP is on PSP stack
@ else if this is a nested interrupt then IRP is on current MSP stack
@
        mov     r1, lr
        movs    r2, #4
        tst     r1, r2          @ context on PSP?
        mov     r2, sp          @ assume no, use MSP
        beq     lab$11
        mrs     r2, psp         @ if yes, then use PSP

lab$11:
        ldr     r0, [r2, #24]   @ get IRP (2nd of 8 items to be pushed)

        push    {r1}            @ save EXC_RETURN

        ldr     r1, dispatchAddr
        blx     r1              @ pass IRP to ti_sysbios_family_arm_v6m_Hwi_dispatchC()
                                @ dispatcher returns swi and task keys in r0

@ Here on return from dispatcher, r0 = swiKey+tskKey, interrupts disabled
        ldr     r2, swiTaskKeyAddr
        ldr     r1, [r2]
        ands    r0, r1, r0
        str     r0, [r2]

        pop     {r1}            @ restore EXC_RETURN
        movs    r2, #8
        tst     r1, r2          @ returning to thread mode?
        bne     lab$2           @ branch if lowest order interrupt

@
@ return now, no further processing on nested interrupts
@
@ This is normal when:
@  1) this is a nested interrupt,
@  2) the Task scheduler has been disabled by the interrupted task thread
@  3) this is a Swi-only system
@
        cpsie   i               @ re-enable interrupts
        bx      r1              @ return from interrupt

@
@ Here when no other interrupt is pending
@ perform a dummy EXC_RETURN to return to thread mode before
@ running the Swi and Task schedulers
@
lab$2:
        movs    r2, #4
        tst     r1, r2          @ context on PSP?
        beq     lab$21
        mrs     r2, psp         @ if yes, then use PSP
        subs    r2, #(8*4)      @ make room for dummy
        msr     psp, r2         @ update PSP
        b       lab$22
lab$21:
        mov     r2, sp          @ else use MSP
        subs    r2, #(8*4)      @ make room for dummy
        mov     sp, r2          @ update MSP

@ copy saved swi and task keys into interrupt context

lab$22:
        ldr     r3, swiTaskKeyAddr
        ldr     r0, [r3]
        str     r0, [r2]        @ tskkey pops into r0
        ldr     r0, swiTaskKeyDefault
        str     r0, [r3]        @ re-init swiTaskKey
        ldr     r0, dummyPSR    @ dummy xPSR with T bit set
        str     r0, [r2, #28]
        ldr     r0, lab$1Addr
        str     r0, [r2, #24]
        bx      r1              @ EXC_RETURN to $1 below

@
@ Now we're back in thread mode and could be running on the task stack
@ or the ISR stack.
@
@ If an ISR hit while a Swi posted from an ISR was running, we'll be on the ISR stack.
@ Otherwise we'll be on the pre-empted Task stack.
@
@       r0 = swiTaskKey
@       r1 = old basepri
@
@ Call Swi and Task schedulers
@
lab$1:
        push    {r0}            @ push copy of swiTaskKey

        mrs     r1, control     @ on ISR stack already?
        cmp     r1, #0
        beq     lab$12
        movs    r2, #0          @ no,
        msr     control, r2     @ switch to ISR stack

lab$12:
        push    {r1}            @ push old control reg onto ISR stack (MSP)
        push    {r1}            @ dummy to force 8 byte stack align

        ldr     r1, doSwiRestoreAddr
        blx     r1              @ pass swiTaskKey (r0) to ti_sysbios_family_arm_v6m_Hwi_doSwiRestore()

        pop     {r0}            @ pop dummy
        pop     {r0}            @ pop old control register

        cmp     r0, #0          @ lowest order ISR?
        beq     lab$13
        movs    r1, #2          @ if yes
        msr     control, r1     @ switch back to Task stack (PSP)

lab$13:
        pop     {r0}            @ pop copy of swiTaskKey
        ldr     r1, doTaskRestoreAddr
        blx     r1              @ pass swiTaskKey to ti_sysbios_family_arm_v6m_Hwi_doTaskRestore()
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
@ which is configured with the same priority level as ti_sysbios_family_arm_v6m_Hwi_disable().
@ Then we re-enable interrupts. This will cause an immediate pendSV
@ exception to be taken which is still at ti_sysbios_family_arm_v6m_Hwi_disable() priority.
@ We then blow off the pendSV stack contents and return to the
@ background thread at the normal priority level.
@
        ldr     r0, nvic_icsr   @ point to ti_sysbios_family_arm_v6m_Hwi_nvic.ICSR
        ldr     r1, pendSvBit
        str     r1, [r0]        @ post a pendSV interrupt

        pop     {r0}            @ retrieve old basepri

@ CAUTION!!! Do NOT single step thru the next instruction
@ else, the processor won't arrive at pendSV thru the
@ exception mechanism  and catastrophic things will happen
@ afterwards.

        cpsie   i               @ allows pendSV to happen
        nop                     @ 2 nops required for prefetch
        nop                     @

@ Arriving here can only happen if interrupts were globally disabled
@ when the pendSV interrupt was posted above. This can happen if
@ user code has set the PRIMASK and not cleared it, or when single
@ stepping with interrupts disabled.

ti_sysbios_family_arm_v6m_Hwi_interruptsAreDisabledButShouldNotBe:
        b   ti_sysbios_family_arm_v6m_Hwi_interruptsAreDisabledButShouldNotBe

        .align 2
lab$1Addr:
        .word       lab$1

        .thumb_func
ti_sysbios_family_arm_v6m_Hwi_pendSV:
        mov     r1, lr
        movs    r2, #4
        tst     r1, r2          @ context on PSP?
        beq     lab$1a
        mrs     r2, psp         @ if yes, then use PSP
        adds    r2, #(8*4)      @ blow off pendSV stack
        msr     psp, r2
        bx      r1              @ return into task, re-enables ints
lab$1a:
        mov     r2, sp          @ else use MSP
        adds    r2, #(8*4)      @ blow off pendSV stack
        mov     sp, r2
ti_sysbios_family_arm_v6m_Hwi_return:

        bx      r1              @ return into task, re-enables ints

        .align 2
nvic_icsr:
        .word   0xe000ed04
dispatchAddr:
        .word   ti_sysbios_family_arm_v6m_Hwi_dispatchC
doSwiRestoreAddr:
        .word   ti_sysbios_family_arm_v6m_Hwi_doSwiRestore
doTaskRestoreAddr:
        .word   ti_sysbios_family_arm_v6m_Hwi_doTaskRestore
swiTaskKeyAddr:
        .word   ti_sysbios_family_arm_v6m_Hwi_Module_state + 0x20
swiTaskKeyDefault:
        .word   0x00000101
dummyPSR:
        .word   0x01000000
pendSvBit:
        .word   0x10000000


        .thumb_func
ti_sysbios_family_arm_v6m_Hwi_excHandlerAsm:
        mov     r1, lr
        movs    r2, #4
        tst     r1, r2          @ context on PSP?
        beq     lab$1b          @ if not, use SP
        mrs     r0, psp         @ else, if yes, then use PSP
        mov     sp, r0
lab$1b:
        mov     r3, r11
        mov     r2, r10
        mov     r1, r9
        mov     r0, r8

        push    {r0-r3}
        push    {r4-r7}         @ save r4-r11 while we're at it
        mov     r0, sp          @ pass sp to exception handler
        mov     r1, lr          @ pass lr too

        ldr     r2, excHandlerAddr
        bx      r2


        .align 2
excHandlerAddr:
        .word   ti_sysbios_family_arm_v6m_Hwi_excHandler

        .thumb_func
NOROM_CPUcpsid:
        movs    r0,#1
        msr     PRIMASK, r0
        bx      lr

        .thumb_func
NOROM_CPUcpsie:
        movs    r0,#0
        msr     PRIMASK, r0
        bx      lr

        .thumb_func
NOROM_SetupTrimDevice:
        bx      lr

        .end
