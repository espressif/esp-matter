/***************************************************************************//**
 * @file
 * @brief File System - Block Device Operations
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
 ********************************************************************************************************
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include <rtos_description.h>

#if (defined(RTOS_MODULE_FS_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#define    FS_BLK_DEV_MODULE
//                                                                 ------------------------ FS ------------------------
#include  <fs_storage_cfg.h>
#include  <fs/source/storage/fs_blk_dev_priv.h>
#include  <fs/source/shared/fs_utils_priv.h>
#if (FS_STORAGE_CFG_DBG_WR_VERIFY_EN == DEF_ENABLED)
#include  <fs/source/shared/crc/edc_crc.h>
#endif

#ifdef  RTOS_MODULE_FS_STORAGE_RAM_DISK_AVAIL
#include  <fs/include/fs_ramdisk.h>
#endif

#ifdef  RTOS_MODULE_FS_STORAGE_NAND_AVAIL
#include  <fs/source/storage/nand/fs_nand_ftl_priv.h>
#endif

#ifdef  RTOS_MODULE_FS_STORAGE_NOR_AVAIL
#include  <fs/source/storage/nor/fs_nor_ftl_priv.h>
#endif

#ifdef  RTOS_MODULE_FS_STORAGE_SCSI_AVAIL
#include  <fs/source/storage/scsi/fs_scsi_priv.h>
#endif

#if  defined(RTOS_MODULE_FS_STORAGE_SD_SPI_AVAIL) \
  || defined(RTOS_MODULE_FS_STORAGE_SD_CARD_AVAIL)
#include  <fs/source/storage/sd/fs_sd_priv.h>
#endif

//                                                                 ----------------------- EXT ------------------------
#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>
#include  <common/include/rtos_prio.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/kal/kal_priv.h>

#include  <em_core.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define    RTOS_MODULE_CUR    RTOS_CFG_MODULE_FS
#define    LOG_DFLT_CH        (FS, BLK_DEV)

/********************************************************************************************************
 ********************************************************************************************************
 *                                               VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

FS_BLK_DEV_DATA FSBlkDev_Data = {
  .IsInit = DEF_NO,
  .BlkDevId = 0u,
  .BlkDevListHeadPtr = DEF_NULL,
  .BlkDevListLockHandle = KAL_LOCK_HANDLE_NULL
};

const FS_BLK_DEV_HANDLE FSBlkDev_NullHandle = { 0 };

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void FSBlkDev_ListLock(void);

static void FSBlkDev_ListUnlock(void);

static void FSBlkDev_ListPush(FS_BLK_DEV *p_blk_dev);

static void FSBlkDev_ListRem(FS_BLK_DEV *p_blk_dev);

static FS_BLK_DEV_HANDLE FSBlkDev_FromObjHandle(FS_OBJ_HANDLE obj_handle);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               FSBlkDev_Init()
 *
 * @brief    Initialize the block device layer.
 *
 * @param    p_err   Pointer to variable that will receive the return error code(s) from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_SEG_OVF
 *******************************************************************************************************/
