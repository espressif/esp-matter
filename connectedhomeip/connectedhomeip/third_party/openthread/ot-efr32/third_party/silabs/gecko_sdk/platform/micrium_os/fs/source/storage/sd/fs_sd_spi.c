/***************************************************************************//**
 * @file
 * @brief File System - Sd Spi Media Driver
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

/********************************************************************************************************
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
 * @note     (3) High capacity MMCPlus cards (>2GB) are not supported by the SPI driver.
 *
 * @note     (4) MMC (eMMC) v4.3 and superior dropped support for the SPI interface and as such cannot
 *               be used with this driver. SPI support is optional for MMCplus and MMCmobile cards.
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include <rtos_description.h>

#if (defined(RTOS_MODULE_FS_STORAGE_SD_SPI_AVAIL))

#if (!defined(RTOS_MODULE_FS_AVAIL))

#error SD module requires File System Storage module. Make sure it is part of your project and that \
  RTOS_MODULE_FS_AVAIL is defined in rtos_description.h.

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#define    FS_DEV_SD_SPI_MODULE

#include  <io/include/serial.h>
#include  <io/include/spi.h>
#include  <io/include/spi_slave.h>

#include  <fs/source/storage/sd/fs_sd_priv.h>
#include  <fs/include/fs_sd_spi.h>
#include  <fs/include/fs_blk_dev.h>
#include  <fs/source/shared/fs_utils_priv.h>

#include  <common/source/rtos/rtos_utils_priv.h>

#include  <common/include/lib_ascii.h>
#include  <common/include/lib_mem.h>
#include  <common/include/lib_str.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                    (FS, DRV, SD)
#define  RTOS_MODULE_CUR                                RTOS_CFG_MODULE_FS

#define  FS_DEV_SD_SPI_XFER_TIMEOUT                     5000u
#define  FS_DEV_SD_SPI_SLAVE_SEL_TIMEOUT                   0u

#define  FS_DEV_SD_SPI_CMD_TIMEOUT                       128u

#define  FS_DEV_SD_SPI_RESP_EMPTY                       0xFFu

//                                                                 ---------- DATA RESPONSE TOKENS (7.3.3.1) ----------
#define  FS_DEV_SD_SPI_TOKEN_RESP_ACCEPTED              0x05u
#define  FS_DEV_SD_SPI_TOKEN_RESP_CRC_REJECTED          0x0Bu
#define  FS_DEV_SD_SPI_TOKEN_RESP_WR_REJECTED           0x0Du
#define  FS_DEV_SD_SPI_TOKEN_RESP_MASK                  0x0Fu

//                                                                 -------------- BLOCK TOKENS (7.3.3.2) --------------
#define  FS_DEV_SD_SPI_TOKEN_START_BLK_MULT             0xFCu
#define  FS_DEV_SD_SPI_TOKEN_STOP_TRAN                  0xFDu
#define  FS_DEV_SD_SPI_TOKEN_START_BLK                  0xFEu

//                                                                 ------------ DATA ERROR TOKENS (7.3.3.3) -----------
#define  FS_DEV_SD_SPI_TOKEN_ERR_ERROR                  0x01u
#define  FS_DEV_SD_SPI_TOKEN_ERR_CC_ERROR               0x02u
#define  FS_DEV_SD_SPI_TOKEN_ERR_CARD_ECC_ERROR         0x04u
#define  FS_DEV_SD_SPI_TOKEN_ERR_OUT_OF_RANGE           0x08u

/********************************************************************************************************
 *                                       R1 RESPONSE BIT DEFINES
 *
 * Note(s) : (1) See [Ref 1], Figure 7-9.
 *******************************************************************************************************/

#define  FS_DEV_SD_SPI_R1_NONE                   DEF_BIT_NONE   // In ready state.
#define  FS_DEV_SD_SPI_R1_IN_IDLE_STATE          DEF_BIT_00     // In idle state.
#define  FS_DEV_SD_SPI_R1_ERASE_RESET            DEF_BIT_01     // Erase sequence clr'd before executing.
#define  FS_DEV_SD_SPI_R1_ILLEGAL_COMMAND        DEF_BIT_02     // Illegal command code detected.
#define  FS_DEV_SD_SPI_R1_COM_CRC_ERROR          DEF_BIT_03     // CRC check of last command failed.
#define  FS_DEV_SD_SPI_R1_ERASE_SEQUENCE_ERROR   DEF_BIT_04     // Error in sequence of erase commands occurred.
#define  FS_DEV_SD_SPI_R1_ADDRESS_ERROR          DEF_BIT_05     // Misaligned address that did not match block length.
#define  FS_DEV_SD_SPI_R1_PARAMETER_ERROR        DEF_BIT_06     // Command's argument outside allowed range for card.

/********************************************************************************************************
 *                                       R2 RESPONSE BIT DEFINES
 *
 * Note(s) : (1) See [Ref 1], Figure 7-10.
 *******************************************************************************************************/

#define  FS_DEV_SD_SPI_R2_CARD_IS_LOCKED         DEF_BIT_00     // Card is locked.
#define  FS_DEV_SD_SPI_R2_WP_ERASE_SKIP          DEF_BIT_01     // Attempt to erase a write protected sector.
#define  FS_DEV_SD_SPI_R2_LOCK_CMD_FAILED        DEF_BIT_01     // Password error during lock/unlock operation.
#define  FS_DEV_SD_SPI_R2_ERROR                  DEF_BIT_02     // General or unknown error during the operation.
#define  FS_DEV_SD_SPI_R2_CC_ERROR               DEF_BIT_03     // Internal card controller error.
#define  FS_DEV_SD_SPI_R2_CARD_ECC_FAILED        DEF_BIT_04     // Card internal ECC applied, failed to correct data.
#define  FS_DEV_SD_SPI_R2_WP_VIOLATION           DEF_BIT_05     // Tried to write a write-protected block.
#define  FS_DEV_SD_SPI_R2_ERASE_PARAM            DEF_BIT_06     // Invalid selection for erase, sectors or groups.
#define  FS_DEV_SD_SPI_R2_OUT_OF_RANGE           DEF_BIT_07     // Parameter out of range.
#define  FS_DEV_SD_SPI_R2_CSD_OVERWRITE          DEF_BIT_07     // CSD overwritten.

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           SD SPI DATA DATA TYPE
 *******************************************************************************************************/

typedef struct fs_sd_spi {
  FS_SD            SD;
  SPI_SLAVE_HANDLE SpiHandle;
  SPI_BUS_HANDLE   SpiBusHandle;

#if (FS_STORAGE_CFG_CTR_STAT_EN == DEF_ENABLED)
  CPU_INT16U       StatRdCtr;
  CPU_INT16U       StatWrCtr;
#endif

#if (FS_STORAGE_CFG_CTR_ERR_EN == DEF_ENABLED)
  CPU_INT16U ErrRdCtr;
  CPU_INT16U ErrWrCtr;

  CPU_INT16U ErrCmdRespCtr;
  CPU_INT16U ErrCmdRespTimeoutCtr;
  CPU_INT16U ErrCmdRespEraseResetCtr;
  CPU_INT16U ErrCmdRespIllegalCmdCtr;
  CPU_INT16U ErrCmdRespCommCRC_Ctr;
  CPU_INT16U ErrCmdRespEraseSeqCtr;
  CPU_INT16U ErrCmdRespAddrCtr;
  CPU_INT16U ErrCmdRespParamCtr;
#endif
} FS_SD_SPI;

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL MACRO'S
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       SPI ENTER & EXIT MACRO'S
 *
 * Note(s) : (1) According to [Ref 2], Section 9.49, eight clocks (i.e., one byte 'clocked') must be
 *               provided after the reception of a response, reading the final byte of a data block
 *               during a read data transaction or receiving the status at the end of a write data
 *               transaction.
 *******************************************************************************************************/

#define  FS_SD_SPI_WITH(p_sd_spi, p_err)    if (1) {                                         \
    SPI_SlaveSel(p_sd_spi->SpiHandle, FS_DEV_SD_SPI_SLAVE_SEL_TIMEOUT, SPI_OPT_NONE, p_err); \
    if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {                                        \
      FS_SD_SPI_ClksWait(p_sd_spi, 1u, p_err);                                               \
      if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {                                      \
        goto PP_UNIQUE_LABEL(spi_slave_with_enter);                                          \
      }                                                                                      \
    }                                                                                        \
} else PP_UNIQUE_LABEL(spi_slave_with_enter) :                                               \
  ON_EXIT {                                                                                  \
    FS_SD_SPI_ClksWait(p_sd_spi, 1u, p_err);                                                 \
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {                                        \
      RTOS_ERR _err;                                                                         \
      RTOS_ERR_SET(_err, RTOS_ERR_NONE);                                                     \
      SPI_SlaveDesel(p_sd_spi->SpiHandle, &_err);                                            \
    } else {                                                                                 \
      SPI_SlaveDesel(p_sd_spi->SpiHandle, p_err);                                            \
    }                                                                                        \
} WITH

