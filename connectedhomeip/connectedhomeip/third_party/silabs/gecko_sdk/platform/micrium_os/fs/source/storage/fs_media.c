/***************************************************************************//**
 * @file
 * @brief File System - Media Operations
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
#include  <fs_storage_cfg.h>
#include  <fs/source/storage/fs_media_priv.h>
#include  <fs/source/shared/fs_utils_priv.h>
#include  <fs/source/shared/cleanup/cleanup_mgmt_priv.h>
#if (FS_STORAGE_CFG_DBG_WR_VERIFY_EN == DEF_ENABLED)
#include  <fs/source/shared/crc/edc_crc.h>
#endif

//                                                                 ----------------------- EXT ------------------------
#include  <cpu/include/cpu.h>
#include  <common/include/lib_utils.h>
#include  <common/include/rtos_err.h>
#include  <common/include/rtos_prio.h>
#include  <common/source/platform_mgr/platform_mgr_priv.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/kal/kal_priv.h>

#ifdef  RTOS_MODULE_FS_STORAGE_NAND_AVAIL
#include  <fs/source/storage/nand/fs_nand_ftl_priv.h>
#endif

#ifdef  RTOS_MODULE_FS_STORAGE_NOR_AVAIL
#include  <fs/source/storage/nor/fs_nor_ftl_priv.h>
#endif

#include  <em_core.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define    RTOS_MODULE_CUR    RTOS_CFG_MODULE_FS
#define    LOG_DFLT_CH        (FS, MEDIA)

/********************************************************************************************************
 ********************************************************************************************************
 *                                               VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

FS_MEDIA_DATA FSMedia_Data = {
#if (FS_STORAGE_CFG_MEDIA_POLL_TASK_EN == DEF_ENABLED)
  .MediaPollPeriodMs = 500u,
  .MediaPollTaskHandle = KAL_TASK_HANDLE_NULL,
#endif
  .IsInit = DEF_NO,
  .CurId = 0u,
  .MediaListHeadPtr = DEF_NULL,
  .MediaListLockHandle = KAL_LOCK_HANDLE_NULL
};

/********************************************************************************************************
 *                                       INIT CONFIGURATION STRUCTURES
 *******************************************************************************************************/

const FS_MEDIA_HANDLE FSMedia_NullHandle = { 0 };

const FS_MEDIA_INIT_CFG *FSMedia_InitCfgPtr;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_MEDIA_POLL_TASK_EN == DEF_ENABLED)
static void FSMedia_PollTask(void *p_arg);
#endif

static void FSMedia_ListLock(void);

static void FSMedia_ListUnlock(void);

static void FSMedia_ListPush(FS_MEDIA *p_media);

static void FSMedia_ListRem(FS_MEDIA *p_media);

static FS_MEDIA_HANDLE FSMedia_ObjToMedia(FS_OBJ_HANDLE obj_handle);

static CPU_BOOLEAN FSMedia_PmItemGetPred(const PLATFORM_MGR_ITEM *p_item,
                                         void                    *p_pred_data_v);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               FSMedia_Init()
 *
 * @brief    Initialize the media sub-module.
 *
 * @param    p_media_cfg     Pointer to a persistent configuration structure.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_SEG_OVF
 *
 * @note     (1) Although this function may be called in addition to the block device initialization
 *               function (i.e. FSBlkDev_Init()), it is intended to be called alone to allow flash memory
 *               (NOR and NAND) raw accesses without the overhead associated with initializing the
 *               block device sub-module.
 *******************************************************************************************************/