void FSBlkDev_Init(RTOS_ERR *p_err)
{
  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    RTOS_ASSERT_DBG(!FSBlkDev_Data.IsInit, RTOS_ERR_ALREADY_INIT,; );

    BREAK_ON_ERR(FSBlkDev_Data.BlkDevListLockHandle = KAL_LockCreate("FS block device list lock",
                                                                     DEF_NULL,
                                                                     p_err));

#ifdef RTOS_MODULE_FS_STORAGE_NOR_AVAIL
    BREAK_ON_ERR(FS_NOR_FTL_Init(p_err));
#endif

#ifdef RTOS_MODULE_FS_STORAGE_NAND_AVAIL
    BREAK_ON_ERR(FS_NAND_FTL_Init(p_err));
#endif

#ifdef  RTOS_MODULE_FS_STORAGE_SCSI_AVAIL
    BREAK_ON_ERR(FS_SCSI_Init(p_err));
#endif

    FSBlkDev_Data.IsInit = DEF_YES;
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                               FSBlkDev_Open()
 *
 * @brief    Open a block device.
 *
 * @param    media_handle    Handle to a media.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_INIT
 *                               - RTOS_ERR_BLK_DEV_OPENED
 *                               - RTOS_ERR_BLK_DEV_FMT_INCOMPATIBLE
 *                               - RTOS_ERR_BLK_DEV_FMT_INVALID
 *                               - RTOS_ERR_IO
 *
 * @return   Handle to the opened block device.
 *******************************************************************************************************/
FS_BLK_DEV_HANDLE FSBlkDev_Open(FS_MEDIA_HANDLE media_handle,
                                RTOS_ERR        *p_err)
{
  FS_MEDIA             *p_media;
  FS_BLK_DEV           *p_blk_dev;
  const FS_BLK_DEV_API *p_blk_dev_api = DEF_NULL;
  FS_BLK_DEV_INFO      blk_dev_info;
  FS_BLK_DEV_HANDLE    blk_dev_handle;
  CORE_DECLARE_IRQ_STATE;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, FSBlkDev_NullHandle);
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    //                                                             Check that Storage layer is initialized.
    CORE_ENTER_ATOMIC();
    if (!FSBlkDev_Data.IsInit) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_INIT);
      CORE_EXIT_ATOMIC();
      break;
    }
    CORE_EXIT_ATOMIC();

    blk_dev_handle = FSBlkDev_Get(media_handle);
    ASSERT_BREAK(FS_BLK_DEV_HANDLE_IS_NULL(blk_dev_handle), RTOS_ERR_BLK_DEV_OPENED);

    FSObj_RefAcquire(FSMedia_ToObjHandle(media_handle));
    ON_BREAK {
      FSObj_RefRelease(FSMedia_ToObj(p_media), FSMedia_OnNullRefCnt);
      break;
    } WITH {
      p_media = media_handle.MediaPtr;

      switch (p_media->PmItemPtr->PmItem.Type) {
#ifdef RTOS_MODULE_FS_STORAGE_NAND_AVAIL
        case PLATFORM_MGR_ITEM_TYPE_HW_INFO_FS_NAND:
          p_blk_dev_api = &FS_NAND_BlkDevApi;
          break;
#endif
#ifdef RTOS_MODULE_FS_STORAGE_NOR_AVAIL
        case PLATFORM_MGR_ITEM_TYPE_HW_INFO_FS_NOR:
          p_blk_dev_api = &FS_NOR_BlkDevApi;
          break;
#endif
#ifdef RTOS_MODULE_FS_STORAGE_SD_CARD_AVAIL
        case PLATFORM_MGR_ITEM_TYPE_HW_INFO_FS_SD_CARD:
          p_blk_dev_api = &FS_SD_Card_BlkDevApi;
          break;
#endif
#ifdef RTOS_MODULE_FS_STORAGE_SD_SPI_AVAIL
        case PLATFORM_MGR_ITEM_TYPE_HW_INFO_FS_SD_SPI:
          p_blk_dev_api = &FS_SD_SPI_BlkDevApi;
          break;
#endif
#ifdef RTOS_MODULE_FS_VDI_AVAIL
        case PLATFORM_MGR_ITEM_TYPE_HW_INFO_FS_VDI:
          p_blk_dev_api = &FS_VDI_BlkDevApi;
          break;
#endif
#ifdef RTOS_MODULE_FS_STORAGE_SCSI_AVAIL
        case PLATFORM_MGR_ITEM_TYPE_HW_INFO_FS_SCSI:
          p_blk_dev_api = &FS_SCSI_BlkDevApi;
          break;
#endif
#ifdef RTOS_MODULE_FS_STORAGE_RAM_DISK_AVAIL
        case PLATFORM_MGR_ITEM_TYPE_HW_INFO_FS_RAM_DISK:
          p_blk_dev_api = &FS_RAM_Disk_BlkDevApi;
          break;
#endif

        default:
          RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, FSBlkDev_NullHandle);
      }

      BREAK_ON_ERR(p_blk_dev = p_blk_dev_api->Add(p_media, p_err));

      ON_BREAK {
        RTOS_ERR err_tmp = RTOS_ERR_INIT_CODE(RTOS_ERR_NONE);
        p_blk_dev_api->Rem(p_blk_dev, &err_tmp);
        break;
      } WITH {
        p_blk_dev->MediaPtr = p_media;
        p_blk_dev->MediaId = p_media->Id;
        p_blk_dev->RefCnt = 1u;
        p_blk_dev->IOCnt = 1u;
        p_blk_dev->OnClose = DEF_NULL;
        p_blk_dev->Id = FSBlkDev_Data.BlkDevId++;
        p_blk_dev->BlkDevApiPtr = p_blk_dev_api;
        p_blk_dev->IsClosing = DEF_NO;

        BREAK_ON_ERR(FS_MEDIA_LOCK_WITH) (p_blk_dev->MediaPtr) {
          BREAK_ON_ERR(p_blk_dev_api->Open(p_blk_dev, p_err));
          ON_BREAK {
            RTOS_ERR err_tmp = RTOS_ERR_INIT_CODE(RTOS_ERR_NONE);
            p_blk_dev_api->Close(p_blk_dev, &err_tmp);
            break;
          } WITH {
            BREAK_ON_ERR(p_blk_dev_api->Query(p_blk_dev,
                                              &blk_dev_info,
                                              p_err));

            p_blk_dev->LbCnt = blk_dev_info.LbCnt;
            p_blk_dev->LbSizeLog2 = blk_dev_info.LbSizeLog2;

            BREAK_ON_ERR(p_blk_dev->LockHandle = KAL_LockCreate("FS block device lock",
                                                                DEF_NULL,
                                                                p_err));

#if (FS_STORAGE_CFG_CTR_STAT_EN == DEF_ENABLED)
            p_blk_dev->StatRdCtr = 0u;
            p_blk_dev->StatWrCtr = 0u;
#endif

            FSBlkDev_ListPush(p_blk_dev);

            blk_dev_handle.BlkDevPtr = p_blk_dev;
            blk_dev_handle.BlkDevId = p_blk_dev->Id;
          }
        }
      }
    }
  } WITH_SCOPE_END

  return (blk_dev_handle);
}