/********************************************************************************************************
 *                                           COUNTER MACRO'S
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_CTR_STAT_EN == DEF_ENABLED)                 // ----------------- STAT CTR MACRO'S -----------------

#define  FS_DEV_SD_SPI_STAT_RD_CTR_INC(p_sd_spi)                    {  FS_BLK_DEV_CTR_STAT_INC((p_sd_spi)->StatRdCtr);                }
#define  FS_DEV_SD_SPI_STAT_WR_CTR_INC(p_sd_spi)                    {  FS_BLK_DEV_CTR_STAT_INC((p_sd_spi)->StatWrCtr);                }

#define  FS_DEV_SD_SPI_STAT_RD_CTR_ADD(p_sd_spi, val)               {  FS_BLK_DEV_CTR_STAT_ADD((p_sd_spi)->StatRdCtr, (CPU_INT16U)(val)); }
#define  FS_DEV_SD_SPI_STAT_WR_CTR_ADD(p_sd_spi, val)               {  FS_BLK_DEV_CTR_STAT_ADD((p_sd_spi)->StatWrCtr, (CPU_INT16U)(val)); }

#else

#define  FS_DEV_SD_SPI_STAT_RD_CTR_INC(p_sd_spi)
#define  FS_DEV_SD_SPI_STAT_WR_CTR_INC(p_sd_spi)

#define  FS_DEV_SD_SPI_STAT_RD_CTR_ADD(p_sd_spi, val)
#define  FS_DEV_SD_SPI_STAT_WR_CTR_ADD(p_sd_spi, val)

#endif

#if (FS_STORAGE_CFG_CTR_ERR_EN == DEF_ENABLED)                  // ------------------ ERR CTR MACRO'S -----------------

#define  FS_DEV_SD_SPI_ERR_RD_CTR_INC(p_sd_spi)                     {  FS_BLK_DEV_CTR_ERR_INC((p_sd_spi)->ErrRdCtr);                  }
#define  FS_DEV_SD_SPI_ERR_WR_CTR_INC(p_sd_spi)                     {  FS_BLK_DEV_CTR_ERR_INC((p_sd_spi)->ErrWrCtr);                  }

#define  FS_DEV_SD_SPI_ERR_CMD_RESP_TIMEOUT_CTR_INC(p_sd_spi)       {  FS_BLK_DEV_CTR_ERR_INC((p_sd_spi)->ErrCmdRespTimeoutCtr);      }
#define  FS_DEV_SD_SPI_ERR_CMD_RESP_RESET_ERASE_CTR_INC(p_sd_spi)   {  FS_BLK_DEV_CTR_ERR_INC((p_sd_spi)->ErrCmdRespEraseResetCtr);   }
#define  FS_DEV_SD_SPI_ERR_CMD_RESP_ILLEGAL_CMD_CTR_INC(p_sd_spi)   {  FS_BLK_DEV_CTR_ERR_INC((p_sd_spi)->ErrCmdRespIllegalCmdCtr);   }
#define  FS_DEV_SD_SPI_ERR_CMD_RESP_COMM_CRC_INC(p_sd_spi)          {  FS_BLK_DEV_CTR_ERR_INC((p_sd_spi)->ErrCmdRespCommCRC_Ctr);     }
#define  FS_DEV_SD_SPI_ERR_CMD_RESP_SEQ_ERASE_CTR_INC(p_sd_spi)     {  FS_BLK_DEV_CTR_ERR_INC((p_sd_spi)->ErrCmdRespEraseSeqCtr);     }
#define  FS_DEV_SD_SPI_ERR_CMD_RESP_ADDR_CTR_INC(p_sd_spi)          {  FS_BLK_DEV_CTR_ERR_INC((p_sd_spi)->ErrCmdRespAddrCtr);         }
#define  FS_DEV_SD_SPI_ERR_CMD_RESP_PARAM_CTR_INC(p_sd_spi)         {  FS_BLK_DEV_CTR_ERR_INC((p_sd_spi)->ErrCmdRespParamCtr);        }

#else

#define  FS_DEV_SD_SPI_ERR_RD_CTR_INC(p_sd_spi)
#define  FS_DEV_SD_SPI_ERR_WR_CTR_INC(p_sd_spi)

#define  FS_DEV_SD_SPI_ERR_CMD_RESP_TIMEOUT_CTR_INC(p_sd_spi)
#define  FS_DEV_SD_SPI_ERR_CMD_RESP_RESET_ERASE_CTR_INC(p_sd_spi)
#define  FS_DEV_SD_SPI_ERR_CMD_RESP_ILLEGAL_CMD_CTR_INC(p_sd_spi)
#define  FS_DEV_SD_SPI_ERR_CMD_RESP_COMM_CRC_INC(p_sd_spi)
#define  FS_DEV_SD_SPI_ERR_CMD_RESP_SEQ_ERASE_CTR_INC(p_sd_spi)
#define  FS_DEV_SD_SPI_ERR_CMD_RESP_ADDR_CTR_INC(p_sd_spi)
#define  FS_DEV_SD_SPI_ERR_CMD_RESP_PARAM_CTR_INC(p_sd_spi)

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           DEV INTERFACE FNCTS
 *******************************************************************************************************/

static void FS_SD_SPI_OpenInternal(FS_SD    *p_sd,
                                   RTOS_ERR *p_err);

static void FS_SD_SPI_CloseInternal(FS_SD    *p_sd,
                                    RTOS_ERR *p_err);

static void FS_SD_SPI_InfoGet(FS_SD      *p_sd,
                              FS_SD_INFO *p_info);

static void FS_SD_SPI_CID_Rd(FS_SD      *p_sd,
                             CPU_INT08U *p_dest,
                             RTOS_ERR   *p_err);

static void FS_SD_SPI_CSD_Rd(FS_SD      *p_sd,
                             CPU_INT08U *p_dest,
                             RTOS_ERR   *p_err);

static FS_MEDIA *FS_SD_SPI_MediaAdd(const FS_MEDIA_PM_ITEM *p_pm_item,
                                    RTOS_ERR               *p_err);

static void FS_SD_SPI_MediaRem(FS_MEDIA *p_media,
                               RTOS_ERR *p_err);

static FS_BLK_DEV *FS_SD_SPI_BlkDevAdd(FS_MEDIA *p_media,
                                       RTOS_ERR *p_err);

static void FS_SD_SPI_BlkDevRem(FS_BLK_DEV *p_blk_dev,
                                RTOS_ERR   *p_err);

static void FS_SD_SPI_Open(FS_BLK_DEV *p_blk_dev,
                           RTOS_ERR   *p_err);

static void FS_SD_SPI_Close(FS_BLK_DEV *p_blk_dev,
                            RTOS_ERR   *p_err);

static void FS_SD_SPI_Rd(FS_BLK_DEV *p_blk_dev,
                         void       *p_dest,
                         FS_LB_NBR  start,
                         FS_LB_QTY  cnt,
                         RTOS_ERR   *p_err);

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_SD_SPI_Wr(FS_BLK_DEV *p_blk_dev,
                         void       *p_src,
                         FS_LB_NBR  start,
                         FS_LB_QTY  cnt,
                         RTOS_ERR   *p_err);

static void FS_SD_SPI_Sync(FS_BLK_DEV *p_blk_dev,
                           RTOS_ERR   *p_err);

static void FS_SD_SPI_Trim(FS_BLK_DEV *p_blk_dev,
                           FS_LB_NBR  lb_nbr,
                           RTOS_ERR   *p_err);
#endif

static void FS_SD_SPI_Query(FS_BLK_DEV      *p_blk_dev,
                            FS_BLK_DEV_INFO *p_info,
                            RTOS_ERR        *p_err);

static CPU_BOOLEAN FS_SD_SPI_IsConn(FS_MEDIA *p_media);

static CPU_SIZE_T FS_SD_SPI_AlignReqGet(FS_MEDIA *p_media,
                                        RTOS_ERR *p_err);

/********************************************************************************************************
   LOCAL FNCTS
 *******************************************************************************************************/

static CPU_INT08U FS_SD_SPI_MakeRdy(FS_SD_SPI *p_sd_spi,
                                    RTOS_ERR  *p_err);

//                                                                 Perform command & read resp byte.
static CPU_INT08U FS_SD_SPI_Cmd(FS_SD_SPI  *p_sd_spi,
                                CPU_INT08U cmd,
                                CPU_INT32U arg,
                                RTOS_ERR   *p_err);

//                                                                 Perform command & get R1 resp.
static CPU_INT08U FS_SD_SPI_CmdR1(FS_SD_SPI  *p_sd_spi,
                                  CPU_INT08U cmd,
                                  CPU_INT32U arg,
                                  RTOS_ERR   *p_err);

//                                                                 Perform command & get R3/R7 resp.
static CPU_INT08U FS_SD_SPI_CmdR3_7(FS_SD_SPI  *p_sd_spi,
                                    CPU_INT08U cmd,
                                    CPU_INT32U arg,
                                    CPU_INT08U resp[],
                                    RTOS_ERR   *p_err);

//                                                                 Perform ACMD & get R1 resp.
static CPU_INT08U FS_SD_SPI_AppCmdR1(FS_SD_SPI  *p_sd_spi,
                                     CPU_INT08U acmd,
                                     CPU_INT32U arg,
                                     RTOS_ERR   *p_err);

//                                                                 Exec cmd & rd data blk.
static void FS_SD_SPI_DataRd(FS_SD_SPI  *p_sd_spi,
                             CPU_INT08U cmd,
                             CPU_INT32U arg,
                             CPU_INT08U *p_dest,
                             CPU_INT32U size,
                             RTOS_ERR   *p_err);

//                                                                 Exec cmd & rd mult data blks.
static void FS_SD_SPI_DataMultiRd(FS_SD_SPI  *p_sd_spi,
                                  CPU_INT08U cmd,
                                  CPU_INT32U arg,
                                  CPU_INT08U *p_dest,
                                  CPU_INT32U size,
                                  CPU_INT32U cnt,
                                  RTOS_ERR   *p_err);

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
//                                                                 Exec cmd & wr data blk.
static void FS_SD_SPI_DataWr(FS_SD_SPI  *p_sd_spi,
                             CPU_INT08U cmd,
                             CPU_INT32U arg,
                             CPU_INT08U *p_src,
                             CPU_INT32U size,
                             RTOS_ERR   *p_err);

//                                                                 Exec cmd & wr mult data blks.
static void FS_SD_SPI_DataMultiWr(FS_SD_SPI  *p_sd_spi,
                                  CPU_INT08U cmd,
                                  CPU_INT32U arg,
                                  CPU_INT08U *p_src,
                                  CPU_INT32U size,
                                  CPU_INT32U cnt,
                                  RTOS_ERR   *p_err);
#endif

//                                                                 Get CID reg.
static void FS_SD_SPI_CID_Send(FS_SD_SPI  *p_sd_spi,
                               CPU_INT08U *p_dest,
                               RTOS_ERR   *p_err);

//                                                                 Get CSD reg.
static void FS_SD_SPI_CSD_Send(FS_SD_SPI  *p_sd_spi,
                               CPU_INT08U *p_dest,
                               RTOS_ERR   *p_err);

//                                                                 Wait certain number of clocks.
static void FS_SD_SPI_ClksWait(FS_SD_SPI  *p_sd_spi,
                               CPU_INT08U nbr_clks,
                               RTOS_ERR   *p_err);

//                                                                 Wait for card to rtn start token.
static CPU_INT08U FS_SD_SPI_WaitForStart(FS_SD_SPI *p_sd_spi);

//                                                                 Wait while card rtns busy token.
static CPU_INT08U FS_SD_SPI_WaitWhileBusy(FS_SD_SPI *p_sd_spi);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INTERFACE STRUCTURE
 ********************************************************************************************************
 *******************************************************************************************************/

const FS_MEDIA_API FS_SD_SPI_MediaApi = {
  .Add = FS_SD_SPI_MediaAdd,
  .Rem = FS_SD_SPI_MediaRem,
  .AlignReqGet = FS_SD_SPI_AlignReqGet,
  .IsConn = FS_SD_SPI_IsConn
};

const FS_BLK_DEV_API FS_SD_SPI_BlkDevApi = {
  .Add = FS_SD_SPI_BlkDevAdd,
  .Rem = FS_SD_SPI_BlkDevRem,
  .Open = FS_SD_SPI_Open,
  .Close = FS_SD_SPI_Close,
  .Rd = FS_SD_SPI_Rd,
#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
  .Wr = FS_SD_SPI_Wr,
  .Sync = FS_SD_SPI_Sync,
  .Trim = FS_SD_SPI_Trim,
#endif
  .Query = FS_SD_SPI_Query
};

