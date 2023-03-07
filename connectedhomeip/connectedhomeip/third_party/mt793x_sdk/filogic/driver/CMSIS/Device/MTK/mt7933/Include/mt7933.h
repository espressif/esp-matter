/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#ifndef __MT7933_H__
#define __MT7933_H__

#ifdef __cplusplus
extern "C" {
#endif

#define CM4_HIF_IRQ               22

typedef enum IRQn {
    /****  CM33 internal exceptions  **********/

    /* Auxiliary constants */
    NotAvail_IRQn                = -128,      /* Not available device specific interrupt */

    /* Core interrupts */
    Reset_IRQn                   = -15,       /* Reset */
    NonMaskableInt_IRQn          = -14,       /* NMI, Non Maskable Interrupt */
    HardFault_IRQn               = -13,       /* HarFault */
    MemoryManagement_IRQn        = -12,       /* Memory Management */
    BusFault_IRQn                = -11,       /* Bus Fault  */
    UsageFault_IRQn              = -10,       /* Usage Fault */
    SecureFault_IRQn             = -9,        /* Secure Fault Handler */
    SVC_IRQn                     = -5,        /* SV Call*/
    DebugMonitor_IRQn            = -4,        /* Debug Monitor */
    PendSV_IRQn                  = -2,        /* Pend SV */
    SysTick_IRQn                 = -1,        /* System Tick */

    /****  MT7933 specific external/peripheral interrupt ****/
    WIC_INT_IRQn                        = 0,
    RESERVED1_IRQn                      = 1,
    RESERVED2_IRQn                      = 2,
    WDT_B0_IRQn                         = 3,
    UART_IRQn                           = 4,
    INFRA_BUS_IRQn                      = 5,
    CDBGPWRUPREQ_IRQn                   = 6,
    CDBGPWRUPACK_IRQn                   = 7,
    WDT_B1_IRQn                         = 8,
    DSP_TO_CM33_IRQn                    = 9,
    APXGPT0_IRQn                        = 10,
    APXGPT1_IRQn                        = 11,
    APXGPT2_IRQn                        = 12,
    APXGPT3_IRQn                        = 13,
    APXGPT4_IRQn                        = 14,
    APXGPT5_IRQn                        = 15,
    DEVAPC_INFRA_AON_SECURE_VIO_IRQn    = 16,
    DEVAPC_AUD_BUS_SECURE_VIO_IRQn      = 17,
    CONN_AP_BUS_REQ_RISE_IRQn           = 18,
    CONN_AP_BUS_REQ_FULL_IRQn           = 19,
    CONN_APSRC_REQ_RISE_IRQn            = 20,
    CONN_APSRC_REQ_FALL_IRQn            = 21,
    CONN_AP_BUS_REQ_HIGH_IRQn           = 22,
    CONN_AP_BUS_REQ_LOW_IRQn            = 23,
    CONN_APSRC_REQ_HIGH_IRQn            = 24,
    CONN_APSRC_REQ_LOW_IRQn             = 25,
    INFRA_BUS_TIMEOUT_IRQn              = 26,
    CM33_LOCAL_BUS_IRQn                 = 27,
    ADSP_INFRA_BUS_TIMEOUT_IRQn         = 28,
    RESERVED29_IRQn                     = 29,
    RESERVED30_IRQn                     = 30,
    RESERVED31_IRQn                     = 31,
    DSP_UART_IRQn                       = 32,
    TOP_UART0_IRQn                      = 33,
    TOP_UART1_IRQn                      = 34,
    I2C0_IRQn                           = 35,
    I2C1_IRQn                           = 36,
    SDCTL_TOP_FW_IRQn                   = 37,
    SDCTL_TOP_FW_QOUT_IRQn              = 38,
    SPIM0_IRQn                          = 39,
    SPIM1_IRQn                          = 40,
    SPIS_IRQn                           = 41,
    KP_IRQn                             = 42,
    IRRX_IRQn                           = 43,
    RESERVED44_IRQn                     = 44,
    RESERVED45_IRQn                     = 45,
    RESERVED46_IRQn                     = 46,
    RESERVED47_IRQn                     = 47,
    SSUSB_XHCI_IRQn                     = 48,
    SSUSB_OTG_IRQn                      = 49,
    SSUSB_DEV_IRQn                      = 50,
    AFE_MCU_IRQn                        = 51,
    RTC_IRQn                            = 52,
    SYSRAM_TOP_IRQn                     = 53,
    MPU_L2_PWR_IRQn                     = 54,
    MPU_PSRAM_PWR_IRQn                  = 55,
    CQDMA0_IRQn                         = 56,
    CQDMA1_IRQn                         = 57,
    CQDMA2_IRQn                         = 58,
    MSDC_IRQn                           = 59,
    MSDC_WAKEUP_IRQn                    = 60,
    DSP_WDT_IRQn                        = 61,
    DSP_TO_CPU_IRQn                     = 62,
    APDMA0_IRQn                         = 63,
    APDMA1_IRQn                         = 64,
    APDMA2_IRQn                         = 65,
    APDMA3_IRQn                         = 66,
    APDMA4_IRQn                         = 67,
    APDMA5_IRQn                         = 68,
    APDMA6_IRQn                         = 69,
    APDMA7_IRQn                         = 70,
    APDMA8_IRQn                         = 71,
    APDMA9_IRQn                         = 72,
    APDMA10_IRQn                        = 73,
    APDMA11_IRQn                        = 74,
    BTIF_HOST_IRQn                      = 75,
    SF_TOP_IRQn                         = 76,
    CONN2AP_WFDMA_IRQn                  = 77,
    BGF2AP_WDT_IRQn                     = 78,
    BGF2AP_BTIF0_WAKEUP_IRQn            = 79,
    CONN2AP_SW_IRQn                     = 80,
    BT2AP_ISOCH_IRQn                    = 81,
    BT_CVSD_IRQn                        = 82,
    CCIF_WF2AP_SW_IRQn                  = 83,
    CCIF_BGF2AP_SW_IRQn                 = 84,
    WM_CONN2AP_WDT_IRQn                 = 85,
    SEMA_RELEASE_INFORM_M2_IRQn         = 86,
    SEMA_RELEASE_INFORM_M3_IRQn         = 87,
    SEMA_M2_TIMEOUT_IRQn                = 88,
    SEMA_M3_TIMEOUT_IRQn                = 89,
    CONN_BGF_HIF_ON_HOST_IRQn           = 90,
    CONN_GPS_HIF_ON_HOST_IRQn           = 91,
    SSUSB_SPM_IRQn                      = 92,
    WF2AP_SW_IRQn                       = 93,
    CQDMA_SEC_ABORT_IRQn                = 94,
    APDMA_SEC_ABORT_IRQn                = 95,
    SDIO_CMD_IRQn                       = 96,
    RESERVED97_IRQn                     = 97,
    ADC_COMP_IRQn                       = 98,
    ADC_FIFO_IRQn                       = 99,
    GCPU_IRQn                           = 100,
    ECC_IRQn                            = 101,
    TRNG_IRQn                           = 102,
    SEJ_APXGPT_IRQn                     = 103,
    SEJ_WDT_IRQn                        = 104,
    RESERVED105_IRQn                    = 105,
    RESERVED106_IRQn                    = 106,
    RESERVED107_IRQn                    = 107,
    GPIO_IRQ0n                          = 108,
    GPIO_IRQ1n                          = 109,
    GPIO_IRQ2n                          = 110,
    GPIO_IRQ3n                          = 111,
    GPIO_IRQ4n                          = 112,
    GPIO_IRQ5n                          = 113,
    GPIO_IRQ6n                          = 114,
    GPIO_IRQ7n                          = 115,
    GPIO_IRQ8n                          = 116,
    GPIO_IRQ9n                          = 117,
    GPIO_IRQ10n                         = 118,
    GPIO_IRQ11n                         = 119,
    GPIO_IRQ12n                         = 120,
    GPIO_IRQ13n                         = 121,
    GPIO_IRQ14n                         = 122,
    GPIO_IRQ15n                         = 123,
    GPIO_IRQ16n                         = 124,
    GPIO_IRQ17n                         = 125,
    GPIO_IRQ18n                         = 126,
    GPIO_IRQ19n                         = 127,
    GPIO_IRQ20n                         = 128,
    GPIO_IRQ21n                         = 129,
    GPIO_IRQ22n                         = 130,
    GPIO_IRQ23n                         = 131,
    GPIO_IRQ24n                         = 132,
    GPIO_IRQ25n                         = 133,
    GPIO_IRQ26n                         = 134,
    GPIO_IRQ27n                         = 135,
    GPIO_IRQ28n                         = 136,
    GPIO_IRQ29n                         = 137,
    GPIO_IRQ30n                         = 138,
    CM33_UART_RX_IRQn                   = 139,
    IRQ_NUMBER_MAX                      = 140
} IRQn_Type;

typedef void (*irq_handler_t)(void);

/* ================================================================================ */
/* ================      Processor and Core Peripheral Section     ================ */
/* ================================================================================ */
#define __CM33_REV                0x0000U   /* Core revision r0p1 */
#define __SAUREGION_PRESENT       1U        /* SAU regions present */
#define __MPU_PRESENT             1U        /* MPU present */
#define __VTOR_PRESENT            1U        /* VTOR present */
#define __NVIC_PRIO_BITS          8U        /* Number of Bits used for Priority Levels */
#define __Vendor_SysTickConfig    0U        /* Set to 1 if different SysTick Config is used */
#define __FPU_PRESENT             1U        /* FPU present */
#define __DSP_PRESENT             1U        /* DSP extension present */

#include "core_cm33.h"                      /* Processor and core peripherals */
//#include "system_ARMCM33.h"                 /* System Header */

//#include "core_cm4.h"                  /* Core Peripheral Access Layer */

typedef IRQn_Type hal_nvic_irq_t;

/* === Fix me: Port from mt7687.h ...Michael ========*/
#define CM4_HIF_IRQ               22
#define DEFAULT_PRI             5
#define HIGHEST_PRI             9
#define CM4_HIF_PRI   DEFAULT_PRI
/* ================================================== */

#include "system_mt7933.h"

/* MT7933 private config */
#include "reg_base.h"


/****************************************************************************
 *
 * VIRTUAL vs PHYSICAL memory address conversion macros:
 *
 ****************************************************************************/


/*  The memory base addresses of MT7933:
    +------------------+---------------+--------------+
    | Memory Region    | Physical Base | Virtual Base |
    +------------------+---------------+--------------+
    | SYSRAM           | 0x8000-0000   | 0x0800-0000  |
    +------------------+---------------+--------------+
    | SERIAL NOR FLASH | 0x9000-0000   | 0x1800-0000  |
    +------------------+---------------+--------------+
    | PSRAM            | 0xA000-0000   | 0x1000-0000  |
    +------------------+---------------+--------------+ */


#define SYSRAM_DIFF     (0x88000000)
#define XIP_DIFF        (0x88000000)
#define PSRAM_DIFF      (0x70000000)
#define RS24(_addr_)    ((_addr_) >> 24)


/**
 * Convert memory address from virtual to physical.
 */
#define MEMORY_VIRTUAL_TO_PHYSICAL(_addr_)              \
    ( (RS24(_addr_) == 0x08) ? (_addr_ - SYSRAM_DIFF) : \
      (RS24(_addr_) == 0x18) ? (_addr_ - XIP_DIFF)    : \
      (RS24(_addr_) == 0x10) ? (_addr_ - PSRAM_DIFF)  : \
      _addr_ )

/**
 * Convert memory address from physical to virtual.
 */
#define MEMORY_PHYSICAL_TO_VIRTUAL(_addr_)              \
    ( (RS24(_addr_) == 0x80) ? (_addr_ + SYSRAM_DIFF) : \
      (RS24(_addr_) == 0x90) ? (_addr_ + XIP_DIFF)    : \
      (RS24(_addr_) == 0xA0) ? (_addr_ + PSRAM_DIFF)  : \
      _addr_ )


#define BTIF_BASE            0xA0090000 /*Bluetooth interface*/

/************************ SDIO_SLAVE register definition start line  *******************************
 */

typedef struct {
    /****************************************************************/
    /*************This part define FW global register****************/
    /****************************************************************/
    __IO uint32_t HGFCR;          /*offset:0x00, HIF Global Firmware Configuration Register.*/
    __IO uint32_t HGFISR;         /*offset:0x04, HIF Global Firmware Interrupt Status Register.*/
    __IO uint32_t HGFIER;         /*offset:0x08, HIF Global Firmware Interrupt Enable Register.*/
    __IO uint32_t rsv0;
    __IO uint32_t HSDBDLSR;       /*offset:0x10, HIF SDIO Bus Delay Selection Register.*/
    __IO uint32_t HSDLSR;         /*offset:0x14, HIF SRAM Delay Selection Register.*/
    __IO uint32_t HCSDCR;         /*offset:0x18, HIF Clock Stop Detection register.*/
    __IO uint32_t HGH2DR;         /*offset:0x1c, HIF Global Host to Device Register.*/
    __IO uint32_t HDBGCR;         /*offset:0x20, HIF Debug Control Register.*/
    __IO uint32_t rsv1[2];
    __IO uint32_t FWDSIOCR;   /*offset:0x2c, DS Pad Macro IO Control Register.*/
    __IO uint32_t HGTMTCR;    /*offset:0x30, Test Mode Trigger Control Register.*/
    __IO uint32_t HGTMCR;     /*offset:0x34, Test Mode Control Register.*/
    __IO uint32_t HGTMDPCR0;     /*offset:0x38, Test Mode Data Pattern Control Register 0.*/
    __IO uint32_t HGTMDPCR1;     /*offset:0x3c, Test Mode Data Pattern Control Register 1.*/
    __IO uint32_t FWCLKIOCR_T28LP;     /*offset:0x40, Clock Pad Macro IO Control Register.*/
    __IO uint32_t FWCMDIOCR_T28LP;     /*offset:0x44, Command Pad Macro IO Control Register.*/
    __IO uint32_t FWDAT0IOCR_T28LP;     /*offset:0x48, Data 0 Pad Macro IO Control Register.*/
    __IO uint32_t FWDAT1IOCR_T28LP;     /*offset:0x4c, Data 1 Pad Macro IO Control Register.*/
    __IO uint32_t FWDAT2IOCR_T28LP;     /*offset:0x50, Data 2 Pad Macro IO Control Register.*/
    __IO uint32_t FWDAT3IOCR_T28LP;     /*offset:0x54, Data 3 Pad Macro IO Control Register.*/
    __IO uint32_t FWCLKDLYCR;     /*offset:0x58, Clock Pad Macro Delay Chain Control Register.*/
    __IO uint32_t FWCMDDLYCR;     /*offset:0x5c, Command Pad Macro Delay Chain Control Register.*/
    __IO uint32_t FWODATDLYCR;     /*offset:0x60, SDIO Output Data Delay Chain Control Register.*/
    __IO uint32_t FWIDATDLYCR1;     /*offset:0x64, SDIO Input Data Delay Chain Control Register 1.*/
    __IO uint32_t FWIDATDLYCR2;     /*offset:0x68, SDIO Input Data Delay Chain Control Register 2.*/
    __IO uint32_t FWILCHCR;     /*offset:0x6c, SDIO Input Data Latch Time Control Register.*/
    __IO uint32_t rsv2[36];

    /****************************************************************/
    /*************This part define FW DMA register*******************/
    /****************************************************************/
    __IO uint32_t HWFISR;     /*offset:0x100, HIF WLAN Firmware Interrupt Status Register.*/
    __IO uint32_t HWFIER;     /*offset:0x104, HIF WLAN Firmware Interrupt Enable Register.*/
    __IO uint32_t HWFISR1;     /*offset:0x108, Reserve for HWFISR1.*/
    __IO uint32_t HWFIER1;     /*offset:0x10c, Reserve for HWFIER1.*/
    __IO uint32_t HWFTE0SR;     /*offset:0x110, HIF WLAN Firmware TX Event 0 Status Register.*/
    __IO uint32_t HWFTE1SR;     /*offset:0x114, Reserve for HWFTE1SR.*/
    __IO uint32_t HWFTE2SR;     /*offset:0x118, Reserve for HWFTE2SR.*/
    __IO uint32_t HWFTE3SR;     /*offset:0x11c, Reserve for HWFTE3SR.*/
    __IO uint32_t HWFTE0ER;     /*offset:0x120, HIF WLAN Firmware TX Event 0 Enable Register.*/
    __IO uint32_t HWFTE1ER;     /*offset:0x124, Reserve for HWFTE1ER.*/
    __IO uint32_t HWFTE2ER;     /*offset:0x128, Reserve for HWFTE2ER.*/
    __IO uint32_t HWFTE3ER;     /*offset:0x12c, Reserve for HWFTE3ER.*/
    __IO uint32_t HWFRE0SR;     /*offset:0x130, HIF WLAN Firmware RX Event 0 Status Register.*/
    __IO uint32_t HWFRE1SR;     /*offset:0x134, HIF WLAN Firmware RX Event 1 Status Register.*/
    __IO uint32_t HWFRE2SR;     /*offset:0x138, Reserve for HWFRE2SR.*/
    __IO uint32_t HWFRE3SR;     /*offset:0x13c, Reserve for HWFRE3SR.*/
    __IO uint32_t HWFRE0ER;     /*offset:0x140, HIF WLAN Firmware RX Event 0 Enable Register.*/
    __IO uint32_t HWFRE1ER;     /*offset:0x144, HIF WLAN Firmware RX Event 1 Enable Register.*/
    __IO uint32_t HWFRE2ER;     /*offset:0x148, Reserve for HWFRE2ER.*/
    __IO uint32_t HWFRE3ER;     /*offset:0x14c, Reserve for HWFRE3ER.*/
    __IO uint32_t HWFICR;       /*offset:0x150, HIF WLAN Firmware Interrupt Control Register.*/
    __IO uint32_t HWFCR;        /*offset:0x154, HIF WLAN Firmware Control Register.*/
    __IO uint32_t HWTDCR;       /*offset:0x158, HIF WLAN TX DMA Control Register.*/
    __IO uint32_t HWTPCCR;       /*offset:0x15c, HIF WLAN TX Packet Count Control Register.*/
    __IO uint32_t HWFTQ0SAR;       /*offset:0x160, HIF WLAN Firmware TX Queue 0 Start Address Register.*/
    __IO uint32_t HWFTQ1SAR;       /*offset:0x164, HIF WLAN Firmware TX Queue 1 Start Address Register.*/
    __IO uint32_t HWFTQ2SAR;       /*offset:0x168, HIF WLAN Firmware TX Queue 2 Start Address Register.*/
    __IO uint32_t HWFTQ3SAR;       /*offset:0x16c, HIF WLAN Firmware TX Queue 3 Start Address Register.*/
    __IO uint32_t HWFTQ4SAR;       /*offset:0x170, HIF WLAN Firmware TX Queue 4 Start Address Register.*/
    __IO uint32_t HWFTQ5SAR;       /*offset:0x174, Reserve for HIF WLAN Firmware TX Queue 5 Start Address Register.*/
    __IO uint32_t HWFTQ6SAR;       /*offset:0x178, Reserve for HIF WLAN Firmware TX Queue 6 Start Address Register.*/
    __IO uint32_t HWFTQ7SAR;       /*offset:0x17c, Reserve for HIF WLAN Firmware TX Queue 7 Start Address Register.*/
    __IO uint32_t HWFRQ0SAR;       /*offset:0x180, HIF WLAN Firmware RX Queue 0 Start Address Register.*/
    __IO uint32_t HWFRQ1SAR;       /*offset:0x184, HIF WLAN Firmware RX Queue 1 Start Address Register.*/
    __IO uint32_t HWFRQ2SAR;       /*offset:0x188, HIF WLAN Firmware RX Queue 2 Start Address Register.*/
    __IO uint32_t HWFRQ3SAR;       /*offset:0x18c, HIF WLAN Firmware RX Queue 3 Start Address Register.*/
    __IO uint32_t HWFRQ4SAR;       /*offset:0x190, Reserve for HIF WLAN Firmware RX Queue 4 Start Address Register.*/
    __IO uint32_t HWFRQ5SAR;       /*offset:0x194, Reserve for HIF WLAN Firmware RX Queue 5 Start Address Register.*/
    __IO uint32_t HWFRQ6SAR;       /*offset:0x198, Reserve for HIF WLAN Firmware RX Queue 6 Start Address Register.*/
    __IO uint32_t HWFRQ7SAR;       /*offset:0x19c, Reserve for HIF WLAN Firmware RX Queue 7 Start Address Register.*/
    __IO uint32_t H2DRM0R;       /*offset:0x1a0, Host to Device Receive Mailbox 0 Register.*/
    __IO  uint32_t H2DRM1R;       /*offset:0x1a4, Host to Device Receive Mailbox 1 Register.*/
    __IO  uint32_t D2HSM0R;       /*offset:0x1a8, Device to Host Send Mailbox 0 Register.*/
    __IO uint32_t D2HSM1R;       /*offset:0x1ac, Device to Host Send Mailbox 1 Register.*/
    __IO uint32_t D2HSM2R;       /*offset:0x1b0, Device to Host Send Mailbox 2 Register.*/
    __IO uint32_t rsv3[3];
    __IO uint32_t HWRQ0CR;       /*offset:0x1c0, HIF WLAN RX Queue 0 Control Register.*/
    __IO uint32_t HWRQ1CR;       /*offset:0x1c4, HIF WLAN RX Queue 1 Control Register.*/
    __IO uint32_t HWRQ2CR;       /*offset:0x1c8, HIF WLAN RX Queue 2 Control Register.*/
    __IO uint32_t HWRQ3CR;       /*offset:0x1cc, HIF WLAN RX Queue 3 Control Register.*/
    __IO uint32_t HWRQ4CR;       /*offset:0x1d0, Reserve for HWRQ4CR.*/
    __IO uint32_t HWRQ5CR;       /*offset:0x1d4, Reserve for HWRQ5CR.*/
    __IO uint32_t HWRQ6CR;       /*offset:0x1d8, Reserve for HWRQ6CR.*/
    __IO uint32_t HWRQ7CR;       /*offset:0x1dc, Reserve for HWRQ7CR.*/
    __I  uint32_t HWRLFACR;       /*offset:0x1e0, HIF WLAN RX Length FIFO Available Count Register.*/
    __I  uint32_t HWRLFACR1;       /*offset:0x1e4, Reserve for HWRLFACR1.*/
    __IO uint32_t HWDMACR;       /*offset:0x1e8, HIF WLAN DMA Control Register.*/
    __IO uint32_t HWFIOCDR;       /*offset:0x1ec, HIF WLAN Firmware GPD IOC bit Disable Register.*/
    __IO uint32_t HSDIOTOCR;       /*offset:0x1f0, HIF SDIO Time-Out Control Register.*/
    __IO uint32_t rsv4[3];
    __I  uint32_t HWFTSR0;       /*offset:0x200, HIF WLAN Firmware TX Status Register 0.*/
    __I  uint32_t HWFTSR1;       /*offset:0x204, HIF WLAN Firmware TX Status Register 1.*/
    __IO uint32_t rsv5[2];
    __IO uint32_t HWDBGCR;       /*offset:0x210, HIF WLAN Debug Control Register.*/
    __IO uint32_t HWDBGPLR;       /*offset:0x214, HIF WLAN Debug Packet Length Register.*/
    __IO uint32_t HSPICSR;       /*offset:0x218, WLAN SPI Control Status Register (SPI Only).*/
    __IO uint32_t rsv6;
    __IO uint32_t HWRX0CGPD;       /*offset:0x220, DMA RX0 Current GPD Address Register.*/
    __IO uint32_t HWRX1CGPD;       /*offset:0x224, DMA RX1 Current GPD Address Register.*/
    __IO uint32_t HWRX2CGPD;       /*offset:0x228, DMA RX2 Current GPD Address Register.*/
    __IO uint32_t HWRX3CGPD;       /*offset:0x22c, DMA RX3 Current GPD Address Register.*/
    __IO uint32_t HWTX0CGPD;       /*offset:0x230, DMA TX0 Current GPD Address Register.*/
    __IO uint32_t HWTX1Q1CGPD;       /*offset:0x234,DMA TX1 Que Type 1 Current GPD Address Register.*/
    __IO uint32_t HWTX1Q2CGPD;       /*offset:0x238, DMA TX1 Que Type 2 Current GPD Address Register.*/
    __IO uint32_t HWTX1Q3CGPD;       /*offset:0x23c, DMA TX1 Que Type 3 Current GPD Address Register.*/
    __IO uint32_t HWTX1Q4CGPD;       /*offset:0x240,DMA TX1 Que Type 4 Current GPD Address Register.*/
    __IO uint32_t HWTX1Q5CGPD;       /*offset:0x244, DMA TX1 Que Type 5 Current GPD Address Register.*/
    __IO uint32_t HWTX1Q6CGPD;       /*offset:0x248, DMA TX1 Que Type 6 Current GPD Address Register.*/
    __IO uint32_t HWTX1Q7CGPD;       /*offset:0x24c, DMA TX1 Que Type 7 Current GPD Address Register.*/
} sdio_slv_register_t;



#define SDIO_SLAVE_REG     ((sdio_slv_register_t *)SDIOS_BASE)

/*********************SDIO FW HGFCR*****************************/
#define HGFCR_DB_HIF_SEL_OFFSET        (0)
#define HGFCR_DB_HIF_SEL_MASK          (0x07 << HGFCR_DB_HIF_SEL_OFFSET)
#define HGFCR_SPI_MODE_OFFSET          (4)
#define HGFCR_SPI_MODE_MASK            (1 << HGFCR_SPI_MODE_OFFSET)
#define HGFCR_EHPI_MODE_OFFSET         (5)
#define HGFCR_EHPI_MODE_MASK           (1 << HGFCR_EHPI_MODE_OFFSET)
#define HGFCR_SDIO_PIO_SEL_OFFSET      (6)
#define HGFCR_SDIO_PIO_SEL_MASK        (1 << HGFCR_SDIO_PIO_SEL_OFFSET)
#define HGFCR_HINT_AS_FW_OB_OFFSET     (8)
#define HGFCR_HINT_AS_FW_OB_MASK       (1 << HGFCR_HINT_AS_FW_OB_OFFSET)
#define HGFCR_CARD_IS_18V_OFFSET       (9)
#define HGFCR_CARD_IS_18V_MASK         (1 << HGFCR_CARD_IS_18V_OFFSET)
#define HGFCR_SDCTL_BUSY_OFFSET        (10)
#define HGFCR_SDCTL_BUSY_MASK          (1 << HGFCR_SDCTL_BUSY_OFFSET)
#define HGFCR_INT_TER_CYC_MASK_OFFSET  (16)
#define HGFCR_INT_TER_CYC_MASK_MASK    (1 << HGFCR_INT_TER_CYC_MASK_OFFSET)
#define HGFCR_HCLK_NO_GATED_OFFSET     (17)
#define HGFCR_HCLK_NO_GATED_MASK       (1 << HGFCR_HCLK_NO_GATED_OFFSET)
#define HGFCR_FORCE_SD_HS_OFFSET       (18)
#define HGFCR_FORCE_SD_HS_MASK         (1 << HGFCR_FORCE_SD_HS_OFFSET)
#define HGFCR_SDIO_HCLK_DIS_OFFSET     (24)
#define HGFCR_SDIO_HCLK_DIS_MASK       (1 << HGFCR_SDIO_HCLK_DIS_OFFSET)
#define HGFCR_SPI_HCLK_DIS_OFFSET      (25)
#define HGFCR_SPI_HCLK_DIS_MASK        (1 << HGFCR_SPI_HCLK_DIS_OFFSET)
#define HGFCR_EHPI_HCLK_DIS_OFFSET     (26)
#define HGFCR_EHPI_HCLK_DIS_MASK       (1 << HGFCR_EHPI_HCLK_DIS_OFFSET)
#define HGFCR_PB_HCLK_DIS_OFFSET       (27)
#define HGFCR_PB_HCLK_DIS_MASK         (1 << HGFCR_PB_HCLK_DIS_OFFSET)
#define HGFCR_PAD_CR_SET_BY_FW_OFFSET  (28)
#define HGFCR_PAD_CR_SET_BY_FW_MASK    (1 << HGFCR_PAD_CR_SET_BY_FW_OFFSET)


/*********************SDIO FW HGFISR*****************************/
#define HGFISR_DRV_CLR_DB_IOE_OFFSET        (0)
#define HGFISR_DRV_CLR_DB_IOE_MASK          (1 << HGFISR_DRV_CLR_DB_IOE_OFFSET)
#define HGFISR_DRV_CLR_PB_IOE_OFFSET        (1)
#define HGFISR_DRV_CLR_PB_IOE_MASK          (1 << HGFISR_DRV_CLR_PB_IOE_OFFSET)
#define HGFISR_DRV_SET_DB_IOE_OFFSET        (2)
#define HGFISR_DRV_SET_DB_IOE_MASK          (1 << HGFISR_DRV_SET_DB_IOE_OFFSET)
#define HGFISR_DRV_SET_PB_IOE_OFFSET        (3)
#define HGFISR_DRV_SET_PB_IOE_MASK          (1 << HGFISR_DRV_SET_PB_IOE_OFFSET)
#define HGFISR_SDIO_SET_RES_OFFSET          (4)
#define HGFISR_SDIO_SET_RES_MASK            (1 << HGFISR_SDIO_SET_RES_OFFSET)
#define HGFISR_SDIO_SET_ABT_OFFSET          (5)
#define HGFISR_SDIO_SET_ABT_MASK            (1 << HGFISR_SDIO_SET_ABT_OFFSET)
#define HGFISR_DB_INT_OFFSET                (6)
#define HGFISR_DB_INT_MASK                  (1 << HGFISR_DB_INT_OFFSET)
#define HGFISR_PB_INT_OFFSET                (7)
#define HGFISR_PB_INT_MASK                  (1 << HGFISR_PB_INT_OFFSET)
#define HGFISR_CRC_ERROR_INT_OFFSET         (8)
#define HGFISR_CRC_ERROR_INT_MASK           (1 << HGFISR_CRC_ERROR_INT_OFFSET)
#define HGFISR_CHG_TO_18V_REQ_INT_OFFSET    (9)
#define HGFISR_CHG_TO_18V_REQ_INT_MASK      (1 << HGFISR_CHG_TO_18V_REQ_INT_OFFSET)
#define HGFISR_SD1_SET_XTAL_UPD_INT_OFFSET  (10)
#define HGFISR_SD1_SET_XTAL_UPD_INT_MASK    (1 << HGFISR_SD1_SET_XTAL_UPD_INT_OFFSET)
#define HGFISR_SD1_SET_DS_INT_OFFSET        (11)
#define HGFISR_SD1_SET_DS_INT_MASK          (1 << HGFISR_SD1_SET_DS_INT_OFFSET)


/*********************SDIO FW HWFCR*****************************/
#define HWFCR_W_FUNC_RDY_OFFSET            (0)
#define HWFCR_W_FUNC_RDY_MASK              (1 << HWFCR_W_FUNC_RDY_OFFSET)
#define HWFCR_TRX_DESC_CHKSUM_EN_OFFSET    (1)
#define HWFCR_TRX_DESC_CHKSUM_EN_MASK      (1 << HWFCR_TRX_DESC_CHKSUM_EN_OFFSET)
#define HWFCR_TRX_DESC_CHKSUM_12B_OFFSET   (2)
#define HWFCR_TRX_DESC_CHKSUM_12B_MASK     (1 << HWFCR_TRX_DESC_CHKSUM_12B_OFFSET)
#define HWFCR_TX_CS_OFLD_EN_OFFSET         (3)
#define HWFCR_TX_CS_OFLD_EN_MASK           (1 << HWFCR_TX_CS_OFLD_EN_OFFSET)
#define HWFCR_TX_NO_HEADER_OFFSET          (8)
#define HWFCR_TX_NO_HEADER_MASK            (1 << HWFCR_TX_NO_HEADER_OFFSET)
#define HWFCR_RX_NO_TAIL_OFFSET            (9)
#define HWFCR_RX_NO_TAIL_MASK              (1 << HWFCR_RX_NO_TAIL_OFFSET)


/*********************SDIO FW HWFISR*****************************/
#define HWFISR_DRV_SET_FW_OWN_OFFSET     (0)
#define HWFISR_DRV_SET_FW_OWN_MASK       (1 << HWFISR_DRV_SET_FW_OWN_OFFSET)
#define HWFISR_DRV_CLR_FW_OWN_OFFSET     (1)
#define HWFISR_DRV_CLR_FW_OWN_MASK       (1 << HWFISR_DRV_CLR_FW_OWN_OFFSET)
#define HWFISR_D2HSM2R_RD_INT_OFFSET     (2)
#define HWFISR_D2HSM2R_RD_INT_MASK       (1 << HWFISR_D2HSM2R_RD_INT_OFFSET)
#define HWFISR_RD_TIMEOUT_INT_OFFSET     (3)
#define HWFISR_RD_TIMEOUT_INT_MASK       (1 << HWFISR_RD_TIMEOUT_INT_OFFSET)
#define HWFISR_WR_TIMEOUT_INT_OFFSET     (4)
#define HWFISR_WR_TIMEOUT_INT_MASK       (1 << HWFISR_WR_TIMEOUT_INT_OFFSET)
#define HWFISR_TX_EVENT_0_OFFSET         (8)
#define HWFISR_TX_EVENT_0_MASK           (1 << HWFISR_TX_EVENT_0_OFFSET)
#define HWFISR_RX_EVENT_0_OFFSET         (12)
#define HWFISR_RX_EVENT_0_MASK           (1 << HWFISR_RX_EVENT_0_OFFSET)
#define HWFISR_RX_EVENT_1_OFFSET         (13)
#define HWFISR_RX_EVENT_1_MASK           (1 << HWFISR_RX_EVENT_1_OFFSET)
#define HWFISR_H2D_SW_INT_OFFSET         (16)
#define HWFISR_H2D_SW_INT_MASK           (0xffff << HWFISR_H2D_SW_INT_OFFSET)

/*********************SDIO FW HWFIER*****************************/
#define HWFIER_DRV_SET_FW_OWN_INT_EN_OFFSET     (0)
#define HWFIER_DRV_SET_FW_OWN_INT_EN_MASK       (1 << HWFIER_DRV_SET_FW_OWN_INT_EN_OFFSET)
#define HWFIER_DRV_CLR_FW_OWN_INT_EN_OFFSET     (1)
#define HWFIER_DRV_CLR_FW_OWN_INT_EN_MASK       (1 << HWFIER_DRV_CLR_FW_OWN_INT_EN_OFFSET)
#define HWFIER_D2HSM2R_RD_INT_EN_OFFSET         (2)
#define HWFIER_D2HSM2R_RD_INT_EN_MASK           (1 << HWFIER_D2HSM2R_RD_INT_EN_OFFSET)
#define HWFIER_RD_TIMEOUT_INT_EN_OFFSET         (3)
#define HWFIER_RD_TIMEOUT_INT_EN_MASK           (1 << HWFIER_RD_TIMEOUT_INT_EN_OFFSET)
#define HWFIER_WR_TIMEOUT_INT_EN_OFFSET         (4)
#define HWFIER_WR_TIMEOUT_INT_EN_MASK           (1 << HWFIER_WR_TIMEOUT_INT_EN_OFFSET)
#define HWFIER_TX_EVENT_0_INT_EN_OFFSET         (8)
#define HWFIER_TX_EVENT_0_INT_EN_MASK           (1 << HWFIER_TX_EVENT_0_INT_EN_OFFSET)
#define HWFIER_RX_EVENT_0_INT_EN_OFFSET         (12)
#define HWFIER_RX_EVENT_0_INT_EN_MASK           (1 << HWFIER_RX_EVENT_0_INT_EN_OFFSET)
#define HWFIER_RX_EVENT_1_INT_EN_OFFSET         (13)
#define HWFIER_RX_EVENT_1_INT_EN_MASK           (1 << HWFIER_RX_EVENT_1_INT_EN_OFFSET)
#define HWFIER_H2D_SW_INT_EN_OFFSET             (16)
#define HWFIER_H2D_SW_INT_EN_MASK               (0xffff << HWFIER_H2D_SW_INT_EN_OFFSET)


/*********************SDIO FW HWFICR*****************************/
#define HWFICR_FW_OWN_BACK_INT_SET_OFFSET   (4)
#define HWFICR_FW_OWN_BACK_INT_SET_MASK     (1 << HWFICR_FW_OWN_BACK_INT_SET_OFFSET)
#define HWFICR_D2H_SW_INT_SET_OFFSET        (8)
#define HWFICR_D2H_SW_INT_SET_MASK          (0xffffff << HWFICR_D2H_SW_INT_SET_OFFSET)


/*********************SDIO FW HWDMACR*****************************/
#define HWFICR_DEST_BST_TYP_OFFSET   (1)
#define HWFICR_DEST_BST_TYP_MASK     (1 << HWFICR_DEST_BST_TYP_OFFSET)
#define HWFICR_DMA_BST_SIZE_OFFSET   (6)
#define HWFICR_DMA_BST_SIZE_MASK     (0x03 << HWFICR_DMA_BST_SIZE_OFFSET)


/*********************SDIO FW HWRQ0CR*****************************/
#define HWRQ0CR_RXQ0_PACKET_LENGTH_OFFSET   (0)
#define HWRQ0CR_RXQ0_PACKET_LENGTH_MASK     (0xffff << HWRQ0CR_RXQ0_PACKET_LENGTH_OFFSET)
#define HWRQ0CR_RXQ0_DMA_STOP_OFFSET        (16)
#define HWRQ0CR_RXQ0_DMA_STOP_MASK          (1 << HWRQ0CR_RXQ0_DMA_STOP_OFFSET)
#define HWRQ0CR_RXQ0_DMA_START_OFFSET       (17)
#define HWRQ0CR_RXQ0_DMA_START_MASK         (1 << HWRQ0CR_RXQ0_DMA_START_OFFSET)
#define HWRQ0CR_RXQ0_DMA_RUM_OFFSET         (18)
#define HWRQ0CR_RXQ0_DMA_RUM_MASK           (1 << HWRQ0CR_RXQ0_DMA_RUM_OFFSET)
#define HWRQ0CR_RXQ0_DMA_STATUS_OFFSET      (19)
#define HWRQ0CR_RXQ0_DMA_STATUS_MASK        (1 << HWRQ0CR_RXQ0_DMA_STATUS_OFFSET)


/*********************SDIO FW HWRQ1CR*****************************/
#define HWRQ1CR_RXQ1_PACKET_LENGTH_OFFSET   (0)
#define HWRQ1CR_RXQ1_PACKET_LENGTH_MASK     (0xffff << HWRQ1CR_RXQ1_PACKET_LENGTH_OFFSET)
#define HWRQ1CR_RXQ1_DMA_STOP_OFFSET        (16)
#define HWRQ1CR_RXQ1_DMA_STOP_MASK          (1 << HWRQ1CR_RXQ1_DMA_STOP_OFFSET)
#define HWRQ1CR_RXQ1_DMA_START_OFFSET       (17)
#define HWRQ1CR_RXQ1_DMA_START_MASK         (1 << HWRQ1CR_RXQ1_DMA_START_OFFSET)
#define HWRQ1CR_RXQ1_DMA_RUM_OFFSET         (18)
#define HWRQ1CR_RXQ1_DMA_RUM_MASK           (1 << HWRQ1CR_RXQ1_DMA_RUM_OFFSET)
#define HWRQ1CR_RXQ1_DMA_STATUS_OFFSET      (19)
#define HWRQ1CR_RXQ1_DMA_STATUS_MASK        (1 << HWRQ1CR_RXQ1_DMA_STATUS_OFFSET)

/*********************SDIO FW HWTPCCR*****************************/
#define HWTPCCR_INC_TQ_CNT_OFFSET     (0)
#define HWTPCCR_INC_TQ_CNT_MASK       (0xff << HWTPCCR_INC_TQ_CNT_OFFSET)
#define HWTPCCR_TQ_INDEX_OFFSET       (12)
#define HWTPCCR_TQ_INDEX_MASK         (0x0f << HWTPCCR_TQ_INDEX_OFFSET)
#define HWTPCCR_TQ_CNT_RESET_OFFSET   (16)
#define HWTPCCR_TQ_CNT_RESET_MASK     (1 << HWTPCCR_TQ_CNT_RESET_OFFSET)

/*********************SDIO SW RESET*******************************/
#define NORMAL_SW_RST                 (TOP_CFG_AON_BASE + 0x120)
#define SDIO_SW_RST_OFFSET            (17)
#define SDIO_SW_RST_MASK              (1 << SDIO_SW_RST_OFFSET)
/************************ SDIO_SLAVE register definition end line  *******************************
 */

/******************* GDMA definition start ******************/
#define GDMA1_base  ((uint32_t)0x34408000)
#define GDMA2_base  ((uint32_t)0x34408080)
#define GDMA3_base  ((uint32_t)0x34408100)

#define GDMA1       ((GDMA_REGISTER_T *) (GDMA1_base))
#define GDMA2       ((GDMA_REGISTER_T *) (GDMA2_base))
#define GDMA3       ((GDMA_REGISTER_T *) (GDMA3_base))

typedef struct {
    __IO uint32_t GDMA_INT_FLAG;
    __IO uint32_t GDMA_INT_EN;
    __IO uint32_t GDMA_START;
    __IO uint32_t GDMA_RST;
    __IO uint32_t GDMA_STOP;
    __IO uint32_t GDMA_FLUSG;
    __IO uint32_t GDMA_CON;
    __IO uint32_t GDMA_SRC;
    __IO uint32_t GDMA_DST;
    __IO uint32_t GDMA_LEN1;
    __IO uint32_t GDMA_LEN2;
    __IO uint32_t GDMA_JUMP;
    __IO uint32_t GDMA_INTERANL_BUF;
    __IO uint32_t GDMA_CONNECT;
    __IO uint32_t GDMA_AXI_ATTR;
    __IO uint32_t GDMA_SEC_EN;
    __IO uint32_t GDMA_SEC_LATCH_ADDR;
    __IO uint32_t GDMA_SEC_ABORT;
    __IO uint32_t GDMA_HW_DCM;
    __IO uint32_t GDMA_RESERVE1;
    __IO uint32_t GDMA_SEC_DEBUG;
    __IO uint32_t GDMA_RESERVE2;
    __IO uint32_t GDMA_RESERVE3;
    __IO uint32_t GDMA_RESERVE4;
    __IO uint32_t GDMA_SRC_ADDR2;
    __IO uint32_t GDMA_DST_ADDR2;
    __IO uint32_t GDMA_JUMP_ADDR2;
} GDMA_REGISTER_T;

#define GDMA_STOP_BIT_OFFSET          (0)
#define GDMA_STOP_BIT_MASK            (0x1<< GDMA_STOP_BIT_OFFSET)
#define GDMA_STOP_CLR_BIT_MASK        (0x0<< GDMA_STOP_BIT_OFFSET)

#define GDMA_INT_FLAG_CLR_BIT_OFFSET  (0)
#define GDMA_INT_FLAG_CLR_BIT_MASK    (0x0<<GDMA_INT_FLAG_CLR_BIT_OFFSET)

#define GDMA_START_BIT_OFFSET         (0)
#define GDMA_START_BIT_MASK           (0x1<< GDMA_START_BIT_OFFSET)

/******************* GDMA definition end ******************/

#ifdef __cplusplus
}
#endif

#endif  /* __MT7933_H__ */
