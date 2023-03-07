/***************************************************************************//**
 * @file
 * @brief startup file for Silicon Labs EFM32ZG devices.
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
                .arch       armv6-m
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
    .equ        Heap_Size, 0x00000000
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
                .long    Default_Handler                      /* Reserved */
                .long    Default_Handler                      /* Reserved */
                .long    Default_Handler                      /* Reserved */
                .long    Default_Handler                      /* Reserved */
                .long    Default_Handler                      /* Reserved */
                .long    Default_Handler                      /* Reserved */
                .long    Default_Handler                      /* Reserved */
                .long    SVC_Handler                          /* -5 SVCall Handler */
                .long    Default_Handler                      /* Reserved */
                .long    sl_app_properties                    /* -3 Application properties */
                .long    PendSV_Handler                       /* -2 PendSV Handler */
                .long    SysTick_Handler                      /* -1 SysTick Handler */

    /* External interrupts */
                .long    DMA_IRQHandler                       /* 0 - DMA */
                .long    GPIO_EVEN_IRQHandler                 /* 1 - GPIO_EVEN */
                .long    TIMER0_IRQHandler                    /* 2 - TIMER0 */
                .long    ACMP0_IRQHandler                     /* 3 - ACMP0 */
                .long    ADC0_IRQHandler                      /* 4 - ADC0 */
                .long    I2C0_IRQHandler                      /* 5 - I2C0 */
                .long    GPIO_ODD_IRQHandler                  /* 6 - GPIO_ODD */
                .long    TIMER1_IRQHandler                    /* 7 - TIMER1 */
                .long    USART1_RX_IRQHandler                 /* 8 - USART1_RX */
                .long    USART1_TX_IRQHandler                 /* 9 - USART1_TX */
                .long    LEUART0_IRQHandler                   /* 10 - LEUART0 */
                .long    PCNT0_IRQHandler                     /* 11 - PCNT0 */
                .long    RTC_IRQHandler                       /* 12 - RTC */
                .long    CMU_IRQHandler                       /* 13 - CMU */
                .long    VCMP_IRQHandler                      /* 14 - VCMP */
                .long    MSC_IRQHandler                       /* 15 - MSC */
                .long    AES_IRQHandler                       /* 16 - AES */
                .long    Default_Handler                      /* 17 - Reserved */
                .long    Default_Handler                      /* 18 - Reserved */

                .space   (13 * 4)                          /* Remaining Interrupts are left out */

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
                blt     .L_loop0_0_done
                ldr     r0, [r1, r3]
                str     r0, [r2, r3]
                b       .L_loop0_0

.L_loop0_0_done:
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
                blt     .L_loop2_0_done
                str     r0, [r1, r2]
                b       .L_loop2_0
.L_loop2_0_done:

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
                Set_Default_Handler     SVC_Handler
                Set_Default_Handler     PendSV_Handler
                Set_Default_Handler     SysTick_Handler
                Set_Default_Handler     DMA_IRQHandler
                Set_Default_Handler     GPIO_EVEN_IRQHandler
                Set_Default_Handler     TIMER0_IRQHandler
                Set_Default_Handler     ACMP0_IRQHandler
                Set_Default_Handler     ADC0_IRQHandler
                Set_Default_Handler     I2C0_IRQHandler
                Set_Default_Handler     GPIO_ODD_IRQHandler
                Set_Default_Handler     TIMER1_IRQHandler
                Set_Default_Handler     USART1_RX_IRQHandler
                Set_Default_Handler     USART1_TX_IRQHandler
                Set_Default_Handler     LEUART0_IRQHandler
                Set_Default_Handler     PCNT0_IRQHandler
                Set_Default_Handler     RTC_IRQHandler
                Set_Default_Handler     CMU_IRQHandler
                Set_Default_Handler     VCMP_IRQHandler
                Set_Default_Handler     MSC_IRQHandler
                Set_Default_Handler     AES_IRQHandler

                .end
