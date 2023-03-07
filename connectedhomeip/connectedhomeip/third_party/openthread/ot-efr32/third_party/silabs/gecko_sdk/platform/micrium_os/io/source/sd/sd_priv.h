/***************************************************************************//**
 * @file
 * @brief IO - Internal Sd Api
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef  _SD_PRIV_H_
#define  _SD_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <common/include/lib_mem.h>
#include  <common/include/rtos_err.h>
#include  <common/source/kal/kal_priv.h>
#include  <common/source/collections/slist_priv.h>

#include  <io/source/sd/sd_io_fnct_priv.h>
#include  <io/source/sd/sd_card_ctrlr_priv.h>
#include  <io/include/sd.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  SD_IO_FNCT_NBR_MAX                         7u

#define  SD_FREQ_DFLT_HZ                       400000u          // Dflt clk spd, in Hz.

/********************************************************************************************************
 *                                               SD COMMANDS
 *
 * Note(s) : (1) Detailed command descriptions are given in [Ref 1], Section 4.7.5, and [Ref 2], Section 7.9.4.
 *
 *           (2) (a) Commands that are not supported in SPI mode include :
 *
 *                       CMD2        CMD11       CMD39
 *                       CMD3        CMD15       CMD40
 *                       CMD4        CMD20
 *                       CMD7        CMD26
 *
 *               (b) Commands that are reserved for I/O mode include :
 *
 *                       CMD5
 *                       CMD52
 *                       CMD53
 *                       CMD54
 *
 *               (c) Reserved commands include :
 *
 *                       CMD14       CMD37       CMD50
 *                       CMD19       CMD41       CMD51
 *                       CMD21       CMD43       CMD57
 *                       CMD22       CMD44       CMD60
 *                       CMD23       CMD45       CMD61
 *                       CMD31       CMD46       CMD62
 *                       CMD34       CMD47       CMD63
 *                       CMD35       CMD48
 *                       CMD36       CMD49
 *******************************************************************************************************/

//                                                                 CMD0: Resets all cards to idle state.
#define  SD_CMD_CMD00                           0u
#define  SD_CMD_GO_IDLE_STATE                   SD_CMD_CMD00
#define  MMC_CMD_GO_IDLE_STATE                  SD_CMD_CMD00

//                                                                 CMD1: Sends host cap supp info & act card's init.
#define  SD_CMD_CMD01                           1u
#define  SD_CMD_SEND_OP_COND                    SD_CMD_CMD01
#define  MMC_CMD_SEND_OP_COND                   SD_CMD_CMD01

//                                                                 CMD2: Asks any card to send CID numbers on CMD line.
#define  SD_CMD_CMD02                           2u
#define  SD_CMD_ALL_SEND_CID                    SD_CMD_CMD02
#define  MMC_CMD_ALL_SEND_CID                   SD_CMD_CMD02

//                                                                 CMD3: Asks card to publish new RCA.
#define  SD_CMD_CMD03                           3u
#define  SD_CMD_SEND_RELATIVE_ADDR              SD_CMD_CMD03
#define  MMC_CMD_SEND_RELATIVE_ADDR             SD_CMD_CMD03

//                                                                 CMD4: Programs the DSR of all cards.
#define  SD_CMD_CMD04                           4u
#define  SD_CMD_SET_DSR                         SD_CMD_CMD04
#define  MMC_CMD_SET_DSR                        SD_CMD_CMD04

//                                                                 CMD5: IO send operational conditions.
#define  SD_CMD_CMD05                           5u
#define  SD_CMD_IO_SEND_OP_COND                 SD_CMD_CMD05

//                                                                 CMD6: Switches card function.
#define  SD_CMD_CMD06                           6u
#define  SD_CMD_SWITCH_FUNC                     SD_CMD_CMD06
#define  MMC_CMD_SWITCH_FUNC                    SD_CMD_CMD06

//                                                                 CMD7: Toggle card between stand-by & transfer states.
#define  SD_CMD_CMD07                           7u
#define  SD_CMD_SEL_DESEL_CARD                  SD_CMD_CMD07
#define  MMC_CMD_SEL_DESEL_CARD                 SD_CMD_CMD07

//                                                                 CMD8: Sends SD interface conditions.
#define  SD_CMD_CMD08                           8u
#define  SD_CMD_SEND_IF_COND                    SD_CMD_CMD08
#define  MMC_CMD_SEND_EXT_CSD                   SD_CMD_CMD08

//                                                                 CMD9: Addr'd card sends Card-Specific Data (CSD).
#define  SD_CMD_CMD09                           9u
#define  SD_CMD_SEND_CSD                        SD_CMD_CMD09
#define  MMC_CMD_SEND_CSD                       SD_CMD_CMD09