/****************************************************************************************************//**
 *                                               FSBlkDev_Close()
 *
 * @brief    Close a block device.
 *
 * @param    blk_dev_handle  Handle to a block device.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_BLK_DEV_CLOSED
 *******************************************************************************************************/
void FSBlkDev_Close(FS_BLK_DEV_HANDLE blk_dev_handle,
                    RTOS_ERR          *p_err)
{
  FS_BLK_DEV *p_blk_dev = DEF_NULL;
  RTOS_ERR   err_tmp = RTOS_ERR_INIT_CODE(RTOS_ERR_NONE);

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    FS_BLK_DEV_WITH_NO_IO(blk_dev_handle, p_err) {
      p_blk_dev = blk_dev_handle.BlkDevPtr;
      ASSERT_BREAK(!FSObj_IsClosingTestSet(FSBlkDev_ToObj(p_blk_dev)),
                   RTOS_ERR_BLK_DEV_CLOSED);
    }

    if (p_blk_dev->OnClose != DEF_NULL) {
      p_blk_dev->OnClose(blk_dev_handle, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        err_tmp = *p_err;
      }
    }

    FSObj_IOEndWait(FSBlkDev_ToObj(p_blk_dev));

    p_blk_dev->BlkDevApiPtr->Close(p_blk_dev, p_err);
    if ((RTOS_ERR_CODE_GET(err_tmp) == RTOS_ERR_NONE)
        && (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE)) {
      err_tmp = *p_err;
    }

    FSObj_RefRelease(FSBlkDev_ToObj(p_blk_dev), FSBlkDev_OnNullRefCnt);

    *p_err = err_tmp;
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                               FSBlkDev_Rd()
 *
 * @brief    Read blocks from a block device.
 *
 * @param    blk_dev_handle  Handle to a block device.
 *
 * @param    p_dest          Pointer to a destination buffer.
 *
 * @param    start_lb_nbr    Logical block number of the block to start reading from.
 *
 * @param    lb_cnt          Number of logical blocks to read.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_BLK_DEV_CLOSED
 *                               - RTOS_ERR_BLK_DEV_CORRUPTED
 *                               - RTOS_ERR_IO
 *******************************************************************************************************/
void FSBlkDev_Rd(FS_BLK_DEV_HANDLE blk_dev_handle,
                 void              *p_dest,
                 FS_LB_NBR         start_lb_nbr,
                 FS_LB_QTY         lb_cnt,
                 RTOS_ERR          *p_err)
{
  FS_BLK_DEV *p_blk_dev;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    BREAK_ON_ERR(FS_BLK_DEV_WITH) (blk_dev_handle, p_err) {
      p_blk_dev = blk_dev_handle.BlkDevPtr;

      BREAK_ON_ERR(FS_MEDIA_LOCK_WITH) (p_blk_dev->MediaPtr) {
        ASSERT_BREAK(p_blk_dev->Id == blk_dev_handle.BlkDevId,
                     RTOS_ERR_BLK_DEV_CLOSED);

        BREAK_ON_ERR(p_blk_dev->BlkDevApiPtr->Rd(p_blk_dev,
                                                 p_dest,
                                                 start_lb_nbr,
                                                 lb_cnt,
                                                 p_err));

        FS_BLK_DEV_CTR_STAT_ADD(p_blk_dev->StatRdCtr, lb_cnt);
      }
    }
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                               FSBlkDev_Wr()
 *
 * @brief    Write blocks to a block device.
 *
 * @param    blk_dev_handle  Handle to a block device.
 *
 * @param    p_src           Pointer to a source buffer.
 *
 * @param    start_lb_nbr    Logical block number of the block to start writing to.
 *
 * @param    lb_cnt          Number of logical blocks to write.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_BLK_DEV_CLOSED
 *                               - RTOS_ERR_BLK_DEV_CORRUPTED
 *                               - RTOS_ERR_IO
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FSBlkDev_Wr(FS_BLK_DEV_HANDLE blk_dev_handle,
                 void              *p_src,
                 FS_LB_NBR         start_lb_nbr,
                 FS_LB_QTY         lb_cnt,
                 RTOS_ERR          *p_err)
{
  FS_BLK_DEV *p_blk_dev;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    BREAK_ON_ERR(FS_BLK_DEV_WITH) (blk_dev_handle, p_err) {
      p_blk_dev = blk_dev_handle.BlkDevPtr;

      BREAK_ON_ERR(FS_MEDIA_LOCK_WITH) (p_blk_dev->MediaPtr) {
        ASSERT_BREAK(p_blk_dev->Id == blk_dev_handle.BlkDevId,
                     RTOS_ERR_BLK_DEV_CLOSED);

        BREAK_ON_ERR(p_blk_dev->BlkDevApiPtr->Wr(p_blk_dev,
                                                 p_src,
                                                 start_lb_nbr,
                                                 lb_cnt,
                                                 p_err));

#if ((FS_STORAGE_CFG_DBG_WR_VERIFY_EN == DEF_ENABLED) \
        && (FS_STORAGE_CFG_CRC32_EN == DEF_ENABLED))
        {
          CPU_INT32U rd_chksum;
          CPU_INT32U wr_chksum;

          wr_chksum = CRC_ChkSumCalc_32Bit((CRC_MODEL_32 *)&CRC_ModelCRC32,
                                           p_src,
                                           FS_UTIL_MULT_PWR2(lb_cnt, p_blk_dev->LbSizeLog2));

          BREAK_ON_ERR(p_blk_dev->BlkDevApiPtr->Rd(p_blk_dev,
                                                   p_src,
                                                   start_lb_nbr,
                                                   lb_cnt,
                                                   p_err));

          rd_chksum = CRC_ChkSumCalc_32Bit((CRC_MODEL_32 *)&CRC_ModelCRC32,
                                           p_src,
                                           FS_UTIL_MULT_PWR2(lb_cnt, p_blk_dev->LbSizeLog2));

          ASSERT_BREAK(rd_chksum == wr_chksum, RTOS_ERR_IO);
        }
#endif

        FS_BLK_DEV_CTR_STAT_ADD(p_blk_dev->StatWrCtr, lb_cnt);
      }
    }
  } WITH_SCOPE_END
}
#endif

