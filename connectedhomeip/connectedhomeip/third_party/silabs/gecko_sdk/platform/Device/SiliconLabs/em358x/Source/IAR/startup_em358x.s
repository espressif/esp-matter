;/**************************************************************************//**
; * @file     startup_em358x.s
; * @brief    CMSIS Cortex-M3 Core Device Startup File for em358x
; * @version 5.8.3
; * @date     30. October 2018
; *
; * @note
; * Copyright (C) 2012 ARM Limited. All rights reserved.
; *
; * @par
; * ARM Limited (ARM) is supplying this software for use with Cortex-M
; * processor based microcontrollers.  This file can be freely distributed
; * within development tools that are supporting such ARM based processors.
; *
; * @par
; * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
; * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
; * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
; * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
; * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
; *
; ******************************************************************************/


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

        MODULE  ?cstartup

        ;; Forward declaration of sections.
        SECTION CSTACK:DATA:NOROOT(3)

        SECTION .intvec:CODE:NOROOT(8)

        EXTERN  __iar_program_start
        EXTERN  SystemInit
        PUBLIC  __vector_table
        PUBLIC  __vector_table_0x1c
        PUBLIC  __Vectors
        PUBLIC  __Vectors_End
        PUBLIC  __Vectors_Size

        DATA

__vector_table
        DCD     sfe(CSTACK)
        DCD     Reset_Handler

        DCD     NMI_Handler
        DCD     HardFault_Handler
        DCD     MemManage_Handler
        DCD     BusFault_Handler
        DCD     UsageFault_Handler
__vector_table_0x1c
        DCD     0
        DCD     0
        DCD     0
        DCD     0
        DCD     SVC_Handler
        DCD     DebugMon_Handler
        DCD     0
        DCD     PendSV_Handler
        DCD     SysTick_Handler

        ; External Interrupts

        DCD TIM1_IRQHandler
        DCD TIM2_IRQHandler
        DCD MGMT_IRQHandler
        DCD BB_IRQHandler
        DCD SLEEPTMR_IRQHandler
        DCD SC1_IRQHandler
        DCD SC2_IRQHandler
        DCD AESCCM_IRQHandler
        DCD MACTMR_IRQHandler
        DCD MACTX_IRQHandler
        DCD MACRX_IRQHandler
        DCD ADC_IRQHandler
        DCD IRQA_IRQHandler
        DCD IRQB_IRQHandler
        DCD IRQC_IRQHandler
        DCD IRQD_IRQHandler
        DCD DEBUG_IRQHandler
        DCD SC3_IRQHandler
        DCD SC4_IRQHandler
        DCD USB_IRQHandler

__Vectors_End

__Vectors       EQU   __vector_table
__Vectors_Size  EQU   __Vectors_End - __Vectors


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Default interrupt handlers.
;;
        THUMB

;;
;; Start Handler called by SystemInit to start the main program running.
;; Since IAR and GCC have very different semantics for this, they are
;; wrapped in this function that can be called by common code without
;; worrying about which compiler is being used.
;;
		
        PUBLIC Start_Handler
        SECTION .text:CODE:REORDER:NOROOT(2)
Start_Handler
        LDR     R0, =__iar_program_start
        BX      R0

        PUBWEAK Reset_Handler
        SECTION .text:CODE:REORDER:NOROOT(2)
Reset_Handler
        LDR     R0, =SystemInit
        BX      R0
;; __iar_program_start is now called by SystemInit.
		
        PUBWEAK NMI_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
NMI_Handler
        B NMI_Handler

        PUBWEAK HardFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
HardFault_Handler
        B HardFault_Handler

        PUBWEAK MemManage_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
MemManage_Handler
        B MemManage_Handler

        PUBWEAK BusFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
BusFault_Handler
        B BusFault_Handler

        PUBWEAK UsageFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
UsageFault_Handler
        B UsageFault_Handler

        PUBWEAK SVC_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
SVC_Handler
        B SVC_Handler

        PUBWEAK DebugMon_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
DebugMon_Handler
        B DebugMon_Handler

        PUBWEAK PendSV_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
PendSV_Handler
        B PendSV_Handler

        PUBWEAK SysTick_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
SysTick_Handler
        B SysTick_Handler

        ; Device specific interrupt handlers

        PUBWEAK TIM1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIM1_IRQHandler
        B TIM1_IRQHandler

        PUBWEAK TIM2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIM2_IRQHandler
        B TIM2_IRQHandler

        PUBWEAK MGMT_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
MGMT_IRQHandler
        B MGMT_IRQHandler

        PUBWEAK BB_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BB_IRQHandler
        B BB_IRQHandler

        PUBWEAK SLEEPTMR_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SLEEPTMR_IRQHandler
        B SLEEPTMR_IRQHandler

        PUBWEAK SC1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SC1_IRQHandler
        B SC1_IRQHandler

        PUBWEAK SC2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SC2_IRQHandler
        B SC2_IRQHandler

        PUBWEAK AESCCM_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
AESCCM_IRQHandler
        B AESCCM_IRQHandler

        PUBWEAK MACTMR_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
MACTMR_IRQHandler
        B MACTMR_IRQHandler

        PUBWEAK MACTX_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
MACTX_IRQHandler
        B MACTX_IRQHandler

        PUBWEAK MACRX_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
MACRX_IRQHandler
        B MACRX_IRQHandler

        PUBWEAK ADC_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
ADC_IRQHandler
        B ADC_IRQHandler

        PUBWEAK IRQA_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
IRQA_IRQHandler
        B IRQA_IRQHandler

        PUBWEAK IRQB_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
IRQB_IRQHandler
        B IRQB_IRQHandler

        PUBWEAK IRQC_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
IRQC_IRQHandler
        B IRQC_IRQHandler

        PUBWEAK IRQD_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
IRQD_IRQHandler
        B IRQD_IRQHandler

        PUBWEAK DEBUG_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
DEBUG_IRQHandler
        B DEBUG_IRQHandler

        PUBWEAK SC3_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SC3_IRQHandler
        B SC3_IRQHandler

        PUBWEAK SC4_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SC4_IRQHandler
        B SC4_IRQHandler

        PUBWEAK USB_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
USB_IRQHandler
        B USB_IRQHandler


        END