//                                                                 CMD10: Addr'd card sends Card Identification (CID).
#define  SD_CMD_CMD10                           10u
#define  SD_CMD_SEND_CID                        SD_CMD_CMD10
#define  MMC_CMD_SEND_CID                       SD_CMD_CMD10

//                                                                 CMD11: Rd data until stop (MMC) or volt switch (SD).
#define  SD_CMD_CMD11                           11u
#define  SD_CMD_VOLTAGE_SWITCH                  SD_CMD_CMD11
#define  MMC_CMD_READ_DAT_UNTIL_STOP            SD_CMD_CMD11

//                                                                 CMD12: Forces the card to stop transmission.
#define  SD_CMD_CMD12                           12u
#define  SD_CMD_STOP_TRANSMISSION               SD_CMD_CMD12
#define  MMC_CMD_STOP_TRANSMISSION              SD_CMD_CMD12

//                                                                 CMD13: Addressed card sends its status register.
#define  SD_CMD_CMD13                           13u
#define  SD_CMD_SEND_STATUS                     SD_CMD_CMD13
#define  MMC_CMD_SEND_STATUS                    SD_CMD_CMD13

//                                                                 CMD14: Read the reversed bus testing data pattern.
#define  SD_CMD_CMD14                           14u
#define  SD_CMD_BUSTEST_R                       SD_CMD_CMD14
#define  MMC_CMD_BUSTEST_R                      SD_CMD_CMD14

//                                                                 CMD15: Sends addressed card into inactive state.
#define  SD_CMD_CMD15                           15u
#define  SD_CMD_GO_INACTIVE_STATE               SD_CMD_CMD15
#define  MMC_CMD_GO_INACTIVE_STATE              SD_CMD_CMD15

//                                                                 CMD16: Sets the block length in block (std cap SD).
#define  SD_CMD_CMD16                           16u
#define  SD_CMD_SET_BLOCKLEN                    SD_CMD_CMD16
#define  FS_DEV_MMc_CMD_SET_BLOCKLEN            SD_CMD_CMD16

//                                                                 CMD17: Reads a block the size sel'd by SET_BLOCKLEN.
#define  SD_CMD_CMD17                           17u
#define  SD_CMD_READ_SINGLE_BLOCK               SD_CMD_CMD17
#define  MMC_CMD_READ_SINGLE_BLOCK              SD_CMD_CMD17

//                                                                 CMD18: Continuously xfers data blks until STOP_TRAN.
#define  SD_CMD_CMD18                           18u
#define  SD_CMD_READ_MULTIPLE_BLOCK             SD_CMD_CMD18
#define  MMC_CMD_READ_MULTIPLE_BLOCK            SD_CMD_CMD18

//                                                                 CMD19: Send the bus test data pattern to a card.
#define  SD_CMD_CMD19                           19u
#define  SD_CMD_BUSTEST_W                       SD_CMD_CMD19
#define  MMC_CMD_BUSTEST_W                      SD_CMD_CMD19

//                                                                 CMD20: Write a data stream until stop transmission.
#define  SD_CMD_CMD20                           20u
#define  SD_CMD_WRITE_DAT_UNTIL_STOP            SD_CMD_CMD20
#define  MMC_CMD_WRITE_DAT_UNTIL_STOP           SD_CMD_CMD20

//                                                                 CMD23: Define the number of blocks to be transferred.
#define  SD_CMD_CMD23                           23u
#define  SD_CMD_SET_BLOCK_COUNT                 SD_CMD_CMD23
#define  MMC_CMD_SET_BLOCK_COUNT                SD_CMD_CMD23

//                                                                 CMD24: Writes a block the size sel'd by SET_BLOCKLEN.
#define  SD_CMD_CMD24                           24u
#define  SD_CMD_WRITE_BLOCK                     SD_CMD_CMD24
#define  MMC_CMD_WRITE_BLOCK                    SD_CMD_CMD24

//                                                                 CMD25: Continuously writes data blks until STOP_TRAN.
#define  SD_CMD_CMD25                           25u
#define  SD_CMD_WRITE_MULTIPLE_BLOCK            SD_CMD_CMD25
#define  MMC_CMD_WRITE_MULTIPLE_BLOCK           SD_CMD_CMD25

//                                                                 CMD26: Programming of the CID.
#define  SD_CMD_CMD26                           26u
#define  SD_CMD_PROGRAM_CID                     SD_CMD_CMD26
#define  MMC_CMD_PROGRAM_CID                    SD_CMD_CMD26

//                                                                 CMD27: Programming of programmable bits of CSD.
#define  SD_CMD_CMD27                           27u
#define  SD_CMD_PROGRAM_CSD                     SD_CMD_CMD27
#define  MMC_CMD_PROGRAM_CSD                    SD_CMD_CMD27

