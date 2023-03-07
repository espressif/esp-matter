/***************************************************************************//**
 * @file
 * @brief startup file for Silicon Labs EFR32ZG13S devices.
 *        For use with GCC for ARM Embedded Processors
 *******************************************************************************
 * # License
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is Third Party Software licensed by Silicon Labs from a third party
 * and is governed by the sections of the MSLA applicable to Third Party
 * Software and the additional terms set forth below.
 *
 ******************************************************************************/
/*
 * Copyright (c) 2009-2021 ARM Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

.syntax     unified
                .arch       armv7e-m
                .section    .stack
                .align      3
#ifdef __STACK_SIZE
                .equ        Stack_Size, __STACK_SIZE
#else
                .equ        Stack_Size, 0x00000400
#endif
                .globl      __StackTop
                .globl      __StackLimit
__StackLimit:
                .space      Stack_Size
                .size       __StackLimit, . - __StackLimit
__StackTop:
                .size       __StackTop, . - __StackTop

                .section    .heap
                .align      3
#ifdef __HEAP_SIZE
                .equ        Heap_Size, __HEAP_SIZE
#else
    .equ        Heap_Size, 0x00000C00
#endif
                .globl      __HeapBase
                .globl      __HeapLimit
__HeapBase:
                .if Heap_Size
                .space      Heap_Size
                .endif
                .size       __HeapBase, . - __HeapBase
__HeapLimit:
                .size       __HeapLimit, . - __HeapLimit

                .section    .vectors
                .align      8
                .globl      __Vectors
                .globl   __Vectors_End
                .globl   __Vectors_Size
__Vectors:
                .long    __StackTop                           /* Top of Stack */
                .long    Reset_Handler                        /* Reset Handler */
                .long    NMI_Handler                          /* -14 NMI Handler */
                .long    HardFault_Handler                    /* -13 Hard Fault Handler */
                .long    MemManage_Handler                    /* -12 MPU Fault Handler */
                .long    BusFault_Handler                     /* -11 Bus Fault Handler */
                .long    UsageFault_Handler                   /* -10 Usage Fault Handler */
                .long    Default_Handler                      /* Reserved */
                .long    Default_Handler                      /* Reserved */
                .long    Default_Handler                      /* Reserved */
                .long    Default_Handler                      /* Reserved */
                .long    SVC_Handler                          /* -5 SVCall Handler */
                .long    DebugMon_Handler                     /* -4 Debug Monitor Handler */
                .long    sl_app_properties                    /* -3 Application properties */
                .long    PendSV_Handler                       /* -2 PendSV Handler */
                .long    SysTick_Handler                      /* -1 SysTick Handler */

    /* External interrupts */

                .long    EMU_IRQHandler                       /* 0 - EMU */
                .long    FRC_PRI_IRQHandler                   /* 1 - FRC_PRI */
                .long    WDOG0_IRQHandler                     /* 2 - WDOG0 */
                .long    WDOG1_IRQHandler                     /* 3 - WDOG1 */
                .long    FRC_IRQHandler                       /* 4 - FRC */
                .long    MODEM_IRQHandler                     /* 5 - MODEM */
                .long    RAC_SEQ_IRQHandler                   /* 6 - RAC_SEQ */
                .long    RAC_RSM_IRQHandler                   /* 7 - RAC_RSM */
                .long    BUFC_IRQHandler                      /* 8 - BUFC */
                .long    LDMA_IRQHandler                      /* 9 - LDMA */
                .long    GPIO_EVEN_IRQHandler                 /* 10 - GPIO_EVEN */
                .long    TIMER0_IRQHandler                    /* 11 - TIMER0 */
                .long    USART0_RX_IRQHandler                 /* 12 - USART0_RX */
                .long    USART0_TX_IRQHandler                 /* 13 - USART0_TX */
                .long    ACMP0_IRQHandler                     /* 14 - ACMP0 */
                .long    ADC0_IRQHandler                      /* 15 - ADC0 */
                .long    IDAC0_IRQHandler                     /* 16 - IDAC0 */
                .long    I2C0_IRQHandler                      /* 17 - I2C0 */
                .long    GPIO_ODD_IRQHandler                  /* 18 - GPIO_ODD */
                .long    TIMER1_IRQHandler                    /* 19 - TIMER1 */
                .long    USART1_RX_IRQHandler                 /* 20 - USART1_RX */
                .long    USART1_TX_IRQHandler                 /* 21 - USART1_TX */
                .long    LEUART0_IRQHandler                   /* 22 - LEUART0 */
                .long    PCNT0_IRQHandler                     /* 23 - PCNT0 */
                .long    CMU_IRQHandler                       /* 24 - CMU */
                .long    MSC_IRQHandler                       /* 25 - MSC */
                .long    CRYPTO0_IRQHandler                   /* 26 - CRYPTO0 */
                .long    LETIMER0_IRQHandler                  /* 27 - LETIMER0 */
                .long    AGC_IRQHandler                       /* 28 - AGC */
                .long    PROTIMER_IRQHandler                  /* 29 - PROTIMER */
                .long    PRORTC_IRQHandler                    /* 30 - PRORTC */
                .long    RTCC_IRQHandler                      /* 31 - RTCC */
                .long    SYNTH_IRQHandler                     /* 32 - SYNTH */
                .long    CRYOTIMER_IRQHandler                 /* 33 - CRYOTIMER */
                .long    RFSENSE_IRQHandler                   /* 34 - RFSENSE */
                .long    FPUEH_IRQHandler                     /* 35 - FPUEH */
                .long    SMU_IRQHandler                       /* 36 - SMU */
                .long    WTIMER0_IRQHandler                   /* 37 - WTIMER0 */
                .long    USART2_RX_IRQHandler                 /* 38 - USART2_RX */
                .long    USART2_TX_IRQHandler                 /* 39 - USART2_TX */
                .long    I2C1_IRQHandler                      /* 40 - I2C1 */
                .long    VDAC0_IRQHandler                     /* 41 - VDAC0 */
                .long    CSEN_IRQHandler                      /* 42 - CSEN */
                .long    LESENSE_IRQHandler                   /* 43 - LESENSE */
                .long    CRYPTO1_IRQHandler                   /* 44 - CRYPTO1 */
                .long    TRNG0_IRQHandler                     /* 45 - TRNG0 */
                .long    Default_Handler                      /* 46 - Reserved */

                .space   (177 * 4)                         /* Remaining Interrupts are left out */
