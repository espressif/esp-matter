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

#ifndef __HAL_MSDC_H__
#define __HAL_MSDC_H__

#include "hal_platform.h"
#include <string.h>
#include "stdio.h"
#include "hal_sd.h"
#include "hal_mtk_sd.h"
#include "hal_sdio.h"

#define SDIO_DEFAULT_MAX_SPEED    (25000)  /*The max speed is 25MHZ in default.*/

#define MSDC_CLOCK_SOURCE  (50000)  /*48000KHz*/

#define MSDC_DATA_LINE_DRIVING_CAPABILITY 2
#define SDIO_MASTER_BASE     0x38120000 /*SDIO MASTER*/
#define MSDC_REG  ((msdc_register_t *)SDIO_MASTER_BASE)

#define MSDC_CMD_INTS       (MSDC_INT_CMDRDY | MSDC_INT_RSPCRCERR | MSDC_INT_CMDTMO)
#define MSDC_ACMD_INTS      (MSDC_INT_ACMDRDY | MSDC_INT_ACMDCRCERR  | MSDC_INT_ACMDTMO)
#define MSDC_DAT_INTS       (MSDC_INT_XFER_COMPL | MSDC_INT_DATCRCERR | MSDC_INT_DATTMO)

#define MSDC_CMD_INTS_EN    (MSDC_INTEN_CMDRDY | MSDC_INTEN_RSPCRCERR | MSDC_INTEN_CMDTMO)
#define MSDC_ACMD_INTS_EN   (MSDC_INTEN_ACMDRDY | MSDC_INTEN_ACMDCRCERR | MSDC_INTEN_ACMDTMO)
#define MSDC_DAT_INTS_EN    (MSDC_INTEN_XFER_COMPL | MSDC_INTEN_DATCRCERR | MSDC_INTEN_DATTMO)

#define MSDC_TXFIFOCNT()    ((MSDC_REG->MSDC_FIFOCS & MSDC_FIFOCS_TXCNT) >> 16)
#define MSDC_RXFIFOCNT()    ((MSDC_REG->MSDC_FIFOCS & MSDC_FIFOCS_RXCNT) >> 0)
#define MSDC_IS_SDC_BUSY()  (MSDC_REG->SDC_STS & SDC_STS_SDCBUSY)
#define MSDC_IS_CMD_BUSY()  (MSDC_REG->SDC_STS & SDC_STS_CMDBUSY)


typedef enum {
    SDIO_STATE_DIS,     /*disabled, initalize,standby and inactive states -- card not selected*/
    SDIO_STATE_CMD,     /*data lines free, command waiting*/
    SDIO_STATE_TRN,     /*transfer*/
    SDIO_STATE_RFU      /*Reserved for Future Use*/
} sdio_state_t;

typedef struct {
    uint16_t vendor;
    uint16_t device;
} sdio_cis_t;

typedef enum {
    MSDC_BUS_WIDTH_1BITS      = 0,                                        /**< The SDIO bus width is 1 bit. */
    MSDC_SDIO_BUS_WIDTH_4BITS = 1,                                         /**< The SDIO bus width is 4 bits. */
    MSDC_SDIO_BUS_WIDTH_8BITS = 2,                                         /**< The SDIO bus width is 4 bits. */
} msdc_bus_width_t;

typedef enum {
    NO_DATA = 0,
    SINGLE_BLOCK = 1,
    MULTI_BLOCK = 2,
    STREAM_OPERATION = 3
} msdc_data_block_t;

typedef enum {
    MSDC_DMA_BURST_8_BYTES = 3,
    MSDC_DMA_BURST_16_BYTES = 4,
    MSDC_DMA_BURST_32_BYTES = 5,
    MSDC_DMA_BURST_64_BYTES = 6
} msdc_burst_type_t;

typedef enum {
    MSDC_OWNER_NONE = 0,
    MSDC_OWNER_SD   = 1,
    MSDC_OWNER_SDIO
} msdc_owner_t;