//                                                                 CMD28: Sets the write protection bit of addr'd group.
#define  SD_CMD_CMD28                           28u
#define  SD_CMD_SET_WRITE_PROT                  SD_CMD_CMD28
#define  MMC_CMD_SET_WRITE_PROT                 SD_CMD_CMD28

//                                                                 CMD29: Clrs the write protection bit of addr'd group.
#define  SD_CMD_CMD29                           29u
#define  SD_CMD_CLR_WRITE_PROT                  SD_CMD_CMD29
#define  MMC_CMD_CLR_WRITE_PROT                 SD_CMD_CMD29

//                                                                 CMD30: Asks card to send status of wr protec bits.
#define  SD_CMD_CMD30                           30u
#define  SD_CMD_SEND_WRITE_PROT                 SD_CMD_CMD30
#define  MMC_CMD_SEND_WRITE_PROT                SD_CMD_CMD30

//                                                                 CMD32: Sets addr of 1st wr blk to be erased.
#define  SD_CMD_CMD32                           32u
#define  SD_CMD_ERASE_WR_BLK_START              SD_CMD_CMD32
#define  MMC_CMD_ERASE_WR_BLK_START             SD_CMD_CMD32

//                                                                 CMD33: Sets addr of last wr blk to be erased.
#define  SD_CMD_CMD33                           33u
#define  SD_CMD_ERASE_WR_BLK_END                SD_CMD_CMD33
#define  MMC_CMD_ERASE_WR_BLK_END               SD_CMD_CMD33

//                                                                 CMD35: Sets addr of first erase group within a range.
#define  SD_CMD_CMD35                           35u
#define  SD_CMD_ERASE_GROUP_START               SD_CMD_CMD35
#define  MMC_CMD_ERASE_GROUP_START              SD_CMD_CMD35

//                                                                 CMD36: Sets addr of last  erase group within a range.
#define  SD_CMD_CMD36                           36u
#define  SD_CMD_ERASE_GROUP_END                 SD_CMD_CMD36
#define  MMC_CMD_ERASE_GROUP_END                SD_CMD_CMD36

//                                                                 CMD38: Erases all prev sel'd wr blks.
#define  SD_CMD_CMD38                           38u
#define  SD_CMD_ERASE                           SD_CMD_CMD38
#define  MMC_CMD_ERASE                          SD_CMD_CMD38

//                                                                 CMD39: Write & read 8-bit data fields.
#define  SD_CMD_CMD39                           39u
#define  SD_CMD_FAST_IO                         SD_CMD_CMD39
#define  MMC_CMD_FAST_IO                        SD_CMD_CMD39

//                                                                 CMD40: Sets the system into interrupt mode.
#define  SD_CMD_CMD40                           40u
#define  SD_CMD_GO_IRQ_STATE                    SD_CMD_CMD40
#define  MMC_CMD_GO_IRQ_STATE                   SD_CMD_CMD40

//                                                                 CMD41: SD-Mem send operational conditions.
#define  SD_CMD_ACMD41                                   41u
#define  SD_CMD_INIT                            SD_CMD_ACMD41
#define  MMC_CMD_INIT                           SD_CMD_ACMD41

//                                                                 CMD42: Set/reset password or lock/unlock card.
#define  SD_CMD_CMD42                           42u
#define  SD_CMD_LOCK_UNLOCK                     SD_CMD_CMD42
#define  MMC_CMD_LOCK_UNLOCK                    SD_CMD_CMD42

//                                                                 CMD52 IO read/write byte using only CMD line.
#define  SD_CMD_CMD52                           52u
#define  SD_CMD_IO_RW_DIRECT                    SD_CMD_CMD52

//                                                                 CMD53: IO read/write buffers.
#define  SD_CMD_CMD53                           53u
#define  SD_CMD_IO_RW_EXTENDED                  SD_CMD_CMD53

//                                                                 CMD55: Indicates that next cmd is app cmd.
#define  SD_CMD_CMD55                           55u
#define  SD_CMD_APP_CMD                         SD_CMD_CMD55
#define  MMC_CMD_APP_CMD                        SD_CMD_CMD55

//                                                                 CMD56: Gets/sends data blk from app cmd.
#define  SD_CMD_CMD56                           56u
#define  SD_CMD_GEN_CMD                         SD_CMD_CMD56
#define  MMC_CMD_GEN_CMD                        SD_CMD_CMD56

//                                                                 CMD57: Reads OCR register of card.
#define  SD_CMD_CMD58                           58u
#define  SD_CMD_READ_OCR                        SD_CMD_CMD58
#define  MMC_CMD_READ_OCR                       SD_CMD_CMD58

//                                                                 CMD59: Turns the CRC option on or off.
#define  SD_CMD_CMD59                           59u
#define  SD_CMD_CRC_ON_OFF                      SD_CMD_CMD59
#define  MMC_CMD_CRC_ON_OFF                     SD_CMD_CMD59

