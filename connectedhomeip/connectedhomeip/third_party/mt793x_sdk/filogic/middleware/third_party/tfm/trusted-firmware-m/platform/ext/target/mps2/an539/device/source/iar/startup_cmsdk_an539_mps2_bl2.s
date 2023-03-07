;/*
; * Copyright (c) 2016-2020 ARM Limited
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
; This file is derivative of ../armclang/startup_cmsdk_mps2_an519_bl2.s

;/*
;//-------- <<< Use Configuration Wizard in Context Menu >>> ------------------
;*/


; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

                MODULE   ?cstartup

                ;; Forward declaration of sections.
                SECTION  ARM_LIB_STACK:DATA:NOROOT(3)

                SECTION  .intvec:CODE:NOROOT(2)

                EXTERN   __iar_program_start
                EXTERN   SystemInit
                PUBLIC   __vector_table
                PUBLIC   __Vectors
                PUBLIC   __Vectors_End
                PUBLIC   __Vectors_Size
                DATA


__vector_table  DCD     sfe(ARM_LIB_STACK)        ; Top of Stack
                DCD     Reset_Handler             ; Reset Handler
                DCD     NMI_Handler               ; NMI Handler
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

                ; Core IoT Interrupts
                DCD     NONSEC_WATCHDOG_RESET_Handler  ; - 0 Non-Secure Watchdog Reset Handler
                DCD     NONSEC_WATCHDOG_Handler        ; - 1 Non-Secure Watchdog Handler
                DCD     0                              ; - 2 Reserved
                DCD     TFM_TIMER0_IRQ_Handler         ; - 3 TIMER 0 IRQ Handler
                DCD     TIMER1_Handler                 ; - 4 TIMER 1 IRQ Handler
                DCD     0                              ; - 5 Reserved
                DCD     CTI_REQ0_IRQHandler            ; - 6 CTI request 0 IRQ Handler
                DCD     CTI_REQ1_IRQHandler            ; - 7 CTI request 1 IRQ Handler
                DCD     0                              ;   8 Reserved
                DCD     MPC_Handler                    ; - 9 MPC Combined (Secure) Handler
                DCD     PPC_Handler                    ; - 10 PPC Combined (Secure) Handler
                DCD     MSC_Handler                    ; - 11 MSC Combined (Secure) Handler
                DCD     BRIDGE_ERROR_Handler           ; - 12 Bridge Error Combined (Secure) Handler
                DCD     0                              ; - 13 Reserved
                DCD     0                              ;   14 Reserved
                DCD     PD_SYS_PPU_IRQHandler          ; - 15 SYS PPU Handler
                DCD     0                              ; - 16 Reserved
                DCD     0                              ; - 17 Reserved
                DCD     0                              ; - 18 Reserved
                DCD     0                              ; - 19 Reserved
                DCD     0                              ; - 20 Reserved
                DCD     0                              ; - 21 Reserved
                DCD     0                              ; - 22 Reserved
                DCD     0                              ; - 23 Reserved
                DCD     0                              ; - 24 Reserved
                DCD     0                              ; - 25 Reserved
                DCD     0                              ; - 26 Reserved
                DCD     0                              ; - 27 Reserved
                DCD     0                              ; - 28 Reserved
                DCD     0                              ; - 29 Reserved
                DCD     0                              ; - 30 Reserved
                DCD     0                              ; - 31 Reserved
                ; External Interrupts
                DCD     UARTRX0_Handler           ; 32 UART 0 RX Handler
                DCD     UARTTX0_Handler           ; 33 UART 0 TX Handler
                DCD     UARTRX1_Handler           ; 34 UART 1 RX Handler
                DCD     UARTTX1_Handler           ; 35 UART 1 TX Handler
                DCD     UARTRX2_Handler           ; 36 UART 2 RX Handler
                DCD     UARTTX2_Handler           ; 37 UART 2 TX Handler
                DCD     UARTRX3_Handler           ; 38 UART 3 RX Handler
                DCD     UARTTX3_Handler           ; 39 UART 3 TX Handler
                DCD     UARTRX4_Handler           ; 40 UART 4 RX Handler
                DCD     UARTTX4_Handler           ; 41 UART 4 TX Handler
                DCD     UART0_Handler             ; 42 UART 0 combined Handler
                DCD     UART1_Handler             ; 43 UART 1 combined Handler
                DCD     UART2_Handler             ; 44 UART 2 combined Handler
                DCD     UART3_Handler             ; 45 UART 3 combined Handler
                DCD     UART4_Handler             ; 46 UART 4 combined Handler
                DCD     UARTOVF_Handler           ; 47 UART 0,1,2,3,4 Overflow Handler
                DCD     ETHERNET_Handler          ; 48 Ethernet Handler
                DCD     I2S_Handler               ; 49 I2S Handler
                DCD     TSC_Handler               ; 50 Touch Screen Handler
                DCD     SPI0_Handler              ; 51 SPI 0 Handler
                DCD     SPI1_Handler              ; 52 SPI 1 Handler
                DCD     SPI2_Handler              ; 53 SPI 2 Handler
                DCD     SPI3_Handler              ; 54 SPI 3 Handler
                DCD     SPI4_Handler              ; 55 SPI 4 Handler
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
                DCD     GPIO0_Handler             ; 68 GPIO 0 Comboned Handler
                DCD     GPIO1_Handler             ; 69 GPIO 1 Comboned Handler
                DCD     GPIO2_Handler             ; 70 GPIO 2 Comboned Handler
                DCD     GPIO3_Handler             ; 71 GPIO 3 Comboned Handler
                DCD     GPIO0_0_Handler           ; 72 /* There are 16 pins for GPIO 0 */
                DCD     GPIO0_1_Handler           ; 73
                DCD     GPIO0_2_Handler           ; 74
                DCD     GPIO0_3_Handler           ; 75
                DCD     GPIO0_4_Handler           ; 76
                DCD     GPIO0_5_Handler           ; 77
                DCD     GPIO0_6_Handler           ; 78
                DCD     GPIO0_7_Handler           ; 79
                DCD     GPIO0_8_Handler           ; 80
                DCD     GPIO0_9_Handler           ; 81
                DCD     GPIO0_10_Handler          ; 82
                DCD     GPIO0_11_Handler          ; 83
                DCD     GPIO0_12_Handler          ; 84
                DCD     GPIO0_13_Handler          ; 85
                DCD     GPIO0_14_Handler          ; 86
                DCD     GPIO0_15_Handler          ; 87
                DCD     GPIO1_0_Handler           ; 88 /* There are 16 pins for GPIO 1 */
                DCD     GPIO1_1_Handler           ; 89
                DCD     GPIO1_2_Handler           ; 90
                DCD     GPIO1_3_Handler           ; 91
                DCD     GPIO1_4_Handler           ; 92
                DCD     GPIO1_5_Handler           ; 93
                DCD     GPIO1_6_Handler           ; 94
                DCD     GPIO1_7_Handler           ; 95
                DCD     GPIO1_8_Handler           ; 96
                DCD     GPIO1_9_Handler           ; 97
                DCD     GPIO1_10_Handler          ; 98
                DCD     GPIO1_11_Handler          ; 99
                DCD     GPIO1_12_Handler          ; 100
                DCD     GPIO1_13_Handler          ; 101
                DCD     GPIO1_14_Handler          ; 102
                DCD     GPIO1_15_Handler          ; 103
                DCD     GPIO2_0_Handler           ; 104 /* There are 16 pins for GPIO 2 */
                DCD     GPIO2_1_Handler           ; 105
                DCD     GPIO2_2_Handler           ; 106
                DCD     GPIO2_3_Handler           ; 107
                DCD     GPIO2_4_Handler           ; 108
                DCD     GPIO2_5_Handler           ; 109
                DCD     GPIO2_6_Handler           ; 110
                DCD     GPIO2_7_Handler           ; 111
                DCD     GPIO2_8_Handler           ; 112
                DCD     GPIO2_9_Handler           ; 113
                DCD     GPIO2_10_Handler          ; 114
                DCD     GPIO2_11_Handler          ; 115
                DCD     GPIO2_12_Handler          ; 116
                DCD     GPIO2_13_Handler          ; 117
                DCD     GPIO2_14_Handler          ; 118
                DCD     GPIO2_15_Handler          ; 119
                DCD     GPIO3_0_Handler           ; 120 /* There are 4 pins for GPIO 4 */
                DCD     GPIO3_1_Handler           ; 121
                DCD     GPIO3_2_Handler           ; 122
                DCD     GPIO3_3_Handler           ; 123

