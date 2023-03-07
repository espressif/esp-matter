/*Copyright Statement:
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

#ifndef __SNFC_H__
#define __SNFC_H__

#include <stdint.h>
#include "gpt_sw.h"


/* Flash opcodes. */
#define NOR_OP_WREN     0x06    /* Write enable */
#define NOR_OP_RDSR     0x05    /* Read status register */
#define NOR_OP_WRSR     0x01    /* Write status register 1 byte */
#define NOR_OP_READ     0x03    /* Read data bytes (low frequency) */
#define NOR_OP_READ_FAST    0x0b    /* Read data bytes (high frequency) */
#define NOR_OP_READ_1_1_2   0x3b    /* Read data bytes (Dual SPI) */
#define NOR_OP_READ_1_1_4   0x6b    /* Read data bytes (Quad SPI) */
#define NOR_OP_READ_2IO 0xbb    /* Read data bytes (2xIO SPI, MXIC) */
#define NOR_OP_READ_4IO 0xeb    /* Read data bytes (4xIO SPI, MXIC) */
#define NOR_OP_PP           0x02    /* Page program (up to 256 bytes) */
#define NOR_OP_BE_4K        0x20    /* Erase 4KiB block */
#define NOR_OP_BE_4K_PMC    0xd7    /* Erase 4KiB block on PMC chips */
#define NOR_OP_BE_32K       0x52    /* Erase 32KiB block */
#define NOR_OP_CHIP_ERASE   0xc7    /* Erase whole flash chip */
#define NOR_OP_BE_64K       0xd8    /* Sector erase (usually 64KiB) */
#define NOR_OP_RDID         0x9f    /* Read JEDEC ID */
#define NOR_OP_UNIQID       0x4B    /* Winbond Read Unique ID Number */
#define NOR_OP_REMS     0x90
#define NOR_OP_RDCR     0x15    /* Read configuration register */
#define NOR_OP_RDFSR        0x70    /* Read flag status register */
#define NOR_OP_RSTEN        0x66
#define NOR_OP_RST          0x99
#define NOR_OP_EN4B     0xb7    /* Enter 4-byte mode */
#define NOR_OP_EX4B     0xe9    /* Exit 4-byte mode */
#define NOR_OP_RDP          0xab    /* Release from deep power-down */
#define NOR_OP_ENDP     0xb9    /* Enter deep power-down */

#define NOR_OP_SUSPEND_MXIC 0xb0    /* MXIC flash suspend */
#define NOR_OP_RESUME_MXIC  0x30    /* MXIC flash resume */
#define NOR_OP_RDSCUR_MXIC  0x2b    /* Read security register */
#define NOR_OP_SUSPEND_WINBOND  0x75    /* Winbond flash suspend */
#define NOR_OP_RESUME_WINBOND   0x7A    /* Winbond flash resume */

/* 4-byte address opcodes - used on Spansion and some Macronix flashes. */
#define NOR_OP_READ4        0x13    /* Read data bytes (low frequency) */
#define NOR_OP_READ4_FAST   0x0c    /* Read data bytes (high frequency) */
#define NOR_OP_READ4_1_1_2  0x3c    /* Read data bytes (Dual SPI) */
#define NOR_OP_READ4_1_1_4  0x6c    /* Read data bytes (Quad SPI) */
#define NOR_OP_PP_4B        0x12    /* Page program (up to 256 bytes) */
#define NOR_OP_SE_4B        0xdc    /* Sector erase (usually 64KiB) */

/* Used for SST flashes only. */
#define NOR_OP_BP           0x02    /* Byte program */
#define NOR_OP_WRDI     0x04    /* Write disable */
/* Auto address increment word program */
#define NOR_OP_AAI_WP       0xad

/* Used for Macronix and Winbond flashes. */
#define NOR_OP_RDCR_MXIC    0x15

/* Used for Spansion flashes only. */
#define NOR_OP_BRWR     0x17    /* Bank register write */

/* Used for Micron flashes only. */
#define NOR_OP_RD_EVCR      0x85    /* Read EVCR register */
#define NOR_OP_WD_EVCR      0x81    /* Write EVCR register */

#ifndef BIT
#define BIT(n) (1UL << (n))
#endif /* #ifndef BIT */

/* Status Register bits. */
#define SR_WIP          BIT(0) /* Write in progress */
#define SR_WEL          BIT(1) /* Write enable latch */

/* meaning of other SR_* bits may differ between vendors */
#define SR_BP0          BIT(2) /* Block protect 0 */
#define SR_BP1          BIT(3) /* Block protect 1 */
#define SR_BP2          BIT(4) /* Block protect 2 */
/* Block protect 3(not all vendor has this bit)*/
#define SR_BP3          BIT(5)
#define SR_BP3_MT       BIT(6)

#define SR_SRWD         BIT(7) /* SR write protect */

#define SR_QUAD_EN_MX       BIT(6) /* Macronix Quad I/O */

/* Enhanced Volatile Configuration Register bits */
#define EVCR_QUAD_EN_MICRON BIT(7) /* Micron Quad I/O */

/* Flag Status Register bits */
#define FSR_READY       BIT(7)

/* Configuration Register bits. */
#define CR_QUAD_EN_SPAN     BIT(1) /* Spansion Quad I/O */

#define NOR_JEDEC_ID_LEN    3 /*we only use 3 bytes, it's enough for us */
#define NOR_UNIQUE_ID_LEN   8 /* Winbond use 8 bytes unique ID to each winbond nor device */

