;/**************************************************************************//**
; * @file
; * @brief    CMSIS Core Device Startup File
; *           Silicon Labs EFM32TG11B Device Series
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


                SECTION  .intvec:CODE:NOROOT(8)
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
                DCD      0                                   ; Reserved
                DCD      0                                   ; Reserved
                DCD      0                                   ; Reserved
__vector_table_0x1c
                DCD      0                                   ; Reserved
                DCD      0                                   ; Reserved
                DCD      0                                   ; Reserved
                DCD      0                                   ; Reserved
                DCD      SVC_Handler                         ; -5 SVCall Handler
                DCD      0                                   ; Reserved
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
                DCD      USART0_IRQHandler                   ; 6: USART0 Interrupt
                DCD      ACMP0_IRQHandler                    ; 7: ACMP0 Interrupt
                DCD      ADC0_IRQHandler                     ; 8: ADC0 Interrupt
                DCD      I2C0_IRQHandler                     ; 9: I2C0 Interrupt
                DCD      I2C1_IRQHandler                     ; 10: I2C1 Interrupt
                DCD      GPIO_ODD_IRQHandler                 ; 11: GPIO_ODD Interrupt
                DCD      TIMER1_IRQHandler                   ; 12: TIMER1 Interrupt
                DCD      USART1_IRQHandler                   ; 13: USART1 Interrupt
                DCD      USART2_IRQHandler                   ; 14: USART2 Interrupt
                DCD      UART0_IRQHandler                    ; 15: UART0 Interrupt
                DCD      LEUART0_IRQHandler                  ; 16: LEUART0 Interrupt
                DCD      LETIMER0_IRQHandler                 ; 17: LETIMER0 Interrupt
                DCD      PCNT0_IRQHandler                    ; 18: PCNT0 Interrupt
                DCD      RTCC_IRQHandler                     ; 19: RTCC Interrupt
                DCD      CMU_IRQHandler                      ; 20: CMU Interrupt
                DCD      MSC_IRQHandler                      ; 21: MSC Interrupt
                DCD      CRYPTO0_IRQHandler                  ; 22: CRYPTO0 Interrupt
                DCD      CRYOTIMER_IRQHandler                ; 23: CRYOTIMER Interrupt
                DCD      USART3_IRQHandler                   ; 24: USART3 Interrupt
                DCD      WTIMER0_IRQHandler                  ; 25: WTIMER0 Interrupt
                DCD      WTIMER1_IRQHandler                  ; 26: WTIMER1 Interrupt
                DCD      VDAC0_IRQHandler                    ; 27: VDAC0 Interrupt
                DCD      CSEN_IRQHandler                     ; 28: CSEN Interrupt
                DCD      LESENSE_IRQHandler                  ; 29: LESENSE Interrupt
                DCD      LCD_IRQHandler                      ; 30: LCD Interrupt
                DCD      CAN0_IRQHandler                     ; 31: CAN0 Interrupt

                DS32    (0)                 ; Remaining Interrupts are left out

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
                PUBWEAK SVC_Handler
                PUBWEAK PendSV_Handler
                PUBWEAK SysTick_Handler



                PUBWEAK EMU_IRQHandler

                PUBWEAK WDOG0_IRQHandler

                PUBWEAK LDMA_IRQHandler

                PUBWEAK GPIO_EVEN_IRQHandler

                PUBWEAK SMU_IRQHandler

                PUBWEAK TIMER0_IRQHandler

                PUBWEAK USART0_IRQHandler

                PUBWEAK ACMP0_IRQHandler

                PUBWEAK ADC0_IRQHandler

                PUBWEAK I2C0_IRQHandler

                PUBWEAK I2C1_IRQHandler

                PUBWEAK GPIO_ODD_IRQHandler

                PUBWEAK TIMER1_IRQHandler

                PUBWEAK USART1_IRQHandler

                PUBWEAK USART2_IRQHandler

                PUBWEAK UART0_IRQHandler

                PUBWEAK LEUART0_IRQHandler

                PUBWEAK LETIMER0_IRQHandler

                PUBWEAK PCNT0_IRQHandler

                PUBWEAK RTCC_IRQHandler

                PUBWEAK CMU_IRQHandler

                PUBWEAK MSC_IRQHandler

                PUBWEAK CRYPTO0_IRQHandler

                PUBWEAK CRYOTIMER_IRQHandler

                PUBWEAK USART3_IRQHandler

                PUBWEAK WTIMER0_IRQHandler

                PUBWEAK WTIMER1_IRQHandler

                PUBWEAK VDAC0_IRQHandler

                PUBWEAK CSEN_IRQHandler

                PUBWEAK LESENSE_IRQHandler

                PUBWEAK LCD_IRQHandler

                PUBWEAK CAN0_IRQHandler
                SECTION .text:CODE:REORDER:NOROOT(1)
NMI_Handler
sl_app_properties     ; Provide a dummy value for the sl_app_properties symbol.
HardFault_Handler
SVC_Handler
PendSV_Handler
SysTick_Handler



EMU_IRQHandler

WDOG0_IRQHandler

LDMA_IRQHandler

GPIO_EVEN_IRQHandler

SMU_IRQHandler

TIMER0_IRQHandler

USART0_IRQHandler

ACMP0_IRQHandler

ADC0_IRQHandler

I2C0_IRQHandler

I2C1_IRQHandler

GPIO_ODD_IRQHandler

TIMER1_IRQHandler

USART1_IRQHandler

USART2_IRQHandler

UART0_IRQHandler

LEUART0_IRQHandler

LETIMER0_IRQHandler

PCNT0_IRQHandler

RTCC_IRQHandler

CMU_IRQHandler

MSC_IRQHandler

CRYPTO0_IRQHandler

CRYOTIMER_IRQHandler

USART3_IRQHandler

WTIMER0_IRQHandler

WTIMER1_IRQHandler

VDAC0_IRQHandler

CSEN_IRQHandler

LESENSE_IRQHandler

LCD_IRQHandler

CAN0_IRQHandler
Default_Handler
                B        .

                END
