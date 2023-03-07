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

#ifndef __MT7933_POS_H__
#define __MT7933_POS_H__

#include "memory_map.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
#ifndef BIT
#define BIT(n)                          (1UL << (n))
#endif /* #ifndef BIT */

#ifndef BITS
/* bits range: for example BITS(16,23) = 0xFF0000
 *   ==>  (BIT(m)-1)   = 0x0000FFFF     ~(BIT(m)-1)   => 0xFFFF0000
 *   ==>  (BIT(n+1)-1) = 0x00FFFFFF
 */
#define BITS(m, n)                      (~(BIT(m)-1) & ((BIT(n) - 1) | BIT(n)))
#endif /* #ifndef BITS */

/* get the n bit of value, binary bit */
#ifndef BBIT
#define BBIT(val, n)               ((((unsigned int)(val)) & (1UL << (n))) >> (n))
#endif /* #ifndef BBIT */
/* get the n bit of value, then reverse it */
#ifndef RBBIT
#define RBBIT(val, n)               ((~(BBIT(val, n))) & 1UL)
#endif /* #ifndef RBBIT */
//---------------------------------------------------------------------------
/*
 * CONNAC AP2CONN: remap 0x60xx_xxxx to 0x18xx_xxxx
 */
#define CON_REG_INFRA_RGU_ADDR                  0x60000000
#define CON_REG_INFRA_CFG_ADDR                  0x60001000
#define CON_REG_INFRA_BUS_ADDR                  0x6000E000
#define CON_REG_INFRA_DBG_AO_ADDR               0x6000F000
#define CON_REG_SPM_BASE_ADDR                   0x60060000
#define BGF_REG_BASE_ADDR                       0x60800000
#define BGF_REG_INFO_BASE_ADDR                  0x60812000

//---------------------------------------------------------------------------
/*
 * ConnInfra RGU Region, 0x1800_0000 ~ 0x1800_0FFF
 */
#define CONN_INFRA_RGU_START                    CON_REG_INFRA_RGU_ADDR

#define CONN_INFRA_RGU_BGFSYS_CPU_SW_RST        (CONN_INFRA_RGU_START + 0x0124)
#define BGF_CPU_SW_RST_B                        BIT(0)

#define CONN_INFRA_RGU_BGFSYS_ON_TOP_PWR_CTL    (CONN_INFRA_RGU_START + 0x0020)
#define CONN_INFRA_RGU_BGFSYS_ON_TOP_PWR_ON     (0x42540080)
#define CONN_INFRA_RGU_BGFSYS_ON_TOP_PWR_OFF_HIGH    (0x42540000)
#define CONN_INFRA_RGU_BGFSYS_ON_TOP_PWR_OFF_LOW     (0xFFFFFF7F)

#define CONN_INFRA_RGU_SYSRAM_HWCTL_PDN_ADDR    (CONN_INFRA_RGU_START + 0x0050)
#define CONN_INFRA_RGU_SYSRAM_HWCTL_PDN_VAL     0x0

#define CONN_INFRA_RGU_SYSRAM_HWCTL_SLP_ADDR    (CONN_INFRA_RGU_START + 0x0054)
#define CONN_INFRA_RGU_SYSRAM_HWCTL_SLP_VAL     0x7

#define CONN_INFRA_RGU_BGFSYS_ON_TOP_PWR_ACK_ST (CONN_INFRA_RGU_START + 0x0414)
#define BGF_ON_PWR_ACK_B                        BITS(24, 25)

#define CONN_INFRA_RGU_BGFSYS_SW_RST_B          (CONN_INFRA_RGU_START + 0x001C)
#define BGF_SW_RST_B                            BIT(0)

#define CONN_INFRA_RGU_BGFSYS_OFF_TOP_PWR_ACK_ST (CONN_INFRA_RGU_START + 0x0424)
#define BGF_OFF_PWR_ACK_B                       BIT(24)
#define BGF_OFF_PWR_ACK_S                       BITS(0, 1)

#define CONN_INFRA_CFG_BT_MANUAL_CTRL           (CONN_INFRA_RGU_START + 0x1108)
#define CONN_INFRA_BT_MANUAL_CTRL_B             BIT(27)

/*
 * ConnInfra CFG Region, 0x1800_1000 ~ 0x1800_1FFF
 */
