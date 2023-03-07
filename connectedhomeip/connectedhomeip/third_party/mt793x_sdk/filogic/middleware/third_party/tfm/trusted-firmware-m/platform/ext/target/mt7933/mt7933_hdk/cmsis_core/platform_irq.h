/*
 * Copyright (c) 2016-2018 ARM Limited
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

#ifndef __ARM_LTD_AN521_IRQN_H__
#define __ARM_LTD_AN521_IRQN_H__

typedef enum _IRQn_Type {
    NonMaskableInt_IRQn         = -14,  /* Non Maskable Interrupt */
    HardFault_IRQn              = -13,  /* HardFault Interrupt */
    MemoryManagement_IRQn       = -12,  /* Memory Management Interrupt */
    BusFault_IRQn               = -11,  /* Bus Fault Interrupt */
    UsageFault_IRQn             = -10,  /* Usage Fault Interrupt */
    SecureFault_IRQn            = -9,   /* Secure Fault Interrupt */
    SVCall_IRQn                 = -5,   /* SV Call Interrupt */
    DebugMonitor_IRQn           = -4,   /* Debug Monitor Interrupt */
    PendSV_IRQn                 = -2,   /* Pend SV Interrupt */
    SysTick_IRQn                = -1,   /* System Tick Interrupt */
    NONSEC_WATCHDOG_RESET_IRQn  = 0,    /* Non-Secure Watchdog Reset
                                         * Interrupt */
    NONSEC_WATCHDOG_IRQn        = 1,    /* Non-Secure Watchdog Interrupt */
    S32K_TIMER_IRQn             = 2,    /* S32K Timer Interrupt */
    TIMER0_IRQn                 = 3,    /* TIMER 0 Interrupt */
    TIMER1_IRQn                 = 4,    /* TIMER 1 Interrupt */
    DUALTIMER_IRQn              = 5,    /* Dual Timer Interrupt */
    MHU0_IRQn                   = 6,    /* Message Handling Unit 0 */
    MHU1_IRQn                   = 7,    /* Message Handling Unit 1 */
    MPC_IRQn                    = 9,    /* MPC Combined (Secure) Interrupt */
    PPC_IRQn                    = 10,   /* PPC Combined (Secure) Interrupt */
    MSC_IRQn                    = 11,   /* MSC Combined (Secure) Interrput */
    BRIDGE_ERROR_IRQn           = 12,   /* Bridge Error Combined
                                         *(Secure) Interrupt */
    INVALID_INSTR_CACHE_IRQn    = 13,   /* CPU Instruction Cache Invalidation
                                         * Interrupt*/
    SYS_PPU_IRQn                = 15,   /* SYS PPU */
    CPU0_PPU_IRQn               = 16,   /* CPU0 PPU */
    CPU1_PPU_IRQn               = 17,   /* CPU1 PPU */
    CPU0_DBG_PPU_IRQn           = 18,   /* CPU0 DBG PPU */
    CPU1_DBG_PPU_IRQn           = 19,   /* CPU1 DBG PPU */
    CRYPT_PPU_IRQn              = 20,   /* CRYPT PPU */
    RAM0_PPU_IRQn               = 22,   /* RAM0 PPU */
    RAM1_PPU_IRQn               = 23,   /* RAM1 PPU */
    RAM2_PPU_IRQn               = 24,   /* RAM2 PPU */
    RAM3_PPU_IRQn               = 25,   /* RAM3 PPU */
    DEBUG_PPU_IRQn              = 26,   /* DEBUG PPU */
    CPU0_CTI_IRQn               = 28,   /* CPU0 CTI */
    CPU1_CTI_IRQn               = 29,   /* CPU1 CTI */
    CORDIOTXCOMB_IRQn           = 30,   /* CORDIO TX combined */
    CORDIORXCOMB_IRQn           = 31,   /* CORDIO RX combined */
    UARTRX0_IRQn                = 32,   /* UART 0 RX Interrupt */
    UARTTX0_IRQn                = 33,   /* UART 0 TX Interrupt */
    UARTRX1_IRQn                = 34,   /* UART 1 RX Interrupt */
    UARTTX1_IRQn                = 35,   /* UART 1 TX Interrupt */
    UARTRX2_IRQn                = 36,   /* UART 2 RX Interrupt */
    UARTTX2_IRQn                = 37,   /* UART 2 TX Interrupt */
    UARTRX3_IRQn                = 38,   /* UART 3 RX Interrupt */
    UARTTX3_IRQn                = 39,   /* UART 3 TX Interrupt */
    UARTRX4_IRQn                = 40,   /* UART 4 RX Interrupt */
    UARTTX4_IRQn                = 41,   /* UART 4 TX Interrupt */
    UART0_IRQn                  = 42,   /* UART 0 combined Interrupt */
    UART1_IRQn                  = 43,   /* UART 1 combined Interrupt */
    UART2_IRQn                  = 44,   /* UART 2 combined Interrupt */
    UART3_IRQn                  = 45,   /* UART 3 combined Interrupt */
    UART4_IRQn                  = 46,   /* UART 4 combined Interrupt */
    UARTOVF_IRQn                = 47,   /* UART Overflow (0, 1, 2, 3 & 4) */
    ETHERNET_IRQn               = 48,   /* Ethernet Interrupt */
    I2S_IRQn                    = 49,   /* Audio I2S Interrupt */
    TSC_IRQn                    = 50,   /* Touch Screen Interrupt */
    SPI0_IRQn                   = 51,   /* SPI 0 Interrupt */
    SPI1_IRQn                   = 52,   /* SPI 1 Interrupt */
    SPI2_IRQn                   = 53,   /* SPI 2 Interrupt */
    SPI3_IRQn                   = 54,   /* SPI 3 Interrupt */
    SPI4_IRQn                   = 55,   /* SPI 4 Interrupt */
    DMA0_ERROR_IRQn             = 56,   /* DMA 0 Error Interrupt */
    DMA0_TC_IRQn                = 57,   /* DMA 0 Terminal Count Interrupt */
    DMA0_IRQn                   = 58,   /* DMA 0 Combined Interrupt */
    DMA1_ERROR_IRQn             = 59,   /* DMA 1 Error Interrupt */
    DMA1_TC_IRQn                = 60,   /* DMA 1 Terminal Count Interrupt */
    DMA1_IRQn                   = 61,   /* DMA 1 Combined Interrupt */
    DMA2_ERROR_IRQn             = 62,   /* DMA 2 Error Interrupt */
    DMA2_TC_IRQn                = 63,   /* DMA 2 Terminal Count Interrupt */
    DMA2_IRQn                   = 64,   /* DMA 2 Combined Interrupt */
    DMA3_ERROR_IRQn             = 65,   /* DMA 3 Error Interrupt */
    DMA3_TC_IRQn                = 66,   /* DMA 3 Terminal Count Interrupt */
    DMA3_IRQn                   = 67,   /* DMA 3 Combined Interrupt */
    GPIO0_IRQn                  = 68,   /* GPIO 0 Combined Interrupt */
    GPIO1_IRQn                  = 69,   /* GPIO 1 Combined Interrupt */
    GPIO2_IRQn                  = 70,   /* GPIO 2 Combined Interrupt */
    GPIO3_IRQn                  = 71,   /* GPIO 3 Combined Interrupt */
    GPIO0_0_IRQn                = 72,   /* GPIO0 has 16 pins with IRQs */
    GPIO0_1_IRQn                = 73,
    GPIO0_2_IRQn                = 74,
    GPIO0_3_IRQn                = 75,
    GPIO0_4_IRQn                = 76,
    GPIO0_5_IRQn                = 77,
    GPIO0_6_IRQn                = 78,
    GPIO0_7_IRQn                = 79,
    GPIO0_8_IRQn                = 80,
    GPIO0_9_IRQn                = 81,
    GPIO0_10_IRQn               = 82,
    GPIO0_11_IRQn               = 83,
    GPIO0_12_IRQn               = 84,
    GPIO0_13_IRQn               = 85,
    GPIO0_14_IRQn               = 86,
    GPIO0_15_IRQn               = 87,
    GPIO1_0_IRQn                = 88,   /* GPIO1 has 16 pins with IRQs */
    GPIO1_1_IRQn                = 89,
    GPIO1_2_IRQn                = 90,
    GPIO1_3_IRQn                = 91,
    GPIO1_4_IRQn                = 92,
    GPIO1_5_IRQn                = 93,
    GPIO1_6_IRQn                = 94,
    GPIO1_7_IRQn                = 95,
    GPIO1_8_IRQn                = 96,
    GPIO1_9_IRQn                = 97,
    GPIO1_10_IRQn               = 98,
    GPIO1_11_IRQn               = 99,
    GPIO1_12_IRQn               = 100,
    GPIO1_13_IRQn               = 101,
    GPIO1_14_IRQn               = 102,
    GPIO1_15_IRQn               = 103,
    GPIO2_0_IRQn                = 104,   /* GPIO2 has 16 pins with IRQs */
    GPIO2_1_IRQn                = 105,
    GPIO2_2_IRQn                = 106,
    GPIO2_3_IRQn                = 107,
    GPIO2_4_IRQn                = 108,
    GPIO2_5_IRQn                = 109,
    GPIO2_6_IRQn                = 110,
    GPIO2_7_IRQn                = 111,
    GPIO2_8_IRQn                = 112,
    GPIO2_9_IRQn                = 113,
    GPIO2_10_IRQn               = 114,
    GPIO2_11_IRQn               = 115,
    GPIO2_12_IRQn               = 116,
    GPIO2_13_IRQn               = 117,
    GPIO2_14_IRQn               = 118,
    GPIO2_15_IRQn               = 119,
    GPIO3_0_IRQn                = 120,   /* GPIO3 has 4 pins with IRQs */
    GPIO3_1_IRQn                = 121,
    GPIO3_2_IRQn                = 122,
    GPIO3_3_IRQn                = 123,
}IRQn_Type;

#endif  /* __ARM_LTD_AN521_IRQN_H__ */