void FSMedia_Init(const FS_MEDIA_INIT_CFG *p_media_cfg,
                  RTOS_ERR                *p_err)
{
  PLATFORM_MGR_ITEM              *p_pm_item;
  FS_MEDIA_PM_ITEM_GET_PRED_DATA pred_data;
  CPU_SIZE_T                     fixed_media_cnt;
  RTOS_ERR                       err;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    RTOS_ASSERT_DBG(!FSMedia_Data.IsInit, RTOS_ERR_ALREADY_INIT,; );

#if (FS_STORAGE_CFG_MEDIA_POLL_TASK_EN == DEF_ENABLED)
    RTOS_ASSERT_DBG_ERR_SET(KAL_FeatureQuery(KAL_FEATURE_TASK_CREATE, KAL_OPT_NONE),
                            *p_err, RTOS_ERR_NOT_AVAIL,; );
    RTOS_ASSERT_DBG_ERR_SET(KAL_FeatureQuery(KAL_FEATURE_PEND_TIMEOUT, KAL_OPT_NONE),
                            *p_err, RTOS_ERR_NOT_AVAIL,; );
#endif
#if defined(RTOS_MODULE_FS_STORAGE_NAND_AVAIL)    \
    || defined(RTOS_MODULE_FS_STORAGE_NOR_AVAIL)  \
    || defined(RTOS_MODULE_FS_STORAGE_SCSI_AVAIL) \
    || defined(RTOS_MODULE_FS_STORAGE_SD_SPI_AVAIL)
    RTOS_ASSERT_DBG_ERR_SET(KAL_FeatureQuery(KAL_FEATURE_PEND_TIMEOUT, KAL_OPT_NONE),
                            *p_err, RTOS_ERR_NOT_AVAIL,; );
#endif

    FSMedia_InitCfgPtr = p_media_cfg;

    BREAK_ON_ERR(FSMedia_Data.MediaListLockHandle = KAL_LockCreate("FS media list lock",
                                                                   DEF_NULL,
                                                                   p_err));

#if (FS_STORAGE_CFG_MEDIA_POLL_TASK_EN == DEF_ENABLED)
    RTOS_ASSERT_DBG(((FSMedia_InitCfgPtr->OnConn == DEF_NULL)
                     && (FSMedia_InitCfgPtr->OnDisconn == DEF_NULL))
                    || ((FSMedia_InitCfgPtr->OnConn != DEF_NULL)),
                    RTOS_ERR_INVALID_ARG,; );

    if (FSMedia_InitCfgPtr->OnConn != DEF_NULL) {
      BREAK_ON_ERR(FSMedia_Data.MediaPollTaskHandle = KAL_TaskAlloc("FS - Media Poll task",
                                                                    (CPU_STK *)FSMedia_InitCfgPtr->PollTaskStkPtr,
                                                                    FSMedia_InitCfgPtr->PollTaskStkSizeElements,
                                                                    DEF_NULL,
                                                                    p_err));

      BREAK_ON_ERR(KAL_TaskCreate(FSMedia_Data.MediaPollTaskHandle,
                                  FSMedia_PollTask,
                                  DEF_NULL,
                                  FS_MEDIA_POLL_TASK_PRIO_DFLT,
                                  DEF_NULL,
                                  p_err));
    }
#endif

    RTOS_ERR_SET(err, RTOS_ERR_NONE);
    pred_data.TargetIx = 0u;
    pred_data.CurIx = 0u;
    fixed_media_cnt = 0u;
    while (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE) {
      p_pm_item = PlatformMgrItemGet(FSMedia_PmItemGetPred, &pred_data, &err);
      if (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE) {
        (void)FSMedia_Add((FS_MEDIA_PM_ITEM *)p_pm_item, p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          return;
        }
        fixed_media_cnt += 1u;
      }
      pred_data.CurIx = 0u;
      pred_data.TargetIx += 1u;
    }

    FSMedia_Data.FixedMediaCnt = fixed_media_cnt;
    FSMedia_Data.IsInit = DEF_YES;
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                               FSMedia_LowFmt()
 *
 * @brief    Low-level format a media so that it can be opened as a block device.
 *
 * @param    media_handle    Handle to a media.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_INIT
 *                               - RTOS_ERR_NOT_FOUND
 *                               - RTOS_ERR_BLK_DEV_OPENED
 *                               - RTOS_ERR_IO
 *
 * @note     (1) This function will write flash translation layer metadata to NOR and NAND flash
 *               memories. For other types of media, this function does nothing.
 *******************************************************************************************************/
#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FSMedia_LowFmt(FS_MEDIA_HANDLE media_handle,
                    RTOS_ERR        *p_err)
{
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  //                                                               Check that Storage layer is initialized.
  CORE_ENTER_ATOMIC();
  if (!FSMedia_Data.IsInit) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_INIT);
    CORE_EXIT_ATOMIC();
    return;
  }
  CORE_EXIT_ATOMIC();

  FS_MEDIA_WITH_NO_IO(media_handle, p_err) {
    if (media_handle.MediaPtr->PmItemPtr->PmItem.Type == PLATFORM_MGR_ITEM_TYPE_HW_INFO_FS_NAND) {
#ifdef RTOS_MODULE_FS_STORAGE_NAND_AVAIL
      FS_NAND_FTL_LowFmt(media_handle, p_err);
#else
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL,; );
#endif
    } else if (media_handle.MediaPtr->PmItemPtr->PmItem.Type == PLATFORM_MGR_ITEM_TYPE_HW_INFO_FS_NOR) {
#ifdef RTOS_MODULE_FS_STORAGE_NOR_AVAIL
      FS_NOR_FTL_LowFmt(media_handle, p_err);
#else
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL,; );
#endif
    }
  }
}
#endif