#define CONN_INFRA_CFG_START            CON_REG_INFRA_CFG_ADDR
#define CONN_HW_VER_OFFSET              (0x0000)
#define CONN_CFG_ID_OFFSET              (0x0004)
#define CONN_HW_VER_E1                  0x02040100
#define CONN_HW_VER_E2                  0x02040200
#define CONN_CFG_ID                     0x3

#define CONN_INFRA_CFG_VERSION          (CONN_INFRA_CFG_START)
#define CONN_INFRA_CFG_ID               (0x02040100)

#define CONN_INFRA_CFG_BT_PWRCTLCR0     (CONN_INFRA_CFG_START + 0x0208)
#define BT_FUNC_EN_B                    BIT(0)

#define CONN_INFRA_CONN2BT_GALS_SLP_CTL     (CONN_INFRA_CFG_START + 0x0550)
#define CONN2BT_SLP_PROT_RX_EN_B        BIT(4)
#define CONN2BT_SLP_PROT_TX_EN_B        BIT(0)

#define CONN_INFRA_CONN2BT_GALS_SLP_STATUS  (CONN_INFRA_CFG_START + 0x0554)
#define CONN2BT_SLP_PROT_RX_ACK_B       BIT(22)
#define CONN2BT_SLP_PROT_TX_ACK_B       BIT(23)

#define CONN_INFRA_BT2CONN_GALS_SLP_CTL     (CONN_INFRA_CFG_START + 0x0560)
#define BT2CONN_SLP_PROT_RX_EN_B        BIT(4)
#define BT2CONN_SLP_PROT_TX_EN_B        BIT(0)

#define CONN_INFRA_BT2CONN_GALS_SLP_STATUS  (CONN_INFRA_CFG_START + 0x0564)
#define BT2CONN_SLP_PROT_RX_ACK_B       BIT(22)
#define BT2CONN_SLP_PROT_TX_ACK_B       BIT(23)

#define CONN_INFRA_CFG_EMI_CTL_BT_EMI_REQ_BT    (CONN_INFRA_CFG_START + 0x0418)
#define BT_EMI_CTRL_BIT             BIT(0)

#define CONN_INFRA_CFG_DBG_MUX_SEL_CONN_INFRA_DBG_SELECTION    (CONN_INFRA_CFG_START + 0x0B00)
#define CONN_INFRA_DBG_SELECTION_B      BITS(0, 3)

/*
 * ConnInfra BUS Region, 0x1800_E000 ~ 0x1800_EFFF
 */
#define CONN_INFRA_LIGHT_SECURITY_CTRL          (CON_REG_INFRA_BUS_ADDR + 0x238)
#define CONN_INFRA_OFF_TIMEOUT_SETTING          (CON_REG_INFRA_BUS_ADDR + 0x300)
#define CONN_INFRA_ON_TIMEOUT_SETTING           (CON_REG_INFRA_BUS_ADDR + 0x31C)
#define CONN_INFRA_CONN2AP_EMI_DET_START_ADDR   (CON_REG_INFRA_BUS_ADDR + 0x350)
#define CONN_INFRA_CONN2AP_EMI_DET_START_VAL    0x09000000
#define CONN_INFRA_CONN2AP_EMI_DET_END_ADDR     (CON_REG_INFRA_BUS_ADDR + 0x354)
#define CONN_INFRA_CONN2AP_EMI_DET_END_VAL      0x09FFFFFF


/*
 * ConnInfra CFG Region, 0x1800_F000 ~ 0x1800_FFFF
 */
#define CONN_INFRA_BUS_HANG_DETECT              (CONN_INFRA_RGU_START + 0xF000)
#define CONN_INFRA_CHECK_ACTION_BEFORE_WAKEUP   (CONN_INFRA_RGU_START + 0x5128)

/*
 * Connsys Host CSR Top Region, 0x1806_0000 ~ 0x1806_FFFF
 */
#define CONN_HOST_CSR_TOP_START                 CON_REG_SPM_BASE_ADDR
#define CONN_INFRA_WAKEUP_BT                    (CONN_HOST_CSR_TOP_START + 0x01A8)
#define CONN_INFRA_WAKE_BT_B                    BIT(0)
#define CONN_HOST_CSR_TOP_CONN_SLP_PROT_CTRL    (CONN_HOST_CSR_TOP_START + 0x0184)
#define CONN_INFRA_ON2OFF_SLP_PROT_ACK          BIT(5)
#define CONN_MCU_PC                             (CONN_HOST_CSR_TOP_START + 0x22C)

