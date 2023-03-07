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
 
#ifndef __MT_REG_BASE
#define __MT_REG_BASE

#include <stdint.h>

//#define TCM_BASE                   0x00100000
#define CM33_CFG_BASE                0x20000000
#define CACHE_BASE                   0x20080000
#define CACHE_MPU_BASE               0x20090000
#define IRQ_CFG_BASE                 0x21000000
#define APXGPT_BASE                  0x21010000
#define WDT_BASE                     0x21020000
#define CM33_GPT_BASE                0x21030000
#define CM33_UART_BASE               0x21040000
#define CM33_REG_BASE               0x21050000
#define CHIP_MISC_BASE               0x21060000
#define CHIP_RESOURCE_BASE           0x21080000
#define SPIS_TEST_MODE_BASE          0x21090000
#define CM33_BCRM_BASE               0x210A0000
#define FIREWALL_ROUTING_BASE        0x30000000
#define TOP_CFG_BASE                 0x30000000
#define EFUSE_CTRL_0_BASE            0x30010000
#define EFUSE_CTRL_1_BASE            0x30020000
#define TOP_CLK_OFF_BASE             0x30020000
#define TOP_CFG_AON_BASE             0x30030000
#define PWM0_BASE                    0x30040000
#define PWM1_BASE                    0x30050000
#define PWM2_BASE                    0x30060000
#define RTC_BASE                     0x30070000
#define SEJ_BASE                     0x30090000
#define SECURITY_WRAPPER_BASE        0x300A0000
#define PMU_CTRL_BASE                0x300B0000
#define PLL_CTRL_BASE                0x300C0000
#define SPM_TOP_BASE                 0x300D0000
#define XTAL_CTRL_BASE               0x300E0000
#define INFRA_DEVAPC_AON_BASE        0x30300000
#define INFRA_BCRM_AON_BASE          0x30400000
#define INFRA_DEBUG_CTRL_AON_BASE    0x30401000
#define PSRAM_BCRM_BASE              0x30402000
#define CHIP_PINMUX_BASE             0x30403000
#define GPIO_BASE_BASE               0x30404000
#define I2C0_BASE                    0x30480000
#define I2C1_BASE                    0x30481000
#define AP_UART0_BASE                0x30482000
#define AP_UART1_BASE                0x30483000
#define CM33_DSP_SIM_MDM_BASE        0x30600000
#define INFRA_REG_BASE               0x34000000
#define SYSRAM_CTRL_BASE             0x34020000
#define TOP_CFG_OFF_BASE             0x34030000
#define OTF_AES_BASE                 0x34040000
#define CPUM_BASE                    0x34050000
#define GCPU_SEC_BASE                0x34401000
#define GCPU_NON_SEC_BASE            0x34402000
#define TRNG_BASE                    0x34404000
#define INFRA_BCRM_OFF_BASE          0x34405000
#define INFRA_DEVAPC_BASE            0x34406000
#define AP_DMA_BASE                  0x34407000
#define CQ_DMA_BASE                  0x34408000
#define ADC_BASE                     0x38000000
#define SPIM0_BASE                   0x38010000
#define SPIM1_BASE                   0x38020000
#define SPIS_BASE                    0x38030000
#define BTIF_HOST_BASE               0x38040000
#define IRRX_BASE                    0x38050000
#define KEYPAD_BASE                  0x38060000
#define NON_UHS_PSRAM_APB_SLAVE_BASE 0x38070000
#define UHS_PSRAMC_NAO_BASE          0x38080000
#define UHS_PSRAMC_AO_BASE           0x38090000
#define UHS_DDRPHY_NAO_BASE          0x380A0000
#define UHS_DDRPHY_AO_BASE           0x380B0000
#define UHS_PEMI_BASE                0x380C0000
#define USB_BASE                     0x38100000
#define USB_SIF_BASE                 0x38110000
#define SDIOM_BASE                   0x38120000
#define SDIOS_BASE                   0x38130000
#define DSP_SRAM0_BASE               0x40040000
#define DSP_SRAM1_BASE               0x40050000
#define DSP_SRAM2_BASE               0x40060000
#define DSP_SRAM3_BASE               0x40070000
#define AUD_TOP_BASE                 0x40080000
#define AUD_SYS_TOP_BASE             0x40090000
#define DSP_TIMER_BASE               0x41000000
#define DSP_UART_BASE                0x41001000
#define DSP_IRQ_CFG_BASE             0x41002000
#define DSP_REG_BASE                 0x41003000
#define DSP_WDT_BASE                 0x41003400
#define AUDIO_BCRM_BASE              0x41004000
#define AUDIO_DAPC_AO_BASE           0x41005000
#define AUDIO_PDN_BCRM_BASE          0x41008000
#define AUDIO_DAPC_PDN_BASE          0x41009000
#define KBCRM_BASE                   0x4100A000
#define DEBUG_BASE                   0x4100B000
#define SYSCFG_AO_BASE               0x4100C000
#define CONNAC_AP2CONN_BASE          0x60000000
#define SYSRAM_TOP_BASE              0x80000000
#define GRP1_EEF_TOP                 0x30405000
#define GRP2_EEF_TOP                 0x30406000
#define EFUSE_TOP                    0x30407000