typedef struct {
    volatile msdc_owner_t    owner;
    uint32_t                 msdc_clock;
    uint32_t                 output_clock;
    uint32_t                 interrupt_register;
    volatile bool            is_card_present;
    volatile bool            is_initialized;
    volatile bool            is_timeout;
    volatile bool            is_card_plug_out;
    uint32_t                 command_status;
    uint32_t                 data_status;
    volatile uint32_t        dma_status;
    volatile uint32_t        dma_count;
    volatile uint32_t        dma_address;
    volatile bool            is_dma_write;
#if defined(HAL_SD_MODULE_ENABLED) && defined(HAL_SD_CARD_DETECTION)
    void (*msdc_card_detect_callback)(hal_sd_card_event_t event, void *user_data);
    void                     *card_detect_user_data;
#endif /* #if defined(HAL_SD_MODULE_ENABLED) && defined(HAL_SD_CARD_DETECTION) */
#if defined(HAL_SD_MODULE_ENABLED)
    void (*msdc_sd_callback)(hal_sd_callback_event_t event, void *user_data);
#endif /* #if defined(HAL_SD_MODULE_ENABLED) */
#if defined(HAL_SDIO_MODULE_ENABLED)
    void (*msdc_sdio_dma_interrupt_callback)(hal_sdio_callback_event_t event, void *user_data);
    void (*msdc_sdio_interrupt_callback)(hal_sdio_callback_event_t event, void *user_data);
#endif /* #if defined(HAL_SDIO_MODULE_ENABLED) */
} msdc_config_t;

/*--------------------------------------------------------------------------*/
/* Descriptor Structure                                                     */
/*--------------------------------------------------------------------------*/
typedef struct {
    uint32_t  hwo: 1; /* could be changed by hw */
    uint32_t  bdp: 1;
    uint32_t  rsv0: 6;
    uint32_t  chksum: 8;
    uint32_t  intr: 1;
    uint32_t  rsv1: 15;
    void      *next;
    void      *ptr;
    uint32_t  buflen: 16;
    uint32_t  extlen: 8;
    uint32_t  rsv2: 8;
    uint32_t  arg;
    uint32_t  blknum;
    uint32_t  cmd;
} gpd_t;

typedef struct {
    uint32_t  eol: 1;
    uint32_t  rsv0: 7;
    uint32_t  chksum: 8;
    uint32_t  rsv1: 1;
    uint32_t  blkpad: 1;
    uint32_t  dwpad: 1;
    uint32_t  rsv2: 13;
    void      *next;
    void      *ptr;
    uint32_t  buflen: 16;
    uint32_t  rsv3: 16;
} bd_t;



typedef enum {
    MSDC_FAIL = -1,
    MSDC_OK = 0,
    MSDC_INITIALIZED = 1
} msdc_status_t;


