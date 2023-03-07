;/*
; * Copyright (c) 2016-2020 Arm Limited
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
; This file is derivative of CMSIS V5.00 startup_ARMv8MML.s

;/*
;//-------- <<< Use Configuration Wizard in Context Menu >>> ------------------
;*/


; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

                IMPORT |Image$$ARM_LIB_STACK_MSP$$ZI$$Limit|

; Vector Table Mapped to Address 0 at Reset

                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size

__Vectors       DCD     |Image$$ARM_LIB_STACK_MSP$$ZI$$Limit|  ; Top of Stack
                DCD     Reset_Handler             ; Reset Handler
                DCD     NMI_Handler               ; NMI Handler
                DCD     HardFault_Handler         ; Hard Fault Handler
                DCD     MemManage_Handler         ; MPU Fault Handler
                DCD     BusFault_Handler          ; Bus Fault Handler
                DCD     UsageFault_Handler        ; Usage Fault Handler
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     SVC_Handler               ; SVCall Handler
                DCD     DebugMon_Handler          ; Debug Monitor Handler
                DCD     0                         ; Reserved
                DCD     PendSV_Handler            ; PendSV Handler
                DCD     SysTick_Handler           ; SysTick Handler

                ; Core IoT Interrupts
                DCD     NONSEC_WATCHDOG_RESET_Handler  ; - 0 Non-Secure Watchdog Reset Handler
                DCD     NONSEC_WATCHDOG_Handler        ; - 1 Non-Secure Watchdog Handler
                DCD     S32K_TIMER_Handler             ; - 2 S32K Timer Handler
                DCD     TIMER0_Handler                 ; - 3 TIMER 0 Handler
                DCD     TIMER1_Handler                 ; - 4 TIMER 1 Handler
                DCD     DUALTIMER_Handler              ; - 5 Dual Timer Handler
                DCD     MHU0_Handler                   ; - 6 Message Handling Unit 0
                DCD     MHU1_Handler                   ; - 7 Message Handling Unit 1
                DCD     0                              ; Reserved - 8
                DCD     0                              ; Reserved - 9
                DCD     0                              ; Reserved - 10
                DCD     MSC_Handler                    ; - 11 MSC Combined (Secure) Handler
                DCD     BRIDGE_ERROR_Handler           ; - 12 Bridge Error Combined (Secure) Handler
                DCD     0                              ; Reserved - 13
                DCD     0                              ; Reserved - 14
                DCD     0                              ; Reserved - 15
                DCD     0                              ; Reserved - 16
                DCD     0                              ; Reserved - 17
                DCD     0                              ; Reserved - 18
                DCD     0                              ; Reserved - 19
                DCD     0                              ; Reserved - 20
                DCD     0                              ; Reserved - 21
                DCD     0                              ; Reserved - 22
                DCD     0                              ; Reserved - 23
                DCD     0                              ; Reserved - 24
                DCD     0                              ; Reserved - 25
                DCD     0                              ; Reserved - 26
                DCD     0                              ; Reserved - 27
                DCD     0                              ; Reserved - 28
                DCD     0                              ; Reserved - 29
                DCD     0                              ; Reserved - 30
                DCD     0                              ; Reserved - 31
                ; External Interrupts
                DCD     UARTRX0_Handler           ; 32 UART 0 RX Handler
                DCD     UARTTX0_Handler           ; 33 UART 0 TX Handler
                DCD     0                         ; 34 Reserved
                DCD     0                         ; 35 Reserved
                DCD     0                         ; 36 Reserved
                DCD     0                         ; 37 Reserved
                DCD     0                         ; 38 Reserved
                DCD     0                         ; 39 Reserved
                DCD     0                         ; 40 Reserved
                DCD     0                         ; 41 Reserved
                DCD     UART0_Handler             ; 42 UART 0 combined Handler
                DCD     0                         ; 43 Reserved
                DCD     0                         ; 44 Reserved
                DCD     0                         ; 45 Reserved
                DCD     0                         ; 46 Reserved
                DCD     UARTOVF_Handler           ; 47 UART 0 Overflow Handler
                DCD     0                         ; 48 Reserved
                DCD     0                         ; 49 Reserved
                DCD     0                         ; 50 Reserved
                DCD     0                         ; 51 Reserved
                DCD     0                         ; 52 Reserved
                DCD     0                         ; 53 Reserved
                DCD     0                         ; 54 Reserved
                DCD     0                         ; 55 Reserved
                DCD     DMA0_ERROR_Handler        ; 56 DMA 0 Error Handler
                DCD     DMA0_TC_Handler           ; 57 DMA 0 Terminal Count Handler
                DCD     DMA0_Handler              ; 58 DMA 0 Combined Handler
                DCD     DMA1_ERROR_Handler        ; 59 DMA 1 Error Handler
                DCD     DMA1_TC_Handler           ; 60 DMA 1 Terminal Count Handler
                DCD     DMA1_Handler              ; 61 DMA 1 Combined Handler
                DCD     DMA2_ERROR_Handler        ; 62 DMA 2 Error Handler
                DCD     DMA2_TC_Handler           ; 63 DMA 2 Terminal Count Handler
                DCD     DMA2_Handler              ; 64 DMA 2 Combined Handler
                DCD     DMA3_ERROR_Handler        ; 65 DMA 3 Error Handler
                DCD     DMA3_TC_Handler           ; 66 DMA 3 Terminal Count Handler
                DCD     DMA3_Handler              ; 67 DMA 3 Combined Handler
                DCD     0                         ; 68 Reserved
                DCD     0                         ; 69 Reserved
                DCD     0                         ; 70 Reserved
                DCD     0                         ; 71 Reserved
                DCD     0                         ; 72 Reserved
                DCD     0                         ; 73 Reserved
                DCD     0                         ; 74 Reserved
                DCD     0                         ; 75 Reserved
                DCD     0                         ; 76 Reserved
                DCD     0                         ; 77 Reserved
                DCD     0                         ; 78 Reserved
                DCD     0                         ; 79 Reserved
                DCD     0                         ; 80 Reserved
                DCD     0                         ; 81 Reserved
                DCD     0                         ; 82 Reserved
                DCD     0                         ; 83 Reserved
                DCD     0                         ; 84 Reserved
                DCD     0                         ; 85 Reserved
                DCD     0                         ; 86 Reserved
                DCD     0                         ; 87 Reserved
                DCD     0                         ; 88 Reserved
                DCD     0                         ; 89 Reserved
                DCD     0                         ; 90 Reserved
                DCD     0                         ; 91 Reserved
                DCD     0                         ; 92 Reserved
                DCD     0                         ; 93 Reserved
                DCD     0                         ; 94 Reserved
                DCD     0                         ; 95 Reserved
