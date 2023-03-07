/*
 * Copyright (c) 2016-2019 Arm Limited
 *
 * Licensed under the Apache License Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing software
 * distributed under the License is distributed on an "AS IS" BASIS
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __ARM_LTD_SSE_200_AWS_IRQN_H__
#define __ARM_LTD_SSE_200_AWS_IRQN_H__

typedef enum _IRQn_Type {
    NonMaskableInt_IRQn         = -14,   /* Non Maskable Interrupt */
    HardFault_IRQn              = -13,   /* HardFault Interrupt */
    MemoryManagement_IRQn       = -12,   /* Memory Management Interrupt */
    BusFault_IRQn               = -11,   /* Bus Fault Interrupt */
    UsageFault_IRQn             = -10,   /* Usage Fault Interrupt */
    SecureFault_IRQn            = -9,    /* Secure Fault Interrupt */
    SVCall_IRQn                 = -5,    /* SV Call Interrupt */
    DebugMonitor_IRQn           = -4,    /* Debug Monitor Interrupt */
    PendSV_IRQn                 = -2,    /* Pend SV Interrupt */
    SysTick_IRQn                = -1,    /* System Tick Interrupt */
    NONSEC_WATCHDOG_RESET_IRQn  = 0,     /* Non-Secure Watchdog Reset
                                          * Interrupt */
    NONSEC_WATCHDOG_IRQn        = 1,     /* Non-Secure Watchdog Interrupt */
    S32K_TIMER_IRQn             = 2,     /* S32K Timer Interrupt */
    TIMER0_IRQn                 = 3,     /* TIMER 0 Interrupt */
    TIMER1_IRQn                 = 4,     /* TIMER 1 Interrupt */
    DUALTIMER_IRQn              = 5,     /* Dual Timer Interrupt */
    MHU0_IRQn                   = 6,    /* Message Handling Unit 0 */
    MHU1_IRQn                   = 7,    /* Message Handling Unit 1 */
    /* Reserved                 = 8,       Reserved */
    MPC_IRQn                    = 9,     /* MPC Combined (Secure) Interrupt */
    PPC_IRQn                    = 10,    /* PPC Combined (Secure) Interrupt */
    MSC_IRQn                    = 11,    /* MSC Combined (Secure) Interrput */
    BRIDGE_ERROR_IRQn           = 12,    /* Bridge Error Combined
                                          * (Secure) Interrupt */
    /* Reserved                 = 13:31,    Reserved */
    UARTRX0_IRQn                = 32,    /* UART 0 RX Interrupt */
    UARTTX0_IRQn                = 33,    /* UART 0 TX Interrupt */
    /* Reserved                 = 34:41,    Reserved */
    UART0_IRQn                  = 42,    /* UART 0 combined Interrupt */
    /* Reserved                 = 43:46,    Reserved */
    UARTOVF_IRQn                = 47,    /* UART Overflow (0, 1, 2, 3 & 4) */
    /* Reserved                 = 48:55,    Reserved */
    DMA0_ERROR_IRQn             = 56,    /* DMA 0 Error Interrupt */
    DMA0_TC_IRQn                = 57,    /* DMA 0 Terminal Count Interrupt */
    DMA0_IRQn                   = 58,    /* DMA 0 Combined Interrupt */
    DMA1_ERROR_IRQn             = 59,    /* DMA 1 Error Interrupt */
    DMA1_TC_IRQn                = 60,    /* DMA 1 Terminal Count Interrupt */
    DMA1_IRQn                   = 61,    /* DMA 1 Combined Interrupt */
    DMA2_ERROR_IRQn             = 62,    /* DMA 2 Error Interrupt */
    DMA2_TC_IRQn                = 63,    /* DMA 2 Terminal Count Interrupt */
    DMA2_IRQn                   = 64,    /* DMA 2 Combined Interrupt */
    DMA3_ERROR_IRQn             = 65,    /* DMA 3 Error Interrupt */
    DMA3_TC_IRQn                = 66,    /* DMA 3 Terminal Count Interrupt */
    DMA3_IRQn                   = 67,    /* DMA 3 Combined Interrupt */
    /* Reserved                 = 68:123,  Reserved */
}IRQn_Type;

#endif  /* __ARM_LTD_SSE_200_AWS_IRQN_H__ */