/*
 * CONN2AP remapping setting
 */
#define CONN2AP_REMAP_MCU_EMI_BASE_ADDR             0x600601c4  // CONNSYS: 0x18xx_xxxx to AP: 0x60xx_xxxx
#define CONN2AP_REMAP_MCU_EMI_ADDR_VAL              0x03040000  // CONNSYS: 0xF000_0000 to AP: 0x0304_0000(EFUSE)
#define CONN2AP_REMAP_MD_SHARE_EMI_BASE_ADDR        0x600601cc  // CONNSYS: 0x18xx_xxxx to AP: 0x60xx_xxxx
#ifdef MTK_WIFI_EMI_IN_PSRAM
#define CONN2AP_REMAP_MD_SHARE_EMI_ADDR_VAL         ((RAM_WIFI_EXT_BASE & 0xffff0000))
#else /* #ifdef MTK_WIFI_EMI_IN_PSRAM */
#define CONN2AP_REMAP_MD_SHARE_EMI_ADDR_VAL         ((RAM_WIFI_EXT_BASE & 0x00ffffff) | 0x90000000)  // CONNSYS: 0xF400_0000 to AP: ref *.LD (FLASH)
#endif /* #ifdef MTK_WIFI_EMI_IN_PSRAM */
#define CONN2AP_REMAP_GPS_EMI_BASE_ADDR             0x600601d0  // CONNSYS: 0x18xx_xxxx to AP: 0x60xx_xxxx
#define CONN2AP_REMAP_GPS_EMI_ADDR_VAL              0x0A000000  // CONNSYS: 0xF800_0000 to AP: 0x0A00_0000(PSRAM)
#define CONN2AP_REMAP_WF_PERI_BASE_ADDR             0x600601d4  // CONNSYS: 0x18xx_xxxx to AP: 0x60xx_xxxx
#define CONN2AP_REMAP_WF_PERI_ADDR_VAL              0x0300D000  // CONNSYS: 0xFD00_0000 to AP: 0x0300_D000(WFDMA)
#define CONN2AP_REMAP_BT_PERI_BASE_ADDR             0x600601d8  // CONNSYS: 0x18xx_xxxx to AP: 0x60xx_xxxx
#define CONN2AP_REMAP_BT_PERI_ADDR_VAL              0x08000000  // CONNSYS: 0xFE00_0000 to AP: 0x0800_0000(SYSRAM)
#define CONN2AP_REMAP_GPS_PERI_BASE_ADDR            0x600601dc  // CONNSYS: 0x18xx_xxxx to AP: 0x60xx_xxxx
#define CONN2AP_REMAP_GPS_PERI_ADDR_VAL             0x0300e000  // CONNSYS: 0xFF00_0000 to AP: 0x0300_E000(XO_CTL_TOP_REG)
#define CONN2AP_REMAP_SCPSYS_SRAM_BASE_ADDR         0x600601e0  // CONNSYS: 0x18xx_xxxx to AP: 0x60xx_xxxx
#define CONN2AP_REMAP_SCPSYS_SRAM_ADDR_VAL          0x03003000  // CONNSYS: 0x7DE0_0000 to AP: 0x0300_3000(32k clock ctl)

/*
 * AP2CONN_INFRA_ON
 *
 * 1. check host_ck CG status
 *  0x3003_0040[1] should be 1'b1
 * 2. check ap2conn gals sleep protect status
 *  0x3003_8010[31]/0x3003_8410[0] (tx/rx)(sleep protect ready) both of them
 *  should be 1'b0 (CR at ap side)
 */
#define BGF_CONN_INFRA_ON_HOST_CLK_CG 0x30030040
#define BGF_CONN_INFRA_ON_GALS_TX     0x30038010
#define BGF_CONN_INFRA_ON_GALS_RX     0x30038410

