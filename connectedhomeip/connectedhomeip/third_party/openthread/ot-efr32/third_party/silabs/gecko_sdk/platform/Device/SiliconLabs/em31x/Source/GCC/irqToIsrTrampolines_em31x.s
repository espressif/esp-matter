/**************************************************************************//**
 * @file     irqToIsrTrampolines_em31x.s
 * @brief    Device IRQ Handler trampoline functions to call Legacy ISRs
 *
 * @section License
 * <b>Copyright 2018 Silicon Laboratories, Inc. www.silabs.com</b>
 ******************************************************************************/


    .syntax     unified

    .text
    .thumb
    .thumb_func
    .align  1

    .weak   NMI_Handler
    .type   NMI_Handler, %function
NMI_Handler:
    b       halNmiIsr
    .size   NMI_Handler, . - NMI_Handler

    .weak   HardFault_Handler
    .type   HardFault_Handler, %function
HardFault_Handler:
    b       halHardFaultIsr
    .size   HardFault_Handler, . - HardFault_Handler

    .weak   MemManage_Handler
    .type   MemManage_Handler, %function
MemManage_Handler:
    b       halMemoryFaultIsr
    .size   MemManage_Handler, . - MemManage_Handler

    .weak   BusFault_Handler
    .type   BusFault_Handler, %function
BusFault_Handler:
    b       halBusFaultIsr
    .size   BusFault_Handler, . - BusFault_Handler

    .weak   UsageFault_Handler
    .type   UsageFault_Handler, %function
UsageFault_Handler:
    b       halUsageFaultIsr
    .size   UsageFault_Handler, . - UsageFault_Handler

    .weak   SVC_Handler
    .type   SVC_Handler, %function
SVC_Handler:
    b       halSvCallIsr
    .size   SVC_Handler, . - SVC_Handler

    .weak   DebugMon_Handler
    .type   DebugMon_Handler, %function
DebugMon_Handler:
    b       halDebugMonitorIsr
    .size   DebugMon_Handler, . - DebugMon_Handler

    .weak   PendSV_Handler
    .type   PendSV_Handler, %function
PendSV_Handler:
    b       halPendSvIsr
    .size   PendSV_Handler, . - PendSV_Handler

    .weak   SysTick_Handler
    .type   SysTick_Handler, %function
SysTick_Handler:
    b       halInternalSysTickIsr
    .size   SysTick_Handler, . - SysTick_Handler

    .weak   TIM1_IRQHandler
    .type   TIM1_IRQHandler, %function
TIM1_IRQHandler:
    b       halTimer1Isr
    .size   TIM1_IRQHandler, . - TIM1_IRQHandler

    .weak   TIM2_IRQHandler
    .type   TIM2_IRQHandler, %function
TIM2_IRQHandler:
    b       halTimer2Isr
    .size   TIM2_IRQHandler, . - TIM2_IRQHandler

    .weak   MGMT_IRQHandler
    .type   MGMT_IRQHandler, %function
MGMT_IRQHandler:
    b       halManagementIsr
    .size   MGMT_IRQHandler, . - MGMT_IRQHandler

    .weak   BB_IRQHandler
    .type   BB_IRQHandler, %function
BB_IRQHandler:
    b       halBaseBandIsr
    .size   BB_IRQHandler, . - BB_IRQHandler

    .weak   SLEEPTMR_IRQHandler
    .type   SLEEPTMR_IRQHandler, %function
SLEEPTMR_IRQHandler:
    b       halSleepTimerIsr
    .size   SLEEPTMR_IRQHandler, . - SLEEPTMR_IRQHandler

    .weak   SC1_IRQHandler
    .type   SC1_IRQHandler, %function
SC1_IRQHandler:
    b       halSc1Isr
    .size   SC1_IRQHandler, . - SC1_IRQHandler

    .weak   SC2_IRQHandler
    .type   SC2_IRQHandler, %function
SC2_IRQHandler:
    b       halSc2Isr
    .size   SC2_IRQHandler, . - SC2_IRQHandler

    .weak   AESCCM_IRQHandler
    .type   AESCCM_IRQHandler, %function
AESCCM_IRQHandler:
    b       halSecurityIsr
    .size   AESCCM_IRQHandler, . - AESCCM_IRQHandler

    .weak   MACTMR_IRQHandler
    .type   MACTMR_IRQHandler, %function
MACTMR_IRQHandler:
    b       halStackMacTimerIsr
    .size   MACTMR_IRQHandler, . - MACTMR_IRQHandler

    .weak   MACTX_IRQHandler
    .type   MACTX_IRQHandler, %function
MACTX_IRQHandler:
    b       emRadioTransmitIsr
    .size   MACTX_IRQHandler, . - MACTX_IRQHandler

    .weak   MACRX_IRQHandler
    .type   MACRX_IRQHandler, %function
MACRX_IRQHandler:
    b       emRadioReceiveIsr
    .size   MACRX_IRQHandler, . - MACRX_IRQHandler

    .weak   ADC_IRQHandler
    .type   ADC_IRQHandler, %function
ADC_IRQHandler:
    b       halAdcIsr
    .size   ADC_IRQHandler, . - ADC_IRQHandler

    .weak   IRQA_IRQHandler
    .type   IRQA_IRQHandler, %function
IRQA_IRQHandler:
    b       halIrqAIsr
    .size   IRQA_IRQHandler, . - IRQA_IRQHandler

    .weak   IRQB_IRQHandler
    .type   IRQB_IRQHandler, %function
IRQB_IRQHandler:
    b       halIrqBIsr
    .size   IRQB_IRQHandler, . - IRQB_IRQHandler

    .weak   IRQC_IRQHandler
    .type   IRQC_IRQHandler, %function
IRQC_IRQHandler:
    b       halIrqCIsr
    .size   IRQC_IRQHandler, . - IRQC_IRQHandler

    .weak   IRQD_IRQHandler
    .type   IRQD_IRQHandler, %function
IRQD_IRQHandler:
    b       halIrqDIsr
    .size   IRQD_IRQHandler, . - IRQD_IRQHandler

    .weak   DEBUG_IRQHandler
    .type   DEBUG_IRQHandler, %function
DEBUG_IRQHandler:
    b       halDebugIsr
    .size   DEBUG_IRQHandler, . - DEBUG_IRQHandler


    .end
