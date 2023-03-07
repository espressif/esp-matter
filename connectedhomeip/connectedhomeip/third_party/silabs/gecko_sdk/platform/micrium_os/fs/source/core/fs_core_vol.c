/***************************************************************************//**
 * @file
 * @brief File System - Core Volume Operations
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

//                                                                 ------------------------ FS ------------------------
#include  <fs_core_cfg.h>
#include  <fs/source/core/fs_core_priv.h>
#include  <fs/source/core/fs_core_vol_priv.h>
#include  <fs/source/core/fs_core_cache_priv.h>
#include  <fs/source/core/fs_core_partition_priv.h>
#include  <fs/source/core/fs_core_working_dir_priv.h>
#include  <fs/source/storage/fs_blk_dev_priv.h>
#include  <fs/source/sys/fs_sys_priv.h>
#include  <fs/source/shared/cleanup/cleanup_mgmt_priv.h>
#include  <fs/include/fs_core_partition.h>

//                                                                 ----------------------- EXT ------------------------
#include  <common/include/rtos_err.h>
#include  <common/include/lib_utils.h>
#include  <common/include/lib_def.h>
#include  <common/source/collections/slist_priv.h>
#include  <common/source/logging/logging_priv.h>

#include  <cpu/include/cpu.h>

#include  <em_core.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH         (FS, CORE, VOL)
#define  RTOS_MODULE_CUR      RTOS_CFG_MODULE_FS

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

SLIST_MEMBER *FSVol_OpenListHeadPtr;

const FS_VOL_HANDLE FSVol_NullHandle = { 0 };

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static FS_VOL_HANDLE FSVol_ObjFindLocked(FS_BLK_DEV_HANDLE blk_dev_handle,
                                         FS_PARTITION_NBR  partition_nbr);

static FS_VOL *FSVol_Alloc(FS_SYS_API *p_sys_api,
                           RTOS_ERR   *p_err);

static void FSVol_Free(FS_VOL *p_vol);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               FSVol_Open()
 *
 * @brief    Open a volume & mount on the file system.
 *
 * @param    blk_dev_handle  Block device handle.
 *
 * @param    partition_nbr   Number of the partition to be opened.
 *
 * @param    vol_name        Volume name to be assigned to the opened volume (see Note #1).
 *
 * @param    open_opt        Open options. Any OR'd combination among:
 *                               - FS_VOL_OPT_DFLT                    Write operations allowed,
 *                                                                    auto sync disabled,
 *                                                                    default sys-specific options.
 *                               - FS_VOL_OPT_ACCESS_MODE_RD_ONLY     Write operations disallowed.
 *                               - FS_VOL_OPT_AUTO_SYNC               Auto sync enabled.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_INIT
 *                               - RTOS_ERR_NOT_FOUND
 *                               - RTOS_ERR_VOL_OPENED
 *                               - RTOS_ERR_NAME_INVALID
 *                               - RTOS_ERR_VOL_FMT_INVALID
 *                               - RTOS_ERR_PARTITION_INVALID
 *                               - RTOS_ERR_BLK_DEV_CORRUPTED
 *                               - RTOS_ERR_BLK_DEV_CLOSED
 *                               - RTOS_ERR_IO
 *
 * @return   Handle to the opened volume.
 *
 * @note     (1) Volume name MUST be unique across all opened volumes.
 *******************************************************************************************************/
