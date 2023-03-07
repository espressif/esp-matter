/***************************************************************************//**
 * @file
 * @brief File System - SD Card Media Driver
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
 * @addtogroup FS_STORAGE
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  FS_SD_CARD_H_
#define  FS_SD_CARD_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <fs/include/fs_sd.h>
#include  <cpu/include/cpu.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       RESPONSE TYPES DEFINES
 *
 * @note     (1) The response type data stored in the FS_SD_Card_CmdRespType[] table encodes the
 *               the SD response to the low nibble while the MMC response type is encoded as the
 *               high nibble
 *******************************************************************************************************/

#define  FS_DEV_SD_CARD_RESP_TYPE_UNKNOWN                  0u
#define  FS_DEV_SD_CARD_RESP_TYPE_NONE                     1u   ///< No  Response.
#define  FS_DEV_SD_CARD_RESP_TYPE_R1                       2u   ///< R1  Response: Normal Response Command.
#define  FS_DEV_SD_CARD_RESP_TYPE_R1B                      3u   ///< R1b Response.
#define  FS_DEV_SD_CARD_RESP_TYPE_R2                       4u   ///< R2  Response: CID, CSD Register.
#define  FS_DEV_SD_CARD_RESP_TYPE_R3                       5u   ///< R3  Response: OCR Register.
#define  FS_DEV_SD_CARD_RESP_TYPE_R4                       6u   ///< R4  Response: Fast I/O Response (MMC).
#define  FS_DEV_SD_CARD_RESP_TYPE_R5                       7u   ///< R5  Response: Interrupt Request Response (MMC).
#define  FS_DEV_SD_CARD_RESP_TYPE_R5B                      8u   ///< R5B Response.
#define  FS_DEV_SD_CARD_RESP_TYPE_R6                       9u   ///< R6  Response: Published RCA Response.
#define  FS_DEV_SD_CARD_RESP_TYPE_R7                      10u   ///< R7  Response: Card Interface Condition.

#define  FS_DEV_MMC_RESP_TYPE_UNKNOWN               (0u << 4u)
#define  FS_DEV_MMC_RESP_TYPE_NONE                  (1u << 4u)  ///< No  Response.
#define  FS_DEV_MMC_RESP_TYPE_R1                    (2u << 4u)  ///< R1  Response: Normal Response Command.
#define  FS_DEV_MMC_RESP_TYPE_R1B                   (3u << 4u)  ///< R1b Response.
#define  FS_DEV_MMC_RESP_TYPE_R2                    (4u << 4u)  ///< R2  Response: CID, CSD Register.
#define  FS_DEV_MMC_RESP_TYPE_R3                    (5u << 4u)  ///< R3  Response: OCR Register.
#define  FS_DEV_MMC_RESP_TYPE_R4                    (6u << 4u)  ///< R4  Response: Fast I/O Response (MMC).
#define  FS_DEV_MMC_RESP_TYPE_R5                    (7u << 4u)  ///< R5  Response: Interrupt Request Response (MMC).
#define  FS_DEV_MMC_RESP_TYPE_R5B                   (8u << 4u)  ///< R5B Response.
#define  FS_DEV_MMC_RESP_TYPE_R6                    (9u << 4u)  ///< R6  Response: Published RCA Response.
#define  FS_DEV_MMC_RESP_TYPE_R7                    10u << 4u)  ///< R7  Response: Card Interface Condition.

#define  FS_DEV_SD_CARD_RESP_TYPE_MASK                   0x0F   ///< SD Card response mask.
#define  FS_DEV_SD_CARD_RESP_TYPE_OFFSET                 0x00   ///< SD Card response offset.
#define  FS_DEV_MMC_RESP_TYPE_MASK                       0xF0   ///< MMC response mask.
#define  FS_DEV_MMC_RESP_TYPE_OFFSET                     0x04   ///< MCC response offset.

/****************************************************************************************************//**
 *                                           CMD FLAG DEFINES
 *
 * @note     (1) Command flag defines :
 *           - (a) If FS_DEV_SD_CARD_CMD_FLAG_INIT is set, then the controller should send the 80-clock
 *               initialization sequence before transmitting the command.
 *           - (b) If FS_DEV_SD_CARD_CMD_FLAG_OPEN_DRAIN is set, then the command should be transmitted
 *               with the command line in an open drain state.
 *           - (c) If FS_DEV_SD_CARD_CMD_FLAG_BUSY is set, then the controller should check for busy after
 *               the response before transmitting any data.
 *           - (d) If FS_DEV_SD_CARD_CMD_FLAG_CRC_VALID is set, then CRC check should be enabled.
 *           - (e) If FS_DEV_SD_CARD_CMD_FLAG_IX_VALID is set, then index check should be enabled.
 *           - (f) If FS_DEV_SD_CARD_CMD_FLAG_START_DATA_TX is set, then this command will either start
 *               transmitting data, or expect to start receiving data.
 *           - (g) If FS_DEV_SD_CARD_CMD_FLAG_STOP_DATA_TX is set, then this command is attempting to stop
 *               (abort) data transmission/reception.
 *           - (h) If FS_DEV_SD_CARD_CMD_FLAG_RESP is set, then a response is expected for this command.
 *           - (i) If FS_DEV_SD_CARD_CMD_FLAG_RESP_LONG is set, then a long response is expected for this
 *               command.
 *******************************************************************************************************/

