;/*
; * Copyright (c) 2009-2019 Arm Limited
; *
; * Licensed under the Apache License, Version 2.0 (the "License");
; * you may not use this file except in compliance with the License.
; * You may obtain a copy of the License at
; *
; *     http://www.apache.org/licenses/LICENSE-2.0
; *
; * Unless required by applicable law or agreed to in writing, software
; * distributed under the License is distributed on an "AS IS" BASIS,
; * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
; * See the License for the specific language governing permissions and
; * limitations under the License.
; *
; *
; * This file is derivative of CMSIS V5.00 startup_ARMCM33.S
; */

    .syntax    unified
    .arch    armv8-m.main

    .section .vectors
    .align 2
    .globl    __Vectors
__Vectors:
    .long    Image$$ARM_LIB_STACK_MSP$$ZI$$Limit   /* Top of Stack */

    /* Core interrupts */
    .long    Reset_Handler                  /* Reset Handler */
    .long    NMI_Handler                    /* NMI Handler */
    .long    HardFault_Handler              /* Hard Fault Handler */
    .long    MemManage_Handler              /* MPU Fault Handler */
    .long    BusFault_Handler               /* Bus Fault Handler */
    .long    UsageFault_Handler             /* Usage Fault Handler */
    .long    SecureFault_Handler            /* Secure Fault Handler */
    .long    0                              /* Reserved */
    .long    0                              /* Reserved */
    .long    0                              /* Reserved */
    .long    SVC_Handler                    /* SVCall Handler */
    .long    DebugMon_Handler               /* Debug Monitor Handler */
    .long    0                              /* Reserved */
    .long    PendSV_Handler                 /* PendSV Handler */
    .long    SysTick_Handler                /* SysTick Handler */

    /* External interrupts */
    .long    NONSEC_WATCHDOG_RESET_Handler  /* 0: Non-Secure Watchdog Reset Handler */
    .long    NONSEC_WATCHDOG_Handler        /* 1: Non-Secure Watchdog Handler */
    .long    S32K_TIMER_Handler             /* 2: S32K Timer Handler */
    .long    TFM_TIMER0_IRQ_Handler         /* 3: TIMER 0 Handler */
    .long    TIMER1_Handler                 /* 4: TIMER 1 Handler */
    .long    DUALTIMER_Handler              /* 5: Dual Timer Handler */
    .long    MHU0_Handler                   /* 6: Message Handling Unit 0 */
    .long    MHU1_Handler                   /* 7: Message Handling Unit 1 */
    .long    0                              /* 8: Reserved */
    .long    MPC_Handler                    /* 9: MPC Combined (Secure) Handler */
    .long    PPC_Handler                    /* 10: PPC Combined (Secure) Handler */
    .long    MSC_Handler                    /* 11: MSC Combined (Secure) Handler */
    .long    BRIDGE_ERROR_Handler           /* 12: Bridge Error Combined (Secure) Handler */
    .long    0                              /* 13: Reserved */
    .long    0                              /* 14: Reserved */
    .long    0                              /* 15: Reserved */
    .long    0                              /* 16: Reserved */
    .long    0                              /* 17: Reserved */
    .long    0                              /* 18: Reserved */
    .long    0                              /* 19: Reserved */
    .long    0                              /* 20: Reserved */
    .long    0                              /* 21: Reserved */
    .long    0                              /* 22: Reserved */
    .long    0                              /* 23: Reserved */
    .long    0                              /* 24: Reserved */
    .long    0                              /* 25: Reserved */
    .long    0                              /* 26: Reserved */
    .long    0                              /* 27: Reserved */
    .long    0                              /* 28: Reserved */
    .long    0                              /* 29: Reserved */
    .long    0                              /* 30: Reserved */
    .long    0                              /* 31: Reserved */
    .long    UARTRX0_Handler                /* 32: UART 0 RX Handler */
    .long    UARTTX0_Handler                /* 33: UART 0 TX Handler */
    .long    0                              /* 34: Reserved */
    .long    0                              /* 35: Reserved */
    .long    0                              /* 36: Reserved */
    .long    0                              /* 37: Reserved */
    .long    0                              /* 38: Reserved */
    .long    0                              /* 39: Reserved */
    .long    0                              /* 40: Reserved */
    .long    0                              /* 41: Reserved */
    .long    UART0_Handler                  /* 42: UART 0 combined Handler */
    .long    0                              /* 43: Reserved */
    .long    0                              /* 44: Reserved */
    .long    0                              /* 45: Reserved */
    .long    0                              /* 46: Reserved */
    .long    UARTOVF_Handler                /* 47: UART 0 Overflow Handler */
    .long    0                              /* 48: Reserved */
    .long    0                              /* 49: Reserved */
    .long    0                              /* 50: Reserved */
    .long    0                              /* 51: Reserved */
    .long    0                              /* 52: Reserved */
    .long    0                              /* 53: Reserved */
    .long    0                              /* 54: Reserved */
    .long    0                              /* 55: Reserved */
    .long    DMA0_ERROR_Handler             /* 56: DMA 0 Error Handler */
    .long    DMA0_TC_Handler                /* 57: DMA 0 Terminal Count Handler */
    .long    DMA0_Handler                   /* 58: DMA 0 Combined Handler */
    .long    DMA1_ERROR_Handler             /* 59: DMA 1 Error Handler */
    .long    DMA1_TC_Handler                /* 60: DMA 1 Terminal Count Handler */
    .long    DMA1_Handler                   /* 61: DMA 1 Combined Handler */
    .long    DMA2_ERROR_Handler             /* 62: DMA 2 Error Handler */
    .long    DMA2_TC_Handler                /* 63: DMA 2 Terminal Count Handler */
    .long    DMA2_Handler                   /* 64: DMA 2 Combined Handler */
    .long    DMA3_ERROR_Handler             /* 65: DMA 3 Error Handler */
    .long    DMA3_TC_Handler                /* 66: DMA 3 Terminal Count Handler */
    .long    DMA3_Handler                   /* 67: DMA 3 Combined Handler */
    .long    0                              /* 68: Reserved */
    .long    0                              /* 69: Reserved */
    .long    0                              /* 70: Reserved */
    .long    0                              /* 71: Reserved */
    .long    0                              /* 72: Reserved */
    .long    0                              /* 73: Reserved */
    .long    0                              /* 74: Reserved */
    .long    0                              /* 75: Reserved */
    .long    0                              /* 76: Reserved */
    .long    0                              /* 77: Reserved */
    .long    0                              /* 78: Reserved */
    .long    0                              /* 79: Reserved */
    .long    0                              /* 80: Reserved */
    .long    0                              /* 81: Reserved */
    .long    0                              /* 82: Reserved */
    .long    0                              /* 83: Reserved */
    .long    0                              /* 84: Reserved */
    .long    0                              /* 85: Reserved */
    .long    0                              /* 86: Reserved */
    .long    0                              /* 87: Reserved */
    .long    0                              /* 88: Reserved */
    .long    0                              /* 89: Reserved */
    .long    0                              /* 90: Reserved */
    .long    0                              /* 91: Reserved */
    .long    0                              /* 92: Reserved */
    .long    0                              /* 93: Reserved */
    .long    0                              /* 94: Reserved */
    .long    0                              /* 95: Reserved */

    .size    __Vectors, . - __Vectors

    .text
    .thumb
    .thumb_func
    .align    2
    .globl    Reset_Handler
    .type    Reset_Handler, %function