/********************************************************************************************************
 *                                       OCR REGISTER BIT DEFINES
 *******************************************************************************************************/

#define  SD_OCR_ACCESS_MODE_SEC                 DEF_BIT_30      // Access mode: sector (for MMC only).

/********************************************************************************************************
 *                                               SD STATUS
 *******************************************************************************************************/

#define  SD_STATUS_IDLE                         DEF_BIT_00
#define  SD_STATUS_ERASE_RESET                  DEF_BIT_01
#define  SD_STATUS_ILLEGAL_CMD                  DEF_BIT_02

/********************************************************************************************************
 *                                       SD COMMAND ARGUMENTS DEFINES
 *******************************************************************************************************/

//                                                                 --------- SD CMD IO_SEND_OP_COND CMD5 ARG ----------
#define  SD_CMD5_ARG_S18R                   DEF_BIT_24
#define  SD_CMD5_ARG_OCR_MSK                DEF_BIT_FIELD(24u, 0u)

//                                                                 --------- SD CMD IO_RW_DIRECT (CMD52) ARG ----------
#define  SD_CMD52_ARG_WR_DATA_MSK           DEF_BIT_FIELD(8u, 0u)
#define  SD_CMD52_ARG_REG_ADDR_MSK          DEF_BIT_FIELD(17u, 9u)
#define  SD_CMD52_ARG_RAW_FLAG              DEF_BIT_27
#define  SD_CMD52_ARG_FNCT_NBR_MSK          DEF_BIT_FIELD(3u, 28u)
#define  SD_CMD52_ARG_RW_FLAG               DEF_BIT_31

//                                                                 -------- SD CMD IO_RW_EXTENDED (CMD53) ARG ---------
#define  SD_CMD53_ARG_BYTE_BLK_CNT_MSK      DEF_BIT_FIELD(9u, 0u)
#define  SD_CMD53_ARG_REG_ADDR_MSK          DEF_BIT_FIELD(17u, 9u)
#define  SD_CMD53_ARG_OP_CODE               DEF_BIT_26
#define  SD_CMD53_ARG_BLK_MODE              DEF_BIT_27
#define  SD_CMD53_ARG_FNCT_NBR_MSK          DEF_BIT_FIELD(3u, 28u)
#define  SD_CMD53_ARG_RW_FLAG               DEF_BIT_31

/********************************************************************************************************
 *                           CARD INTERFACE CONDITION (R7) RESPONSE BIT DEFINES
 *******************************************************************************************************/

#define  SD_R7_ACCEPTED_VOLT_27_36          DEF_BIT_00
#define  SD_R7_ACCEPTED_VOLT_LOW_RANGE      DEF_BIT_01

/********************************************************************************************************
 *                                       SD CARD CLOCK FREQUENCIES
 *******************************************************************************************************/

//                                                                 ------------------ 3.3V SIGNALING ------------------
#define  SD_CARD_CLK_FREQ_DFLT              25000000u
#define  SD_CARD_CLK_FREQ_HS                50000000u

//                                                                 ------------------ 1.8V SIGNALING ------------------
#define  SD_CARD_CLK_FREQ_SDR12             25000000u
#define  SD_CARD_CLK_FREQ_SDR25             50000000u
#define  SD_CARD_CLK_FREQ_SDR50             100000000u
#define  SD_CARD_CLK_FREQ_SDR104            208000000u
#define  SD_CARD_CLK_FREQ_DDR50             50000000u

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           FOREWORD DECLARATIONS
 *******************************************************************************************************/

typedef struct sd_io_fnct_drv_entry SD_IO_FNCT_DRV_ENTRY;

typedef struct sd_io_fnct SD_IO_FNCT;

/*
 ********************************************************************************************************
 *                                   R4 (IO_SEND_OP_COND) response data
 *******************************************************************************************************/

typedef struct sd_cmd_r4_data {
  CPU_INT32U  IO_OCR;                                           ///< Operation Conditions Register Value.
  CPU_BOOLEAN MemPresent;                                       ///< Indicates if memory present on card.
  CPU_INT08U  IO_FnctNbr;                                       ///< Number of IO functions available on card.
  CPU_BOOLEAN CardRdy;                                          ///< Indicates card ready to operate after init.
  CPU_BOOLEAN Switch18_Accepted;                                ///< Indicates card can operate with 1.8v signaling.
} SD_CMD_R4_DATA;

/*
 ********************************************************************************************************
 *                                   R7 (SEND_IF_COND) response data
 *******************************************************************************************************/

typedef struct sd_cmd_r7_data {
  CPU_INT08U ChkPatternEcho;                                    ///< Check pattern echo back.
  CPU_INT08U VoltageAccepted;                                   ///< Bitmap representing the voltage accepted by card.
} SD_CMD_R7_DATA;