FS_VOL_HANDLE FSVol_Open(FS_BLK_DEV_HANDLE blk_dev_handle,
                         FS_PARTITION_NBR  partition_nbr,
                         const CPU_CHAR    *vol_name,
                         FS_FLAGS          open_opt,
                         RTOS_ERR          *p_err)
{
  FS_VOL            *p_vol = DEF_NULL;
  FS_SYS_API        *p_sys_api;
  FS_PARTITION_INFO partition_info;
  FS_VOL_HANDLE     vol_handle;
  CPU_SIZE_T        vol_name_len;
  FS_LB_QTY         dev_lb_cnt;
  CPU_BOOLEAN       partition_found;
  RTOS_ERR          err_tmp;
  CORE_DECLARE_IRQ_STATE;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, FSVol_NullHandle);
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    //                                                             Check that Core layer is initialized.
    CORE_ENTER_ATOMIC();
    if (!FSCore_Data.IsInit) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_INIT);
      CORE_EXIT_ATOMIC();
      break;
    }
    CORE_EXIT_ATOMIC();

    RTOS_ASSERT_DBG(partition_nbr != 0u, RTOS_ERR_INVALID_ARG, FSVol_NullHandle);

    vol_handle = FSVol_NullHandle;

    p_sys_api = FSSys_API_Tbl[FS_SYS_TYPE_FAT];                 // &&&& To be replaced by sys detection in the future.

    BREAK_ON_ERR(dev_lb_cnt = FSBlkDev_LbCntGet(blk_dev_handle, p_err));

    BREAK_ON_ERR(FS_OP_LOCK_WITH) (&FSCore_Data.OpLock, FS_GLOBAL_OP_OPEN_VOL_LIST_WR) {
      vol_handle = FSVol_ObjFindLocked(blk_dev_handle, partition_nbr);
      ASSERT_BREAK(FS_VOL_HANDLE_IS_NULL(vol_handle), RTOS_ERR_VOL_OPENED);

      vol_handle = FSVol_GetLocked((CPU_CHAR *)vol_name);
      ASSERT_BREAK(FS_VOL_HANDLE_IS_NULL(vol_handle), RTOS_ERR_VOL_OPENED);

      BREAK_ON_ERR(p_vol = FSVol_Alloc(p_sys_api, p_err));

      p_vol->Id = FSObj_CurId++;
      p_vol->RefCnt = 1u;
      p_vol->IOCnt = 1u;
      p_vol->IsClosing = DEF_NO;
      p_vol->PartitionNbr = partition_nbr;
      p_vol->BlkDevHandle = blk_dev_handle;

      SList_Push(&FSVol_OpenListHeadPtr, &p_vol->OpenListMember);
    }

    ON_BREAK {
      FS_OP_LOCK_WITH(&FSCore_Data.OpLock, FS_GLOBAL_OP_OPEN_VOL_LIST_WR) {
        SList_Rem(&FSVol_OpenListHeadPtr, &p_vol->OpenListMember);
        FSVol_Free(p_vol);
      }
      break;
    } WITH {
      //                                                           --------------------- VOL INIT ---------------------
      p_vol->CacheBlkDevDataPtr = FSCache_BlkDevDataGet(blk_dev_handle);
      ASSERT_BREAK_LOG((p_vol->CacheBlkDevDataPtr != DEF_NULL), RTOS_ERR_NULL_PTR,
                       ("Blk dev must be assigned to a cache instance"));

      p_vol->SysPtr = p_sys_api;
      p_vol->AutoSync = DEF_BIT_IS_SET(open_opt, FS_VOL_OPT_AUTO_SYNC);
      p_vol->AccessMode = DEF_BIT_IS_SET(open_opt, FS_VOL_OPT_ACCESS_MODE_RD_ONLY)
                          ? FS_VOL_STATUS_ACCESS_MODE_RD_ONLY : FS_VOL_STATUS_ACCESS_MODE_NONE;

      vol_name_len = Str_Len(vol_name);
      RTOS_ASSERT_DBG_ERR_SET(vol_name_len <= FS_CORE_CFG_MAX_VOL_NAME_LEN,
                              *p_err, RTOS_ERR_INVALID_ARG, FSVol_NullHandle);

      Str_Copy_N(p_vol->Name, vol_name, FS_CORE_CFG_MAX_VOL_NAME_LEN);

#if (FS_CORE_CFG_CTR_STAT_EN == DEF_ENABLED)
      p_vol->StatRdSecCtr = 0u;
      p_vol->StatWrSecCtr = 0u;
#endif

      //                                                           ------------------ FIND PARTITION ------------------
      //                                                           Get partition params.
      BREAK_ON_ERR(partition_found = FSPartition_Find(blk_dev_handle,
                                                      partition_nbr,
                                                      &partition_info,
                                                      p_err));
      if (partition_found) {
        p_vol->PartitionStart = partition_info.StartSec;
        p_vol->PartitionSize = partition_info.SecCnt;
      } else if (partition_nbr == 1u) {
        p_vol->PartitionStart = 0u;
        p_vol->PartitionSize = dev_lb_cnt;
      } else {
        BREAK_ERR_SET(RTOS_ERR_NOT_FOUND);
      }

      //                                                           ------------------- SYS DRV OPEN -------------------
      RTOS_ERR_SET(err_tmp, RTOS_ERR_NONE);
      p_vol->SysPtr->VolOpen(p_vol, open_opt, &err_tmp);
      if (partition_found && (partition_nbr == 1u)
          && (RTOS_ERR_CODE_GET(err_tmp) == RTOS_ERR_VOL_FMT_INVALID)) {
        p_vol->PartitionStart = 0u;
        p_vol->PartitionSize = dev_lb_cnt;
        BREAK_ON_ERR(p_vol->SysPtr->VolOpen(p_vol, open_opt, p_err));
      } else if (RTOS_ERR_CODE_GET(err_tmp) != RTOS_ERR_NONE) {
        *p_err = err_tmp;
        break;
      } else {
        //                                                         No err so nothing to do.
      }

      //                                                           ------------------- HANDLE INIT --------------------
      vol_handle.VolPtr = p_vol;
      vol_handle.VolId = p_vol->Id;
    }
  } WITH_SCOPE_END

  return (vol_handle);
}

