/*
 * MediaTek Inc. (C) 2018. All rights reserved.
 *
 * Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

#ifndef __SNFC_REG_H__
#define __SNFC_REG_H__

/* ----------------- Register Definitions ------------------- */
#define REG_SF_CMD                  0x00000000
#define REG_SF_CMD_RD               BIT(0)
#define REG_SF_CMD_RDSR_CMD         BIT(1)
#define REG_SF_CMD_PRG_CMD          BIT(2)
#define REG_SF_CMD_ERASE_CMD            BIT(3)
#define REG_SF_CMD_WR_CMD           BIT(4)
#define REG_SF_CMD_WRSR_CMD         BIT(5)
#define REG_SF_CMD_AUTO_INCR            BIT(7)
#define REG_SF_CNT                  0x00000004
#define REG_SF_CNT_SF_CNT           GENMASK(15, 0)
#define REG_SF_RDSR                 0x00000008
#define REG_SF_RDSR_RDSR_DATA           GENMASK(7, 0)
#define REG_SF_RDATA                    0x0000000c
#define REG_SF_RDATA_SF_RDATA           GENMASK(7, 0)
#define REG_SF_RADR0                    0x00000010
#define REG_SF_RADR0_SFP_ADR0           GENMASK(7, 0)
#define REG_SF_RADR1                    0x00000014
#define REG_SF_RADR1_SFP_ADR1           GENMASK(7, 0)
#define REG_SF_RADR2                    0x00000018
#define REG_SF_RADR2_SFP_ADR2           GENMASK(7, 0)
#define REG_SF_WDATA                    0x0000001c
#define REG_SF_WDATA_SFP_WR_DATA        GENMASK(7, 0)
#define REG_SF_PRGDATA0                 0x00000020
#define REG_SF_PRGDATA0_SFP_PRGDATA0        GENMASK(7, 0)
#define REG_SF_PRGDATA1                 0x00000024
#define REG_SF_PRGDATA1_SFP_PRGDATA1        GENMASK(7, 0)
#define REG_SF_PRGDATA2                 0x00000028
#define REG_SF_PRGDATA2_SFP_PRGDATA2        GENMASK(7, 0)
#define REG_SF_PRGDATA3                 0x0000002c
#define REG_SF_PRGDATA3_SFP_PRGDATA3        GENMASK(7, 0)
#define REG_SF_PRGDATA4                 0x00000030
#define REG_SF_PRGDATA4_SFP_PRGDATA4        GENMASK(7, 0)
#define REG_SF_PRGDATA5                 0x00000034
#define REG_SF_PRGDATA5_SFP_PRGDATA5        GENMASK(7, 0)
#define REG_SF_SHREG0                   0x00000038
#define REG_SF_SHREG0_SHIFT_REG0        GENMASK(7, 0)
#define REG_SF_SHREG1                   0x0000003c
#define REG_SF_SHREG1_SHIFT_REG1        GENMASK(7, 0)
#define REG_SF_SHREG2                   0x00000040
#define REG_SF_SHREG2_SHIFT_REG2        GENMASK(7, 0)
#define REG_SF_SHREG3                   0x00000044
#define REG_SF_SHREG3_SHIFT_REG3        GENMASK(7, 0)
#define REG_SF_SHREG4                   0x00000048
#define REG_SF_SHREG4_SHIFT_REG4        GENMASK(7, 0)
#define REG_SF_SHREG5                   0x0000004c
#define REG_SF_SHREG5_SHIFT_REG5        GENMASK(7, 0)
#define REG_SF_SHREG6                   0x00000050
#define REG_SF_SHREG6_SHIFT_REG6        GENMASK(7, 0)
#define REG_SF_SHREG7                   0x00000054
#define REG_SF_SHREG7_SHIFT_REG7        GENMASK(7, 0)
#define REG_SF_SHREG8                   0x00000058
#define REG_SF_SHREG8_SHIFT_REG8        GENMASK(7, 0)
#define REG_SF_SHREG9                   0x0000005c
#define REG_SF_SHREG9_SHIFT_REG9        GENMASK(7, 0)
#define REG_SF_CFG1                 0x00000060
#define REG_SF_CFG1_FAST_READ           BIT(0)
#define REG_SF_CFG1_FL_MON_SEL          GENMASK(7, 6)
#define REG_SF_CFG2                 0x00000064
#define REG_SF_CFG2_BUF2WR_EN_SET       BIT(0)
#define REG_SF_CFG2_PRG_WR_OPCODE_EN        BIT(4)
#define REG_SF_CFG2_AAI_CFG         BIT(6)
#define REG_SF_SHREG10                  0x00000068
#define REG_SF_SHREG10_SHIFT_REG10      GENMASK(7, 0)
#define REG_FL_MODE_MON                 0x0000006c
#define REG_FL_STATUS0                  0x00000070
#define REG_FL_STATUS0_FLASH_STATUS     GENMASK(31, 0)
#define REG_FL_STATUS1                  0x00000074
#define REG_FL_STATUS2                  0x00000078
#define REG_FL_STATUS3                  0x0000007c
#define RREG_FLASH_TIME                 0x00000080
#define RREG_FLASH_TIME_LATE_LATCH      BIT(5)
#define RREG_FLASH_CFG                  0x00000084
#define RREG_SFLASH_TIME                0x00000094
#define RREG_SFLASH_TIME_TCSS_REG       GENMASK(2, 0)
#define RREG_SFLASH_TIME_TCSH_REG       GENMASK(6, 4)
#define RREG_SFLASH_TIME_TCSHI_REG      GENMASK(11, 8)
#define REG_SF_PP_DW_DATA               0x00000098
#define REG_SF_PP_DW_DATA_REG_SF_PP_DW_DATA GENMASK(31, 0)
#define REG_SF_DELSEL0                  0x000000a0
#define REG_SF_DELSEL0_SIO0_IN_DELAY_SEL    GENMASK(3, 0)
#define REG_SF_DELSEL0_SCK_OUT_DELAY_SEL    GENMASK(7, 4)
#define REG_SF_DELSEL1                  0x000000a4
#define REG_SF_DELSEL1_SIO1_OUT_DELAY_SEL   GENMASK(3, 0)
#define REG_SF_DELSEL1_SMPCK_INV        BIT(4)
#define REG_SF_INTRSTUS                 0x000000a8
#define REG_SF_INTRSTUS_RD_INT          BIT(0)
#define REG_SF_INTRSTUS_RDSR_INT        BIT(1)
#define REG_SF_INTRSTUS_PRG_INT         BIT(2)
#define REG_SF_INTRSTUS_ERASE_INT       BIT(3)
#define REG_SF_INTRSTUS_WR_INT          BIT(4)
#define REG_SF_INTRSTUS_WRSR_INT        BIT(5)
#define REG_SF_INTRSTUS_AAI_INT         BIT(6)
#define REG_SF_INTRSTUS_DMA_INT         BIT(7)
#define REG_SF_INTREN                   0x000000ac
#define REG_SF_INTREN_RD_INTREN         BIT(0)
#define REG_SF_INTREN_RDSR_INTREN       BIT(1)
#define REG_SF_INTREN_PRG_INTREN        BIT(2)
#define REG_SF_INTREN_ERASE_INTREN      BIT(3)
#define REG_SF_INTREN_WR_INTREN         BIT(4)
#define REG_SF_INTREN_WRSR_INTREN       BIT(5)
#define REG_SF_INTREN_AAI_INTREN        BIT(6)
#define REG_SF_INTREN_DMA_INTREN        BIT(7)
#define REG_SF_CFG3                 0x000000b4
#define REG_SF_CFG3_STUS_POS            GENMASK(2, 0)
#define REG_SF_CFG3_RDY_VLUE            BIT(3)
#define REG_SF_CFG3_PRG_RD_OPCODE_EN        BIT(4)
#define REG_SF_CFG3_POL_RDY_BIT_DIS     BIT(5)
#define REG_SF_CFG3_PRG_WREN_OPCODE_EN      BIT(6)
#define REG_SF_CFG3_WREN_DIS            BIT(7)
#define REG_FL_CHKSUM                   0x000000bc
#define REG_SF_AAICMD                   0x000000c0
#define REG_SF_AAICMD_AAI_WR_CMD        BIT(0)
#define REG_SF_WRPROT                   0x000000c4
#define REG_SF_WRPROT_WRITE_PROTECT     GENMASK(7, 0)
#define REG_SF_RADR3                    0x000000c8
#define REG_SF_RADR3_SFP_ADR3           GENMASK(7, 0)
#define REG_SF_DUAL                 0x000000cc
#define REG_SF_DUAL_DUAL_READ_EN        BIT(0)
#define REG_SF_DUAL_ADDR_DUAL           BIT(1)
#define REG_SF_DUAL_QUAD_READ_EN        BIT(2)
#define REG_SF_DUAL_ADDR_QUAD           BIT(3)
#define REG_SF_DUAL_LARGE_ADDR_EN       BIT(4)
#define REG_SF_DELSEL2                  0x000000d0
#define REG_SF_DELSEL2_SIO0_OUT_DELAY_SEL   GENMASK(3, 0)
#define REG_SF_DELSEL2_SIO1_IN_DELAY_SEL    GENMASK(7, 4)
#define REG_SF_DELSEL3                  0x000000d4
#define REG_SF_DELSEL3_SIO2_OUT_DELAY_SEL   GENMASK(3, 0)
#define REG_SF_DELSEL3_SIO2_IN_DELAY_SEL    GENMASK(7, 4)
#define REG_SF_DELSEL4                  0x000000d8
#define REG_SF_DELSEL4_SIO3_OUT_DELAY_SEL   GENMASK(3, 0)
#define REG_SF_DELSEL4_SIO3_IN_DELAY_SEL    GENMASK(7, 4)
#define REG_SF_PREADY                   0x000000fc
#define REG_SF_SHREG11                  0x00000100
#define REG_SF_SHREG11_SHIFT_REG11      GENMASK(7, 0)
#define REG_SF_SHREG12                  0x00000104
#define REG_SF_SHREG12_SHIFT_REG12      GENMASK(7, 0)
#define REG_SF_SHREG13                  0x00000108
#define REG_SF_SHREG13_SHIFT_REG13      GENMASK(7, 0)
#define REG_SF_SHREG14                  0x0000010c
#define REG_SF_SHREG14_SHIFT_REG14      GENMASK(7, 0)
#define REG_SF_SHREG15                  0x00000110
#define REG_SF_SHREG15_SHIFT_REG15      GENMASK(7, 0)
#define REG_SF_PRGDATAN1                0x00000114
#define REG_SF_PRGDATAN1_SFP_PRGDATAN1      GENMASK(7, 0)
#define REG_SF_PRGDATAN2                0x00000118
#define REG_SF_PRGDATAN2_SFP_PRGDATAN2      GENMASK(7, 0)
#define REG_SF_PRGDATAN3                0x0000011c
#define REG_SF_PRGDATAN3_SFP_PRGDATAN3      GENMASK(7, 0)
#define REG_SF_PRGDATAN4                0x00000120
#define REG_SF_PRGDATAN4_SFP_PRGDATAN4      GENMASK(7, 0)
#define REG_SF_PRGDATAN5                0x00000124
#define REG_SF_PRGDATAN5_SFP_PRGDATAN5      GENMASK(7, 0)
#define REG_SF_PRGDATAN6                0x00000128
#define REG_SF_PRGDATAN6_SFP_PRGDATAN6      GENMASK(7, 0)
#define REG_SF_PRGDATAN7                0x0000012c
#define REG_SF_PRGDATAN7_SFP_PRGDATAN7      GENMASK(7, 0)
#define REG_SF_PRGDATAN8                0x00000130
#define REG_SF_PRGDATAN8_SFP_PRGDATAN8      GENMASK(7, 0)
#define REG_SF_PRGDATAN9                0x00000134
#define REG_SF_PRGDATAN9_SFP_PRGDATAN9      GENMASK(7, 0)
#define REG_SF_PRGDATANA                0x00000138
#define REG_SF_PRGDATANA_SFP_PRGDATANA      GENMASK(7, 0)
#define REG_SF_DUMMY                    0x0000013c
#define REG_CFG1_BRI                    0x00000710
#define REG_CFG1_BRI_WMERGE_EN          BIT(3)
#define REG_CFG1_BRI_WMERGE_PLUS1       BIT(4)
#define REG_CFG1_BRI_WMERGE_EQUAL       BIT(5)
#define REG_CFG1_BRI_CHECK_BVALID       BIT(6)
#define REG_CFG1_BRI_OUTSTAND_W_NUM     GENMASK(15, 12)
#define REG_CFG2_BRI                    0x00000714
#define REG_CFG2_BRI_AWPROT         GENMASK(2, 0)
#define REG_FDMA_CTL                    0x00000718
#define REG_FDMA_CTL_DMA_TRIG           BIT(0)
#define REG_FDMA_CTL_DMA_SW_RST         BIT(1)
#define REG_FDMA_CTL_WDLE_EN            BIT(2)
#define REG_FDMA_FADR                   0x0000071c
#define REG_FDMA_FADR_SFLASH_ADDR       GENMASK(31, 0)
#define REG_FDMA_DADR                   0x00000720
#define REG_FDMA_DADR_WDRAM_ADDR        GENMASK(31, 4)
#define REG_FDMA_END_DADR               0x00000724
#define REG_FDMA_END_DADR_WDRAM_END_ADDR    GENMASK(31, 4)
#define REG_CG_DIS                  0x00000728
#define REG_CG_DIS_CLK_AXI_CG_DIS       BIT(0)
#define REG_CG_DIS_CLK_DRAM_CG_DIS      BIT(1)
#define REG_CG_DIS_SFC_SW_RST           BIT(2)
#define REG_CG_DIS_TOUT_CNT         GENMASK(23, 16)
#define REG_CG_DIS_TOUT_EN          BIT(24)
#define REG_SF_PAGECNT                  0x0000072c
#define REG_SF_PAGECNT_SF_PAGECNT       GENMASK(7, 0)
#define REG_SF_PAGESIZE                 0x00000730
#define REG_SF_PAGESIZE_SF_PAGESIZE     GENMASK(7, 0)
#define REG_MPP_EN                  0x00000734
#define REG_MPP_EN_MPP_EN           BIT(0)
#define REG_FDMA_DADR_HB                0x00000738
#define REG_FDMA_DADR_HB_WDRAM_ADDR_HB      GENMASK(3, 0)
#define REG_FDMA_END_DADR_HB                0x0000073c
#define REG_FDMA_END_DADR_HB_WDRAM_END_ADDR_HB  GENMASK(3, 0)

#endif /* #ifndef __SNFC_REG_H__ */
