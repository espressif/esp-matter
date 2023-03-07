;
;  Copyright (c) 2020, Texas Instruments Incorporated
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
; ======== TaskSupport_asm_iar.sv6M ========
;
;
        PRESERVE8

        PUBLIC ti_sysbios_knl_TaskSupport_swap
        PUBLIC ti_sysbios_knl_TaskSupport_glue
        PUBLIC ti_sysbios_knl_Task_swapReturn


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

        mov     r3, lr
        push    {r3}
        push    {r4-r7}
        mov     r2, r8
        mov     r3, r9
        mov     r4, r10
        mov     r5, r11
        push    {r2-r5}
        mov     r2, sp
        str     r2, [OLD]

ti_sysbios_knl_Task_swapReturn:
        ldr     r2, [NEW]
        mov     sp, r2
        pop     {r2-r5}
        mov     r8, r2
        mov     r9, r3
        mov     r10,r4
        mov     r11,r5
        pop     {r4-r7, pc}

;
;  ======== ti_sysbios_knl_TaskSupport_glue ========
;
;  This glue function is executed immediately before executing the "fxn"
;  passed to Task_start.  This function loads the parameters to "fxn" into
;  registers - necessary for the v6m register passing model.
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

        ldr     r3, [sp, #12]   ; Task exit function
        mov     lr, r3
        bx      r2              ; call task fxn,  ("returns" to exit func)

        END