/****************************************************************************************************//**
 *                                           FSMedia_MaxCntGet()
 *
 * @brief    Get the maximum number of co-existing media.
 *
 * @return   Maximum number of media.
 *******************************************************************************************************/
CPU_SIZE_T FSMedia_MaxCntGet(void)
{
  if (FSMedia_InitCfgPtr->MaxSCSILuCnt == LIB_MEM_BLK_QTY_UNLIMITED) {
    return (LIB_MEM_BLK_QTY_UNLIMITED);
  }

  return (FSMedia_InitCfgPtr->MaxSCSILuCnt + FSMedia_Data.FixedMediaCnt);
}

/****************************************************************************************************//**
 *                                           FSMedia_AlignReqGet()
 *
 * @brief    Get the read/write buffer alignment requirement for a media.
 *
 * @param    media_handle    Handle to a media.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_IO
 *
 * @return   Alignment requirement.
 *******************************************************************************************************/
CPU_SIZE_T FSMedia_AlignReqGet(FS_MEDIA_HANDLE media_handle,
                               RTOS_ERR        *p_err)
{
  FS_MEDIA   *p_media;
  CPU_SIZE_T align_req = 0u;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, RTOS_ERR_NONE);
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    BREAK_ON_ERR(FS_MEDIA_WITH_NO_IO) (media_handle, p_err) {
      p_media = media_handle.MediaPtr;
      BREAK_ON_ERR(FS_MEDIA_LOCK_WITH) (p_media) {
        align_req = p_media->PmItemPtr->MediaApiPtr->AlignReqGet(p_media, p_err);
      }
    }
  } WITH_SCOPE_END

  return (align_req);
}

/****************************************************************************************************//**
 *                                               FSMedia_IsConn()
 *
 * @brief    Check whether a media is connected.
 *
 * @param    media_handle    Handle to a media.
 *
 * @return   DEF_YES, if the media is connected.
 *           DEF_NO,  otherwise.
 *******************************************************************************************************/
CPU_BOOLEAN FSMedia_IsConn(FS_MEDIA_HANDLE media_handle)
{
  FS_MEDIA    *p_media;
  CPU_BOOLEAN is_conn;

  p_media = media_handle.MediaPtr;
  FS_MEDIA_LOCK_WITH(p_media) {
    is_conn = p_media->PmItemPtr->MediaApiPtr->IsConn(p_media);
  }

  return (is_conn);
}

/****************************************************************************************************//**
 *                                               FSMedia_TypeGet()
 *
 * @brief    Get the type of a media.
 *
 * @param    media_handle    Handle to a media.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_FOUND
 *                               - RTOS_ERR_IO
 *
 * @return   Type of the media. The possible types can be:
 *               - FS_MEDIA_TYPE_RAM_DISK
 *               - FS_MEDIA_TYPE_NOR
 *               - FS_MEDIA_TYPE_NAND
 *               - FS_MEDIA_TYPE_SD_SPI
 *               - FS_MEDIA_TYPE_SD_CARD
 *               - FS_MEDIA_TYPE_SCSI
 *******************************************************************************************************/
FS_MEDIA_TYPE FSMedia_TypeGet(FS_MEDIA_HANDLE media_handle,
                              RTOS_ERR        *p_err)
{
  FS_MEDIA      *p_media;
  FS_MEDIA_TYPE type = FS_MEDIA_TYPE_GENERIC;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, FS_MEDIA_TYPE_GENERIC);
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  FS_MEDIA_WITH_NO_IO(media_handle, p_err) {
    p_media = media_handle.MediaPtr;
    type = (FS_MEDIA_TYPE)p_media->PmItemPtr->PmItem.Type;
  }

  return (type);
}