typedef struct {
    __IO uint32_t MSDC_CFG;      /* base+0x00h, MSDC configuration register. */
    __IO uint32_t MSDC_IOCON;    /* base+0x04h, MSDC IO configuration register. */
    __IO uint32_t MSDC_PS;       /* base+0x08h, MSDC Pin Status register. */
    __IO uint32_t MSDC_INT;      /* base+0x0ch, MSDC Interrupt Register. */
    __IO uint32_t MSDC_INTEN;    /* base+0x10h, MSDC Interrupt Enable Register. */
    __IO uint32_t MSDC_FIFOCS;   /* base+0x14h, MSDC FIFO Control and Status Register. */
    __IO uint32_t MSDC_TXDATA;   /* base+0x18h, MSDC TX Data Port Register. */
    __IO uint32_t MSDC_RXDATA;   /* base+0x1ch, MSDC RX Data Port Register. */
    __IO uint32_t rsv1[4];
    __IO uint32_t SDC_CFG;       /* base+0x30h, SD Configuration Register. */
    __IO uint32_t SDC_CMD;       /* base+0x34h, SD Command Register. */
    __IO uint32_t SDC_ARG;       /* base+0x38h, SD Argument Register. */
    __I  uint32_t SDC_STS;       /* base+0x3ch, SD Status Register. */
    __I  uint32_t SDC_RESP0;     /* base+0x40h, SD Response Register 0. */
    __I  uint32_t SDC_RESP1;     /* base+0x44h, SD Response Register 1. */
    __I  uint32_t SDC_RESP2;     /* base+0x48h, SD Response Register 2. */
    __I  uint32_t SDC_RESP3;     /* base+0x4ch, SD Response Register 3. */
    __IO uint32_t SDC_BLK_NUM;   /* base+0x50h, SD Block Number Register. */
    __IO uint32_t rsv2[1];
    __IO uint32_t SDC_CSTS;      /* base+0x58h, SD Card Status Register. */
    __IO uint32_t SDC_CSTS_EN;   /* base+0x5ch, SD Card Status Enable Register. */
    __IO uint32_t SDC_DATCRC_STS;/* base+0x60h, SD Card Data CRC Status Register. */
    __IO uint32_t rsv3[7];
    __IO uint32_t SDC_ACMD_RESP; /* base+0x80h, SD ACMD Response Register. */
    __IO uint32_t rsv4[3];
    __IO uint32_t DMA_SA;        /* base+0x90h, DMA Start Address Register.*/
    __IO uint32_t DMA_CA;        /* base+0x94h, DMA Current Address Register. */
    __IO uint32_t DMA_CTRL;      /* base+0x98h, DMA Control Register. */
    __IO uint32_t DMA_CFG;       /* base+0x9ch, DMA Configuration Register. */
    __IO uint32_t SW_DBG_DEL;    /* base+0xa0h, MSDC S/W Debug Selection Register. */
    __IO uint32_t SW_DBG_OUT;    /* base+0xa4h, MSDC S/W Debug Output Register. */
    __IO uint32_t DMA_LENGTH;    /* base+0xa8h, DMA Length Register. */
    __IO uint32_t rsv5[1];
    __IO uint32_t PATCH_BIT0;    /* base+0xb0h, MSDC Patch Bit Register 0. */
    __IO uint32_t PATCH_BIT1;    /* base+0xb4h, MSDC Patch Bit Register 1. */
    __IO uint32_t rsv6[13];
    __IO uint32_t PAD_TUNE;      /* base+0xech, MSDC Pad Tuning Register. */
    __IO uint32_t DAT_RD_DLY0;   /* base+0xf0h, MSDC Data Delay Line Register 0. */
    __IO uint32_t DAT_RD_DLY1;   /* base+0xf4h, MSDC Data Delay Line Register 1. */
    __IO uint32_t HW_DBG_SEL;    /* base+0xf8h, MSDC H/W Debug Selection Register. */
    __IO uint32_t rsv8[1];
    __IO uint32_t MAIN_VER;      /* base+0x100h, MSDC Main Version Register. */
    __IO uint32_t ECO_VER;       /* base+0x104h, MSDC ECO Version Register. */
} msdc_register_t;


#define MSDC_CFG_MODE           (0x1  << 0)     /* RW */
#define MSDC_CFG_CKPDN          (0x1  << 1)     /* RW */
#define MSDC_CFG_RST            (0x1  << 2)     /* RW */
#define MSDC_CFG_PIO            (0x1  << 3)     /* RW */
#define MSDC_CFG_CKDRVEN        (0x1  << 4)     /* RW */
#define MSDC_CFG_BV18SDT        (0x1  << 5)     /* RW */
#define MSDC_CFG_BV18PSS        (0x1  << 6)     /* R  */
#define MSDC_CFG_CKSTB          (0x1  << 7)     /* R  */
#define MSDC_CFG_CKDIV          (0xfff << 8)    /* RW */
#define MSDC_CFG_CCKMD          (1 << 20)

/* [0x04]MSDC_IOCON mask */
#define MSDC_IOCON_SDR104CKS    (0x1  << 0)     /* RW */
#define MSDC_IOCON_RSPL         (0x1  << 1)     /* RW */
#define MSDC_IOCON_DSPL         (0x1  << 2)     /* RW */
#define MSDC_IOCON_DDLSEL       (0x1  << 3)     /* RW */
#define MSDC_IOCON_DDR50CKD     (0x1  << 4)     /* RW */
#define MSDC_IOCON_DSPLSEL      (0x1  << 5)     /* RW */
#define MSDC_IOCON_D0SPL        (0x1  << 16)    /* RW */
#define MSDC_IOCON_D1SPL        (0x1  << 17)    /* RW */
#define MSDC_IOCON_D2SPL        (0x1  << 18)    /* RW */
#define MSDC_IOCON_D3SPL        (0x1  << 19)    /* RW */
#define MSDC_IOCON_D4SPL        (0x1  << 20)    /* RW */
#define MSDC_IOCON_D5SPL        (0x1  << 21)    /* RW */
#define MSDC_IOCON_D6SPL        (0x1  << 22)    /* RW */
#define MSDC_IOCON_D7SPL        (0x1  << 23)    /* RW */
#define MSDC_IOCON_RISCSZ       (0x3  << 24)    /* RW */

