;
;  Copyright (c) 2016, Texas Instruments Incorporated
;  http://www.ti.com
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
; ======== tls_get_tp.asm ========
;

;******************************************************************************
;* __c6xabi_get_tp() - This function must return the Thread Pointer (TP) of   *
;*                     the current thread. The definition provided below is   *
;*                     for the case where the SYS/BIOS runtime manages TLS    *
;*                     and returns the current thread's TP.                   *
;*                                                                            *
;*  NOTE: This function must not modify any register other than the return    *
;*        register A4 and B30/B31                                             *
;******************************************************************************

;;
;; It seems we can't include more than one XDC-generated C header file,
;; else we get tons of errors such as:
;;     "/tmp/750661e3kvY", ERROR!   at line 504: [E0300] Enumeration tag can't be global
;;        xdc_runtime_Types_CreatePolicy      .enum
;;
;;     "/tmp/750661e3kvY", ERROR!   at line 543: [E0300] Structure tag can't be global
;;        xdc_runtime_Core_ObjDesc                  .struct 0,1          ; struct size=(0 bytes|0 bits), alignment=0
;;
;; It is more useful to have the BIOS.xdc.h header file in play than the
;; ThreadLocalStorage.xdc.h header file, since a broken hard-coded value
;; within ThreadLocalStorage (i.e., the module to which this file belongs)
;; that is caused by a change in this module is easier to find than a broken
;; hard-coded value within the BIOS module when the unrelated BIOS module is
;; changed.
;;

;;        .cdecls C,NOLIST,"package/internal/ThreadLocalStorage.xdc.h"
;;ti_sysbios_rts_ti_ThreadLocalStorage_Module__state__V .tag ti_sysbios_rts_ti_ThreadLocalStorage_Module_State

        .cdecls C,NOLIST,"ti/sysbios/package/internal/BIOS.xdc.h"
ti_sysbios_BIOS_Module__state__V .tag ti_sysbios_BIOS_Module_State

        .asg ti_sysbios_BIOS_Module__state__V, BIOS_Module__state__V
        .asg ti_sysbios_rts_ti_ThreadLocalStorage_Module__state__V, ThreadLocalStorage_Module__state__V

        .global BIOS_Module__state__V
        .global ThreadLocalStorage_Module__state__V
        .global __TI_TLS_MAIN_THREAD_Base
        .weak   __TI_TLS_MAIN_THREAD_Base


        .sect   ".text:tls:get_tp"
        .clink
        .global __c6xabi_get_tp
        .weak   __c6xabi_get_tp

;;
;; __c6xabi_get_tp is a special API used by the compiler in the TLS
;; model, in that the compiler expects that it modifies *only* registers
;; A4 (which it must modify since it's the return value register) and B30/B31.
;;
;; This asm implementation represents the following C code:
;;
;;Ptr __c6xabi_get_tp()
;;{
;;    if (BIOS_getThreadType() == BIOS_ThreadType_Task) {
;;        return ThreadLocalStorage_module->currentTP;
;;    }
;;    else {
;;        return &__TI_TLS_MAIN_THREAD_Base;
;;    }
;;}
;;

__c6xabi_get_tp:  .asmfunc
        STDW          B1:B0, *B15--[1]
||      MVKL          BIOS_Module__state__V.threadType, B0
||      MVKL          ThreadLocalStorage_Module__state__V+0x0, A4
        MVKH          BIOS_Module__state__V.threadType, B0
||      MVKH          ThreadLocalStorage_Module__state__V+0x0, A4
        LDW           *+B0[0], B0
        LDW           *+A4[0], A4
        NOP           3
        CMPEQ         BIOS_ThreadType.BIOS_ThreadType_Task, B0, B1
||      B             B3
        LDDW           *++B15[1], B1:B0
  [!B1] MVKL          __TI_TLS_MAIN_THREAD_Base, A4  ; thread != Task
  [!B1] MVKH          __TI_TLS_MAIN_THREAD_Base, A4  ; thread != Task
        NOP           2
            .endasmfunc
