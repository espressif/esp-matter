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

#ifndef __HAL_SD_DEFINE_H__
#define __HAL_SD_DEFINE_H__

#include "hal_platform.h"

#if defined(HAL_SD_MODULE_ENABLED) || defined(HAL_SDIO_MODULE_ENABLED)


#define SD_BLOCK_LENGTH   (512)


/* MMC bus commands */
#define COMMAND0_GO_IDLE_STATE          0x00    /* Resets the MMC */
#define COMMAND1_SEND_OP_COND           0x01    /* Activates the card's initialization process */
#define COMMAND2_ALL_SEND_CID           0x02    /* Asks all cards in ready state to send their CID */
#define COMMAND3_SET_RELATIVE_ADDR      0x03    /* Assigns relative address to the card in identification state */
#define COMMAND4_SET_DSR                0x04    /* Sets drive stage register DSR */
#define COMMAND6_SWITCH                 0x06
#define COMMAND7_SELECT_CARD            0x07
#define COMMAND8_READ_CONDITION         0x08    /*read SD memory card interface condition*/
#define COMMAND8_SEND_EXT_CSD_MMC       0x08    /*read EXT CSD register as a block data*/
#define COMMAND9_SEND_CSD               0x09    /* Asks the selected card to send its CSD */
#define COMMAND10_SEND_CID              0x0A    /* Asks the selected card to send its CID */
#define COMMAND11_READ_DAT_UNTIL_STOP_MMC   0x0B
#define COMMAND12_STOP_TRANSMISSION     0x0C    /* Stop data transmission */
#define COMMAND13_SEND_STATUS           0x0D    /* Asks the selected card to send its status register */
#define COMMAND15_GO_INACTIVE_STATE     0x0F
#define COMMAND16_SET_BLOCKLEN          0x10    /* Selects a block length for all following block commands */
#define COMMAND17_READ_SINGLE_BLOCK     0x11    /* Reads a block of the size selected by the SET_BLOCKLEN command */
#define COMMAND18_READ_MULTIPLE_BLOCK   0x12
#define COMMAND20_WRITE_DAT_UNTIL_STOP_MMC  0x14
#define COMMAND24_WRITE_SINGLE_BLOCK    0x18    /* Writes a block of the size selected by SET_BLOCKLEN command */
#define COMMAND25_WRITE_MULTIPLE_BLOCK  0x19     /* Writes multiple block until COMMAND 12 */
#define COMMAND26_PROGRAM_CID           0x1A
#define COMMAND27_PROGRAM_CSD           0x1B    /* Programming of the programmable bits of the CSD */
#define COMMAND28_SET_WRITE_PROT        0x1C    /* If the card has write protection features, sets the write protection bit */
#define COMMAND29_CLR_WRITE_PROT        0x1D    /* If with write protection features, clears the write protection bit */
#define COMMAND30_SEND_WRITE_PROT       0x1E    /* If with write protection features, asks the card to send status of write protection bit */
#define COMMAND32_TAG_SECTOR_START      0x20    /* Sets the address of the first sector */
#define COMMAND33_TAG_SECTOR_END        0x21    /* Sets the address of the last sectore */
#define COMMAND34_UNTAG_SECTOR          0x22    /* Removes one previously selected sector */
#define COMMAND35_TAG_ERASE_GROUP_START 0x23    /* Sets the address of the first erase group */
#define COMMAND36_TAG_ERASE_GROUP_END   0x24    /* Sets the address of the last erase group */
#define COMMAND37_UNTAG_ERASE_GROUP     0x25    /* Removes on previously selected erase group */
#define COMMAND38_ERASE                 0x26    /* Erases all previously selected sectors */
#define COMMAND39_FAST_IO_MMC           0x27
#define COMMAND40_GO_IRQ_STATE          0x28
#define COMMAND42_LOCK_UNLOCK           0x2A    /* Used to set/reset the password or lock/unlock the card */
#define COMMAND55_APP                   0x37    /* Indicates to the card the next cmd is an application specific command */
#define COMMAND56_GEN                   0x38    /* Used either to transfer a data block or to get a data block form the card for general purpse */
/*application-specific commands*/
#define ACOMMAND6_SET_BUS_WIDTH                        0x06
#define ACOMMAND13_READ_SD_STATUS                      0x0d
#define ACOMMAND22_SEND_NUMBER_WRITE_BLOCK             0x16
#define ACOMMAND23_SET_WRITE_BLOCK_ERASE_COUNT         0x17
#define ACOMMAND41_READ_OCR                            0x29
#define ACOMMAND42_SET_CARD_DETECT                     0x2A
#define ACOMMAND51_SEND_SCR                            0x33


