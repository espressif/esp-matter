;/**************************************************************************//**
; * @file
; * @brief    CMSIS Core Device Startup File for
; *           Silicon Labs EZR32WG Device Series
; ******************************************************************************
; * # License
; *
; * The licensor of this software is Silicon Laboratories Inc. Your use of this
; * software is governed by the terms of Silicon Labs Master Software License
; * Agreement (MSLA) available at
; * www.silabs.com/about-us/legal/master-software-license-agreement. This
; * software is Third Party Software licensed by Silicon Labs from a third party
; * and is governed by the sections of the MSLA applicable to Third Party
; * Software and the additional terms set forth below.
; *
; *****************************************************************************/
;/*
; ****************************************************************************/
;/*
; * Copyright (c) 2009-2021 Arm Limited. All rights reserved.
; *
; * SPDX-License-Identifier: Apache-2.0
; *
; * Licensed under the Apache License, Version 2.0 (the License); you may
; * not use this file except in compliance with the License.
; * You may obtain a copy of the License at
; *
; * www.apache.org/licenses/LICENSE-2.0
; *
; * Unless required by applicable law or agreed to in writing, software
; * distributed under the License is distributed on an AS IS BASIS, WITHOUT
; * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
; * See the License for the specific language governing permissions and
; * limitations under the License.
; */


                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset

                AREA    RESET, DATA, READONLY, ALIGN=8
                EXPORT   __Vectors
                EXPORT   __Vectors_End
                EXPORT   __Vectors_Size

__Vectors       DCD      __initial_sp                        ;     Top of Stack
                DCD      Reset_Handler                       ; Reset Handler
                DCD      NMI_Handler                         ; -14 NMI Handler
                DCD      HardFault_Handler                   ; -13 Hard Fault Handler
                DCD      MemManage_Handler                   ; -12 MPU Fault Handler
                DCD      BusFault_Handler                    ; -11 Bus Fault Handler
                DCD      UsageFault_Handler                  ; -10 Usage Fault Handler
                DCD      0                                   ; Reserved
                DCD      0                                   ; Reserved
                DCD      0                                   ; Reserved
                DCD      0                                   ; Reserved
                DCD      SVC_Handler                         ; -5 SVCall Handler
                DCD      DebugMon_Handler                    ; -4 Debug Monitor Handler
                DCD      sl_app_properties                   ; Application properties
                DCD      PendSV_Handler                      ; PendSV Handler
                DCD      SysTick_Handler                     ; SysTick Handler

                ; External Interrupts

                DCD      DMA_IRQHandler                      ; 0: DMA Interrupt
                DCD      GPIO_EVEN_IRQHandler                ; 1: GPIO_EVEN Interrupt
                DCD      TIMER0_IRQHandler                   ; 2: TIMER0 Interrupt
                DCD      USARTRF0_RX_IRQHandler              ; 3: USARTRF0_RX Interrupt
                DCD      USARTRF0_TX_IRQHandler              ; 4: USARTRF0_TX Interrupt
                DCD      USB_IRQHandler                      ; 5: USB Interrupt
                DCD      ACMP0_IRQHandler                    ; 6: ACMP0 Interrupt
                DCD      ADC0_IRQHandler                     ; 7: ADC0 Interrupt
                DCD      DAC0_IRQHandler                     ; 8: DAC0 Interrupt
                DCD      I2C0_IRQHandler                     ; 9: I2C0 Interrupt
                DCD      I2C1_IRQHandler                     ; 10: I2C1 Interrupt
                DCD      GPIO_ODD_IRQHandler                 ; 11: GPIO_ODD Interrupt
                DCD      TIMER1_IRQHandler                   ; 12: TIMER1 Interrupt
                DCD      TIMER2_IRQHandler                   ; 13: TIMER2 Interrupt
                DCD      TIMER3_IRQHandler                   ; 14: TIMER3 Interrupt
                DCD      USART1_RX_IRQHandler                ; 15: USART1_RX Interrupt
                DCD      USART1_TX_IRQHandler                ; 16: USART1_TX Interrupt
                DCD      LESENSE_IRQHandler                  ; 17: LESENSE Interrupt
                DCD      USART2_RX_IRQHandler                ; 18: USART2_RX Interrupt
                DCD      USART2_TX_IRQHandler                ; 19: USART2_TX Interrupt
                DCD      UART0_RX_IRQHandler                 ; 20: UART0_RX Interrupt
                DCD      UART0_TX_IRQHandler                 ; 21: UART0_TX Interrupt
                DCD      UART1_RX_IRQHandler                 ; 22: UART1_RX Interrupt
                DCD      UART1_TX_IRQHandler                 ; 23: UART1_TX Interrupt
                DCD      LEUART0_IRQHandler                  ; 24: LEUART0 Interrupt
                DCD      LEUART1_IRQHandler                  ; 25: LEUART1 Interrupt
                DCD      LETIMER0_IRQHandler                 ; 26: LETIMER0 Interrupt
                DCD      PCNT0_IRQHandler                    ; 27: PCNT0 Interrupt
                DCD      PCNT1_IRQHandler                    ; 28: PCNT1 Interrupt
                DCD      PCNT2_IRQHandler                    ; 29: PCNT2 Interrupt
                DCD      RTC_IRQHandler                      ; 30: RTC Interrupt
                DCD      BURTC_IRQHandler                    ; 31: BURTC Interrupt
                DCD      CMU_IRQHandler                      ; 32: CMU Interrupt
                DCD      VCMP_IRQHandler                     ; 33: VCMP Interrupt
                DCD      0                                   ; 34: Reserved
                DCD      MSC_IRQHandler                      ; 35: MSC Interrupt
                DCD      AES_IRQHandler                      ; 36: AES Interrupt
                DCD      0                                   ; 37: Reserved
                DCD      EMU_IRQHandler                      ; 38: EMU Interrupt
                DCD      FPUEH_IRQHandler                    ; 39: FPUEH Interrupt

                SPACE   (184 * 4)          ; Remaining Interrupts are left out

