/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2020. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

#ifndef __HAL_ECC_INTERNAL_H__
#define __HAL_ECC_INTERNAL_H__


#define ECC_SRAM_SIZE_DWORD         (17)



/***********************************
 * ECC register map
 ***********************************/

/*
 * Define hardware registers base address.
 */
#ifndef ECC_HW_BASE
#define ECC_HW_BASE                        0x34400000
#endif /* #ifndef ECC_HW_BASE */

#define ECC_ADDR_RW_IRQ_CFG                (ECC_HW_BASE + 0x0000)

#define ECC_ADDR_RW_CLK_CFG                (ECC_HW_BASE + 0x0004)

#define ECC_ADDR_RW_SWRST                  (ECC_HW_BASE + 0x0008)

#define ECC_ADDR_RW_SRAM_PARAM             (ECC_HW_BASE + 0x000c)

/*
 * Define ECC configuration.
 */
#define ECC_ADDR_RW_ECC_CFG                (ECC_HW_BASE + 0x0010)


#define ECC_ADDR_RW_SECIF_CFG              (ECC_HW_BASE + 0x0014)


#define ECC_ADDR_RW_MEM_ADDR               (ECC_HW_BASE + 0x0020)


/*
 * Description   : write sram data bus.
 * Width(bit)    : 32
 * Access        : RW
 * Default Value : 32'b0
*/
#define ECC_ADDR_RW_WDATA_00               (ECC_HW_BASE + 0x0080)
#define ECC_ADDR_RW_WDATA_01               (ECC_HW_BASE + 0x0084)
#define ECC_ADDR_RW_WDATA_02               (ECC_HW_BASE + 0x0088)
#define ECC_ADDR_RW_WDATA_03               (ECC_HW_BASE + 0x008c)
#define ECC_ADDR_RW_WDATA_04               (ECC_HW_BASE + 0x0090)
#define ECC_ADDR_RW_WDATA_05               (ECC_HW_BASE + 0x0094)
#define ECC_ADDR_RW_WDATA_06               (ECC_HW_BASE + 0x0098)
#define ECC_ADDR_RW_WDATA_07               (ECC_HW_BASE + 0x009c)


/*
 * Clear IRQ
 */
#define ECC_ADDR_WO_ECC_IRQ_CLR            (ECC_HW_BASE + 0x0100)

/*
 * ECC trigger command.
 */
#define ECC_ADDR_WO_ECC_TRIG               (ECC_HW_BASE + 0x0104)

#define ECC_ADDR_WO_SRAM_WR                (ECC_HW_BASE + 0x0110)
#define ECC_ADDR_WO_SRAM_RD                (ECC_HW_BASE + 0x0114)

#define ECC_ADDR_WO_SEC_TX_TRIG            (ECC_HW_BASE + 0x0118)
#define ECC_ADDR_WO_SEC_RX_TRIG            (ECC_HW_BASE + 0x011c)



#define ECC_ADDR_RO_ECC_STATUS             (ECC_HW_BASE + 0x0200)

#define ECC_ADDR_RO_FSM_SECIF              (ECC_HW_BASE + 0x0208)


/*
    Private Register
*/
#define ECC_ADDR_RO_FSM_SUB                (ECC_HW_BASE + 0x020c)
#define ECC_ADDR_RO_FSM_SIGN               (ECC_HW_BASE + 0x0210)
#define ECC_ADDR_RO_FSM_VERIFY             (ECC_HW_BASE + 0x0214)
#define ECC_ADDR_RO_FSM_DG                 (ECC_HW_BASE + 0x0218)
#define ECC_ADDR_RO_FSM_DQ                 (ECC_HW_BASE + 0x021c)


/*
 * Description   : read sram data bus.
 * Width(bit)    : 32
 * Access        : RO
 * Default Value : 32'b0
*/
#define ECC_ADDR_RO_RDATA_00               (ECC_HW_BASE + 0x0280)
#define ECC_ADDR_RO_RDATA_01               (ECC_HW_BASE + 0x0284)
#define ECC_ADDR_RO_RDATA_02               (ECC_HW_BASE + 0x0288)
#define ECC_ADDR_RO_RDATA_03               (ECC_HW_BASE + 0x028c)
#define ECC_ADDR_RO_RDATA_04               (ECC_HW_BASE + 0x0290)
#define ECC_ADDR_RO_RDATA_05               (ECC_HW_BASE + 0x0294)
#define ECC_ADDR_RO_RDATA_06               (ECC_HW_BASE + 0x0298)
#define ECC_ADDR_RO_RDATA_07               (ECC_HW_BASE + 0x029c)


