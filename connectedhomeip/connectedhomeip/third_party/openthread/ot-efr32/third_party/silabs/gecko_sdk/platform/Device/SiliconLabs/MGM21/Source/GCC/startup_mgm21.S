/**************************************************************************//**
 * @file     startup_mgm21.S
 * @brief    CMSIS-Core(M) Device Startup File for
 *           Device MGM21
 *           Device MGM21
 * @version  V2.3.0
 * @date     26. May 2021
 ******************************************************************************
 ******************************************************************************
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
 *****************************************************************************/
/*
 * Copyright (c) 2009-2021 Arm Limited. All rights reserved.
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

                .syntax  unified
                .arch    armv8-m.main


                .section    .stack
                .align      9
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

                #define __INITIAL_SP     __StackTop
                #if defined(SL_TRUSTZONE_SECURE)
                #define __STACK_LIMIT    __StackLimit
                #if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
                #define __STACK_SEAL     __StackSeal
                #endif
                #endif // SL_TRUSTZONE_SECURE

                .section .vectors
                .align   2
                .globl   __Vectors
                .globl   __Vectors_End
                .globl   __Vectors_Size
__Vectors:
                .long    __INITIAL_SP                       /*     Initial Stack Pointer */
                .long    Reset_Handler                      /*     Reset Handler */
                .long    NMI_Handler                        /* -14 NMI Handler */
                .long    HardFault_Handler                  /* -13 Hard Fault Handler */
                .long    MemManage_Handler                  /* -12 MPU Fault Handler */
                .long    BusFault_Handler                   /* -11 Bus Fault Handler */
                .long    UsageFault_Handler                 /* -10 Usage Fault Handler */
                .long    SecureFault_Handler                /* -9  Secure Fault Handler */
                .long    Default_Handler                    /*     Reserved */
                .long    Default_Handler                    /*     Reserved */
                .long    Default_Handler                    /*     Reserved */
                .long    SVC_Handler                        /*  -5 SVC Handler */
                .long    DebugMon_Handler                   /*  -4 Debug Monitor Handler */
                .long    sl_app_properties                  /*     Application properties */
                .long    PendSV_Handler                     /*  -2 PendSV Handler */
                .long    SysTick_Handler                    /*  -1 SysTick Handler */


    /* External interrupts */
                .long    SETAMPERHOST_IRQHandler /* 0 - SETAMPERHOST */
                .long    SEMBRX_IRQHandler       /* 1 - SEMBRX */
                .long    SEMBTX_IRQHandler       /* 2 - SEMBTX */
                .long    SMU_SECURE_IRQHandler   /* 3 - SMU_SECURE */
                .long    SMU_PRIVILEGED_IRQHandler /* 4 - SMU_PRIVILEGED */
                .long    EMU_IRQHandler          /* 5 - EMU */
                .long    TIMER0_IRQHandler       /* 6 - TIMER0 */
                .long    TIMER1_IRQHandler       /* 7 - TIMER1 */
                .long    TIMER2_IRQHandler       /* 8 - TIMER2 */
                .long    TIMER3_IRQHandler       /* 9 - TIMER3 */
                .long    RTCC_IRQHandler         /* 10 - RTCC */
                .long    USART0_RX_IRQHandler    /* 11 - USART0_RX */
                .long    USART0_TX_IRQHandler    /* 12 - USART0_TX */
                .long    USART1_RX_IRQHandler    /* 13 - USART1_RX */
                .long    USART1_TX_IRQHandler    /* 14 - USART1_TX */
                .long    USART2_RX_IRQHandler    /* 15 - USART2_RX */
                .long    USART2_TX_IRQHandler    /* 16 - USART2_TX */
                .long    ICACHE0_IRQHandler      /* 17 - ICACHE0 */
                .long    BURTC_IRQHandler        /* 18 - BURTC */
                .long    LETIMER0_IRQHandler     /* 19 - LETIMER0 */
                .long    SYSCFG_IRQHandler       /* 20 - SYSCFG */
                .long    LDMA_IRQHandler         /* 21 - LDMA */
                .long    LFXO_IRQHandler         /* 22 - LFXO */
                .long    LFRCO_IRQHandler        /* 23 - LFRCO */
                .long    ULFRCO_IRQHandler       /* 24 - ULFRCO */
                .long    GPIO_ODD_IRQHandler     /* 25 - GPIO_ODD */
                .long    GPIO_EVEN_IRQHandler    /* 26 - GPIO_EVEN */
                .long    I2C0_IRQHandler         /* 27 - I2C0 */
                .long    I2C1_IRQHandler         /* 28 - I2C1 */
                .long    EMUDG_IRQHandler        /* 29 - EMUDG */
                .long    EMUSE_IRQHandler        /* 30 - EMUSE */
                .long    AGC_IRQHandler          /* 31 - AGC */
                .long    BUFC_IRQHandler         /* 32 - BUFC */
                .long    FRC_PRI_IRQHandler      /* 33 - FRC_PRI */
                .long    FRC_IRQHandler          /* 34 - FRC */
                .long    MODEM_IRQHandler        /* 35 - MODEM */
                .long    PROTIMER_IRQHandler     /* 36 - PROTIMER */
                .long    RAC_RSM_IRQHandler      /* 37 - RAC_RSM */
                .long    RAC_SEQ_IRQHandler      /* 38 - RAC_SEQ */
                .long    PRORTC_IRQHandler       /* 39 - PRORTC */
                .long    SYNTH_IRQHandler        /* 40 - SYNTH */
                .long    ACMP0_IRQHandler        /* 41 - ACMP0 */
                .long    ACMP1_IRQHandler        /* 42 - ACMP1 */
                .long    WDOG0_IRQHandler        /* 43 - WDOG0 */
                .long    WDOG1_IRQHandler        /* 44 - WDOG1 */
                .long    HFXO00_IRQHandler       /* 45 - HFXO00 */
                .long    HFRCO0_IRQHandler       /* 46 - HFRCO0 */
                .long    HFRCOEM23_IRQHandler    /* 47 - HFRCOEM23 */
                .long    CMU_IRQHandler          /* 48 - CMU */
                .long    AES_IRQHandler          /* 49 - AES */
                .long    IADC_IRQHandler         /* 50 - IADC */
                .long    MSC_IRQHandler          /* 51 - MSC */
                .long    DPLL0_IRQHandler        /* 52 - DPLL0 */
                .long    SW0_IRQHandler          /* 53 - SW0 */
                .long    SW1_IRQHandler          /* 54 - SW1 */
                .long    SW2_IRQHandler          /* 55 - SW2 */
                .long    SW3_IRQHandler          /* 56 - SW3 */
                .long    KERNEL0_IRQHandler      /* 57 - KERNEL0 */
                .long    KERNEL1_IRQHandler      /* 58 - KERNEL1 */
                .long    M33CTI0_IRQHandler      /* 59 - M33CTI0 */
                .long    M33CTI1_IRQHandler      /* 60 - M33CTI1 */