/****************************************************************************************************//**
 *                                               FSVol_Close()
 *
 * @brief    Close a volume.
 *
 * @param    vol_handle  Handle to a volume.
 *
 * @param    p_err       Pointer to variable that will receive the return error code(s) from this
 *                       function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_VOL_CLOSED
 *
 * @note     (1) The volume is closed even when entries are opened.
 *
 * @note     (2) During the volume close operation, an IO access could happen to the media. In the
 *               case of a removable media, if the close operation is performed after the media
 *               removal, the IO access will return an IO error. This error condition is expected
 *               in that case. Hence the IO error is considered as NO error.
 *               In the case of a fixed media or if the device close operation is done before a
 *               removable media is disconnected, an IO error means that something important did not
 *               work. This condition will be detected by the application when re-opening the volume.
 *******************************************************************************************************/
void FSVol_Close(FS_VOL_HANDLE vol_handle,
                 RTOS_ERR      *p_err)
{
  FS_VOL *p_vol = DEF_NULL;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    FS_VOL_WITH_NO_IO(vol_handle, p_err) {
      p_vol = vol_handle.VolPtr;
      ASSERT_BREAK(!FSObj_IsClosingTestSet(FSVol_ToObj(p_vol)),
                   RTOS_ERR_VOL_CLOSED);
    }

    FSObj_IOEndWait(FSVol_ToObj(p_vol));

    p_vol->SysPtr->VolClose(p_vol, p_err);

    FSObj_RefRelease(FSVol_ToObj(p_vol), FSVol_OnNullRefCnt);
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                               FSVol_CloseAssociated()
 *
 * @brief    Close a volume associated to the supplied block device.
 *
 * @param    blk_dev_handle  Handle to the block device.
 *
 * @param    partition_nbr   Partition number to look for.
 *
 * @param    p_err       Pointer to variable that will receive the return error code(s) from this
 *                       function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_VOL_CLOSED
 *******************************************************************************************************/
void FSVol_CloseAssociated(FS_BLK_DEV_HANDLE blk_dev_handle,
                           FS_PARTITION_NBR  partition_nbr,
                           RTOS_ERR          *p_err)
{
  FS_VOL_HANDLE vol_handle;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  vol_handle = FSVol_ObjFindLocked(blk_dev_handle, partition_nbr);
  if (FS_VOL_HANDLE_IS_NULL(vol_handle) == 0u) {
    FSVol_Close(vol_handle, p_err);
  } else {
    RTOS_ERR_SET(*p_err, RTOS_ERR_VOL_CLOSED);
  }
}

/****************************************************************************************************//**
 *                                               FSVol_Sync()
 *
 * @brief    Ensure that all pending write operations reach the underlying physical media.
 *
 * @param    vol_handle  Handle to a volume.
 *
 * @param    p_err       Pointer to variable that will receive the return error code(s) from this
 *                       function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_VOL_CLOSED
 *                           - RTOS_ERR_BLK_DEV_CLOSED
 *                           - RTOS_ERR_BLK_DEV_CORRUPTED
 *                           - RTOS_ERR_IO
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FSVol_Sync(FS_VOL_HANDLE vol_handle,
                RTOS_ERR      *p_err)
{
  FS_VOL *p_vol;

  WITH_SCOPE_BEGIN(p_err) {
    FS_VOL_WITH(vol_handle, p_err) {
      p_vol = vol_handle.VolPtr;

      ASSERT_BREAK(p_vol->AccessMode != FS_VOL_STATUS_ACCESS_MODE_RD_ONLY,
                   RTOS_ERR_VOL_RD_ONLY);

      BREAK_ON_ERR(p_vol->SysPtr->VolSync(p_vol, p_err));

      BREAK_ON_ERR(FSCache_Sync(p_vol->CacheBlkDevDataPtr->CachePtr,
                                p_vol->BlkDevHandle,
                                p_err));

      BREAK_ON_ERR(FSBlkDev_Sync(p_vol->BlkDevHandle, p_err));
    }
  } WITH_SCOPE_END
}
#endif

/****************************************************************************************************//**
 *                                               FSVol_LabelGet()
 *
 * @brief    Get volume label.
 *
 * @param    vol_handle  Handle to a volume.
 *
 * @param    p_label     Pointer to a buffer that will receive volume label.
 *
 * @param    label_size  Size of the string buffer (see Note #1).
 *
 * @param    p_err       Pointer to variable that will receive the return error code(s) from this
 *                       function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_WOULD_OVF
 *                           - RTOS_ERR_VOL_CLOSED
 *                           - RTOS_ERR_VOL_CORRUPTED
 *                           - RTOS_ERR_BLK_DEV_CLOSED
 *                           - RTOS_ERR_BLK_DEV_CORRUPTED
 *                           - RTOS_ERR_IO
 *
 * @note     (1) 'label_size' is the maximum length string that can be stored in the buffer; it
 *               does NOT include the final NULL character. The buffer MUST be of at least
 *               'label_size' + 1 characters.
 *******************************************************************************************************/
void FSVol_LabelGet(FS_VOL_HANDLE vol_handle,
                    CPU_CHAR      *p_label,
                    CPU_SIZE_T    label_size,
                    RTOS_ERR      *p_err)
{
  FS_VOL *p_vol;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    RTOS_ASSERT_DBG_ERR_SET(p_label != DEF_NULL, *p_err, RTOS_ERR_NULL_PTR,; );
    RTOS_ASSERT_DBG_ERR_SET(label_size != 0u, *p_err, RTOS_ERR_INVALID_ARG,; );

    BREAK_ON_ERR(FS_VOL_WITH) (vol_handle, p_err) {
      p_vol = vol_handle.VolPtr;
      BREAK_ON_ERR(p_vol->SysPtr->VolLabelGet(p_vol,
                                              p_label,
                                              label_size,
                                              p_err));
    }
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                               FSVol_LabelSet()
 *
 * @brief    Set volume label.
 *
 * @param    vol_handle  Handle to a volume.
 *
 * @param    p_label     Pointer to volume label.
 *
 * @param    p_err       Pointer to variable that will receive the return error code(s) from this
 *                       function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_NAME_INVALID
 *                           - RTOS_ERR_WOULD_OVF
 *                           - RTOS_ERR_VOL_CLOSED
 *                           - RTOS_ERR_VOL_CORRUPTED
 *                           - RTOS_ERR_BLK_DEV_CLOSED
 *                           - RTOS_ERR_BLK_DEV_CORRUPTED
 *                           - RTOS_ERR_IO
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FSVol_LabelSet(FS_VOL_HANDLE  vol_handle,
                    const CPU_CHAR *p_label,
                    RTOS_ERR       *p_err)
{
  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    ASSERT_BREAK(vol_handle.VolPtr->AccessMode != FS_VOL_STATUS_ACCESS_MODE_RD_ONLY,
                 RTOS_ERR_VOL_RD_ONLY);

    RTOS_ASSERT_DBG_ERR_SET(p_label != DEF_NULL, *p_err, RTOS_ERR_NULL_PTR,; );

    BREAK_ON_ERR(FS_VOL_WITH) (vol_handle, p_err) {
      BREAK_ON_ERR(vol_handle.VolPtr->SysPtr->VolLabelSet(vol_handle.VolPtr,
                                                          (CPU_CHAR *)p_label,
                                                          p_err));
    }
  } WITH_SCOPE_END
}
#endif

/****************************************************************************************************//**
 *                                               FSVol_Query()
 *
 * @brief    Obtain information about a volume.
 *
 * @param    vol_handle  Volume handle.
 *
 * @param    p_vol_info  Pointer to structure that will receive volume information.
 *
 * @param    p_sys_info  Pointer to structure that will receive file system specific information.
 *                       It is optional. It can be set to DEF_NULL.
 *
 * @param    p_err       Pointer to variable that will receive the return error code(s) from this
 *                       function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_VOL_CLOSED
 *                           - RTOS_ERR_VOL_CORRUPTED
 *                           - RTOS_ERR_BLK_DEV_CLOSED
 *                           - RTOS_ERR_BLK_DEV_CORRUPTED
 *                           - RTOS_ERR_IO
 *******************************************************************************************************/
void FSVol_Query(FS_VOL_HANDLE vol_handle,
                 FS_VOL_INFO   *p_vol_info,
                 void          *p_sys_info,
                 RTOS_ERR      *p_err)
{
  FS_VOL      *p_vol;
  FS_SYS_INFO sys_info;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    RTOS_ASSERT_DBG_ERR_SET(p_vol_info != DEF_NULL, *p_err, RTOS_ERR_NULL_PTR,; );

    BREAK_ON_ERR(FS_VOL_WITH) (vol_handle, p_err) {
      p_vol = vol_handle.VolPtr;

      sys_info.DataInfoPtr = p_sys_info;

      BREAK_ON_ERR(p_vol->SysPtr->VolQuery(p_vol,
                                           &sys_info,
                                           p_err));

      p_vol_info->BadSecCnt = sys_info.BadSecCnt;
      p_vol_info->FreeSecCnt = sys_info.FreeSecCnt;
      p_vol_info->UsedSecCnt = sys_info.UsedSecCnt;
      p_vol_info->TotSecCnt = sys_info.TotSecCnt;
      p_vol_info->SysTypeStrPtr = sys_info.SysTypeStrPtr;
    }
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                               FSVol_BlkDevGet()
 *
 * @brief    Get parent block device handle.
 *
 * @param    vol_handle  Handle to a volume.
 *
 * @return   Handle to the parent block device
 *           NULL handle if the volume is closed.
 *******************************************************************************************************/
FS_BLK_DEV_HANDLE FSVol_BlkDevGet(FS_VOL_HANDLE vol_handle)
{
  FS_BLK_DEV_HANDLE blk_dev_handle;
  RTOS_ERR          err;

  RTOS_ERR_SET(err, RTOS_ERR_NONE);

  blk_dev_handle = FSBlkDev_NullHandle;

  FS_VOL_WITH(vol_handle, &err) {
    blk_dev_handle = vol_handle.VolPtr->BlkDevHandle;
  }

  PP_UNUSED_PARAM(err);

  return (blk_dev_handle);
}

/****************************************************************************************************//**
 *                                           FSVol_PartitionNbrGet()
 *
 * @brief    Get the partition number where the volume resides.
 *
 * @param    vol_handle  Handle to a volume.
 *
 * @param    p_err       Pointer to variable that will receive the return error code(s) from this
 *                       function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_VOL_CLOSED
 *
 * @return   Partition number.
 *******************************************************************************************************/
FS_PARTITION_NBR FSVol_PartitionNbrGet(FS_VOL_HANDLE vol_handle,
                                       RTOS_ERR      *p_err)
{
  FS_PARTITION_NBR partition_nbr;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  partition_nbr = 0u;
  FS_VOL_WITH(vol_handle, p_err) {
    partition_nbr = vol_handle.VolPtr->PartitionNbr;
  }

  return (partition_nbr);
}

/****************************************************************************************************//**
 *                                               FSVol_NameGet()
 *
 * @brief    Get a volume's name.
 *
 * @param    vol_handle  Handle to a volume.
 *
 * @param    p_buf       Pointer to a buffer that will receive the volume name.
 *
 * @param    buf_size    Size of the provided buffer.
 *
 * @param    p_err       Pointer to variable that will receive the return error code(s) from this
 *                       function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_VOL_CLOSED
 *                           - RTOS_ERR_WOULD_OVF
 *******************************************************************************************************/
void FSVol_NameGet(FS_VOL_HANDLE vol_handle,
                   CPU_CHAR      *p_buf,
                   CPU_SIZE_T    buf_size,
                   RTOS_ERR      *p_err)
{
  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    RTOS_ASSERT_DBG_ERR_SET(p_buf != DEF_NULL, *p_err, RTOS_ERR_NULL_PTR,; );
    RTOS_ASSERT_DBG_ERR_SET(buf_size > 0u, *p_err, RTOS_ERR_INVALID_ARG,; );

    FS_VOL_WITH(vol_handle, p_err) {
      p_buf[buf_size - 1u] = '\0';
      Str_Copy_N(p_buf, vol_handle.VolPtr->Name, buf_size);
      ASSERT_BREAK(p_buf[buf_size - 1u] == '\0', RTOS_ERR_WOULD_OVF);
    }
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                               FSVol_Get()
 *
 * @brief    Get a volume by name.
 *
 * @param    p_vol_name  Pointer to name of the volume.
 *
 * @return   Handle to the found volume
 *           NULL handle if no volume is found.
 *******************************************************************************************************/
FS_VOL_HANDLE FSVol_Get(const CPU_CHAR *p_vol_name)
{
  FS_VOL_HANDLE vol_handle = FSVol_NullHandle;

  FS_OP_LOCK_WITH(&FSCore_Data.OpLock, FS_GLOBAL_OP_OPEN_VOL_LIST_RD) {
    vol_handle = FSVol_GetLocked((CPU_CHAR *)p_vol_name);
  }

  return (vol_handle);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               FSVol_GetLocked()
 *
 * @brief    Get the volume handle associated to a given volume name (locked).
 *
 * @param    p_path  Pointer to volume name terminated by a null character or a path separator.
 *
 * @param    p_err   Error pointer.
 *
 * @return   Handle to the volume matching the given name.
 *
 * @note     (1) The caller is responsible for synchronizing accesses to the opened volume list.
 *
 * @note     (2) This check allows to discard false positive of volume names comparison. For instance,
 *               if the two volumes to compare are 'vol1' and 'vol10', it will avoid detecting that
 *               'vol1' matches 'vol10' because the first four characters are the same whereas the
 *               two volume names are in fact unique.
 *******************************************************************************************************/
FS_VOL_HANDLE FSVol_GetLocked(CPU_CHAR *p_path)
{
  FS_VOL        *p_cur_vol;
  FS_VOL        *p_found_vol;
  CPU_SIZE_T    vol_name_len;
  CPU_SIZE_T    cur_vol_name_len;
  FS_VOL_HANDLE vol_handle;
  CPU_INT08S    cmp;

  vol_name_len = FSCore_PathSegLenGet(p_path);
  p_found_vol = DEF_NULL;
  SLIST_FOR_EACH_ENTRY(FSVol_OpenListHeadPtr, p_cur_vol, FS_VOL, OpenListMember) {
    cmp = Str_CmpIgnoreCase_N(p_path, p_cur_vol->Name, vol_name_len);
    if (cmp == 0) {
      cur_vol_name_len = Str_Len(p_cur_vol->Name);
      if (vol_name_len == cur_vol_name_len) {                   // See Note #2.
        p_found_vol = p_cur_vol;
      }
    }
  }

  if (p_found_vol != DEF_NULL) {
    vol_handle.VolPtr = p_found_vol;
    vol_handle.VolId = p_found_vol->Id;
  } else {
    vol_handle = FSVol_NullHandle;
  }

  return (vol_handle);
}

/****************************************************************************************************//**
 *                                               FSVol_Fmt()
 *
 * @brief    Format a volume.
 *
 * @param    blk_dev_handle  Block device handle.
 *
 * @param    partition_nbr   Number of the partition to be formatted.
 *
 * @param    sys_type        File system type.
 *
 * @param    p_fs_cfg        Pointer to file system-specific configuration structure.
 *
 * @param    p_err           Error pointer.
 *
 * @note     (1) Returns an error if files or directories are open on the volume.  All files &
 *               directories MUST be closed prior to formatting the volume.
 *
 * @note     (2) For any file system driver, if 'p_fs_cfg' is a pointer to NULL, then the default
 *               configuration will be selected.  If non-NULL, the argument should be passed a pointer
 *               to the appropriate configuration structure.
 *
 * @note     (3) Formatting invalidates cache contents.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FSVol_Fmt(FS_BLK_DEV_HANDLE blk_dev_handle,
               FS_PARTITION_NBR  partition_nbr,
               FS_SYS_TYPE       sys_type,
               void              *p_fs_cfg,
               RTOS_ERR          *p_err)
{
  FS_VOL_HANDLE     vol_handle;
  FS_PARTITION_INFO partition_entry;
  FS_LB_QTY         partition_sec_cnt;
  FS_LB_NBR         partition_start;
  FS_LB_QTY         dev_sec_cnt;
  CPU_BOOLEAN       partition_found;

  WITH_SCOPE_BEGIN(p_err) {
    //                                                             --------------------- ARG CHK ----------------------
    RTOS_ASSERT_DBG_ERR_SET(sys_type == FS_SYS_TYPE_FAT, *p_err, RTOS_ERR_INVALID_ARG,; );
    RTOS_ASSERT_DBG_ERR_SET(partition_nbr > 0u, *p_err, RTOS_ERR_INVALID_ARG,; );

    BREAK_ON_ERR(dev_sec_cnt = FSBlkDev_LbCntGet(blk_dev_handle, p_err));

    //                                                             ------------- CHK IF VOL ALREADY OPEN --------------
    BREAK_ON_ERR(FS_OP_LOCK_WITH) (&FSCore_Data.OpLock, FS_GLOBAL_OP_OPEN_VOL_LIST_RD) {
      vol_handle = FSVol_ObjFindLocked(blk_dev_handle, partition_nbr);
      ASSERT_BREAK(FS_VOL_HANDLE_IS_NULL(vol_handle), RTOS_ERR_VOL_OPENED);
    }

    //                                                             ------------------ FIND PARTITION ------------------
    //                                                             Get partition params.
    if (partition_nbr != FS_PARTITION_NBR_VOID) {
      BREAK_ON_ERR(partition_found = FSPartition_Find(blk_dev_handle,
                                                      partition_nbr,
                                                      &partition_entry,
                                                      p_err));

      ASSERT_BREAK(partition_found, RTOS_ERR_NOT_FOUND);

      partition_start = partition_entry.StartSec;
      partition_sec_cnt = partition_entry.SecCnt;
    } else {
      partition_start = 0u;
      partition_sec_cnt = dev_sec_cnt;
    }

    //                                                             --------------------- FMT VOL ----------------------
    BREAK_ON_ERR(FSSys_API_Tbl[sys_type]->VolFmt(blk_dev_handle,
                                                 partition_nbr,
                                                 partition_start,
                                                 partition_sec_cnt,
                                                 p_fs_cfg,
                                                 p_err));
  } WITH_SCOPE_END
}
#endif

/****************************************************************************************************//**
 *                                               FSVol_Trim()
 *
 * @brief    Trim a volume
 *
 * @param    p_vol   Pointer to volume.
 *
 * @param    start   Start logical block number.
 *
 * @param    cnt     Number of logical blocks.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FSVol_Trim(FS_VOL    *p_vol,
                FS_LB_NBR start,
                FS_LB_QTY cnt,
                RTOS_ERR  *p_err)
{
  WITH_SCOPE_BEGIN(p_err) {
    //                                                             ------------------ VALIDATE ARGS -------------------
    RTOS_ASSERT_DBG_ERR_SET(start + cnt - 1u <= p_vol->PartitionSize, *p_err, RTOS_ERR_INVALID_ARG,; );

#if 0                                                           // --------------- RELEASED CACHED DATA ---------------
    if (p_vol->CacheAPI_Ptr != DEF_NULL) {
      p_vol->CacheAPI_Ptr->Release(p_vol,
                                   start,
                                   cnt,
                                   p_err);
    }
#endif

    //                                                             ----------------- RELEASE DEV SECS -----------------
    start += p_vol->PartitionStart;
    BREAK_ON_ERR(FSBlkDev_Trim(p_vol->BlkDevHandle,
                               start,
                               cnt,
                               p_err));
  } WITH_SCOPE_END
}
#endif

/****************************************************************************************************//**
 *                                           FSVol_ModuleInit()
 *
 * @brief    Initialize the volume module.
 *******************************************************************************************************/
void FSVol_ModuleInit(void)
{
  SList_Init(&FSVol_OpenListHeadPtr);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           FSVol_ObjFindLocked()
 *
 * @brief    Find an opened volume corresponding to the given partition number on the given block
 *           device.
 *
 * @param    block_dev_handle    Handle to a block device.
 *
 * @param    partition_nbr       Partition number to look for.
 *
 * @return   Handle to the found volume.
 *           Null handle if no corresponding opened volume is found.
 *******************************************************************************************************/
static FS_VOL_HANDLE FSVol_ObjFindLocked(FS_BLK_DEV_HANDLE blk_dev_handle,
                                         FS_PARTITION_NBR  partition_nbr)
{
  FS_VOL        *p_cur_vol;
  FS_VOL_HANDLE found_vol_handle = FSVol_NullHandle;

  SLIST_FOR_EACH_ENTRY(FSVol_OpenListHeadPtr, p_cur_vol, FS_VOL, OpenListMember) {
    if (FS_BLK_DEV_HANDLE_ARE_EQUAL(p_cur_vol->BlkDevHandle, blk_dev_handle)
        && (p_cur_vol->PartitionNbr == partition_nbr)) {
      found_vol_handle.VolPtr = p_cur_vol;
      found_vol_handle.VolId = p_cur_vol->Id;
      break;
    }
  }

  return (found_vol_handle);
}

/****************************************************************************************************//**
 *                                               FSVol_Alloc()
 *
 * @brief    Allocate a file system specific volume descriptor structure and op lock.
 *
 * @param    p_sys_api   Pointer to a file system API structure.
 *
 * @param    p_err       Error pointer.
 *
 * @return   Pointer to the allocated volume descriptor structure.
 *******************************************************************************************************/
static FS_VOL *FSVol_Alloc(FS_SYS_API *p_sys_api,
                           RTOS_ERR   *p_err)
{
  FS_VOL *p_vol;

  WITH_SCOPE_BEGIN(p_err) {
    BREAK_ON_ERR(p_vol = p_sys_api->VolAlloc(p_err));

#if (FS_CORE_CFG_THREAD_SAFETY_EN == DEF_ENABLED)
    BREAK_ON_ERR(OpLockCreate(&p_vol->OpLock, p_err));
#endif
  } WITH_SCOPE_END

  return (p_vol);
}

/****************************************************************************************************//**
 *                                               FSVol_Free()
 *
 * @brief    Free a file system specific volume descriptor structure.
 *
 * @param    p_vol   Pointer to a volume.
 *******************************************************************************************************/
static void FSVol_Free(FS_VOL *p_vol)
{
#if (FS_CORE_CFG_THREAD_SAFETY_EN == DEF_ENABLED)
  OpLockDel(&p_vol->OpLock);
#endif

  p_vol->SysPtr->VolFree(p_vol);
}

/****************************************************************************************************//**
 *                                           FSVol_OnNullRefCnt()
 *
 * @brief    On volume descriptor null reference count callback.
 *
 * @param    p_obj   Pointer to a volume descriptor.
 *
 * @note     (1) The volume will be freed regardless of possible errors occurring while:
 *               - (a) closing the volume at the driver level;
 *               - (b) flushing the cache;
 *               - (c) performing any operation related to detaching from a parent object.
 *               The volume closing sequence cannot exit early in case of error. All the operations
 *               must be tried. If one or several operations fail, only the first error condition
 *               will be returned by FSVol_OnNullRefCnt().
 *******************************************************************************************************/
void FSVol_OnNullRefCnt(FS_OBJ *p_obj)
{
  FS_VOL *p_vol;

  p_vol = (FS_VOL *)p_obj;

  //                                                               Remove from open list.
  FS_OP_LOCK_WITH(&FSCore_Data.OpLock, FS_GLOBAL_OP_OPEN_VOL_LIST_WR) {
    SList_Rem(&FSVol_OpenListHeadPtr, &p_vol->OpenListMember);
  }

  FSVol_Free((FS_VOL *)p_obj);
}

/****************************************************************************************************//**
 *                                           FSVol_ToObjHandle()
 *
 * @brief    Convert a volume handle to an object handle.
 *
 * @param    vol_handle  Handle to a volume.
 *
 * @return   Handle to an object.
 *******************************************************************************************************/
FS_OBJ_HANDLE FSVol_ToObjHandle(FS_VOL_HANDLE vol_handle)
{
  FS_OBJ_HANDLE obj_handle;

  obj_handle.ObjPtr = (FS_OBJ *)vol_handle.VolPtr;
  obj_handle.ObjId = vol_handle.VolId;

  return (obj_handle);
}

/****************************************************************************************************//**
 *                                               FSVol_ToObj()
 *
 * @brief    Convert a volume to an object.
 *
 * @param    p_vol   Pointer to a volume.
 *
 * @return   Pointer to an object.
 *******************************************************************************************************/
FS_OBJ *FSVol_ToObj(FS_VOL *p_vol)
{
  return ((FS_OBJ *)p_vol);
}

/****************************************************************************************************//**
 *                                           FSVol_FirstAcquire()
 *
 * @brief    Get first opened volume.
 *
 * @return   Handle to the first opened volume
 *           'FS_VOL_NULL' if no volume is opened.
 *******************************************************************************************************/
FS_VOL_HANDLE FSVol_FirstAcquire(void)
{
  FS_OBJ_HANDLE obj_handle;
  FS_VOL_HANDLE vol_handle;

  FS_OP_LOCK_WITH(&FSCore_Data.OpLock, FS_GLOBAL_OP_OPEN_VOL_LIST_RD) {
    obj_handle = FSObj_FirstRefAcquireLocked(FSVol_OpenListHeadPtr);
  }

  vol_handle.VolPtr = (FS_VOL *)obj_handle.ObjPtr;
  vol_handle.VolId = obj_handle.ObjId;

  return (vol_handle);
}

/****************************************************************************************************//**
 *                                           FSVol_NextAcquire()
 *
 * @brief    Get the next opened volume.
 *
 * @param    vol_handle  Current volume handle.
 *
 * @return   Handle to the next volume
 *           'FS_VOL_NULL' if no more volume is opened.
 *******************************************************************************************************/
FS_VOL_HANDLE FSVol_NextAcquire(FS_VOL_HANDLE vol_handle)
{
  FS_OBJ_HANDLE cur_obj_handle;
  FS_OBJ_HANDLE next_obj_handle;
  FS_VOL_HANDLE next_vol_handle;

  cur_obj_handle.ObjPtr = (FS_OBJ *)vol_handle.VolPtr;
  cur_obj_handle.ObjId = vol_handle.VolId;

  FS_OP_LOCK_WITH(&FSCore_Data.OpLock, FS_GLOBAL_OP_OPEN_VOL_LIST_RD) {
    next_obj_handle = FSObj_NextRefAcquireLocked(cur_obj_handle);
  }

  FSVol_Release(vol_handle);

  next_vol_handle.VolPtr = (FS_VOL *)next_obj_handle.ObjPtr;
  next_vol_handle.VolId = next_obj_handle.ObjId;

  return (next_vol_handle);
}

/****************************************************************************************************//**
 *                                               FSVol_Release()
 *
 * @brief    Release a volume.
 *
 * @param    vol_handle  Handle to a volume.
 *******************************************************************************************************/
void FSVol_Release(FS_VOL_HANDLE vol_handle)
{
  FSObj_RefRelease(FSVol_ToObj(vol_handle.VolPtr), FSVol_OnNullRefCnt);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_FS_AVAIL