#ifndef __IO
#define     __IO    volatile             /*!< Defines 'read / write' permissions */
#endif

/* structure type to access the CACHE register
 */
typedef struct {
    __IO uint32_t CACHE_CON;
    __IO uint32_t CACHE_OP;
    __IO uint32_t CACHE_HCNT0L;
    __IO uint32_t CACHE_HCNT0U;
    __IO uint32_t CACHE_CCNT0L;
    __IO uint32_t CACHE_CCNT0U;
    __IO uint32_t CACHE_HCNT1L;
    __IO uint32_t CACHE_HCNT1U;
    __IO uint32_t CACHE_CCNT1L;
    __IO uint32_t CACHE_CCNT1U;
    uint32_t RESERVED0[1];
    __IO uint32_t CACHE_REGION_EN;
    uint32_t RESERVED1[16372];                  /**< (0x10000-12*4)/4 */
    __IO uint32_t CACHE_ENTRY_N[16];
    __IO uint32_t CACHE_END_ENTRY_N[16];
} CACHE_REGISTER_T;

/* CACHE_CON register definitions
 */
#define CACHE_CON_MCEN_OFFSET                   (0)
#define CACHE_CON_MCEN_MASK                     (0x1UL<<CACHE_CON_MCEN_OFFSET)

#define CACHE_CON_CNTEN0_OFFSET                 (2)
#define CACHE_CON_CNTEN0_MASK                   (0x1UL<<CACHE_CON_CNTEN0_OFFSET)

#define CACHE_CON_CNTEN1_OFFSET                 (3)
#define CACHE_CON_CNTEN1_MASK                   (0x1UL<<CACHE_CON_CNTEN1_OFFSET)

#define CACHE_CON_MDRF_OFFSET                   (7)
#define CACHE_CON_MDRF_MASK                     (0x1UL<<CACHE_CON_MDRF_OFFSET)

#define CACHE_CON_CACHESIZE_OFFSET              (8)
#define CACHE_CON_CACHESIZE_MASK                (0x3UL<<CACHE_CON_CACHESIZE_OFFSET)

/* CACHE_OP register definitions
 */
#define CACHE_OP_EN_OFFSET                      (0)
#define CACHE_OP_EN_MASK                        (0x1UL<<CACHE_OP_EN_OFFSET)

#define CACHE_OP_OP_OFFSET                      (1)
#define CACHE_OP_OP_MASK                        (0xFUL<<CACHE_OP_OP_OFFSET)

#define CACHE_OP_TADDR_OFFSET                   (5)
#define CACHE_OP_TADDR_MASK                     (0x7FFFFFFUL<<CACHE_OP_TADDR_OFFSET)

/* CACHE_HCNT0L register definitions
 */
#define CACHE_HCNT0L_CHIT_CNT0_MASK             (0xFFFFFFFFUL<<0)

/* CACHE_HCNT0U register definitions
 */
#define CACHE_HCNT0U_CHIT_CNT0_MASK             (0xFFFFUL<<0)

/* CACHE_CCNT0L register definitions
 */
#define CACHE_CCNT0L_CACC_CNT0_MASK             (0xFFFFFFFFUL<<0)

/* CACHE_CCNT0U register definitions
 */
#define CACHE_CCNT0U_CACC_CNT0_MASK             (0xFFFFUL<<0)

/* CACHE_HCNT1L register definitions
 */
#define CACHE_HCNT1L_CHIT_CNT1_MASK             (0xFFFFFFFFUL<<0)

/* CACHE_HCNT1U register definitions
 */
#define CACHE_HCNT1U_CHIT_CNT1_MASK             (0xFFFFUL<<0)

/* CACHE_CCNT1L register definitions
 */
#define CACHE_CCNT1L_CACC_CNT1_MASK             (0xFFFFFFFFUL<<0)

/* CACHE_CCNT1U register definitions
 */
#define CACHE_CCNT1U_CACC_CNT1_MASK             (0xFFFFUL<<0)

/* CACHE_ENTRY_N register definitions
 */
#define CACHE_ENTRY_N_C_MASK                    (0x1UL<<8)
#define CACHE_ENTRY_BASEADDR_MASK               (0xFFFFFUL<<12)

/* CACHE_END_ENTRY_N register definitions
 */
#define CACHE_END_ENTRY_N_BASEADDR_MASK         (0xFFFFFUL<<12)

/* memory mapping of MT7687
 */
#define CACHE                                   ((CACHE_REGISTER_T *)CACHE_BASE)

/* CACHE register End
 */


#endif

