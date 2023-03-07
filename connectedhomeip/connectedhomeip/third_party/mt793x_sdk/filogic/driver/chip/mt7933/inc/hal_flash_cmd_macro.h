/* Copyright Statement:
 *
 * (C) 2005-2017  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc.
 * ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part,
 * shall be strictly prohibited.
 * You may only use, reproduce, modify,
 * or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit
 * permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY.
 * MEDIATEK EXPRESSLY DISCLAIMS
 * ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY
 * ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM
 * ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE
 * FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO
 * CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND
 * EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT
 * TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */


#ifndef __SFLASH_H__
#define __SFLASH_H__

#include "common.h"
#include "hal_flash_reg.h"
#include "memory_attribute.h"

#define udelay(u) hal_gpt_delay_us(u)
#define ERASE_TIMEOUT       (60 * 1000 * 1000ULL)
#define ST_BUSY_TIMEOUT     (20 * 1000 * 1000ULL)
#define CMD_TIMEOUT     (10 * 1000 * 1000ULL)
#define CHECK_INTERVAL_10_US    10
#define CHECK_INTERVAL_100_US   100
#define host_wp_off()       SF_WRITEL(REG_SF_WRPROT, SF_WP_TURN_OFF)
#define host_en_buf_wr()    SF_WRITEL(REG_SF_CFG2, SF_BUF_WRITE_EN)
#define host_dis_buf_wr()   SF_WRITEL(REG_SF_CFG2, SF_BUF_WRITE_DIS)


/* Command types which defined in REG_SF_CMD */
#define CMD_TYPE_RD     REG_SF_CMD_RD
#define CMD_TYPE_RDSR       REG_SF_CMD_RDSR_CMD
#define CMD_TYPE_PRG        REG_SF_CMD_PRG_CMD
#define CMD_TYPE_ERASE  REG_SF_CMD_ERASE_CMD
#define CMD_TYPE_WR     REG_SF_CMD_WR_CMD
#define CMD_TYPE_WRSR       REG_SF_CMD_WRSR_CMD
#define CMD_TYPE_AUTOINC    REG_SF_CMD_AUTO_INCR

#define SF_ADDR_WIDTH_4B        4
#define SF_ADDR_WIDTH_3B        3

#define SF_LARGE_ADDR_EN    REG_SF_DUAL_LARGE_ADDR_EN
#define SF_READ_ADDR_QUAD   REG_SF_DUAL_ADDR_QUAD
#define SF_READ_QUAD_EN     REG_SF_DUAL_QUAD_READ_EN


/* control valued defined in REG_SF_WRPROT */
#define SF_WP_TURN_OFF  0x30

/* control value defined in REG_SF_CFG2 */
#define SF_BUF_WRITE_EN 0x1
#define SF_BUF_WRITE_DIS    0x0


#define NOR_OP_WREN     0x06    /* Write enable */
#define NOR_OP_RDSR     0x05    /* Read status register */
#define NOR_OP_WRSR     0x01    /* Write status register 1 byte */
#define NOR_OP_READ     0x03    /* Read data bytes (low frequency) */
#define NOR_OP_READ_4IO 0xeb    /* Read data bytes (4xIO SPI, MXIC) */
#define NOR_OP_BE_4K        0x20    /* Erase 4KiB block */
#define NOR_OP_BE_32K       0x52    /* Erase 32KiB block */
#define NOR_OP_CHIP_ERASE   0xc7    /* Erase whole flash chip */
#define NOR_OP_SE   0xd8    /* Sector erase (usually 64KiB) */
#define NOR_OP_RDID 0x9f    /* Read JEDEC ID */
#define NOR_OP_RDCR 0x35    /* Read configuration register */
#define NOR_OP_RSTEN    0x66
#define NOR_OP_RST  0x99
#define NOR_OP_EN4B 0xb7    /* Enter 4-byte mode */
#define NOR_OP_EX4B 0xe9    /* Exit 4-byte mode */
#define NOR_OP_RDP  0xab    /* Release from deep power-down */
#define NOR_OP_ENTER_DP 0xB9
#define NOR_OP_AAI_WP   0xad
#define NOR_OP_RDCR_MXIC    0x15


/* Used for Winbond flashes only */
#define NOR_OP_SEL_DIE      0xc2

/* Status Register bits. */
#define SR_WIP          BIT(0)  /* Write in progress */
#define SR_WEL          BIT(1)  /* Write enable latch */

#define SR_QUAD_EN_MX       BIT(6)  /* Macronix Quad I/O */
#define CR_QUAD_EN_SPAN     BIT(1)  /* Spansion Quad I/O */

#define SPANSION_OP_WRAR    0x71
#define SPANSION_CR2V_ADDR  0x800003
#define SPANSION_CR3V_ADDR  0x800004
#define SPANSION_CR3NV_ADDR 0x04

#define SPANSION_FL_L_TYPE_ID   0x60
#define SPANSION_LATENCY_CODE   0x5
#define SPANSION_MODE_CYCLE     0x2
#define SPANSION_4KB_ERASE_DIS  0x8