/* calculate the empty space according the used Cortex-Core */
                .space   (419 * 4)  /* Interrupts 61 .. 480 are left out */
__Vectors_End:
                .equ     __Vectors_Size, __Vectors_End - __Vectors
                .size    __Vectors, . - __Vectors


                .thumb
                .section .text
                .align   2

                .thumb_func
                .type    Reset_Handler, %function
                .globl   Reset_Handler
                .fnstart
Reset_Handler:
                #if defined (SL_TRUSTZONE_SECURE)
                ldr      r0, =__STACK_LIMIT
                msr      msplim, r0

                #if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
                ldr      r0, =__STACK_SEAL
                ldr      r1, =0xFEF5EDA5U
                strd     r1,r1,[r0,#0]
                #endif
                #endif // SL_TRUSTZONE_SECURE

                #ifndef __NO_SYSTEM_INIT
                bl       SystemInit
                #endif

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
                subs     r3, #4                  /* decrement byte count */
                ittt     ge
                ldrge    r0, [r1, r3]
                strge    r0, [r2, r3]
                bge      .L_loop0_0

                adds     r4, #12
                b        .L_loop0
.L_loop0_done:

                ldr      r3, =__zero_table_start__
                ldr      r4, =__zero_table_end__

.L_loop2:
                cmp      r3, r4
                bge      .L_loop2_done
                ldr      r1, [r3]                /* destination address */
                ldr      r2, [r3, #4]            /* word count */
                lsls     r2, r2, #2              /* byte count */
                movs     r0, 0

.L_loop2_0:
                subs     r2, #4                  /* decrement byte count */
                itt      ge
                strge    r0, [r1, r2]
                bge      .L_loop2_0

                adds     r3, #8
                b        .L_loop2
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
                b        .
                .fnend
                .size    Default_Handler, . - Default_Handler

/* Macro to define default exception/interrupt handlers.
 * Default handler are weak symbols with an endless loop.
 * They can be overwritten by real handlers.
 */
                .macro   Set_Default_Handler  Handler_Name
                .weak    \Handler_Name
                .set     \Handler_Name, Default_Handler
                .endm
                Set_Default_Handler  NMI_Handler
                Set_Default_Handler  MemManage_Handler
                Set_Default_Handler  BusFault_Handler
                Set_Default_Handler  UsageFault_Handler
                Set_Default_Handler  SecureFault_Handler
                Set_Default_Handler  SVC_Handler
                Set_Default_Handler  DebugMon_Handler
                Set_Default_Handler  sl_app_properties
                Set_Default_Handler  PendSV_Handler
                Set_Default_Handler  SysTick_Handler
                Set_Default_Handler  SETAMPERHOST_IRQHandler
                Set_Default_Handler  SEMBRX_IRQHandler
                Set_Default_Handler  SEMBTX_IRQHandler
                Set_Default_Handler  SMU_SECURE_IRQHandler
                Set_Default_Handler  SMU_PRIVILEGED_IRQHandler
                Set_Default_Handler  EMU_IRQHandler
                Set_Default_Handler  TIMER0_IRQHandler
                Set_Default_Handler  TIMER1_IRQHandler
                Set_Default_Handler  TIMER2_IRQHandler
                Set_Default_Handler  TIMER3_IRQHandler
                Set_Default_Handler  RTCC_IRQHandler
                Set_Default_Handler  USART0_RX_IRQHandler
                Set_Default_Handler  USART0_TX_IRQHandler
                Set_Default_Handler  USART1_RX_IRQHandler
                Set_Default_Handler  USART1_TX_IRQHandler
                Set_Default_Handler  USART2_RX_IRQHandler
                Set_Default_Handler  USART2_TX_IRQHandler
                Set_Default_Handler  ICACHE0_IRQHandler
                Set_Default_Handler  BURTC_IRQHandler
                Set_Default_Handler  LETIMER0_IRQHandler
                Set_Default_Handler  SYSCFG_IRQHandler
                Set_Default_Handler  LDMA_IRQHandler
                Set_Default_Handler  LFXO_IRQHandler
                Set_Default_Handler  LFRCO_IRQHandler
                Set_Default_Handler  ULFRCO_IRQHandler
                Set_Default_Handler  GPIO_ODD_IRQHandler
                Set_Default_Handler  GPIO_EVEN_IRQHandler
                Set_Default_Handler  I2C0_IRQHandler
                Set_Default_Handler  I2C1_IRQHandler
                Set_Default_Handler  EMUDG_IRQHandler
                Set_Default_Handler  EMUSE_IRQHandler
                Set_Default_Handler  AGC_IRQHandler
                Set_Default_Handler  BUFC_IRQHandler
                Set_Default_Handler  FRC_PRI_IRQHandler
                Set_Default_Handler  FRC_IRQHandler
                Set_Default_Handler  MODEM_IRQHandler
                Set_Default_Handler  PROTIMER_IRQHandler
                Set_Default_Handler  RAC_RSM_IRQHandler
                Set_Default_Handler  RAC_SEQ_IRQHandler
                Set_Default_Handler  PRORTC_IRQHandler
                Set_Default_Handler  SYNTH_IRQHandler
                Set_Default_Handler  ACMP0_IRQHandler
                Set_Default_Handler  ACMP1_IRQHandler
                Set_Default_Handler  WDOG0_IRQHandler
                Set_Default_Handler  WDOG1_IRQHandler
                Set_Default_Handler  HFXO00_IRQHandler
                Set_Default_Handler  HFRCO0_IRQHandler
                Set_Default_Handler  HFRCOEM23_IRQHandler
                Set_Default_Handler  CMU_IRQHandler
                Set_Default_Handler  AES_IRQHandler
                Set_Default_Handler  IADC_IRQHandler
                Set_Default_Handler  MSC_IRQHandler
                Set_Default_Handler  DPLL0_IRQHandler
                Set_Default_Handler  SW0_IRQHandler
                Set_Default_Handler  SW1_IRQHandler
                Set_Default_Handler  SW2_IRQHandler
                Set_Default_Handler  SW3_IRQHandler
                Set_Default_Handler  KERNEL0_IRQHandler
                Set_Default_Handler  KERNEL1_IRQHandler
                Set_Default_Handler  M33CTI0_IRQHandler
                Set_Default_Handler  M33CTI1_IRQHandler

                .end