/*
 ********************************************************************************************************
 *                                               SD event type
 *******************************************************************************************************/

typedef enum sd_event_type {
  SD_EVENT_TYPE_NONE,
  SD_EVENT_TYPE_CARD_DETECT,                                    ///< Card detected event.
  SD_EVENT_TYPE_CARD_REMOVE,                                    ///< Card removed  event.
  SD_EVENT_TYPE_CARD_IO_INT,                                    ///< SD IO function interrupt event.
  SD_EVENT_TYPE_DATA_XFER_CMPL                                  ///< Data transfer complete event.
} SD_EVENT_TYPE;

/*
 ********************************************************************************************************
 *                                           SD transfer type
 *******************************************************************************************************/

typedef enum sd_xfer_type {
  SD_XFER_TYPE_NONE,
  SD_XFER_TYPE_SYNC,                                            ///< Synchronous  (blocking)     data transfer.
  SD_XFER_TYPE_ASYNC                                            ///< Asynchronous (non-blocking) data transfer.
} SD_XFER_TYPE;

/*
 ********************************************************************************************************
 *                                           SD transport mode
 *******************************************************************************************************/

typedef enum sd_transport_mode {
  SD_TRANSPORT_MODE_NONE,
  SD_TRANSPORT_MODE_SD,                                         ///< Transport via SD mode.
  SD_TRANSPORT_MODE_SPI                                         ///< Transport via SPI mode.
} SD_TRANSPORT_MODE;

/*
 ********************************************************************************************************
 *                                               SD event data
 *******************************************************************************************************/

typedef struct sd_event {
  SD_BUS_HANDLE   BusHandle;                                    ///< Handle to SD bus.
  SD_EVENT_TYPE   Type;                                         ///< Type of event.
  RTOS_ERR        Err;                                          ///< Error variable associated to event.

  struct sd_event *NextPtr;                                     ///< Pointer to next event in list.
} SD_EVENT;

/*
 ********************************************************************************************************
 *                                           SD transfer data
 *******************************************************************************************************/

typedef struct sd_xfer_data {
  struct sd_xfer_data *NextPtr;                                 ///< Pointer to next asynchronous data transfer in list.

  SD_XFER_TYPE        Type;                                     ///< Type of SD transfer.
  SD_FNCT_HANDLE      FnctHandle;                               ///< Handle to SD function.
  CPU_BOOLEAN         DirIsRd;                                  ///< Flag that indicates if transfer is read.
  CPU_INT08U          CmdNbr;                                   ///< Command number.
  CPU_INT32U          CmdArg;                                   ///< Command arguments.
  CPU_INT08U          *BufPtr;                                  ///< Pointer to data buffer.
  CPU_INT32U          BlkQty;                                   ///< Quantity of blocks to transfer.
  CPU_INT32U          BlkLen;                                   ///< Size of blocks, in octets.
} SD_XFER_DATA;

/*
 ********************************************************************************************************
 *                                       SD async transfer data
 *******************************************************************************************************/

typedef struct sd_async_xfer_data {
  //                                                               ---------------- GENERIC XFER DATA -----------------
  struct sd_xfer_data *NextPtr;

  SD_XFER_TYPE        Type;
  SD_FNCT_HANDLE      FnctHandle;
  CPU_BOOLEAN         DirIsRd;
  CPU_INT08U          CmdNbr;
  CPU_INT32U          CmdArg;
  CPU_INT08U          *BufPtr;
  CPU_INT32U          BlkQty;
  CPU_INT32U          BlkLen;

  //                                                               ------------ ASYNC SPECIFIC XFER DATA --------------
  void                *AsyncDataPtr;                            ///< Pointer to caller's data.
  SD_ASYNC_FNCT       AsyncFnct;                                ///< Function to call once transfer completes.
} SD_ASYNC_XFER_DATA;

/*
 ********************************************************************************************************
 *                                           SD sync transfer data
 *******************************************************************************************************/

typedef struct sd_sync_xfer_data {
  //                                                               ---------------- GENERIC XFER DATA -----------------
  struct sd_xfer_data *NextPtr;

  SD_XFER_TYPE        Type;
  SD_FNCT_HANDLE      FnctHandle;
  CPU_BOOLEAN         DirIsRd;
  CPU_INT08U          CmdNbr;
  CPU_INT32U          CmdArg;
  CPU_INT08U          *BufPtr;
  CPU_INT32U          BlkQty;
  CPU_INT32U          BlkLen;

  //                                                               ------------- SYNC SPECIFIC XFER DATA --------------
  KAL_SEM_HANDLE      SemHandle;                                ///< Handle to semaphore used to notify completion.
  RTOS_ERR            *ErrPtr;                                  ///< Pointer to error variable.
} SD_SYNC_XFER_DATA;

/*
 ********************************************************************************************************
 *                                       SD module root structure
 *******************************************************************************************************/