/* [0x08]MSDC_PS mask */
#define MSDC_PS_CDEN            (0x1  << 0)     /* RW */
#define MSDC_PS_CDSTS           (0x1  << 1)     /* R  */
#define MSDC_PS_CDDEBOUNCE      (0xf  << 12)    /* RW */
#define MSDC_PS_DAT             (0xff << 16)    /* R  */
#define MSDC_PS_CMD             (0x1  << 24)    /* R  */
#define MSDC_PS_WP              (0x1UL<< 31)    /* R  */

/* [0x0c]MSDC_INT mask */
#define MSDC_INT_MMCIRQ         (0x1  << 0)     /* W1C */
#define MSDC_INT_CDSC           (0x1  << 1)     /* W1C */
#define MSDC_INT_ACMDRDY        (0x1  << 3)     /* W1C */
#define MSDC_INT_ACMDTMO        (0x1  << 4)     /* W1C */
#define MSDC_INT_ACMDCRCERR     (0x1  << 5)     /* W1C */
#define MSDC_INT_DMAQ_EMPTY     (0x1  << 6)     /* W1C */
#define MSDC_INT_SDIOIRQ        (0x1  << 7)     /* W1C */
#define MSDC_INT_CMDRDY         (0x1  << 8)     /* W1C */
#define MSDC_INT_CMDTMO         (0x1  << 9)     /* W1C */
#define MSDC_INT_RSPCRCERR      (0x1  << 10)    /* W1C */
#define MSDC_INT_CSTA           (0x1  << 11)    /* R */
#define MSDC_INT_XFER_COMPL     (0x1  << 12)    /* W1C */
#define MSDC_INT_DXFER_DONE     (0x1  << 13)    /* W1C */
#define MSDC_INT_DATTMO         (0x1  << 14)    /* W1C */
#define MSDC_INT_DATCRCERR      (0x1  << 15)    /* W1C */
#define MSDC_INT_ACMD19_DONE    (0x1  << 16)    /* W1C */

/* [0x10]MSDC_INTEN mask */
#define MSDC_INTEN_MMCIRQ       (0x1  << 0)     /* RW */
#define MSDC_INTEN_CDSC         (0x1  << 1)     /* RW */
#define MSDC_INTEN_ACMDRDY      (0x1  << 3)     /* RW */
#define MSDC_INTEN_ACMDTMO      (0x1  << 4)     /* RW */
#define MSDC_INTEN_ACMDCRCERR   (0x1  << 5)     /* RW */
#define MSDC_INTEN_DMAQ_EMPTY   (0x1  << 6)     /* RW */
#define MSDC_INTEN_SDIOIRQ      (0x1  << 7)     /* RW */
#define MSDC_INTEN_CMDRDY       (0x1  << 8)     /* RW */
#define MSDC_INTEN_CMDTMO       (0x1  << 9)     /* RW */
#define MSDC_INTEN_RSPCRCERR    (0x1  << 10)    /* RW */
#define MSDC_INTEN_CSTA         (0x1  << 11)    /* RW */
#define MSDC_INTEN_XFER_COMPL   (0x1  << 12)    /* RW */
#define MSDC_INTEN_DXFER_DONE   (0x1  << 13)    /* RW */
#define MSDC_INTEN_DATTMO       (0x1  << 14)    /* RW */
#define MSDC_INTEN_DATCRCERR    (0x1  << 15)    /* RW */
#define MSDC_INTEN_ACMD19_DONE  (0x1  << 16)    /* RW */

/* [0x14]MSDC_FIFOCS mask */
#define MSDC_FIFOCS_RXCNT       (0xff << 0)     /* R */
#define MSDC_FIFOCS_TXCNT       (0xff << 16)    /* R */
#define MSDC_FIFOCS_CLR         (0x1UL<< 31)    /* RW */

/* [0x18]MSDC_TXDATA mask */
/* [0x1C]MSDC_RXDATA mask */

