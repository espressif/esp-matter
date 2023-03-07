;
;  Copyright (c) 2013-2020, Texas Instruments Incorporated
;  All rights reserved.
;
;  Redistribution and use in source and binary forms, with or without
;  modification, are permitted provided that the following conditions
;  are met:
;
;  *  Redistributions of source code must retain the above copyright
;     notice, this list of conditions and the following disclaimer.
;
;  *  Redistributions in binary form must reproduce the above copyright
;     notice, this list of conditions and the following disclaimer in the
;     documentation and/or other materials provided with the distribution.
;
;  *  Neither the name of Texas Instruments Incorporated nor the names of
;     its contributors may be used to endorse or promote products derived
;     from this software without specific prior written permission.
;
;  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
;  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
;  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
;  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
;  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
;  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
;  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
;  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
;  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
;  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
;  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;
;
; ======== TaskSupport_asm_iar.sv7M ========
;
;
        PRESERVE8

        PUBLIC ti_sysbios_knl_TaskSupport_swap
        PUBLIC ti_sysbios_knl_TaskSupport_glue
        PUBLIC ti_sysbios_knl_TaskSupport_glueUnpriv
        PUBLIC ti_sysbios_knl_TaskSupport_swapReturn

;
;  ======== ti_sysbios_knl_TaskSupport_swap ========
;
;  ti_sysbios_knl_TaskSupport_swap(oldSP, newSP)
;

; 1st argument
#define OLD      r0
; 2nd argument
#define NEW      r1

        SECTION CODE:CODE:NOROOT
        THUMB
ti_sysbios_knl_TaskSupport_swap:

#if defined(ti_sysbios_knl_TaskSupport_usesMonitors__D)
        clrex
#endif
        push    {r4-r11, lr}
#if defined(__ARMVFP__)
        vpush   {d8-d15}
#endif
#if (__CORE__ == __ARM8M_MAINLINE__)
        mrs     r2, psplim
        push    {r2, r3}
        mov     r2, #0
        msr     psplim, r2
#endif
        str     sp, [OLD]

ti_sysbios_knl_TaskSupport_swapReturn:
        ldr     sp, [NEW]
#if (__CORE__ == __ARM8M_MAINLINE__)
        pop     {r2, r3}
        msr     psplim, r3
#endif
#if defined(__ARMVFP__)
        vpop    {d8-d15}                ; pop vfp caller regs
#endif
        pop     {r4-r11, pc}

;
;  ======== ti_sysbios_knl_TaskSupport_glue ========
;
;  This glue function is executed immediately before executing the "fxn"
;  passed to Task_start.  This function loads the parameters to "fxn" into
;  registers - necessary for the m3 register passing model.
;
        SECTION CODE:CODE:NOROOT
        THUMB

ti_sysbios_knl_TaskSupport_glue:

        ldr     r0, [sp, #16]   ; Task enter function
        blx     r0              ; call Task enter function

; return from Task enter function here
        ldr     r0, [sp, #0]    ; task arg0
        ldr     r1, [sp, #4]    ; task arg1
        ldr     r2, [sp, #8]    ; task fxn

        ldr     lr, [sp, #12]   ; Task exit function
        bx      r2              ; call task fxn,  ("returns" to exit func)

;
;  ======== ti_sysbios_knl_TaskSupport_glueUnpriv ========
;
;        SECTION CODE:CODE:NOROOT
;        THUMB

ti_sysbios_knl_TaskSupport_glueUnpriv:

; This is an empty stub for targets not supporting
; memory protection extensions. It is added to
; resolve code references at build time and will
; never be invoked.

        bx      lr

        END
