;/**************************************************************************//**
; * @file     irqToIsrTrampolines_em358x.s
; * @brief    Device IRQ Handler trampoline functions to call Legacy ISRs
; *
; * @section License
; * <b>Copyright 2017 Silicon Laboratories, Inc. www.silabs.com</b>
; ******************************************************************************/


        MODULE  ?irqToIsrTrampolines

        EXTERN  halNmiIsr
        EXTERN  halHardFaultIsr
        EXTERN  halMemoryFaultIsr
        EXTERN  halBusFaultIsr
        EXTERN  halUsageFaultIsr
        EXTERN  halSvCallIsr
        EXTERN  halDebugMonitorIsr
        EXTERN  halPendSvIsr
        EXTERN  halInternalSysTickIsr
        EXTERN  halTimer1Isr
        EXTERN  halTimer2Isr
        EXTERN  halManagementIsr
        EXTERN  halBaseBandIsr
        EXTERN  halSleepTimerIsr
        EXTERN  halSc1Isr
        EXTERN  halSc2Isr
        EXTERN  halSecurityIsr
        EXTERN  halStackMacTimerIsr
        EXTERN  emRadioTransmitIsr
        EXTERN  emRadioReceiveIsr
        EXTERN  halAdcIsr
        EXTERN  halIrqAIsr
        EXTERN  halIrqBIsr
        EXTERN  halIrqCIsr
        EXTERN  halIrqDIsr
        EXTERN  halDebugIsr
        EXTERN  halSc3Isr
        EXTERN  halSc4Isr
        EXTERN  halUsbIsr

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Define the IRQ handlers to be a simple branch to hal Isr routines.
;; This method allows CMSIS Device IRQs to remain as-is while supporting
;; legacy hal ISRs and not pushing/popping any stack behavior as a result
;; of interrupt routines.
;;

;; A trampoline function from Reset_Handler to halEntryPoint is not necessary.
;; The Source/IAR startup code must instantiate the Reset_Handler and as
;; a consequence the functon halEntryPoint is not used.

        PUBWEAK NMI_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
NMI_Handler
        B halNmiIsr

        PUBWEAK HardFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
HardFault_Handler
        B halHardFaultIsr

        PUBWEAK MemManage_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
MemManage_Handler
        B halMemoryFaultIsr

        PUBWEAK BusFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
BusFault_Handler
        B halBusFaultIsr

        PUBWEAK UsageFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
UsageFault_Handler
        B halUsageFaultIsr

        PUBWEAK SVC_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
SVC_Handler
        B halSvCallIsr

        PUBWEAK DebugMon_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
DebugMon_Handler
        B halDebugMonitorIsr

        PUBWEAK PendSV_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
PendSV_Handler
        B halPendSvIsr

        PUBWEAK SysTick_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
SysTick_Handler
        B halInternalSysTickIsr

        ; Device specific interrupt handlers

        PUBWEAK TIM1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIM1_IRQHandler
        B halTimer1Isr

        PUBWEAK TIM2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIM2_IRQHandler
        B halTimer2Isr

        PUBWEAK MGMT_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
MGMT_IRQHandler
        B halManagementIsr

        PUBWEAK BB_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BB_IRQHandler
        B halBaseBandIsr

        PUBWEAK SLEEPTMR_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SLEEPTMR_IRQHandler
        B halSleepTimerIsr

        PUBWEAK SC1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SC1_IRQHandler
        B halSc1Isr

        PUBWEAK SC2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SC2_IRQHandler
        B halSc2Isr

        PUBWEAK AESCCM_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
AESCCM_IRQHandler
        B halSecurityIsr

        PUBWEAK MACTMR_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
MACTMR_IRQHandler
        B halStackMacTimerIsr

        PUBWEAK MACTX_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
MACTX_IRQHandler
        B emRadioTransmitIsr

        PUBWEAK MACRX_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
MACRX_IRQHandler
        B emRadioReceiveIsr

        PUBWEAK ADC_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
ADC_IRQHandler
        B halAdcIsr

        PUBWEAK IRQA_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
IRQA_IRQHandler
        B halIrqAIsr

        PUBWEAK IRQB_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
IRQB_IRQHandler
        B halIrqBIsr

        PUBWEAK IRQC_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
IRQC_IRQHandler
        B halIrqCIsr

        PUBWEAK IRQD_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
IRQD_IRQHandler
        B halIrqDIsr

        PUBWEAK DEBUG_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
DEBUG_IRQHandler
        B halDebugIsr

        PUBWEAK SC3_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SC3_IRQHandler
        B halSc3Isr

        PUBWEAK SC4_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SC4_IRQHandler
        B halSc4Isr

        PUBWEAK USB_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
USB_IRQHandler
        B halUsbIsr

        END
