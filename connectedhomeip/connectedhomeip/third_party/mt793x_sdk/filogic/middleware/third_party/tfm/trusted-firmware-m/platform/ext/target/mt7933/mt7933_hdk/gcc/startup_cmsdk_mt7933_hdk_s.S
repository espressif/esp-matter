;/*
; * Copyright (c) 2009-2018 ARM Limited
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
    /* aligned mt7933.h interrupt definitions */
    .long    WIC_INT_IRQn                        /* 0 */
    .long    RESERVED1_IRQn                      /* 1 */
    .long    RESERVED2_IRQn                      /* 2 */
    .long    WDT_B0_IRQn                         /* 3 */
    .long    UART_IRQn                           /* 4 */
    .long    INFRA_BUS_IRQn                      /* 5 */
    .long    CDBGPWRUPREQ_IRQn                   /* 6 */
    .long    CDBGPWRUPACK_IRQn                   /* 7 */
    .long    WDT_B1_IRQn                         /* 8 */
    .long    DSP_TO_CM33_IRQn                    /* 9 */
    .long    APXGPT0_IRQn                        /* 10 */
    .long    APXGPT1_IRQn                        /* 11 */
    .long    APXGPT2_IRQn                        /* 12 */
    .long    APXGPT3_IRQn                        /* 13 */
    .long    APXGPT4_IRQn                        /* 14 */
    .long    APXGPT5_IRQn                        /* 15 */
    .long    DEVAPC_INFRA_AON_SECURE_VIO_IRQn_Handler    /* 16 */
    .long    DEVAPC_AUD_BUS_SECURE_VIO_IRQn_Handler      /* 17 */
    .long    CONN_AP_BUS_REQ_RISE_IRQn           /* 18 */
    .long    CONN_AP_BUS_REQ_FULL_IRQn           /* 19 */
    .long    CONN_APSRC_REQ_RISE_IRQn            /* 20 */
    .long    CONN_APSRC_REQ_FALL_IRQn            /* 21 */
    .long    CONN_AP_BUS_REQ_HIGH_IRQn           /* 22 */
    .long    CONN_AP_BUS_REQ_LOW_IRQn            /* 23 */
    .long    CONN_APSRC_REQ_HIGH_IRQn            /* 24 */
    .long    CONN_APSRC_REQ_LOW_IRQn             /* 25 */
    .long    INFRA_BUS_TIMEOUT_IRQn              /* 26 */
    .long    CM33_LOCAL_BUS_IRQn                 /* 27 */
    .long    ADSP_INFRA_BUS_TIMEOUT_IRQn         /* 28 */
    .long    RESERVED29_IRQn                     /* 29 */
    .long    RESERVED30_IRQn                     /* 30 */
    .long    RESERVED31_IRQn                     /* 31 */
    .long    DSP_UART_IRQn                       /* 32 */
    .long    TOP_UART0_IRQn                      /* 33 */
    .long    TOP_UART1_IRQn                      /* 34 */
    .long    I2C0_IRQn                           /* 35 */
    .long    I2C1_IRQn                           /* 36 */
    .long    SDCTL_TOP_FW_IRQn                   /* 37 */
    .long    SDCTL_TOP_FW_QOUT_IRQn              /* 38 */
    .long    SPIM0_IRQn                          /* 39 */
    .long    SPIM1_IRQn                          /* 40 */
    .long    SPIS_IRQn                           /* 41 */
    .long    KP_IRQn                             /* 42 */
    .long    IRRX_IRQn                           /* 43 */
    .long    RESERVED44_IRQn                     /* 44 */
    .long    RESERVED45_IRQn                     /* 45 */
    .long    RESERVED46_IRQn                     /* 46 */
    .long    RESERVED47_IRQn                     /* 47 */
    .long    SSUSB_XHCI_IRQn                     /* 48 */
    .long    SSUSB_OTG_IRQn                      /* 49 */
    .long    SSUSB_DEV_IRQn                      /* 50 */
    .long    AFE_MCU_IRQn                        /* 51 */
    .long    RTC_IRQn                            /* 52 */
    .long    SYSRAM_TOP_IRQn                     /* 53 */
    .long    ASIC_MPU_SECURE_VIO_IRQn_Handler       /* 54 */
    .long    ASIC_MPU_PSRAM_SECURE_VIO_IRQn_Handler /* 55 */
    .long    CQDMA0_IRQn                         /* 56 */
    .long    CQDMA1_IRQn                         /* 57 */
    .long    CQDMA2_IRQn                         /* 58 */
    .long    MSDC_IRQn                           /* 59 */
    .long    MSDC_WAKEUP_IRQn                    /* 60 */
    .long    DSP_WDT_IRQn                        /* 61 */
    .long    DSP_TO_CPU_IRQn                     /* 62 */
    .long    APDMA0_IRQn                         /* 63 */
    .long    APDMA1_IRQn                         /* 64 */
    .long    APDMA2_IRQn                         /* 65 */
    .long    APDMA3_IRQn                         /* 66 */
    .long    APDMA4_IRQn                         /* 67 */
    .long    APDMA5_IRQn                         /* 68 */
    .long    APDMA6_IRQn                         /* 69 */
    .long    APDMA7_IRQn                         /* 70 */
    .long    APDMA8_IRQn                         /* 71 */
    .long    APDMA9_IRQn                         /* 72 */
    .long    APDMA10_IRQn                        /* 73 */
    .long    APDMA11_IRQn                        /* 74 */
    .long    BTIF_HOST_IRQn                      /* 75 */
    .long    SF_TOP_IRQn                         /* 76 */
    .long    CONN2AP_WFDMA_IRQn                  /* 77 */
    .long    BGF2AP_WDT_IRQn                     /* 78 */
    .long    BGF2AP_BTIF0_WAKEUP_IRQn            /* 79 */
    .long    CONN2AP_SW_IRQn                     /* 80 */
    .long    BT2AP_ISOCH_IRQn                    /* 81 */
    .long    BT_CVSD_IRQn                        /* 82 */
    .long    CCIF_WF2AP_SW_IRQn                  /* 83 */
    .long    CCIF_BGF2AP_SW_IRQn                 /* 84 */
    .long    WM_CONN2AP_WDT_IRQn                 /* 85 */
    .long    SEMA_RELEASE_INFORM_M2_IRQn         /* 86 */
    .long    SEMA_RELEASE_INFORM_M3_IRQn         /* 87 */
    .long    SEMA_M2_TIMEOUT_IRQn                /* 88 */
    .long    SEMA_M3_TIMEOUT_IRQn                /* 89 */
    .long    CONN_BGF_HIF_ON_HOST_IRQn           /* 90 */
    .long    CONN_GPS_HIF_ON_HOST_IRQn           /* 91 */
    .long    SSUSB_SPM_IRQn                      /* 92 */
    .long    WF2AP_SW_IRQn                       /* 93 */
    .long    CQDMA_SEC_ABORT_IRQn                /* 94 */
    .long    APDMA_SEC_ABORT_IRQn                /* 95 */
    .long    SDIO_CMD_IRQn                       /* 96 */
    .long    RESERVED97_IRQn                     /* 97 */
    .long    ADC_COMP_IRQn                       /* 98 */
    .long    ADC_FIFO_IRQn                       /* 99 */
    .long    GCPU_IRQn                           /* 100 */
    .long    ECC_IRQn                            /* 101 */
    .long    TRNG_IRQn                           /* 102 */
    .long    TFM_TIMER0_IRQ_Handler              /* 103, SEJ_APXGPT_IRQn */
    .long    SEJ_WDT_IRQn                        /* 104 */
    .long    RESERVED105_IRQn                    /* 105 */
    .long    RESERVED106_IRQn                    /* 106 */
    .long    RESERVED107_IRQn                    /* 107 */
    .long    GPIO_IRQ0n                          /* 108 */
    .long    GPIO_IRQ1n                          /* 109 */
    .long    GPIO_IRQ2n                          /* 110 */
    .long    GPIO_IRQ3n                          /* 111 */
    .long    GPIO_IRQ4n                          /* 112 */
    .long    GPIO_IRQ5n                          /* 113 */
    .long    GPIO_IRQ6n                          /* 114 */
    .long    GPIO_IRQ7n                          /* 115 */
    .long    GPIO_IRQ8n                          /* 116 */
    .long    GPIO_IRQ9n                          /* 117 */
    .long    GPIO_IRQ10n                         /* 118 */
    .long    GPIO_IRQ11n                         /* 119 */
    .long    GPIO_IRQ12n                         /* 120 */
    .long    GPIO_IRQ13n                         /* 121 */
    .long    GPIO_IRQ14n                         /* 122 */
    .long    GPIO_IRQ15n                         /* 123 */
    .long    GPIO_IRQ16n                         /* 124 */
    .long    GPIO_IRQ17n                         /* 125 */
    .long    GPIO_IRQ18n                         /* 126 */
    .long    GPIO_IRQ19n                         /* 127 */
    .long    GPIO_IRQ20n                         /* 128 */
    .long    GPIO_IRQ21n                         /* 129 */
    .long    GPIO_IRQ22n                         /* 130 */
    .long    GPIO_IRQ23n                         /* 131 */
    .long    GPIO_IRQ24n                         /* 132 */
    .long    GPIO_IRQ25n                         /* 133 */
    .long    GPIO_IRQ26n                         /* 134 */
    .long    GPIO_IRQ27n                         /* 135 */
    .long    GPIO_IRQ28n                         /* 136 */
    .long    GPIO_IRQ29n                         /* 137 */
    .long    GPIO_IRQ30n                         /* 138 */
    .long    CM33_UART_RX_IRQn                   /* 139 */

    .size    __Vectors, . - __Vectors

    .text
    .thumb
    .thumb_func
    .align    2
    .globl    Reset_Handler
    .type    Reset_Handler, %function