/* [0x30]SDC_CFG mask */
#define SDC_CFG_BUSWIDTH_OFFSET (16)           /* RW */
#define SDC_CFG_DTOC_OFFSET     (24)           /* RW */


#define SDC_CFG_SDIOINTWKUP     (0x1  << 0)     /* RW */
#define SDC_CFG_INSWKUP         (0x1  << 1)     /* RW */
#define SDC_CFG_BUSWIDTH        (0x3  << 16)    /* RW */
#define SDC_CFG_SDIO            (0x1  << 19)    /* RW */
#define SDC_CFG_SDIOIDE         (0x1  << 20)    /* RW */
#define SDC_CFG_INTATGAP        (0x1  << 21)    /* RW */
#define SDC_CFG_DTOC            (0xffUL << 24)  /* RW */

/* [0x34]SDC_CMD mask */
#define SDC_CMD_LEN_OFFSET      (16)

#define SDC_CMD_OPC             (0x3f << 0)     /* RW */
#define SDC_CMD_BRK             (0x1  << 6)     /* RW */
#define SDC_CMD_RSPTYP          (0x7  << 7)     /* RW */
#define SDC_CMD_DTYP            (0x3  << 11)    /* RW */
#define SDC_CMD_RW              (0x1  << 13)    /* RW */
#define SDC_CMD_STOP            (0x1  << 14)    /* RW */
#define SDC_CMD_GOIRQ           (0x1  << 15)    /* RW */
#define SDC_CMD_BLKLEN          (0xfff<< 16)    /* RW */
#define SDC_CMD_AUTOCMD         (0x3  << 28)    /* RW */
#define SDC_CMD_VOLSWTH         (0x1  << 30)    /* RW */

/*[0x38] SDC_ARG mask */

/*[0x3C] SDC_STS mask */
#define SDC_STS_SDCBUSY         (0x1  << 0)     /* RW */
#define SDC_STS_CMDBUSY         (0x1  << 1)     /* RW */
#define SDC_STS_SWR_COMPL       (0x1  << 31)    /* RW */

/* [0x60]SDC_DCRC_STS mask */
#define SDC_DCRC_STS_NEG        (0xf  << 8)     /* RO */
#define SDC_DCRC_STS_POS        (0xff << 0)     /* RO */


/* [0x84]SDC_ACMD19_TRG mask */
//#define SDC_ACMD19_TRG_TUNESEL  (0xf  << 0)     /* RW */

/* [0x98]MSDC_DMA_CTRL mask */
#define MSDC_DMA_CTRL_START     (0x1  << 0)     /* W */
#define MSDC_DMA_CTRL_STOP      (0x1  << 1)     /* W */
#define MSDC_DMA_CTRL_RESUME    (0x1  << 2)     /* W */
#define MSDC_DMA_CTRL_MODE      (0x1  << 8)     /* RW */
#define MSDC_DMA_CTRL_LASTBUF   (0x1  << 10)    /* RW */
#define MSDC_DMA_CTRL_BRUSTSZ   (0x7  << 12)    /* RW */
#define MSDC_DMA_CTRL_XFERSZ    (0xffffUL << 16)/* RW */

/* [0x9c]MSDC_DMA_CFG mask */
#define MSDC_DMA_CFG_STS        (0x1  << 0)     /* R */
#define MSDC_DMA_CFG_DECSEN     (0x1  << 1)     /* RW */
#define MSDC_DMA_CFG_BDCSERR    (0x1  << 4)     /* R */
#define MSDC_DMA_CFG_GPDCSERR   (0x1  << 5)     /* R */


/* [0xb0]MSDC_PATCH_BIT mask */
#define MSDC_PATCH_BIT_WFLSMODE (0x1  << 0)     /* RW */
#define MSDC_PATCH_BIT_ODDSUPP  (0x1  << 1)     /* RW */
#define MSDC_PATCH_BIT_CKGEN_CK (0x1  << 6)     /* E2: Fixed to 1 */
#define MSDC_PATCH_BIT_IODSSEL  (0x1  << 16)    /* RW */
#define MSDC_PATCH_BIT_IOINTSEL (0x1  << 17)    /* RW */
#define MSDC_PATCH_BIT_BUSYDLY  (0xf  << 18)    /* RW */
#define MSDC_PATCH_BIT_WDOD     (0xf  << 22)    /* RW */
#define MSDC_PATCH_BIT_IDRTSEL  (0x1  << 26)    /* RW */
#define MSDC_PATCH_BIT_CMDFSEL  (0x1  << 27)    /* RW */
#define MSDC_PATCH_BIT_INTDLSEL (0x1  << 28)    /* RW */
#define MSDC_PATCH_BIT_SPCPUSH  (0x1  << 29)    /* RW */
#define MSDC_PATCH_BIT_DECRCTMO (0x1  << 30)    /* RW */