__Vectors_End
__Vectors_Size  EQU      __Vectors_End - __Vectors


                AREA     |.text|, CODE, READONLY


; Reset Handler

Reset_Handler   PROC
                EXPORT  Reset_Handler             [WEAK]
                IMPORT   SystemInit
                IMPORT   __main

                LDR      R0, =SystemInit
                BLX      R0
                LDR      R0, =__main
                BX       R0
                ENDP

; The default macro is not used for HardFault_Handler
; because this results in a poor debug illusion.
HardFault_Handler PROC
                EXPORT  HardFault_Handler         [WEAK]
                B        .
                ENDP

; Macro to define default exception/interrupt handlers.
; Default handler are weak symbols with an endless loop.
; They can be overwritten by real handlers.
                MACRO
                Set_Default_Handler  $Handler_Name
$Handler_Name   PROC
                EXPORT  $Handler_Name             [WEAK]
                B        .
                ENDP
                MEND


; Default exception/interrupt handler

                Set_Default_Handler  NMI_Handler
                Set_Default_Handler  MemManage_Handler
                Set_Default_Handler  BusFault_Handler
                Set_Default_Handler  UsageFault_Handler
                Set_Default_Handler  SVC_Handler
                Set_Default_Handler  DebugMon_Handler
                Set_Default_Handler  PendSV_Handler
                Set_Default_Handler  SysTick_Handler

                Set_Default_Handler  DMA_IRQHandler
                Set_Default_Handler  GPIO_EVEN_IRQHandler
                Set_Default_Handler  TIMER0_IRQHandler
                Set_Default_Handler  USARTRF0_RX_IRQHandler
                Set_Default_Handler  USARTRF0_TX_IRQHandler
                Set_Default_Handler  USB_IRQHandler
                Set_Default_Handler  ACMP0_IRQHandler
                Set_Default_Handler  ADC0_IRQHandler
                Set_Default_Handler  DAC0_IRQHandler
                Set_Default_Handler  I2C0_IRQHandler
                Set_Default_Handler  I2C1_IRQHandler
                Set_Default_Handler  GPIO_ODD_IRQHandler
                Set_Default_Handler  TIMER1_IRQHandler
                Set_Default_Handler  TIMER2_IRQHandler
                Set_Default_Handler  TIMER3_IRQHandler
                Set_Default_Handler  USART1_RX_IRQHandler
                Set_Default_Handler  USART1_TX_IRQHandler
                Set_Default_Handler  LESENSE_IRQHandler
                Set_Default_Handler  USART2_RX_IRQHandler
                Set_Default_Handler  USART2_TX_IRQHandler
                Set_Default_Handler  UART0_RX_IRQHandler
                Set_Default_Handler  UART0_TX_IRQHandler
                Set_Default_Handler  UART1_RX_IRQHandler
                Set_Default_Handler  UART1_TX_IRQHandler
                Set_Default_Handler  LEUART0_IRQHandler
                Set_Default_Handler  LEUART1_IRQHandler
                Set_Default_Handler  LETIMER0_IRQHandler
                Set_Default_Handler  PCNT0_IRQHandler
                Set_Default_Handler  PCNT1_IRQHandler
                Set_Default_Handler  PCNT2_IRQHandler
                Set_Default_Handler  RTC_IRQHandler
                Set_Default_Handler  BURTC_IRQHandler
                Set_Default_Handler  CMU_IRQHandler
                Set_Default_Handler  VCMP_IRQHandler
                Set_Default_Handler  MSC_IRQHandler
                Set_Default_Handler  AES_IRQHandler
                Set_Default_Handler  EMU_IRQHandler
                Set_Default_Handler  FPUEH_IRQHandler

                ALIGN


; User setup Stack & Heap

                IF       :LNOT::DEF:__MICROLIB
                IMPORT   __use_two_region_memory
                ENDIF

                EXPORT   __stack_limit
                EXPORT   __initial_sp
                IF       Heap_Size != 0                      ; Heap is provided
                EXPORT   __heap_base
                EXPORT   __heap_limit
                ENDIF

                END