typedef struct sd {
  SLIST_MEMBER         *BusHandleListHeadPtr;                   ///< Head pointer of bus handle list.

  KAL_TASK_HANDLE      CoreTaskHandle;                          ///< Handle to core task.
  KAL_SEM_HANDLE       CoreEventSemHandle;                      ///< Handle to semaphore to trigger bus event.
#if (IO_SD_CFG_CARD_DETECT_CORE_POLLING_EN == DEF_ENABLED)
  KAL_TMR_HANDLE       CoreCardPollingTmr;                      ///< Handle to core card polling timer.
#endif
  KAL_TASK_HANDLE      AsyncTaskHandle;                         ///< Handle to async task.
  KAL_SEM_HANDLE       AsyncEventSemHandle;                     ///< Handle to semaphore to trigger data event.

  MEM_DYN_POOL         EventPool;                               ///< Pool of SD event structures.
  SD_EVENT             *CoreEventHeadPtr;                       ///< Head of SD core event list.
  SD_EVENT             *CoreEventTailPtr;                       ///< Tail of SD core event list.
  SD_EVENT             *AsyncEventHeadPtr;                      ///< Head of SD async event list.
  SD_EVENT             *AsyncEventTailPtr;                      ///< Tail of SD async event list.
  SD_EVENT             *PreAllocEventHeadPtr;                   ///< Head of pre-allocated SD event list.

  MEM_DYN_POOL         XferDataPool;                            ///< Pool of transfers data.

  CPU_INT32U           LockAcquireTimeoutMs;                    ///< Timeout used for lock acquiring.

  SD_IO_FNCT_DRV_ENTRY *IO_FnctDrvEntryHeadPtr;                 ///< Head of registered IO function drivers list.
  MEM_DYN_POOL         IO_FnctPool;                             ///< Pool of IO functions strcture.
} SD;

/*
 ********************************************************************************************************
 *                                               SD bus handle
 *******************************************************************************************************/

struct sd_bus_handle {
  //                                                               Node for SD bus handle list.
  SLIST_MEMBER         ListNode;
  //                                                               Name of SD bus controller.
  const CPU_CHAR       *Name;
  //                                                               Bitmap that represents capabilities.
  SD_CAPABILITIES      Capabilities;
  //                                                               Indicates the transport mode of bus.
  SD_TRANSPORT_MODE    TransportMode;
  //                                                               Pointer to bus driver API.
  const SD_BUS_DRV_API *BusDrvApiPtr;
  //                                                               Pointer to bus driver data.
  void                 *BusDrvDataPtr;
  //                                                               Handle to bus lock.
  KAL_LOCK_HANDLE      BusLockHandle;

  //                                                               Event structure associated to this bus.
  SD_EVENT             BusEvent;
#if (IO_SD_CFG_CARD_DETECT_CORE_POLLING_EN == DEF_ENABLED)
  //                                                               Flag that indicates if bus has started.
  CPU_BOOLEAN          BusStart;
#endif
  //                                                               Type of SD card.
  SD_CARDTYPE          CardType;
  //                                                               Flag that indicates if a card is present.
  CPU_BOOLEAN          CardPresent;
  //                                                               Flag that indicates if card is enabled.
  CPU_BOOLEAN          CardEn;
  //                                                               Flag that Indicates if card can operate with
  //                                                               1.8v signaling.
  CPU_BOOLEAN          CardSupports1_8v;
  //                                                               Card detect mode.
  SD_CARD_DETECT_MODE  CardDetectMode;
  //                                                               Pointer to head of transfer complete list.
  SD_XFER_DATA         *XferListHeadPtr;
  //                                                               Pointer to tail of transfer complete list.
  SD_XFER_DATA         *XferListTailPtr;

  //                                                               Table of IO functions.
  SD_IO_FNCT           *IO_FnctTbl[SD_IO_FNCT_NBR_MAX];
  //                                                               Number of IO functions present.
  CPU_INT08U           IO_FnctNbr;
  //                                                               Flag that indicates if IO block mode operations are
  //                                                               supported on this card.
  CPU_INT08U           IO_BlkOperSupported;
  //                                                               Version of SD IO specification followed by the card.
  CPU_INT08U           IO_SpecVer;
  //                                                               Version of CCCR register format.
  CPU_INT08U           IO_CCCR_Ver;
  //                                                               Pointer to CIS.
  CPU_INT32U           IO_CIS_ComPointer;
  //                                                               Maximum block size for function 0.
  CPU_INT16U           IO_Fnct0MaxBlkSize;
  //                                                               Current block size for function 0.
  CPU_INT16U           IO_Fnct0BlkSize;
};

/********************************************************************************************************
 *                                           SD CARD STATUS
 *******************************************************************************************************/

typedef CPU_INT08U SD_STATUS;