/*SDIO command*/
#define COMMAND5_READ_OCR                  (0x05)
#define COMMAND52_READ_WRITE_DIRECT        (0x34)
#define COMMAND53_READ_WRITE_EXTENDED      (0x35)


/*MSDC Command configuration define*/
#define COMMAND_NO_RESPONSE         0x0000
#define COMMAND_R1_RESPONSE         0x0080
#define COMMAND_R2_RESPONSE         0x0100
#define COMMAND_R3_RESPONSE         0x0180
#define COMMAND_R4_RESPONSE         0x0200
#define COMMAND_R5_RESPONSE         0x0280
#define COMMAND_R6_RESPONSE         0x0300
#define COMMAND_R1B_RESPONSE        0x0380

#define COMMAND_NID_RESPONSE        0x0400

#define COMMAND_NO_DATA_TRANSACTION        0x0000
#define COMMAND_SINGLE_BLOCK_TRANSACTION   0x0800
#define COMMAND_MULTIPLE_BLOCK_TRANSACTION 0x1000
#define COMMAND_MMC_STREAM_TRANSACTION     0x1800


#define COMMAND_READ                0x0000
#define COMMAND_WRITE               0x2000

#define COMMAND_STOP_TRANSMISSION                   0x4000
#define COMMAND_NOT_STOP_TRANSMISSION               0x0000

#define COMMAND_GO_IRQ_STATE                   0x8000
#define COMMAND_NOT_GO_IRQ_STATE               0x0000