#define  FS_DEV_SD_CARD_CMD_FLAG_NONE             DEF_BIT_NONE
#define  FS_DEV_SD_CARD_CMD_FLAG_INIT             DEF_BIT_00    ///< Initializaton sequence before command.
#define  FS_DEV_SD_CARD_CMD_FLAG_BUSY             DEF_BIT_01    ///< Busy signal expected after command.
#define  FS_DEV_SD_CARD_CMD_FLAG_CRC_VALID        DEF_BIT_02    ///< CRC valid after command.
#define  FS_DEV_SD_CARD_CMD_FLAG_IX_VALID         DEF_BIT_03    ///< Index valid after command.
#define  FS_DEV_SD_CARD_CMD_FLAG_OPEN_DRAIN       DEF_BIT_04    ///< Command line is open drain.
#define  FS_DEV_SD_CARD_CMD_FLAG_START_DATA_TX    DEF_BIT_05    ///< Data start command.
#define  FS_DEV_SD_CARD_CMD_FLAG_STOP_DATA_TX     DEF_BIT_06    ///< Data stop  command.
#define  FS_DEV_SD_CARD_CMD_FLAG_RESP             DEF_BIT_07    ///< Response expected.
#define  FS_DEV_SD_CARD_CMD_FLAG_RESP_LONG        DEF_BIT_08    ///< Long response expected.

/****************************************************************************************************//**
 *                                       DATA DIR & TYPE DEFINES
 *
 * @note     (a) The data direction will be otherwise than 'none' if & only if the command flag
 *               FS_DEV_SD_CARD_CMD_FLAG_DATA_START is set.
 *******************************************************************************************************/

#define  FS_DEV_SD_CARD_DATA_DIR_NONE                      0u   ///< No data transfer.
#define  FS_DEV_SD_CARD_DATA_DIR_HOST_TO_CARD              1u   ///< Transfer host-to-card (write).
#define  FS_DEV_SD_CARD_DATA_DIR_CARD_TO_HOST              2u   ///< Transfer card-to-host (read).

#define  FS_DEV_SD_CARD_DATA_TYPE_NONE                     0u   ///< No data transfer.
#define  FS_DEV_SD_CARD_DATA_TYPE_SINGLE_BLOCK             1u   ///< Single data block.
#define  FS_DEV_SD_CARD_DATA_TYPE_MULTI_BLOCK              2u   ///< Multiple data blocks.
#define  FS_DEV_SD_CARD_DATA_TYPE_STREAM                   3u   ///< Stream data.

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/*
 ********************************************************************************************************
 *                                       SD CARD COMMAND DATA TYPE
 *******************************************************************************************************/

typedef struct fs_dev_sd_card_cmd {
  CPU_INT08U Cmd;                                               ///< Command number.
  CPU_INT32U Arg;                                               ///< Command argument.
  CPU_INT32U Flags;                                             ///< Command flags.
  CPU_INT08U RespType;                                          ///< Response type.
  CPU_INT08U DataDir;                                           ///< Data transfer direction.
  CPU_INT08U DataType;                                          ///< Data transfer type.
  CPU_INT32U BlkSize;                                           ///< Size of block(s) in data transfer.
  CPU_INT32U BlkCnt;                                            ///< Number of blocks in data transfer.
} FS_DEV_SD_CARD_CMD;

/****************************************************************************************************//**
 *                                               CARD ERRORS
 *
 * @note     (a) One of these errors should be returned from the BSP from 'FS_SD_Card_BSP_CmdStart()',
 *               'FS_SD_Card_BSP_CmdWaitEnd()', 'FS_SD_Card_BSP_CmdDataRd()' and
 *               'FS_SD_Card_BSP_CmdDataWr()'.
 *******************************************************************************************************/

