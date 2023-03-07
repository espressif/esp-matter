/***************************************************************************//**
 * @file
 * @brief File System - Sd Card Media Driver
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.
 * The software is governed by the sections of the MSLA applicable to Micrium
 * Software.
 *
 ******************************************************************************/

/****************************************************************************************************//**
 * @note     (1) This driver complies with the following specifications:
 *             - (a) SD Card Association.  "Physical Layer Simplified Specification Version 2.00".
 *                   July 26, 2006.
 *             - (b) JEDEC Solid State Technology Association.  "MultiMediaCard (MMC) Electrical
 *                   Standard, High Capacity".  JESD84-B42.  July 2007.
 *
 * @note     (2) This driver has been tested with MOST SD/MMC media types, including :
 *             - (a) Standard capacity SD cards, v1.x & v2.0.
 *             - (b) SDmicro cards.
 *             - (c) High capacity SD cards (SDHC)
 *             - (d) MMC
 *             - (e) MMCplus
 *               It should also work with devices complying with the relevant SD or MMC specifications.
 *
 * @note     (3) High capacity MMC cards (>2GB) requires an additional file system buffer for its
 *               identification sequence.
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include <rtos_description.h>

#if (defined(RTOS_MODULE_FS_STORAGE_SD_CARD_AVAIL))

#if (!defined(RTOS_MODULE_FS_AVAIL))

#error SD module requires File System Storage module. Make sure it is part of your project and that \
  RTOS_MODULE_FS_AVAIL is defined in rtos_description.h.

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------------------ FS ------------------------
#include  <fs_storage_cfg.h>
#include  <fs/include/fs_sd_card.h>
#include  <fs/source/storage/sd/fs_sd_priv.h>

#include  <fs/source/shared/fs_utils_priv.h>

#include  <fs/source/storage/fs_blk_dev_priv.h>
#include  <fs/source/shared/cleanup/cleanup_mgmt_priv.h>

//                                                                 ----------------------- EXT ------------------------
#include  <common/include/lib_ascii.h>
#include  <common/include/lib_mem.h>
#include  <common/include/lib_str.h>
#include  <common/include/lib_utils.h>
#include  <common/source/logging/logging_priv.h>

#include  <em_core.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  RTOS_MODULE_CUR                                    RTOS_CFG_MODULE_FS
#define  LOG_DFLT_CH                                        (FS, DRV, SD)

#define  FS_DEV_SD_CARD_RETRIES_CMD                      256u
#define  FS_DEV_SD_CARD_RETRIES_CMD_BUSY               50000u
#define  FS_DEV_SD_CARD_RETRIES_DATA                       5u

#define  FS_DEV_SD_CARD_TIMEOUT_CMD_ms                  1000u

#define  FS_DEV_SD_CARD_RCA_DFLT                      0x0001u

#define  FS_DEV_SD_CARD_BUF_SIZE                    FS_DEV_SD_CID_EXT_CSD_LEN

//                                                                 ------------- CARD STATES (Table 4-35) -------------
#define  FS_DEV_SD_CARD_STATE_IDLE                         0u
#define  FS_DEV_SD_CARD_STATE_READY                        1u
#define  FS_DEV_SD_CARD_STATE_IDENT                        2u
#define  FS_DEV_SD_CARD_STATE_STBY                         3u
#define  FS_DEV_SD_CARD_STATE_TRAN                         4u
#define  FS_DEV_SD_CARD_STATE_DATA                         5u
#define  FS_DEV_SD_CARD_STATE_RCV                          6u
#define  FS_DEV_SD_CARD_STATE_PRG                          7u
#define  FS_DEV_SD_CARD_STATE_DIS                          8u
#define  FS_DEV_SD_CARD_STATE_IO                          15u

//                                                                 -------------- CMD FLAGS PER RESP TYPE -------------
#define  FS_DEV_SD_CARD_CMD_FLAG_UNKNOWN         (FS_DEV_SD_CARD_CMD_FLAG_NONE)
#define  FS_DEV_SD_CARD_CMD_FLAG_R1              (FS_DEV_SD_CARD_CMD_FLAG_IX_VALID | FS_DEV_SD_CARD_CMD_FLAG_CRC_VALID                                | FS_DEV_SD_CARD_CMD_FLAG_RESP)
#define  FS_DEV_SD_CARD_CMD_FLAG_R1B             (FS_DEV_SD_CARD_CMD_FLAG_IX_VALID | FS_DEV_SD_CARD_CMD_FLAG_CRC_VALID | FS_DEV_SD_CARD_CMD_FLAG_BUSY | FS_DEV_SD_CARD_CMD_FLAG_RESP)
#define  FS_DEV_SD_CARD_CMD_FLAG_R2              (FS_DEV_SD_CARD_CMD_FLAG_CRC_VALID                                | FS_DEV_SD_CARD_CMD_FLAG_RESP | FS_DEV_SD_CARD_CMD_FLAG_RESP_LONG)
#define  FS_DEV_SD_CARD_CMD_FLAG_R3              (FS_DEV_SD_CARD_CMD_FLAG_NONE                                                                        | FS_DEV_SD_CARD_CMD_FLAG_RESP)
#define  FS_DEV_SD_CARD_CMD_FLAG_R4              (FS_DEV_SD_CARD_CMD_FLAG_NONE                                                                        | FS_DEV_SD_CARD_CMD_FLAG_RESP)
#define  FS_DEV_SD_CARD_CMD_FLAG_R5              (FS_DEV_SD_CARD_CMD_FLAG_IX_VALID | FS_DEV_SD_CARD_CMD_FLAG_CRC_VALID                                | FS_DEV_SD_CARD_CMD_FLAG_RESP)
#define  FS_DEV_SD_CARD_CMD_FLAG_R5B             (FS_DEV_SD_CARD_CMD_FLAG_IX_VALID | FS_DEV_SD_CARD_CMD_FLAG_CRC_VALID | FS_DEV_SD_CARD_CMD_FLAG_BUSY | FS_DEV_SD_CARD_CMD_FLAG_RESP | FS_DEV_SD_CARD_CMD_FLAG_RESP_LONG)
#define  FS_DEV_SD_CARD_CMD_FLAG_R6              (FS_DEV_SD_CARD_CMD_FLAG_IX_VALID | FS_DEV_SD_CARD_CMD_FLAG_CRC_VALID                                | FS_DEV_SD_CARD_CMD_FLAG_RESP)
#define  FS_DEV_SD_CARD_CMD_FLAG_R7              (FS_DEV_SD_CARD_CMD_FLAG_IX_VALID | FS_DEV_SD_CARD_CMD_FLAG_CRC_VALID                                | FS_DEV_SD_CARD_CMD_FLAG_RESP)

/********************************************************************************************************
 *                                   CARD STATUS (R1) RESPONSE BIT DEFINES
 *
 * Note(s) : (1) See the [Ref 1], Table 4-35.
 *******************************************************************************************************/

#define  FS_DEV_SD_CARD_R1_AKE_SEQ_ERROR         DEF_BIT_03     // Error in the sequence of the authentication process.
#define  FS_DEV_SD_CARD_R1_APP_CMD               DEF_BIT_05     // Card will expect ACMD, or command interpreted as.
#define  FS_DEV_SD_CARD_R1_READY_FOR_DATA        DEF_BIT_08     // Corresponds to buffer empty signaling on the bus.
#define  FS_DEV_SD_CARD_R1_ERASE_RESET           DEF_BIT_13     // An erase seq was clr'd before executing.
#define  FS_DEV_SD_CARD_R1_CARD_ECC_DISABLED     DEF_BIT_14     // COmmand has been executed without using internal ECC.
#define  FS_DEV_SD_CARD_R1_WP_ERASE_SKIP         DEF_BIT_15     // Only partial addr space  erased due to WP'd blks.
#define  FS_DEV_SD_CARD_R1_CSD_OVERWRITE         DEF_BIT_16     // Read-only section of CSD does not match card.
#define  FS_DEV_SD_CARD_R1_ERROR                 DEF_BIT_19     // General or unknown error occurred.
#define  FS_DEV_SD_CARD_R1_CC_ERROR              DEF_BIT_20     // Internal card controller error.
#define  FS_DEV_SD_CARD_R1_CARD_ECC_FAILED       DEF_BIT_21     // Card internal ECC applied but failed to correct data.
#define  FS_DEV_SD_CARD_R1_ILLEGAL_COMMAND       DEF_BIT_22     // Command not legal for the card state.
#define  FS_DEV_SD_CARD_R1_COM_CRC_ERROR         DEF_BIT_23     // CRC check of previous command failed.
#define  FS_DEV_SD_CARD_R1_LOCK_UNLOCK_FAILED    DEF_BIT_24     // Sequence of password error has been detected.
#define  FS_DEV_SD_CARD_R1_CARD_IS_LOCKED        DEF_BIT_25     // Signals that card is locked by host.
#define  FS_DEV_SD_CARD_R1_WP_VIOLATION          DEF_BIT_26     // Set when the host attempts to write to protected blk.
#define  FS_DEV_SD_CARD_R1_ERASE_PARAM           DEF_BIT_27     // Invalid selection of write-blocks for erase occurred.
#define  FS_DEV_SD_CARD_R1_ERASE_SEQ_ERROR       DEF_BIT_28     // Error in the sequence of erase commands occurred.
#define  FS_DEV_SD_CARD_R1_BLOCK_LEN_ERROR       DEF_BIT_29     // Transferred block length is not allowed for card.
#define  FS_DEV_SD_CARD_R1_ADDRESS_ERROR         DEF_BIT_30     // Misaligned address used in the command.
#define  FS_DEV_SD_CARD_R1_OUT_OF_RANGE          DEF_BIT_31     // Command's argument was out of the allowed range.

//                                                                 Mask of card's state when receiving cmd.
#define  FS_DEV_SD_CARD_R1_CUR_STATE_MASK        DEF_BIT_FIELD(4, 9)
//                                                                 Msk that covers any err that might be reported in R1.
#define  FS_DEV_SD_CARD_R1_ERR_ANY               DEF_BIT_FIELD(13, 19)

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef struct fs_sd_card_data {
  void        *BufPtr;                                          // Ptr to internal buf used for some cmds.
  void        *RespPtr;                                         // Ptr to scratch pad buf used for some cmds.
  CPU_BOOLEAN IsInit;
} FS_SD_CARD_DATA;

/********************************************************************************************************
 *                                       SD CARD DATA DATA TYPE
 *******************************************************************************************************/

typedef struct fs_sd_card_pm_item FS_SD_CARD_PM_ITEM;

typedef struct fs_sd_card FS_SD_CARD;

struct fs_sd_card {
  FS_SD                    SD;

  CPU_INT32U               RCA;
  CPU_INT32U               BlkCntMax;
  CPU_INT08U               BusWidth;

  const FS_SD_CARD_BSP_API *BspPtr;
  void                     *BspDataPtr;

#if (FS_STORAGE_CFG_CTR_ERR_EN == DEF_ENABLED)
  CPU_INT16U               ErrRdCtr;
  CPU_INT16U               ErrWrCtr;