/****************************************************************************************************//**
 *                                               FSBlkDev_Sync()
 *
 * @brief    Sync a block device.
 *
 * @param    blk_dev_handle  Handle to a block device.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_BLK_DEV_CLOSED
 *                               - RTOS_ERR_BLK_DEV_CORRUPTED
 *                               - RTOS_ERR_IO
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FSBlkDev_Sync(FS_BLK_DEV_HANDLE blk_dev_handle,
                   RTOS_ERR          *p_err)
{
  FS_BLK_DEV *p_blk_dev;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    BREAK_ON_ERR(FS_BLK_DEV_WITH) (blk_dev_handle, p_err) {
      p_blk_dev = blk_dev_handle.BlkDevPtr;

      BREAK_ON_ERR(FS_MEDIA_LOCK_WITH) (p_blk_dev->MediaPtr) {
        ASSERT_BREAK(p_blk_dev->Id == blk_dev_handle.BlkDevId, RTOS_ERR_BLK_DEV_CLOSED);
        BREAK_ON_ERR(p_blk_dev->BlkDevApiPtr->Sync(p_blk_dev, p_err));
      }
    }
  } WITH_SCOPE_END
}
#endif

/****************************************************************************************************//**
 *                                               FSBlkDev_Trim()
 *
 * @brief    Trim a block device.
 *
 * @param    blk_dev_handle  Handle to a block device.
 *
 * @param    start_lb_nbr    Logical block number of the block to start trimming from.
 *
 * @param    lb_cnt          Number of logical blocks to trim.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_BLK_DEV_CLOSED
 *                               - RTOS_ERR_BLK_DEV_CORRUPTED
 *                               - RTOS_ERR_IO
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FSBlkDev_Trim(FS_BLK_DEV_HANDLE blk_dev_handle,
                   FS_LB_NBR         start_lb_nbr,
                   FS_LB_QTY         lb_cnt,
                   RTOS_ERR          *p_err)
{
  FS_BLK_DEV *p_blk_dev;
  FS_LB_NBR  cur_lb_nbr;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    BREAK_ON_ERR(FS_BLK_DEV_WITH) (blk_dev_handle, p_err) {
      p_blk_dev = blk_dev_handle.BlkDevPtr;

      BREAK_ON_ERR(FS_MEDIA_LOCK_WITH) (p_blk_dev->MediaPtr) {
        ASSERT_BREAK(p_blk_dev->Id == blk_dev_handle.BlkDevId,
                     RTOS_ERR_BLK_DEV_CLOSED);

        cur_lb_nbr = start_lb_nbr;
        BREAK_ON_ERR(while) (cur_lb_nbr < start_lb_nbr + lb_cnt) {
          BREAK_ON_ERR(p_blk_dev->BlkDevApiPtr->Trim(p_blk_dev,
                                                     cur_lb_nbr,
                                                     p_err));
          cur_lb_nbr += 1u;
        }
      }
    }
  } WITH_SCOPE_END
}
#endif

/****************************************************************************************************//**
 *                                           FSBlkDev_LbSizeLog2Get()
 *
 * @brief    Get base-2 logarithm of the logical block size.
 *
 * @param    blk_dev_handle  Handle to a block device.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_BLK_DEV_CLOSED
 *
 * @return   Base-2 logarithm of the logical block size.
 *******************************************************************************************************/