/*
 ********************************************************************************************************
 *                                           SD bus driver API
 *******************************************************************************************************/

struct sd_bus_drv_api {
  //                                                               Initializes bus driver.
  void *(*Init)                (SD_BUS_HANDLE bus_handle,
                                void          *p_drv_info,
                                RTOS_ERR      *p_err);

  //                                                               Initializes SD Host controller.
  SD_CAPABILITIES (*InitHW)              (void     *p_bus_drv_data,
                                          RTOS_ERR *p_err);

  //                                                               Starts bus driver.
  void (*Start)               (void     *p_bus_drv_data,
                               RTOS_ERR *p_err);

  //                                                               Stops bus driver.
  void (*Stop)                (void     *p_bus_drv_data,
                               RTOS_ERR *p_err);

  //                                                               Gets required buffer alignment.
  CPU_SIZE_T (*AlignReqGet)         (void     *p_bus_drv_data,
                                     RTOS_ERR *p_err);

  //                                                               Sets clock frequency.
  void (*ClkFreqSet)          (void       *p_bus_drv_data,
                               CPU_INT32U freq,
                               RTOS_ERR   *p_err);

  //                                                               Sets bus supply voltage.
  void (*BusSupplyVoltSet)    (void             *p_bus_drv_data,
                               SD_CARD_BUS_VOLT volt,
                               RTOS_ERR         *p_err);

  //                                                               Sets bus signaling voltage.
  void (*BusSignalVoltInit)   (void     *p_bus_drv_data,
                               RTOS_ERR *p_err);

  //                                                               Sets bus width.
  void (*BusWidthSet)         (void       *p_bus_drv_data,
                               CPU_INT08U width,
                               RTOS_ERR   *p_err);

  //                                                               Updates bus signal voltage.
  void (*BusSignalVoltSwitch) (void     *p_bus_drv_data,
                               RTOS_ERR *p_err);

  //                                                               Executes command that receives a R1 response.
  SD_STATUS (*CmdR1Exec)           (void        *p_bus_drv_data,
                                    SD_CARDTYPE card_type,
                                    CPU_INT08U  cmd_nbr,
                                    CPU_INT32U  arg,
                                    RTOS_ERR    *p_err);

  //                                                               Executes CMD3 (SEND_RELATIVE_ADDR). SD mode only.
  SD_STATUS (*Cmd3Exec)            (void        *p_bus_drv_data,
                                    SD_CARDTYPE card_type,
                                    RTOS_ERR    *p_err);

  //                                                               Executes CMD7 (SELECT/DESELECT_CARD). SD mode only.
  SD_STATUS (*Cmd7Exec)            (void        *p_bus_drv_data,
                                    SD_CARDTYPE card_type,
                                    CPU_BOOLEAN select,
                                    RTOS_ERR    *p_err);

  //                                                               Executes command that receives a R3 response.
  void (*CmdR3Exec)           (void        *p_bus_drv_data,
                               SD_CARDTYPE card_type,
                               CPU_INT08U  cmd_nbr,
                               CPU_INT32U  arg,
                               CPU_INT32U  *p_r3_resp,
                               RTOS_ERR    *p_err);

  //                                                               Executes command that receives a R4 response.
  void (*CmdR4Exec)           (void           *p_bus_drv_data,
                               SD_CARDTYPE    card_type,
                               CPU_INT08U     cmd_nbr,
                               CPU_INT32U     arg,
                               SD_CMD_R4_DATA *p_r4_resp,
                               RTOS_ERR       *p_err);

  //                                                               Executes command that receives a R5 response.
  SD_STATUS (*CmdR5Exec)           (void        *p_bus_drv_data,
                                    SD_CARDTYPE card_type,
                                    CPU_INT08U  cmd_nbr,
                                    CPU_INT32U  arg,
                                    CPU_INT08U  *p_rw_data,
                                    RTOS_ERR    *p_err);

  //                                                               Executes command that receives a R7 response.
  void (*CmdR7Exec)           (void           *bus_drv_data,
                               SD_CARDTYPE    card_type,
                               CPU_INT08U     cmd_nbr,
                               CPU_INT32U     arg,
                               SD_CMD_R7_DATA *p_r7_resp,
                               RTOS_ERR       *p_err);

  //                                                               Executes command that receives no response.
  void (*CmdNoRespExec)       (void        *p_bus_drv_data,
                               SD_CARDTYPE card_type,
                               CPU_INT08U  cmd_nbr,
                               CPU_INT32U  arg,
                               RTOS_ERR    *p_err);

  //                                                               Prepare buffer for upcoming data transfer command.
  void (*DataXferPrepare)     (void        *p_bus_drv_data,
                               SD_CARDTYPE card_type,
                               CPU_INT08U  *p_buf,
                               CPU_INT32U  blk_nbr,
                               CPU_INT32U  blk_len,
                               CPU_BOOLEAN dir_is_rd,
                               RTOS_ERR    *p_err);