  CPU_INT16U               ErrCardBusyCtr;
  CPU_INT16U               ErrCardUnknownCtr;
  CPU_INT16U               ErrCardWaitTimeoutCtr;
  CPU_INT16U               ErrCardRespTimeoutCtr;
  CPU_INT16U               ErrCardRespChksumCtr;
  CPU_INT16U               ErrCardRespCmdIxCtr;
  CPU_INT16U               ErrCardRespEndBitCtr;
  CPU_INT16U               ErrCardRespCtr;
  CPU_INT16U               ErrCardDataUnderrunCtr;
  CPU_INT16U               ErrCardDataOverrunCtr;
  CPU_INT16U               ErrCardDataTimeoutCtr;
  CPU_INT16U               ErrCardDataChksumCtr;
  CPU_INT16U               ErrCardDataStartBitCtr;
  CPU_INT16U               ErrCardDataCtr;
#endif
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                               VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static FS_SD_CARD_DATA FS_SD_Card_Data = {
  .RespPtr = DEF_NULL,
  .BufPtr = DEF_NULL,
  .IsInit = DEF_NO
};

/********************************************************************************************************
 *                                       COMMAND RESPONSE TYPES
 *
 * Note(s) : (1) Detailed command descriptions are given in [Ref 1], Section 4.7.5, and [Ref 2], Section 7.9.4.
 *
 *           (2) MMC command names are enclosed in parenthesis when different from SD.
 *******************************************************************************************************/

static const CPU_INT08U FS_SD_Card_CmdRespType[] = {
  FS_DEV_SD_CARD_RESP_TYPE_NONE     |  FS_DEV_MMC_RESP_TYPE_NONE, // CMD0     GO_IDLE_STATE  // SD MMC SDIO
  FS_DEV_SD_CARD_RESP_TYPE_R3       |  FS_DEV_MMC_RESP_TYPE_R3,   // CMD1    (SEND_OP_COND)  // MMC
  FS_DEV_SD_CARD_RESP_TYPE_R2       |  FS_DEV_MMC_RESP_TYPE_R2,   // CMD2     ALL_SEND_CID  // SD MMC SDIO
  FS_DEV_SD_CARD_RESP_TYPE_R6       |  FS_DEV_MMC_RESP_TYPE_R1,   // CMD3     SEND_RELATIVE_ADDR  // SD MMC SDIO
  FS_DEV_SD_CARD_RESP_TYPE_NONE     |  FS_DEV_MMC_RESP_TYPE_NONE, // CMD4     SET_DSR  // SD MMC SDIO
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_R1B,  // CMD5     IO_SEND_OP_CMD  // SDIO
  FS_DEV_SD_CARD_RESP_TYPE_R1B      |  FS_DEV_MMC_RESP_TYPE_R1B,  // CMD6    (SWITCH)  // MMC
  FS_DEV_SD_CARD_RESP_TYPE_R1B      |  FS_DEV_MMC_RESP_TYPE_R1B,  // CMD7     SELECT/DESELECT_CARD  // SD MMC SDIO //*
  FS_DEV_SD_CARD_RESP_TYPE_R7       |  FS_DEV_MMC_RESP_TYPE_R1,   // CMD8     SEND_IF_COND  (SEND_EXT_CSD)  // SD //*
  FS_DEV_SD_CARD_RESP_TYPE_R2       |  FS_DEV_MMC_RESP_TYPE_R2,   // CMD9     SEND_CSD  // SD MMC SDIO
  FS_DEV_SD_CARD_RESP_TYPE_R2       |  FS_DEV_MMC_RESP_TYPE_R2,   // CMD10    SEND_CID  // SD MMC SDIO
  FS_DEV_SD_CARD_RESP_TYPE_R1       |  FS_DEV_MMC_RESP_TYPE_R1,   // CMD11   (READ_DAT_UNTIL_STOP)  // MMC
  FS_DEV_SD_CARD_RESP_TYPE_R1B      |  FS_DEV_MMC_RESP_TYPE_R1B,  // CMD12    STOP_TRANSMISSION  // SD MMC SDIO //*
  FS_DEV_SD_CARD_RESP_TYPE_R1       |  FS_DEV_MMC_RESP_TYPE_R1,   // CMD13    SEND_STATUS  // SD MMC SDIO
  FS_DEV_SD_CARD_RESP_TYPE_R1       |  FS_DEV_MMC_RESP_TYPE_R1,   // CMD14   (BUSTEST_R)  // MMC
  FS_DEV_SD_CARD_RESP_TYPE_NONE     |  FS_DEV_MMC_RESP_TYPE_NONE, // CMD15    GO_INACTIVE_STATE  // SD MMC SDIO

  FS_DEV_SD_CARD_RESP_TYPE_R1       |  FS_DEV_MMC_RESP_TYPE_R1,   // CMD16    SET_BLOCKLEN  // SD MMC SDIO
  FS_DEV_SD_CARD_RESP_TYPE_R1       |  FS_DEV_MMC_RESP_TYPE_R1,   // CMD17    READ_SINGLE_BLOCK  // SD MMC SDIO
  FS_DEV_SD_CARD_RESP_TYPE_R1       |  FS_DEV_MMC_RESP_TYPE_R1,   // CMD18    READ_MULTIPLE_BLOCK  // SD MMC SDIO
  FS_DEV_SD_CARD_RESP_TYPE_R1       |  FS_DEV_MMC_RESP_TYPE_R1,   // CMD19   (BUSTEST_W)  // MMC
  FS_DEV_SD_CARD_RESP_TYPE_R1       |  FS_DEV_MMC_RESP_TYPE_R1,   // CMD20   (WRITE_DAT_UNTIL_STOP)  // MMC
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_R1       |  FS_DEV_MMC_RESP_TYPE_R1,   // CMD23    SET_BLOCK_COUNT  // MMC
  FS_DEV_SD_CARD_RESP_TYPE_R1       |  FS_DEV_MMC_RESP_TYPE_R1,   // CMD24    WRITE_BLOCK  // SD MMC SDIO
  FS_DEV_SD_CARD_RESP_TYPE_R1       |  FS_DEV_MMC_RESP_TYPE_R1,   // CMD25    WRITE_MULTIPLE_BLOCK  // SD MMC SDIO
  FS_DEV_SD_CARD_RESP_TYPE_R1       |  FS_DEV_MMC_RESP_TYPE_R1,   // CMD26   (PROGRAM_CID)  // MMC
  FS_DEV_SD_CARD_RESP_TYPE_R1       |  FS_DEV_MMC_RESP_TYPE_R1,   // CMD27    PROGRAM_CSD  // SD MMC SDIO
  FS_DEV_SD_CARD_RESP_TYPE_R1B      |  FS_DEV_MMC_RESP_TYPE_R1B,  // CMD28    SET_WRITE_PROT  // SD MMC SDIO
  FS_DEV_SD_CARD_RESP_TYPE_R1B      |  FS_DEV_MMC_RESP_TYPE_R1B,  // CMD29    CLR_WRITE_PROT  // SD MMC SDIO
  FS_DEV_SD_CARD_RESP_TYPE_R1       |  FS_DEV_MMC_RESP_TYPE_R1,   // CMD30    SEND_WRITE_PROT  // SD MMC SDIO
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_R1,   // CMD31    (SEND_WRITE_PROT_TYPE)  // MMC

  FS_DEV_SD_CARD_RESP_TYPE_R1       |  FS_DEV_MMC_RESP_TYPE_R1,   // CMD32    ERASE_WR_BLK_START  // SD     SDIO
  FS_DEV_SD_CARD_RESP_TYPE_R1       |  FS_DEV_MMC_RESP_TYPE_R1,   // CMD33    ERASE_WR_BLK_END  // SD     SDIO
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_R1       |  FS_DEV_MMC_RESP_TYPE_R1,   // CMD35   (ERASE_GROUP_START)  // MMC
  FS_DEV_SD_CARD_RESP_TYPE_R1       |  FS_DEV_MMC_RESP_TYPE_R1,   // CMD36   (ERASE_GROUP_END)  // MMC
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_R1B      |  FS_DEV_MMC_RESP_TYPE_R1B,  // CMD38    ERASE  // SD MMC SDIO
  FS_DEV_SD_CARD_RESP_TYPE_R4       |  FS_DEV_MMC_RESP_TYPE_R4,   // CMD39   (FAST_IO)  // MMC
  FS_DEV_SD_CARD_RESP_TYPE_R5       |  FS_DEV_MMC_RESP_TYPE_R5,   // CMD40   (GO_IRQ_STATE)  // MMC
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_R1       |  FS_DEV_MMC_RESP_TYPE_R1,   // CMD42    LOCK_UNLOCK  // SD MMC SDIO
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,

  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN, // CMD52    IO_RW_DIRECT  // SDIO
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN, // CMD53    IO_RW_EXTENDED  // SDIO
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_R1       |  FS_DEV_MMC_RESP_TYPE_R1,      // CMD55    APP_CMD  // SD MMC SDIO
  FS_DEV_SD_CARD_RESP_TYPE_R1       |  FS_DEV_MMC_RESP_TYPE_R1,      // CMD56    GEN_CMD  // SD MMC SDIO
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,

  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_R1       |  FS_DEV_MMC_RESP_TYPE_UNKNOWN, // ACMD6    SET_BUS_WIDTH  // SD
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_R1       |  FS_DEV_MMC_RESP_TYPE_UNKNOWN, // ACMD13  SD_STATUS  // SD
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,

  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_R1       |  FS_DEV_MMC_RESP_TYPE_UNKNOWN, // ACMD22  SEND_NUM_WR_BLOCKS  // SD
  FS_DEV_SD_CARD_RESP_TYPE_R1       |  FS_DEV_MMC_RESP_TYPE_UNKNOWN, // ACMD23  SEND_WR_BLK_ERASE_CNT  // SD
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,

  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_R3       |  FS_DEV_MMC_RESP_TYPE_UNKNOWN, // ACMD41  SD_SEND_OP_COND  // SD
  FS_DEV_SD_CARD_RESP_TYPE_R1       |  FS_DEV_MMC_RESP_TYPE_UNKNOWN, // ACMD42  SET_CLR_CARD_DETECT  // SD
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,

  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_R1       |  FS_DEV_MMC_RESP_TYPE_UNKNOWN, // ACMD51  SEND_SCR  // SD
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN  |  FS_DEV_MMC_RESP_TYPE_UNKNOWN,
};

static const CPU_INT32U FS_SD_Card_CmdFlag[] = {
  FS_DEV_SD_CARD_CMD_FLAG_UNKNOWN,
  FS_DEV_SD_CARD_CMD_FLAG_NONE,
  FS_DEV_SD_CARD_CMD_FLAG_R1,
  FS_DEV_SD_CARD_CMD_FLAG_R1B,
  FS_DEV_SD_CARD_CMD_FLAG_R2,
  FS_DEV_SD_CARD_CMD_FLAG_R3,
  FS_DEV_SD_CARD_CMD_FLAG_R4,
  FS_DEV_SD_CARD_CMD_FLAG_R5,
  FS_DEV_SD_CARD_CMD_FLAG_R5B,
  FS_DEV_SD_CARD_CMD_FLAG_R6,
  FS_DEV_SD_CARD_CMD_FLAG_R7
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL MACRO'S
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           COUNTER MACRO'S
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_CTR_STAT_EN == DEF_ENABLED)                 // ----------------- STAT CTR MACRO'S -----------------

#define  FS_DEV_SD_CARD_STAT_RD_CTR_INC(p_sd_card_data)          {  FS_BLK_DEV_CTR_STAT_INC((p_sd_card_data)->StatRdCtr);                }
#define  FS_DEV_SD_CARD_STAT_WR_CTR_INC(p_sd_card_data)          {  FS_BLK_DEV_CTR_STAT_INC((p_sd_card_data)->StatWrCtr);                }

#define  FS_DEV_SD_CARD_STAT_RD_CTR_ADD(p_sd_card_data, val)     {  FS_BLK_DEV_CTR_STAT_ADD((p_sd_card_data)->StatRdCtr, (FS_CTR)(val)); }
#define  FS_DEV_SD_CARD_STAT_WR_CTR_ADD(p_sd_card_data, val)     {  FS_BLK_DEV_CTR_STAT_ADD((p_sd_card_data)->StatWrCtr, (FS_CTR)(val)); }

#else

#define  FS_DEV_SD_CARD_STAT_RD_CTR_INC(p_sd_card_data)
#define  FS_DEV_SD_CARD_STAT_WR_CTR_INC(p_sd_card_data)

#define  FS_DEV_SD_CARD_STAT_RD_CTR_ADD(p_sd_card_data, val)
#define  FS_DEV_SD_CARD_STAT_WR_CTR_ADD(p_sd_card_data, val)

#endif

#if (FS_STORAGE_CFG_CTR_ERR_EN == DEF_ENABLED)                  // ------------------ ERR CTR MACRO'S -----------------

#define  FS_DEV_SD_CARD_ERR_RD_CTR_INC(p_sd_card_data)           {  FS_BLK_DEV_CTR_ERR_INC((p_sd_card_data)->ErrRdCtr);                  }
#define  FS_DEV_SD_CARD_ERR_WR_CTR_INC(p_sd_card_data)           {  FS_BLK_DEV_CTR_ERR_INC((p_sd_card_data)->ErrWrCtr);                  }

#define  FS_DEV_SD_CARD_HANDLE_ERR_BSP(p_sd_card_data, err)         FS_SD_Card_HandleErrBSP((p_sd_card_data), (err))

#else

#define  FS_DEV_SD_CARD_ERR_RD_CTR_INC(p_sd_card_data)
#define  FS_DEV_SD_CARD_ERR_WR_CTR_INC(p_sd_card_data)

#define  FS_DEV_SD_CARD_HANDLE_ERR_BSP(p_sd_card_data, err)

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                               BLOCK DEVICE INTERFACE FUNCTION PROTOTYPES
 *******************************************************************************************************/

static FS_MEDIA *FS_SD_Card_MediaAdd(const FS_MEDIA_PM_ITEM *p_pm_item,
                                     RTOS_ERR               *p_err);

static void FS_SD_Card_MediaRem(FS_MEDIA *p_media,
                                RTOS_ERR *p_err);

static FS_BLK_DEV *FS_SD_Card_BlkDevAdd(FS_MEDIA *p_media,
                                        RTOS_ERR *p_err);

static void FS_SD_Card_BlkDevRem(FS_BLK_DEV *p_blk_dev,
                                 RTOS_ERR   *p_err);

static void FS_SD_Card_Open(FS_BLK_DEV *p_blk_dev,
                            RTOS_ERR   *p_err);

static void FS_SD_Card_Close(FS_BLK_DEV *p_blk_dev,
                             RTOS_ERR   *p_err);

static void FS_SD_Card_Rd(FS_BLK_DEV *p_blk_dev,
                          void       *p_dest,
                          FS_LB_NBR  start,
                          FS_LB_QTY  cnt,
                          RTOS_ERR   *p_err);

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_SD_Card_Wr(FS_BLK_DEV *p_blk_dev,
                          void       *p_src,
                          FS_LB_NBR  start,
                          FS_LB_QTY  cnt,
                          RTOS_ERR   *p_err);

static void FS_SD_Card_Sync(FS_BLK_DEV *p_blk_dev,
                            RTOS_ERR   *p_err);

static void FS_SD_Card_Trim(FS_BLK_DEV *p_blk_dev,
                            FS_LB_NBR  lb_nbr,
                            RTOS_ERR   *p_err);
#endif

static void FS_SD_Card_Query(FS_BLK_DEV      *p_blk_dev,
                             FS_BLK_DEV_INFO *p_info,
                             RTOS_ERR        *p_err);

static CPU_SIZE_T FS_SD_Card_AlignReqGet(FS_MEDIA *p_media,
                                         RTOS_ERR *p_err);

static CPU_BOOLEAN FS_SD_Card_IsConn(FS_MEDIA *p_media);

/********************************************************************************************************
 *                                   PHY INTERFACE FUNCTION PROTOTYPES
 *******************************************************************************************************/

static void FS_SD_Card_OpenInternal(FS_SD    *p_sd,
                                    RTOS_ERR *p_err);

static void FS_SD_Card_CloseInternal(FS_SD    *p_sd,
                                     RTOS_ERR *p_err);

static void FS_SD_Card_InfoGet(FS_SD      *p_sd,
                               FS_SD_INFO *p_info);

static void FS_SD_Card_CID_Rd(FS_SD      *p_sd,
                              CPU_INT08U *p_dest,
                              RTOS_ERR   *p_err);

static void FS_SD_Card_CSD_Rd(FS_SD      *p_sd,
                              CPU_INT08U *p_dest,
                              RTOS_ERR   *p_err);

/********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 *******************************************************************************************************/

static CPU_INT08U FS_SD_Card_MakeRdy(FS_SD_CARD *p_sd_card,
                                     RTOS_ERR   *p_err);

static void FS_SD_Card_Cmd(FS_SD_CARD *p_sd_card,
                           CPU_INT08U cmd_nbr,
                           CPU_INT32U arg,
                           RTOS_ERR   *p_err);

static void FS_SD_Card_CmdRShort(FS_SD_CARD *p_sd_card,
                                 CPU_INT08U cmd_nbr,
                                 CPU_INT32U arg,
                                 CPU_INT32U *p_resp,
                                 RTOS_ERR   *p_err);

static void FS_SD_Card_CmdRLong(FS_SD_CARD *p_sd_card,
                                CPU_INT08U cmd_nbr,
                                CPU_INT32U arg,
                                CPU_INT32U *p_resp,
                                RTOS_ERR   *p_err);

static void FS_SD_Card_AppCmdRShort(FS_SD_CARD *p_sd_card,
                                    CPU_INT08U acmd_nbr,
                                    CPU_INT32U arg,
                                    CPU_INT32U *p_resp,
                                    RTOS_ERR   *p_err);

static void FS_SD_Card_DataRd(FS_SD_CARD *p_sd_card,
                              CPU_INT08U cmd_nbr,
                              CPU_INT32U arg,
                              CPU_INT08U *p_dest,
                              CPU_INT32U size,
                              CPU_INT32U cnt,
                              RTOS_ERR   *p_err);

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_SD_Card_DataWr(FS_SD_CARD *p_sd_card,
                              CPU_INT08U cmd_nbr,
                              CPU_INT32U arg,
                              CPU_INT08U *p_src,
                              CPU_INT32U size,
                              CPU_INT32U cnt,
                              RTOS_ERR   *p_err);
#endif

static void FS_SD_Card_CID_Send(FS_SD_CARD *p_sd_card,
                                CPU_INT08U *p_dest,
                                RTOS_ERR   *p_err);

static void FS_SD_Card_CSD_Send(FS_SD_CARD *p_sd_card,
                                CPU_INT08U *p_dest,
                                RTOS_ERR   *p_err);

static void FS_SD_Card_EXT_CSD_Send(FS_SD_CARD *p_sd_card,
                                    CPU_INT08U *p_dest,
                                    RTOS_ERR   *p_err);

static void FS_SD_Card_SCR_Send(FS_SD_CARD *p_sd_card,
                                CPU_INT08U *p_dest,
                                RTOS_ERR   *p_err);

static void FS_SD_Card_SDStatusSend(FS_SD_CARD *p_sd_card,
                                    CPU_INT08U *p_dest,
                                    RTOS_ERR   *p_err);

static CPU_INT08U FS_SD_Card_BusWidthSet(FS_SD_CARD *p_sd_card,
                                         CPU_INT08U width);

static void FS_SD_Card_Stop(FS_SD_CARD *p_sd_card,
                            RTOS_ERR   *p_err);

static void FS_SD_Card_Sel(FS_SD_CARD  *p_sd_card,
                           CPU_BOOLEAN sel,
                           RTOS_ERR    *p_err);

static void FS_SD_Card_WaitWhileBusy(FS_SD_CARD *p_sd_card,
                                     RTOS_ERR   *p_err);

#if (FS_STORAGE_CFG_CTR_ERR_EN == DEF_ENABLED)
static void FS_SD_Card_HandleErrBSP(FS_SD_CARD         *p_sd_card,
                                    FS_DEV_SD_CARD_ERR err);
#endif

static void FS_SD_Card_CmdInit(FS_DEV_SD_CARD_CMD *p_cmd,
                               CPU_INT08U         cmd_nbr,
                               CPU_INT32U         arg,
                               CPU_INT08U         card_type);

/********************************************************************************************************
 *                                       BLOCK DEVICE INTERFACE
 *******************************************************************************************************/

const FS_MEDIA_API FS_SD_Card_MediaApi = {
  .Add = FS_SD_Card_MediaAdd,
  .Rem = FS_SD_Card_MediaRem,
  .AlignReqGet = FS_SD_Card_AlignReqGet,
  .IsConn = FS_SD_Card_IsConn
};

const FS_BLK_DEV_API FS_SD_Card_BlkDevApi = {
  .Add = FS_SD_Card_BlkDevAdd,
  .Rem = FS_SD_Card_BlkDevRem,
  .Open = FS_SD_Card_Open,
  .Close = FS_SD_Card_Close,
  .Rd = FS_SD_Card_Rd,
#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
  .Wr = FS_SD_Card_Wr,
  .Sync = FS_SD_Card_Sync,
  .Trim = FS_SD_Card_Trim,
#endif
  .Query = FS_SD_Card_Query
};

static const FS_SD_BUS_API FS_SD_Card_BusApi = {
  .Open = FS_SD_Card_OpenInternal,
  .Close = FS_SD_Card_CloseInternal,
  .InfoGet = FS_SD_Card_InfoGet,
  .CidRd = FS_SD_Card_CID_Rd,
  .CsdRd = FS_SD_Card_CSD_Rd
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                        PHY INTERFACE FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           FS_SD_Card_OpenInternal()
 *
 * @brief    Open an SD card.
 *
 * @param    p_sd    Pointer to a SD card.
 *
 * @param    p_err   Error pointer.
 *
 * @note     (1) The initialization flow for a SD card is charted in [Ref 1] Figure 4-2.  Similarly,
 *               the power-up sequence for a MMC card is given in Section 1.11 of [Ref 2].
 *               @verbatim
 *                                               POWER ON
 *                                                   |
 *                                                   |
 *                                                   v
 *                                                   CMD0
 *                                                   |
 *                                                   |
 *                           NO RESPONSE              v             CARD RETURNS RESPONSE
 *                                   +---------------CMD8---------------+
 *                                   |                                  |
 *                                   |                                  |
 *                                   v                                  v             NO
 *           NO RESPONSE       ACMD41                            RESPONSE------------------>UNUSABLE
 *           ------------------w/ HCS = 0<---------+                VALID?                      CARD
 *                                   |              |                   |
 *                                   |              | BUSY              |
 *                                   v              |                   v
 *                           CARD IS READY?-------+                 ACMD41
 *                                   |                     +------->w/ HCS = 1
 *                                   | YES                 |             |          TIMEOUT
 *                                   |                BUSY |             |          OR NO
 *                                   |                     |             v          RESPONSE
 *                                   |                     +-------CARD IS READY?------------->UNUSABLE
 *            ------------+          |                                   |                       CARD
 *                        |          |                                   |
 *                        | BUSY     |                                   |
 *                        |          |                                   | YES
 *            EADY?-------+          |                                   |
 *                                   |                                   |
 *            ES                     |                                   |
 *                                   |                     NO            v             YES
 *                                   |               +----------- CCS IN RESPONSE? -----------+
 *                                   |               |                                        |
 *                                   v               v                                        v
 *                           VER 1.x STD      VER 2.0+ STD                            VER 2.0+ HIGH
 *                           CAPACITY SD      CAPACITY SD                              CAPACITY SD
 *                                   |               |                                        |
 *                                   |               |                                        |
 *            -----------------------+---------------+----------------------------------------+
 *                                                   |
 *                                                   |
 *                                                   v
 *                                               CMD2
 *                                                   |
 *                                                   |
 *                                                   v
 *                                               CMD3
 *               @endverbatim
 * @note     (2) After the type of card is determined, the card's Relative Card Address (RCA) must be
 *               identified or assigned.
 *               - (a) For SD cards, CMD2 is issued; in response, "[c]ard that is unidentified ... sends
 *                     its CID number as the response" and "goes into the Identification state".  CMD3
 *                     is then issued to get that card's RCA, which is "used to address the card in the
 *                     future data transfer state".
 *               - (b) For MMC cards, CMD2 is issued; "there should be only one card which successfully
 *                     sends its full CID-number to the host".  This card "goes into Identification state".
 *                     CMD3 is then issued "to assign to this card a relative card address (RCA) ...
 *                     which will be used to address the card in the future data transfer mode".
 *
 * @note     (3) The caller is responsible for serializing the access to this buffer.
 *******************************************************************************************************/
static void FS_SD_Card_OpenInternal(FS_SD    *p_sd,
                                    RTOS_ERR *p_err)
{
  FS_SD_CARD         *p_sd_card;
  FS_SD_INFO         *p_sd_info;
  FS_SD_CARD_BSP_API *p_bsp;
  CPU_INT08U         card_type;
  CPU_INT16U         retries;
  CPU_INT32U         rca = 0u;
  CPU_INT32U         resp1;
  CPU_INT32U         resp6;
  CPU_INT08U         width;
  CPU_BOOLEAN        ok;
  RTOS_ERR           err_tmp;

  WITH_SCOPE_BEGIN(p_err) {
    p_sd_card = (FS_SD_CARD *)p_sd;
    p_sd_info = (FS_SD_INFO *)&p_sd_card->SD.Info;
    p_bsp = (FS_SD_CARD_BSP_API *)p_sd_card->BspPtr;

    FS_SD_ClrInfo(p_sd_info);                                   // Clr old info.

    p_sd_card->RCA = 0u;
    p_sd_card->BlkCntMax = 0u;
    p_sd_card->BusWidth = 1u;

    ok = p_bsp->Open();                                         // Init HW.
    ASSERT_BREAK(ok, RTOS_ERR_IO);
    //                                                             open operation possibly done in FS_SD_Card_IsConn().
    ON_BREAK {
      p_sd_card->SD.IsClosed = DEF_YES;                         // Mark SD as closed. Open has failed.
      p_sd_card->BspPtr->Unlock();                              // Release bus lock.
      p_bsp->Close();
      break;
    } WITH {
      //                                                           Set clk to dflt clk.
      p_bsp->ClkFreqSet((CPU_INT32U)FS_SD_DFLT_CLK_SPD);
      p_bsp->TimeoutDataSet(0xFFFFFFFFuL);
      p_bsp->TimeoutRespSet(1000u);
      p_bsp->BusWidthSet(1u);

      p_sd_card->BspPtr->Lock();                                // Acquire bus lock.
                                                                // -------------- SEND CMD0 TO RESET BUS --------------
      retries = 0u;
      while (retries < FS_DEV_SD_CARD_RETRIES_CMD) {
        RTOS_ERR_SET(err_tmp, RTOS_ERR_NONE);
        FS_SD_Card_Cmd(p_sd_card,
                       FS_DEV_SD_CMD_GO_IDLE_STATE,
                       0u,
                       &err_tmp);
        if (RTOS_ERR_CODE_GET(err_tmp) == RTOS_ERR_NONE) {
          break;
        }
        KAL_Dly(2u);
        retries += 1u;
      }

      //                                                           If card never responded, rtn err.
      ASSERT_BREAK(retries < FS_DEV_SD_CARD_RETRIES_CMD, RTOS_ERR_IO);

      //                                                           --------------- DETERMINE CARD TYPE ----------------
      //                                                           Make card rd & determine card type.
      BREAK_ON_ERR(card_type = FS_SD_Card_MakeRdy(p_sd_card, p_err));

      p_sd_info->CardType = card_type;

      if ((card_type == FS_DEV_SD_CARDTYPE_SD_V2_0_HC)
          || (card_type == FS_DEV_SD_CARDTYPE_MMC_HC)) {
        p_sd_info->HighCapacity = DEF_YES;
      }

      //                                                           SET OR GET RCA (see Note #2)
      BREAK_ON_ERR(FS_SD_Card_CID_Send(p_sd_card,               // Get CID reg from card.
                                       (CPU_INT08U *)FS_SD_Card_Data.BufPtr,
                                       p_err));

      FS_SD_ParseCID((CPU_INT08U *)FS_SD_Card_Data.BufPtr,      // Parse CID info.
                     p_sd_info,
                     card_type);

      //                                                           Perform CMD3 (R6 resp).
      BREAK_ON_ERR(switch) (card_type) {
        case FS_DEV_SD_CARDTYPE_MMC:
        case FS_DEV_SD_CARDTYPE_MMC_HC:
          rca = (CPU_INT32U)FS_DEV_SD_CARD_RCA_DFLT << 16;      // For MMC cards, RCA assigned by host.
          BREAK_ON_ERR(FS_SD_Card_CmdRShort(p_sd_card,
                                            FS_DEV_SD_CMD_SEND_RELATIVE_ADDR,
                                            rca,
                                            &resp6,
                                            p_err));
          break;

        case FS_DEV_SD_CARDTYPE_SD_V1_X:
        case FS_DEV_SD_CARDTYPE_SD_V2_0:
        case FS_DEV_SD_CARDTYPE_SD_V2_0_HC:
        default:
          BREAK_ON_ERR(FS_SD_Card_CmdRShort(p_sd_card,
                                            FS_DEV_SD_CMD_SEND_RELATIVE_ADDR,
                                            0u,
                                            &resp6,
                                            p_err));

          rca = resp6 & 0xFFFF0000uL;                           // For SD cards, RCA returned by card.
          break;
      }

      p_sd_card->RCA = rca;                                     // Save RCA.

      //                                                           -------------------- RD CSD REG --------------------
      //                                                           Get CSD reg from card.
      BREAK_ON_ERR(FS_SD_Card_CSD_Send(p_sd_card,
                                       (CPU_INT08U *)FS_SD_Card_Data.BufPtr,
                                       p_err));

      //                                                           Parse CSD info.
      ok = FS_SD_ParseCSD((CPU_INT08U *)FS_SD_Card_Data.BufPtr,
                          p_sd_info,
                          card_type);
      ASSERT_BREAK(ok, RTOS_ERR_IO);

      p_bsp->ClkFreqSet(p_sd_info->ClkFreq);                    // Set clk freq with max val supported by SD Card.

      BREAK_ON_ERR(FS_SD_Card_Sel(p_sd_card, DEF_YES, p_err));

      //                                                           ------------------- SET BLK LEN --------------------
      BREAK_ON_ERR(FS_SD_Card_CmdRShort(p_sd_card,              // Perform CMD16 (R1 resp).
                                        FS_DEV_SD_CMD_SET_BLOCKLEN,
                                        FS_DEV_SD_BLK_SIZE,
                                        &resp1,
                                        p_err));

      p_sd_card->BlkCntMax = p_bsp->BlkCntMaxGet(FS_DEV_SD_BLK_SIZE);
      if (p_sd_card->BlkCntMax < 1u) {
        p_sd_card->BlkCntMax = 1u;
      }

      //                                                           ------------------ SET BUS WIDTH -------------------
      p_bsp->BusWidthSet(1u);
      p_sd_card->BusWidth = 1u;
      width = p_bsp->BusWidthMaxGet();
      if (width < 1u) {
        width = 1u;
      }

      width = FS_SD_Card_BusWidthSet(p_sd_card, width);
      LOG_VRB(("Bus width set to ", (u)width, "."));
      p_sd_card->BusWidth = width;

      //                                                           -------------- READ EXT_CSD (MMC-HC) ---------------
      if (card_type == FS_DEV_SD_CARDTYPE_MMC_HC) {             // See Note #3.
        BREAK_ON_ERR(FS_SD_Card_EXT_CSD_Send(p_sd_card,
                                             (CPU_INT08U *)FS_SD_Card_Data.BufPtr,
                                             p_err));

        //                                                         Parse EXT_CSD info.
        FS_SD_ParseEXT_CSD((CPU_INT08U *)FS_SD_Card_Data.BufPtr,
                           p_sd_info);
      }

      p_sd_card->SD.IsClosed = DEF_NO;                          // SD open completed without err.
                                                                // ---------------- OUTPUT TRACE INFO -----------------
#if LOG_VRB_IS_EN()
      FS_SD_TraceInfo(p_sd_info);
#endif
      p_sd_card->BspPtr->Unlock();                              // Release bus lock.
    }
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                       FS_SD_Card_CloseInternal()
 *
 * @brief    Close an sd card.
 *
 * @param    p_sd    Pointer to a SD card.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/
static void FS_SD_Card_CloseInternal(FS_SD    *p_sd,
                                     RTOS_ERR *p_err)
{
  FS_SD_CARD         *p_sd_card;
  FS_SD_CARD_BSP_API *p_bsp;

  PP_UNUSED_PARAM(p_err);

  p_sd_card = (FS_SD_CARD *)p_sd;
  p_bsp = (FS_SD_CARD_BSP_API *)p_sd_card->BspPtr;

  p_sd_card->SD.IsClosed = DEF_YES;
  p_bsp->Close();
}

/****************************************************************************************************//**
 *                                           FS_SD_Card_InfoGet()
 *
 * @brief    Get low-level information about SD/MMC card.
 *
 * @param    p_sd    Pointer to a SD card.
 *
 * @param    p_info  Pointer to structure that will receive SD/MMC card information.
 *******************************************************************************************************/
static void FS_SD_Card_InfoGet(FS_SD      *p_sd,
                               FS_SD_INFO *p_info)
{
  *p_info = p_sd->Info;
}

/****************************************************************************************************//**
 *                                           FS_SD_Card_CID_Rd()
 *
 * @brief    Read SD/MMC Card ID (CID) register.
 *
 * @param    p_sd    Pointer to a SD card.
 *
 * @param    p_dest  Pointer to 16-byte buffer that will receive SD/MMC Card ID register.
 *
 * @param    p_err   Error pointer.
 *
 * @note     (1) For SD cards, the structure of the CID is defined in [Ref 1] Section 5.1.
 *               For MMC cards, the structure of the CID is defined in [Ref 2] Section 8.2.
 *******************************************************************************************************/
static void FS_SD_Card_CID_Rd(FS_SD      *p_sd,
                              CPU_INT08U *p_dest,
                              RTOS_ERR   *p_err)
{
  FS_SD_CARD *p_sd_card;

  p_sd_card = (FS_SD_CARD *)p_sd;
  p_sd_card->BspPtr->Lock();                                    // Acquire bus lock.
  FS_SD_Card_CID_Send(p_sd_card, p_dest, p_err);
  p_sd_card->BspPtr->Unlock();                                  // Release bus lock.
}

/****************************************************************************************************//**
 *                                           FS_SD_Card_CSD_Rd()
 *
 * @brief    Read SD/MMC Card-Specific Data (CSD) register.
 *
 * @param    p_sd    Pointer to a SD card.
 *
 * @param    p_dest  Pointer to 16-byte buffer that will receive SD/MMC Card specific data register.
 *
 * @param    p_err   Error pointer.
 *
 * @note     (1) For SD v1.x & SD v2.0 standard capacity, the structure of the CSD is defined in
 *               [Ref 1], Section 5.3.2.
 *               For MMC cards, the structure of the CSD is defined in [Ref 2], Section 8.3.
 *               For SD v2.0 high capacity cards, the structure of the CSD is defined in [Ref 1],
 *               Section 5.3.3.
 *******************************************************************************************************/
static void FS_SD_Card_CSD_Rd(FS_SD      *p_sd,
                              CPU_INT08U *p_dest,
                              RTOS_ERR   *p_err)
{
  FS_SD_CARD *p_sd_card;

  p_sd_card = (FS_SD_CARD *)p_sd;
  p_sd_card->BspPtr->Lock();                                    // Acquire bus lock.
  FS_SD_Card_CSD_Send(p_sd_card, p_dest, p_err);
  p_sd_card->BspPtr->Unlock();                                  // Release bus lock.
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                       DRIVER INTERFACE FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           FS_SD_Card_MediaAdd()
 *
 * @brief    Add a SD card media instance.
 *
 * @param    p_pm_item   Pointer to a media platform manager item.
 *
 * @param    p_err       Error pointer.
 *
 * @return   Pointer to the added media.
 *
 * @note     (1) The pointer 'RespPtr' will point to the middle of the internal buffer. It will be
 *               used as a scratch pad to read the content of the CID and CSD registers of the SD
 *               Card. This scratch pad area will allow byte-ordering manipulation so that CID and
 *               CSD data content will be correctly interpreted.
 *******************************************************************************************************/
static FS_MEDIA *FS_SD_Card_MediaAdd(const FS_MEDIA_PM_ITEM *p_pm_item,
                                     RTOS_ERR               *p_err)
{
  FS_SD_CARD_PM_ITEM *p_sd_card_pm_item;
  FS_SD_CARD         *p_sd_card;
  CORE_DECLARE_IRQ_STATE;

  p_sd_card_pm_item = (FS_SD_CARD_PM_ITEM *)p_pm_item;

  p_sd_card = (FS_SD_CARD *)Mem_SegAlloc("FS - SD Card instance",
                                         FSMedia_InitCfgPtr->MemSegPtr,
                                         sizeof(FS_SD_CARD),
                                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  //                                                               --------------------- INIT SD ----------------------
  FS_SD_ClrInfo(&p_sd_card->SD.Info);                           // Clr SD info.

#if (FS_STORAGE_CFG_CTR_ERR_EN == DEF_ENABLED)                  // Clr err ctrs.
  p_sd_card->ErrRdCtr = 0u;
  p_sd_card->ErrWrCtr = 0u;

  p_sd_card->ErrCardBusyCtr = 0u;
  p_sd_card->ErrCardUnknownCtr = 0u;
  p_sd_card->ErrCardWaitTimeoutCtr = 0u;
  p_sd_card->ErrCardRespTimeoutCtr = 0u;
  p_sd_card->ErrCardRespChksumCtr = 0u;
  p_sd_card->ErrCardRespCmdIxCtr = 0u;
  p_sd_card->ErrCardRespEndBitCtr = 0u;
  p_sd_card->ErrCardRespCtr = 0u;
  p_sd_card->ErrCardDataUnderrunCtr = 0u;
  p_sd_card->ErrCardDataOverrunCtr = 0u;
  p_sd_card->ErrCardDataTimeoutCtr = 0u;
  p_sd_card->ErrCardDataChksumCtr = 0u;
  p_sd_card->ErrCardDataStartBitCtr = 0u;
  p_sd_card->ErrCardDataCtr = 0u;
#endif

  p_sd_card->RCA = 0u;
  p_sd_card->BlkCntMax = 0u;
  p_sd_card->BusWidth = 0u;

  p_sd_card->SD.IsClosed = DEF_YES;                             // SD instance considered as closed when adding it.
  p_sd_card->SD.BusApiPtr = &FS_SD_Card_BusApi;
  p_sd_card->BspPtr = p_sd_card_pm_item->HwInfoPtr->BspApiPtr;
  p_sd_card->BspDataPtr = p_sd_card->BspPtr->Add(p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  //                                                               Alloc internal buf.
  CORE_ENTER_ATOMIC();
  if (FS_SD_Card_Data.BufPtr == DEF_NULL) {
    CPU_SIZE_T size_reqd;
    CPU_INT08U *p_buf_temp;

    FS_SD_Card_Data.BufPtr = Mem_SegAllocExt("FS - SD Card buffer",
                                             FSMedia_InitCfgPtr->MemSegPtr,
                                             FS_DEV_SD_CARD_BUF_SIZE,
                                             p_sd_card_pm_item->HwInfoPtr->AlignReq,
                                             &size_reqd,
                                             p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      CORE_EXIT_ATOMIC();
      return (DEF_NULL);
    }
    //                                                             Alloc a temp buf used for some cmds (see Note #1).
    p_buf_temp = (CPU_INT08U *)FS_SD_Card_Data.BufPtr;
    p_buf_temp += (FS_DEV_SD_CARD_BUF_SIZE / 2u);               // Temp buf will point to middle of internal buf.
    FS_SD_Card_Data.RespPtr = (void *)p_buf_temp;
  }
  CORE_EXIT_ATOMIC();

  return (&p_sd_card->SD.Media);
}

/****************************************************************************************************//**
 *                                           FS_SD_Card_MediaRem()
 *
 * @brief    Remove a SD Card media instance.
 *
 * @param    p_media     Pointer to a SD media.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_SD_Card_MediaRem(FS_MEDIA *p_media,
                                RTOS_ERR *p_err)
{
  PP_UNUSED_PARAM(p_media);
  PP_UNUSED_PARAM(p_err);

  RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_NOT_AVAIL,; );
}

/****************************************************************************************************//**
 *                                           FS_SD_Card_BlkDevAdd()
 *
 * @brief    Add a SD block device.
 *
 * @param    p_media     Pointer to a SD media instance.
 *
 * @param    p_err       Error pointer.
 *
 * @return   Pointer to block device.
 *******************************************************************************************************/
static FS_BLK_DEV *FS_SD_Card_BlkDevAdd(FS_MEDIA *p_media,
                                        RTOS_ERR *p_err)
{
  PP_UNUSED_PARAM(p_err);

  return (&((FS_SD *)p_media)->BlkDev);
}

/****************************************************************************************************//**
 *                                           FS_SD_Card_BlkDevRem()
 *
 * @brief    Remove a SD block device.
 *
 * @param    p_blk_dev   Pointer to a SD block device.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_SD_Card_BlkDevRem(FS_BLK_DEV *p_blk_dev,
                                 RTOS_ERR   *p_err)
{
  PP_UNUSED_PARAM(p_blk_dev);
  PP_UNUSED_PARAM(p_err);
}

/****************************************************************************************************//**
 *                                               FS_SD_Card_Open()
 *
 * @brief    Open (initialize) a device instance.
 *
 * @param    p_blk_dev   Pointer to a block device.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_SD_Card_Open(FS_BLK_DEV *p_blk_dev,
                            RTOS_ERR   *p_err)
{
  FS_SD_Card_OpenInternal((FS_SD *)p_blk_dev->MediaPtr, p_err);
}

/****************************************************************************************************//**
 *                                           FS_SD_Card_Close()
 *
 * @brief    Close a SD card.
 *
 * @param    p_blk_dev   Pointer to a SD card block device.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_SD_Card_Close(FS_BLK_DEV *p_blk_dev,
                             RTOS_ERR   *p_err)
{
  FS_SD_Card_CloseInternal((FS_SD *)p_blk_dev->MediaPtr, p_err);
}

/****************************************************************************************************//**
 *                                           FS_SD_Card_IsConn()
 *
 * @brief    Check whether a media is connected.
 *
 * @param    p_media     Pointer to media.
 *
 * @return   DEF_YES, if card is present and has not changed.
 *           DEF_NO,  otherwise.
 *
 * @note     (1) This IF statement is executed in 4 possible situations:
 *               - (a) Insertion of SD card (device was not mounted)
 *               - (b) Removal of SD card (device was mounted)
 *               - (c) Removal of SD card (device was mounted) followed quickly by a new SD Card
 *                     inserted
 *               - (d) SD Card closed by application.
 *******************************************************************************************************/
static CPU_BOOLEAN FS_SD_Card_IsConn(FS_MEDIA *p_media)
{
  FS_SD_CARD_BSP_API *p_bsp_api;
  FS_SD_CARD         *p_sd_card;
  CPU_INT32U         retries;
  CPU_BOOLEAN        is_closed;
  RTOS_ERR           err_tmp;
  CPU_BOOLEAN        is_conn;
  CPU_BOOLEAN        ok;

  p_sd_card = (FS_SD_CARD *)p_media;
  p_bsp_api = (FS_SD_CARD_BSP_API *)p_sd_card->BspPtr;

  is_closed = p_sd_card->SD.IsClosed;
  if (is_closed) {                                              // If the card was not mounted, perform bsp lvl open.
    ok = p_bsp_api->Open();
    if (!ok) {
      return (DEF_NO);
    }
  }

  //                                                               --------------- CHK IF CARD MOUNTED ----------------
  if (!is_closed) {                                             // If the card was mounted, chk if it is still mounted.
    RTOS_ERR_SET(err_tmp, RTOS_ERR_NONE);
    p_sd_card->BspPtr->Lock();                                  // Acquire bus lock.
    FS_SD_Card_CID_Send(p_sd_card,
                        (CPU_INT08U *)FS_SD_Card_Data.BufPtr,
                        &err_tmp);
    p_sd_card->BspPtr->Unlock();                                // Release bus lock.
    if (RTOS_ERR_CODE_GET(err_tmp) == RTOS_ERR_NONE) {
      return (DEF_YES);
    } else {
      return (DEF_NO);
    }
  }

  //                                                               -------------- CARD IF CARD CONNECTED --------------
  p_sd_card->RCA = 0u;                                          // Reset relative card addr and bus width.
  p_sd_card->BusWidth = 1u;

  //                                                               Set clk to dflt clk.
  p_bsp_api->ClkFreqSet((CPU_INT32U)FS_SD_DFLT_CLK_SPD);
  p_bsp_api->TimeoutDataSet(0xFFFFFFFFuL);
  p_bsp_api->TimeoutRespSet(1000u);
  p_bsp_api->BusWidthSet(1u);

  p_sd_card->BspPtr->Lock();                                    // Acquire bus lock.
  retries = 0u;                                                 // Send CMD0 to reset the bus.
  while (retries < FS_DEV_SD_CARD_RETRIES_CMD) {
    RTOS_ERR_SET(err_tmp, RTOS_ERR_NONE);
    FS_SD_Card_Cmd(p_sd_card,
                   FS_DEV_SD_CMD_GO_IDLE_STATE,
                   0u,
                   &err_tmp);
    if (RTOS_ERR_CODE_GET(err_tmp) == RTOS_ERR_NONE) {
      break;
    }
    KAL_Dly(2u);
    retries++;
  }

  is_conn = DEF_NO;
  if (retries < FS_DEV_SD_CARD_RETRIES_CMD) {                   // If bus reset succeeds, chk if card is connected.
    RTOS_ERR_SET(err_tmp, RTOS_ERR_NONE);
    (void)FS_SD_Card_MakeRdy(p_sd_card, &err_tmp);
    if (RTOS_ERR_CODE_GET(err_tmp) == RTOS_ERR_NONE) {
      is_conn = DEF_YES;
    }
  }
  p_sd_card->BspPtr->Unlock();                                  // Release bus lock.

  if (is_closed) {
    p_bsp_api->Close();
  }

  return (is_conn);
}

/****************************************************************************************************//**
 *                                           FS_SD_Card_AlignReqGet()
 *
 * @brief    Get buffer alignment requirement of SDIO controller.
 *
 * @param    p_media     Pointer to media.
 *
 * @param    p_err       Error pointer.
 *
 * @return   Buffer alignment in bytes.
 *******************************************************************************************************/
static CPU_SIZE_T FS_SD_Card_AlignReqGet(FS_MEDIA *p_media,
                                         RTOS_ERR *p_err)
{
  FS_SD_CARD         *p_sd_card;
  FS_SD_CARD_PM_ITEM *p_sd_card_pm_item;

  PP_UNUSED_PARAM(p_err);

  p_sd_card = (FS_SD_CARD *)p_media;
  p_sd_card_pm_item = (FS_SD_CARD_PM_ITEM *)p_sd_card->SD.Media.PmItemPtr;

  return (p_sd_card_pm_item->HwInfoPtr->AlignReq);
}

/****************************************************************************************************//**
 *                                               FS_SD_Card_Rd()
 *
 * @brief    Read from a device & store data in buffer.
 *
 * @param    p_blk_dev   Pointer to a SD card block device.
 *
 * @param    p_dest      Pointer to destination buffer.
 *
 * @param    start       Start sector of read.
 *
 * @param    cnt         Number of sectors to read.
 *
 * @param    p_err       Error pointer.
 *
 * @note     (1) Standard-capacity devices receive the start byte address as the argument of the read
 *               command, limiting device access to 4-GB (the range of a 32-bit variable).  To solve
 *               that problem, high-capacity devices (like SDHC cards) receive the block number as the
 *               argument of the read command.
 *
 * @note     (2) A multiple block transfer will be performed if more than one sector is to be read and
 *               the maximum block count, returned by p_bsp->GetMaxBlkCnt(),  is greater
 *               than 1.  If the controller is not capable of multiple block reads or writes, that
 *               function should return 1.
 *
 * @note     (3) The card is always re-selected after the lock is acquired, in case multiple cards
 *               are located on the same bus.  Since only one card may be in use (or selected) at any
 *               time, the lock can be held by only one device.
 *******************************************************************************************************/
static void FS_SD_Card_Rd(FS_BLK_DEV *p_blk_dev,
                          void       *p_dest,
                          FS_LB_NBR  start,
                          FS_LB_QTY  cnt,
                          RTOS_ERR   *p_err)
{
  CPU_INT08U *p_dest_08;
  FS_SD_INFO *p_sd_info;
  FS_SD_CARD *p_sd_card;
  CPU_INT32U blk_cnt;
  CPU_INT08U cmd;
  CPU_INT32U addr_inc;
  CPU_INT32U start_addr;
  CPU_INT32U retries;
  RTOS_ERR   err_tmp;

  WITH_SCOPE_BEGIN(p_err) {
    p_sd_card = (FS_SD_CARD *)p_blk_dev->MediaPtr;
    p_dest_08 = (CPU_INT08U *)p_dest;

    p_sd_info = &p_sd_card->SD.Info;

    //                                                             See Note #2.
    start_addr = (p_sd_info->HighCapacity == DEF_YES) ? start : (start * FS_DEV_SD_BLK_SIZE);
    addr_inc = (p_sd_info->HighCapacity == DEF_YES) ? 1u    : FS_DEV_SD_BLK_SIZE;
    retries = FS_DEV_SD_CARD_RETRIES_DATA;

    p_sd_card->BspPtr->Lock();                                  // Acquire bus lock.
    BREAK_ON_ERR(FS_SD_Card_Sel(p_sd_card,                      // Sel card (see Note #3).
                                DEF_YES,
                                p_err));

    BREAK_ON_ERR(while) (cnt > 0u) {
      //                                                           -------------- WAIT UNTIL CARD IS RDY --------------
      BREAK_ON_ERR(FS_SD_Card_WaitWhileBusy(p_sd_card, p_err));

      //                                                           --------------- PERFORM MULTIPLE RD ----------------
      blk_cnt = DEF_MIN(p_sd_card->BlkCntMax, cnt);             // See Note #3.
      cmd = (blk_cnt > 1u) ? FS_DEV_SD_CMD_READ_MULTIPLE_BLOCK : FS_DEV_SD_CMD_READ_SINGLE_BLOCK;

      RTOS_ERR_SET(err_tmp, RTOS_ERR_NONE);
      FS_SD_Card_DataRd(p_sd_card,
                        cmd,
                        start_addr,
                        p_dest_08,
                        FS_DEV_SD_BLK_SIZE,
                        blk_cnt,
                        &err_tmp);

      if (RTOS_ERR_CODE_GET(err_tmp) != RTOS_ERR_NONE) {
        ASSERT_BREAK(retries > 0u, RTOS_ERR_CODE_GET(err_tmp));
        LOG_DBG(("Failed to rd card ... retrying."));
        retries--;
      } else {
        if (blk_cnt > 1u) {                                     // ---------------- STOP TRANSMISSION -----------------
          BREAK_ON_ERR(FS_SD_Card_Stop(p_sd_card, p_err));
        }

        p_dest_08 += blk_cnt * FS_DEV_SD_BLK_SIZE;
        start_addr += blk_cnt * addr_inc;
        cnt -= blk_cnt;

        retries = FS_DEV_SD_CARD_RETRIES_DATA;
      }
    }
  } WITH_SCOPE_END

  p_sd_card->BspPtr->Unlock();                                  // Release bus lock.

  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    FS_DEV_SD_CARD_ERR_RD_CTR_INC(p_sd_card);
  }
}

/****************************************************************************************************//**
 *                                               FS_SD_Card_Wr()
 *
 * @brief    Write blocks to an SD card.
 *
 * @param    p_blk_dev   Pointer to a SD card block device.
 *
 * @param    p_src       Pointer to a source buffer to write from.
 *
 * @param    start       Start logical block number.
 *
 * @param    cnt         Number of sectors to write.
 *
 * @param    p_err       Error pointer.
 *
 * @note     (1) Standard-capacity devices receive the start byte address as the argument of the write
 *               command, limiting device access to 4-GB (the range of a 32-bit variable).  To solve
 *               that problem, high-capacity devices (like SDHC cards) receive the block number as the
 *               argument of the write command.
 *
 * @note     (2) A multiple block transfer will be performed if more than one sector is to be read and
 *               the maximum block count, returned by p_bsp->GetMaxBlkCnt(),  is greater
 *               than 1.  If the controller is not capable of multiple block reads or writes, that
 *               function should return 1.
 *
 * @note     (3) The card is always re-selected after the lock is acquired, in case multiple cards
 *               are located on the same bus.  Since only one card may be in use (or selected) at any
 *               time, the lock can be held by only one device.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_SD_Card_Wr(FS_BLK_DEV *p_blk_dev,
                          void       *p_src,
                          FS_LB_NBR  start,
                          FS_LB_QTY  cnt,
                          RTOS_ERR   *p_err)
{
  CPU_INT08U *p_src_08;
  FS_SD_INFO *p_sd_info;
  FS_SD_CARD *p_sd_card;
  CPU_INT32U blk_cnt;
  CPU_INT08U cmd;
  CPU_INT32U addr_inc;
  CPU_INT32U start_addr;
  CPU_INT32U retries;
  RTOS_ERR   err_tmp;

  WITH_SCOPE_BEGIN(p_err) {
    p_sd_card = (FS_SD_CARD *)p_blk_dev->MediaPtr;
    p_src_08 = (CPU_INT08U *)p_src;
    p_sd_info = &p_sd_card->SD.Info;
    //                                                             See Note #2.
    start_addr = (p_sd_info->HighCapacity == DEF_YES) ? start : (start * FS_DEV_SD_BLK_SIZE);
    addr_inc = (p_sd_info->HighCapacity == DEF_YES) ? 1u    : FS_DEV_SD_BLK_SIZE;
    retries = FS_DEV_SD_CARD_RETRIES_DATA;

    p_sd_card->BspPtr->Lock();                                  // Acquire bus lock.
    BREAK_ON_ERR(FS_SD_Card_Sel(p_sd_card, DEF_YES, p_err));    // Sel card (see Note #3).

    BREAK_ON_ERR(while) (cnt > 0u) {
      //                                                           -------------- WAIT UNTIL CARD IS RDY --------------
      BREAK_ON_ERR(FS_SD_Card_WaitWhileBusy(p_sd_card, p_err));

      //                                                           --------------- PERFORM MULTIPLE WR ----------------
      blk_cnt = DEF_MIN(p_sd_card->BlkCntMax, cnt);             // See Note #3.
      cmd = (blk_cnt > 1u) ? FS_DEV_SD_CMD_WRITE_MULTIPLE_BLOCK : FS_DEV_SD_CMD_WRITE_BLOCK;

      RTOS_ERR_SET(err_tmp, RTOS_ERR_NONE);
      FS_SD_Card_DataWr(p_sd_card,
                        cmd,
                        start_addr,
                        p_src_08,
                        FS_DEV_SD_BLK_SIZE,
                        blk_cnt,
                        &err_tmp);

      if (RTOS_ERR_CODE_GET(err_tmp) != RTOS_ERR_NONE) {
        ASSERT_BREAK(retries > 0u, RTOS_ERR_IO);
        LOG_DBG(("Failed to wr card ... retrying."));
        retries--;
      } else {
        if (blk_cnt > 1u) {                                     // ---------------- STOP TRANSMISSION -----------------
          BREAK_ON_ERR(FS_SD_Card_Stop(p_sd_card, p_err));
        }

        p_src_08 += blk_cnt * FS_DEV_SD_BLK_SIZE;
        start_addr += blk_cnt * addr_inc;
        cnt -= blk_cnt;

        retries = FS_DEV_SD_CARD_RETRIES_DATA;
      }
    }
  } WITH_SCOPE_END

  p_sd_card->BspPtr->Unlock();                                  // Release bus lock.

  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    FS_DEV_SD_CARD_ERR_WR_CTR_INC(p_sd_card);
  }
}
#endif

/****************************************************************************************************//**
 *                                               FS_SD_Card_Sync()
 *
 * @brief    Sync SD card.
 *
 * @param    p_blk_dev   Pointer to a SD card block device.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_SD_Card_Sync(FS_BLK_DEV *p_blk_dev,
                            RTOS_ERR   *p_err)
{
  PP_UNUSED_PARAM(p_blk_dev);
  PP_UNUSED_PARAM(p_err);
}
#endif

/****************************************************************************************************//**
 *                                               FS_SD_Card_Trim()
 *
 * @brief    Trim SD card.
 *
 * @param    p_blk_dev   Pointer to a SD card block device.
 *
 * @param    lb_nbr      Logical block number.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_SD_Card_Trim(FS_BLK_DEV *p_blk_dev,
                            FS_LB_NBR  lb_nbr,
                            RTOS_ERR   *p_err)
{
  PP_UNUSED_PARAM(p_blk_dev);
  PP_UNUSED_PARAM(lb_nbr);
  PP_UNUSED_PARAM(p_err);
}
#endif

/****************************************************************************************************//**
 *                                           FS_SD_Card_Query()
 *
 * @brief    Get information about a device.
 *
 * @param    p_blk_dev   Pointer to a SD card block device.
 *
 * @param    p_info      Pointer to structure that will receive device information.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_SD_Card_Query(FS_BLK_DEV      *p_blk_dev,
                             FS_BLK_DEV_INFO *p_info,
                             RTOS_ERR        *p_err)
{
  PP_UNUSED_PARAM(p_err);

  FS_SD_CARD *p_sd_card = (FS_SD_CARD *)p_blk_dev->MediaPtr;

  p_info->LbSizeLog2 = FS_DEV_SD_BLK_SIZE_LOG2;
  p_info->LbCnt = p_sd_card->SD.Info.NbrBlks * (p_sd_card->SD.Info.BlkSize >> FS_DEV_SD_BLK_SIZE_LOG2);
  p_info->Fixed = DEF_NO;
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           FS_SD_Card_MakeRdy()
 *
 * @brief    Move card into 'ready' state (& get card type).
 *
 * @param    p_sd_card   Pointer to a SD card.
 *
 * @param    p_err       Error pointer.
 *
 * @return   Card type :
 *               - FS_DEV_SD_SPI_CARDTYPE_NONE          Card type could not be determined.
 *               - FS_DEV_SD_SPI_CARDTYPE_SD_V1_X       SD card v1.x.
 *               - FS_DEV_SD_SPI_CARDTYPE_SD_V2_0       SD card v2.0, standard capacity.
 *               - FS_DEV_SD_SPI_CARDTYPE_SD_V2_0_HC    SD card v2.0, high capacity.
 *               - FS_DEV_SD_SPI_CARDTYPE_MMC           MMC, standard capacity.
 *               - FS_DEV_SD_SPI_CARDTYPE_MMC_HC        MMC, high capacity.
 *
 * @note     (1) Error is not check in this case. The SD/MMC specification allowed multiple retries
 *               on the same register. The connected card can answer to a request at any moment, so
 *               you must not check the error code.
 *
 * @note     (2) A SD card responds to ACMD55 with R3, the OCR register.  The power up status bit,
 *               bit 31, will be set when the card has finished its power up routine.
 *               A MMC card responds to CMD1  with R3, the OCR register.  The power up status bit,
 *               bit 31, will be set when the card has finished its power up routine.
 *******************************************************************************************************/
static CPU_INT08U FS_SD_Card_MakeRdy(FS_SD_CARD *p_sd_card,
                                     RTOS_ERR   *p_err)
{
  CPU_INT08U  card_type = FS_DEV_SD_CARDTYPE_NONE;
  CPU_INT32U  resp_short;
  CPU_INT16U  retries;
  CPU_BOOLEAN responded;
  RTOS_ERR    err_tmp;

  WITH_SCOPE_BEGIN(p_err) {
    //                                                             ------------- SEND CMD8 TO GET CARD OP -------------
    resp_short = 0u;
    RTOS_ERR_SET(err_tmp, RTOS_ERR_NONE);
    FS_SD_Card_CmdRShort(p_sd_card,                             // Perform CMD8 (R7 resp).
                         FS_DEV_SD_CMD_SEND_IF_COND,
                         (FS_DEV_SD_CMD8_VHS_27_36_V | FS_DEV_SD_CMD8_CHK_PATTERN),
                         &resp_short,
                         &err_tmp);

    if (RTOS_ERR_CODE_GET(err_tmp) == RTOS_ERR_NONE) {
      if ((resp_short & DEF_OCTET_MASK) == FS_DEV_SD_CMD8_CHK_PATTERN) {
        card_type = FS_DEV_SD_CARDTYPE_SD_V2_0;
      }
    } else if (RTOS_ERR_CODE_GET(err_tmp) == RTOS_ERR_IO) {
      resp_short = 0u;
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

      FS_SD_Card_AppCmdRShort(p_sd_card,                        // Perform ACMD41 (R3 resp).
                              FS_DEV_SD_ACMD_SD_SEND_OP_COND,
                              0u,
                              &resp_short,
                              p_err);
      if ((resp_short != 0u)
          && (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE)) {
        retries = 0u;
        responded = DEF_NO;
        while ((retries < FS_DEV_SD_CARD_RETRIES_CMD) && !responded) {
          resp_short = 0u;
          RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

          FS_SD_Card_AppCmdRShort(p_sd_card,                    // Perform ACMD41 (R3 resp).
                                  FS_DEV_SD_ACMD_SD_SEND_OP_COND,
                                  (FS_DEV_SD_ACMD41_HCS | FS_DEV_SD_OCR_VOLTAGE_MASK),
                                  &resp_short,
                                  p_err);                       // Chk error (see Note #1).
          if (DEF_BIT_IS_SET(resp_short, FS_DEV_SD_OCR_BUSY)) {
            card_type = FS_DEV_SD_CARDTYPE_SD_V1_X;
            responded = DEF_YES;
          }

          if (responded == DEF_NO) {
            KAL_Dly(2u);
          }

          retries++;
        }

        if (retries == FS_DEV_SD_CARD_RETRIES_CMD) {
          card_type = FS_DEV_SD_CARDTYPE_NONE;
        }
      } else if ((resp_short == 0u)
                 && (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_IO)) {
        card_type = FS_DEV_SD_CARDTYPE_MMC;
      }
    }

    switch (card_type) {
      case FS_DEV_SD_CARDTYPE_MMC:

        //                                                         ------------- SEND CMD1 (FOR MMC CARD) -------------
        retries = 0u;
        responded = DEF_NO;
        while ((retries < FS_DEV_SD_CARD_RETRIES_CMD) && !responded) {
          resp_short = 0u;
          RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
          FS_SD_Card_CmdRShort(p_sd_card,                       // Perform CMD1 (R3 resp).
                               FS_DEV_SD_CMD_SEND_OP_COND,
                               (FS_DEV_SD_OCR_ACCESS_MODE_SEC | FS_DEV_SD_OCR_VOLTAGE_MASK),
                               &resp_short,
                               p_err);                          // Chk error (see Note #1).

          //                                                       Check card power up status bit.
          if ((DEF_BIT_IS_SET(resp_short, FS_DEV_SD_OCR_BUSY) == DEF_YES)
              && (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_IO)) {
            RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
            responded = DEF_YES;
          }

          if (responded == DEF_NO) {
            KAL_Dly(2u);                                        // If card has not finished the power up, wait 2 ms...
                                                                // ...before retrying cmd.
          }
          retries++;
        }

        if ((retries == FS_DEV_SD_CARD_RETRIES_CMD)) {
          card_type = FS_DEV_SD_CARDTYPE_NONE;
        }

        break;

      case FS_DEV_SD_CARDTYPE_SD_V2_0:

        //                                                         ---------- SEND ACMD41 TO GET CARD STATUS ----------
        retries = 0u;
        responded = DEF_NO;
        while ((retries < FS_DEV_SD_CARD_RETRIES_CMD) && !responded) {
          resp_short = 0u;
          RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
          FS_SD_Card_AppCmdRShort(p_sd_card,                    // Perform ACMD41 (R3 resp).
                                  FS_DEV_SD_ACMD_SD_SEND_OP_COND,
                                  (FS_DEV_SD_ACMD41_HCS | FS_DEV_SD_OCR_VOLTAGE_MASK),
                                  &resp_short,
                                  p_err);

          if (DEF_BIT_IS_SET(resp_short, FS_DEV_SD_OCR_BUSY)) {
            responded = DEF_YES;
            RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
          }

          if (responded == DEF_NO) {
            KAL_Dly(2u);
          }

          retries++;
        }

        break;

      case FS_DEV_SD_CARDTYPE_SD_V1_X:
      case FS_DEV_SD_CARDTYPE_NONE:
      default:
        break;
    }
    //                                                             -------------------- CHECK CCS ---------------------
    //                                                             Check for High-Capacity card.
    if (DEF_BIT_IS_SET(resp_short, FS_DEV_SD_OCR_CCS)) {
      if (card_type == FS_DEV_SD_CARDTYPE_SD_V2_0) {
        card_type = FS_DEV_SD_CARDTYPE_SD_V2_0_HC;
      } else {
        if (card_type == FS_DEV_SD_CARDTYPE_MMC) {
          card_type = FS_DEV_SD_CARDTYPE_MMC_HC;
        }
      }
    }
  } WITH_SCOPE_END

  return (card_type);
}

/****************************************************************************************************//**
 *                                               FS_SD_Card_Cmd()
 *
 * @brief    Send command requiring no response to device.
 *
 * @param    p_sd_card   Pointer to SD card.
 *
 * @param    cmd_nbr     Control command index.
 *
 * @param    arg         Command argument.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_SD_Card_Cmd(FS_SD_CARD *p_sd_card,
                           CPU_INT08U cmd_nbr,
                           CPU_INT32U arg,
                           RTOS_ERR   *p_err)
{
  FS_SD_CARD_BSP_API *p_bsp;
  FS_DEV_SD_CARD_CMD cmd;
  FS_DEV_SD_CARD_ERR err;

  p_bsp = (FS_SD_CARD_BSP_API *)p_sd_card->BspPtr;

  FS_SD_Card_CmdInit(&cmd,                                      // Init cmd info.
                     cmd_nbr,
                     arg,
                     p_sd_card->SD.Info.CardType);

  p_bsp->CmdStart(&cmd,                                         // Start cmd exec.
                  0u,
                  &err);
  if (err != FS_DEV_SD_CARD_ERR_NONE) {
    FS_DEV_SD_CARD_HANDLE_ERR_BSP(p_sd_card, err);
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return;
  }

  p_bsp->CmdEndWait(&cmd,                                       // Wait for cmd exec end.
                    DEF_NULL,
                    &err);
  if (err != FS_DEV_SD_CARD_ERR_NONE) {
    FS_DEV_SD_CARD_HANDLE_ERR_BSP(p_sd_card, err);
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return;
  }
}

/****************************************************************************************************//**
 *                                           FS_SD_SPI_CmdRShort()
 *
 * @brief    Send command with R1, R1b, R3, R4, R5, R5b, R6 or R7 response to device.
 *
 * @param    p_sd_card   Pointer to a SD card.
 *
 * @param    cmd_nbr     Command index.
 *
 * @param    arg         Command argument.
 *
 * @param    p_resp      Pointer to buffer that will receive 4-byte command response.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_SD_Card_CmdRShort(FS_SD_CARD *p_sd_card,
                                 CPU_INT08U cmd_nbr,
                                 CPU_INT32U arg,
                                 CPU_INT32U *p_resp,
                                 RTOS_ERR   *p_err)
{
  FS_SD_CARD_BSP_API *p_bsp;
  FS_DEV_SD_CARD_CMD cmd;
  FS_DEV_SD_CARD_ERR err;

  err = FS_DEV_SD_CARD_ERR_NONE;
  p_bsp = (FS_SD_CARD_BSP_API *)p_sd_card->BspPtr;

  FS_SD_Card_CmdInit(&cmd,                                      // Init cmd info.
                     cmd_nbr,
                     arg,
                     p_sd_card->SD.Info.CardType);
  //                                                               Start cmd exec.
  p_bsp->CmdStart(&cmd,
                  DEF_NULL,
                  &err);
  if (err != FS_DEV_SD_CARD_ERR_NONE) {
    FS_DEV_SD_CARD_HANDLE_ERR_BSP(p_sd_card, err);
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return;
  }
  //                                                               Wait for cmd exec end & rd 4-byte resp.
  p_bsp->CmdEndWait(&cmd,
                    p_resp,
                    &err);
  if (err != FS_DEV_SD_CARD_ERR_NONE) {
    FS_DEV_SD_CARD_HANDLE_ERR_BSP(p_sd_card, err);
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return;
  }
}

/****************************************************************************************************//**
 *                                           FS_SD_Card_CmdRLong()
 *
 * @brief    Perform command with R2 response.
 *
 * @param    p_sd_card   Pointer to SD card.
 *
 * @param    cmd_nbr     Command index.
 *
 * @param    arg         Command argument.
 *
 * @param    p_resp      Pointer to buffer that will receive 16-byte command response.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_SD_Card_CmdRLong(FS_SD_CARD *p_sd_card,
                                CPU_INT08U cmd_nbr,
                                CPU_INT32U arg,
                                CPU_INT32U *p_resp,
                                RTOS_ERR   *p_err)
{
  FS_SD_CARD_BSP_API *p_bsp;
  FS_DEV_SD_CARD_CMD cmd;
  FS_DEV_SD_CARD_ERR err;

  err = FS_DEV_SD_CARD_ERR_NONE;

  p_bsp = (FS_SD_CARD_BSP_API *)p_sd_card->BspPtr;

  FS_SD_Card_CmdInit(&cmd,                                      // Init cmd info.
                     cmd_nbr,
                     arg,
                     p_sd_card->SD.Info.CardType);
  //                                                               Start cmd exec.
  p_bsp->CmdStart(&cmd,
                  DEF_NULL,
                  &err);
  if (err != FS_DEV_SD_CARD_ERR_NONE) {
    FS_DEV_SD_CARD_HANDLE_ERR_BSP(p_sd_card, err);
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return;
  }
  //                                                               Wait for cmd exec end & rd 16-byte resp.
  p_bsp->CmdEndWait(&cmd,
                    p_resp,
                    &err);
  if (err != FS_DEV_SD_CARD_ERR_NONE) {
    FS_DEV_SD_CARD_HANDLE_ERR_BSP(p_sd_card, err);
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return;
  }
}

/****************************************************************************************************//**
 *                                       FS_SD_Card_AppCmdRShort()
 *
 * @brief    Perform app cmd with R1, R1b, R3, R4, R5, R5b, R6 or R7 response.
 *
 * @param    p_sd_card   Pointer to SD card.
 *
 * @param    acmd_nbr    Application-specific command index.
 *
 * @param    arg         Command argument.
 *
 * @param    p_resp      Pointer to buffer that will receive 4-byte command response.
 *
 * @param    p_err       Error pointer.
 *
 * @note     (1) CMD55, which indicates to the card that the following command will be an app command,
 *               should receive a R1 response.  The correct R1 response should have the APP_CMD bit
 *               (bit 5) set.
 *******************************************************************************************************/
static void FS_SD_Card_AppCmdRShort(FS_SD_CARD *p_sd_card,
                                    CPU_INT08U acmd_nbr,
                                    CPU_INT32U arg,
                                    CPU_INT32U *p_resp,
                                    RTOS_ERR   *p_err)
{
  CPU_INT32U cmd55_resp;

  WITH_SCOPE_BEGIN(p_err) {
    BREAK_ON_ERR(FS_SD_Card_CmdRShort(p_sd_card,                // Perform CMD55 (R1 resp).
                                      FS_DEV_SD_CMD_APP_CMD,
                                      p_sd_card->RCA,
                                      &cmd55_resp,
                                      p_err));

    //                                                             Chk resp (see Note #1).
    ASSERT_BREAK(DEF_BIT_IS_SET(cmd55_resp, FS_DEV_SD_CARD_R1_APP_CMD),
                 RTOS_ERR_IO);

    BREAK_ON_ERR(FS_SD_Card_CmdRShort(p_sd_card,                // Perform ACMD.
                                      acmd_nbr + 64u,
                                      arg,
                                      p_resp,
                                      p_err));
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                           FS_SD_Card_DataRd()
 *
 * @brief    Execute command & read data blocks from card.
 *
 * @param    p_sd_card   Pointer to SD card.
 *
 * @param    cmd_nbr     Command index.
 *
 * @param    arg         Command argument.
 *
 * @param    p_dest      Pointer to destination buffer.
 *
 * @param    size        Size of each data block to be read, in octets.
 *
 * @param    cnt         Number of data blocks to be read.
 *
 * @param    p_err       Error pointer.
 *
 * @note     (1) The response from the command MUST be a short response.
 *******************************************************************************************************/
static void FS_SD_Card_DataRd(FS_SD_CARD *p_sd_card,
                              CPU_INT08U cmd_nbr,
                              CPU_INT32U arg,
                              CPU_INT08U *p_dest,
                              CPU_INT32U size,
                              CPU_INT32U cnt,
                              RTOS_ERR   *p_err)
{
  FS_SD_CARD_BSP_API *p_bsp;
  FS_DEV_SD_CARD_CMD cmd;
  FS_DEV_SD_CARD_ERR err;
  CPU_INT32U         resp;

  err = FS_DEV_SD_CARD_ERR_NONE;
  p_bsp = (FS_SD_CARD_BSP_API *)p_sd_card->BspPtr;

  FS_SD_Card_CmdInit(&cmd,                                      // Init cmd info.
                     cmd_nbr,
                     arg,
                     p_sd_card->SD.Info.CardType);

  cmd.BlkSize = size;
  cmd.BlkCnt = cnt;
  //                                                               Start cmd exec.
  p_bsp->CmdStart(&cmd,
                  p_dest,
                  &err);
  if (err != FS_DEV_SD_CARD_ERR_NONE) {
    FS_DEV_SD_CARD_HANDLE_ERR_BSP(p_sd_card, err);
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return;
  }
  //                                                               Wait for cmd exec end & rd 4-byte resp.
  p_bsp->CmdEndWait(&cmd,
                    &resp,
                    &err);
  if (err != FS_DEV_SD_CARD_ERR_NONE) {
    FS_DEV_SD_CARD_HANDLE_ERR_BSP(p_sd_card, err);
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return;
  }
  //                                                               Rd data.
  p_bsp->CmdDataRd(&cmd,
                   p_dest,
                   &err);
  if (err != FS_DEV_SD_CARD_ERR_NONE) {
    FS_DEV_SD_CARD_HANDLE_ERR_BSP(p_sd_card, err);
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return;
  }
}

/****************************************************************************************************//**
 *                                           FS_SD_Card_DataWr()
 *
 * @brief    Execute command & write data block to card.
 *
 * @param    p_sd_card   Pointer to a SD card.
 *
 * @param    cmd_nbr     Command index.
 *
 * @param    arg         Command argument.
 *
 * @param    p_src       Pointer to source buffer.
 *
 * @param    size        Size of each data block to be written, in octets.
 *
 * @param    cnt         Number of data blocks to be written.
 *
 * @param    p_err       Error pointer.
 *
 * @note     (1) The response from the command MUST be a short response.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_SD_Card_DataWr(FS_SD_CARD *p_sd_card,
                              CPU_INT08U cmd_nbr,
                              CPU_INT32U arg,
                              CPU_INT08U *p_src,
                              CPU_INT32U size,
                              CPU_INT32U cnt,
                              RTOS_ERR   *p_err)
{
  FS_SD_CARD_BSP_API *p_bsp;
  FS_DEV_SD_CARD_CMD cmd;
  FS_DEV_SD_CARD_ERR err;
  CPU_INT32U         resp;

  p_bsp = (FS_SD_CARD_BSP_API *)p_sd_card->BspPtr;

  FS_SD_Card_CmdInit(&cmd,                                      // Init cmd info.
                     cmd_nbr,
                     arg,
                     p_sd_card->SD.Info.CardType);

  cmd.BlkSize = size;
  cmd.BlkCnt = cnt;
  //                                                               Start cmd exec.
  p_bsp->CmdStart(&cmd,
                  (void *)p_src,
                  &err);
  if (err != FS_DEV_SD_CARD_ERR_NONE) {
    FS_DEV_SD_CARD_HANDLE_ERR_BSP(p_sd_card, err);
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return;
  }
  //                                                               Wait for cmd exec end & rd 4-byte resp.
  p_bsp->CmdEndWait(&cmd,
                    &resp,
                    &err);
  if (err != FS_DEV_SD_CARD_ERR_NONE) {
    FS_DEV_SD_CARD_HANDLE_ERR_BSP(p_sd_card, err);
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return;
  }
  //                                                               Wr data.
  p_bsp->CmdDataWr(&cmd,
                   (void *)p_src,
                   &err);
  if (err != FS_DEV_SD_CARD_ERR_NONE) {
    FS_DEV_SD_CARD_HANDLE_ERR_BSP(p_sd_card, err);
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return;
  }
}
#endif

/****************************************************************************************************//**
 *                                           FS_SD_Card_CID_Send()
 *
 * @brief    Get Card ID (CID) register from card.
 *
 * @param    p_sd_card   Pointer to a SD card.
 *
 * @param    p_dest      Pointer to 16-byte buffer that will receive SD/MMC Card ID register.
 *
 * @param    p_err       Error pointer.
 *
 * @note     (1) For SD cards, the structure of the CID is defined in [Ref 1] Section 5.1.
 *               For MMC cards, the structure of the CID is defined in [Ref 2] Section 8.2.
 *
 * @note     (2) See Note #1 of function FS_SD_Card_AddInternal() for more details.
 *
 * @note     (3) ALL_SEND_CID is performed during startup to activate the card that will later be
 *               addressed.  Once     the card is addressed (whereupon the RCA is assigned/obtained),
 *               SEND_CID should be performed instead.
 *               - (a) SEND_CID can only be performed when the card is in the 'stby' state.  The card
 *                     is deselected before this command (thereby moving the card to the 'stby' state)
 *                     & re-selected after the command (moving the card back to the 'tran' state).
 *
 * @note     (4) On little-endian processors, the long card response is read in 32-bit words.  The
 *               byte-order of the words MUST be reversed to obtain the bit-order shown in [Ref 1],
 *               Table 5-2, or [Ref 2], Table 30.
 *               - (a) The highest bit of the first byte is bit 127 of the CID as defined in [Ref 1],
 *                     Table 5-2, or [Ref 2], Table 30.
 *******************************************************************************************************/
static void FS_SD_Card_CID_Send(FS_SD_CARD *p_sd_card,
                                CPU_INT08U *p_dest,
                                RTOS_ERR   *p_err)
{
  CPU_INT32U *p_resp;

  WITH_SCOPE_BEGIN(p_err) {
    p_resp = (CPU_INT32U *)FS_SD_Card_Data.RespPtr;             // See Note #2.

    if (p_sd_card->RCA == 0u) {                                 // Chk if addr'd (see Note #3).
      BREAK_ON_ERR(FS_SD_Card_CmdRLong(p_sd_card,               // Perform CMD2 (R2 resp).
                                       FS_DEV_SD_CMD_ALL_SEND_CID,
                                       0u,
                                       p_resp,
                                       p_err));
    } else {
      BREAK_ON_ERR(FS_SD_Card_Sel(p_sd_card,                    // Desel card (see Note #3a).
                                  DEF_NO,
                                  p_err));

      BREAK_ON_ERR(FS_SD_Card_CmdRLong(p_sd_card,               // Perform CMD10 (R2 resp).
                                       FS_DEV_SD_CMD_SEND_CID,
                                       p_sd_card->RCA,
                                       p_resp,
                                       p_err));
    }

    //                                                             Copy CID into buf (see Note #4).
    MEM_VAL_COPY_GET_INT32U_BIG((void *)&p_dest[0], (void *)&p_resp[0]);
    MEM_VAL_COPY_GET_INT32U_BIG((void *)&p_dest[4], (void *)&p_resp[1]);
    MEM_VAL_COPY_GET_INT32U_BIG((void *)&p_dest[8], (void *)&p_resp[2]);
    MEM_VAL_COPY_GET_INT32U_BIG((void *)&p_dest[12], (void *)&p_resp[3]);
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                           FS_SD_Card_CSD_Send()
 *
 * @brief    Get Card-Specific Data (CSD) register from card.
 *
 * @param    p_sd_card   Pointer to a SD card.
 *
 * @param    p_dest      Pointer to 16-byte buffer that will receive SD/MMC Card-Specific Data register.
 *
 * @param    p_err       Error pointer.
 *
 * @note     (1) For SD v1.x & SD v2.0 standard capacity, the structure of the CSD is defined in
 *               [Ref 1], Section 5.3.2.
 *               For MMC cards, the structure of the CSD is defined in [Ref 2], Section 8.3.
 *               For SD v2.0 high capacity cards, the structure of the CSD is defined in [Ref 1],
 *               Section 5.3.3.
 *
 * @note     (2) See Note #1 of function FS_SD_Card_AddInternal() for more details.
 *
 * @note     (2) SEND_CSD can only be performed when the card is in the 'stby' state.  The card
 *               is deselected before this command (thereby moving the card to the 'stby' state)
 *               & re-selected after the command (moving the card back to the 'tran' state).
 *
 * @note     (3) On little-endian processors, the long card response is read in 32-bit words.  The
 *               byte-order of the words MUST be reversed to obtain the bit-order shown in [Ref 1],
 *               Table 5-4 or Table 5-16, or [Ref 2], Table 31.
 *               - (a) The highest bit of the first byte is bit 127 of the CSD as defined in [Ref 1],
 *                     Table 5-4 or Table 5-16, or [Ref 2], Table 31.
 *******************************************************************************************************/
static void FS_SD_Card_CSD_Send(FS_SD_CARD *p_sd_card,
                                CPU_INT08U *p_dest,
                                RTOS_ERR   *p_err)
{
  CPU_INT32U *p_resp;

  WITH_SCOPE_BEGIN(p_err) {
    p_resp = (CPU_INT32U *)FS_SD_Card_Data.RespPtr;             // See Note #2.

    BREAK_ON_ERR(FS_SD_Card_Sel(p_sd_card,                      // Desel card (see Note #3).
                                DEF_NO,
                                p_err));

    BREAK_ON_ERR(FS_SD_Card_CmdRLong(p_sd_card,                 // Perform CMD9 (R2 resp).
                                     FS_DEV_SD_CMD_SEND_CSD,
                                     p_sd_card->RCA,
                                     p_resp,
                                     p_err));

    //                                                             Copy CSD into buf (see Note #4).
    MEM_VAL_COPY_GET_INT32U_BIG((void *)&p_dest[0], (void *)&p_resp[0]);
    MEM_VAL_COPY_GET_INT32U_BIG((void *)&p_dest[4], (void *)&p_resp[1]);
    MEM_VAL_COPY_GET_INT32U_BIG((void *)&p_dest[8], (void *)&p_resp[2]);
    MEM_VAL_COPY_GET_INT32U_BIG((void *)&p_dest[12], (void *)&p_resp[3]);
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                       FS_SD_Card_EXT_CSD_Send()
 *
 * @brief   Get Extended Card-Specific Data (EXT_CSD) register from card.
 *
 * @param   p_sd_card       Pointer to SD card.
 *
 * @param   p_dest          Pointer to 512-byte buffer that will receive MMC Extended Card-Specific
 *                          Data register.
 *
 * @param   p_err           Error pointer.
 *
 * @note     (1) Only valid for High Capacity MMC cards.
 *******************************************************************************************************/
static void FS_SD_Card_EXT_CSD_Send(FS_SD_CARD *p_sd_card,
                                    CPU_INT08U *p_dest,
                                    RTOS_ERR   *p_err)
{
  WITH_SCOPE_BEGIN(p_err) {
    BREAK_ON_ERR(FS_SD_Card_Sel(p_sd_card,
                                DEF_YES,
                                p_err));

    BREAK_ON_ERR(FS_SD_Card_DataRd(p_sd_card,
                                   FS_DEV_MMC_CMD_SEND_EXT_CSD,
                                   0u,
                                   p_dest,
                                   FS_DEV_SD_CID_EXT_CSD_LEN,
                                   1u,
                                   p_err));
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                           FS_SD_Card_SCR_Send()
 *
 * @brief   Get SD CARD Configuration Register (SCR) from card.
 *
 * @param   p_sd_card       Pointer to a SD card.
 *
 * @param   p_dest          Pointer to 8-byte buffer that will receive SD CARD Configuration Register.
 *
 * @param   p_err           Error pointer.
 *
 * @note     (1) The SCR register is present ONLY on SD cards.  The register is defined in [Ref 1],
 *               Section 5.6.
 *
 * @note     (1) CMD55, which indicates to the card that the following command will be an app command,
 *               should receive a R1 response.  The correct R1 response should have the APP_CMD bit
 *               (bit 5) set.
 *******************************************************************************************************/
static void FS_SD_Card_SCR_Send(FS_SD_CARD *p_sd_card,
                                CPU_INT08U *p_dest,
                                RTOS_ERR   *p_err)
{
  CPU_INT32U cmd55_resp;

  WITH_SCOPE_BEGIN(p_err) {
    BREAK_ON_ERR(FS_SD_Card_CmdRShort(p_sd_card,                // Perform CMD55 (R1 resp).
                                      FS_DEV_SD_CMD_APP_CMD,
                                      p_sd_card->RCA,
                                      &cmd55_resp,
                                      p_err));

    //                                                             Chk resp (see Note #2).
    ASSERT_BREAK(DEF_BIT_IS_SET(cmd55_resp, FS_DEV_SD_CARD_R1_APP_CMD), RTOS_ERR_IO);

    BREAK_ON_ERR(FS_SD_Card_DataRd(p_sd_card,                   // Perform ACMD13 & rd data.
                                   (FS_DEV_SD_ACMD_SEND_SCR + 64u),
                                   0u,
                                   p_dest,
                                   FS_DEV_SD_CID_SCR_LEN,
                                   1u,
                                   p_err));
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                           FS_SD_Card_SDStatusSend()
 *
 * @brief    Get SD status from card.
 *
 * @param    p_sd_card   Pointer to a SD card.
 *
 * @param    p_dest      Pointer to 64-byte buffer that will receive the SD status.
 *
 * @param    p_err       Error pointer.
 *
 * @note     (1) The SD status is present ONLY on SD cards.  The information format is defined in
 *               [Ref 1], Section 4.10.2.
 *               - (a) The card MUST have been previously selected, since the SD status can only be
 *                     read in the 'tran' state.
 *
 * @note     (1) CMD55, which indicates to the card that the following command will be an app command,
 *               should receive a R1 response.  The correct R1 response should have the APP_CMD bit
 *               (bit 5) set.
 *******************************************************************************************************/
static void FS_SD_Card_SDStatusSend(FS_SD_CARD *p_sd_card,
                                    CPU_INT08U *p_dest,
                                    RTOS_ERR   *p_err)
{
  CPU_INT32U cmd55_resp;

  WITH_SCOPE_BEGIN(p_err) {
    BREAK_ON_ERR(FS_SD_Card_CmdRShort(p_sd_card,                // Perform CMD55 (R1 resp).
                                      FS_DEV_SD_CMD_APP_CMD,
                                      p_sd_card->RCA,
                                      &cmd55_resp,
                                      p_err));

    //                                                             Chk resp (see Note #2).
    ASSERT_BREAK(DEF_BIT_IS_SET(cmd55_resp, FS_DEV_SD_CARD_R1_APP_CMD), RTOS_ERR_IO);

    BREAK_ON_ERR(FS_SD_Card_DataRd(p_sd_card,                   // Perform ACMD51 & rd data.
                                   (FS_DEV_SD_ACMD_SD_STATUS + 64u),
                                   0u,
                                   p_dest,
                                   64u,
                                   1u,
                                   p_err));
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                           FS_SD_Card_BusWidthSet()
 *
 * @brief    Set bus width.
 *
 * @param    p_sd_card   Pointer to a SD card.
 *
 * @param    width       Maximum bus width, in bits.
 *
 * @return   The configured bus width.
 *
 * @note     (1) The card MUST have been already selected.
 *
 * @note     (2) Bus width:
 *           - (a) Current SD cards always support 1- & 4-bit bus widths according to [Ref 1],
 *                 Section 5-6.
 *                 - (1) The 4-bit field [51..48] of the SCR register specifies allowed bus widths.
 *                       Bit 0 and bit 2 indicate support for 1- and 4-bit bus widths, respectively.
 *                 - (2) The actual bus width can be determined from bits 511:510 of the SD status.
 *           - (b) MMC cards may support 4- or 8-bit bus widths.
 *                 - (1) A bus-test procedure is defined in [Ref 2], Section 7.4.4.   Currently, the
 *                       bus-test procedure is NOT used.
 *                 - (2) The bus width is set using CMD_SWITCH_FUNC with the appropriate argument to
 *                       assign byte 183 of the EXT_CSD register.   The assigned value is 1 for a
 *                       4-bit bus or 2 for a 8-bit bus, as listed in [Ref 2], Table 59.
 *                 - (3) According to [Ref 2], Section 7.4.1, the "host should read the card status,
 *                       using the SEND_STATUS command, after the busy signal is de-asserted, to check
 *                       of the SWITCH operation".
 *******************************************************************************************************/
static CPU_INT08U FS_SD_Card_BusWidthSet(FS_SD_CARD *p_sd_card,
                                         CPU_INT08U width)
{
  FS_SD_INFO         *p_sd_info;
  FS_SD_CARD_BSP_API *p_bsp;
  CPU_INT08U         *p_buf;
  CPU_INT32U         cmd_arg;
  CPU_INT32U         resp1;
  CPU_INT08U         state;
  CPU_INT08U         width_cfgd;
  CPU_INT08U         widths_sd;
  RTOS_ERR           err_tmp;

  p_bsp = (FS_SD_CARD_BSP_API *)p_sd_card->BspPtr;

  p_sd_info = &p_sd_card->SD.Info;
  width_cfgd = 1u;
  p_buf = (CPU_INT08U *)FS_SD_Card_Data.BufPtr;

  switch (p_sd_info->CardType) {
    case FS_DEV_SD_CARDTYPE_SD_V1_X:                            // --------------------- SD CARD ----------------------
    case FS_DEV_SD_CARDTYPE_SD_V2_0:                            // See Note #2a.
    case FS_DEV_SD_CARDTYPE_SD_V2_0_HC:
      if (width >= 4u) {
        RTOS_ERR_SET(err_tmp, RTOS_ERR_NONE);
        FS_SD_Card_SCR_Send(p_sd_card,                          // Rd SCR reg (see Note #2a1) ...
                            p_buf,
                            &err_tmp);
        if (RTOS_ERR_CODE_GET(err_tmp) == RTOS_ERR_NONE) {
          widths_sd = p_buf[1] & 0x0Fu;                         // ... chk supported widths.
          width = (DEF_BIT_IS_CLR(widths_sd, DEF_BIT_02) == DEF_YES) ? 1u : 4u;
        } else {
          LOG_DBG(("Failed to read SCR."));
          width = 1u;
        }
      }

      if (width == 4u) {                                        // Cfg 4-bit width.
        RTOS_ERR_SET(err_tmp, RTOS_ERR_NONE);
        FS_SD_Card_AppCmdRShort(p_sd_card,
                                FS_DEV_SD_ACMD_BUS_WIDTH,
                                FS_DEV_SD_ACMD06_DATA_BUS_WIDTH_4_BIT,
                                &resp1,
                                &err_tmp);

        if (RTOS_ERR_CODE_GET(err_tmp) == RTOS_ERR_NONE) {
          state = DEF_BIT_FIELD_RD(resp1, FS_DEV_SD_CARD_R1_CUR_STATE_MASK);
          //                                                       If cmd performed ...
          //                                                       ... chk actual width (see Note #2a2).
          if (state == FS_DEV_SD_CARD_STATE_TRAN) {
            p_bsp->BusWidthSet(4u);
            FS_SD_Card_SDStatusSend(p_sd_card, p_buf, &err_tmp);
            if (RTOS_ERR_CODE_GET(err_tmp) == RTOS_ERR_NONE) {
              widths_sd = (p_buf[0] & 0xC0u) >> 6;
              if (widths_sd == 2u) {
                width_cfgd = 4u;
              } else {
                LOG_DBG(("Failed to set 4-bit width: DAT_BUS_WIDTH = ", (u)widths_sd, "."));
              }
            } else {
              LOG_DBG(("Failed to read SD status; bus width unknown?"));
            }
          } else {
            LOG_DBG(("Failed to set 4-bit width."));
          }
        } else {
          LOG_DBG(("Failed to set 4-bit width."));
        }
      }
      break;

    //                                                             ----------------------- MMC ------------------------
    case FS_DEV_SD_CARDTYPE_MMC:                                // See Note #2b.
    case FS_DEV_SD_CARDTYPE_MMC_HC:
    default:
      if (width >= 8u) {
        width = 8u;
      } else if (width >= 4u) {
        width = 4u;
      } else {
        width = 1u;
      }

      if (width != 1u) {                                        // Perform CMD6 (sw fnct).
                                                                // See Note #2b2.
        cmd_arg = (width == 4u) ? (1u << 8) : (2u << 8);
        cmd_arg |= 0x03000000u | (183u << 16) | 0x00000001u;

        RTOS_ERR_SET(err_tmp, RTOS_ERR_NONE);
        FS_SD_Card_CmdRShort(p_sd_card,
                             FS_DEV_SD_CMD_SWITCH_FUNC,
                             cmd_arg,
                             &resp1,
                             &err_tmp);

        if (RTOS_ERR_CODE_GET(err_tmp) == RTOS_ERR_NONE) {
          FS_SD_Card_WaitWhileBusy(p_sd_card, &err_tmp);        // See Note #2b3.
        }

        if (RTOS_ERR_CODE_GET(err_tmp) != RTOS_ERR_NONE) {
          LOG_DBG(("Failed to set ", (u)width, "-bit width."));
          width = 1u;
        }
      }
      width_cfgd = width;
      break;
  }

  p_bsp->BusWidthSet(width_cfgd);

  return (width_cfgd);
}

/****************************************************************************************************//**
 *                                               FS_SD_Card_Stop()
 *
 * @brief    Stop transmission.
 *
 * @param    p_sd_card   Pointer to a SD card.
 *
 * @note     (1) When this function is invoked, the card should be in either the 'data' state (after a
 *               multiple block read) or 'rcv' state (after a multiple block write).  CMD12 ("stop
 *               transmission") should move the card into either 'tran' or 'prg' state, respectively.
 *               - (a) If the stop transmission fails, then the card may no longer be in either the
 *                     'data' or 'rcv' state, the only states when CMD12 is valid.  In that case, CMD13
 *                     ("send status") is performed to check the current card state.
 *******************************************************************************************************/
static void FS_SD_Card_Stop(FS_SD_CARD *p_sd_card,
                            RTOS_ERR   *p_err)
{
  CPU_INT32U  resp1b;
  CPU_BOOLEAN stop_succeed;
  CPU_INT08U  state;
  RTOS_ERR    err_tmp;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ERR_SET(err_tmp, RTOS_ERR_NONE);
    FS_SD_Card_CmdRShort(p_sd_card,                             // Stop transmission ... perform CMD12 (R1b resp).
                         FS_DEV_SD_CMD_STOP_TRANSMISSION,
                         0u,
                         &resp1b,
                         &err_tmp);

    //                                                             Check if card is in state 'rcv' or 'data', as expected.
    stop_succeed = DEF_NO;
    if (RTOS_ERR_CODE_GET(err_tmp) == RTOS_ERR_NONE) {
      state = DEF_BIT_FIELD_RD(resp1b, FS_DEV_SD_CARD_R1_CUR_STATE_MASK);
      if ((state == FS_DEV_SD_CARD_STATE_DATA)
          || (state == FS_DEV_SD_CARD_STATE_RCV)) {
        stop_succeed = DEF_YES;
      }
    }

    if (!stop_succeed) {                                        // If stop transmission failed (see Note #1a) ...
      CPU_INT16U retries = 0u;

      while (retries < FS_DEV_SD_CARD_RETRIES_CMD_BUSY) {
        RTOS_ERR_SET(err_tmp, RTOS_ERR_NONE);
        FS_SD_Card_CmdRShort(p_sd_card,                         // ... get status ... perform CMD13 (R1 resp).
                             FS_DEV_SD_CMD_SEND_STATUS,
                             p_sd_card->RCA,
                             &resp1b,
                             &err_tmp);
        if (RTOS_ERR_CODE_GET(err_tmp) == RTOS_ERR_NONE) {
          if (DEF_BIT_IS_SET(resp1b, FS_DEV_SD_CARD_R1_READY_FOR_DATA)) {
            break;
          }
        }
        retries++;
      }

      ASSERT_BREAK(retries < FS_DEV_SD_CARD_RETRIES_CMD_BUSY, RTOS_ERR_IO);
    }
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                               FS_SD_Card_Sel()
 *
 * @brief    Select card.
 *
 * @param    p_sd_card   Pointer to a SD card.
 *
 * @param    sel         Indicates whether card should be selected or deselected :
 *                           - DEF_YES    Card should be selected.
 *                           - DEF_NO     Card should be deselected.
 *
 * @note     (1) A card state transition table is provided in [Ref 1], Table 4-28.
 *               - (a) Before the card is selected (or after the card is deselected), it should be in
 *                     'stby' state.  After the card is selected, it should be in the 'tran' state and be
 *                     ready for data.
 *               - (b) The transition between the 'prg' & 'dis' states are not handled; the card should
 *                     NEVER be in either of these states when this function is called.
 *
 * @note     (2) The state returned in the response to the SEL_DESEL_CARD command is the card state
 *               when that command was received, not the state of the device after the transition
 *               is effected.  The active state of the card must be checked with the SEND_STATUS
 *               command.
 *
 * @note     (3) In certain cases, the SEL_DESEL_CARD command may fail & the state should be checked
 *               directly.
 *               - (a) If the card is already selected, the SEL_DESEL_CARD command will fail if
 *                     performed with the card's RCA.  The state of the card must be checked with the
 *                     SEND_STATUS command; it should be in the 'tran' state & ready for data.
 *               - (b) The SEL_DESEL_CARD command may fail if performed with a RCA of 0, whether or not
 *                     the card is already selected.  The state of the card must be checked with the
 *                     SEND_STATUS command; it should be in the 'stby' state.
 *******************************************************************************************************/
static void FS_SD_Card_Sel(FS_SD_CARD  *p_sd_card,
                           CPU_BOOLEAN sel,
                           RTOS_ERR    *p_err)
{
  CPU_INT32U  resp1;
  CPU_INT16U  retries;
  CPU_INT32U  state;
  CPU_BOOLEAN rtn;
  CPU_INT32U  rca;
  RTOS_ERR    err_tmp;

  WITH_SCOPE_BEGIN(p_err) {
    rca = sel ? p_sd_card->RCA : 0u;
    retries = 0u;
    rtn = DEF_NO;
    BREAK_ON_ERR(while) ((retries < FS_DEV_SD_CARD_RETRIES_CMD) && !rtn) {
      RTOS_ERR_SET(err_tmp, RTOS_ERR_NONE);
      FS_SD_Card_CmdRShort(p_sd_card,                           // Perform CMD7 (R1b resp).
                           FS_DEV_SD_CMD_SEL_DESEL_CARD,
                           rca,
                           &resp1,
                           &err_tmp);
      if (RTOS_ERR_CODE_GET(err_tmp) != RTOS_ERR_NONE) {
        rtn = DEF_YES;
      }

      RTOS_ERR_SET(err_tmp, RTOS_ERR_NONE);
      FS_SD_Card_CmdRShort(p_sd_card,                           // Chk current card state (see Note #2).
                           FS_DEV_SD_CMD_SEND_STATUS,
                           p_sd_card->RCA,
                           &resp1,
                           &err_tmp);

      if (RTOS_ERR_CODE_GET(err_tmp) == RTOS_ERR_NONE) {
        state = DEF_BIT_FIELD_RD(resp1, FS_DEV_SD_CARD_R1_CUR_STATE_MASK);
        if (sel) {                                              // Chk if card sel'd (see Note #3a).
          if (state == FS_DEV_SD_CARD_STATE_TRAN) {
            if (DEF_BIT_IS_SET(resp1, FS_DEV_SD_CARD_R1_READY_FOR_DATA) == DEF_YES) {
              break;
            }
          }
        } else {                                                // Chk if card desel'd (see Note #3b).
          if (state == FS_DEV_SD_CARD_STATE_STBY) {
            break;
          }
        }
      }
      retries++;
    }

    ASSERT_BREAK(retries < FS_DEV_SD_CARD_RETRIES_CMD, RTOS_ERR_IO);
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                       FS_SD_Card_WaitWhileBusy()
 *
 * @brief    Wait while card is busy.
 *
 * @param    p_sd_card   Pointer to a SD card.
 *
 * @param    p_err       Error pointer.
 *
 * @note     (1) When the card is no longer busy, it should be in 'tran' state and be ready for data.
 *
 * @note     (2) If the card is in either the 'rcv' or 'data' states, then CMD12 (STOP_TRANSMISSION)
 *               must be performed.
 *******************************************************************************************************/
static void FS_SD_Card_WaitWhileBusy(FS_SD_CARD *p_sd_card,
                                     RTOS_ERR   *p_err)
{
  CPU_INT32U resp1;
  CPU_INT16U retries;
  CPU_INT32U state;

  WITH_SCOPE_BEGIN(p_err) {
    retries = 0u;
    BREAK_ON_ERR(while) (retries < FS_DEV_SD_CARD_RETRIES_CMD_BUSY) {
      BREAK_ON_ERR(FS_SD_Card_CmdRShort(p_sd_card,              // Perform CMD13 (R1 resp).
                                        FS_DEV_SD_CMD_SEND_STATUS,
                                        p_sd_card->RCA,
                                        &resp1,
                                        p_err));

      state = DEF_BIT_FIELD_RD(resp1, FS_DEV_SD_CARD_R1_CUR_STATE_MASK);
      if (state == FS_DEV_SD_CARD_STATE_TRAN) {
        if (DEF_BIT_IS_SET(resp1, FS_DEV_SD_CARD_R1_READY_FOR_DATA)) {
          break;
        }
      } else if ((state == FS_DEV_SD_CARD_STATE_RCV)
                 || (state == FS_DEV_SD_CARD_STATE_DATA)) {
        //                                                         Move card to 'tran' state (see Note #2).
        BREAK_ON_ERR(FS_SD_Card_Stop(p_sd_card, p_err));
      } else {
        //                                                         Nothing to do. Retry.
      }

      retries++;
    }

    if (retries >= FS_DEV_SD_CARD_RETRIES_CMD_BUSY) {
      BREAK_ERR_SET(RTOS_ERR_IO);
    }
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                           FS_SD_Card_HandleErrBSP()
 *
 * @brief    Handle BSP error.
 *
 * @param    p_sd_card   Pointer to a SD card.
 *
 * @param    err         BSP error.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_CTR_ERR_EN == DEF_ENABLED)
static void FS_SD_Card_HandleErrBSP(FS_SD_CARD         *p_sd_card,
                                    FS_DEV_SD_CARD_ERR err)
{
  switch (err) {
    case FS_DEV_SD_CARD_ERR_BUSY:                               // Controller is busy.
      FS_BLK_DEV_CTR_ERR_INC(p_sd_card->ErrCardBusyCtr);
      break;
    case FS_DEV_SD_CARD_ERR_UNKNOWN:                            // Unknown or other error.
      FS_BLK_DEV_CTR_ERR_INC(p_sd_card->ErrCardUnknownCtr);
      break;

    case FS_DEV_SD_CARD_ERR_WAIT_TIMEOUT:                       // Timeout in waiting for cmd resp/data.
      FS_BLK_DEV_CTR_ERR_INC(p_sd_card->ErrCardWaitTimeoutCtr);
      break;

    case FS_DEV_SD_CARD_ERR_RESP_TIMEOUT:                       // Timeout in rx'ing cmd resp.
      FS_BLK_DEV_CTR_ERR_INC(p_sd_card->ErrCardRespTimeoutCtr);
      break;

    case FS_DEV_SD_CARD_ERR_RESP_CHKSUM:                        // Err in resp checksum.
      FS_BLK_DEV_CTR_ERR_INC(p_sd_card->ErrCardRespChksumCtr);
      break;

    case FS_DEV_SD_CARD_ERR_RESP_CMD_IX:                        // Resp cmd ix err.
      FS_BLK_DEV_CTR_ERR_INC(p_sd_card->ErrCardRespCmdIxCtr);
      break;

    case FS_DEV_SD_CARD_ERR_RESP_END_BIT:                       // Resp end bit err.
      FS_BLK_DEV_CTR_ERR_INC(p_sd_card->ErrCardRespEndBitCtr);
      break;

    case FS_DEV_SD_CARD_ERR_RESP:                               // Other resp err.
      FS_BLK_DEV_CTR_ERR_INC(p_sd_card->ErrCardRespCtr);
      break;

    case FS_DEV_SD_CARD_ERR_DATA_UNDERRUN:                      // Data underrun.
      FS_BLK_DEV_CTR_ERR_INC(p_sd_card->ErrCardDataUnderrunCtr);
      break;

    case FS_DEV_SD_CARD_ERR_DATA_OVERRUN:                       // Data overrun.
      FS_BLK_DEV_CTR_ERR_INC(p_sd_card->ErrCardDataOverrunCtr);
      break;

    case FS_DEV_SD_CARD_ERR_DATA_TIMEOUT:                       // Timeout in rx'ing data.
      FS_BLK_DEV_CTR_ERR_INC(p_sd_card->ErrCardDataTimeoutCtr);
      break;

    case FS_DEV_SD_CARD_ERR_DATA_CHKSUM:                        // Err in data checksum.
      FS_BLK_DEV_CTR_ERR_INC(p_sd_card->ErrCardDataChksumCtr);
      break;

    case FS_DEV_SD_CARD_ERR_DATA_START_BIT:                     // Data start bit err.
      FS_BLK_DEV_CTR_ERR_INC(p_sd_card->ErrCardDataStartBitCtr);
      break;

    case FS_DEV_SD_CARD_ERR_DATA:                               // Other data err.
      FS_BLK_DEV_CTR_ERR_INC(p_sd_card->ErrCardDataCtr);
      break;

    case FS_DEV_SD_CARD_ERR_NONE:                               // No error.
    case FS_DEV_SD_CARD_ERR_NO_CARD:                            // No card present.
    default:
      break;
  }
}
#endif

/****************************************************************************************************//**
 *                                           FS_SD_Card_InitCmd()
 *
 * @brief    Initialize SD card command information.
 *
 * @param    p_cmd       Pointer to a SD card.
 *
 * @param    cmd_nbr     Control index.
 *
 * @param    arg         Command argument.
 *
 * @param    card_type   Card type :
 *                           - FS_DEV_SD_CARDTYPE_NONE
 *                           - FS_DEV_SD_CARDTYPE_SD_V1_X
 *                           - FS_DEV_SD_CARDTYPE_SD_V2_0
 *                           - FS_DEV_SD_CARDTYPE_SD_V2_0_HC
 *                           - FS_DEV_SD_CARDTYPE_MMC
 *                           - FS_DEV_SD_CARDTYPE_MMC_HC
 *******************************************************************************************************/
static void FS_SD_Card_CmdInit(FS_DEV_SD_CARD_CMD *p_cmd,
                               CPU_INT08U         cmd_nbr,
                               CPU_INT32U         arg,
                               CPU_INT08U         card_type)
{
  CPU_INT08U resp_type;
  CPU_INT32U flags;
  CPU_INT08U data_dir;
  CPU_INT08U data_type;

  p_cmd->Cmd = (cmd_nbr >= 64u) ? (cmd_nbr - 64u) : cmd_nbr;
  p_cmd->Arg = arg;

  resp_type = FS_SD_Card_CmdRespType[cmd_nbr];

  switch (card_type) {
    case FS_DEV_SD_CARDTYPE_SD_V1_X:
    case FS_DEV_SD_CARDTYPE_SD_V2_0:
    case FS_DEV_SD_CARDTYPE_SD_V2_0_HC:
      resp_type = (resp_type & FS_DEV_SD_CARD_RESP_TYPE_MASK) >> FS_DEV_SD_CARD_RESP_TYPE_OFFSET;
      break;

    case FS_DEV_SD_CARDTYPE_MMC:
    case FS_DEV_SD_CARDTYPE_MMC_HC:
      resp_type = (resp_type & FS_DEV_MMC_RESP_TYPE_MASK) >> FS_DEV_MMC_RESP_TYPE_OFFSET;
      break;

    case FS_DEV_SD_CARDTYPE_NONE:
    default:
      resp_type = (resp_type & FS_DEV_SD_CARD_RESP_TYPE_MASK) >> FS_DEV_SD_CARD_RESP_TYPE_OFFSET;
      break;
  }

  flags = FS_SD_Card_CmdFlag[resp_type];

  if (cmd_nbr == FS_DEV_SD_CMD_GO_IDLE_STATE) {
    flags |= FS_DEV_SD_CARD_CMD_FLAG_INIT;
  }

  if ((card_type == FS_DEV_SD_CARDTYPE_NONE)
      || (card_type == FS_DEV_SD_CARDTYPE_SD_V1_X)
      || (card_type == FS_DEV_SD_CARDTYPE_SD_V2_0)
      || (card_type == FS_DEV_SD_CARDTYPE_SD_V2_0_HC)) {
    switch (cmd_nbr) {                                          // ------------------- GET DATA DIR -------------------
      case FS_DEV_SD_CMD_READ_DAT_UNTIL_STOP:
      case FS_DEV_SD_CMD_READ_SINGLE_BLOCK:
      case FS_DEV_SD_CMD_READ_MULTIPLE_BLOCK:
      case FS_DEV_SD_CMD_BUSTEST_R:
      case (FS_DEV_SD_ACMD_SD_STATUS + 64u):
      case (FS_DEV_SD_ACMD_SEND_SCR + 64u):
        data_dir = FS_DEV_SD_CARD_DATA_DIR_CARD_TO_HOST;
        flags |= FS_DEV_SD_CARD_CMD_FLAG_START_DATA_TX;
        break;

      case FS_DEV_SD_CMD_STOP_TRANSMISSION:
        data_dir = FS_DEV_SD_CARD_DATA_DIR_NONE;
        flags |= FS_DEV_SD_CARD_CMD_FLAG_STOP_DATA_TX;
        break;

      case FS_DEV_SD_CMD_WRITE_DAT_UNTIL_STOP:
      case FS_DEV_SD_CMD_WRITE_BLOCK:
      case FS_DEV_SD_CMD_WRITE_MULTIPLE_BLOCK:
      case FS_DEV_SD_CMD_BUSTEST_W:
        data_dir = FS_DEV_SD_CARD_DATA_DIR_HOST_TO_CARD;
        flags |= FS_DEV_SD_CARD_CMD_FLAG_START_DATA_TX;
        break;

      default:
        data_dir = FS_DEV_SD_CARD_DATA_DIR_NONE;
        break;
    }

    switch (cmd_nbr) {                                          // ------------------- GET DATA TYPE ------------------
      case FS_DEV_SD_CMD_READ_DAT_UNTIL_STOP:
      case FS_DEV_SD_CMD_WRITE_DAT_UNTIL_STOP:
        data_type = FS_DEV_SD_CARD_DATA_TYPE_STREAM;
        break;

      case FS_DEV_SD_CMD_READ_SINGLE_BLOCK:
      case FS_DEV_SD_CMD_WRITE_BLOCK:
      case (FS_DEV_SD_ACMD_SD_STATUS + 64u):
      case (FS_DEV_SD_ACMD_SEND_SCR + 64u):
      case FS_DEV_SD_CMD_BUSTEST_R:
      case FS_DEV_SD_CMD_BUSTEST_W:
        data_type = FS_DEV_SD_CARD_DATA_TYPE_SINGLE_BLOCK;
        break;

      case FS_DEV_SD_CMD_READ_MULTIPLE_BLOCK:
      case FS_DEV_SD_CMD_WRITE_MULTIPLE_BLOCK:
        data_type = FS_DEV_SD_CARD_DATA_TYPE_MULTI_BLOCK;
        break;

      default:
        data_type = FS_DEV_SD_CARD_DATA_TYPE_NONE;
        break;
    }
  } else {
    switch (cmd_nbr) {                                          // ------------------- GET DATA DIR -------------------
      case FS_DEV_MMC_CMD_READ_DAT_UNTIL_STOP:
      case FS_DEV_MMC_CMD_READ_SINGLE_BLOCK:
      case FS_DEV_MMC_CMD_READ_MULTIPLE_BLOCK:
      case FS_DEV_MMC_CMD_BUSTEST_R:
      case FS_DEV_MMC_CMD_SEND_EXT_CSD:
        data_dir = FS_DEV_SD_CARD_DATA_DIR_CARD_TO_HOST;
        flags |= FS_DEV_SD_CARD_CMD_FLAG_START_DATA_TX;
        break;

      case FS_DEV_MMC_CMD_STOP_TRANSMISSION:
        data_dir = FS_DEV_SD_CARD_DATA_DIR_NONE;
        flags |= FS_DEV_SD_CARD_CMD_FLAG_STOP_DATA_TX;
        break;

      case FS_DEV_MMC_CMD_WRITE_DAT_UNTIL_STOP:
      case FS_DEV_MMC_CMD_WRITE_BLOCK:
      case FS_DEV_MMC_CMD_WRITE_MULTIPLE_BLOCK:
      case FS_DEV_MMC_CMD_BUSTEST_W:
        data_dir = FS_DEV_SD_CARD_DATA_DIR_HOST_TO_CARD;
        flags |= FS_DEV_SD_CARD_CMD_FLAG_START_DATA_TX;
        break;

      default:
        data_dir = FS_DEV_SD_CARD_DATA_DIR_NONE;
        break;
    }

    switch (cmd_nbr) {                                          // ------------------- GET DATA TYPE ------------------
      case FS_DEV_MMC_CMD_READ_DAT_UNTIL_STOP:
      case FS_DEV_MMC_CMD_WRITE_DAT_UNTIL_STOP:
        data_type = FS_DEV_SD_CARD_DATA_TYPE_STREAM;
        break;

      case FS_DEV_MMC_CMD_READ_SINGLE_BLOCK:
      case FS_DEV_MMC_CMD_WRITE_BLOCK:
      case FS_DEV_MMC_CMD_BUSTEST_R:
      case FS_DEV_MMC_CMD_BUSTEST_W:
      case FS_DEV_MMC_CMD_SEND_EXT_CSD:
        data_type = FS_DEV_SD_CARD_DATA_TYPE_SINGLE_BLOCK;
        break;

      case FS_DEV_MMC_CMD_READ_MULTIPLE_BLOCK:
      case FS_DEV_MMC_CMD_WRITE_MULTIPLE_BLOCK:
        data_type = FS_DEV_SD_CARD_DATA_TYPE_MULTI_BLOCK;
        break;

      default:
        data_type = FS_DEV_SD_CARD_DATA_TYPE_NONE;
        break;
    }
  }

  //                                                               ----------------- ASSIGN CMD PARAMS ----------------
  p_cmd->RespType = resp_type;
  p_cmd->Flags = flags;
  p_cmd->DataType = data_type;
  p_cmd->DataDir = data_dir;

  p_cmd->BlkSize = 0u;
  p_cmd->BlkCnt = 0u;
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_FS_STORAGE_SD_CARD_AVAIL