CPU_INT08U FSBlkDev_LbSizeLog2Get(FS_BLK_DEV_HANDLE blk_dev_handle,
                                  RTOS_ERR          *p_err)
{
  FS_BLK_DEV *p_blk_dev;
  CPU_INT08U lb_size_log2 = 0u;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    BREAK_ON_ERR(FS_BLK_DEV_WITH) (blk_dev_handle, p_err) {
      p_blk_dev = blk_dev_handle.BlkDevPtr;
      BREAK_ON_ERR(FS_MEDIA_LOCK_WITH) (p_blk_dev->MediaPtr) {
        ASSERT_BREAK(p_blk_dev->Id == blk_dev_handle.BlkDevId,
                     RTOS_ERR_BLK_DEV_CLOSED);

        lb_size_log2 = p_blk_dev->LbSizeLog2;
      }
    }
  } WITH_SCOPE_END

  return (lb_size_log2);
}

/****************************************************************************************************//**
 *                                           FSBlkDev_LbSizeGet()
 *
 * @brief    Get logical block size.
 *
 * @param    blk_dev_handle  Handle to a block device.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_BLK_DEV_CLOSED
 *
 * @return   Logical block size.
 *******************************************************************************************************/
FS_LB_SIZE FSBlkDev_LbSizeGet(FS_BLK_DEV_HANDLE blk_dev_handle,
                              RTOS_ERR          *p_err)
{
  CPU_INT08U lb_size_log2;

  lb_size_log2 = FSBlkDev_LbSizeLog2Get(blk_dev_handle, p_err);

  return (FS_UTIL_PWR2(lb_size_log2));
}

/****************************************************************************************************//**
 *                                           FSBlkDev_LbCntGet()
 *
 * @brief    Get the number of logical block on a block device.
 *
 * @param    blk_dev_handle  Handle to a block device.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_BLK_DEV_CLOSED
 *
 * @return   Number of logical blocks, if NO error(s).
 *           0,                        otherwise.
 *******************************************************************************************************/
