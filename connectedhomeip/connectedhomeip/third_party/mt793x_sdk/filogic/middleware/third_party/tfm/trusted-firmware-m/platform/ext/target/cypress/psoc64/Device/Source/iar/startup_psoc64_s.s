;/*
; * Copyright (c) 2017-2018 ARM Limited
; * Copyright (c) 2019-2020, Cypress Semiconductor Corporation. All rights reserved.
; *
; * Licensed under the Apache License, Version 2.0 (the "License");
; * you may not use this file except in compliance with the License.
; * You may obtain a copy of the License at
; *
; *     http://www.apache.org/licenses/LICENSE-2.0
; *
; * Unless required by applicable law or agreed to in writing, software
; * distributed under the License is distributed on an "AS IS" BASIS,
; * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
; * See the License for the specific language governing permissions and
; * limitations under the License.
; */
;
; This file is adapted from ../armclang/startup_psoc64_s.s
; Git SHA: 8a1d9d6ee18b143ae5befefa14d89fb5b3f99c75

;/*
;//-------- <<< Use Configuration Wizard in Context Menu >>> ------------------
;*/


; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

; Address of the NMI handler in ROM
CY_NMI_HANLDER_ADDR    EQU    0x0000000D

                PRESERVE8

; Vector Table Mapped to Address 0 at Reset

                SECTION  ARM_LIB_STACK_MSP:DATA:NOROOT(3)
                SECTION  ARM_LIB_STACK:DATA:NOROOT(3)

                SECTION  .intvec:CODE:NOROOT(2)

                EXTERN   __iar_program_start
                EXTERN   SystemInit
                PUBLIC   __vector_table
                PUBLIC   __Vectors
                PUBLIC   __Vectors_End
                PUBLIC   __Vectors_Size
                PUBLIC  __ramVectors

                IMPORT  HardFault_Handler
                IMPORT  SVC_Handler
                IMPORT  PendSV_Handler
                IMPORT  NvicMux7_IRQHandler
                IMPORT  Cy_SysIpcPipeIsrCm0

                DATA

__vector_table       ;Core Interrupts
                DCD     sfe(ARM_LIB_STACK_MSP)    ; Top of Stack
                DCD     Reset_Handler             ; Reset Handler
                DCD     CY_NMI_HANLDER_ADDR       ; NMI Handler
                DCD     HardFault_Handler         ; Hard Fault Handler
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     SVC_Handler               ; SVCall Handler
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     PendSV_Handler            ; PendSV Handler
                DCD     SysTick_Handler           ; SysTick Handler

                ; External interrupts                           Description
                DCD     NvicMux0_IRQHandler                   ; CPU User Interrupt #0
                DCD     Cy_SysIpcPipeIsrCm0
                DCD     NvicMux2_IRQHandler                   ; CPU User Interrupt #2
                DCD     TFM_TIMER0_IRQ_Handler                ; CPU User Interrupt #3
                DCD     NvicMux4_IRQHandler                   ; CPU User Interrupt #4
                DCD     NvicMux5_IRQHandler                   ; CPU User Interrupt #5
                DCD     NvicMux6_IRQHandler                   ; CPU User Interrupt #6
                DCD     NvicMux7_IRQHandler                   ; CPU User Interrupt #7
                DCD     Internal0_IRQHandler                  ; Internal SW Interrupt #0
                DCD     Internal1_IRQHandler                  ; Internal SW Interrupt #1
                DCD     Internal2_IRQHandler                  ; Internal SW Interrupt #2
                DCD     Internal3_IRQHandler                  ; Internal SW Interrupt #3
                DCD     Internal4_IRQHandler                  ; Internal SW Interrupt #4
                DCD     Internal5_IRQHandler                  ; Internal SW Interrupt #5
                DCD     Internal6_IRQHandler                  ; Internal SW Interrupt #6
                DCD     Internal7_IRQHandler                  ; Internal SW Interrupt #7

__Vectors_End

__Vectors       EQU     __vector_table
__Vectors_Size  EQU     __Vectors_End - __Vectors

;                AREA    RESET_RAM, READWRITE, NOINIT
                DATA
__ramVectors
                DS8   __Vectors_Size

; Reset Handler
                SECTION  .text:CODE:REORDER:NOROOT(2)
Reset_Handler
                CPSID   i              ; Disable IRQs
                LDR     R0, =SystemInit
                BLX     R0
                LDR     R0, =sfe(ARM_LIB_STACK)      ; End of PROC_STACK
                MSR     PSP, R0
                MRS     R0, control    ; Get control value
                MOVS    R1, #2
                ORRS    R0, R0, R1     ; Select switch to PSP
                MSR     control, R0
                LDR     R0, =__iar_program_start
                BX      R0
End_Of_Main
                B       .


; Dummy Exception Handlers (infinite loops which can be modified)
Default_Handler MACRO handler_name
                PUBWEAK  handler_name
handler_name
                B       .
                ENDM

                Default_Handler SysTick_Handler
                Default_Handler NvicMux0_IRQHandler
                Default_Handler NvicMux2_IRQHandler
                Default_Handler TFM_TIMER0_IRQ_Handler
                Default_Handler NvicMux4_IRQHandler
                Default_Handler NvicMux5_IRQHandler
                Default_Handler NvicMux6_IRQHandler
                Default_Handler Internal0_IRQHandler
                Default_Handler Internal1_IRQHandler
                Default_Handler Internal2_IRQHandler
                Default_Handler Internal3_IRQHandler
                Default_Handler Internal4_IRQHandler
                Default_Handler Internal5_IRQHandler
                Default_Handler Internal6_IRQHandler
                Default_Handler Internal7_IRQHandler

                END