/*Command defined for MSDC use directly */
#define MSDC_COMMAND0           (COMMAND0_GO_IDLE_STATE | COMMAND_NO_RESPONSE) /* value = 0x0000 */
#define MSDC_COMMAND1_MMC       (COMMAND1_SEND_OP_COND  | COMMAND_R3_RESPONSE)
#define MSDC_COMMAND2           (COMMAND2_ALL_SEND_CID  | COMMAND_R2_RESPONSE | COMMAND_NID_RESPONSE)
#define MSDC_COMMAND3           (COMMAND3_SET_RELATIVE_ADDR  | COMMAND_R6_RESPONSE)
#define MSDC_COMMAND3_MMC       (COMMAND3_SET_RELATIVE_ADDR  | COMMAND_R1_RESPONSE)
#define MSDC_COMMAND4           (COMMAND4_SET_DSR  | COMMAND_NO_RESPONSE)
#define MSDC_COMMAND7           (COMMAND7_SELECT_CARD  | COMMAND_R1B_RESPONSE)
#define MSDC_COMMAND8           (COMMAND8_READ_CONDITION | COMMAND_R1_RESPONSE)
#define MSDC_COMMAND9           (COMMAND9_SEND_CSD | COMMAND_R2_RESPONSE)
#define MSDC_COMMAND10          (COMMAND10_SEND_CID | COMMAND_R2_RESPONSE)    /*the response time delay is not NID(5 cycles)*/
#define MSDC_COMMAND11_MMC      (COMMAND11_READ_DAT_UNTIL_STOP_MMC | COMMAND_R1_RESPONSE | COMMAND_MMC_STREAM_TRANSACTION) /* value = 0x108b */
#define MSDC_COMMAND12          (COMMAND12_STOP_TRANSMISSION | COMMAND_R1B_RESPONSE | COMMAND_STOP_TRANSMISSION)    /* value = 0x438c */
#define MSDC_COMMAND13          (COMMAND13_SEND_STATUS | COMMAND_R1_RESPONSE) /* value = 0x008d */
#define MSDC_COMMAND15          (COMMAND15_GO_INACTIVE_STATE | COMMAND_NO_RESPONSE) /* value = 0x000f */
#define MSDC_COMMAND16          (COMMAND16_SET_BLOCKLEN | COMMAND_R1_RESPONSE) /* value = 0x0090 */
#define MSDC_COMMAND17          (COMMAND17_READ_SINGLE_BLOCK | COMMAND_R1_RESPONSE | COMMAND_SINGLE_BLOCK_TRANSACTION) /* value = 0x0891 */
#define MSDC_COMMAND18          (COMMAND18_READ_MULTIPLE_BLOCK | COMMAND_R1_RESPONSE | COMMAND_MULTIPLE_BLOCK_TRANSACTION) /* value = 0x1092 */
#define MSDC_COMMAND20_MMC      (COMMAND20_WRITE_DAT_UNTIL_STOP_MMC | COMMAND_R1_RESPONSE | COMMAND_MMC_STREAM_TRANSACTION | COMMAND_WRITE) /* value = 0x3894 */
#define MSDC_COMMAND24          (COMMAND24_WRITE_SINGLE_BLOCK | COMMAND_R1_RESPONSE | COMMAND_SINGLE_BLOCK_TRANSACTION | COMMAND_WRITE)/* value = 0x2898 */
#define MSDC_COMMAND25          (COMMAND25_WRITE_MULTIPLE_BLOCK | COMMAND_R1_RESPONSE | COMMAND_MULTIPLE_BLOCK_TRANSACTION | COMMAND_WRITE)/* value = 0x3099 */
#define MSDC_COMMAND26          (COMMAND26_PROGRAM_CID | COMMAND_R1_RESPONSE) /* value = 0x009a */
#define MSDC_COMMAND27          (COMMAND27_PROGRAM_CSD | COMMAND_R1_RESPONSE) /* value = 0x009b */
#define MSDC_COMMAND28          (COMMAND28_SET_WRITE_PROT | COMMAND_R1B_RESPONSE) /* value = 0x039c */
#define MSDC_COMMAND29          (COMMAND29_CLR_WRITE_PROT | COMMAND_R1B_RESPONSE) /* value = 0x039d */
#define MSDC_COMMAND30          (COMMAND30_SEND_WRITE_PROT | COMMAND_R1_RESPONSE | COMMAND_SINGLE_BLOCK_TRANSACTION) /* value = 0x089e */
#define MSDC_COMMAND32          (COMMAND32_TAG_SECTOR_START | COMMAND_R1_RESPONSE) /* value = 0x00a0 */
#define MSDC_COMMAND33          (COMMAND33_TAG_SECTOR_END | COMMAND_R1_RESPONSE) /* value = 0x00a1 */
#define MSDC_COMMAND34_MMC      (COMMAND34_UNTAG_SECTOR | COMMAND_R1_RESPONSE) /* value = 0x00a2 */
#define MSDC_COMMAND35_MMC      (COMMAND35_TAG_ERASE_GROUP_START | COMMAND_R1_RESPONSE) /* value = 0x00a3 */
#define MSDC_COMMAND36_MMC      (COMMAND36_TAG_ERASE_GROUP_END | COMMAND_R1_RESPONSE) /* value = 0x00a4 */
#define MSDC_COMMAND37_MMC      (COMMAND37_UNTAG_ERASE_GROUP | COMMAND_R1_RESPONSE) /* value = 0x00a5 */
#define MSDC_COMMAND38          (COMMAND38_ERASE | COMMAND_R1B_RESPONSE) /* value = 0x03a6 */
#define MSDC_COMMAND39_MMC      (COMMAND39_FAST_IO_MMC | COMMAND_R4_RESPONSE) /* value = 0x0227 */
#define MSDC_COMMAND40_MMC      (COMMAND40_GO_IRQ_STATE | COMMAND_R5_RESPONSE | COMMAND_GO_IRQ_STATE) /* value = 0x82a8 */
#define MSDC_COMMAND42          (COMMAND42_LOCK_UNLOCK | COMMAND_R1B_RESPONSE | COMMAND_SINGLE_BLOCK_TRANSACTION | COMMAND_WRITE)/* value = 0x2baa */
#define MSDC_COMMAND55          (COMMAND55_APP | COMMAND_R1_RESPONSE) /* value = 0x00b7 */
#define MSDC_COMMAND56          (COMMAND56_GEN | COMMAND_R1_RESPONSE) /* value = 0x00b8 */
/*application-specific commands*/
#define MSDC_ACOMMAND6          (ACOMMAND6_SET_BUS_WIDTH | COMMAND_R1_RESPONSE) /* value = 0x0086 */
#define MSDC_ACOMMAND13         (ACOMMAND13_READ_SD_STATUS | COMMAND_R1_RESPONSE | COMMAND_SINGLE_BLOCK_TRANSACTION) /* value = 0x088d */
#define MSDC_ACOMMAND22         (ACOMMAND22_SEND_NUMBER_WRITE_BLOCK | COMMAND_R1_RESPONSE | COMMAND_SINGLE_BLOCK_TRANSACTION) /* value = 0x0896 */
#define MSDC_ACOMMAND23         (ACOMMAND23_SET_WRITE_BLOCK_ERASE_COUNT | COMMAND_R1_RESPONSE) /* value = 0x0097 */
#define MSDC_ACOMMAND41         (ACOMMAND41_READ_OCR | COMMAND_R3_RESPONSE) /* value = 0x01a9 */
#define MSDC_ACOMMAND42         (ACOMMAND42_SET_CARD_DETECT | COMMAND_R1_RESPONSE) /* value = 0x00aa */
#define MSDC_ACOMMAND51         (ACOMMAND51_SEND_SCR | COMMAND_R1_RESPONSE | COMMAND_SINGLE_BLOCK_TRANSACTION) /* value = 0x08b3 */