__Vectors_End

__Vectors       EQU     __vector_table
__Vectors_Size  EQU     __Vectors_End - __Vectors

; Reset Handler
                PUBWEAK  Reset_Handler
                SECTION  .text:CODE:REORDER:NOROOT(2)
Reset_Handler
                LDR     R0, =SystemInit
                BLX     R0
                LDR     R0, =__iar_program_start
                BX      R0

; Dummy Exception Handlers (infinite loops which can be modified)
Default_Handler MACRO handler_name
                PUBWEAK  handler_name
handler_name
                B       .
                ENDM

                Default_Handler NMI_Handler
                Default_Handler HardFault_Handler
                Default_Handler SVC_Handler
                Default_Handler PendSV_Handler
                Default_Handler SysTick_Handler

; Core IoT Interrupts
                Default_Handler NONSEC_WATCHDOG_RESET_Handler ; - 0 Non-Secure Watchdog Reset Handler
                Default_Handler NONSEC_WATCHDOG_Handler       ; - 1 Non-Secure Watchdog Handler
                Default_Handler TFM_TIMER0_IRQ_Handler        ; - 3 TIMER 0 Handler
                Default_Handler TIMER1_Handler                ; - 4 TIMER 1 Handler
                Default_Handler CTI_REQ0_IRQHandler           ; - 6 CTI request 0 IRQ Handler
                Default_Handler CTI_REQ1_IRQHandler           ; - 7 CTI request 1 IRQ Handler
                Default_Handler MPC_Handler                   ; - 9 MPC Combined (Secure) Handler
                Default_Handler PPC_Handler                   ; - 10 PPC Combined (Secure) Handler
                Default_Handler MSC_Handler                   ; - 11 MSC Combined (Secure) Handler
                Default_Handler BRIDGE_ERROR_Handler          ; - 12 Bridge Error Combined (Secure) Handler
                Default_Handler PD_SYS_PPU_IRQHandler         ; - 15 SYS PPU Handler
                ; External Interrupts
                Default_Handler UARTRX0_Handler     ; 32 UART 0 RX Handler
                Default_Handler UARTTX0_Handler     ; 33 UART 0 TX Handler
                Default_Handler UARTRX1_Handler     ; 34 UART 1 RX Handler
                Default_Handler UARTTX1_Handler     ; 35 UART 1 TX Handler
                Default_Handler UARTRX2_Handler     ; 36 UART 2 RX Handler
                Default_Handler UARTTX2_Handler     ; 37 UART 2 TX Handler
                Default_Handler UARTRX3_Handler     ; 38 UART 3 RX Handler
                Default_Handler UARTTX3_Handler     ; 39 UART 3 TX Handler
                Default_Handler UARTRX4_Handler     ; 40 UART 4 RX Handler
                Default_Handler UARTTX4_Handler     ; 41 UART 4 TX Handler
                Default_Handler UART0_Handler       ; 42 UART 0 combined Handler
                Default_Handler UART1_Handler       ; 43 UART 1 combined Handler
                Default_Handler UART2_Handler       ; 44 UART 2 combined Handler
                Default_Handler UART3_Handler       ; 45 UART 3 combined Handler
                Default_Handler UART4_Handler       ; 46 UART 4 combined Handler
                Default_Handler UARTOVF_Handler     ; 47 UART 0,1,2,3,4 Overflow Handler
                Default_Handler ETHERNET_Handler    ; 48 Ethernet Handler
                Default_Handler I2S_Handler         ; 49 I2S Handler
                Default_Handler TSC_Handler         ; 50 Touch Screen Handler
                Default_Handler SPI0_Handler        ; 51 SPI 0 Handler
                Default_Handler SPI1_Handler        ; 52 SPI 1 Handler
                Default_Handler SPI2_Handler        ; 53 SPI 2 Handler
                Default_Handler SPI3_Handler        ; 54 SPI 3 Handler
                Default_Handler SPI4_Handler        ; 55 SPI 4 Handler
                Default_Handler DMA0_ERROR_Handler  ; 56 DMA 0 Error Handler
                Default_Handler DMA0_TC_Handler     ; 57 DMA 0 Terminal Count Handler
                Default_Handler DMA0_Handler        ; 58 DMA 0 Combined Handler
                Default_Handler DMA1_ERROR_Handler  ; 59 DMA 1 Error Handler
                Default_Handler DMA1_TC_Handler     ; 60 DMA 1 Terminal Count Handler
                Default_Handler DMA1_Handler        ; 61 DMA 1 Combined Handler
                Default_Handler DMA2_ERROR_Handler  ; 62 DMA 2 Error Handler
                Default_Handler DMA2_TC_Handler     ; 63 DMA 2 Terminal Count Handler
                Default_Handler DMA2_Handler        ; 64 DMA 2 Combined Handler
                Default_Handler DMA3_ERROR_Handler  ; 65 DMA 3 Error Handler
                Default_Handler DMA3_TC_Handler     ; 66 DMA 3 Terminal Count Handler
                Default_Handler DMA3_Handler        ; 67 DMA 3 Combined Handler
                Default_Handler GPIO0_Handler       ; 68 GPIO 0 Comboned Handler
                Default_Handler GPIO1_Handler       ; 69 GPIO 1 Comboned Handler
                Default_Handler GPIO2_Handler       ; 70 GPIO 2 Comboned Handler
                Default_Handler GPIO3_Handler       ; 71 GPIO 3 Comboned Handler
                Default_Handler GPIO0_0_Handler     ; 72 GPIO 0 has 16 individual Handlers
                Default_Handler GPIO0_1_Handler     ; 73
                Default_Handler GPIO0_2_Handler     ; 74
                Default_Handler GPIO0_3_Handler     ; 75
                Default_Handler GPIO0_4_Handler     ; 76
                Default_Handler GPIO0_5_Handler     ; 77
                Default_Handler GPIO0_6_Handler     ; 78
                Default_Handler GPIO0_7_Handler     ; 79
                Default_Handler GPIO0_8_Handler     ; 80
                Default_Handler GPIO0_9_Handler     ; 81
                Default_Handler GPIO0_10_Handler    ; 82
                Default_Handler GPIO0_11_Handler    ; 83
                Default_Handler GPIO0_12_Handler    ; 84
                Default_Handler GPIO0_13_Handler    ; 85
                Default_Handler GPIO0_14_Handler    ; 86
                Default_Handler GPIO0_15_Handler    ; 87
                Default_Handler GPIO1_0_Handler     ; 88 GPIO 1 has 16 individual Handlers
                Default_Handler GPIO1_1_Handler     ; 89
                Default_Handler GPIO1_2_Handler     ; 90
                Default_Handler GPIO1_3_Handler     ; 91
                Default_Handler GPIO1_4_Handler     ; 92
                Default_Handler GPIO1_5_Handler     ; 93
                Default_Handler GPIO1_6_Handler     ; 94
                Default_Handler GPIO1_7_Handler     ; 95
                Default_Handler GPIO1_8_Handler     ; 96
                Default_Handler GPIO1_9_Handler     ; 97
                Default_Handler GPIO1_10_Handler    ; 98
                Default_Handler GPIO1_11_Handler    ; 99
                Default_Handler GPIO1_12_Handler    ; 100
                Default_Handler GPIO1_13_Handler    ; 101
                Default_Handler GPIO1_14_Handler    ; 102
                Default_Handler GPIO1_15_Handler    ; 103
                Default_Handler GPIO2_0_Handler     ; 104 GPIO 2 has 16 individual Handlers
                Default_Handler GPIO2_1_Handler     ; 105
                Default_Handler GPIO2_2_Handler     ; 106
                Default_Handler GPIO2_3_Handler     ; 107
                Default_Handler GPIO2_4_Handler     ; 108
                Default_Handler GPIO2_5_Handler     ; 109
                Default_Handler GPIO2_6_Handler     ; 110
                Default_Handler GPIO2_7_Handler     ; 111
                Default_Handler GPIO2_8_Handler     ; 112
                Default_Handler GPIO2_9_Handler     ; 113
                Default_Handler GPIO2_10_Handler    ; 114
                Default_Handler GPIO2_11_Handler    ; 115
                Default_Handler GPIO2_12_Handler    ; 116
                Default_Handler GPIO2_13_Handler    ; 117
                Default_Handler GPIO2_14_Handler    ; 118
                Default_Handler GPIO2_15_Handler    ; 119
                Default_Handler GPIO3_0_Handler     ; 120 GPIO 3 has 16 individual Handlers
                Default_Handler GPIO3_1_Handler     ; 121
                Default_Handler GPIO3_2_Handler     ; 122
                Default_Handler GPIO3_3_Handler     ; 123

                END