__Vectors_End

__Vectors_Size  EQU     __Vectors_End - __Vectors

                AREA    |.text|, CODE, READONLY


; Reset Handler

Reset_Handler   PROC
                EXPORT  Reset_Handler             [WEAK]
                IMPORT  __main
                MRS     R0, control    ; Get control value
                ORR     R0, R0, #2     ; Select switch to PSP
                MSR     control, R0
                LDR     R0, =__main
                BX      R0
                ENDP


; Dummy Exception Handlers (infinite loops which can be modified)
                MACRO
                Default_Handler $handler_name
$handler_name   PROC
                EXPORT  $handler_name             [WEAK]
                B       .
                ENDP
                MEND

                Default_Handler NMI_Handler
                Default_Handler HardFault_Handler
                Default_Handler MemManage_Handler
                Default_Handler BusFault_Handler
                Default_Handler UsageFault_Handler
                Default_Handler SecureFault_Handler
                Default_Handler SVC_Handler
                Default_Handler DebugMon_Handler
                Default_Handler PendSV_Handler
                Default_Handler SysTick_Handler

; Core IoT Interrupts
                Default_Handler NONSEC_WATCHDOG_RESET_Handler ; - 0 Non-Secure Watchdog Reset Handler
                Default_Handler NONSEC_WATCHDOG_Handler       ; - 1 Non-Secure Watchdog Handler
                Default_Handler S32K_TIMER_Handler            ; - 2 S32K Timer Handler
                Default_Handler TIMER0_Handler                ; - 3 TIMER 0 Handler
                Default_Handler TIMER1_Handler                ; - 4 TIMER 1 Handler
                Default_Handler DUALTIMER_Handler             ; - 5 Dual Timer Handler
                Default_Handler MHU0_Handler                  ; - 6 Message Handling Unit 0
                Default_Handler MHU1_Handler                  ; - 7 Message Handling Unit 1
                Default_Handler MSC_Handler                   ; - 11 MSC Combined (Secure) Handler
                Default_Handler BRIDGE_ERROR_Handler          ; - 12 Bridge Error Combined (Secure) Handler
; External Interrupts
                Default_Handler UARTRX0_Handler             ; 32 UART 0 RX Handler
                Default_Handler UARTTX0_Handler             ; 33 UART 0 TX Handler
                Default_Handler UART0_Handler               ; 42 UART 0 combined Handler
                Default_Handler UARTOVF_Handler             ; 47 UART 0 Overflow Handler
                Default_Handler DMA0_ERROR_Handler          ; 56 DMA 0 Error Handler
                Default_Handler DMA0_TC_Handler             ; 57 DMA 0 Terminal Count Handler
                Default_Handler DMA0_Handler                ; 58 DMA 0 Combined Handler
                Default_Handler DMA1_ERROR_Handler          ; 59 DMA 1 Error Handler
                Default_Handler DMA1_TC_Handler             ; 60 DMA 1 Terminal Count Handler
                Default_Handler DMA1_Handler                ; 61 DMA 1 Combined Handler
                Default_Handler DMA2_ERROR_Handler          ; 62 DMA 2 Error Handler
                Default_Handler DMA2_TC_Handler             ; 63 DMA 2 Terminal Count Handler
                Default_Handler DMA2_Handler                ; 64 DMA 2 Combined Handler
                Default_Handler DMA3_ERROR_Handler          ; 65 DMA 3 Error Handler
                Default_Handler DMA3_TC_Handler             ; 66 DMA 3 Terminal Count Handler
                Default_Handler DMA3_Handler                ; 67 DMA 3 Combined Handler

                ALIGN

                END