Reset_Handler:
/*  Firstly it copies data from read only memory to RAM. There are two schemes
 *  to copy. One can copy more than one sections. Another can only copy
 *  one section.  The former scheme needs more instructions and read-only
 *  data to implement than the latter.
 *  Macro __STARTUP_COPY_MULTIPLE is used to choose between two schemes.  */

#ifdef __STARTUP_COPY_MULTIPLE
/*  Multiple sections scheme.
 *
 *  Between symbol address __copy_table_start__ and __copy_table_end__,
 *  there are array of triplets, each of which specify:
 *    offset 0: LMA of start of a section to copy from
 *    offset 4: VMA of start of a section to copy to
 *    offset 8: size of the section to copy. Must be multiply of 4
 *
 *  All addresses must be aligned to 4 bytes boundary.
 */
    ldr    r4, =__copy_table_start__
    ldr    r5, =__copy_table_end__

.L_loop0:
    cmp    r4, r5
    bge    .L_loop0_done
    ldr    r1, [r4]
    ldr    r2, [r4, #4]
    ldr    r3, [r4, #8]

.L_loop0_0:
    subs    r3, #4
    ittt    ge
    ldrge    r0, [r1, r3]
    strge    r0, [r2, r3]
    bge    .L_loop0_0

    adds    r4, #12
    b    .L_loop0

.L_loop0_done:
#else
/*  Single section scheme.
 *
 *  The ranges of copy from/to are specified by following symbols
 *    __etext: LMA of start of the section to copy from. Usually end of text
 *    __data_start__: VMA of start of the section to copy to
 *    __data_end__: VMA of end of the section to copy to
 *
 *  All addresses must be aligned to 4 bytes boundary.
 */
    ldr    r1, =__etext
    ldr    r2, =__data_start__
    ldr    r3, =__data_end__

.L_loop1:
    cmp    r2, r3
    ittt    lt
    ldrlt    r0, [r1], #4
    strlt    r0, [r2], #4
    blt    .L_loop1
#endif /*__STARTUP_COPY_MULTIPLE */

/*  This part of work usually is done in C library startup code. Otherwise,
 *  define this macro to enable it in this startup.
 *
 *  There are two schemes too. One can clear multiple BSS sections. Another
 *  can only clear one section. The former is more size expensive than the
 *  latter.
 *
 *  Define macro __STARTUP_CLEAR_BSS_MULTIPLE to choose the former.
 *  Otherwise efine macro __STARTUP_CLEAR_BSS to choose the later.
 */
#ifdef __STARTUP_CLEAR_BSS_MULTIPLE
/*  Multiple sections scheme.
 *
 *  Between symbol address __copy_table_start__ and __copy_table_end__,
 *  there are array of tuples specifying:
 *    offset 0: Start of a BSS section
 *    offset 4: Size of this BSS section. Must be multiply of 4
 */
    ldr    r3, =__zero_table_start__
    ldr    r4, =__zero_table_end__

.L_loop2:
    cmp    r3, r4
    bge    .L_loop2_done
    ldr    r1, [r3]
    ldr    r2, [r3, #4]
    movs    r0, 0

.L_loop2_0:
    subs    r2, #4
    itt    ge
    strge    r0, [r1, r2]
    bge    .L_loop2_0

    adds    r3, #8
    b    .L_loop2
.L_loop2_done:
#elif defined (__STARTUP_CLEAR_BSS)
/*  Single BSS section scheme.
 *
 *  The BSS section is specified by following symbols
 *    __bss_start__: start of the BSS section.
 *    __bss_end__: end of the BSS section.
 *
 *  Both addresses must be aligned to 4 bytes boundary.
 */
    ldr    r1, =__bss_start__
    ldr    r2, =__bss_end__

    movs    r0, 0
.L_loop3:
    cmp    r1, r2
    itt    lt
    strlt    r0, [r1], #4
    blt    .L_loop3
#endif /* __STARTUP_CLEAR_BSS_MULTIPLE || __STARTUP_CLEAR_BSS */

    cpsid   i              /* Disable IRQs */
    bl    SystemInit

    mrs     r0, control    /* Get control value */
    orr     r0, r0, #2     /* Select switch to PSP */
    msr     control, r0
    ldr     r0, =Image$$ARM_LIB_STACK$$ZI$$Limit
    msr     psp, r0

#ifndef __START
#define __START _start
#endif
    bl    __START

    .pool
    .size    Reset_Handler, . - Reset_Handler


/*  Macro to define default handlers. */
    .macro    def_irq_handler    handler_name
    .align    1
    .thumb_func
    .weak    \handler_name
    \handler_name:
    b        \handler_name
    .endm

    /* Core interrupts */
    def_irq_handler      NMI_Handler
    def_irq_handler      HardFault_Handler
    def_irq_handler      MemManage_Handler
    def_irq_handler      BusFault_Handler
    def_irq_handler      UsageFault_Handler
    def_irq_handler      SecureFault_Handler
    def_irq_handler      SVC_Handler
    def_irq_handler      DebugMon_Handler
    def_irq_handler      PendSV_Handler
    def_irq_handler      SysTick_Handler

    /* External interrupts */
    def_irq_handler     NONSEC_WATCHDOG_RESET_Handler  /* 0: Non-Secure Watchdog Reset Handler */
    def_irq_handler     NONSEC_WATCHDOG_Handler        /* 1: Non-Secure Watchdog Handler */
    def_irq_handler     S32K_TIMER_Handler             /* 2: S32K Timer Handler */
    def_irq_handler     TFM_TIMER0_IRQ_Handler         /* 3: TIMER 0 Handler */
    def_irq_handler     TIMER1_Handler                 /* 4: TIMER 1 Handler */
    def_irq_handler     DUALTIMER_Handler              /* 5: Dual Timer Handler */
    def_irq_handler     MHU0_Handler                   /* 6: Message Handling Unit 0 */
    def_irq_handler     MHU1_Handler                   /* 7: Message Handling Unit 1 */
    def_irq_handler     MPC_Handler                    /* 9 MPC Combined (Secure) Handler */
    def_irq_handler     PPC_Handler                    /* 10 PPC Combined (Secure) Handler */
    def_irq_handler     MSC_Handler                    /* 11 MSC Combined (Secure) Handler */
    def_irq_handler     BRIDGE_ERROR_Handler           /* 12 Bridge Error Combined (Secure) Handler */
    def_irq_handler     UARTRX0_Handler                /* 32: UART 0 RX Handler */
    def_irq_handler     UARTTX0_Handler                /* 33: UART 0 TX Handler */
    def_irq_handler     UART0_Handler                  /* 42: UART 0 combined Handler */
    def_irq_handler     UARTOVF_Handler                /* 47: UART 0 Overflow Handler */
    def_irq_handler     DMA0_ERROR_Handler             /* 56: DMA 0 Error Handler */
    def_irq_handler     DMA0_TC_Handler                /* 57: DMA 0 Terminal Count Handler */
    def_irq_handler     DMA0_Handler                   /* 58: DMA 0 Combined Handler */
    def_irq_handler     DMA1_ERROR_Handler             /* 59: DMA 1 Error Handler */
    def_irq_handler     DMA1_TC_Handler                /* 60: DMA 1 Terminal Count Handler */
    def_irq_handler     DMA1_Handler                   /* 61: DMA 1 Combined Handler */
    def_irq_handler     DMA2_ERROR_Handler             /* 62: DMA 2 Error Handler */
    def_irq_handler     DMA2_TC_Handler                /* 63: DMA 2 Terminal Count Handler */
    def_irq_handler     DMA2_Handler                   /* 64: DMA 2 Combined Handler */
    def_irq_handler     DMA3_ERROR_Handler             /* 65: DMA 3 Error Handler */
    def_irq_handler     DMA3_TC_Handler                /* 66: DMA 3 Terminal Count Handler */
    def_irq_handler     DMA3_Handler                   /* 67: DMA 3 Combined Handler */

    .end
