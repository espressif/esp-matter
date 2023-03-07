;/*
; * Copyright (c) 2016-2020 Arm Limited. All rights reserved.
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


; <h> Process Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

    IMPORT |Image$$ARM_LIB_STACK_MSP$$ZI$$Limit|

; Vector Table Mapped to Address 0 at Reset

    AREA    RESET, DATA, READONLY
    EXPORT  __Vectors
    EXPORT  __Vectors_End
    EXPORT  __Vectors_Size

__Vectors
    DCD     |Image$$ARM_LIB_STACK_MSP$$ZI$$Limit|  ; Top of Stack
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
    DCD     NONSEC_WATCHDOG_RESET_Handler ; 0 Non-Secure Watchdog Reset Handler
    DCD     NONSEC_WATCHDOG_Handler       ; 1 Non-Secure Watchdog Handler
    DCD     S32K_TIMER_Handler            ; 2 S32K Timer Handler
    DCD     TIMER0_Handler                ; 3 TIMER 0 Handler
    DCD     TIMER1_Handler                ; 4 TIMER 1 Handler
    DCD     DUALTIMER_Handler             ; 5 Dual Timer Handler
    DCD     0                             ; 6 Reserved
    DCD     0                             ; 7 Reserved
    DCD     0                             ; 8 Reserved
    DCD     0                             ; 9 Reserved
    DCD     0                             ; 10 Reserved
    DCD     0                             ; 11 Reserved
    DCD     0                             ; 12 Reserved
    DCD     0                             ; 13 Reserved
    DCD     0                             ; 14 Reserved
    DCD     0                             ; 15 Reserved
    DCD     0                             ; 16 Reserved
    DCD     0                             ; 17 Reserved
    DCD     0                             ; 18 Reserved
    DCD     0                             ; 19 Reserved
    DCD     0                             ; 20 Reserved
    DCD     0                             ; 21 Reserved
    DCD     0                             ; 22 Reserved
    DCD     0                             ; 23 Reserved
    DCD     0                             ; 24 Reserved
    DCD     0                             ; 25 Reserved
    DCD     0                             ; 26 Reserved
    DCD     0                             ; 27 Reserved
    DCD     CPU0_CTI_Handler              ; 28 CPU0 CTI Handler
    DCD     CPU1_CTI_Handler              ; 29 CPU1 CTI Handler
    DCD     0                             ; 30 Reserved
    DCD     0                             ; 31 Reserved
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
    DCD     UARTOVF_Handler           ; 47 UART Overflow Handler
    DCD     ETHERNET_Handler          ; 48 Ethernet Handler
    DCD     I2S_Handler               ; 49 I2S Handler
    DCD     TSC_Handler               ; 50 Touch Screen Handler
    DCD     0                         ; 51 Reserved
    DCD     SPI0_Handler              ; 52 SPI ADC Handler
    DCD     SPI1_Handler              ; 53 SPI (Shield 0) Handler
    DCD     SPI2_Handler              ; 54 SPI (Shield 1) Handler
    DCD     0                         ; 55 Reserved
    DCD     0                         ; 56 Reserved
    DCD     0                         ; 57 Reserved
    DCD     0                         ; 58 Reserved
    DCD     0                         ; 59 Reserved
    DCD     0                         ; 60 Reserved
    DCD     0                         ; 61 Reserved
    DCD     0                         ; 62 Reserved
    DCD     0                         ; 63 Reserved
    DCD     0                         ; 64 Reserved
    DCD     0                         ; 65 Reserved
    DCD     0                         ; 66 Reserved
    DCD     0                         ; 67 Reserved
    DCD     GPIO0_Handler             ; 68 GPIO 0 Comboned Handler
    DCD     GPIO1_Handler             ; 69 GPIO 1 Comboned Handler
    DCD     GPIO2_Handler             ; 70 GPIO 2 Comboned Handler
    DCD     GPIO3_Handler             ; 71 GPIO 3 Comboned Handler
    DCD     GPIO0_0_Handler           ; 72 GPIO0_0 Handler
    DCD     GPIO0_1_Handler           ; 73 GPIO0_1 Handler
    DCD     GPIO0_2_Handler           ; 74 GPIO0_2 Handler
    DCD     GPIO0_3_Handler           ; 75 GPIO0_3 Handler
    DCD     GPIO0_4_Handler           ; 76 GPIO0_4 Handler
    DCD     GPIO0_5_Handler           ; 77 GPIO0_5 Handler
    DCD     GPIO0_6_Handler           ; 78 GPIO0_6 Handler
    DCD     GPIO0_7_Handler           ; 79 GPIO0_7 Handler
    DCD     GPIO0_8_Handler           ; 80 GPIO0_8 Handler
    DCD     GPIO0_9_Handler           ; 81 GPIO0_9 Handler
    DCD     GPIO0_10_Handler          ; 82 GPIO0_10 Handler
    DCD     GPIO0_11_Handler          ; 83 GPIO0_11 Handler
    DCD     GPIO0_12_Handler          ; 84 GPIO0_12 Handler
    DCD     GPIO0_13_Handler          ; 85 GPIO0_13 Handler
    DCD     GPIO0_14_Handler          ; 86 GPIO0_14 Handler
    DCD     GPIO0_15_Handler          ; 87 GPIO0_15 Handler
    DCD     GPIO1_0_Handler           ; 88 GPIO1_0 Handler
    DCD     GPIO1_1_Handler           ; 89 GPIO1_1 Handler
    DCD     GPIO1_2_Handler           ; 90 GPIO1_2 Handler
    DCD     GPIO1_3_Handler           ; 91 GPIO1_3 Handler
    DCD     GPIO1_4_Handler           ; 92 GPIO1_4 Handler
    DCD     GPIO1_5_Handler           ; 93 GPIO1_5 Handler
    DCD     GPIO1_6_Handler           ; 94 GPIO1_6 Handler
    DCD     GPIO1_7_Handler           ; 95 GPIO1_7 Handler
    DCD     GPIO1_8_Handler           ; 96 GPIO1_8 Handler
    DCD     GPIO1_9_Handler           ; 97 GPIO1_0 Handler
    DCD     GPIO1_10_Handler          ; 98 GPIO1_10 Handler
    DCD     GPIO1_11_Handler          ; 99 GPIO1_11 Handler
    DCD     GPIO1_12_Handler          ; 100 GPIO1_12 Handler
    DCD     GPIO1_13_Handler          ; 101 GPIO1_13 Handler
    DCD     GPIO1_14_Handler          ; 102 GPIO1_14 Handler
    DCD     GPIO1_15_Handler          ; 103 GPIO1_15 Handler
    DCD     GPIO2_0_Handler           ; 104 GPIO2_0 Handler
    DCD     GPIO2_1_Handler           ; 105 GPIO2_1 Handler
    DCD     GPIO2_2_Handler           ; 106 GPIO2_2 Handler
    DCD     GPIO2_3_Handler           ; 107 GPIO2_3 Handler
    DCD     GPIO2_4_Handler           ; 108 GPIO2_4 Handler
    DCD     GPIO2_5_Handler           ; 109 GPIO2_5 Handler
    DCD     GPIO2_6_Handler           ; 110 GPIO2_6 Handler
    DCD     GPIO2_7_Handler           ; 111 GPIO2_7 Handler
    DCD     GPIO2_8_Handler           ; 112 GPIO2_8 Handler
    DCD     GPIO2_9_Handler           ; 113 GPIO2_9 Handler
    DCD     GPIO2_10_Handler          ; 114 GPIO2_10 Handler
    DCD     GPIO2_11_Handler          ; 115 GPIO2_11 Handler
    DCD     GPIO2_12_Handler          ; 116 GPIO2_12 Handler
    DCD     GPIO2_13_Handler          ; 117 GPIO2_13 Handler
    DCD     GPIO2_14_Handler          ; 118 GPIO2_14 Handler
    DCD     GPIO2_15_Handler          ; 119 GPIO2_15 Handler
    DCD     GPIO3_0_Handler           ; 120 GPIO2_16 Handler
    DCD     GPIO3_1_Handler           ; 121 GPIO3_0 Handler
    DCD     GPIO3_2_Handler           ; 122 GPIO3_1 Handler
    DCD     GPIO3_3_Handler           ; 123 GPIO3_3 Handler
    DCD     UARTRX5_Handler           ; 124 UART 5 RX Handler
    DCD     UARTTX5_Handler           ; 125 UART 5 TX Handler
    DCD     UART5_Handler             ; 126 UART 5 combined Handler
    DCD     HDLCD_Handler             ; 127 HDCLCD interrupt
__Vectors_End

__Vectors_Size  EQU     __Vectors_End - __Vectors

    AREA    |.text|, CODE, READONLY


; Reset Handler

Reset_Handler \
    PROC
    EXPORT  Reset_Handler             [WEAK]
    IMPORT  __main
    MRS     R0, control    ; Get control value
    ORR     R0, R0, #2     ; Select switch to PSP
    MSR     control, R0
    LDR     R0, =__main
    BX      R0
    ENDP


; Dummy Exception Handlers (infinite loops which can be modified)

NMI_Handler \
    PROC
    EXPORT  NMI_Handler               [WEAK]
    B       .
    ENDP

HardFault_Handler \
    PROC
    EXPORT  HardFault_Handler         [WEAK]
    B       .
    ENDP

MemManage_Handler \
    PROC
    EXPORT  MemManage_Handler         [WEAK]
    B       .
    ENDP

BusFault_Handler \
    PROC
    EXPORT  BusFault_Handler          [WEAK]
    B       .
    ENDP

UsageFault_Handler \
    PROC
    EXPORT  UsageFault_Handler        [WEAK]
    B       .
    ENDP

SVC_Handler \
    PROC
    EXPORT  SVC_Handler               [WEAK]
    B       .
    ENDP

DebugMon_Handler \
    PROC
    EXPORT  DebugMon_Handler          [WEAK]
    B       .
    ENDP

PendSV_Handler  \
    PROC
    EXPORT  PendSV_Handler            [WEAK]
    B       .
    ENDP

SysTick_Handler \
    PROC
    EXPORT  SysTick_Handler           [WEAK]
    B       .
    ENDP

Default_Handler \
    PROC
    ; Core IoT Interrupts
    EXPORT NONSEC_WATCHDOG_RESET_Handler [WEAK] ; 0 Non-Secure Watchdog
                                                ;   Reset Handler
    EXPORT NONSEC_WATCHDOG_Handler       [WEAK] ; 1 Non-Secure Watchdog Handler
    EXPORT S32K_TIMER_Handler            [WEAK] ; 2 S32K Timer Handler
    EXPORT TIMER0_Handler                [WEAK] ; 3 TIMER 0 Handler
    EXPORT TIMER1_Handler                [WEAK] ; 4 TIMER 1 Handler
    EXPORT DUALTIMER_Handler             [WEAK] ; 5 Dual Timer Handler
    EXPORT MHU0_Handler                  [WEAK] ; 6 Message Handling Unit 0
    EXPORT MHU1_Handler                  [WEAK] ; 7 Message Handling Unit 1
    EXPORT INVALID_INSTR_CACHE_Handler   [WEAK] ; 13 CPU Instruction Cache
                                                ;    Invalidation Handler
    EXPORT SYS_PPU_Handler               [WEAK] ; 15 SYS PPU Handler
    EXPORT CPU0_PPU_Handler              [WEAK] ; 16 CPU0 PPU Handler
    EXPORT CPU1_PPU_Handler              [WEAK] ; 17 CPU1 PPU Handler
    EXPORT CPU0_DBG_PPU_Handler          [WEAK] ; 18 CPU0 DBG PPU_Handler
    EXPORT CPU1_DBG_PPU_Handler          [WEAK] ; 19 CPU1 DBG PPU_Handler
    EXPORT CRYPT_PPU_Handler             [WEAK] ; 20 CRYPT PPU Handler
    EXPORT CORDIO_PPU_Handler            [WEAK] ; 21 CORDIO PPU Handler
    EXPORT RAM0_PPU_Handler              [WEAK] ; 22 RAM0 PPU Handler
    EXPORT RAM1_PPU_Handler              [WEAK] ; 23 RAM1 PPU Handler
    EXPORT RAM2_PPU_Handler              [WEAK] ; 24 RAM2 PPU Handler
    EXPORT RAM3_PPU_Handler              [WEAK] ; 25 RAM3 PPU Handler
    EXPORT CPU0_CTI_Handler              [WEAK] ; 28 CPU0 CTI Handler
    EXPORT CPU1_CTI_Handler              [WEAK] ; 29 CPU1 CTI Handler
    ; External Interrupts
    EXPORT UARTRX0_Handler               [WEAK] ; 32 UART 0 RX Handler
    EXPORT UARTTX0_Handler               [WEAK] ; 33 UART 0 TX Handler
    EXPORT UARTRX1_Handler               [WEAK] ; 34 UART 1 RX Handler
    EXPORT UARTTX1_Handler               [WEAK] ; 35 UART 1 TX Handler
    EXPORT UARTRX2_Handler               [WEAK] ; 36 UART 2 RX Handler
    EXPORT UARTTX2_Handler               [WEAK] ; 37 UART 2 TX Handler
    EXPORT UARTRX3_Handler               [WEAK] ; 38 UART 3 RX Handler
    EXPORT UARTTX3_Handler               [WEAK] ; 39 UART 3 TX Handler
    EXPORT UARTRX4_Handler               [WEAK] ; 40 UART 4 RX Handler
    EXPORT UARTTX4_Handler               [WEAK] ; 41 UART 4 TX Handler
    EXPORT UART0_Handler                 [WEAK] ; 42 UART 0 combined Handler
    EXPORT UART1_Handler                 [WEAK] ; 43 UART 1 combined Handler
    EXPORT UART2_Handler                 [WEAK] ; 44 UART 2 combined Handler
    EXPORT UART3_Handler                 [WEAK] ; 45 UART 3 combined Handler
    EXPORT UART4_Handler                 [WEAK] ; 46 UART 4 combined Handler
    EXPORT UARTOVF_Handler               [WEAK] ; 47 UART Overflow Handler
    EXPORT ETHERNET_Handler              [WEAK] ; 48 Ethernet Handler
    EXPORT I2S_Handler                   [WEAK] ; 49 I2S Handler
    EXPORT TSC_Handler                   [WEAK] ; 50 Touch Screen Handler
    EXPORT SPI0_Handler                  [WEAK] ; 52 SPI ADC Handler
    EXPORT SPI1_Handler                  [WEAK] ; 53 SPI (Shield 0) Handler
    EXPORT SPI2_Handler                  [WEAK] ; 54 SPI (Shield 1) Handler
    EXPORT GPIO0_Handler                 [WEAK] ; 68 GPIO 0 Comboned Handler
    EXPORT GPIO1_Handler                 [WEAK] ; 69 GPIO 1 Comboned Handler
    EXPORT GPIO2_Handler                 [WEAK] ; 70 GPIO 2 Comboned Handler
    EXPORT GPIO3_Handler                 [WEAK] ; 71 GPIO 3 Comboned Handler
    EXPORT GPIO0_0_Handler               [WEAK] ; 72 GPIO0_0 Handlers
    EXPORT GPIO0_1_Handler               [WEAK] ; 73 GPIO0_1 Handler
    EXPORT GPIO0_2_Handler               [WEAK] ; 74 GPIO0_2 Handler
    EXPORT GPIO0_3_Handler               [WEAK] ; 75 GPIO0_3 Handler
    EXPORT GPIO0_4_Handler               [WEAK] ; 76 GPIO0_4 Handler
    EXPORT GPIO0_5_Handler               [WEAK] ; 77 GPIO0_5 Handler
    EXPORT GPIO0_6_Handler               [WEAK] ; 78 GPIO0_6 Handler
    EXPORT GPIO0_7_Handler               [WEAK] ; 79 GPIO0_7 Handler
    EXPORT GPIO0_8_Handler               [WEAK] ; 80 GPIO0_8 Handler
    EXPORT GPIO0_9_Handler               [WEAK] ; 81 GPIO0_9 Handler
    EXPORT GPIO0_10_Handler              [WEAK] ; 82 GPIO0_10 Handler
    EXPORT GPIO0_11_Handler              [WEAK] ; 83 GPIO0_11 Handler
    EXPORT GPIO0_12_Handler              [WEAK] ; 84 GPIO0_12 Handler
    EXPORT GPIO0_13_Handler              [WEAK] ; 85 GPIO0_13 Handler
    EXPORT GPIO0_14_Handler              [WEAK] ; 86 GPIO0_14 Handler
    EXPORT GPIO0_15_Handler              [WEAK] ; 87 GPIO0_15 Handler
    EXPORT GPIO1_0_Handler               [WEAK] ; 88 GPIO1_0 Handler
    EXPORT GPIO1_1_Handler               [WEAK] ; 89 GPIO1_1 Handler
    EXPORT GPIO1_2_Handler               [WEAK] ; 90 GPIO1_2 Handler
    EXPORT GPIO1_3_Handler               [WEAK] ; 91 GPIO1_3 Handler
    EXPORT GPIO1_4_Handler               [WEAK] ; 92 GPIO1_4 Handler
    EXPORT GPIO1_5_Handler               [WEAK] ; 93 GPIO1_5 Handler
    EXPORT GPIO1_6_Handler               [WEAK] ; 94 GPIO1_6 Handler
    EXPORT GPIO1_7_Handler               [WEAK] ; 95 GPIO1_7 Handler
    EXPORT GPIO1_8_Handler               [WEAK] ; 96 GPIO1_8 Handler
    EXPORT GPIO1_9_Handler               [WEAK] ; 97 GPIO1_9 Handler
    EXPORT GPIO1_10_Handler              [WEAK] ; 98 GPIO1_10 Handler
    EXPORT GPIO1_11_Handler              [WEAK] ; 99 GPIO1_11 Handler
    EXPORT GPIO1_12_Handler              [WEAK] ; 100 GPIO1_12 Handler
    EXPORT GPIO1_13_Handler              [WEAK] ; 101 GPIO1_13 Handler
    EXPORT GPIO1_14_Handler              [WEAK] ; 102 GPIO1_14 Handler
    EXPORT GPIO1_15_Handler              [WEAK] ; 103 GPIO1_15 Handler
    EXPORT GPIO2_0_Handler               [WEAK] ; 104 GPIO2_0 Handler
    EXPORT GPIO2_1_Handler               [WEAK] ; 105 GPIO2_1 Handler
    EXPORT GPIO2_2_Handler               [WEAK] ; 106 GPIO2_2 Handler
    EXPORT GPIO2_3_Handler               [WEAK] ; 107 GPIO2_3 Handler
    EXPORT GPIO2_4_Handler               [WEAK] ; 108 GPIO2_4 Handler
    EXPORT GPIO2_5_Handler               [WEAK] ; 109 GPIO2_5 Handler
    EXPORT GPIO2_6_Handler               [WEAK] ; 110 GPIO2_6 Handler
    EXPORT GPIO2_7_Handler               [WEAK] ; 111 GPIO2_7 Handler
    EXPORT GPIO2_8_Handler               [WEAK] ; 112 GPIO2_8 Handler
    EXPORT GPIO2_9_Handler               [WEAK] ; 113 GPIO2_9 Handler
    EXPORT GPIO2_10_Handler              [WEAK] ; 114 GPIO2_10 Handler
    EXPORT GPIO2_11_Handler              [WEAK] ; 115 GPIO2_11 Handler
    EXPORT GPIO2_12_Handler              [WEAK] ; 116 GPIO2_12 Handler
    EXPORT GPIO2_13_Handler              [WEAK] ; 117 GPIO2_13 Handler
    EXPORT GPIO2_14_Handler              [WEAK] ; 118 GPIO2_14 Handler
    EXPORT GPIO2_15_Handler              [WEAK] ; 119 GPIO2_15 Handler
    EXPORT GPIO3_0_Handler               [WEAK] ; 120 GPIO3_0 Handler
    EXPORT GPIO3_1_Handler               [WEAK] ; 121 GPIO3_1 Handler
    EXPORT GPIO3_2_Handler               [WEAK] ; 122 GPIO3_2 Handler
    EXPORT GPIO3_3_Handler               [WEAK] ; 123 GPIO3_3 Handler
    EXPORT UARTRX5_Handler               [WEAK] ; 124 UART 5 RX Handler
    EXPORT UARTTX5_Handler               [WEAK] ; 125 UART 5 TX Handler
    EXPORT UART5_Handler                 [WEAK] ; 126 UART 5 combined Handler
    EXPORT HDLCD_Handler                 [WEAK] ; 127 HDCLCD interrupt

; Core IoT Interrupts
NONSEC_WATCHDOG_RESET_Handler  ; 0 Non-Secure Watchdog Reset Handler
NONSEC_WATCHDOG_Handler        ; 1 Non-Secure Watchdog Handler
S32K_TIMER_Handler             ; 2 S32K Timer Handler
TIMER0_Handler                 ; 3 TIMER 0 Handler
TIMER1_Handler                 ; 4 TIMER 1 Handler
DUALTIMER_Handler              ; 5 Dual Timer Handler
MHU0_Handler                   ; 6 Message Handling Unit 0
MHU1_Handler                   ; 7 Message Handling Unit 1
INVALID_INSTR_CACHE_Handler    ; 13 CPU Instruction Cache Invalidation Handler
SYS_PPU_Handler                ; 15 SYS PPU Handler
CPU0_PPU_Handler               ; 16 CPU0 PPU Handler
CPU1_PPU_Handler               ; 17 CPU1 PPU Handler
CPU0_DBG_PPU_Handler           ; 18 CPU0 DBG PPU_Handler
CPU1_DBG_PPU_Handler           ; 19 CPU1 DBG PPU_Handler
CRYPT_PPU_Handler              ; 20 CRYPT PPU Handler
CORDIO_PPU_Handler             ; 21 CORDIO PPU Handler
RAM0_PPU_Handler               ; 22 RAM0 PPU Handler
RAM1_PPU_Handler               ; 23 RAM1 PPU Handler
RAM2_PPU_Handler               ; 24 RAM2 PPU Handler
RAM3_PPU_Handler               ; 25 RAM3 PPU Handler
CPU0_CTI_Handler               ; 28 CPU0 CTI Handler
CPU1_CTI_Handler               ; 29 CPU1 CTI Handler
; External Interrupts
UARTRX0_Handler           ; 32 UART 0 RX Handler
UARTTX0_Handler           ; 33 UART 0 TX Handler
UARTRX1_Handler           ; 34 UART 1 RX Handler
UARTTX1_Handler           ; 35 UART 1 TX Handler
UARTRX2_Handler           ; 36 UART 2 RX Handler
UARTTX2_Handler           ; 37 UART 2 TX Handler
UARTRX3_Handler           ; 38 UART 3 RX Handler
UARTTX3_Handler           ; 39 UART 3 TX Handler
UARTRX4_Handler           ; 40 UART 4 RX Handler
UARTTX4_Handler           ; 41 UART 4 TX Handler
UART0_Handler             ; 42 UART 0 combined Handler
UART1_Handler             ; 43 UART 1 combined Handler
UART2_Handler             ; 44 UART 2 combined Handler
UART3_Handler             ; 45 UART 3 combined Handler
UART4_Handler             ; 46 UART 4 combined Handler
UARTOVF_Handler           ; 47 UART Overflow Handler
ETHERNET_Handler          ; 48 Ethernet Handler
I2S_Handler               ; 49 I2S Handler
TSC_Handler               ; 50 Touch Screen Handler
SPI0_Handler              ; 52 SPI ADC Handler
SPI1_Handler              ; 53 SPI (Shield 0) Handler
SPI2_Handler              ; 54 SPI (Shield 1) Handler
GPIO0_Handler             ; 68 GPIO 0 Comboned Handler
GPIO1_Handler             ; 69 GPIO 1 Comboned Handler
GPIO2_Handler             ; 70 GPIO 2 Comboned Handler
GPIO3_Handler             ; 71 GPIO 3 Comboned Handler
GPIO0_0_Handler           ; 72 GPIO0_0 Handler
GPIO0_1_Handler           ; 73 GPIO0_1 Handler
GPIO0_2_Handler           ; 74 GPIO0_2 Handler
GPIO0_3_Handler           ; 75 GPIO0_3 Handler
GPIO0_4_Handler           ; 76 GPIO0_4 Handler
GPIO0_5_Handler           ; 77 GPIO0_5 Handler
GPIO0_6_Handler           ; 78 GPIO0_6 Handler
GPIO0_7_Handler           ; 79 GPIO0_7 Handler
GPIO0_8_Handler           ; 80 GPIO0_8 Handler
GPIO0_9_Handler           ; 81 GPIO0_9 Handler
GPIO0_10_Handler          ; 82 GPIO0_10 Handler
GPIO0_11_Handler          ; 83 GPIO0_11 Handler
GPIO0_12_Handler          ; 84 GPIO0_12 Handler
GPIO0_13_Handler          ; 85 GPIO0_13 Handler
GPIO0_14_Handler          ; 86 GPIO0_14 Handler
GPIO0_15_Handler          ; 87 GPIO0_15 Handler
GPIO1_0_Handler           ; 88 GPIO1_0 Handler
GPIO1_1_Handler           ; 89 GPIO1_1 Handler
GPIO1_2_Handler           ; 90 GPIO1_2 Handler
GPIO1_3_Handler           ; 91 GPIO1_3 Handler
GPIO1_4_Handler           ; 92 GPIO1_4 Handler
GPIO1_5_Handler           ; 93 GPIO1_5 Handler
GPIO1_6_Handler           ; 94 GPIO1_6 Handler
GPIO1_7_Handler           ; 95 GPIO1_7 Handler
GPIO1_8_Handler           ; 96 GPIO1_8 Handler
GPIO1_9_Handler           ; 97 GPIO1_9 Handler
GPIO1_10_Handler          ; 98 GPIO1_10 Handler
GPIO1_11_Handler          ; 99 GPIO1_11 Handler
GPIO1_12_Handler          ; 100 GPIO1_12 Handler
GPIO1_13_Handler          ; 101 GPIO1_13 Handler
GPIO1_14_Handler          ; 102 GPIO1_14 Handler
GPIO1_15_Handler          ; 103 GPIO1_15 Handler
GPIO2_0_Handler           ; 104 GPIO2_0 Handler
GPIO2_1_Handler           ; 105 GPIO2_1 Handler
GPIO2_2_Handler           ; 106 GPIO2_2 Handler
GPIO2_3_Handler           ; 107 GPIO2_3 Handler
GPIO2_4_Handler           ; 108 GPIO2_4 Handler
GPIO2_5_Handler           ; 109 GPIO2_5 Handler
GPIO2_6_Handler           ; 110 GPIO2_6 Handler
GPIO2_7_Handler           ; 111 GPIO2_7 Handler
GPIO2_8_Handler           ; 112 GPIO2_8 Handler
GPIO2_9_Handler           ; 113 GPIO2_9 Handler
GPIO2_10_Handler          ; 114 GPIO2_10 Handler
GPIO2_11_Handler          ; 115 GPIO2_11 Handler
GPIO2_12_Handler          ; 116 GPIO2_12 Handler
GPIO2_13_Handler          ; 117 GPIO2_13 Handler
GPIO2_14_Handler          ; 118 GPIO2_14 Handler
GPIO2_15_Handler          ; 119 GPIO2_15 Handler
GPIO3_0_Handler           ; 120 GPIO3_0 Handler
GPIO3_1_Handler           ; 121 GPIO2_1 Handler
GPIO3_2_Handler           ; 122 GPIO2_2 Handler
GPIO3_3_Handler           ; 123 GPIO2_3 Handler
UARTRX5_Handler           ; 124 UART 5 RX Handler
UARTTX5_Handler           ; 125 UART 5 TX Handler
UART5_Handler             ; 125 UART 5 combined Handler
HDLCD_Handler             ; 127 HDCLCD interrupt Handler
    B .

    ENDP

    ALIGN

    END