#define MSDC_COMMAND6_MMC       (COMMAND6_SWITCH | COMMAND_R1B_RESPONSE)  /* value = 0x0386 */
#define MSDC_COMMAND6           (COMMAND6_SWITCH | COMMAND_R1_RESPONSE | COMMAND_SINGLE_BLOCK_TRANSACTION)  /* value = 0x0886 */
#define MSDC_COMMAND8_MMC       (COMMAND8_SEND_EXT_CSD_MMC | COMMAND_R1_RESPONSE | COMMAND_SINGLE_BLOCK_TRANSACTION)  /* value = 0x0888 */



/*SDIO cmomand, for MSDC use directly */
#define MSDC_COMMAND5_SDIO       (COMMAND5_READ_OCR | COMMAND_R3_RESPONSE)              /* value = 0x0185, use R3 to replace R4 */
#define MSDC_COMMAND52_SDIO      (COMMAND52_READ_WRITE_DIRECT | COMMAND_R5_RESPONSE)    /* value = 0x02B4 */
#define MSDC_COMMAND53_SDIO      (COMMAND53_READ_WRITE_EXTENDED | COMMAND_R1_RESPONSE)  /* value = 0x0B5 */


#define COMMAND6_RESPONSE_LENGTH      (64)          /*byte*/
#define SD_COMMAND6_QUERY_HIGH_SPEED  (0xFFFFF1)    /*argument of command6 to query the high speed interface*/
#define SD_COMMAND6_SELECT_HIGH_SPEED (0x80FFFFF1)  /*argument of command6 to select the high speed interface*/