  //                                                               Complete data transfer from previous command.
  void (*DataXferCmpl)        (void        *p_bus_drv_data,
                               SD_CARDTYPE card_type,
                               CPU_INT08U  *p_buf,
                               CPU_INT32U  blk_nbr,
                               CPU_INT32U  blk_len,
                               CPU_BOOLEAN dir_is_rd,
                               RTOS_ERR    *p_err);

  //                                                               Reads data.
  void (*DataRd)              (void        *bus_drv_data,
                               SD_CARDTYPE card_type,
                               CPU_INT08U  *p_buf,
                               CPU_INT32U  buf_len,
                               RTOS_ERR    *p_err);

  //                                                               Writes data.
  void (*DataWr)              (void        *p_bus_drv_data,
                               SD_CARDTYPE card_type,
                               CPU_INT08U  *p_buf,
                               CPU_INT32U  buf_len,
                               RTOS_ERR    *p_err);

  //                                                               Enables/Disables the Card Interrupt.
  void (*CardIntEnDis)        (void        *p_bus_drv_data,
                               CPU_BOOLEAN enable);
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void SD_Init(RTOS_ERR *p_err);

void SD_IO_Init(RTOS_ERR *p_err);

void SD_IO_CardInit(SD_BUS_HANDLE bus_handle,
                    RTOS_ERR      *p_err);

void SD_IO_CardFnctsInit(SD_BUS_HANDLE bus_handle,
                         RTOS_ERR      *p_err);

void SD_IO_CardRem(SD_BUS_HANDLE bus_handle,
                   RTOS_ERR      *p_err);

SD_CAP_BITMAP SD_IO_CardBusWidthCapGet(SD_BUS_HANDLE bus_handle,
                                       RTOS_ERR      *p_err);

void SD_IO_CardBusWidthSet(SD_BUS_HANDLE bus_handle,
                           CPU_INT08U    bus_width_supported,
                           RTOS_ERR      *p_err);

void SD_IO_CardIntHandle(SD_BUS_HANDLE bus_handle,
                         RTOS_ERR      *p_err);

void SD_IO_CardIntMasterEnDis(SD_BUS_HANDLE bus_handle,
                              CPU_BOOLEAN   enable,
                              RTOS_ERR      *p_err);

void SD_IO_CardIntEvent(SD_BUS_HANDLE bus_handle);

SD_STATUS SD_IO_DataXferSubmit(SD_BUS_HANDLE bus_handle,
                               CPU_BOOLEAN   dir_is_rd,
                               CPU_INT32U    cmd_arg,
                               CPU_INT08U    *p_buf,
                               CPU_INT16U    blk_qty,
                               CPU_INT16U    blk_len,
                               RTOS_ERR      *p_err);

void SD_BusCardDetectEvent(SD_BUS_HANDLE bus_handle);

void SD_BusCardRemoveEvent(SD_BUS_HANDLE bus_handle);

void SD_BusLock(SD_BUS_HANDLE bus_handle,
                RTOS_ERR      *p_err);

void SD_BusUnlock(SD_BUS_HANDLE bus_handle);

void SD_CoreEventPost(SD_BUS_HANDLE bus_handle,
                      SD_EVENT_TYPE event_type,
                      RTOS_ERR      err);

void SD_AsyncEventPost(SD_BUS_HANDLE bus_handle,
                       SD_EVENT_TYPE event_type,
                       RTOS_ERR      err);

void SD_EventPreAlloc(RTOS_ERR *p_err);

SD_EVENT *SD_PreAllocEventGet(void);

void SD_PreAllocEventFree(void);

void SD_BusSyncXferExec(SD_BUS_HANDLE  bus_handle,
                        SD_FNCT_HANDLE fnct_handle,
                        CPU_INT08U     cmd_nbr,
                        CPU_INT32U     cmd_arg,
                        CPU_BOOLEAN    dir_is_rd,
                        CPU_INT08U     *p_buf,
                        CPU_INT32U     blk_qty,
                        CPU_INT32U     blk_len,
                        CPU_INT32U     timeout,
                        RTOS_ERR       *p_err);

void SD_BusAsyncXferAdd(SD_BUS_HANDLE  bus_handle,
                        SD_FNCT_HANDLE fnct_handle,
                        SD_ASYNC_FNCT  async_fnct,
                        CPU_INT08U     cmd_nbr,
                        CPU_INT32U     cmd_arg,
                        CPU_BOOLEAN    dir_is_rd,
                        CPU_INT08U     *p_buf,
                        CPU_INT32U     blk_qty,
                        CPU_INT32U     blk_len,
                        void           *p_async_data,
                        RTOS_ERR       *p_err);
#endif