/*OTP*/
#define WINBOND_OTP_REGION_SIZE         0x300         /* WB has three 256bytes security registers */
#define WINBOND_OTP_SR_SIZE             0x100         /* one of security registers size is 0x100 */
#define WINBOND_OTP_SR_ADDR_1           0x001000      /* security register1 base addr */
                                                      /* security register2 base addr  WINBOND_OTP_SR_ADDR_1*2 */
                                                      /* security register3 base addr  WINBOND_OTP_SR_ADDR_1*3 */
#define WINBOND_OTP_WRITE               0x42          /* Program OTP Security Registers */
#define WINBOND_OTP_READ                0x48          /* Read OTP Security Registers */

#define MXIC_OTP_REGION_SIZE            0x200
#define MXIC_ENSO                       0xB1          /* Enter Secured OTP (ENSO) */
#define MXIC_EXSO                       0xC1          /* Exit Secured OTP (EXSO) */
#define MXIC_OTP_RDSCUR                 0x2B          /* read security register */
#define MXIC_OTP_WRSCUR                 0x2F          /* write security register */



#define SF_WRITEB(offset, value) \
        DRV_WriteReg8(sflash.reg_base + (offset), value)

#define SF_READB(offset) \
        DRV_Reg8(sflash.reg_base + (offset))

#define SF_WRITEL(offset, value) \
        DRV_WriteReg32(sflash.reg_base + (offset), value)

#define SF_READL(offset) \
        DRV_Reg32(sflash.reg_base + (offset))

#define HHB(d)      ((u8)((u32)(d) >> 24))
#define HLB(d)      ((u8)(((d) >> 16) & 0xff))
#define LHB(d)      ((u8)(((d) >> 8) & 0xff))
#define LLB(d)      ((u8)((d) & 0xff))

/* For CQ_DMA */
#define CQ_DMA_G_EN ((U32)(CQ_DMA_BASE + 0x8))
#define CQ_DMA_G_SRC_ADDR   ((U32)(CQ_DMA_BASE + 0x1c))
#define CQ_DMA_G_DEST_ADDR  ((U32)(CQ_DMA_BASE + 0x20))
#define CQ_DMA_G_DMA_0_LEN1 ((U32)(CQ_DMA_BASE + 0x24))
#define CQ_DMA_G_DMA_0_SEC_EN   0x3440803C

#define SF_8_BITS_DUMMY_CYCLES  0x0


/* Host register interfaces */
extern struct sf_desc sflash;

enum sf_read_mode {
    SINGLE_READ,
    FAST_READ,
    DUALIO_READ,
    QUADIO_READ,
};

enum sf_read_path {
    PIO_PATH,
    MEMCOPY_PATH,
    DMA_PATH,
};


enum sf_erase_mode {
    ERASE_CHIP,
    ERASE_SEC,
    ERASE_32K_BLK,
    ERASE_4K_BLK,
};


enum Manufacture {
    NOR_MF_ATMEL = 0x1f,
    NOR_MF_INTEL = 0x89,
    //NOR_MF_MICRON = 0x20,
    NOR_MF_XMC = 0x20,
    NOR_MF_MACRONIX  = 0xc2,
    NOR_MF_ISSI = 0x9d,
    NOR_MF_ESMT = 0x1c,
    NOR_MF_WINBOND = 0xef,
    NOR_MF_SPANSION = 0x01,
    NOR_MF_SST = 0xbf,
    NOR_MF_GIGADEVICE = 0xc8
};

struct nor_flash_info {
    u8      manufact;
    u8      mem_type;
    u8      mem_density;
    u8      die_num;
    u32     chip_size;
    u32     sect_size;
    //const char  *flash_name;
};


struct sf_desc {
    unsigned long    reg_base;
    unsigned long    map_base;
    unsigned int     wrbuf_size;
    unsigned int    addr_width;
    enum sf_read_mode read_mode;
};

enum Err_type {
    NO_ERROR = 0,
    ERR_NOT_FOUND = -1,
    ERR_NOT_VALID = -2,
    ERR_INVALID_ARGS = -3,
    ERR_TIMED_OUT = -4,
    ERR_NOT_SUPPORTED = -5,
    ERR_BUSY = -6,
    ERR_BAD_STATE = -7,
};


/* recore info for wake-up */
struct info_record {
    U8 io_mode;
    U8 address_mode;
    U8 frequency;
    U8 manu_id;
    U8 io_delay;
};

struct sf_info_record_table {
    int count;
    struct info_record *infos;
};


enum SF_CLK {
    FREQUENCY_26M = 0,
    FREQUENCY_37M = 1,
    FREQUENCY_46M = 2,
    FREQUENCY_60M = 3,
};


enum OTP_Lock_Status {
    UNLOCK = 0,
    LOCK = 1,
};


#define SF_SIZE_16M     0x1000000
#define SF_SIZE_32K     0x8000
#define SF_SIZE_4K      0x1000
#define ALIGNED_32K(x)  (!((x) & (u32)(SF_SIZE_32K - 1)))
#define ALIGNED_4K(x)   (!((x) & (u32)(SF_SIZE_4K - 1)))

#define DUMMY_CFG_EN    0x80


#endif /* #ifndef __SFLASH_H__ */