typedef enum fs_dev_sd_card_err {
  FS_DEV_SD_CARD_ERR_NONE = 0u,                                 ///< No error.
  FS_DEV_SD_CARD_ERR_NO_CARD = 1u,                              ///< No card present.
  FS_DEV_SD_CARD_ERR_BUSY = 2u,                                 ///< Controller is busy.
  FS_DEV_SD_CARD_ERR_UNKNOWN = 3u,                              ///< Unknown or other error.
  FS_DEV_SD_CARD_ERR_WAIT_TIMEOUT = 4u,                         ///< Timeout in waiting for cmd response/data.
  FS_DEV_SD_CARD_ERR_RESP_TIMEOUT = 5u,                         ///< Timeout in receiving cmd response.
  FS_DEV_SD_CARD_ERR_RESP_CHKSUM = 6u,                          ///< Error in response checksum.
  FS_DEV_SD_CARD_ERR_RESP_CMD_IX = 7u,                          ///< Response command index err.
  FS_DEV_SD_CARD_ERR_RESP_END_BIT = 8u,                         ///< Response end bit error.
  FS_DEV_SD_CARD_ERR_RESP = 9u,                                 ///< Other response error.
  FS_DEV_SD_CARD_ERR_DATA_UNDERRUN = 10u,                       ///< Data underrun.
  FS_DEV_SD_CARD_ERR_DATA_OVERRUN = 11u,                        ///< Data overrun.
  FS_DEV_SD_CARD_ERR_DATA_TIMEOUT = 12u,                        ///< Timeout in receiving data.
  FS_DEV_SD_CARD_ERR_DATA_CHKSUM = 13u,                         ///< Error in data checksum.
  FS_DEV_SD_CARD_ERR_DATA_START_BIT = 14u,                      ///< Data start bit error.
  FS_DEV_SD_CARD_ERR_DATA = 15u                                 ///< Other data error.
} FS_DEV_SD_CARD_ERR;

/****************************************************************************************************//**
 *                                           SD CARD BSP API
 * @{
 *******************************************************************************************************/
//                                                                 *INDENT-OFF*
typedef struct fs_sd_card_bsp_api {
  void *(*Add)(RTOS_ERR *p_err);                                ///< Add a SD/MMC instance.

  CPU_BOOLEAN (*Open)(void);                                    ///< Open (init) SD/MMC card interface.

  void (*Close)(void);                                          ///< Close (uninit) SD/MMC card interface.

  void (*Lock)(void);                                           ///< Acquire SD/MMC card bus lock.

  void (*Unlock)(void);                                         ///< Release SD/MMC card bus lock.

  void (*CmdStart)(FS_DEV_SD_CARD_CMD *p_cmd,                   ///< Start a command.
                   void               *p_data,
                   FS_DEV_SD_CARD_ERR *p_err);

  void (*CmdEndWait)(FS_DEV_SD_CARD_CMD *p_cmd,                 ///< Wait for command end (& response).
                     CPU_INT32U         *p_resp,
                     FS_DEV_SD_CARD_ERR *p_err);

  void (*CmdDataRd)(FS_DEV_SD_CARD_CMD *p_cmd,                  ///< Read data following command.
                    void               *p_dest,
                    FS_DEV_SD_CARD_ERR *p_err);

  void (*CmdDataWr)(FS_DEV_SD_CARD_CMD *p_cmd,                  ///< Write data following command.
                    void               *p_src,
                    FS_DEV_SD_CARD_ERR *p_err);

  CPU_INT32U (*BlkCntMaxGet)(CPU_INT32U blk_size);              ///< Get max block cnt for block size.

  CPU_INT08U (*BusWidthMaxGet)(void);                           ///< Get max bus width, in bits.

  void (*BusWidthSet)(CPU_INT08U width);                        ///< Set bus width.

  void (*ClkFreqSet)(CPU_INT32U freq);                          ///< Set clock frequency.

  void (*TimeoutDataSet)(CPU_INT32U to_clks);                   ///< Set data timeout.

  void (*TimeoutRespSet)(CPU_INT32U to_ms);                     ///< Set response timeout.
} FS_SD_CARD_BSP_API;
//                                                                 *INDENT-ON*
typedef struct fs_sd_card_hw_info {
  const FS_SD_CARD_BSP_API *BspApiPtr;
  CPU_SIZE_T               AlignReq;
} FS_SD_CARD_HW_INFO;

struct fs_sd_card_pm_item {
  struct fs_media_pm_item  MediaPmItem;
  const FS_SD_CARD_HW_INFO *HwInfoPtr;
};

///< @}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

extern const struct fs_media_api FS_SD_Card_MediaApi;

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MACRO'S
 ********************************************************************************************************
 *******************************************************************************************************/

#define  FS_SD_CARD_HW_INFO_REG(p_name, p_hw_info)                               \
  do {                                                                           \
    if (((FS_SD_CARD_HW_INFO *)(p_hw_info))->BspApiPtr != DEF_NULL) {            \
      static const struct fs_sd_card_pm_item _pm_item = {                        \
        .MediaPmItem.PmItem.StrID = p_name,                                      \
        .MediaPmItem.PmItem.Type = PLATFORM_MGR_ITEM_TYPE_HW_INFO_FS_SD_CARD,    \
        .MediaPmItem.MediaApiPtr = &FS_SD_Card_MediaApi,                         \
        .HwInfoPtr = (const FS_SD_CARD_HW_INFO *)(p_hw_info),                    \
      };                                                                         \
      PlatformMgrItemInitAdd((PLATFORM_MGR_ITEM *)&_pm_item.MediaPmItem.PmItem); \
    }                                                                            \
  } while (0);

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                            MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