/* **********************************
 * definite values
 * **********************************/

#define ECC_VALUE_ZERO                     (0)
#define ECC_VALUE_ONE                      (0x1)

#define ECC_CFG_NIST_P_192                 (0x00)
#define ECC_CFG_NIST_P_224                 (0x01)
#define ECC_CFG_NIST_P_256                 (0x02)
#define ECC_CFG_NIST_P_384                 (0x03)
#define ECC_CFG_NIST_P_521                 (0x04)

#define ECC_CFG_CMD_ECDSA_SIGN             (0x00)
#define ECC_CFG_CMD_ECDSA_VERIFY           (0x10)
#define ECC_CFG_CMD_ECDH_DG                (0x20)
#define ECC_CFG_CMD_ECDH_DQ                (0x30)
#define ECC_CFG_MODE_NORMAL                (0x000)
#define ECC_CFG_MODE_SECURE                (0x100)
#define ECC_CFG_KG_K_INDEPEND_ENABLE       (0x10000)

#define ECC_CLK_CFG_ECLK_EN                (0x1)     /* ecc clock enable */
#define ECC_CLK_CFG_PCLK_EN                (0x10)    /* secif clock enable */

#define ECC_STATUS_MASK_BUSY               (0x1)
#define ECC_STATUS_MASK_ERROR              (0x10000)

#define HAL_ECC_SECIF_CFG_RX_MASK              (0x1)

#define HAL_ECC_SECIF_CFG_TX_MASK              (0x10000)

#define HAL_ECC_SECIF_CFG_TX_TRANSMIT_DQX      (0x0)
#define HAL_ECC_SECIF_CFG_TX_TRANSMIT_DQY      (0x100000)

#define HAL_ECC_SECIF_CFG_TX_LAST              (0x1)

/* **********************************
 * sram address map
 * **********************************/

/* SW fill */
#define ECC_SRAM_ADDR_CON_P             0x00
#define ECC_SRAM_ADDR_CON_N             0x01
#define ECC_SRAM_ADDR_CON_P_NEG         0x02
#define ECC_SRAM_ADDR_CON_N_NEG         0x03
#define ECC_SRAM_ADDR_CON_22N_P         0x04
#define ECC_SRAM_ADDR_CON_22N_N         0x05
#define ECC_SRAM_ADDR_CON_A             0x06
#define ECC_SRAM_ADDR_CON_ONE           0x07

/*
    SW fill: ECDSA_VERIFY
    SW fill: ECDH_DQ_normal
    HW output: ECDH_DG
    HW output: ECDH_DQ_normal
*/
#define ECC_SRAM_ADDR_Q_X               0x08
#define ECC_SRAM_ADDR_Q_Y               0x09

/* SW fill */
#define ECC_SRAM_ADDR_SW_D              0x0a
#define ECC_SRAM_ADDR_SW_K              0x0b
#define ECC_SRAM_ADDR_E                 0x0c

#define ECC_SRAM_ADDR_R                 0x0d  /* HW output: ECDSA_SIGN */
#define ECC_SRAM_ADDR_S                 0x0e  /* HW output: ECDSA_SIGN */
#define ECC_SRAM_ADDR_V                 0x0f  /* HW output: ECDSA_VERIFY */


#define TOP_CLK_OFF_BASE_ADDR   (0x30020000)
#define ECC_CLK_CTL             ((uint32_t *)(TOP_CLK_OFF_BASE_ADDR + 0x0304))
#define RG_ECC_CLK_SEL          (0x1 << 0)

typedef enum {
    ECC_CLK_SRC_XTAL = 0,                 /**<  select cm33 clk source as xtal */
    ECC_CLK_SRC_DIV_300M,                  /**<  select cm33 clk source as 300M by DIV */
} top_cm33_ecc_clock_control_t;


#define HAL_ECC_KEYSIZE_BYTE_P_192  24
#define HAL_ECC_KEYSIZE_BYTE_P_224  28
#define HAL_ECC_KEYSIZE_BYTE_P_256  32
#define HAL_ECC_KEYSIZE_BYTE_P_384  48
#define HAL_ECC_KEYSIZE_BYTE_P_521  68


#endif /* #ifndef __HAL_ECC_INTERNAL_H__ */