/*
 * AP2CONN_INFRA_OFF
 *
 * 1. chek AP2CONN INFRA ON step is ok
 * 2. check conn_infra off bus clock
 *  - write 0x1 to 0x1806_0000[0], reset clock detect
 *  - 0x1806_0000[2] conn_infra off bus clock(should be 1'b1 if clock exist)
 *  - 0x1806_0000[1] osc clock(should be 1'b1 if clock exist)
 * 3. read conn_infra IP version
 *  - read 0x1800_1000 = 0x02060000
 * 4. check conn_infra off domain bus hang irq status
 *  - 0x1806_02D4[1], should be 1'b1, or means conn_infra off bus might hang
 *  - the next Register check which part might be hang
 *    1) 0x1806014c[0], should be 1'b1, means conn_infra main bus timeout for VDNR timeout mechanism
 *    2) 0x18060448[0] should be 1'b0, or means conn_infra axi layer bus timeout for VDNR timeout mechanism
 *    3) 0x18060434[0] should be 1'b1, or means conn_infra_on timeout for AHB/APB timeout mechanism
 *    4) 0x18060434[1] should be 1'b1, or means conn_infra_off timeout for AHB/APB timeout mechanism
 *  - if bus hang, dump CONN_INFRA_DBG
 */
#define BGF_CONN_INFRA_OFF_BUS_CLK      CON_REG_SPM_BASE_ADDR
#define BGF_CONN_INFRA_OFF_IP_VER       CON_REG_INFRA_CFG_ADDR
#define BGF_CONN_INFRA_IP_VERSION       0x02040100
#define BGF_CONN_INFRA_OFF_BUS_HANG_IRQ_ST (BGF_CONN_INFRA_OFF_BUS_CLK + 0x02D4)
#define BGF_CONN_INFRA_OFF_VDNR_MAIN_BUS_TOUT (BGF_CONN_INFRA_OFF_BUS_CLK + 0x014c)
#define BGF_CONN_INFRA_OFF_VDNR_AXI_LAYER_BUS_TOUT (BGF_CONN_INFRA_OFF_BUS_CLK + 0x0448)
#define BGF_CONN_INFRA_OFF_AHB_APB_BUS_TOUT (BGF_CONN_INFRA_OFF_BUS_CLK + 0x0434)
#define BGF_CONN_INFRA_DEBUG_AO_TOUT     CON_REG_INFRA_DBG_AO_ADDR
#define BGF_CONN_INFRA_DEBUG_AO_ST_BASE (CON_REG_INFRA_DBG_AO_ADDR + 0x0408)
#define BGF_CONN_INFRA_AHB_APB_TOUT_MECH (CON_REG_SPM_BASE_ADDR + 0x0410)
#define BGF_CONN_INFRA_AHB_APB_TOUT_A1  (CON_REG_SPM_BASE_ADDR + 0x0414)
#define BGF_CONN_INFRA_AHB_APB_TOUT_A2  (CON_REG_SPM_BASE_ADDR + 0x0418)
#define BGF_CONN_INFRA_AHB_APB_TOUT_A3  (CON_REG_SPM_BASE_ADDR + 0x042C)
#define BGF_CONN_INFRA_AHB_APB_TOUT_A4  (CON_REG_SPM_BASE_ADDR + 0x0430)
#define BGF_CONN_INFRA_AHB_APB_TOUT_A5  (CON_REG_SPM_BASE_ADDR + 0x041C)
#define BGF_CONN_INFRA_AHB_APB_TOUT_A6  (CON_REG_SPM_BASE_ADDR + 0x0420)
#define BGF_CONN_INFRA_SLEEP_PRO_A1     (CON_REG_INFRA_RGU_ADDR + 0x1514)
#define BGF_CONN_INFRA_SLEEP_PRO_A2     (CON_REG_INFRA_RGU_ADDR + 0xE2A0)
#define BGF_CONN_INFRA_SLEEP_PRO_A3     (CON_REG_INFRA_RGU_ADDR + 0x1504)
#define BGF_CONN_INFRA_SLEEP_PRO_A4     (CON_REG_INFRA_RGU_ADDR + 0xE2A4)
#define BGF_CONN_INFRA_SLEEP_PRO_A5     (CON_REG_INFRA_RGU_ADDR + 0x1534)
#define BGF_CONN_INFRA_SLEEP_PRO_A6     (CON_REG_INFRA_RGU_ADDR + 0x1524)
#define BGF_CONN_INFRA_SLEEP_PRO_A7     (CON_REG_INFRA_RGU_ADDR + 0x1544)
#define BGF_CONN_INFRA_SLEEP_PRO_A8     (CON_REG_INFRA_RGU_ADDR + 0x1554)
#define BGF_CONN_INFRA_SLEEP_PRO_A9     (CON_REG_INFRA_RGU_ADDR + 0xE2AC)
#define BGF_CONN_INFRA_SLEEP_PRO_A10    (CON_REG_INFRA_RGU_ADDR + 0x1564)
#define BGF_CONN_INFRA_SLEEP_PRO_A11    (CON_REG_INFRA_RGU_ADDR + 0xE1A8)