/****************************************************************************************************//**
 *                                               FSMedia_NameGet()
 *
 * @brief    Get a media name.
 *
 * @param    media_handle    Handle to a media.
 *
 * @param    p_buf           Pointer to a buffer that will receive the media name.
 *
 * @param    buf_size        Size of the given buffer.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_IO
 *******************************************************************************************************/
void FSMedia_NameGet(FS_MEDIA_HANDLE media_handle,
                     CPU_CHAR        *p_buf,
                     CPU_SIZE_T      buf_size,
                     RTOS_ERR        *p_err)
{
  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    RTOS_ASSERT_DBG_ERR_SET(p_buf != DEF_NULL, *p_err, RTOS_ERR_NULL_PTR,; );
    RTOS_ASSERT_DBG_ERR_SET(buf_size > 0u, *p_err, RTOS_ERR_INVALID_ARG,; );

    FS_MEDIA_WITH_NO_IO(media_handle, p_err) {
      p_buf[buf_size - 1u] = '\0';
      Str_Copy_N(p_buf, media_handle.MediaPtr->PmItemPtr->PmItem.StrID, buf_size);
      ASSERT_BREAK(p_buf[buf_size - 1u] == '\0', RTOS_ERR_WOULD_OVF);
    }
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                           FSMedia_TypeStrGet()
 *
 * @brief    Get a string corresponding to the given media type.
 *
 * @param    type    Media type.
 *                       - FS_MEDIA_TYPE_RAM_DISK
 *                       - FS_MEDIA_TYPE_NOR
 *                       - FS_MEDIA_TYPE_NAND
 *                       - FS_MEDIA_TYPE_SD_SPI
 *                       - FS_MEDIA_TYPE_SD_CARD
 *                       - FS_MEDIA_TYPE_SCSI
 *
 * @return   Pointer to a string literal corresponding to the given media type.
 *******************************************************************************************************/
const CPU_CHAR *FSMedia_TypeStrGet(FS_MEDIA_TYPE type)
{
  CPU_CHAR *p_str = DEF_NULL;

  switch (type) {
    case FS_MEDIA_TYPE_NAND:
      p_str = "NAND";
      break;
    case FS_MEDIA_TYPE_NOR:
      p_str = "NOR";
      break;
    case FS_MEDIA_TYPE_SD_CARD:
      p_str = "SD (card)";
      break;
    case FS_MEDIA_TYPE_SD_SPI:
      p_str = "SD (SPI)";
      break;
    case FS_MEDIA_TYPE_RAM_DISK:
      p_str = "RAM disk";
      break;
    case FS_MEDIA_TYPE_VDI:
      p_str = "Virtual disk image";
      break;
    case FS_MEDIA_TYPE_SCSI:
      p_str = "SCSI logical unit";
      break;
    default:
      RTOS_DBG_FAIL_EXEC(RTOS_ERR_INVALID_TYPE, DEF_NULL);
  }

  return (p_str);
}

/****************************************************************************************************//**
 *                                           FSMedia_FirstGet()
 *
 * @brief    Get the first added media.
 *
 * @return   Handle to a media.
 *******************************************************************************************************/
FS_MEDIA_HANDLE FSMedia_FirstGet(void)
{
  FS_MEDIA        *p_media;
  FS_MEDIA_HANDLE media_handle;

  FSMedia_ListLock();
  if (FSMedia_Data.MediaListHeadPtr == DEF_NULL) {
    FSMedia_ListUnlock();
    return (FSMedia_NullHandle);
  }
  p_media = CONTAINER_OF(FSMedia_Data.MediaListHeadPtr, FS_MEDIA, OpenListMember);

  media_handle.MediaPtr = p_media;
  media_handle.MediaId = p_media->Id;
  FSMedia_ListUnlock();

  return (media_handle);
}

/****************************************************************************************************//**
 *                                               FSMedia_Get()
 *
 * @brief    Get a handle to a media by name.
 *
 * @param    p_name  Pointer to a media name.
 *
 * @return   Handle to a media.
 *           NULL handle if no media is found with the given name.
 *
 * @note     (1) You may check for NULL handle using the macro FS_MEDIA_HANDLE_IS_NULL().
 *******************************************************************************************************/
FS_MEDIA_HANDLE FSMedia_Get(const CPU_CHAR *p_name)
{
  FS_MEDIA        *p_cur_media;
  FS_MEDIA_HANDLE media_handle = FSMedia_NullHandle;
  CPU_INT16S      cmp;

  SLIST_FOR_EACH_ENTRY(FSMedia_Data.MediaListHeadPtr, p_cur_media, FS_MEDIA, OpenListMember) {
    cmp = Str_Cmp(p_name, p_cur_media->PmItemPtr->PmItem.StrID);
    if (cmp == 0u) {
      media_handle.MediaPtr = p_cur_media;
      media_handle.MediaId = p_cur_media->Id;
      break;
    }
  }

  return (media_handle);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               FSMedia_Add()
 *
 * @brief    Add a media instance.
 *
 * @param    p_media_pm_item     Pointer to a media platform manager item.
 *
 * @param    p_err               Error pointer.
 *
 * @return   Pointer to the added media.
 *******************************************************************************************************/
FS_MEDIA *FSMedia_Add(const FS_MEDIA_PM_ITEM *p_media_pm_item,
                      RTOS_ERR               *p_err)
{
  FS_MEDIA *p_media;

  WITH_SCOPE_BEGIN(p_err) {
    p_media = p_media_pm_item->MediaApiPtr->Add(p_media_pm_item, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (DEF_NULL);
    }

    p_media->PmItemPtr = p_media_pm_item;
    p_media->IsConn = DEF_NO;
    p_media->IsClosing = DEF_NO;
    p_media->Id = FSMedia_Data.CurId++;
    p_media->RefCnt = 1u;

    BREAK_ON_ERR(p_media->LockHandle = KAL_LockCreate("FS media lock",
                                                      DEF_NULL,
                                                      p_err));
    FSMedia_ListPush(p_media);
  } WITH_SCOPE_END

  return (p_media);
}

/****************************************************************************************************//**
 *                                               FSMedia_Rem()
 *
 * @brief    Remove a media instance.
 *
 * @param    p_media     Pointer to a media.
 *******************************************************************************************************/
void FSMedia_Rem(FS_MEDIA *p_media)
{
  FS_MEDIA_LOCK_WITH(p_media) {
    p_media->IsConn = DEF_NO;
    p_media->Id = FSMedia_Data.CurId++;
  }
  //                                                               Rel media to balance acquire done in FSMedia_Add().
  FSObj_RefRelease(FSMedia_ToObj(p_media), FSMedia_OnNullRefCnt);
}

/********************************************************************************************************
 *                                            FSMedia_Lock()
 *
 * Description : Acquire exclusive access to a media.
 *
 * Argument(s) : p_media     Pointer to a media.
 *
 * Return(s)   : none.
 *
 * Note(s)     : none.
 *
 *******************************************************************************************************/
void FSMedia_Lock(FS_MEDIA *p_media)
{
  RTOS_ERR err;

  KAL_LockAcquire(p_media->LockHandle, KAL_OPT_NONE, 0u, &err);
  RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}

/****************************************************************************************************//**
 *                                               FSMedia_TryLock()
 *
 * @brief    Try to acquire exclusive access to a media.
 *
 * @param    p_media     Pointer to a media.
 *
 * @return   DEF_YES, if the lock has been successfully acquired.
 *           DEF_NO,  otherwise.
 *******************************************************************************************************/
CPU_BOOLEAN FSMedia_TryLock(FS_MEDIA *p_media)
{
  RTOS_ERR    err = RTOS_ERR_INIT_CODE(RTOS_ERR_NONE);
  CPU_BOOLEAN lock_acquired;

  KAL_LockAcquire(p_media->LockHandle, KAL_OPT_PEND_NON_BLOCKING, 0u, &err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE)
                       || (RTOS_ERR_CODE_GET(err) == RTOS_ERR_WOULD_BLOCK), RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_NO);

  lock_acquired = (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE);

  return (lock_acquired);
}

/****************************************************************************************************//**
 *                                               FSMedia_Unlock()
 *
 * @brief    Release exclusive access to a media.
 *
 * @param    p_media     Pointer to a media.
 *******************************************************************************************************/
void FSMedia_Unlock(FS_MEDIA *p_media)
{
  RTOS_ERR err = RTOS_ERR_INIT_CODE(RTOS_ERR_NONE);

  KAL_LockRelease(p_media->LockHandle, &err);
  RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}

/****************************************************************************************************//**
 *                                               FSMedia_Conn()
 *
 * @brief    Connect a media.
 *
 * @param    p_media     Pointer to a media.
 *******************************************************************************************************/
void FSMedia_Conn(FS_MEDIA *p_media)
{
  CPU_BOOLEAN conn;

  conn = DEF_NO;
  FS_MEDIA_LOCK_WITH(p_media) {
    if (!p_media->IsConn) {
      p_media->IsConn = DEF_YES;
      conn = DEF_YES;
    }
  }

  if (conn && (FSMedia_InitCfgPtr->OnConn != DEF_NULL)) {
    FS_MEDIA_HANDLE media_handle;
    media_handle.MediaPtr = p_media;
    media_handle.MediaId = p_media->Id;
    FSMedia_InitCfgPtr->OnConn(media_handle);
  }
}

/****************************************************************************************************//**
 *                                               FSMedia_Disconn()
 *
 * @brief    Disconnect a media.
 *
 * @param    p_media     Pointer to a media.
 *******************************************************************************************************/
void FSMedia_Disconn(FS_MEDIA *p_media)
{
  CPU_BOOLEAN disconn;

  disconn = DEF_NO;
  FS_MEDIA_LOCK_WITH(p_media) {
    if (p_media->IsConn) {
      p_media->IsConn = DEF_NO;
      disconn = DEF_YES;
    }
  }

  if (disconn && (FSMedia_InitCfgPtr->OnDisconn != DEF_NULL)) {
    FS_MEDIA_HANDLE media_handle;
    media_handle.MediaPtr = p_media;
    media_handle.MediaId = p_media->Id;
    FSMedia_InitCfgPtr->OnDisconn(media_handle);
  }
}

/****************************************************************************************************//**
 *                                           FSMedia_ListPush()
 *
 * @brief    Add a media to the media list.
 *
 * @param    p_media     Pointer to a media.
 *******************************************************************************************************/
static void FSMedia_ListPush(FS_MEDIA *p_media)
{
  FSMedia_ListLock();
  SList_PushBack(&FSMedia_Data.MediaListHeadPtr, &p_media->OpenListMember);
  FSMedia_ListUnlock();
}

/****************************************************************************************************//**
 *                                               FSMedia_ListRem()
 *
 * @brief    Remove a media from the media list.
 *
 * @param    p_media     Pointer to a media.
 *******************************************************************************************************/
static void FSMedia_ListRem(FS_MEDIA *p_media)
{
  FSMedia_ListLock();
  SList_Rem(&FSMedia_Data.MediaListHeadPtr, &p_media->OpenListMember);
  FSMedia_ListUnlock();
}

/****************************************************************************************************//**
 *                                           FSMedia_ObjToMedia()
 *
 * @brief    Convert an object handle to a media handle.
 *
 * @param    obj_handle  Handle to an object.
 *
 * @return   Handle to a media.
 *******************************************************************************************************/
static FS_MEDIA_HANDLE FSMedia_ObjToMedia(FS_OBJ_HANDLE obj_handle)
{
  FS_MEDIA_HANDLE media_handle;

  media_handle.MediaPtr = (FS_MEDIA *)obj_handle.ObjPtr;
  media_handle.MediaId = obj_handle.ObjId;

  return (media_handle);
}

/****************************************************************************************************//**
 *                                           FSMedia_ListLock()
 *
 * @brief    Get exclusive access to the media list.
 *******************************************************************************************************/
static void FSMedia_ListLock(void)
{
  RTOS_ERR err = RTOS_ERR_INIT_CODE(RTOS_ERR_NONE);

  KAL_LockAcquire(FSMedia_Data.MediaListLockHandle, KAL_OPT_NONE, 0u, &err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}

/****************************************************************************************************//**
 *                                           FSMedia_ListUnlock()
 *
 * @brief    Release exclusive access to the media list.
 *******************************************************************************************************/
static void FSMedia_ListUnlock(void)
{
  RTOS_ERR err = RTOS_ERR_INIT_CODE(RTOS_ERR_NONE);

  KAL_LockRelease(FSMedia_Data.MediaListLockHandle, &err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}

/****************************************************************************************************//**
 *                                           FSBlkDev_MediaPollTask()
 *
 * @brief    Media polling task.
 *
 * @param    p_arg   Task argument (not used).
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_MEDIA_POLL_TASK_EN == DEF_ENABLED)
static void FSMedia_PollTask(void *p_arg)
{
  FS_MEDIA        *p_media;
  FS_MEDIA_HANDLE media_handle;
  CPU_BOOLEAN     is_conn;
  CPU_BOOLEAN     lock_acquired;

  PP_UNUSED_PARAM(p_arg);

  while (DEF_ON) {
    KAL_Dly(FSMedia_Data.MediaPollPeriodMs);

    //                                                             ----------------- POLL EACH MEDIA ------------------
    FS_MEDIA_FOR_EACH(media_handle) {
      p_media = media_handle.MediaPtr;

      lock_acquired = FSMedia_TryLock(p_media);
      if (!lock_acquired) {
        continue;
      }

      if (p_media->IsConn) {
        is_conn = p_media->PmItemPtr->MediaApiPtr->IsConn(p_media);
        if (!is_conn) {
          p_media->IsConn = DEF_NO;
          FSMedia_Unlock(p_media);

          if (FSMedia_InitCfgPtr->OnDisconn != DEF_NULL) {
            media_handle.MediaPtr = p_media;
            media_handle.MediaId = p_media->Id;
            FSMedia_InitCfgPtr->OnDisconn(media_handle);
          }
          continue;
        }
      } else {
        is_conn = p_media->PmItemPtr->MediaApiPtr->IsConn(p_media);
        if (is_conn) {
          p_media->IsConn = DEF_YES;
          p_media->Id = FSMedia_Data.CurId++;
          FSMedia_Unlock(p_media);

          if (FSMedia_InitCfgPtr->OnConn != DEF_NULL) {
            media_handle.MediaPtr = p_media;
            media_handle.MediaId = p_media->Id;
            FSMedia_InitCfgPtr->OnConn(media_handle);
          }
          continue;
        }
      }

      FSMedia_Unlock(p_media);
    }
  }
}
#endif

/****************************************************************************************************//**
 *                                           FSMedia_OnNullRefCnt()
 *
 * @brief    On media null reference count callback.
 *
 * @param    p_obj   Pointer to a media.
 *******************************************************************************************************/
void FSMedia_OnNullRefCnt(FS_OBJ *p_obj)
{
  FS_MEDIA *p_media = (FS_MEDIA *)p_obj;
  RTOS_ERR err = RTOS_ERR_INIT_CODE(RTOS_ERR_NONE);

  FSMedia_ListRem(p_media);

  KAL_LockDel(p_media->LockHandle);

  p_media->PmItemPtr->MediaApiPtr->Rem(p_media, &err);
}

/****************************************************************************************************//**
 *                                           FSMedia_FirstAcquire()
 *
 * @brief    Acquire the first added media.
 *
 * @return   Handle to a media.
 *******************************************************************************************************/
FS_MEDIA_HANDLE FSMedia_FirstAcquire(void)
{
  FS_OBJ_HANDLE obj_handle;

  FSMedia_ListLock();
  obj_handle = FSObj_FirstRefAcquireLocked(FSMedia_Data.MediaListHeadPtr);
  FSMedia_ListUnlock();

  return (FSMedia_ObjToMedia(obj_handle));
}

/****************************************************************************************************//**
 *                                           FSMedia_NextAcquire()
 *
 * @brief    Acquire the next media.
 *
 * @param    media_handle    Handle to the current media.
 *
 * @return   Handle to a media.
 *******************************************************************************************************/
FS_MEDIA_HANDLE FSMedia_NextAcquire(FS_MEDIA_HANDLE media_handle)
{
  FS_OBJ_HANDLE obj_handle;

  FSMedia_ListLock();
  obj_handle = FSObj_NextRefAcquireLocked(FSMedia_ToObjHandle(media_handle));
  FSMedia_ListUnlock();

  FSMedia_Release(media_handle);

  return (FSMedia_ObjToMedia(obj_handle));
}

/****************************************************************************************************//**
 *                                               FSMedia_Release()
 *
 * @brief    Release a media.
 *
 * @param    media_handle    Handle to a media.
 *******************************************************************************************************/
void FSMedia_Release(FS_MEDIA_HANDLE media_handle)
{
  FSObj_RefRelease(FSMedia_ToObj(media_handle.MediaPtr), FSMedia_OnNullRefCnt);
}

/****************************************************************************************************//**
 *                                       FSMedia_PollTaskPeriodSet()
 *
 * @brief    Set the media poll task period.
 *
 * @param    period_ms   Interval of time (in miliseconds) between two polling.
 *
 * @param    p_err       Pointer to variable that will receive the return error code(s) from this
 *                       function:
 *                           - RTOS_ERR_NONE
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_MEDIA_POLL_TASK_EN == DEF_ENABLED)
void FSMedia_PollTaskPeriodSet(CPU_INT32U period_ms,
                               RTOS_ERR   *p_err)
{
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  RTOS_ASSERT_DBG(FSMedia_Data.IsInit, RTOS_ERR_NOT_INIT,; );

  CORE_ENTER_ATOMIC();
  FSMedia_Data.MediaPollPeriodMs = period_ms;
  CORE_EXIT_ATOMIC();
}
#endif

/****************************************************************************************************//**
 *                                           FSMedia_PollTaskPrioSet()
 *
 * @brief    Set the priority of the poll task.
 *
 * @param    prio    New priority for the poll task.
 *
 * @param    p_err   Pointer to variable that will receive the return error code(s) from this function:
 *                       - RTOS_ERR_NONE
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_MEDIA_POLL_TASK_EN == DEF_ENABLED)
void FSMedia_PollTaskPrioSet(RTOS_TASK_PRIO prio,
                             RTOS_ERR       *p_err)
{
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  RTOS_ASSERT_DBG(FSMedia_Data.IsInit, RTOS_ERR_NOT_INIT,; );

  KAL_TaskPrioSet(FSMedia_Data.MediaPollTaskHandle, prio, p_err);
  RTOS_ASSERT_DBG_ERR_SET(RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE,
                          *p_err, RTOS_ERR_CODE_GET(*p_err),; );
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           FSMedia_PmItemGetPred()
 *
 * @brief    Platform manager media lookup predicate.
 *
 * @param    p_item          Pointer to the current platform manager item.
 *
 * @param    p_pred_data_v   Pointer to predicate-specific caller data.
 *
 * @return   DEF_YES, if the item is a media item.
 *           DEF_NO,  otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN FSMedia_PmItemGetPred(const PLATFORM_MGR_ITEM *p_item,
                                         void                    *p_pred_data_v)
{
  FS_MEDIA_PM_ITEM_GET_PRED_DATA *p_pred_data;

  p_pred_data = (FS_MEDIA_PM_ITEM_GET_PRED_DATA *)p_pred_data_v;

  switch (p_item->Type) {
    case PLATFORM_MGR_ITEM_TYPE_HW_INFO_FS_NAND:
    case PLATFORM_MGR_ITEM_TYPE_HW_INFO_FS_NOR:
    case PLATFORM_MGR_ITEM_TYPE_HW_INFO_FS_SD_CARD:
    case PLATFORM_MGR_ITEM_TYPE_HW_INFO_FS_SD_SPI:
    case PLATFORM_MGR_ITEM_TYPE_HW_INFO_FS_SCSI:
    case PLATFORM_MGR_ITEM_TYPE_HW_INFO_FS_RAM_DISK:
    case PLATFORM_MGR_ITEM_TYPE_HW_INFO_FS_VDI:
      if (p_pred_data->CurIx == p_pred_data->TargetIx) {
        return (DEF_YES);
      }
      p_pred_data->CurIx += 1u;
      break;
    default:
      break;
  }

  return (DEF_NO);
}

/****************************************************************************************************//**
 *                                           FSMedia_ToObjHandle()
 *
 * @brief    Convert the media handle into a generic object handle.
 *
 * @param    media_handle    Handle to a media.
 *
 * @return   Handle to a generic object.
 *******************************************************************************************************/
FS_OBJ_HANDLE FSMedia_ToObjHandle(FS_MEDIA_HANDLE media_handle)
{
  FS_OBJ_HANDLE obj_handle;

  obj_handle.ObjPtr = (FS_OBJ *)media_handle.MediaPtr;
  obj_handle.ObjId = media_handle.MediaId;

  return (obj_handle);
}

/****************************************************************************************************//**
 *                                               FSMedia_ToObj()
 *
 * @brief    Convert a working directory to an object.
 *
 * @param    p_media     Pointer to a media.
 *
 * @return   Pointer to an object.
 *******************************************************************************************************/
FS_OBJ *FSMedia_ToObj(FS_MEDIA *p_media)
{
  return ((FS_OBJ *)p_media);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_FS_AVAIL