/* Used for Spansion flashes only */
#define SPANSION_OP_RDAR    0x65
#define SPANSION_OP_WRAR    0x71
#define SPANSION_SR1NV      0x0
#define SPANSION_CR1NV      0x2
#define SPANSION_SR1V       0x800000
#define SPANSION_SR2V       0x800001
#define SPANSION_CR1V       0x800002
#define SPANSION_CR2NV      0x3
#define SPANSION_CR2V       0x800003
#define SPANSION_CR3V       0x800004

#define HHB(d)      ((uint8_t)((uint32_t)(d) >> 24))
#define HLB(d)      ((uint8_t)(((d) >> 16) & 0xff))
#define LHB(d)      ((uint8_t)(((d) >> 8) & 0xff))
#define LLB(d)      ((uint8_t)((d) & 0xff))

struct snfc_delsel {
    uint8_t sio_0_in; /*REG_SF_DELSEL0 bits 3:0*/
    uint8_t sio_0_out; /*REG_SF_DELSEL2 bits 3:0*/
    uint8_t sio_1_in; /*REG_SF_DELSEL2 bits 7:4*/
    uint8_t sio_1_out; /*REG_SF_DELSEL1 bits 3:0*/
    uint8_t sio_2_in; /*REG_SF_DELSEL3 bits 7:4*/
    uint8_t sio_2_out; /*REG_SF_DELSEL3 bits 3:0*/
    uint8_t sio_3_in; /*REG_SF_DELSEL4 bits 7:4*/
    uint8_t sio_3_out; /*REG_SF_DELSEL4 bits 3:0*/
    uint8_t sck_out; /*REG_SF_DELSEL0 bits 7:4*/
};

enum {
    /* Read protocol mode */
    SF_SINGLE_READ = 0,
    SF_FAST_READ   = 1,
    SF_1_1_2_READ  = 2,
    SF_2XIO_READ   = 3,
    SF_1_1_4_READ  = 4,
    SF_4XIO_READ   = 5,

    /* Read controller path */
    SF_PIO_PATH    = 0,
    SF_DIRECT_PATH = 1,
    SF_DMA_PATH    = 2,

    /* Write controller mode */
    SF_BYTE_WRITE       = 0,
    SF_PAGE_WRITE       = 1,
    SF_MULTI_PAGE_WRITE = 2,

    /* Erase protocol mode */
    SF_4K_ERASE     = 0,
    SF_32K_ERASE    = 1,
    SF_64K_ERASE    = 2,
    SF_CHIP_ERASE   = 3,

    /* Addr width */
    SF_ADDR_WIDTH_3B = 3,
    SF_ADDR_WIDTH_4B = 4,
};

#define NOR_MF_ATMEL        0x1f
#define NOR_MF_INTEL        0x89
//#define NOR_MF_MICRON       0x20
#define NOR_MF_XMC       0x20
#define NOR_MF_MACRONIX     0xc2
#define NOR_MF_ISSI     0x9d
#define NOR_MF_ESMT     0x8c
#define NOR_MF_WINBOND      0xef
#define NOR_MF_SPANSION     0x01
#define NOR_MF_SST      0xbf
#define NOR_MF_GIGADEVICE   0xc8

struct snfc_handler {
    const struct spi_nor *nor;
    const struct snfc_ops *ops;
    struct snfc_property *sp;
};

struct spi_nor {
    const char *name;
    uint8_t manufact;
    uint8_t mem_type;
    uint8_t mem_density;
    uint32_t page_size;
    uint32_t sect_size;
    uint32_t chip_size;
    uint32_t read_cap;
};

struct snfc_property {
    unsigned long reg_base;
    unsigned long mem_base;
    int buf_size;
    int addr_width;
    int read_mode;
    int read_path;
    int erase_mode;
    int write_mode;
};

struct snfc_ops {
    int (*read_reg)(struct snfc_handler *sh, uint8_t opcode, uint8_t *buf, int len);
    int (*write_reg)(struct snfc_handler *sh, uint8_t opcode, uint8_t *buf, int len);

    int (*set_delsel)(struct snfc_handler *sh, struct snfc_delsel *ds);
    int (*get_irq_status)(struct snfc_handler *sh, uint32_t *status);
    int (*clr_irq_status)(struct snfc_handler *sh);

    int (*set_addr_width)(struct snfc_handler *sh, int addr_width);
    int (*set_read_path)(struct snfc_handler *sh, int read_path);
    int (*set_read_mode)(struct snfc_handler *sh, int read_mode);
    int (*set_erase_mode)(struct snfc_handler *sh, int erase_mode);
    int (*set_write_mode)(struct snfc_handler *sh, int write_mode);

    int (*read)(struct snfc_handler *sh, uint32_t addr, uint8_t *buf, uint32_t len);
    int (*write)(struct snfc_handler *sh, uint32_t addr, const uint8_t *buf, uint32_t len);
    int (*erase)(struct snfc_handler *sh, uint32_t addr, uint32_t len);

    int (*nor_reset)(struct snfc_handler *sh);
    int (*nor_write_protect)(struct snfc_handler *sh, bool enable);
    int (*nor_get_status)(struct snfc_handler *sh, uint8_t *status);
    int (*nor_get_config)(struct snfc_handler *sh, uint8_t *config);
    int (*wait_nor_ready)(struct snfc_handler *sh, uint32_t tmo, uint32_t interval);
};


#define US_DELAY(u) hal_gpt_delay_us(u)

#endif /* #ifndef __SNFC_H__ */