__Vectors_End:
                .equ     __Vectors_Size, __Vectors_End - __Vectors
                .size       __Vectors, . - __Vectors

                .thumb
                .section .text
                .align   2

                .thumb_func
                .type    Reset_Handler, %function
                .globl   Reset_Handler
                .fnstart
Reset_Handler:
                bl       SystemInit

                ldr      r4, =__copy_table_start__
                ldr      r5, =__copy_table_end__

.L_loop0:
                cmp      r4, r5
                bge      .L_loop0_done
                ldr      r1, [r4]                /* source address */
                ldr      r2, [r4, #4]            /* destination address */
                ldr      r3, [r4, #8]            /* word count */
                lsls     r3, r3, #2              /* byte count */

.L_loop0_0:
                subs    r3, #4
                ittt    ge
                ldrge   r0, [r1, r3]
                strge   r0, [r2, r3]
                bge     .L_loop0_0

                adds    r4, #12
                b       .L_loop0

.L_loop0_done:

                ldr      r3, =__zero_table_start__
                ldr      r4, =__zero_table_end__

.L_loop2:
                cmp     r3, r4
                bge     .L_loop2_done
                ldr     r1, [r3]
                ldr     r2, [r3, #4]
                movs    r0, 0

.L_loop2_0:
                subs    r2, #4
                itt     ge
                strge   r0, [r1, r2]
                bge     .L_loop2_0
                adds    r3, #8
                b       .L_loop2
.L_loop2_done:

                #ifndef __START
                #define __START _start
                #endif
                bl      __START

                .fnend
                .size    Reset_Handler, . - Reset_Handler


/* The default macro is not used for HardFault_Handler
 * because this results in a poor debug illusion.
 */
                .thumb_func
                .type    HardFault_Handler, %function
                .weak    HardFault_Handler
                .fnstart
HardFault_Handler:
                b        .
                .fnend
                .size    HardFault_Handler, . - HardFault_Handler

                .thumb_func
                .type    Default_Handler, %function
                .weak    Default_Handler
                .fnstart
Default_Handler:
sl_app_properties: /* Provide a dummy value for the sl_app_properties symbol. */
                b        .
                .fnend
                .size   Default_Handler, . - Default_Handler

/* Macro to define default exception/interrupt handlers.
 * Default handler are weak symbols with an endless loop.
 * They can be overwritten by real handlers.
 */
                .macro   Set_Default_Handler  Handler_Name
                .weak    \Handler_Name
                .set     \Handler_Name, Default_Handler
                .endm

/* Default exception/interrupt handler */
                Set_Default_Handler     NMI_Handler
                Set_Default_Handler     MemManage_Handler
                Set_Default_Handler     BusFault_Handler
                Set_Default_Handler     UsageFault_Handler
                Set_Default_Handler     SVC_Handler
                Set_Default_Handler     DebugMon_Handler
                Set_Default_Handler     PendSV_Handler
                Set_Default_Handler     SysTick_Handler

                Set_Default_Handler     EMU_IRQHandler
                Set_Default_Handler     FRC_PRI_IRQHandler
                Set_Default_Handler     WDOG0_IRQHandler
                Set_Default_Handler     WDOG1_IRQHandler
                Set_Default_Handler     FRC_IRQHandler
                Set_Default_Handler     MODEM_IRQHandler
                Set_Default_Handler     RAC_SEQ_IRQHandler
                Set_Default_Handler     RAC_RSM_IRQHandler
                Set_Default_Handler     BUFC_IRQHandler
                Set_Default_Handler     LDMA_IRQHandler
                Set_Default_Handler     GPIO_EVEN_IRQHandler
                Set_Default_Handler     TIMER0_IRQHandler
                Set_Default_Handler     USART0_RX_IRQHandler
                Set_Default_Handler     USART0_TX_IRQHandler
                Set_Default_Handler     ACMP0_IRQHandler
                Set_Default_Handler     ADC0_IRQHandler
                Set_Default_Handler     IDAC0_IRQHandler
                Set_Default_Handler     I2C0_IRQHandler
                Set_Default_Handler     GPIO_ODD_IRQHandler
                Set_Default_Handler     TIMER1_IRQHandler
                Set_Default_Handler     USART1_RX_IRQHandler
                Set_Default_Handler     USART1_TX_IRQHandler
                Set_Default_Handler     LEUART0_IRQHandler
                Set_Default_Handler     PCNT0_IRQHandler
                Set_Default_Handler     CMU_IRQHandler
                Set_Default_Handler     MSC_IRQHandler
                Set_Default_Handler     CRYPTO0_IRQHandler
                Set_Default_Handler     LETIMER0_IRQHandler
                Set_Default_Handler     AGC_IRQHandler
                Set_Default_Handler     PROTIMER_IRQHandler
                Set_Default_Handler     PRORTC_IRQHandler
                Set_Default_Handler     RTCC_IRQHandler
                Set_Default_Handler     SYNTH_IRQHandler
                Set_Default_Handler     CRYOTIMER_IRQHandler
                Set_Default_Handler     RFSENSE_IRQHandler
                Set_Default_Handler     FPUEH_IRQHandler
                Set_Default_Handler     SMU_IRQHandler
                Set_Default_Handler     WTIMER0_IRQHandler
                Set_Default_Handler     USART2_RX_IRQHandler
                Set_Default_Handler     USART2_TX_IRQHandler
                Set_Default_Handler     I2C1_IRQHandler
                Set_Default_Handler     VDAC0_IRQHandler
                Set_Default_Handler     CSEN_IRQHandler
                Set_Default_Handler     LESENSE_IRQHandler
                Set_Default_Handler     CRYPTO1_IRQHandler
                Set_Default_Handler     TRNG0_IRQHandler

                .end