FS_LB_QTY FSBlkDev_LbCntGet(FS_BLK_DEV_HANDLE blk_dev_handle,
                            RTOS_ERR          *p_err)
{
  FS_BLK_DEV *p_blk_dev;
  FS_LB_NBR  lb_cnt = 0u;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, RTOS_ERR_NONE);
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    BREAK_ON_ERR(FS_BLK_DEV_WITH) (blk_dev_handle, p_err) {
      p_blk_dev = blk_dev_handle.BlkDevPtr;
      BREAK_ON_ERR(FS_MEDIA_LOCK_WITH) (p_blk_dev->MediaPtr) {
        ASSERT_BREAK(p_blk_dev->Id == blk_dev_handle.BlkDevId,
                     RTOS_ERR_BLK_DEV_CLOSED);

        lb_cnt = p_blk_dev->LbCnt;
      }
    }
  } WITH_SCOPE_END

  return (lb_cnt);
}

/****************************************************************************************************//**
 *                                           FSBlkDev_AlignReqGet()
 *
 * @brief    Get the alignment requirement.
 *
 * @param    blk_dev_handle  Handle to a block device.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_BLK_DEV_CLOSED
 *
 * @return   Alignment requirement in bytes, if NO error(s).
 *           0,                              otherwise.
 *******************************************************************************************************/
CPU_SIZE_T FSBlkDev_AlignReqGet(FS_BLK_DEV_HANDLE blk_dev_handle,
                                RTOS_ERR          *p_err)
{
  FS_BLK_DEV *p_blk_dev;
  CPU_SIZE_T align_req = 0u;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, RTOS_ERR_NONE);
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    BREAK_ON_ERR(FS_BLK_DEV_WITH) (blk_dev_handle, p_err) {
      p_blk_dev = blk_dev_handle.BlkDevPtr;
      BREAK_ON_ERR(FS_MEDIA_LOCK_WITH) (p_blk_dev->MediaPtr) {
        ASSERT_BREAK(p_blk_dev->Id == blk_dev_handle.BlkDevId,
                     RTOS_ERR_BLK_DEV_CLOSED);

        align_req = p_blk_dev->MediaPtr->PmItemPtr->MediaApiPtr->AlignReqGet(p_blk_dev->MediaPtr, p_err);
      }
    }
  } WITH_SCOPE_END

  return (align_req);
}

/****************************************************************************************************//**
 *                                           FSBlkDev_OnCloseCbSet()
 *
 * @brief    Set the block device on close callback.
 *
 * @param    blk_dev_handle  Handle to a block device.
 *
 * @param    on_close        Pointer to on close callback.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_BLK_DEV_CLOSED
 *******************************************************************************************************/