#define COMMAND_NO_ARGUMENT           (0)
#define COMMAND8_ARGUMENT             (0x01AA)
#define SD_OCR_VOLTAGE_ARGUMENT       (0x00FF8000)
#define SD_OCR_CAPACITY_STATUS        (0x40000000)
#define SD_OCR_BUSY_STATUS            (0x80000000)

#define SD_MID_BYTE_SHIFT             (15)
#define COMMAND_RCA_ARGUMENT_SHIFT    (16)

#define COMMAND_DCR_ARGUMENT          (0x404)

#define SD_SCR_LENGTH                 (8)

#define COMMAND6_BUS_WIDTH_1          (0)
#define COMMAND6_BUS_WIDTH_4          (2)


#define SD_RCA_DEFAULT                          (0x00)

#define SD_CARD_STATUS_ERROR_MASK               (0xffff8000)
#define SD_CARD_STATUS_APP_COMMAND_MASK         (0x20)
#define SD_CARD_STATUS_LOCKED                   (0x02000000)
#define SD_CARD_STATUS_STATE_BIT_SHIFT          (9)
#define SD_CARD_STATUS_STATE_BIT_MASK           (0x0F << SD_CARD_STATUS_STATE_BIT_SHIFT)
#define SD_CARD_STATUS_READ_FOR_DATA_BIT_MASK   (0x100)

#define SD_DEFAULT_SPEED_MAX    25000 /*25MHZ*/
#define SD_HIGH_SPEED_MAX       50000 /*50MHZ*/

#define MSDC_TIMEOUT_PERIOD_INIT        1100000        /* us*/
#define MSDC_TIMEOUT_PERIOD_DATA        1500000        /* us*/
#define MSDC_TIMEOUT_PERIOD_COMMAND     500000         /* us*/

#define SD_FLAG_MMC_MRSW_FAIL  (0x01)        /* some special MMC card will fail at multi-read follow a single wirte */
#define SD_FLAG_HCS_SUPPORT    (0x02)        /* support block access mode */
#define SD_FLAG_CMD6_SUPPORT   (0x04)        /* support CMD6 (SD1.1 higher) */
#define SD_FLAG_HS_SUPPORT     (0x08)        /* support high speed interface (SD1.1 higher) */
#define SD_FLAG_HS_ENABLED     (0x10)        /* enable high speed interface (SD1.1 higher) */
#define SD_FLAG_SD_TYPE_CARD   (0x20)        /* to indicate SD or MMC type card*/
#define SD_FLAG_USE_USB_CLK    (0x40)        /* to indicate use USB clock */
#define SD_FLAG_LIMITED_HIGH_SPEED_CARD    (0x8000) /* to indicate special high speed card that we can only run with 26M */


/*defined for emmc*/
#define MMC_SWITCH_ERROR_BIT_MASK       (0x80)

#define MMC_HIGH_DESITY_CHECK_BIT       (0x40000000)
#define MMC_HIGH_DESITY_CHECK_MASK      (0x60000000)

#define COMMAND_MMC_RCA_ARGUMENT        (1)


typedef enum {
    MMC_HIGH_SPEED_26M = 0,
    MMC_HIGH_SPEED_52M = 1
} mmc_speed_type_t;

typedef enum {
    MMC_EXTENDED_CSD_VERSION_40  = 0,
    MMC_EXTENDED_CSD_VERSION_41  = 1,
    MMC_EXTENDED_CSD_VERSION_42  = 2,
    MMC_EXTENDED_CSD_VERSION_43  = 3,
    MMC_EXTENDED_CSD_VERSION_441 = 5
} mmc_exetend_csd_version_t;

typedef struct {
    bool is_mmc_version_44;
    bool is_support_partition;
    bool is_support_enhanced_partition;
    uint32_t boot_partition_size;        /*size of boot partition, the uint is 512-byte */
    uint32_t group1_partition_size;      /*size of GP1 partition, the uint is 512-byte */
} mmc_information_t;