// Wt_slp clock check, read 0x6000_904C[1:0] should be 2'b01
#define BGF_CONN_WT_SLEEP_CLK (CON_REG_INFRA_RGU_ADDR + 0x904C)
//conn_infra cfg status, read 0x6000_92C8
#define BGF_CONN_INFRA_CFG_ST (CON_REG_INFRA_RGU_ADDR + 0x92C8)

/*
 * VDNR current state
 * write addr: 0x6006_0138, read addr: 0x6006_0150
 */
#define BGF_CONN_VNDR_CUR_ST_WRITE (CON_REG_SPM_BASE_ADDR + 0x0138)
#define BGF_CONN_VNDR_CUR_ST_READ  (CON_REG_SPM_BASE_ADDR + 0x0150)

/*
 * AP2WF
 *
 * 1. check "AP2CONN_INFRA_ON step is ok" and check "AP2CONN_INFRA_OFF step is ok"
 * 2. check conn2wf sleep protect
 *  - 0x1800_1544[29:28](sleep protect enable ready), should be 1'b0
 * 3. read WF IP version
 *  - read 0x184F_0000 = 0x02050000
 * 4. check wf_mcusys bus hang irq status
 *  - read 0x1806_016C[0] = 1'b0
 */
#define BGF_CONN_WF_MCUSYS_BUS_HANG_IRQ     (CON_REG_SPM_BASE_ADDR + 0x016C)
#define BGF_CONN_MCUSYS_POWER_ST            (CON_REG_SPM_BASE_ADDR + 0x0294) //power status

/*
 * AP2BT
 *
 * 1. check "AP2CONN_INFRA_ON step is ok" and check "AP2CONN_INFRA_OFF step is ok"
 * 2. check conn2bt gals tx/rx sleep protect
 *   - 0x1800_1554[31]/0x1800_1554[22]/0x1800_1554[23](sleep protect enable ready),
 *     both of them should be 1'b0
 */

/*
 * AP2BT BT Wakeup part
 * write 3b'010 to 0x1806_015c[2:0], read 0x1806_02c8[30] is bgf_osc_on_ack
 */
#define BGF_CONN_INFRA_CFG_CLK_WR (CON_REG_SPM_BASE_ADDR + 0x015C)
#define BGF_CONN_INFRA_CFG_CLK_RD (CON_REG_SPM_BASE_ADDR + 0x02C8)

/*
 * BGF SYS BUS TIMEOUT
 *
 * bgf sys bus hang, dump register from 0x6080_0410 ~ 0x6080_0444 (14 registers)
 */
#define BGF_SYS_BUS_HANG_BASE_ADDR (BGF_REG_BASE_ADDR + 0x0410)
#define BGF_SYS_BUS_HANG_ADDR_ADD(base, delta) ((unsigned int)(base + delta))

/*
 * BGF SYS SLEEP FAIL
 *
 * bt debug log signal address: write: 0x180600AC, read: 0x1806023C
 */
#define BGF_BT_TOP_SIGNAL_WR (CON_REG_SPM_BASE_ADDR + 0x00AC)
#define BGF_BT_TOP_SIGNAL_RD (CON_REG_SPM_BASE_ADDR + 0x023C)
#define BGF_SYSTRAP_RD_DEBUG (CON_REG_SPM_BASE_ADDR + 0x0230)

/*
 * BGFSYS Control Register Region:
 *     (BGFSYS_BASE) ~ (BGFSYS_BASE + BGFSYS_LENGTH)
 */
