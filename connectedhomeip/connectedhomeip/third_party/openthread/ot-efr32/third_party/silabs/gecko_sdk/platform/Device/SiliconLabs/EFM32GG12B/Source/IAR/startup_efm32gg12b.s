;/**************************************************************************//**
; * @file
; * @brief    CMSIS Core Device Startup File
; *           Silicon Labs EFM32GG12B Device Series
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
; * Copyright (c) 2009-2016 ARM Limited. All rights reserved.
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

;
; The modules in this file are included in the libraries, and may be replaced
; by any user-defined modules that define the PUBLIC symbol _program_start or
; a user defined start symbol.
; To override the cstartup defined in the library, simply add your modified
; version to the workbench project.
;
; The vector table is normally located at address 0.
; When debugging in RAM, it can be located in RAM, aligned to at least 2^6.
; The name "__vector_table" has special meaning for C-SPY:
; it is where the SP start value is found, and the NVIC vector
; table register (VTOR) is initialized to this address if != 0.
;
; Cortex-M version
;


                MODULE   ?cstartup

                ;; Forward declaration of sections.
                SECTION  CSTACK:DATA:NOROOT(3)


                SECTION  .intvec:CODE:NOROOT(9)
                EXTERN   __iar_program_start
                EXTERN   SystemInit
                PUBLIC   __vector_table
                PUBLIC   __vector_table_0x1c
                PUBLIC   __Vectors
                PUBLIC   __Vectors_End
                PUBLIC   __Vectors_Size

                DATA

__vector_table
                DCD      sfe(CSTACK)                         ; Top of Stack
                DCD      Reset_Handler                       ; Reset Handler
                DCD      NMI_Handler                         ; -14 NMI Handler
                DCD      HardFault_Handler                   ; -13 Hard Fault Handler
                DCD      MemManage_Handler                   ; -12 MPU Fault Handler
                DCD      BusFault_Handler                    ; -11 Bus Fault Handler
                DCD      UsageFault_Handler                  ; -10 Usage Fault Handler
__vector_table_0x1c
                DCD      0                                   ; Reserved
                DCD      0                                   ; Reserved
                DCD      0                                   ; Reserved
                DCD      0                                   ; Reserved
                DCD      SVC_Handler                         ; -5 SVCall Handler
                DCD      DebugMon_Handler                    ; -4 Debug Monitor Handler
                DCD      sl_app_properties                   ; -3 Application properties
                DCD      PendSV_Handler                      ; -2 PendSV Handler
                DCD      SysTick_Handler                     ; -1 SysTick Handler

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
                DCD      VDAC0_IRQHandler                    ; 43: VDAC0 Interrupt
                DCD      CSEN_IRQHandler                     ; 44: CSEN Interrupt
                DCD      LESENSE_IRQHandler                  ; 45: LESENSE Interrupt
                DCD      EBI_IRQHandler                      ; 46: EBI Interrupt
                DCD      ACMP2_IRQHandler                    ; 47: ACMP2 Interrupt
                DCD      ADC1_IRQHandler                     ; 48: ADC1 Interrupt
                DCD      LCD_IRQHandler                      ; 49: LCD Interrupt
                DCD      SDIO_IRQHandler                     ; 50: SDIO Interrupt
                DCD      CAN0_IRQHandler                     ; 51: CAN0 Interrupt
                DCD      CAN1_IRQHandler                     ; 52: CAN1 Interrupt
                DCD      USB_IRQHandler                      ; 53: USB Interrupt
                DCD      RTC_IRQHandler                      ; 54: RTC Interrupt
                DCD      WDOG1_IRQHandler                    ; 55: WDOG1 Interrupt
                DCD      LETIMER1_IRQHandler                 ; 56: LETIMER1 Interrupt
                DCD      TRNG0_IRQHandler                    ; 57: TRNG0 Interrupt
                DCD      QSPI0_IRQHandler                    ; 58: QSPI0 Interrupt
                DCD      PDM_IRQHandler                      ; 59: PDM Interrupt

                DS32    (164)                ; Remaining Interrupts are left out
__Vectors_End

__Vectors       EQU      __vector_table
__Vectors_Size  EQU      __Vectors_End - __Vectors

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Default interrupt handlers.
;;
                THUMB

; Reset Handler

                PUBWEAK  Reset_Handler
                SECTION  .text:CODE:REORDER:NOROOT(2)
Reset_Handler
                LDR      R0, =SystemInit
                BLX      R0
                LDR      R0, =__iar_program_start
                BX       R0


                PUBWEAK NMI_Handler
                PUBWEAK sl_app_properties
                PUBWEAK HardFault_Handler
                PUBWEAK MemManage_Handler
                PUBWEAK BusFault_Handler
                PUBWEAK UsageFault_Handler
                PUBWEAK SVC_Handler
                PUBWEAK DebugMon_Handler
                PUBWEAK PendSV_Handler
                PUBWEAK SysTick_Handler


                PUBWEAK EMU_IRQHandler
                PUBWEAK WDOG0_IRQHandler
                PUBWEAK LDMA_IRQHandler
                PUBWEAK GPIO_EVEN_IRQHandler
                PUBWEAK SMU_IRQHandler
                PUBWEAK TIMER0_IRQHandler
                PUBWEAK USART0_RX_IRQHandler
                PUBWEAK USART0_TX_IRQHandler
                PUBWEAK ACMP0_IRQHandler
                PUBWEAK ADC0_IRQHandler
                PUBWEAK IDAC0_IRQHandler
                PUBWEAK I2C0_IRQHandler
                PUBWEAK I2C1_IRQHandler
                PUBWEAK GPIO_ODD_IRQHandler
                PUBWEAK TIMER1_IRQHandler
                PUBWEAK TIMER2_IRQHandler
                PUBWEAK TIMER3_IRQHandler
                PUBWEAK USART1_RX_IRQHandler
                PUBWEAK USART1_TX_IRQHandler
                PUBWEAK USART2_RX_IRQHandler
                PUBWEAK USART2_TX_IRQHandler
                PUBWEAK UART0_RX_IRQHandler
                PUBWEAK UART0_TX_IRQHandler
                PUBWEAK UART1_RX_IRQHandler
                PUBWEAK UART1_TX_IRQHandler
                PUBWEAK LEUART0_IRQHandler
                PUBWEAK LEUART1_IRQHandler
                PUBWEAK LETIMER0_IRQHandler
                PUBWEAK PCNT0_IRQHandler
                PUBWEAK PCNT1_IRQHandler
                PUBWEAK PCNT2_IRQHandler
                PUBWEAK RTCC_IRQHandler
                PUBWEAK CMU_IRQHandler
                PUBWEAK MSC_IRQHandler
                PUBWEAK CRYPTO0_IRQHandler
                PUBWEAK CRYOTIMER_IRQHandler
                PUBWEAK FPUEH_IRQHandler
                PUBWEAK USART3_RX_IRQHandler
                PUBWEAK USART3_TX_IRQHandler
                PUBWEAK USART4_RX_IRQHandler
                PUBWEAK USART4_TX_IRQHandler
                PUBWEAK WTIMER0_IRQHandler
                PUBWEAK WTIMER1_IRQHandler
                PUBWEAK VDAC0_IRQHandler
                PUBWEAK CSEN_IRQHandler
                PUBWEAK LESENSE_IRQHandler
                PUBWEAK EBI_IRQHandler
                PUBWEAK ACMP2_IRQHandler
                PUBWEAK ADC1_IRQHandler
                PUBWEAK LCD_IRQHandler
                PUBWEAK SDIO_IRQHandler
                PUBWEAK CAN0_IRQHandler
                PUBWEAK CAN1_IRQHandler
                PUBWEAK USB_IRQHandler
                PUBWEAK RTC_IRQHandler
                PUBWEAK WDOG1_IRQHandler
                PUBWEAK LETIMER1_IRQHandler
                PUBWEAK TRNG0_IRQHandler
                PUBWEAK QSPI0_IRQHandler
                PUBWEAK PDM_IRQHandler
                SECTION .text:CODE:REORDER:NOROOT(1)
NMI_Handler
sl_app_properties     ; Provide a dummy value for the sl_app_properties symbol.
HardFault_Handler
MemManage_Handler
BusFault_Handler
UsageFault_Handler
SVC_Handler
DebugMon_Handler
PendSV_Handler
SysTick_Handler


EMU_IRQHandler
WDOG0_IRQHandler
LDMA_IRQHandler
GPIO_EVEN_IRQHandler
SMU_IRQHandler
TIMER0_IRQHandler
USART0_RX_IRQHandler
USART0_TX_IRQHandler
ACMP0_IRQHandler
ADC0_IRQHandler
IDAC0_IRQHandler
I2C0_IRQHandler
I2C1_IRQHandler
GPIO_ODD_IRQHandler
TIMER1_IRQHandler
TIMER2_IRQHandler
TIMER3_IRQHandler
USART1_RX_IRQHandler
USART1_TX_IRQHandler
USART2_RX_IRQHandler
USART2_TX_IRQHandler
UART0_RX_IRQHandler
UART0_TX_IRQHandler
UART1_RX_IRQHandler
UART1_TX_IRQHandler
LEUART0_IRQHandler
LEUART1_IRQHandler
LETIMER0_IRQHandler
PCNT0_IRQHandler
PCNT1_IRQHandler
PCNT2_IRQHandler
RTCC_IRQHandler
CMU_IRQHandler
MSC_IRQHandler
CRYPTO0_IRQHandler
CRYOTIMER_IRQHandler
FPUEH_IRQHandler
USART3_RX_IRQHandler
USART3_TX_IRQHandler
USART4_RX_IRQHandler
USART4_TX_IRQHandler
WTIMER0_IRQHandler
WTIMER1_IRQHandler
VDAC0_IRQHandler
CSEN_IRQHandler
LESENSE_IRQHandler
EBI_IRQHandler
ACMP2_IRQHandler
ADC1_IRQHandler
LCD_IRQHandler
SDIO_IRQHandler
CAN0_IRQHandler
CAN1_IRQHandler
USB_IRQHandler
RTC_IRQHandler
WDOG1_IRQHandler
LETIMER1_IRQHandler
TRNG0_IRQHandler
QSPI0_IRQHandler
PDM_IRQHandler
Default_Handler
                B        .

                END
