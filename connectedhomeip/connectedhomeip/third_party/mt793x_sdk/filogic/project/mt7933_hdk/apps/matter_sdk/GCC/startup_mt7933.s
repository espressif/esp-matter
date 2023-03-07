/**************************************************************************//**
 * @file     startup_ARMCM33.S
 * @brief    CMSIS-Core(M) Device Startup File for Cortex-M33 Device
 * @version  V2.0.1
 * @date     23. July 2019
 ******************************************************************************/
/*
 * Copyright (c) 2009-2019 Arm Limited. All rights reserved.
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

    /*
     * Placed SP and PC at the beginning of output binary.
     */

    .section .vectorsTop
    .align   2
    .globl   __VectorsTop
__VectorsTop:
    .long    __StackTop          /*     Top of Stack                      */
    .long    Reset_Handler       /*     Reset Handler                     */

    /*
     * ARMv8-M architecture requires vector table being 1024 bytes aligned
     * to support up to 256 vectors.
     */

    .section .vectors
    .align   10
    .globl   __Vectors
    .globl   __Vectors_End
    .globl   __Vectors_Size
__Vectors:
    .long    __StackTop          /*     Top of Stack                      */
    .long    Reset_Handler       /*     Reset Handler                     */
    .long    NMI_Handler         /* -14 NMI Handler                       */
    .long    HardFault_Handler   /* -13 Hard Fault Handler                */
    .long    MemManage_Handler   /* -12 MPU Fault Handler                 */
    .long    BusFault_Handler    /* -11 Bus Fault Handler                 */
    .long    UsageFault_Handler  /* -10 Usage Fault Handler               */
    .long    SecureFault_Handler /*  -9 Secure Fault Handler              */
    .long    0                   /*     Reserved                          */
    .long    0                   /*     Reserved                          */
    .long    0                   /*     Reserved                          */
    .long    SVC_Handler         /*  -5 SVCall Handler                    */
    .long    DebugMon_Handler    /*  -4 Debug Monitor Handler             */
    .long    0                   /*     Reserved                          */
    .long    PendSV_Handler      /*  -2 PendSV Handler                    */
    .long    SysTick_Handler     /*  -1 SysTick Handler                   */

    /* Interrupts */
    .long    isrC_main           /*   0 WIC_INT_IRQn                      */
    .long    isrC_main           /*   1 RESERVED1_IRQn                    */
    .long    isrC_main           /*   2 RESERVED2_IRQn                    */
    .long    isrC_main           /*   3 WDT_B0_IRQn                       */
    .long    isrC_main           /*   4 UART_IRQn                         */
    .long    isrC_main           /*   5 INFRA_BUS_IRQn                    */
    .long    isrC_main           /*   6 CDBGPWRUPREQ_IRQn                 */
    .long    isrC_main           /*   7 CDBGPWRUPACK_IRQn                 */
    .long    isrC_main           /*   8 WDT_B1_IRQn                       */
    .long    isrC_main           /*   9 DSP_TO_CM33_IRQn                  */
    .long    isrC_main           /*  10 APXGPT0_IRQn                      */
    .long    isrC_main           /*  11 APXGPT1_IRQn                      */
    .long    isrC_main           /*  12 APXGPT2_IRQn                      */
    .long    isrC_main           /*  13 APXGPT3_IRQn                      */
    .long    isrC_main           /*  14 APXGPT4_IRQn                      */
    .long    isrC_main           /*  15 APXGPT5_IRQn                      */
    .long    isrC_main           /*  16 DEVAPC_INFRA_AON_SECURE_VIO_IRQn  */
    .long    isrC_main           /*  17 DEVAPC_AUD_BUS_SECURE_VIO_IRQn    */
    .long    isrC_main           /*  18 CONN_AP_BUS_REQ_RISE_IRQn         */
    .long    isrC_main           /*  19 CONN_AP_BUS_REQ_FULL_IRQn         */
    .long    isrC_main           /*  20 CONN_APSRC_REQ_RISE_IRQn          */
    .long    isrC_main           /*  21 CONN_APSRC_REQ_FALL_IRQn          */
    .long    isrC_main           /*  22 CONN_AP_BUS_REQ_HIGH_IRQn         */
    .long    isrC_main           /*  23 CONN_AP_BUS_REQ_LOW_IRQn          */
    .long    isrC_main           /*  24 CONN_APSRC_REQ_HIGH_IRQn          */
    .long    isrC_main           /*  25 CONN_APSRC_REQ_LOW_IRQn           */
    .long    isrC_main           /*  26 INFRA_BUS_TIMEOUT_IRQn            */
    .long    isrC_main           /*  27 CM33_LOCAL_BUS_IRQn               */
    .long    isrC_main           /*  28 ADSP_INFRA_BUS_TIMEOUT_IRQn       */
    .long    isrC_main           /*  29 RESERVED29_IRQn                   */
    .long    isrC_main           /*  30 RESERVED30_IRQn                   */
    .long    isrC_main           /*  31 RESERVED31_IRQn                   */
    .long    isrC_main           /*  32 DSP_UART_IRQn                     */
    .long    isrC_main           /*  33 TOP_UART0_IRQn                    */
    .long    isrC_main           /*  34 TOP_UART1_IRQn                    */
    .long    isrC_main           /*  35 I2C0_IRQn                         */
    .long    isrC_main           /*  36 I2C1_IRQn                         */
    .long    isrC_main           /*  37 SDCTL_TOP_FW_IRQn                 */
    .long    isrC_main           /*  38 SDCTL_TOP_FW_QOUT_IRQn            */
    .long    isrC_main           /*  39 SPIM0_IRQn                        */
    .long    isrC_main           /*  40 SPIM1_IRQn                        */
    .long    isrC_main           /*  41 SPIS_IRQn                         */
    .long    isrC_main           /*  42 KP_IRQn                           */
    .long    isrC_main           /*  43 IRRX_IRQn                         */
    .long    isrC_main           /*  44 RESERVED44_IRQn                   */
    .long    isrC_main           /*  45 RESERVED45_IRQn                   */
    .long    isrC_main           /*  46 RESERVED46_IRQn                   */
    .long    isrC_main           /*  47 RESERVED47_IRQn                   */
    .long    isrC_main           /*  48 SSUSB_XHCI_IRQn                   */
    .long    isrC_main           /*  49 SSUSB_OTG_IRQn                    */
    .long    isrC_main           /*  50 SSUSB_DEV_IRQn                    */
    .long    isrC_main           /*  51 AFE_MCU_IRQn                      */
    .long    isrC_main           /*  52 RTC_IRQn                          */
    .long    isrC_main           /*  53 SYSRAM_TOP_IRQn                   */
    .long    isrC_main           /*  54 MPU_L2_PWR_IRQn                   */
    .long    isrC_main           /*  55 MPU_PSRAM_PWR_IRQn                */
    .long    isrC_main           /*  56 CQDMA0_IRQn                       */
    .long    isrC_main           /*  57 CQDMA1_IRQn                       */
    .long    isrC_main           /*  58 CQDMA2_IRQn                       */
    .long    isrC_main           /*  59 MSDC_IRQn                         */
    .long    isrC_main           /*  60 MSDC_WAKEUP_IRQn                  */
    .long    isrC_main           /*  61 DSP_WDT_IRQn                      */
    .long    isrC_main           /*  62 DSP_TO_CPU_IRQn                   */
    .long    isrC_main           /*  63 APDMA0_IRQn                       */
    .long    isrC_main           /*  64 APDMA1_IRQn                       */
    .long    isrC_main           /*  65 APDMA2_IRQn                       */
    .long    isrC_main           /*  66 APDMA3_IRQn                       */
    .long    isrC_main           /*  67 APDMA4_IRQn                       */
    .long    isrC_main           /*  68 APDMA5_IRQn                       */
    .long    isrC_main           /*  69 APDMA6_IRQn                       */
    .long    isrC_main           /*  70 APDMA7_IRQn                       */
    .long    isrC_main           /*  71 APDMA8_IRQn                       */
    .long    isrC_main           /*  72 APDMA9_IRQn                       */
    .long    isrC_main           /*  73 APDMA10_IRQn                      */
    .long    isrC_main           /*  74 APDMA11_IRQn                      */
    .long    isrC_main           /*  75 BTIF_HOST_IRQn                    */
    .long    isrC_main           /*  76 SF_TOP_IRQn                       */
    .long    isrC_main           /*  77 CONN2AP_WFDMA_IRQn                */
    .long    isrC_main           /*  78 BGF2AP_WDT_IRQn                   */
    .long    isrC_main           /*  79 BGF2AP_BTIF0_WAKEUP_IRQn          */
    .long    isrC_main           /*  80 CONN2AP_SW_IRQn                   */
    .long    isrC_main           /*  81 BT2AP_ISOCH_IRQn                  */
    .long    isrC_main           /*  82 BT_CVSD_IRQn                      */
    .long    isrC_main           /*  83 CCIF_WF2AP_SW_IRQn                */
    .long    isrC_main           /*  84 CCIF_BGF2AP_SW_IRQn               */
    .long    isrC_main           /*  85 WM_CONN2AP_WDT_IRQn               */
    .long    isrC_main           /*  86 SEMA_RELEASE_INFORM_M2_IRQn       */
    .long    isrC_main           /*  87 SEMA_RELEASE_INFORM_M3_IRQn       */
    .long    isrC_main           /*  88 SEMA_M2_TIMEOUT_IRQn              */
    .long    isrC_main           /*  89 SEMA_M3_TIMEOUT_IRQn              */
    .long    isrC_main           /*  90 CONN_BGF_HIF_ON_HOST_IRQn         */
    .long    isrC_main           /*  91 CONN_GPS_HIF_ON_HOST_IRQn         */
    .long    isrC_main           /*  92 SSUSB_SPM_IRQn                    */
    .long    isrC_main           /*  93 WF2AP_SW_IRQn                     */
    .long    isrC_main           /*  94 CQDMA_SEC_ABORT_IRQn              */
    .long    isrC_main           /*  95 APDMA_SEC_ABORT_IRQn              */
    .long    isrC_main           /*  96 SDIO_CMD_IRQn                     */
    .long    isrC_main           /*  97 RESERVED97_IRQn                   */
    .long    isrC_main           /*  98 ADC_COMP_IRQn                     */
    .long    isrC_main           /*  99 ADC_FIFO_IRQn                     */
    .long    isrC_main           /* 100 GCPU_IRQn                         */
    .long    isrC_main           /* 101 ECC_IRQn                          */
    .long    isrC_main           /* 102 TRNG_IRQn                         */
    .long    isrC_main           /* 103 SEJ_APXGPT_IRQn                   */
    .long    isrC_main           /* 104 SEJ_WDT_IRQn                      */
    .long    isrC_main           /* 105 RESERVED105_IRQn                  */
    .long    isrC_main           /* 106 RESERVED106_IRQn                  */
    .long    isrC_main           /* 107 RESERVED107_IRQn                  */
    .long    isrC_main           /* 108 GPIO_IRQ0n                        */
    .long    isrC_main           /* 109 GPIO_IRQ1n                        */
    .long    isrC_main           /* 110 GPIO_IRQ2n                        */
    .long    isrC_main           /* 111 GPIO_IRQ3n                        */
    .long    isrC_main           /* 112 GPIO_IRQ4n                        */
    .long    isrC_main           /* 113 GPIO_IRQ5n                        */
    .long    isrC_main           /* 114 GPIO_IRQ6n                        */
    .long    isrC_main           /* 115 GPIO_IRQ7n                        */
    .long    isrC_main           /* 116 GPIO_IRQ8n                        */
    .long    isrC_main           /* 117 GPIO_IRQ9n                        */
    .long    isrC_main           /* 118 GPIO_IRQ10n                       */
    .long    isrC_main           /* 119 GPIO_IRQ11n                       */
    .long    isrC_main           /* 120 GPIO_IRQ12n                       */
    .long    isrC_main           /* 121 GPIO_IRQ13n                       */
    .long    isrC_main           /* 122 GPIO_IRQ14n                       */
    .long    isrC_main           /* 123 GPIO_IRQ15n                       */
    .long    isrC_main           /* 124 GPIO_IRQ16n                       */
    .long    isrC_main           /* 125 GPIO_IRQ17n                       */
    .long    isrC_main           /* 126 GPIO_IRQ18n                       */
    .long    isrC_main           /* 127 GPIO_IRQ19n                       */
    .long    isrC_main           /* 128 GPIO_IRQ20n                       */
    .long    isrC_main           /* 129 GPIO_IRQ21n                       */
    .long    isrC_main           /* 130 GPIO_IRQ22n                       */
    .long    isrC_main           /* 131 GPIO_IRQ23n                       */
    .long    isrC_main           /* 132 GPIO_IRQ24n                       */
    .long    isrC_main           /* 133 GPIO_IRQ25n                       */
    .long    isrC_main           /* 134 GPIO_IRQ26n                       */
    .long    isrC_main           /* 135 GPIO_IRQ27n                       */
    .long    isrC_main           /* 136 GPIO_IRQ28n                       */
    .long    isrC_main           /* 137 GPIO_IRQ29n                       */
    .long    isrC_main           /* 138 GPIO_IRQ30n                       */
    .long    isrC_main           /* 139 CM33_UART_RX_IRQn                 */
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
    ldr      r0, =__StackTop
    msr      msp, r0
    ldr      r0, =__StackLimit
    msr      msplim, r0

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
    bl      CachePreInit
    ldr     r3, =deep_sleep_return
    ldr     r3, [r3, #4]
    bx      r3


Is_coldboot:
    /* preinit cache to accelerate region init progress */
    bl      CachePreInit
    /* dump trace if reset by WDT */
    bl      SLA_WDTResetDump

    /* stack space zero init */
    movs    r0, #0
    ldr     r1, =_stack_start
    ldr     r2, =_stack_end
FillZero:
    str     r0, [r1], #4
    cmp     r1, r2
    blo     FillZero

    /* sysram_text section init (cached_text) */
    ldr     r1, =_sysram_code_load
    ldr     r2, =_sysram_code_start

    cmp     r1, r2
    beq     System_Init

    ldr     r3, =_sysram_code_end
    bl      Data_Init

    /* cached_data section */
    ldr     r1, =_sysram_data_load
    ldr     r2, =_sysram_data_start
    ldr     r3, =_sysram_data_end
    bl      Data_Init

    ldr     r2, =_sysram_bss_start
    ldr     r3, =_sysram_bss_end
    bl      Bss_Init

    /* noncached_sysram_text section init */
    ldr     r1, =_noncached_sysram_code_load
    ldr     r2, =_noncached_sysram_code_start
    ldr     r3, =_noncached_sysram_code_end
    bl      Data_Init

    /* noncached_data section init */
    ldr     r1, =_noncached_sysram_data_load
    ldr     r2, =_noncached_sysram_data_start
    ldr     r3, =_noncached_sysram_data_end
    bl      Data_Init

    ldr     r2, =_noncached_sysram_bss_start
    ldr     r3, =_noncached_sysram_bss_end
    bl      Bss_Init

System_Init:
    /* tcm code section init */
    ldr     r1, =_tcm_code_load
    ldr     r2, =_tcm_code_start
    ldr     r3, =_tcm_code_end
    bl      Data_Init

    /* tcm data section init */
    ldr     r1, =_tcm_data_load
    ldr     r2, =_tcm_data_start
    ldr     r3, =_tcm_data_end
    bl      Data_Init

    ldr     r2, =_tcm_bss_start
    ldr     r3, =_tcm_bss_end
    bl      Bss_Init

    /* system pre-init call before psram data move */
    bl      system_preinit

    /* ram_text section init */
    ldr     r1, =_ram_code_load
    ldr     r2, =_ram_code_start
    ldr     r3, =_ram_code_end
    bl      Data_Init

    /* cached_data section */
    ldr     r1, =_ram_data_load
    ldr     r2, =_ram_data_start
    ldr     r3, =_ram_data_end
    bl      Data_Init

    ldr     r2, =_ram_bss_start
    ldr     r3, =_ram_bss_end
    bl      Bss_Init

    /* noncached ram_text section init */
    ldr     r1, =_noncached_ram_code_load
    ldr     r2, =_noncached_ram_code_start
    ldr     r3, =_noncached_ram_code_end
    bl      Data_Init

    /* noncached_data section init */
    ldr     r1, =_noncached_ram_data_load
    ldr     r2, =_noncached_ram_data_start
    ldr     r3, =_noncached_ram_data_end
    bl      Data_Init

    ldr     r2, =_noncached_ram_bss_start
    ldr     r3, =_noncached_ram_bss_end
    bl      Bss_Init

    /* CMSIS System Initialization */
    bl      __libc_init_array
    bl      SystemInit
    bl      main

    .fnend
    .size   Reset_Handler, . - Reset_Handler

/**
 * @brief  This is data init sub-function
 * @param  None
 * @retval None
 */
    .section  .text.Data_Init,"ax",%progbits
Data_Init:
copy_data_loop:
    cmp     r2, r3
    ittt    lo
    ldrlo   r0, [r1], #4
    strlo   r0, [r2], #4
    blo     copy_data_loop
    bx      lr
    .size  Data_Init, . - Data_Init

/**
 * @brief  This is bss init sub-function
 * @param  None
 * @retval None
 */
    .section  .text.Bss_Init,"ax",%progbits
Bss_Init:
ZeroBssLoop:
    cmp     r2, r3
    ittt    lo
    movlo   r0, #0
    strlo   r0, [r2], #4
    blo     ZeroBssLoop
    bx      lr
    .size  Bss_Init, .-Bss_Init

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


/* Default exception/interrupt handler */

    Set_Default_Handler  NMI_Handler
    Set_Default_Handler  MemManage_Handler
    Set_Default_Handler  BusFault_Handler
    Set_Default_Handler  UsageFault_Handler
    Set_Default_Handler  SecureFault_Handler
    Set_Default_Handler  SVC_Handler
    Set_Default_Handler  DebugMon_Handler
    Set_Default_Handler  PendSV_Handler
    Set_Default_Handler  SysTick_Handler

    .end