#define BGF_MCCR                        (BGF_REG_BASE_ADDR + 0x0100)
#define BGF_CON_CR_AHB_AUTO_DIS         BIT(31)
#define BGF_MCCR_SET                    (BGF_REG_BASE_ADDR + 0x0104)
#define BGF_CON_CR_AHB_STOP             BIT(2)
#define BGF_EMI_CTL_ST                 (BGF_REG_BASE_ADDR + 0x0150)
#define BGF_FW_LOG_NOTIFY               BIT(24)
#define BGF_SW_IRQ_ASSERT_SUBSYS        BIT(25)
#define BGF_SW_IRQ_ASSERT_WHOLE         BIT(26)
#define BGF_EMI_CTL_CLR                 (BGF_REG_BASE_ADDR + 0x014C)
#define BGF_SW_IRQ_CLR_BT               BIT(25)
#define BGF_MCU_CFG_SW_DBG_CTL          (BGF_REG_BASE_ADDR + 0x016C)

#define BGF_HW_VERSION                  BGF_REG_INFO_BASE_ADDR
#define BGF_HW_VER_ID                   0x00008A00

#define BGF_FW_VERSION                  (BGF_REG_INFO_BASE_ADDR + 0x04)
#define BGF_FW_VER_ID                   0x00008A00

#define BGF_HW_CODE                     (BGF_REG_INFO_BASE_ADDR + 0x08)
#define BGF_HW_CODE_ID                  0x00000000

#define BGF_IP_VERSION                  (BGF_REG_INFO_BASE_ADDR + 0x10)
#define BGF_IP_VER_ID                   0x02040100

//---------------------------------------------------------------------------
#define SET_BIT(addr, bit) \
            (*((volatile uint32_t *)(addr))) |= ((uint32_t)bit)
#define CLR_BIT(addr, bit) \
            (*((volatile uint32_t *)(addr))) &= ~((uint32_t)bit)

//---------------------------------------------------------------------------
#define BITS_PER_LONG 32
#define GENMASK(h, l) \
    (((~0U) - (1U << (l)) + 1) & (~0U >> (BITS_PER_LONG - 1 - (h))))

#define GET_BIT_MASK(value, mask) ((value) & (mask))
#define SET_BIT_MASK(pdest, value, mask) (*(pdest) = (GET_BIT_MASK(*(pdest), ~(mask)) | GET_BIT_MASK(value, mask)))
#define GET_BIT_RANGE(data, end, begin) ((data) & GENMASK(end, begin))
#define SET_BIT_RANGE(pdest, data, end, begin) (SET_BIT_MASK(pdest, data, GENMASK(end, begin)))

#define CONSYS_REG_READ(addr) DRV_Reg32(addr)
#define CONSYS_REG_READ_BIT(addr, BITVAL) (*((volatile unsigned int *)(addr)) & ((unsigned int)(BITVAL)))
#define CONSYS_REG_WRITE(addr, data)  DRV_WriteReg32(addr, data)
#define CONSYS_REG_WRITE_RANGE(reg, data, end, begin) {\
    unsigned int v = CONSYS_REG_READ(reg); \
    SET_BIT_RANGE(&v, data, end, begin); \
    CONSYS_REG_WRITE(reg, v); \
}

/*
 * Write value with value_offset bits of right shift and size bits,
 * to the reg_offset-th bit of address reg
 * value  -----------XXXXXXXXXXXX-------------------
 *                   |<--size-->|<--value_offset-->|
 * reg    -------------OOOOOOOOOOOO-----------------
 *                     |<--size-->|<--reg_offset-->|
 * result -------------XXXXXXXXXXXX-----------------
 */
#define CONSYS_REG_WRITE_OFFSET_RANGE(reg, value, reg_offset, value_offset, size) ({\
    unsigned int d = (value) >> (value_offset); \
    d = GET_BIT_RANGE(d, size, 0); \
    d = d << (reg_offset); \
    CONSYS_REG_WRITE_RANGE(reg, d, ((reg_offset) + ((size) - 1)), reg_offset); \
})

//---------------------------------------------------------------------------
extern int consys_polling_chipid(void);
extern void consys_emi_set_remapping_reg(void);
extern void consys_bus_protect_timeout(void);
extern void consys_enable_bus_hang_detect(void);
extern void consys_select_cfg_selection(void);
extern void ap_bus_req_rising_handler(hal_nvic_irq_t irq);
extern void apsrc_req_rising_handler(hal_nvic_irq_t irq);
extern void mt7933_conninfra_init(void);
extern void connsys_power_on(void);
extern void connsys_power_off(void);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef __MT7933_POS_H__ */