typedef enum {
    EXT_CSD_ACCESS_MODE_COMMAND_SET = 0,
    EXT_CSD_ACCESS_MODE_SET_BIT     = 1,
    EXT_CSD_ACCESS_MODE_CLEAR_BIT   = 2,
    EXT_CSD_ACCESS_MODE_WRITE_BYTE  = 3
} ext_csd_access_mode_t;

#define  MMC_COMMAND6_ACCESS_BIT_SHIFT        (24)
#define  MMC_COMMAND6_INDEX_BIT_SHIFT         (16)
#define  MMC_COMMAND6_VALUE_BIT_SHIFT         (8)
#define  MMC_COMMAND6_COMMAND_SET_BIT_SHIFT   (0)

typedef enum {
    MMC_BUS_WIDTH_1   = 0,
    MMC_BUS_WIDTH_4   = 1,
    MMC_BUS_WIDTH_8   = 2
} mmc_bus_width_t;


#define EXT_CSD_BUS_WIDTH_INDEX     (183)
#define EXT_CSD_ERASE_GROUP_INDEX   (175)


typedef enum {
    ERROR_SDIO_NOT_SUPPORT_4MI                   = -39,
    ERROR_CARD_IS_LOCKED                         = -38,
    ERROR_HIGH_SPEED_CONSUMPTION                 = -37,
    ERROR_HIGH_SPEED_NOT_SUPPORT                 = -36,
    ERROR_HIGH_SPEED_COMMON_ERROR                = -35,
    ERROR_HIGH_SPEED_TIMEOUT                     = -34,
    ERROR_HIGH_SPEED_BUSY                        = -33,
    ERROR_MMC_SWITCH_ERROR                       = -32,
    ERROR_INVALID_BLOCK                          = -31,
    ERROR_ERRORS                                 = -30,
    ERROR_AUTO_CALIBRATION_STATUS_ENABLE_FAILED  = -29,
    ERROR_AUTO_CALIBRATION_TIMEOUT               = -28,
    ERROR_AUTO_CALIBRATION_FAILED                = -27,
    ERROR_NOT_FOUND                              = -26,
    ERROR_SD_HS_FAILED                           = -25,
    ERROR_COMMAND8_INVALID                       = -24,
    ERROR_SEND_EXT_CSD                           = -23,
    ERROR_MMC_BUS_HS_ERROR                       = -22,
    ERROR_COMMAND_OK_DATA_READ_FAILED            = -21,
    ERROR_DAT_ERROR                              = -20,
    ERROR_OCR_NOT_SUPPORT                        = -19,
    ERROR_CARD_NOT_PRESENT                       = -18,
    ERROR_DATA_NOT_READY                         = -17,
    ERROR_WRITE_PROTECT                          = -16,
    ERROR_NO_RESPONSE                            = -15,
    ERROR_NOT_SUPPORT_4BITS                      = -14,
    ERROR_APPCOMMAND_FAILED                      = -13,
    ERROR_LOCK_UNLOCK_FAILED                     = -12,
    ERROR_STATUS                                 = -11,
    ERROR_DATA_TIMEOUT                           = -10,
    ERROR_COMMAND_CRC_ERROR                      = -9,
    ERROR_RW_COMMAND_ERROR                       = -8,
    ERROR_DATA_CRC_ERROR                         = -7,
    ERROR_INVALID_BLOCK_LENGTH                   = -6,
    ERROR_INVALID_CARD                           = -5,
    ERROR_RCA_FAILED                             = -4,
    ERROR_R3_OCR_BUSY                            = -3,
    ERROR_APP_COMMAND_NOT_ACCEPT                 = -2,
    ERROR_COMMANDD_TIMEOUT                       = -1,
    NO_ERROR                                     =  0,
} mtk_sd_status_t, mtk_sdio_status_t;

#endif /* #if defined(HAL_SD_MODULE_ENABLED) || defined(HAL_SDIO_MODULE_ENABLED) */

#endif /* #ifndef __HAL_SD_DEFINE_H__ */