void FSBlkDev_OnCloseCbSet(FS_BLK_DEV_HANDLE   blk_dev_handle,
                           FS_BLK_DEV_ON_CLOSE on_close,
                           RTOS_ERR            *p_err)
{
  FS_BLK_DEV *p_blk_dev;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    BREAK_ON_ERR(FS_BLK_DEV_WITH) (blk_dev_handle, p_err) {
      p_blk_dev = blk_dev_handle.BlkDevPtr;
      BREAK_ON_ERR(FS_MEDIA_LOCK_WITH) (p_blk_dev->MediaPtr) {
        ASSERT_BREAK(p_blk_dev->Id == blk_dev_handle.BlkDevId,
                     RTOS_ERR_BLK_DEV_CLOSED);

        p_blk_dev->OnClose = on_close;
      }
    }
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                               FSBlkDev_Get()
 *
 * @brief    Get a block device handle from the media handle.
 *
 * @param    media_handle    Handle to a media.
 *
 * @return   Handle to the block device.
 *
 * @note     (1) If a block device has no been open, the returned handle is NULL. You may check for
 *               NULL handle using the macro FS_BLK_DEV_HANDLE_IS_NULL().
 *******************************************************************************************************/
FS_BLK_DEV_HANDLE FSBlkDev_Get(FS_MEDIA_HANDLE media_handle)
{
  FS_BLK_DEV        *p_cur_blk_dev;
  FS_BLK_DEV_HANDLE blk_dev_handle = FSBlkDev_NullHandle;

  SLIST_FOR_EACH_ENTRY(FSBlkDev_Data.BlkDevListHeadPtr, p_cur_blk_dev, FS_BLK_DEV, OpenListMember) {
    if ((p_cur_blk_dev->MediaPtr == media_handle.MediaPtr)
        && (p_cur_blk_dev->MediaId == media_handle.MediaId)) {
      blk_dev_handle.BlkDevPtr = p_cur_blk_dev;
      blk_dev_handle.BlkDevId = p_cur_blk_dev->Id;
      break;
    }
  }

  return (blk_dev_handle);
}

/****************************************************************************************************//**
 *                                           FSBlkDev_NameGet()
 *
 * @brief    Get block device name.
 *
 * @param    blk_dev_handle  Handle to a block device.
 *
 * @param    p_buf           Pointer to a buffer that will receive the block device name.
 *
 * @param    buf_size        Size of the given buffer.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_BLK_DEV_CLOSED
 *******************************************************************************************************/
void FSBlkDev_NameGet(FS_BLK_DEV_HANDLE blk_dev_handle,
                      CPU_CHAR          *p_buf,
                      CPU_SIZE_T        buf_size,
                      RTOS_ERR          *p_err)
{
  FS_MEDIA_HANDLE media_handle;

  FS_BLK_DEV_WITH(blk_dev_handle, p_err) {
    media_handle.MediaPtr = blk_dev_handle.BlkDevPtr->MediaPtr;
    media_handle.MediaId = blk_dev_handle.BlkDevPtr->MediaId;
    FSMedia_NameGet(media_handle, p_buf, buf_size, p_err);
  }
}

/****************************************************************************************************//**
 *                                           FSBlkDev_MediaGet()
 *
 * @brief    Get parent media.
 *
 * @param    blk_dev_handle  Handle to a block device.
 *
 * @return   Handle to the parent media or NULL handle if the block device handle is invalid.
 *******************************************************************************************************/
FS_MEDIA_HANDLE FSBlkDev_MediaGet(FS_BLK_DEV_HANDLE blk_dev_handle)
{
  FS_MEDIA_HANDLE media_handle;
  RTOS_ERR        err;

  RTOS_ERR_SET(err, RTOS_ERR_NONE);
  media_handle = FSMedia_NullHandle;

  FS_BLK_DEV_WITH(blk_dev_handle, &err) {
    media_handle.MediaPtr = blk_dev_handle.BlkDevPtr->MediaPtr;
    media_handle.MediaId = blk_dev_handle.BlkDevPtr->MediaId;
  }

  PP_UNUSED_PARAM(err);

  return (media_handle);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           FSBlkDev_ListPush()
 *
 * @brief    Add a block device to the block device list.
 *
 * @param    p_blk_dev   Pointer to a block device.
 *******************************************************************************************************/
static void FSBlkDev_ListPush(FS_BLK_DEV *p_blk_dev)
{
  FSBlkDev_ListLock();
  SList_Push(&FSBlkDev_Data.BlkDevListHeadPtr, &p_blk_dev->OpenListMember);
  FSBlkDev_ListUnlock();
}

/****************************************************************************************************//**
 *                                           FSBlkDev_ListRem()
 *
 * @brief    Remove a block device from the block device list.
 *
 * @param    p_blk_dev   Pointer to a block device.
 *******************************************************************************************************/
static void FSBlkDev_ListRem(FS_BLK_DEV *p_blk_dev)
{
  FSBlkDev_ListLock();
  SList_Rem(&FSBlkDev_Data.BlkDevListHeadPtr, &p_blk_dev->OpenListMember);
  FSBlkDev_ListUnlock();
}

/****************************************************************************************************//**
 *                                           FSBlkDev_ListLock()
 *
 * @brief    Get exclusive access to the block device list.
 *******************************************************************************************************/
static void FSBlkDev_ListLock(void)
{
  RTOS_ERR err = RTOS_ERR_INIT_CODE(RTOS_ERR_NONE);

  KAL_LockAcquire(FSBlkDev_Data.BlkDevListLockHandle, KAL_OPT_NONE, 0u, &err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}

/****************************************************************************************************//**
 *                                           FSBlkDev_ListUnlock()
 *
 * @brief    Release exclusive access to the block device list.
 *******************************************************************************************************/
static void FSBlkDev_ListUnlock(void)
{
  RTOS_ERR err = RTOS_ERR_INIT_CODE(RTOS_ERR_NONE);

  KAL_LockRelease(FSBlkDev_Data.BlkDevListLockHandle, &err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}

/****************************************************************************************************//**
 *                                           FSBlkDev_FromObjHandle()
 *
 * @brief    Convert an object handle to a block device handle.
 *
 * @param    obj_handle  Handle to an object.
 *
 * @return   Handle to block device.
 *******************************************************************************************************/
static FS_BLK_DEV_HANDLE FSBlkDev_FromObjHandle(FS_OBJ_HANDLE obj_handle)
{
  FS_BLK_DEV_HANDLE blk_dev_handle;

  blk_dev_handle.BlkDevPtr = (FS_BLK_DEV *)obj_handle.ObjPtr;
  blk_dev_handle.BlkDevId = obj_handle.ObjId;

  return (blk_dev_handle);
}

/****************************************************************************************************//**
 *                                           FSBlkDev_OnNullRefCnt()
 *
 * @brief    On block device null reference count callback.
 *
 * @param    p_blk_dev   Pointer to a block device instance.
 *******************************************************************************************************/
void FSBlkDev_OnNullRefCnt(FS_OBJ *p_obj)
{
  FS_BLK_DEV *p_blk_dev;
  FS_MEDIA   *p_media;
  RTOS_ERR   err = RTOS_ERR_INIT_CODE(RTOS_ERR_NONE);

  p_blk_dev = (FS_BLK_DEV *)p_obj;

  p_media = p_blk_dev->MediaPtr;

  FSBlkDev_ListRem(p_blk_dev);

  KAL_LockDel(p_blk_dev->LockHandle);

  p_blk_dev->BlkDevApiPtr->Rem(p_blk_dev, &err);

  FSObj_RefRelease(FSMedia_ToObj(p_media), FSMedia_OnNullRefCnt);
}

/****************************************************************************************************//**
 *                                           FSBlkDev_ToObjHandle()
 *
 * @brief    Convert a block device handle to an object handle.
 *
 * @param    blk_dev_handle  Handle to a block device.
 *
 * @return   Handle to an object.
 *******************************************************************************************************/
FS_OBJ_HANDLE FSBlkDev_ToObjHandle(FS_BLK_DEV_HANDLE blk_dev_handle)
{
  FS_OBJ_HANDLE obj_handle;

  obj_handle.ObjPtr = (FS_OBJ *)blk_dev_handle.BlkDevPtr;
  obj_handle.ObjId = blk_dev_handle.BlkDevId;

  return (obj_handle);
}

/****************************************************************************************************//**
 *                                               FSBlkDev_ToObj()
 *
 * @brief    Convert a block device to an object.
 *
 * @param    p_blk_dev   Pointer to a block device.
 *
 * @return   Pointer to an object.
 *******************************************************************************************************/
FS_OBJ *FSBlkDev_ToObj(FS_BLK_DEV *p_blk_dev)
{
  return ((FS_OBJ *)p_blk_dev);
}

/****************************************************************************************************//**
 *                                           FSBlkDev_FirstAcquire()
 *
 * @brief    Get first opened block device.
 *
 * @return   Handle to the first opened block device.
 *           'FSBlkDev_NullHandle' if no block device is opened.
 *******************************************************************************************************/
FS_BLK_DEV_HANDLE FSBlkDev_FirstAcquire(void)
{
  FS_OBJ_HANDLE obj_handle;

  FSBlkDev_ListLock();
  obj_handle = FSObj_FirstRefAcquireLocked(FSBlkDev_Data.BlkDevListHeadPtr);
  FSBlkDev_ListUnlock();

  return (FSBlkDev_FromObjHandle(obj_handle));
}

/****************************************************************************************************//**
 *                                           FSBlkDev_NextAcquire()
 *
 * @brief    Get the next opened block device.
 *
 * @param    blk_dev_handle  Handle to a block device.
 *
 * @return   Handle to the next block device.
 *           'FSBlkDev_NullHandle' if no more block device is opened.
 *******************************************************************************************************/
FS_BLK_DEV_HANDLE FSBlkDev_NextAcquire(FS_BLK_DEV_HANDLE blk_dev_handle)
{
  FS_OBJ_HANDLE obj_handle;

  FSBlkDev_ListLock();
  obj_handle = FSObj_NextRefAcquireLocked(FSBlkDev_ToObjHandle(blk_dev_handle));
  FSBlkDev_ListUnlock();

  FSBlkDev_Release(blk_dev_handle);

  return (FSBlkDev_FromObjHandle(obj_handle));
}

/****************************************************************************************************//**
 *                                           FSBlkDev_Release()
 *
 * @brief    Release a block device.
 *
 * @param    blk_dev_handle  Handle to a block device.
 *******************************************************************************************************/
void FSBlkDev_Release(FS_BLK_DEV_HANDLE blk_dev_handle)
{
  FSObj_RefRelease((FS_OBJ *)blk_dev_handle.BlkDevPtr, FSBlkDev_OnNullRefCnt);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_FS_AVAIL