/* [0xb4]MSDC_PATCH_BIT1 mask */
#define MSDC_PATCH_BIT1_WRDAT_CRCS  (0x7 << 0)
#define MSDC_PATCH_BIT1_CMD_RSP     (0x7 << 3)

/* [0xec]MSDC_PAD_TUNE mask */
#define MSDC_PAD_TUNE_DATWRDLY  (0x1F << 0)     /* RW */
#define MSDC_PAD_TUNE_DATRRDLY  (0x1F << 8)     /* RW */
#define MSDC_PAD_TUNE_CMDRDLY   (0x1F << 16)    /* RW */
#define MSDC_PAD_TUNE_CMDRRDLY  (0x1FUL << 22)  /* RW */
#define MSDC_PAD_TUNE_CLKTXDLY  (0x1FUL << 27)  /* RW */

/* [0xf0/f4]MSDC_DAT_RDDLY0/1 mask */
#define MSDC_DAT_RDDLY0_D0      (0x1F << 0)     /* RW */
#define MSDC_DAT_RDDLY0_D1      (0x1F << 8)     /* RW */
#define MSDC_DAT_RDDLY0_D2      (0x1F << 16)    /* RW */
#define MSDC_DAT_RDDLY0_D3      (0x1F << 24)    /* RW */

#define MSDC_DAT_RDDLY1_D4      (0x1F << 0)     /* RW */
#define MSDC_DAT_RDDLY1_D5      (0x1F << 8)     /* RW */
#define MSDC_DAT_RDDLY1_D6      (0x1F << 16)    /* RW */
#define MSDC_DAT_RDDLY1_D7      (0x1F << 24)    /* RW */

/* MSDC_PATCH_BIT0 mask */
#define MSDC_CKGEN_MSDC_DLY_SEL   (0x1F<<10)
#define MSDC_INT_DAT_LATCH_CK_SEL (0x7<<7)
#define MSDC_CKGEN_MSDC_CK_SEL    (0x1<<6)
#define MSDC_CKGEN_RX_SDCLKO_SEL  (0x1<<0)

/* MSDC_PATCH_BIT1 mask */
#define MSDC_SINGLEBURST_SEL   (0x1<<16)

#define MSDC_FIFO_SIZE (128)

//#define MSDC_USE_INT

#define MSDC_MAX_BLOCK_LENGTH (2048)
#define MSDC_DATA_TIMEOUT_COUNT               (80)
#define MSDC_CLOCK           50000 /*(kHz) 89140*/
#define MSDC_INIT_CLOCK      400 /*KHz*/
#define MSDC_OUTPUT_CLOCK    50000

void msdc_wait(uint32_t wait_ms);
void msdc_power_set(bool is_power_on);
void msdc_reset(void);
void msdc_clear_fifo(void);
void msdc_set_bus_width(msdc_bus_width_t bus_width);
void msdc_set_output_clock(uint32_t clock);
uint32_t msdc_get_output_clock(void);
void msdc_sdio_interrupt_set(bool enable);
void msdc_data_interrupt_handle(uint32_t status);
void msdc_command_interrupt_handle(uint32_t status);
void msdc_nvic_set(bool enable);
void msdc_interrupt_init(void);
void msdc_eint_isr(void *user_data);
void msdc_eint_registration(void);
msdc_status_t msdc_init(msdc_bus_width_t bus_width);
void msdc_deinit(void);
bool msdc_card_is_present(void);
void msdc_dma_enable(void);
void msdc_dma_disable(void);
msdc_owner_t msdc_get_owner(void);
void msdc_set_owner(msdc_owner_t owner);
void msdc_card_power_set(bool is_power_on);
void msdc_clock_init(void);
void msdc_set_burst_type(msdc_burst_type_t burst_type);

#endif /* #ifndef __HAL_MSDC_H__ */