Reset_Handler:

Reboot_Check:
    ldr     r3, =0x2106000C
    ldr     r3, [r3]
    lsls    r3, r3, #0x1F
    bpl     Is_coldboot
    ldr     r3, =deep_sleep_return
    ldr     r3, [r3]
    cbz     r3, Is_coldboot

Reboot_Handler:
    msr     msp, r3
    ldr     r0, =Image$$ARM_LIB_STACK_MSP$$ZI$$Base
    msr     msplim, r0
    ldr     r3, =deep_sleep_return
    ldr     r3, [r3, #4]
    bx      r3

Is_coldboot:

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
#ifdef TFM_INIT_PSRAM
/*
   Because the PSRAM is not initialized by bootloader, TFM can't use the PSRAM yet.
   TFM must initialize the psram first by using SYSRAM.
   step1. Copy the data and bss section from PSRAM initialization lib to SYSRAM.
   step2. Set the msp to the SYSRAM space.
   step3. Start to initialize the PSRAM.
   step4. Clean the SYSRAM space used in the PSRAM initialization flow.
*/
    //step1. Copy the data and bss section from PSRAM initialization lib to SYSRAM.
    ldr    r4, =__copy_table_sysram_start__
    ldr    r5, =__copy_table_sysram_end__

.L_loop4:
    cmp    r4, r5
    bge    .L_loop4_done
    ldr    r1, [r4]
    ldr    r2, [r4, #4]
    ldr    r3, [r4, #8]

.L_loop4_0:
    subs    r3, #4
    ittt    ge
    ldrge    r0, [r1, r3]
    strge    r0, [r2, r3]
    bge    .L_loop4_0

    adds    r4, #12
    b    .L_loop4

.L_loop4_done:


    ldr    r3, =__zero_table_sysram_start__
    ldr    r4, =__zero_table_sysram_end__

.L_loop5:
    cmp    r3, r4
    bge    .L_loop5_done
    ldr    r1, [r3]
    ldr    r2, [r3, #4]
    movs    r0, 0

.L_loop5_0:
    subs    r2, #4
    itt    ge
    strge    r0, [r1, r2]
    bge    .L_loop5_0

    adds    r3, #8
    b    .L_loop5
.L_loop5_done:


    //step2. Set the msp to the SYSRAM space.
    cpsid   i              /* Disable IRQs */
    ldr  r0, =Image$$ARM_LIB_STACK_MSP_SYSRAM$$ZI$$Base
    msr    msplim, r0
    ldr  r0, =Image$$ARM_LIB_STACK_MSP_SYSRAM$$ZI$$Limit
    msr    msp, r0

    //step3. Start to initialize the PSRAM.
    bl     sysram_hw_uart_init
    bl     sysram_hal_clock_init
    // For E2 psram mbist calibration
    bl     tfm_psram_init

    //step4. Clean SYSRAM space
    movs    r0, #0
    ldr     r1, =Image$$ARM_LIB_STACK_MSP_SYSRAM$$ZI$$Base
    ldr     r2, =Image$$TFM_SYSRAM_LINKER_DATA$$ZI$$Limit
FillSYSRAMZero:
    str     r0, [r1], #4
    cmp     r1, r2
    blo     FillSYSRAMZero

#endif /* TFM_INIT_PSRAM */

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

#ifndef TFM_INIT_PSRAM
    cpsid   i              /* Disable IRQs */
#endif
    ldr  r0, =Image$$ARM_LIB_STACK_MSP$$ZI$$Base
    msr    msplim, r0
    ldr  r0, =Image$$ARM_LIB_STACK_MSP$$ZI$$Limit
    msr    msp, r0
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
    def_irq_handler    TIMER1_Handler                      /* TIMER 1 Handler */
    def_irq_handler    WIC_INT_IRQn                        /* 0 */
    def_irq_handler    RESERVED1_IRQn                      /* 1 */
    def_irq_handler    RESERVED2_IRQn                      /* 2 */
    def_irq_handler    WDT_B0_IRQn                         /* 3 */
    def_irq_handler    UART_IRQn                           /* 4 */
    def_irq_handler    INFRA_BUS_IRQn                      /* 5 */
    def_irq_handler    CDBGPWRUPREQ_IRQn                   /* 6 */
    def_irq_handler    CDBGPWRUPACK_IRQn                   /* 7 */
    def_irq_handler    WDT_B1_IRQn                         /* 8 */
    def_irq_handler    DSP_TO_CM33_IRQn                    /* 9 */
    def_irq_handler    APXGPT0_IRQn                        /* 10 */
    def_irq_handler    APXGPT1_IRQn                        /* 11 */
    def_irq_handler    APXGPT2_IRQn                        /* 12 */
    def_irq_handler    APXGPT3_IRQn                        /* 13 */
    def_irq_handler    APXGPT4_IRQn                        /* 14 */
    def_irq_handler    APXGPT5_IRQn                        /* 15 */
    def_irq_handler    DEVAPC_INFRA_AON_SECURE_VIO_IRQn_Handler   /* 16 */
    def_irq_handler    DEVAPC_AUD_BUS_SECURE_VIO_IRQn_Handler     /* 17 */
    def_irq_handler    CONN_AP_BUS_REQ_RISE_IRQn           /* 18 */
    def_irq_handler    CONN_AP_BUS_REQ_FULL_IRQn           /* 19 */
    def_irq_handler    CONN_APSRC_REQ_RISE_IRQn            /* 20 */
    def_irq_handler    CONN_APSRC_REQ_FALL_IRQn            /* 21 */
    def_irq_handler    CONN_AP_BUS_REQ_HIGH_IRQn           /* 22 */
    def_irq_handler    CONN_AP_BUS_REQ_LOW_IRQn            /* 23 */
    def_irq_handler    CONN_APSRC_REQ_HIGH_IRQn            /* 24 */
    def_irq_handler    CONN_APSRC_REQ_LOW_IRQn             /* 25 */
    def_irq_handler    INFRA_BUS_TIMEOUT_IRQn              /* 26 */
    def_irq_handler    CM33_LOCAL_BUS_IRQn                 /* 27 */
    def_irq_handler    ADSP_INFRA_BUS_TIMEOUT_IRQn         /* 28 */
    def_irq_handler    RESERVED29_IRQn                     /* 29 */
    def_irq_handler    RESERVED30_IRQn                     /* 30 */
    def_irq_handler    RESERVED31_IRQn                     /* 31 */
    def_irq_handler    DSP_UART_IRQn                       /* 32 */
    def_irq_handler    TOP_UART0_IRQn                      /* 33 */
    def_irq_handler    TOP_UART1_IRQn                      /* 34 */
    def_irq_handler    I2C0_IRQn                           /* 35 */
    def_irq_handler    I2C1_IRQn                           /* 36 */
    def_irq_handler    SDCTL_TOP_FW_IRQn                   /* 37 */
    def_irq_handler    SDCTL_TOP_FW_QOUT_IRQn              /* 38 */
    def_irq_handler    SPIM0_IRQn                          /* 39 */
    def_irq_handler    SPIM1_IRQn                          /* 40 */
    def_irq_handler    SPIS_IRQn                           /* 41 */
    def_irq_handler    KP_IRQn                             /* 42 */
    def_irq_handler    IRRX_IRQn                           /* 43 */
    def_irq_handler    RESERVED44_IRQn                     /* 44 */
    def_irq_handler    RESERVED45_IRQn                     /* 45 */
    def_irq_handler    RESERVED46_IRQn                     /* 46 */
    def_irq_handler    RESERVED47_IRQn                     /* 47 */
    def_irq_handler    SSUSB_XHCI_IRQn                     /* 48 */
    def_irq_handler    SSUSB_OTG_IRQn                      /* 49 */
    def_irq_handler    SSUSB_DEV_IRQn                      /* 50 */
    def_irq_handler    AFE_MCU_IRQn                        /* 51 */
    def_irq_handler    RTC_IRQn                            /* 52 */
    def_irq_handler    SYSRAM_TOP_IRQn                     /* 53 */
    def_irq_handler    ASIC_MPU_SECURE_VIO_IRQn_Handler         /* 54 */
    def_irq_handler    ASIC_MPU_PSRAM_SECURE_VIO_IRQn_Handler   /* 55 */
    def_irq_handler    CQDMA0_IRQn                         /* 56 */
    def_irq_handler    CQDMA1_IRQn                         /* 57 */
    def_irq_handler    CQDMA2_IRQn                         /* 58 */
    def_irq_handler    MSDC_IRQn                           /* 59 */
    def_irq_handler    MSDC_WAKEUP_IRQn                    /* 60 */
    def_irq_handler    DSP_WDT_IRQn                        /* 61 */
    def_irq_handler    DSP_TO_CPU_IRQn                     /* 62 */
    def_irq_handler    APDMA0_IRQn                         /* 63 */
    def_irq_handler    APDMA1_IRQn                         /* 64 */
    def_irq_handler    APDMA2_IRQn                         /* 65 */
    def_irq_handler    APDMA3_IRQn                         /* 66 */
    def_irq_handler    APDMA4_IRQn                         /* 67 */
    def_irq_handler    APDMA5_IRQn                         /* 68 */
    def_irq_handler    APDMA6_IRQn                         /* 69 */
    def_irq_handler    APDMA7_IRQn                         /* 70 */
    def_irq_handler    APDMA8_IRQn                         /* 71 */
    def_irq_handler    APDMA9_IRQn                         /* 72 */
    def_irq_handler    APDMA10_IRQn                        /* 73 */
    def_irq_handler    APDMA11_IRQn                        /* 74 */
    def_irq_handler    BTIF_HOST_IRQn                      /* 75 */
    def_irq_handler    SF_TOP_IRQn                         /* 76 */
    def_irq_handler    CONN2AP_WFDMA_IRQn                  /* 77 */
    def_irq_handler    BGF2AP_WDT_IRQn                     /* 78 */
    def_irq_handler    BGF2AP_BTIF0_WAKEUP_IRQn            /* 79 */
    def_irq_handler    CONN2AP_SW_IRQn                     /* 80 */
    def_irq_handler    BT2AP_ISOCH_IRQn                    /* 81 */
    def_irq_handler    BT_CVSD_IRQn                        /* 82 */
    def_irq_handler    CCIF_WF2AP_SW_IRQn                  /* 83 */
    def_irq_handler    CCIF_BGF2AP_SW_IRQn                 /* 84 */
    def_irq_handler    WM_CONN2AP_WDT_IRQn                 /* 85 */
    def_irq_handler    SEMA_RELEASE_INFORM_M2_IRQn         /* 86 */
    def_irq_handler    SEMA_RELEASE_INFORM_M3_IRQn         /* 87 */
    def_irq_handler    SEMA_M2_TIMEOUT_IRQn                /* 88 */
    def_irq_handler    SEMA_M3_TIMEOUT_IRQn                /* 89 */
    def_irq_handler    CONN_BGF_HIF_ON_HOST_IRQn           /* 90 */
    def_irq_handler    CONN_GPS_HIF_ON_HOST_IRQn           /* 91 */
    def_irq_handler    SSUSB_SPM_IRQn                      /* 92 */
    def_irq_handler    WF2AP_SW_IRQn                       /* 93 */
    def_irq_handler    CQDMA_SEC_ABORT_IRQn                /* 94 */
    def_irq_handler    APDMA_SEC_ABORT_IRQn                /* 95 */
    def_irq_handler    SDIO_CMD_IRQn                       /* 96 */
    def_irq_handler    RESERVED97_IRQn                     /* 97 */
    def_irq_handler    ADC_COMP_IRQn                       /* 98 */
    def_irq_handler    ADC_FIFO_IRQn                       /* 99 */
    def_irq_handler    GCPU_IRQn                           /* 100 */
    def_irq_handler    ECC_IRQn                            /* 101 */
    def_irq_handler    TRNG_IRQn                           /* 102 */
    def_irq_handler    TFM_TIMER0_IRQ_Handler              /* 103, SEJ_APXGPT_IRQn */
    def_irq_handler    SEJ_WDT_IRQn                        /* 104 */
    def_irq_handler    RESERVED105_IRQn                    /* 105 */
    def_irq_handler    RESERVED106_IRQn                    /* 106 */
    def_irq_handler    RESERVED107_IRQn                    /* 107 */
    def_irq_handler    GPIO_IRQ0n                          /* 108 */
    def_irq_handler    GPIO_IRQ1n                          /* 109 */
    def_irq_handler    GPIO_IRQ2n                          /* 110 */
    def_irq_handler    GPIO_IRQ3n                          /* 111 */
    def_irq_handler    GPIO_IRQ4n                          /* 112 */
    def_irq_handler    GPIO_IRQ5n                          /* 113 */
    def_irq_handler    GPIO_IRQ6n                          /* 114 */
    def_irq_handler    GPIO_IRQ7n                          /* 115 */
    def_irq_handler    GPIO_IRQ8n                          /* 116 */
    def_irq_handler    GPIO_IRQ9n                          /* 117 */
    def_irq_handler    GPIO_IRQ10n                         /* 118 */
    def_irq_handler    GPIO_IRQ11n                         /* 119 */
    def_irq_handler    GPIO_IRQ12n                         /* 120 */
    def_irq_handler    GPIO_IRQ13n                         /* 121 */
    def_irq_handler    GPIO_IRQ14n                         /* 122 */
    def_irq_handler    GPIO_IRQ15n                         /* 123 */
    def_irq_handler    GPIO_IRQ16n                         /* 124 */
    def_irq_handler    GPIO_IRQ17n                         /* 125 */
    def_irq_handler    GPIO_IRQ18n                         /* 126 */
    def_irq_handler    GPIO_IRQ19n                         /* 127 */
    def_irq_handler    GPIO_IRQ20n                         /* 128 */
    def_irq_handler    GPIO_IRQ21n                         /* 129 */
    def_irq_handler    GPIO_IRQ22n                         /* 130 */
    def_irq_handler    GPIO_IRQ23n                         /* 131 */
    def_irq_handler    GPIO_IRQ24n                         /* 132 */
    def_irq_handler    GPIO_IRQ25n                         /* 133 */
    def_irq_handler    GPIO_IRQ26n                         /* 134 */
    def_irq_handler    GPIO_IRQ27n                         /* 135 */
    def_irq_handler    GPIO_IRQ28n                         /* 136 */
    def_irq_handler    GPIO_IRQ29n                         /* 137 */
    def_irq_handler    GPIO_IRQ30n                         /* 138 */
    def_irq_handler    CM33_UART_RX_IRQn                   /* 139 */

    .end