static const FS_SD_BUS_API FS_SD_SPI_BusApi = {
  .Open = FS_SD_SPI_OpenInternal,
  .Close = FS_SD_SPI_CloseInternal,
  .InfoGet = FS_SD_SPI_InfoGet,
  .CidRd = FS_SD_SPI_CID_Rd,
  .CsdRd = FS_SD_SPI_CSD_Rd
};

static const SPI_SLAVE_INFO FS_SD_SPI_DfltSlaveInfo = {
  .Mode = (SERIAL_SPI_BUS_MODE_CPHA_BIT | SERIAL_SPI_BUS_MODE_CPOL_BIT),
  .BitsPerFrame = 8u,
  .LSbFirst = DEF_NO,
  .SClkFreqMax = FS_SD_DFLT_CLK_SPD,
  .SlaveID = 0u,
  .TxDummyByte = 0xFFu,
  .ActiveLow = DEF_YES
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           FS_SD_SPI_OpenInternal()
 *
 * @brief    Open (initialize) a device instance.
 *
 * @param    p_sd    Pointer to a SD SPI.
 *
 * @param    p_err   Error pointer.
 *
 * @note     (1) Tracking whether a device is open is not necessary, because this should NEVER be
 *               called when a device is already open.
 *
 * @note     (2) Some drivers may need to track whether a device has been previously opened
 *               (indicating that the hardware has previously been initialized).
 *
 * @note     (3) This function will be called EVERY time the device is opened.
 *
 * @note     (4) The initialization flow for a SD card is charted in [Ref 1] Figure 4-2.  Similarly,
 *               the power-up sequence for a MMC card is given in Section 1.11 of [Ref 2].
 *               @verbatim
 *                                                           POWER ON
 *                                                               |
 *                                                               |
 *                                                               v
 *                                                           CMD0
 *                                                               |
 *                                                               |
 *                                       NO RESPONSE              v             CARD RETURNS RESPONSE
 *                                           +---------------CMD8---------------+
 *                                           |                                  |
 *                                           |                                  |
 *                                           v                                  v             NO
 *                       NO RESPONSE       ACMD41                            RESPONSE------------------>UNUSABLE
 *                   +--------------------w/ HCS = 0<---------+                VALID?                      CARD
 *                   |                         |              |                   |
 *                   |                         |              | BUSY              |
 *                   |                         v              |                   v
 *                   |                   CARD IS READY?-------+                 ACMD41
 *                   |                         |                     +------->w/ HCS = 1
 *                   |                         | YES                 |             |          TIMEOUT
 *                   |                         |                BUSY |             |          OR NO
 *                   |                         |                     |             v          RESPONSE
 *                   |                         |                     +-------CARD IS READY?------------->UNUSABLE
 *                 CMD1<------------+          |                                   |                       CARD
 *                   |              |          |                                   | YES
 *                   |              | BUSY     |                                   |
 *           P       v              |          |                                   v
 *           --CARD IS READY?-------+          |                                CMD58
 *                   |                         |                                   |
 *                   | YES                     |                                   |
 *                   |                         |                     NO            v             YES
 *           SABLE   |                         |               +----------- CCS IN RESPONSE? -----------+
 *           ARD     |                         |               |                                        |
 *                   v                         v               v                                        v
 *                   MMC                  VER 1.x STD      VER 2.0+ STD                            VER 2.0+ HIGH
 *                   |                   CAPACITY SD      CAPACITY SD                              CAPACITY SD
 *                   |                         |               |                                        |
 *                   |                         |               |                                        |
 *                   +-------------------------+---------------+----------------------------------------+
 *                                                             |
 *                                                             |
 *                                                             v
 *           @endverbatim
 *******************************************************************************************************/
static void FS_SD_SPI_OpenInternal(FS_SD    *p_sd,
                                   RTOS_ERR *p_err)
{
  FS_SD_SPI         *p_sd_spi;
  FS_SD_INFO        *p_sd_info;
  FS_SD_SPI_PM_ITEM *p_sd_spi_pm_item;
  SPI_SLAVE_INFO    slave_info;
  CPU_INT08U        card_type;
  CPU_INT08U        resp_r1;
  CPU_INT08U        resp_reg[16];
  CPU_INT08U        retries;
  CPU_BOOLEAN       ok;

  WITH_SCOPE_BEGIN(p_err) {
    p_sd_spi = (FS_SD_SPI *)p_sd;
    p_sd_spi_pm_item = (FS_SD_SPI_PM_ITEM *)p_sd->Media.PmItemPtr;

    //                                                             --------------------- INIT HW ----------------------
    p_sd_info = &p_sd_spi->SD.Info;

    FS_SD_ClrInfo(p_sd_info);                                   // Clr old info.

    p_sd_spi->SpiBusHandle = SPI_BusHandleGetFromName(p_sd_spi_pm_item->SpiCtrlrNamePtr);
    ASSERT_BREAK(p_sd_spi->SpiBusHandle != SPI_BusHandleNull, RTOS_ERR_NOT_FOUND);

    slave_info = FS_SD_SPI_DfltSlaveInfo;
    slave_info.SlaveID = p_sd_spi_pm_item->SlaveID;
    BREAK_ON_ERR(p_sd_spi->SpiHandle = SPI_SlaveOpen(p_sd_spi->SpiBusHandle,
                                                     &slave_info,
                                                     p_err));
    ON_BREAK {
      RTOS_ERR err;
      SPI_SlaveClose(p_sd_spi->SpiHandle, &err);
      break;
    } WITH {
      //                                                           -------------- SEND CMD0 TO RESET BUS --------------
      retries = 0u;
      resp_r1 = FS_DEV_SD_SPI_RESP_EMPTY;
      BREAK_ON_ERR(while) ((retries < FS_DEV_SD_SPI_CMD_TIMEOUT)
                           && (resp_r1 != FS_DEV_SD_SPI_R1_IN_IDLE_STATE)) {
        BREAK_ON_ERR(FS_SD_SPI_WITH) (p_sd_spi, p_err) {
          //                                                       Send empty clks.
          BREAK_ON_ERR(FS_SD_SPI_ClksWait(p_sd_spi, 20u, p_err));
          //                                                       Perform SD CMD0.
          BREAK_ON_ERR(resp_r1 = FS_SD_SPI_Cmd(p_sd_spi,
                                               FS_DEV_SD_CMD_GO_IDLE_STATE,
                                               0u,
                                               p_err));
        }

        if (resp_r1 != FS_DEV_SD_SPI_R1_IN_IDLE_STATE) {
          KAL_Dly(2u);
          retries++;
        }
      }

      //                                                           If card never responded, return error.
      ASSERT_BREAK(resp_r1 == FS_DEV_SD_SPI_R1_IN_IDLE_STATE, RTOS_ERR_IO);

      KAL_Dly(100u);

      //                                                           --------------- DETERMINE CARD TYPE ----------------
      //                                                           Make card rdy & determine card type.
      BREAK_ON_ERR(card_type = FS_SD_SPI_MakeRdy(p_sd_spi, p_err));
      ASSERT_BREAK(card_type != FS_DEV_SD_CARDTYPE_NONE, RTOS_ERR_IO);

      p_sd_info->CardType = card_type;
      p_sd_info->HighCapacity = (card_type == FS_DEV_SD_CARDTYPE_SD_V2_0_HC)
                                || (card_type == FS_DEV_SD_CARDTYPE_MMC_HC);

      //                                                           -------------------- RD CSD REG --------------------
      BREAK_ON_ERR(FS_SD_SPI_CSD_Send(p_sd_spi,                 // Get CSD reg from card.
                                      &resp_reg[0],
                                      p_err));

      ok = FS_SD_ParseCSD(resp_reg,                             // Parse CSD info.
                          p_sd_info,
                          card_type);
      ASSERT_BREAK(ok, RTOS_ERR_IO);
    }

    slave_info = FS_SD_SPI_DfltSlaveInfo;
    slave_info.SClkFreqMax = p_sd_info->ClkFreq;
    slave_info.SlaveID = p_sd_spi_pm_item->SlaveID;

    BREAK_ON_ERR(SPI_SlaveOpen(p_sd_spi->SpiBusHandle,
                               &slave_info,
                               p_err));
    ON_BREAK {
      RTOS_ERR err;
      SPI_SlaveClose(p_sd_spi->SpiHandle, &err);
      p_sd_spi->SD.IsClosed = DEF_YES;                          // Mark SD as closed. Open has failed.
      break;
    } WITH {
      //                                                           -------------------- RD CID REG --------------------
      //                                                           Get CID reg from card.
      BREAK_ON_ERR(FS_SD_SPI_CID_Send(p_sd_spi,
                                      &resp_reg[0],
                                      p_err));

      (void)FS_SD_ParseCID(resp_reg,                            // Parse CID info.
                           p_sd_info,
                           card_type);

      //                                                           ------------------- SET BLK LEN --------------------
      //                                                           Perform CMD16.
      BREAK_ON_ERR(resp_r1 = FS_SD_SPI_CmdR1(p_sd_spi,
                                             FS_DEV_SD_CMD_SET_BLOCKLEN,
                                             FS_DEV_SD_BLK_SIZE,
                                             p_err));

      if (resp_r1 != FS_DEV_SD_SPI_R1_NONE) {
        LOG_ERR(("Could not set block length: error: ", (x)resp_r1, "."));
      }

#if (FS_SD_SPI_CFG_CRC_EN == DEF_ENABLED)                       // ------------- ENABLE CRC VERIFICATION --------------
      BREAK_ON_ERR(resp_r1 = FS_SD_SPI_CmdR1(p_sd_spi,
                                             FS_DEV_SD_CMD_CRC_ON_OFF,
                                             FS_DEV_SD_CMD59_CRC_OPT,
                                             p_err));

      if (resp_r1 != FS_DEV_SD_SPI_R1_NONE) {
        LOG_ERR(("Could not enable CRC: error: ", (x)resp_r1, "."));
      }
#endif
      p_sd_spi->SD.IsClosed = DEF_NO;                           // SD open completed without err.
                                                                // ---------------- OUTPUT TRACE INFO -----------------
#if LOG_VRB_IS_EN()
      FS_SD_TraceInfo(p_sd_info);
#endif
    }
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                           FS_SD_SPI_CloseInternal()
 *
 * @brief    Close a SD SPI block device.
 *
 * @param    p_sd    Pointer to a SD SPI.
 *
 * @param    p_err   Error pointer.
 *
 * @note     (1) This function will be called EVERY time the device is closed.
 *******************************************************************************************************/
static void FS_SD_SPI_CloseInternal(FS_SD    *p_sd,
                                    RTOS_ERR *p_err)
{
  FS_SD_SPI *p_sd_spi;

  PP_UNUSED_PARAM(p_err);

  p_sd_spi = (FS_SD_SPI *)p_sd;

  SPI_SlaveClose(p_sd_spi->SpiHandle, p_err);
  p_sd_spi->SD.IsClosed = DEF_YES;
}

/****************************************************************************************************//**
 *                                           FS_SD_SPI_InfoGet()
 *
 * @brief    Get low-level information about SD/MMC card.
 *
 * @param    p_sd    Pointer to a SD SPI.
 *
 * @param    p_info  Pointer to structure that will receive SD/MMC card information.
 *******************************************************************************************************/
static void FS_SD_SPI_InfoGet(FS_SD      *p_sd,
                              FS_SD_INFO *p_info)
{
  *p_info = p_sd->Info;
}

/****************************************************************************************************//**
 *                                           FS_SD_SPI_CID_Rd()
 *
 * @brief    Read SD/MMC Card ID (CID) register.
 *
 * @param    p_sd    Pointer to a SD SPI.
 *
 * @param    p_dest  Pointer to 16-byte buffer that will receive SD/MMC Card ID register.
 *
 * @param    p_err   Error pointer.
 *
 * @note     (1) The device MUST be a SD/MMC device (e.g., "sd:0:").
 *
 * @note     (2) For SD cards, the structure of the CID is defined in [Ref 1] Section 5.1.
 *               For MMC cards, the structure of the CID is defined in [Ref 2] Section 8.2.
 *******************************************************************************************************/
static void FS_SD_SPI_CID_Rd(FS_SD      *p_sd,
                             CPU_INT08U *p_dest,
                             RTOS_ERR   *p_err)
{
  FS_SD_SPI_CID_Send((FS_SD_SPI *)p_sd, p_dest, p_err);
}

/****************************************************************************************************//**
 *                                           FS_SD_SPI_CSD_Rd()
 *
 * @brief    Read SD/MMC Card-Specific Data (CSD) register.
 *
 * @param    p_sd    Pointer to a SD SPI.
 *
 * @param    p_dest  Pointer to 16-byte buffer that will receive SD/MMC Card ID register.
 *
 * @param    p_err   Error pointer.
 *
 * @note     (1) The device MUST be a SD/MMC device (e.g., "sd:0:").
 *
 * @note     (2) For SD v1.x & SD v2.0 standard capacity, the structure of the CSD is defined in
 *               [Ref 1], Section 5.3.2.
 *               For MMC cards, the structure of the CSD is defined in [Ref 2], Section 8.3.
 *               For SD v2.0 high capacity cards, the structure of the CSD is defined in [Ref 1],
 *               Section 5.3.3.
 *******************************************************************************************************/
static void FS_SD_SPI_CSD_Rd(FS_SD      *p_sd,
                             CPU_INT08U *p_dest,
                             RTOS_ERR   *p_err)
{
  FS_SD_SPI_CSD_Send((FS_SD_SPI *)p_sd, p_dest, p_err);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                       DRIVER INTERFACE FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           FS_SD_SPI_MediaAdd()
 *
 * @brief    Add a SD SPI media instance.
 *
 * @param    p_pm_item   Pointer to a media platform manager item.
 *
 * @param    p_err       Error pointer.
 *
 * @return   Pointer to the added media.
 *******************************************************************************************************/
static FS_MEDIA *FS_SD_SPI_MediaAdd(const FS_MEDIA_PM_ITEM *p_pm_item,
                                    RTOS_ERR               *p_err)
{
  FS_SD_SPI *p_sd_spi;

  PP_UNUSED_PARAM(p_pm_item);

  WITH_SCOPE_BEGIN(p_err) {
    BREAK_ON_ERR(p_sd_spi = (FS_SD_SPI *)Mem_SegAlloc("FS - SD SPI instance",
                                                      FSMedia_InitCfgPtr->MemSegPtr,
                                                      sizeof(FS_SD_SPI),
                                                      p_err));

    //                                                             --------------------- CLR DATA ---------------------
    FS_SD_ClrInfo(&p_sd_spi->SD.Info);                          // Clr SD info.

#if (FS_STORAGE_CFG_CTR_STAT_EN == DEF_ENABLED)                 // Clr stat ctrs.
    p_sd_spi->StatRdCtr = 0u;
    p_sd_spi->StatWrCtr = 0u;
#endif

#if (FS_STORAGE_CFG_CTR_ERR_EN == DEF_ENABLED)                  // Clr err ctrs.
    p_sd_spi->ErrRdCtr = 0u;
    p_sd_spi->ErrWrCtr = 0u;

    p_sd_spi->ErrCmdRespCtr = 0u;
    p_sd_spi->ErrCmdRespTimeoutCtr = 0u;
    p_sd_spi->ErrCmdRespEraseResetCtr = 0u;
    p_sd_spi->ErrCmdRespIllegalCmdCtr = 0u;
    p_sd_spi->ErrCmdRespCommCRC_Ctr = 0u;
    p_sd_spi->ErrCmdRespEraseSeqCtr = 0u;
    p_sd_spi->ErrCmdRespAddrCtr = 0u;
    p_sd_spi->ErrCmdRespParamCtr = 0u;
#endif

    p_sd_spi->SD.IsClosed = DEF_YES;                            // SD instance considered as closed when adding it.
    p_sd_spi->SD.BusApiPtr = &FS_SD_SPI_BusApi;
  } WITH_SCOPE_END

  return (&p_sd_spi->SD.Media);
}

/****************************************************************************************************//**
 *                                           FS_SD_SPI_MediaRem()
 *
 * @brief    Remove a SD SPI media instance.
 *
 * @param    p_media     Pointer to media.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_SD_SPI_MediaRem(FS_MEDIA *p_media,
                               RTOS_ERR *p_err)
{
  PP_UNUSED_PARAM(p_media);
  PP_UNUSED_PARAM(p_err);

  RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_NOT_AVAIL,; );
}

/****************************************************************************************************//**
 *                                           FS_SD_SPI_BlkDevAdd()
 *
 * @brief    Add a SD block device.
 *
 * @param    p_media     Pointer to a SD media instance.
 *
 * @param    p_err       Error pointer.
 *
 * @return   Pointer to block device.
 *******************************************************************************************************/
static FS_BLK_DEV *FS_SD_SPI_BlkDevAdd(FS_MEDIA *p_media,
                                       RTOS_ERR *p_err)
{
  PP_UNUSED_PARAM(p_err);

  return (&((FS_SD *)p_media)->BlkDev);
}

/****************************************************************************************************//**
 *                                           FS_SD_SPI_BlkDevRem()
 *
 * @brief    Remove a SD block device.
 *
 * @param    p_media     Pointer to a SD media instance.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_SD_SPI_BlkDevRem(FS_BLK_DEV *p_blk_dev,
                                RTOS_ERR   *p_err)
{
  PP_UNUSED_PARAM(p_blk_dev);
  PP_UNUSED_PARAM(p_err);
}

/****************************************************************************************************//**
 *                                               FS_SD_SPI_Open()
 *
 * @brief    Open (initialize) a device instance.
 *
 * @param    p_blk_dev   Pointer to a block device.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_SD_SPI_Open(FS_BLK_DEV *p_blk_dev,
                           RTOS_ERR   *p_err)
{
  FS_SD_SPI_OpenInternal((FS_SD *)p_blk_dev->MediaPtr, p_err);
}

/****************************************************************************************************//**
 *                                               FS_SD_SPI_Close()
 *
 * @brief    Close an SD SPI block device.
 *
 * @param    p_dev   Pointer to a block device.
 *
 * @param    p_err   Error pointer.
 *
 * @note     (1) This function will be called EVERY time the device is closed.
 *******************************************************************************************************/
static void FS_SD_SPI_Close(FS_BLK_DEV *p_blk_dev,
                            RTOS_ERR   *p_err)
{
  FS_SD_SPI_CloseInternal((FS_SD *)p_blk_dev->MediaPtr, p_err);
}

/****************************************************************************************************//**
 *                                               FS_SD_SPI_Rd()
 *
 * @brief    Read from a device & store data in buffer.
 *
 * @param    p_blk_dev   Pointer to a block device to read from.
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
 *******************************************************************************************************/
static void FS_SD_SPI_Rd(FS_BLK_DEV *p_blk_dev,
                         void       *p_dest,
                         FS_LB_NBR  start,
                         FS_LB_QTY  cnt,
                         RTOS_ERR   *p_err)
{
  CPU_INT08U *p_dest_08;
  FS_SD_INFO *p_sd_info;
  FS_SD_SPI  *p_sd_spi;
  CPU_INT32U start_addr;

  //                                                               ------------------ PREPARE FOR RD ------------------
  p_sd_spi = (FS_SD_SPI *)p_blk_dev->MediaPtr;
  p_dest_08 = (CPU_INT08U *)p_dest;
  p_sd_info = &p_sd_spi->SD.Info;

  //                                                               See Note #2.
  start_addr = (p_sd_info->HighCapacity == DEF_YES) ? start : (start * FS_DEV_SD_BLK_SIZE);

  if (cnt > 1u) {                                               // ---------------- PERFORM MULTIPLE RD ---------------
    FS_SD_SPI_DataMultiRd(p_sd_spi,                             // Rd data.
                          FS_DEV_SD_CMD_READ_MULTIPLE_BLOCK,
                          start_addr,
                          p_dest_08,
                          FS_DEV_SD_BLK_SIZE,
                          cnt,
                          p_err);
  } else {                                                      // ---------------- PERFORM SINGLE RD -----------------
    FS_SD_SPI_DataRd(p_sd_spi,                                  // Rd data.
                     FS_DEV_SD_CMD_READ_SINGLE_BLOCK,
                     start_addr,
                     p_dest_08,
                     FS_DEV_SD_BLK_SIZE,
                     p_err);
  }

  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {             // --------------------- CHK ERR ----------------------
    FS_DEV_SD_SPI_STAT_RD_CTR_ADD(p_sd_spi, cnt);
  } else {
    FS_DEV_SD_SPI_ERR_RD_CTR_INC(p_sd_spi);
  }
}

/****************************************************************************************************//**
 *                                               FS_SD_SPI_Wr()
 *
 * @brief    Write data to a device from a buffer.
 *
 * @param    p_blk_dev   Pointer to block device to write to.
 *
 * @param    p_src       Pointer to source buffer.
 *
 * @param    start       Start sector of write.
 *
 * @param    cnt         Number of sectors to write.
 *
 * @param    p_err       Error pointer.
 *
 * @note     (1) Standard-capacity devices receive the start byte address as the argument of the write
 *               command, limiting device access to 4-GB (the range of a 32-bit variable).  To solve
 *               that problem, high-capacity devices (like SDHC cards) receive the block number as the
 *               argument of the write command.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_SD_SPI_Wr(FS_BLK_DEV *p_blk_dev,
                         void       *p_src,
                         FS_LB_NBR  start,
                         FS_LB_QTY  cnt,
                         RTOS_ERR   *p_err)
{
  FS_SD_INFO *p_sd_info;
  FS_SD_SPI  *p_sd_spi;
  CPU_INT08U *p_src_08;
  CPU_INT32U start_addr;

  //                                                               ------------------ PREPARE FOR WR ------------------
  p_sd_spi = (FS_SD_SPI *)p_blk_dev->MediaPtr;
  p_src_08 = (CPU_INT08U *)p_src;
  p_sd_info = &p_sd_spi->SD.Info;

  //                                                               See Note #2.
  start_addr = (p_sd_info->HighCapacity == DEF_YES) ? start : (start * FS_DEV_SD_BLK_SIZE);

  if (cnt > 1u) {                                               // --------------- PERFORM MULTIPLE WR ----------------
    FS_SD_SPI_DataMultiWr(p_sd_spi,                             // Wr data.
                          FS_DEV_SD_CMD_WRITE_MULTIPLE_BLOCK,
                          start_addr,
                          p_src_08,
                          FS_DEV_SD_BLK_SIZE,
                          cnt,
                          p_err);
  } else {                                                      // ---------------- PERFORM SINGLE WR -----------------
    FS_SD_SPI_DataWr(p_sd_spi,                                  // Wr data.
                     FS_DEV_SD_CMD_WRITE_BLOCK,
                     start_addr,
                     p_src_08,
                     FS_DEV_SD_BLK_SIZE,
                     p_err);
  }

  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
    FS_DEV_SD_SPI_STAT_WR_CTR_ADD(p_sd_spi, cnt);
  } else {                                                      // --------------------- CHK ERR ----------------------
    FS_DEV_SD_SPI_ERR_WR_CTR_INC(p_sd_spi);
  }
}
#endif

/****************************************************************************************************//**
 *                                               FS_SD_SPI_Sync()
 *
 * @brief    Synchronize a block device.
 *
 * @param    p_blk_dev   Pointer to a block device.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_SD_SPI_Sync(FS_BLK_DEV *p_blk_dev,
                           RTOS_ERR   *p_err)
{
  PP_UNUSED_PARAM(p_blk_dev);
  PP_UNUSED_PARAM(p_err);
}
#endif

/****************************************************************************************************//**
 *                                               FS_SD_SPI_Trim()
 *
 * @brief    Trim a block device.
 *
 * @param    p_blk_dev   Pointer to a block device.
 *
 * @param    lb_nbr      Logical block number.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_SD_SPI_Trim(FS_BLK_DEV *p_blk_dev,
                           FS_LB_NBR  lb_nbr,
                           RTOS_ERR   *p_err)
{
  PP_UNUSED_PARAM(p_blk_dev);
  PP_UNUSED_PARAM(lb_nbr);
  PP_UNUSED_PARAM(p_err);
}
#endif

/****************************************************************************************************//**
 *                                               FS_SD_SPI_Query()
 *
 * @brief    Get information about a device.
 *
 * @param    p_blk_dev   Pointer to block device to query.
 *
 * @param    p_info      Pointer to structure that will receive device information.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_SD_SPI_Query(FS_BLK_DEV      *p_blk_dev,
                            FS_BLK_DEV_INFO *p_info,
                            RTOS_ERR        *p_err)
{
  PP_UNUSED_PARAM(p_err);

  FS_SD_SPI *p_sd_spi = (FS_SD_SPI *)p_blk_dev->MediaPtr;

  p_info->LbSizeLog2 = FS_DEV_SD_BLK_SIZE_LOG2;
  p_info->LbCnt = p_sd_spi->SD.Info.NbrBlks * (p_sd_spi->SD.Info.BlkSize >> FS_DEV_SD_BLK_SIZE_LOG2);
  p_info->Fixed = DEF_NO;
}

/****************************************************************************************************//**
 *                                           FS_SD_SPI_IsConn()
 *
 * @brief    Check whether a media is connected.
 *
 * @param    p_media     Pointer to media.
 *
 * @return   DEF_YES, if card is present and has not changed.
 *           DEF_NO,  otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN FS_SD_SPI_IsConn(FS_MEDIA *p_media)
{
  FS_SD_SPI         *p_sd_spi;
  FS_SD_SPI_PM_ITEM *p_sd_spi_pm_item;
  CPU_INT32U        retries;
  CPU_INT08U        cid[FS_DEV_SD_CID_REG_LEN];
  CPU_INT08U        card_type;
  CPU_INT08U        resp_r1;
  RTOS_ERR          err;

  p_sd_spi = (FS_SD_SPI *)p_media;
  p_sd_spi_pm_item = (FS_SD_SPI_PM_ITEM *)p_media->PmItemPtr;

  if (p_sd_spi->SD.IsClosed) {                                  // If the card was not mounted, perform bsp lvl open.
    SPI_SLAVE_INFO slave_info;

    RTOS_ERR_SET(err, RTOS_ERR_NONE);

    p_sd_spi->SpiBusHandle = SPI_BusHandleGetFromName(p_sd_spi_pm_item->SpiCtrlrNamePtr);
    if (p_sd_spi->SpiBusHandle == SPI_BusHandleNull) {
      LOG_ERR(("Could not get SPI bus handle from serial controller name."));
      return (DEF_NO);
    }

    slave_info = FS_SD_SPI_DfltSlaveInfo;
    slave_info.SlaveID = p_sd_spi_pm_item->SlaveID;

    p_sd_spi->SpiHandle = SPI_SlaveOpen(p_sd_spi->SpiBusHandle,
                                        &slave_info,
                                        &err);
    if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
      return (DEF_NO);
    }
  }

  //                                                               --------------- CHK IF CARD MOUNTED ----------------
  if (!p_sd_spi->SD.IsClosed) {                                 // If the card was mounted, chk if it is still mounted.
    RTOS_ERR_SET(err, RTOS_ERR_NONE);
    FS_SD_SPI_CID_Send(p_sd_spi, &cid[0], &err);
    if (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE) {
      return (DEF_YES);
    }
  }

  //                                                               -------------- SEND CMD0 TO RESET BUS --------------
  retries = 0u;
  resp_r1 = FS_DEV_SD_SPI_RESP_EMPTY;
  RTOS_ERR_SET(err, RTOS_ERR_NONE);
  while ((retries < FS_DEV_SD_SPI_CMD_TIMEOUT)
         && (resp_r1 != FS_DEV_SD_SPI_R1_IN_IDLE_STATE)) {
    FS_SD_SPI_WITH(p_sd_spi, &err) {
      FS_SD_SPI_ClksWait(p_sd_spi, 20u, &err);                  // Send empty clks.
      if (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE) {
        resp_r1 = FS_SD_SPI_Cmd(p_sd_spi,                       // Perform SD CMD0.
                                FS_DEV_SD_CMD_GO_IDLE_STATE,
                                0u,
                                &err);
      }
    }
    if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
      return (DEF_NO);
    }

    if (resp_r1 != FS_DEV_SD_SPI_R1_IN_IDLE_STATE) {
      KAL_Dly(2u);
      retries++;
    }
  }

  if (resp_r1 != FS_DEV_SD_SPI_R1_IN_IDLE_STATE) {              // If card never responded ...
    return (DEF_NO);
  }

  KAL_Dly(100u);

  //                                                               --------------- DETERMINE CARD TYPE ----------------
  card_type = FS_SD_SPI_MakeRdy(p_sd_spi, &err);                // Make card rdy & determine card type.
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    return (DEF_NO);
  }
  if (card_type == FS_DEV_SD_CARDTYPE_NONE) {                   // If card type NOT determined ...
    return (DEF_NO);
  }

  if (p_sd_spi->SD.IsClosed) {                                  // If card was not mounted, perform bsp lvl close.
    SPI_SlaveClose(p_sd_spi->SpiHandle, &err);
  }

  return (DEF_YES);
}

/****************************************************************************************************//**
 *                                           FS_SD_SPI_AlignReqGet()
 *
 * @brief    Get buffer alignment requirement of SPI controller.
 *
 * @param    p_media     Pointer to media.
 *
 * @param    p_err       Error pointer.
 *
 * @return   Buffer alignment in bytes.
 *******************************************************************************************************/
static CPU_SIZE_T FS_SD_SPI_AlignReqGet(FS_MEDIA *p_media,
                                        RTOS_ERR *p_err)
{
  CPU_SIZE_T        req_align;
  FS_SD_SPI_PM_ITEM *p_sd_spi_pm_item = (FS_SD_SPI_PM_ITEM *)p_media->PmItemPtr;

  req_align = Serial_CtrlrReqAlignGet(p_sd_spi_pm_item->SpiCtrlrNamePtr);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (req_align);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           FS_SD_SPI_MakeRdy()
 *
 * @brief    Move card into 'ready' state (& get card type).
 *
 * @param    p_sd_spi    Pointer to SD SPI data.
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
 * @note     (1) #### Add voltage range check.
 *******************************************************************************************************/
static CPU_INT08U FS_SD_SPI_MakeRdy(FS_SD_SPI *p_sd_spi,
                                    RTOS_ERR  *p_err)
{
  CPU_INT08U card_type;
  CPU_INT08U resp_r1;
  CPU_INT08U resp_r3_7[4];
  CPU_INT08U retries;
  RTOS_ERR   err_local;

  //                                                               ------------- SEND CMD8 TO GET CARD OP -------------
  RTOS_ERR_SET(err_local, RTOS_ERR_NONE);
  resp_r1 = FS_SD_SPI_CmdR3_7(p_sd_spi,                         // Perform CMD8.
                              FS_DEV_SD_CMD_SEND_IF_COND,
                              FS_DEV_SD_CMD8_VHS_27_36_V | FS_DEV_SD_CMD8_CHK_PATTERN,
                              resp_r3_7,
                              &err_local);

  //                                                               If card does resp, v2.0 card.
  card_type = FS_DEV_SD_CARDTYPE_SD_V1_X;
  if (resp_r1 == FS_DEV_SD_SPI_R1_IN_IDLE_STATE) {
    if (resp_r3_7[3] == FS_DEV_SD_CMD8_CHK_PATTERN) {
      card_type = FS_DEV_SD_CARDTYPE_SD_V2_0;
    }
  }

  //                                                               ---------- SEND ACMD41 TO GET CARD STATUS ----------
  retries = 0u;
  resp_r1 = FS_DEV_SD_SPI_RESP_EMPTY;
  while (retries < FS_DEV_SD_SPI_CMD_TIMEOUT) {
    RTOS_ERR_SET(err_local, RTOS_ERR_NONE);
    resp_r1 = FS_SD_SPI_AppCmdR1(p_sd_spi,                      // Perform ACMD41.
                                 FS_DEV_SD_ACMD_SD_SEND_OP_COND,
                                 FS_DEV_SD_ACMD41_HCS,
                                 &err_local);

    if (resp_r1 == FS_DEV_SD_SPI_R1_NONE) {
      break;
    }

    KAL_Dly(2u);

    retries++;
  }

  //                                                               ------------- SEND CMD1 (FOR MMC CARD) -------------
  if (resp_r1 != FS_DEV_SD_SPI_R1_NONE) {
    retries = 0u;
    while (retries < FS_DEV_SD_SPI_CMD_TIMEOUT) {
      RTOS_ERR_SET(err_local, RTOS_ERR_NONE);
      resp_r1 = FS_SD_SPI_CmdR1(p_sd_spi,
                                FS_DEV_SD_CMD_SEND_OP_COND,
                                0u,
                                &err_local);

      if (resp_r1 == FS_DEV_SD_SPI_R1_NONE) {
        break;
      }

      KAL_Dly(2u);
      retries++;
    }

    if (resp_r1 != FS_DEV_SD_SPI_R1_NONE) {
      return (FS_DEV_SD_CARDTYPE_NONE);
    }

    card_type = FS_DEV_SD_CARDTYPE_MMC;
  }
  //                                                               ------------ SEND CMD58 TO GET CARD OCR ------------
  retries = 0u;
  while (retries < FS_DEV_SD_SPI_CMD_TIMEOUT) {
    RTOS_ERR_SET(err_local, RTOS_ERR_NONE);
    resp_r1 = FS_SD_SPI_CmdR3_7(p_sd_spi,                       // Perform CMD58.
                                FS_DEV_SD_CMD_READ_OCR,
                                DEF_BIT_30,
                                resp_r3_7,
                                &err_local);

    if (resp_r1 == FS_DEV_SD_SPI_R1_NONE) {
      if (DEF_BIT_IS_SET(resp_r3_7[0], (FS_DEV_SD_OCR_BUSY >> (3u * DEF_OCTET_NBR_BITS))) == DEF_YES) {
        //                                                         Chk for HC card.
        if (DEF_BIT_IS_SET(resp_r3_7[0], (FS_DEV_SD_OCR_CCS >> (3u * DEF_OCTET_NBR_BITS))) == DEF_YES) {
          if (card_type == FS_DEV_SD_CARDTYPE_SD_V2_0) {
            card_type = FS_DEV_SD_CARDTYPE_SD_V2_0_HC;
          } else {
            if (card_type == FS_DEV_SD_CARDTYPE_MMC) {
              card_type = FS_DEV_SD_CARDTYPE_MMC_HC;
            }
          }
        }
        break;
      }
    }

    retries++;
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  return (card_type);
}

/****************************************************************************************************//**
 *                                               FS_SD_SPI_Cmd()
 *
 * @brief    Perform command & get response byte.
 *
 * @param    p_sd_spi    Pointer to SD SPI.
 *
 * @param    cmd         Command index.
 *
 * @param    arg         Command argument.
 *
 * @param    p_err       Error pointer.
 *
 * @return   Command response.
 *******************************************************************************************************/
static CPU_INT08U FS_SD_SPI_Cmd(FS_SD_SPI  *p_sd_spi,
                                CPU_INT08U cmd,
                                CPU_INT32U arg,
                                RTOS_ERR   *p_err)
{
  CPU_INT08U cmd_pkt[6];
  CPU_INT08U chk_sum;
  CPU_INT16U timeout;
  CPU_INT08U resp = FS_DEV_SD_SPI_RESP_EMPTY;

  //                                                               --------------------- FORM CMD ---------------------
  cmd_pkt[0] = DEF_BIT_06 | cmd;
  cmd_pkt[1] = (CPU_INT08U)(arg >> (3u * DEF_OCTET_NBR_BITS)) & DEF_OCTET_MASK;
  cmd_pkt[2] = (CPU_INT08U)(arg >> (2u * DEF_OCTET_NBR_BITS)) & DEF_OCTET_MASK;
  cmd_pkt[3] = (CPU_INT08U)(arg >> (1u * DEF_OCTET_NBR_BITS)) & DEF_OCTET_MASK;
  cmd_pkt[4] = (CPU_INT08U)(arg >> (0u * DEF_OCTET_NBR_BITS)) & DEF_OCTET_MASK;

  chk_sum = FS_SD_ChkSumCalc_7Bit(&cmd_pkt[0], 5u);

  cmd_pkt[5] = (CPU_INT08U)(chk_sum << 1) | DEF_BIT_00;

  //                                                               ---------------------- WR CMD ----------------------
  SPI_SlaveTx(p_sd_spi->SpiHandle,
              &cmd_pkt[0],
              6u,
              FS_DEV_SD_SPI_XFER_TIMEOUT,
              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (FS_DEV_SD_SPI_RESP_EMPTY);
  }

  //                                                               ---------------------- RD RESP ---------------------
  timeout = 0u;
  while (timeout < FS_DEV_SD_SPI_CMD_TIMEOUT) {
    SPI_SlaveRx(p_sd_spi->SpiHandle,                            // Rd resp byte.
                &resp,
                1u,
                FS_DEV_SD_SPI_XFER_TIMEOUT,
                p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (FS_DEV_SD_SPI_RESP_EMPTY);
    }
    if (DEF_BIT_IS_CLR(resp, DEF_BIT_07) == DEF_YES) {          // Top bit of resp MUST be clr.
      break;
    }
    timeout++;
  }

  if (timeout == FS_DEV_SD_SPI_CMD_TIMEOUT) {                   // If timeout, rtn err.
    FS_DEV_SD_SPI_ERR_CMD_RESP_TIMEOUT_CTR_INC(p_sd_spi);
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return (FS_DEV_SD_SPI_RESP_EMPTY);
  }

#if (FS_STORAGE_CFG_CTR_ERR_EN == DEF_ENABLED)
  if (cmd != FS_DEV_SD_CMD_STOP_TRANSMISSION) {
    if (DEF_BIT_IS_SET(resp, FS_DEV_SD_SPI_R1_ERASE_RESET) == DEF_YES) {
      FS_DEV_SD_SPI_ERR_CMD_RESP_RESET_ERASE_CTR_INC(p_sd_spi);
    }

    if (DEF_BIT_IS_SET(resp, FS_DEV_SD_SPI_R1_ILLEGAL_COMMAND) == DEF_YES) {
      FS_DEV_SD_SPI_ERR_CMD_RESP_ILLEGAL_CMD_CTR_INC(p_sd_spi);
    }

    if (DEF_BIT_IS_SET(resp, FS_DEV_SD_SPI_R1_COM_CRC_ERROR) == DEF_YES) {
      FS_DEV_SD_SPI_ERR_CMD_RESP_COMM_CRC_INC(p_sd_spi);
    }

    if (DEF_BIT_IS_SET(resp, FS_DEV_SD_SPI_R1_ERASE_SEQUENCE_ERROR) == DEF_YES) {
      FS_DEV_SD_SPI_ERR_CMD_RESP_SEQ_ERASE_CTR_INC(p_sd_spi);
    }

    if (DEF_BIT_IS_SET(resp, FS_DEV_SD_SPI_R1_ADDRESS_ERROR) == DEF_YES) {
      FS_DEV_SD_SPI_ERR_CMD_RESP_ADDR_CTR_INC(p_sd_spi);
    }

    if (DEF_BIT_IS_SET(resp, FS_DEV_SD_SPI_R1_PARAMETER_ERROR) == DEF_YES) {
      FS_DEV_SD_SPI_ERR_CMD_RESP_PARAM_CTR_INC(p_sd_spi);
    }
  }
#endif

  return (resp);
}

/****************************************************************************************************//**
 *                                               FS_SD_SPI_CmdR1()
 *
 * @brief    Perform command & get R1 response.
 *
 * @param    p_sd_spi    Pointer to SD SPI.
 *
 * @param    cmd         Command index.
 *
 * @param    arg         Command argument.
 *
 * @param    p_err       Error pointer.
 *
 * @return   R1 response.
 *******************************************************************************************************/
static CPU_INT08U FS_SD_SPI_CmdR1(FS_SD_SPI  *p_sd_spi,
                                  CPU_INT08U cmd,
                                  CPU_INT32U arg,
                                  RTOS_ERR   *p_err)
{
  CPU_INT08U resp_r1 = FS_DEV_SD_SPI_RESP_EMPTY;

  WITH_SCOPE_BEGIN(p_err) {
    BREAK_ON_ERR(FS_SD_SPI_WITH) (p_sd_spi, p_err) {            // Select/Unselect SPI slave.
                                                                // Get 1st resp byte.
      BREAK_ON_ERR(resp_r1 = FS_SD_SPI_Cmd(p_sd_spi,
                                           cmd,
                                           arg,
                                           p_err));
    }
  } WITH_SCOPE_END

  return (resp_r1);
}

/****************************************************************************************************//**
 *                                           FS_SD_SPI_CmdR3_7()
 *
 * @brief    Perform command & get R3 or R7 response.
 *
 * @param    p_sd_spi    Pointer to SD SPI.
 *
 * @param    cmd         Command index.
 *
 * @param    arg         Command argument.
 *
 * @param    resp        Buffer in which response will be stored.
 *
 * @param    p_err       Error pointer.
 *
 * @return   R1 response.
 *******************************************************************************************************/
static CPU_INT08U FS_SD_SPI_CmdR3_7(FS_SD_SPI  *p_sd_spi,
                                    CPU_INT08U cmd,
                                    CPU_INT32U arg,
                                    CPU_INT08U resp[],
                                    RTOS_ERR   *p_err)
{
  CPU_INT08U resp_r1 = FS_DEV_SD_SPI_RESP_EMPTY;

  WITH_SCOPE_BEGIN(p_err) {
    BREAK_ON_ERR(FS_SD_SPI_WITH) (p_sd_spi, p_err) {
      //                                                           Get 1st resp byte.
      BREAK_ON_ERR(resp_r1 = FS_SD_SPI_Cmd(p_sd_spi,
                                           cmd,
                                           arg,
                                           p_err));

      ASSERT_BREAK((resp_r1 == FS_DEV_SD_SPI_R1_NONE)
                   || (resp_r1 == FS_DEV_SD_SPI_R1_IN_IDLE_STATE),
                   RTOS_ERR_IO);

      //                                                           Rd rest of resp.
      BREAK_ON_ERR(SPI_SlaveRx(p_sd_spi->SpiHandle,
                               resp,
                               4u,
                               FS_DEV_SD_SPI_XFER_TIMEOUT,
                               p_err));
    }
  } WITH_SCOPE_END

  return (resp_r1);
}

/****************************************************************************************************//**
 *                                           FS_SD_SPI_AppCmdR1()
 *
 * @brief    Perform application-specific command & get R1 response.
 *
 * @param    p_sd_spi    Pointer to SD SPI.
 *
 * @param    acmd        Application-specific command index.
 *
 * @param    arg         Command argument.
 *
 * @param    p_err       Error pointer.
 *
 * @return   Command response.
 *******************************************************************************************************/
static CPU_INT08U FS_SD_SPI_AppCmdR1(FS_SD_SPI  *p_sd_spi,
                                     CPU_INT08U acmd,
                                     CPU_INT32U arg,
                                     RTOS_ERR   *p_err)
{
  CPU_INT08U resp_r1 = FS_DEV_SD_SPI_RESP_EMPTY;

  WITH_SCOPE_BEGIN(p_err) {
    BREAK_ON_ERR(FS_SD_SPI_WITH) (p_sd_spi, p_err) {
      //                                                           Next cmd is app cmd.
      BREAK_ON_ERR(resp_r1 = FS_SD_SPI_Cmd(p_sd_spi,
                                           FS_DEV_SD_CMD_APP_CMD,
                                           0u,
                                           p_err));

      ASSERT_BREAK((resp_r1 == FS_DEV_SD_SPI_R1_NONE)
                   || (resp_r1 == FS_DEV_SD_SPI_R1_IN_IDLE_STATE),
                   RTOS_ERR_IO);
      //                                                           Send empty clk.
      BREAK_ON_ERR(FS_SD_SPI_ClksWait(p_sd_spi, 1u, p_err));
      //                                                           Send app cmd.
      BREAK_ON_ERR(resp_r1 = FS_SD_SPI_Cmd(p_sd_spi,
                                           acmd,
                                           arg,
                                           p_err));
    }
  } WITH_SCOPE_END

  return (resp_r1);
}

/****************************************************************************************************//**
 *                                           FS_SD_SPI_DataRd()
 *
 * @brief    Execute command & read data block from card.
 *
 * @param    p_sd_spi    Pointer to SD SPI.
 *
 * @param    cmd         Command index.
 *
 * @param    arg         Command argument.
 *
 * @param    p_dest      Pointer to destination buffer.
 *
 * @param    size        Size of data block to be read, in octets.
 *
 * @param    p_err       Error pointer.
 *
 * @return   DEF_OK   if the data was read.
 *           DEF_FAIL otherwise.
 *******************************************************************************************************/
static void FS_SD_SPI_DataRd(FS_SD_SPI  *p_sd_spi,
                             CPU_INT08U cmd,
                             CPU_INT32U arg,
                             CPU_INT08U *p_dest,
                             CPU_INT32U size,
                             RTOS_ERR   *p_err)
{
  CPU_INT08U crc_buf[2];
#if (FS_SD_SPI_CFG_CRC_EN == DEF_ENABLED)
  CPU_INT16U crc;
  CPU_INT16U crc_chk;
#endif
  CPU_INT08U resp_r1;
  CPU_INT08U token;

  WITH_SCOPE_BEGIN(p_err) {
    BREAK_ON_ERR(FS_SD_SPI_WITH) (p_sd_spi, p_err) {
      //                                                           Perform send CSD command.
      BREAK_ON_ERR(resp_r1 = FS_SD_SPI_Cmd(p_sd_spi,
                                           cmd,
                                           arg,
                                           p_err));

      ASSERT_BREAK(resp_r1 == FS_DEV_SD_SPI_R1_NONE, RTOS_ERR_IO);

      token = FS_SD_SPI_WaitForStart(p_sd_spi);                 // Wait for start token of data block.
      ASSERT_BREAK(token == FS_DEV_SD_SPI_TOKEN_START_BLK, RTOS_ERR_IO);

      //                                                           Rd rest of resp.
      BREAK_ON_ERR(SPI_SlaveRx(p_sd_spi->SpiHandle,
                               p_dest,
                               size,
                               FS_DEV_SD_SPI_XFER_TIMEOUT,
                               p_err));

      //                                                           Rd CRC ...
      BREAK_ON_ERR(SPI_SlaveRx(p_sd_spi->SpiHandle,
                               crc_buf,
                               2u,
                               FS_DEV_SD_SPI_XFER_TIMEOUT,
                               p_err));

#if (FS_SD_SPI_CFG_CRC_EN == DEF_ENABLED)
      crc = MEM_VAL_GET_INT16U_BIG((void *)&crc_buf[0]);        // ... & chk CRC.
      crc_chk = FS_SD_ChkSumCalc_16Bit(p_dest, size);
      ASSERT_BREAK(crc == crc_chk, RTOS_ERR_IO);
#endif
    }
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                           FS_SD_SPI_DataMultiRd()
 *
 * @brief    Execute command & read multiple data blocks from card.
 *
 * @param    p_sd_spi    Pointer to SD SPI.
 *
 * @param    cmd         Command index.
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
 * @return   DEF_OK   if the data was read.
 *           DEF_FAIL otherwise.
 *******************************************************************************************************/
static void FS_SD_SPI_DataMultiRd(FS_SD_SPI  *p_sd_spi,
                                  CPU_INT08U cmd,
                                  CPU_INT32U arg,
                                  CPU_INT08U *p_dest,
                                  CPU_INT32U size,
                                  CPU_INT32U cnt,
                                  RTOS_ERR   *p_err)
{
  CPU_INT08U crc_buf[2];
#if (FS_SD_SPI_CFG_CRC_EN == DEF_ENABLED)
  CPU_INT16U crc;
  CPU_INT16U crc_chk;
#endif
  CPU_INT08U resp_r1;
  CPU_INT08U token;

  WITH_SCOPE_BEGIN(p_err) {
    BREAK_ON_ERR(FS_SD_SPI_WITH) (p_sd_spi, p_err) {
      //                                                           Perform send CSD command.
      BREAK_ON_ERR(resp_r1 = FS_SD_SPI_Cmd(p_sd_spi,
                                           cmd,
                                           arg,
                                           p_err));

      ASSERT_BREAK(resp_r1 == FS_DEV_SD_SPI_R1_NONE, RTOS_ERR_IO);

      BREAK_ON_ERR(while) (cnt > 0u) {
        //                                                         Wait for start token of data block.
        token = FS_SD_SPI_WaitForStart(p_sd_spi);
        ASSERT_BREAK(token == FS_DEV_SD_SPI_TOKEN_START_BLK, RTOS_ERR_IO);

        //                                                         Rd rest of resp.
        BREAK_ON_ERR(SPI_SlaveRx(p_sd_spi->SpiHandle,
                                 p_dest,
                                 size,
                                 FS_DEV_SD_SPI_XFER_TIMEOUT,
                                 p_err));

        //                                                         Rd CRC ...
        BREAK_ON_ERR(SPI_SlaveRx(p_sd_spi->SpiHandle,
                                 &crc_buf[0u],
                                 2u,
                                 FS_DEV_SD_SPI_XFER_TIMEOUT,
                                 p_err));

#if (FS_SD_SPI_CFG_CRC_EN == DEF_ENABLED)
        crc = MEM_VAL_GET_INT16U_BIG((void *)&crc_buf[0]);
        crc_chk = FS_SD_ChkSumCalc_16Bit(p_dest, size);         // ... & chk CRC.
        ASSERT_BREAK(crc == crc_chk, RTOS_ERR_IO);
#endif

        p_dest += size;
        cnt--;
      }
      //                                                           Perform stop transmission command.
      BREAK_ON_ERR((void)FS_SD_SPI_Cmd(p_sd_spi,
                                       FS_DEV_SD_CMD_STOP_TRANSMISSION,
                                       0u,
                                       p_err));

      (void)FS_SD_SPI_WaitWhileBusy(p_sd_spi);                  // Wait while busy token rx'd.
    }
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                           FS_SD_SPI_DataWr()
 *
 * @brief    Execute command & write data block to card.
 *
 * @param    p_sd_spi    Pointer to SD SPI.
 *
 * @param    cmd         Command to execute.
 *
 * @param    arg         Command argument.
 *
 * @param    p_src       Pointer to source buffer.
 *
 * @param    size        Size of data block to be written, in octets.
 *
 * @param    p_err       Error pointer.
 *
 * @return   DEF_OK,   if the data was written.
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_SD_SPI_DataWr(FS_SD_SPI  *p_sd_spi,
                             CPU_INT08U cmd,
                             CPU_INT32U arg,
                             CPU_INT08U *p_src,
                             CPU_INT32U size,
                             RTOS_ERR   *p_err)
{
#if (FS_SD_SPI_CFG_CRC_EN == DEF_ENABLED)
  CPU_INT16U crc;
#endif
  CPU_INT08U crc_buf[2];
  CPU_INT08U resp;
  CPU_INT08U token;

  WITH_SCOPE_BEGIN(p_err) {
    BREAK_ON_ERR(FS_SD_SPI_WITH) (p_sd_spi, p_err) {
      //                                                           Wr cmd & get resp byte.
      BREAK_ON_ERR(resp = FS_SD_SPI_Cmd(p_sd_spi,
                                        cmd,
                                        arg,
                                        p_err));

      ASSERT_BREAK(resp == FS_DEV_SD_SPI_R1_NONE, RTOS_ERR_IO);

      token = FS_DEV_SD_SPI_TOKEN_START_BLK;

      //                                                           Wr token.
      BREAK_ON_ERR(SPI_SlaveTx(p_sd_spi->SpiHandle,
                               &token,
                               1u,
                               FS_DEV_SD_SPI_XFER_TIMEOUT,
                               p_err));

      //                                                           Wr data.
      BREAK_ON_ERR(SPI_SlaveTx(p_sd_spi->SpiHandle,
                               p_src,
                               size,
                               FS_DEV_SD_SPI_XFER_TIMEOUT,
                               p_err));

#if (FS_SD_SPI_CFG_CRC_EN == DEF_ENABLED)
      crc = FS_SD_ChkSumCalc_16Bit(p_src, size);
      MEM_VAL_SET_INT16U_BIG((void *)&crc_buf[0], crc);
#endif

      //                                                           Wr CRC.
      BREAK_ON_ERR(SPI_SlaveTx(p_sd_spi->SpiHandle,
                               &crc_buf[0],
                               2u,
                               FS_DEV_SD_SPI_XFER_TIMEOUT,
                               p_err));

      //                                                           Rd data resp token.
      BREAK_ON_ERR(SPI_SlaveRx(p_sd_spi->SpiHandle,
                               &resp,
                               1u,
                               FS_DEV_SD_SPI_XFER_TIMEOUT,
                               p_err));

      //                                                           Chk data resp token.
      if ((resp & FS_DEV_SD_SPI_TOKEN_RESP_MASK) != FS_DEV_SD_SPI_TOKEN_RESP_ACCEPTED) {
        //                                                         Wait while busy token rx'd.
        (void)FS_SD_SPI_WaitWhileBusy(p_sd_spi);
        BREAK_ERR_SET(RTOS_ERR_IO);
      }

      resp = FS_SD_SPI_WaitWhileBusy(p_sd_spi);                 // Wait while busy token rx'd.
      ASSERT_BREAK(resp == FS_DEV_SD_SPI_RESP_EMPTY, RTOS_ERR_IO);
    }
  } WITH_SCOPE_END
}
#endif

/****************************************************************************************************//**
 *                                           FS_SD_SPI_DataMultiWr()
 *
 * @brief    Execute command & write multiple data blocks to card.
 *
 * @param    p_sd_spi    Pointer to SD SPI.
 *
 * @param    cmd         Command to execute.
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
 * @return   DEF_OK,   if the data was written.
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_SD_SPI_DataMultiWr(FS_SD_SPI  *p_sd_spi,
                                  CPU_INT08U cmd,
                                  CPU_INT32U arg,
                                  CPU_INT08U *p_src,
                                  CPU_INT32U size,
                                  CPU_INT32U cnt,
                                  RTOS_ERR   *p_err)
{
#if (FS_SD_SPI_CFG_CRC_EN == DEF_ENABLED)
  CPU_INT16U crc;
#endif
  CPU_INT08U crc_buf[2];
  CPU_INT08U resp;
  CPU_INT08U token;

  WITH_SCOPE_BEGIN(p_err) {
    BREAK_ON_ERR(FS_SD_SPI_WITH) (p_sd_spi, p_err) {
      //                                                           Perform send CSD command.
      BREAK_ON_ERR(resp = FS_SD_SPI_Cmd(p_sd_spi,
                                        cmd,
                                        arg,
                                        p_err));

      ASSERT_BREAK(resp == FS_DEV_SD_SPI_R1_NONE, RTOS_ERR_IO);

      BREAK_ON_ERR(while) (cnt > 0u) {
        token = FS_DEV_SD_SPI_TOKEN_START_BLK_MULT;

        //                                                         Wr token.
        BREAK_ON_ERR(SPI_SlaveTx(p_sd_spi->SpiHandle,
                                 &token,
                                 1u,
                                 FS_DEV_SD_SPI_XFER_TIMEOUT,
                                 p_err));

        //                                                         Wr data.
        BREAK_ON_ERR(SPI_SlaveTx(p_sd_spi->SpiHandle,
                                 p_src,
                                 size,
                                 FS_DEV_SD_SPI_XFER_TIMEOUT,
                                 p_err));

#if (FS_SD_SPI_CFG_CRC_EN == DEF_ENABLED)
        crc = FS_SD_ChkSumCalc_16Bit(p_src, size);
        MEM_VAL_SET_INT16U_BIG((void *)&crc_buf[0], crc);
#endif

        //                                                         Wr CRC.
        BREAK_ON_ERR(SPI_SlaveTx(p_sd_spi->SpiHandle,
                                 &crc_buf[0],
                                 2u,
                                 FS_DEV_SD_SPI_XFER_TIMEOUT,
                                 p_err));

        //                                                         Rd data resp token.
        BREAK_ON_ERR(SPI_SlaveRx(p_sd_spi->SpiHandle,
                                 &resp,
                                 1u,
                                 FS_DEV_SD_SPI_XFER_TIMEOUT,
                                 p_err));

        if ((resp & FS_DEV_SD_SPI_TOKEN_RESP_MASK) != FS_DEV_SD_SPI_TOKEN_RESP_ACCEPTED) {
          //                                                       Wait while busy token rx'd.
          (void)FS_SD_SPI_WaitWhileBusy(p_sd_spi);
          BREAK_ERR_SET(RTOS_ERR_IO);
        }

        //                                                         Wait while busy token rx'd.
        resp = FS_SD_SPI_WaitWhileBusy(p_sd_spi);
        ASSERT_BREAK(resp == FS_DEV_SD_SPI_RESP_EMPTY, RTOS_ERR_IO);

        p_src += size;
        cnt--;
      }

      token = FS_DEV_SD_SPI_TOKEN_STOP_TRAN;
      //                                                           Wr token.
      BREAK_ON_ERR(SPI_SlaveTx(p_sd_spi->SpiHandle,
                               &token,
                               1u,
                               FS_DEV_SD_SPI_XFER_TIMEOUT,
                               p_err));

      resp = FS_SD_SPI_WaitWhileBusy(p_sd_spi);                 // Wait while busy token rx'd.
      ASSERT_BREAK(resp == FS_DEV_SD_SPI_RESP_EMPTY, RTOS_ERR_IO);
    }
  } WITH_SCOPE_END
}
#endif

/****************************************************************************************************//**
 *                                           FS_SD_SPI_CID_Send()
 *
 * @brief    Get Card ID (CID) register from card.
 *
 * @param    p_sd_spi    Pointer to SD SPI.
 *
 * @param    p_dest      Pointer to 16-byte buffer that will receive SD/MMC Card ID register.
 *
 * @param    p_err       Error pointer.
 *
 * @return   DEF_OK,   if the data was read.
 *           DEF_FAIL, otherwise.
 *
 * @note     (1) For SD cards, the structure of the CID is defined in [Ref 1] Section 5.1.
 *               For MMC cards, the structure of the CID is defined in [Ref 2] Section 8.2.
 *******************************************************************************************************/
static void FS_SD_SPI_CID_Send(FS_SD_SPI  *p_sd_spi,
                               CPU_INT08U *p_dest,
                               RTOS_ERR   *p_err)
{
  FS_SD_SPI_DataRd(p_sd_spi,
                   FS_DEV_SD_CMD_SEND_CID,
                   0u,
                   p_dest,
                   FS_DEV_SD_CID_REG_LEN,
                   p_err);
}

/****************************************************************************************************//**
 *                                           FS_SD_SPI_CSD_Send()
 *
 * @brief    Get Card-Specific Data (CSD) register from card.
 *
 * @param    p_sd_spi    Pointer to SD SPI.
 *
 * @param    p_dest      Pointer to 16-byte buffer that will receive SD/MMC Card-Specific Data register.
 *
 * @param    p_err       Error pointer.
 *
 * @return   DEF_OK,   if the data was read.
 *           DEF_FAIL, otherwise.
 *
 * @note     (1) For SD v1.x & SD v2.0 standard capacity, the structure of the CSD is defined in
 *               [Ref 1], Section 5.3.2.
 *               For MMC cards, the structure of the CSD is defined in [Ref 2], Section 8.3.
 *               For SD v2.0 high capacity cards, the structure of the CSD is defined in [Ref 1],
 *               Section 5.3.3.
 *******************************************************************************************************/
static void FS_SD_SPI_CSD_Send(FS_SD_SPI  *p_sd_spi,
                               CPU_INT08U *p_dest,
                               RTOS_ERR   *p_err)
{
  FS_SD_SPI_DataRd(p_sd_spi,
                   FS_DEV_SD_CMD_SEND_CSD,
                   0u,
                   p_dest,
                   FS_DEV_SD_CSD_REG_LEN,
                   p_err);
}

/****************************************************************************************************//**
 *                                           FS_SD_SPI_ClksWait()
 *
 * @brief    Send empty data for a certain number of clocks.
 *
 * @param    p_sd_spi    Pointer to SD SPI.
 *
 * @param    nbr_clks    Number of clock cycles.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_SD_SPI_ClksWait(FS_SD_SPI  *p_sd_spi,
                               CPU_INT08U nbr_clks,
                               RTOS_ERR   *p_err)
{
  CPU_INT08U clks;
  CPU_INT08U data;

  clks = 0u;
  while (clks < nbr_clks) {
    data = FS_DEV_SD_SPI_RESP_EMPTY;

    SPI_SlaveTx(p_sd_spi->SpiHandle,
                &data,
                1u,
                FS_DEV_SD_SPI_XFER_TIMEOUT,
                p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    clks++;
  }
}

/****************************************************************************************************//**
 *                                           FS_SD_SPI_WaitForStart()
 *
 * @brief    Wait for card to return start token.
 *
 * @param    p_sd_spi    Pointer to SD SPI.
 *
 * @return   Final token.
 *******************************************************************************************************/
static CPU_INT08U FS_SD_SPI_WaitForStart(FS_SD_SPI *p_sd_spi)
{
  CPU_INT32U clks;
  CPU_INT08U datum;
  RTOS_ERR   err;

  RTOS_ERR_SET(err, RTOS_ERR_NONE);

  clks = 0u;
  datum = FS_DEV_SD_SPI_RESP_EMPTY;
  while (clks < 312500u) {
    SPI_SlaveRx(p_sd_spi->SpiHandle,
                &datum,
                1u,
                FS_DEV_SD_SPI_XFER_TIMEOUT,
                &err);
    if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
      return (FS_DEV_SD_SPI_RESP_EMPTY);
    }
    if (datum != FS_DEV_SD_SPI_RESP_EMPTY) {
      return (datum);
    }

    datum = FS_DEV_SD_SPI_RESP_EMPTY;

    clks++;
  }

  return (datum);
}

/****************************************************************************************************//**
 *                                           FS_SD_SPI_WaitWhileBusy()
 *
 * @brief    Wait while card returns busy token.
 *
 * @param    p_sd_spi    Pointer to SD SPI.
 *
 * @return   Final token.
 *******************************************************************************************************/
static CPU_INT08U FS_SD_SPI_WaitWhileBusy(FS_SD_SPI *p_sd_spi)
{
  CPU_INT32U clks;
  CPU_INT08U datum;
  RTOS_ERR   err;

  RTOS_ERR_SET(err, RTOS_ERR_NONE);

  clks = 0u;
  datum = 0x00u;
  while (clks < 781250u) {
    SPI_SlaveRx(p_sd_spi->SpiHandle,
                &datum,
                1u,
                FS_DEV_SD_SPI_XFER_TIMEOUT,
                &err);
    if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
      return (0x00);
    }

    if (datum == FS_DEV_SD_SPI_RESP_EMPTY) {
      return (datum);
    }

    datum = 0x00u;

    clks++;
  }

  return (datum);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_FS_STORAGE_SD_SPI_AVAIL
