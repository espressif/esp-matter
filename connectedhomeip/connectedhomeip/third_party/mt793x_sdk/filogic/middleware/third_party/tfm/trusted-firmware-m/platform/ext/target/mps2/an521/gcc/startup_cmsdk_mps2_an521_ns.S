;/*
; * Copyright (c) 2009-2020 ARM Limited
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
    .long    0                              /* Reserved */
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
    .long    TIMER0_Handler                 /* 3: TIMER 0 Handler */
    .long    TIMER1_Handler                 /* 4: TIMER 1 Handler */
    .long    DUALTIMER_Handler              /* 5: Dual Timer Handler */
    .long    MHU0_Handler                   /* 6: Message Handling Unit 0 */
    .long    MHU1_Handler                   /* 7: Message Handling Unit 1 */
    .long    0                              /* 8: Reserved */
    .long    0                              /* 9: Reserved */
    .long    0                              /* 10: Reserved */
    .long    MSC_Handler                    /* 11: MSC Combined (Secure) Handler */
    .long    BRIDGE_ERROR_Handler           /* 12: Bridge Error Combined (Secure) Handler */
    .long    INVALID_INSTR_CACHE_Handler    /* 13: CPU Instruction Cache Invalidation Handler */
    .long    0                              /* 14: Reserved */
    .long    SYS_PPU_Handler                /* 15: SYS PPU Handler */
    .long    CPU0_PPU_Handler               /* 16: CPU0 PPU Handler */
    .long    CPU1_PPU_Handler               /* 17: CPU1 PPU Handler */
    .long    CPU0_DBG_PPU_Handler           /* 18: CPU0 DBG PPU_Handler */
    .long    CPU1_DBG_PPU_Handler           /* 19: CPU1 DBG PPU_Handler */
    .long    CRYPT_PPU_Handler              /* 20: CRYPT PPU Handler */
    .long    0                              /* 21: Reserved */
    .long    RAM0_PPU_Handler               /* 22: RAM0 PPU Handler */
    .long    RAM1_PPU_Handler               /* 23: RAM1 PPU Handler */
    .long    RAM2_PPU_Handler               /* 24: RAM2 PPU Handler */
    .long    RAM3_PPU_Handler               /* 25: RAM3 PPU Handler */
    .long    DBG_PPU_Handler                /* 26: DBG PPU Handler */
    .long    0                              /* 27: Reserved */
    .long    CPU0_CTI_Handler               /* 28: CPU0 CTI Handler */
    .long    CPU1_CTI_Handler               /* 29: CPU1 CTI Handler */
    .long    0                              /* 30: Reserved */
    .long    0                              /* 31: Reserved */
    .long    UARTRX0_Handler                /* 32: UART 0 RX Handler */
    .long    UARTTX0_Handler                /* 33: UART 0 TX Handler */
    .long    UARTRX1_Handler                /* 34: UART 1 RX Handler */
    .long    UARTTX1_Handler                /* 35: UART 1 TX Handler */
    .long    UARTRX2_Handler                /* 36: UART 2 RX Handler */
    .long    UARTTX2_Handler                /* 37: UART 2 TX Handler */
    .long    UARTRX3_Handler                /* 38: UART 3 RX Handler */
    .long    UARTTX3_Handler                /* 39: UART 3 TX Handler */
    .long    UARTRX4_Handler                /* 40: UART 4 RX Handler */
    .long    UARTTX4_Handler                /* 41: UART 4 TX Handler */
    .long    UART0_Handler                  /* 42: UART 0 combined Handler */
    .long    UART1_Handler                  /* 43: UART 1 combined Handler */
    .long    UART2_Handler                  /* 44: UART 2 combined Handler */
    .long    UART3_Handler                  /* 45: UART 3 combined Handler */
    .long    UART4_Handler                  /* 46: UART 4 combined Handler */
    .long    UARTOVF_Handler                /* 47: UART 0,1,2,3,4 Overflow Handler */
    .long    ETHERNET_Handler               /* 48: Ethernet Handler */
    .long    I2S_Handler                    /* 49: I2S Handler */
    .long    TSC_Handler                    /* 50: Touch Screen Handler */
    .long    SPI0_Handler                   /* 51: SPI 0 Handler */
    .long    SPI1_Handler                   /* 52: SPI 1 Handler */
    .long    SPI2_Handler                   /* 53: SPI 2 Handler */
    .long    SPI3_Handler                   /* 54: SPI 3 Handler */
    .long    SPI4_Handler                   /* 55: SPI 4 Handler */
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
    .long    GPIO0_Handler                  /* 68: GPIO 0 Combined Handler */
    .long    GPIO1_Handler                  /* 69: GPIO 1 Combined Handler */
    .long    GPIO2_Handler                  /* 70: GPIO 2 Combined Handler */
    .long    GPIO3_Handler                  /* 71: GPIO 3 Combined Handler */
    .long    GPIO0_0_Handler                /* 72: GPIO 0_0 Handler */
    .long    GPIO0_1_Handler                /* 73: GPIO 0_1 Handler */
    .long    GPIO0_2_Handler                /* 74: GPIO 0_2 Handler */
    .long    GPIO0_3_Handler                /* 75: GPIO 0_3 Handler */
    .long    GPIO0_4_Handler                /* 76: GPIO 0_4 Handler */
    .long    GPIO0_5_Handler                /* 77: GPIO 0_5 Handler */
    .long    GPIO0_6_Handler                /* 78: GPIO 0_6 Handler */
    .long    GPIO0_7_Handler                /* 79: GPIO 0_7 Handler */
    .long    GPIO0_8_Handler                /* 80: GPIO 0_8 Handler */
    .long    GPIO0_9_Handler                /* 81: GPIO 0_9 Handler */
    .long    GPIO0_10_Handler               /* 82: GPIO 0_10 Handler */
    .long    GPIO0_11_Handler               /* 83: GPIO 0_11 Handler */
    .long    GPIO0_12_Handler               /* 84: GPIO 0_12 Handler */
    .long    GPIO0_13_Handler               /* 85: GPIO 0_13 Handler */
    .long    GPIO0_14_Handler               /* 86: GPIO 0_14 Handler */
    .long    GPIO0_15_Handler               /* 87: GPIO 0_15 Handler */
    .long    GPIO1_0_Handler                /* 88: GPIO 1_0 Handler */
    .long    GPIO1_1_Handler                /* 89: GPIO 1_1 Handler */
    .long    GPIO1_2_Handler                /* 90: GPIO 1_2 Handler */
    .long    GPIO1_3_Handler                /* 91: GPIO 1_3 Handler */
    .long    GPIO1_4_Handler                /* 92: GPIO 1_4 Handler */
    .long    GPIO1_5_Handler                /* 93: GPIO 1_5 Handler */
    .long    GPIO1_6_Handler                /* 94: GPIO 1_6 Handler */
    .long    GPIO1_7_Handler                /* 95: GPIO 1_7 Handler */

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
    def_irq_handler     NMI_Handler
    def_irq_handler     HardFault_Handler
    def_irq_handler     MemManage_Handler
    def_irq_handler     BusFault_Handler
    def_irq_handler     UsageFault_Handler
    def_irq_handler     SVC_Handler
    def_irq_handler     DebugMon_Handler
    def_irq_handler     PendSV_Handler
    def_irq_handler     SysTick_Handler

    /* External interrupts */
    def_irq_handler     NONSEC_WATCHDOG_RESET_Handler  /* 0: Non-Secure Watchdog Reset Handler */
    def_irq_handler     NONSEC_WATCHDOG_Handler        /* 1: Non-Secure Watchdog Handler */
    def_irq_handler     S32K_TIMER_Handler             /* 2: S32K Timer Handler */
    def_irq_handler     TIMER0_Handler                 /* 3: TIMER 0 Handler */
    def_irq_handler     TIMER1_Handler                 /* 4: TIMER 1 Handler */
    def_irq_handler     DUALTIMER_Handler              /* 5: Dual Timer Handler */
    def_irq_handler     MHU0_Handler                   /* 6: Message Handling Unit 0 */
    def_irq_handler     MHU1_Handler                   /* 7: Message Handling Unit 1 */
    def_irq_handler     MSC_Handler                    /* 11 MSC Combined (Secure) Handler */
    def_irq_handler     BRIDGE_ERROR_Handler           /* 12 Bridge Error Combined (Secure) Handler */
    def_irq_handler     INVALID_INSTR_CACHE_Handler    /* 13 CPU Instruction Cache Invalidation Handler */
    def_irq_handler     SYS_PPU_Handler                /* 15 SYS PPU Handler */
    def_irq_handler     CPU0_PPU_Handler               /* 16 CPU0 PPU Handler */
    def_irq_handler     CPU1_PPU_Handler               /* 17 CPU1 PPU Handler */
    def_irq_handler     CPU0_DBG_PPU_Handler           /* 18 CPU0 DBG PPU_Handler */
    def_irq_handler     CPU1_DBG_PPU_Handler           /* 19 CPU1 DBG PPU_Handler */
    def_irq_handler     CRYPT_PPU_Handler              /* 20 CRYPT PPU Handler */
    def_irq_handler     RAM0_PPU_Handler               /* 22 RAM0 PPU Handler */
    def_irq_handler     RAM1_PPU_Handler               /* 23 RAM1 PPU Handler */
    def_irq_handler     RAM2_PPU_Handler               /* 24 RAM2 PPU Handler */
    def_irq_handler     RAM3_PPU_Handler               /* 25 RAM3 PPU Handler */
    def_irq_handler     DBG_PPU_Handler                /* 26 DBG PPU Handler */
    def_irq_handler     CPU0_CTI_Handler               /* 28: CPU0 CTI Handler */
    def_irq_handler     CPU1_CTI_Handler               /* 29: CPU1 CTI Handler */
    def_irq_handler     UARTRX0_Handler                /* 32: UART 0 RX Handler */
    def_irq_handler     UARTTX0_Handler                /* 33: UART 0 TX Handler */
    def_irq_handler     UARTRX1_Handler                /* 34: UART 1 RX Handler */
    def_irq_handler     UARTTX1_Handler                /* 35: UART 1 TX Handler */
    def_irq_handler     UARTRX2_Handler                /* 36: UART 2 RX Handler */
    def_irq_handler     UARTTX2_Handler                /* 37: UART 2 TX Handler */
    def_irq_handler     UARTRX3_Handler                /* 38: UART 3 RX Handler */
    def_irq_handler     UARTTX3_Handler                /* 39: UART 3 TX Handler */
    def_irq_handler     UARTRX4_Handler                /* 40: UART 4 RX Handler */
    def_irq_handler     UARTTX4_Handler                /* 41: UART 4 TX Handler */
    def_irq_handler     UART0_Handler                  /* 42: UART 0 combined Handler */
    def_irq_handler     UART1_Handler                  /* 43: UART 1 combined Handler */
    def_irq_handler     UART2_Handler                  /* 44: UART 2 combined Handler */
    def_irq_handler     UART3_Handler                  /* 45: UART 3 combined Handler */
    def_irq_handler     UART4_Handler                  /* 46: UART 4 combined Handler */
    def_irq_handler     UARTOVF_Handler                /* 47: UART 0,1,2,3,4 Overflow Handler */
    def_irq_handler     ETHERNET_Handler               /* 48: Ethernet Handler */
    def_irq_handler     I2S_Handler                    /* 49: I2S Handler */
    def_irq_handler     TSC_Handler                    /* 50: Touch Screen Handler */
    def_irq_handler     SPI0_Handler                   /* 51: SPI 0 Handler */
    def_irq_handler     SPI1_Handler                   /* 52: SPI 1 Handler */
    def_irq_handler     SPI2_Handler                   /* 53: SPI 2 Handler */
    def_irq_handler     SPI3_Handler                   /* 54: SPI 3 Handler */
    def_irq_handler     SPI4_Handler                   /* 55: SPI 4 Handler */
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
    def_irq_handler     GPIO0_Handler                  /* 68: GPIO 0 Combined Handler */
    def_irq_handler     GPIO1_Handler                  /* 69: GPIO 1 Combined Handler */
    def_irq_handler     GPIO2_Handler                  /* 70: GPIO 2 Combined Handler */
    def_irq_handler     GPIO3_Handler                  /* 71: GPIO 3 Combined Handler */
    def_irq_handler     GPIO0_0_Handler                /* 72: GPIO 0_0 Handler */
    def_irq_handler     GPIO0_1_Handler                /* 73: GPIO 0_1 Handler */
    def_irq_handler     GPIO0_2_Handler                /* 74: GPIO 0_2 Handler */
    def_irq_handler     GPIO0_3_Handler                /* 75: GPIO 0_3 Handler */
    def_irq_handler     GPIO0_4_Handler                /* 76: GPIO 0_4 Handler */
    def_irq_handler     GPIO0_5_Handler                /* 77: GPIO 0_5 Handler */
    def_irq_handler     GPIO0_6_Handler                /* 78: GPIO 0_6 Handler */
    def_irq_handler     GPIO0_7_Handler                /* 79: GPIO 0_7 Handler */
    def_irq_handler     GPIO0_8_Handler                /* 80: GPIO 0_8 Handler */
    def_irq_handler     GPIO0_9_Handler                /* 81: GPIO 0_9 Handler */
    def_irq_handler     GPIO0_10_Handler               /* 82: GPIO 0_10 Handler */
    def_irq_handler     GPIO0_11_Handler               /* 83: GPIO 0_11 Handler */
    def_irq_handler     GPIO0_12_Handler               /* 84: GPIO 0_12 Handler */
    def_irq_handler     GPIO0_13_Handler               /* 85: GPIO 0_13 Handler */
    def_irq_handler     GPIO0_14_Handler               /* 86: GPIO 0_14 Handler */
    def_irq_handler     GPIO0_15_Handler               /* 87: GPIO 0_15 Handler */
    def_irq_handler     GPIO1_0_Handler                /* 88: GPIO 1_0 Handler */
    def_irq_handler     GPIO1_1_Handler                /* 89: GPIO 1_1 Handler */
    def_irq_handler     GPIO1_2_Handler                /* 90: GPIO 1_2 Handler */
    def_irq_handler     GPIO1_3_Handler                /* 91: GPIO 1_3 Handler */
    def_irq_handler     GPIO1_4_Handler                /* 92: GPIO 1_4 Handler */
    def_irq_handler     GPIO1_5_Handler                /* 93: GPIO 1_5 Handler */
    def_irq_handler     GPIO1_6_Handler                /* 94: GPIO 1_6 Handler */
    def_irq_handler     GPIO1_7_Handler                /* 95: GPIO 1_7 Handler */

    .end
