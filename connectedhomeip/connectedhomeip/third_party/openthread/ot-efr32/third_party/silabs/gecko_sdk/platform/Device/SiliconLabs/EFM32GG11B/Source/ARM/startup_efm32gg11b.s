;/**************************************************************************//**
; * @file
; * @brief    CMSIS Core Device Startup File for
; *           Silicon Labs EFM32GG11B Device Series
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

                AREA     RESET, DATA, READONLY

                AREA     RESET, DATA, READONLY, ALIGN=9
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


                DCD      EMU_IRQHandler                      ; 0: EMU Interrupt
                DCD      WDOG0_IRQHandler                    ; 1: WDOG0 Interrupt
                DCD      LDMA_IRQHandler                     ; 2: LDMA Interrupt
                DCD      GPIO_EVEN_IRQHandler                ; 3: GPIO_EVEN Interrupt
                DCD      SMU_IRQHandler                      ; 4: SMU Interrupt
                DCD      TIMER0_IRQHandler                   ; 5: TIMER0 Interrupt
                DCD      USART0_RX_IRQHandler                ; 6: USART0_RX Interrupt
                DCD      USART0_TX_IRQHandler                ; 7: USART0_TX Interrupt
                DCD      ACMP0_IRQHandler                    ; 8: ACMP0 Interrupt
                DCD      ADC0_IRQHandler                     ; 9: ADC0 Interrupt
                DCD      IDAC0_IRQHandler                    ; 10: IDAC0 Interrupt
                DCD      I2C0_IRQHandler                     ; 11: I2C0 Interrupt
                DCD      I2C1_IRQHandler                     ; 12: I2C1 Interrupt
                DCD      GPIO_ODD_IRQHandler                 ; 13: GPIO_ODD Interrupt
                DCD      TIMER1_IRQHandler                   ; 14: TIMER1 Interrupt
                DCD      TIMER2_IRQHandler                   ; 15: TIMER2 Interrupt
                DCD      TIMER3_IRQHandler                   ; 16: TIMER3 Interrupt
                DCD      USART1_RX_IRQHandler                ; 17: USART1_RX Interrupt
                DCD      USART1_TX_IRQHandler                ; 18: USART1_TX Interrupt
                DCD      USART2_RX_IRQHandler                ; 19: USART2_RX Interrupt
                DCD      USART2_TX_IRQHandler                ; 20: USART2_TX Interrupt
                DCD      UART0_RX_IRQHandler                 ; 21: UART0_RX Interrupt
                DCD      UART0_TX_IRQHandler                 ; 22: UART0_TX Interrupt
                DCD      UART1_RX_IRQHandler                 ; 23: UART1_RX Interrupt
                DCD      UART1_TX_IRQHandler                 ; 24: UART1_TX Interrupt
                DCD      LEUART0_IRQHandler                  ; 25: LEUART0 Interrupt
                DCD      LEUART1_IRQHandler                  ; 26: LEUART1 Interrupt
                DCD      LETIMER0_IRQHandler                 ; 27: LETIMER0 Interrupt
                DCD      PCNT0_IRQHandler                    ; 28: PCNT0 Interrupt
                DCD      PCNT1_IRQHandler                    ; 29: PCNT1 Interrupt
                DCD      PCNT2_IRQHandler                    ; 30: PCNT2 Interrupt
                DCD      RTCC_IRQHandler                     ; 31: RTCC Interrupt
                DCD      CMU_IRQHandler                      ; 32: CMU Interrupt
                DCD      MSC_IRQHandler                      ; 33: MSC Interrupt
                DCD      CRYPTO0_IRQHandler                  ; 34: CRYPTO0 Interrupt
                DCD      CRYOTIMER_IRQHandler                ; 35: CRYOTIMER Interrupt
                DCD      FPUEH_IRQHandler                    ; 36: FPUEH Interrupt
                DCD      USART3_RX_IRQHandler                ; 37: USART3_RX Interrupt
                DCD      USART3_TX_IRQHandler                ; 38: USART3_TX Interrupt
                DCD      USART4_RX_IRQHandler                ; 39: USART4_RX Interrupt
                DCD      USART4_TX_IRQHandler                ; 40: USART4_TX Interrupt
                DCD      WTIMER0_IRQHandler                  ; 41: WTIMER0 Interrupt
                DCD      WTIMER1_IRQHandler                  ; 42: WTIMER1 Interrupt
                DCD      WTIMER2_IRQHandler                  ; 43: WTIMER2 Interrupt
                DCD      WTIMER3_IRQHandler                  ; 44: WTIMER3 Interrupt
                DCD      I2C2_IRQHandler                     ; 45: I2C2 Interrupt
                DCD      VDAC0_IRQHandler                    ; 46: VDAC0 Interrupt
                DCD      TIMER4_IRQHandler                   ; 47: TIMER4 Interrupt
                DCD      TIMER5_IRQHandler                   ; 48: TIMER5 Interrupt
                DCD      TIMER6_IRQHandler                   ; 49: TIMER6 Interrupt
                DCD      USART5_RX_IRQHandler                ; 50: USART5_RX Interrupt
                DCD      USART5_TX_IRQHandler                ; 51: USART5_TX Interrupt
                DCD      CSEN_IRQHandler                     ; 52: CSEN Interrupt
                DCD      LESENSE_IRQHandler                  ; 53: LESENSE Interrupt
                DCD      EBI_IRQHandler                      ; 54: EBI Interrupt
                DCD      ACMP2_IRQHandler                    ; 55: ACMP2 Interrupt
                DCD      ADC1_IRQHandler                     ; 56: ADC1 Interrupt
                DCD      LCD_IRQHandler                      ; 57: LCD Interrupt
                DCD      SDIO_IRQHandler                     ; 58: SDIO Interrupt
                DCD      ETH_IRQHandler                      ; 59: ETH Interrupt
                DCD      CAN0_IRQHandler                     ; 60: CAN0 Interrupt
                DCD      CAN1_IRQHandler                     ; 61: CAN1 Interrupt
                DCD      USB_IRQHandler                      ; 62: USB Interrupt
                DCD      RTC_IRQHandler                      ; 63: RTC Interrupt
                DCD      WDOG1_IRQHandler                    ; 64: WDOG1 Interrupt
                DCD      LETIMER1_IRQHandler                 ; 65: LETIMER1 Interrupt
                DCD      TRNG0_IRQHandler                    ; 66: TRNG0 Interrupt
                DCD      QSPI0_IRQHandler                    ; 67: QSPI0 Interrupt

                SPACE   (154 * 4)          ; Remaining Interrupts are left out

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


                Set_Default_Handler  EMU_IRQHandler
                Set_Default_Handler  WDOG0_IRQHandler
                Set_Default_Handler  LDMA_IRQHandler
                Set_Default_Handler  GPIO_EVEN_IRQHandler
                Set_Default_Handler  SMU_IRQHandler
                Set_Default_Handler  TIMER0_IRQHandler
                Set_Default_Handler  USART0_RX_IRQHandler
                Set_Default_Handler  USART0_TX_IRQHandler
                Set_Default_Handler  ACMP0_IRQHandler
                Set_Default_Handler  ADC0_IRQHandler
                Set_Default_Handler  IDAC0_IRQHandler
                Set_Default_Handler  I2C0_IRQHandler
                Set_Default_Handler  I2C1_IRQHandler
                Set_Default_Handler  GPIO_ODD_IRQHandler
                Set_Default_Handler  TIMER1_IRQHandler
                Set_Default_Handler  TIMER2_IRQHandler
                Set_Default_Handler  TIMER3_IRQHandler
                Set_Default_Handler  USART1_RX_IRQHandler
                Set_Default_Handler  USART1_TX_IRQHandler
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
                Set_Default_Handler  RTCC_IRQHandler
                Set_Default_Handler  CMU_IRQHandler
                Set_Default_Handler  MSC_IRQHandler
                Set_Default_Handler  CRYPTO0_IRQHandler
                Set_Default_Handler  CRYOTIMER_IRQHandler
                Set_Default_Handler  FPUEH_IRQHandler
                Set_Default_Handler  USART3_RX_IRQHandler
                Set_Default_Handler  USART3_TX_IRQHandler
                Set_Default_Handler  USART4_RX_IRQHandler
                Set_Default_Handler  USART4_TX_IRQHandler
                Set_Default_Handler  WTIMER0_IRQHandler
                Set_Default_Handler  WTIMER1_IRQHandler
                Set_Default_Handler  WTIMER2_IRQHandler
                Set_Default_Handler  WTIMER3_IRQHandler
                Set_Default_Handler  I2C2_IRQHandler
                Set_Default_Handler  VDAC0_IRQHandler
                Set_Default_Handler  TIMER4_IRQHandler
                Set_Default_Handler  TIMER5_IRQHandler
                Set_Default_Handler  TIMER6_IRQHandler
                Set_Default_Handler  USART5_RX_IRQHandler
                Set_Default_Handler  USART5_TX_IRQHandler
                Set_Default_Handler  CSEN_IRQHandler
                Set_Default_Handler  LESENSE_IRQHandler
                Set_Default_Handler  EBI_IRQHandler
                Set_Default_Handler  ACMP2_IRQHandler
                Set_Default_Handler  ADC1_IRQHandler
                Set_Default_Handler  LCD_IRQHandler
                Set_Default_Handler  SDIO_IRQHandler
                Set_Default_Handler  ETH_IRQHandler
                Set_Default_Handler  CAN0_IRQHandler
                Set_Default_Handler  CAN1_IRQHandler
                Set_Default_Handler  USB_IRQHandler
                Set_Default_Handler  RTC_IRQHandler
                Set_Default_Handler  WDOG1_IRQHandler
                Set_Default_Handler  LETIMER1_IRQHandler
                Set_Default_Handler  TRNG0_IRQHandler
                Set_Default_Handler  QSPI0_IRQHandler

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
