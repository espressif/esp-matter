/***************************************************************************//**
 * @file
 * @brief File System - Ram Disk Media Driver
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

#if  (defined(RTOS_MODULE_FS_STORAGE_RAM_DISK_AVAIL))

#if (!defined(RTOS_MODULE_FS_AVAIL))

#error RAM Disk module requires File System Storage module. Make sure it is part of your project and that \
  RTOS_MODULE_FS_AVAIL is defined in rtos_description.h.

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------------------ FS ------------------------
#include  <fs_storage_cfg.h>
#include  <fs/source/storage/ramdisk/fs_ramdisk_priv.h>

#include  <fs/source/storage/fs_blk_dev_priv.h>
#include  <fs/source/shared/fs_utils_priv.h>
//                                                                 ----------------------- EXT ------------------------
#include  <common/include/lib_ascii.h>
#include  <common/include/lib_mem.h>
#include  <common/include/lib_utils.h>

#include  <common/source/platform_mgr/platform_mgr_priv.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/logging/logging_priv.h>

#include  <em_core.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  RTOS_MODULE_CUR                            RTOS_CFG_MODULE_FS
#define  LOG_DFLT_CH                                (FS, DRV, RAMDISK)

#define  FS_RAM_DISK_MEDIA_NAME_MAX_LEN             20u

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

struct fs_ram_disk {
  FS_MEDIA         Media;
  FS_BLK_DEV       BlkDev;
  FS_MEDIA_PM_ITEM MediaPmItem;
  void             *DiskPtr;
  FS_LB_QTY        LbCnt;
  FS_LB_SIZE       LbSize;
  CPU_BOOLEAN      IsConn;
  CPU_CHAR         Name[FS_RAM_DISK_MEDIA_NAME_MAX_LEN];
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static FS_MEDIA *FS_RAM_Disk_MediaAdd(const FS_MEDIA_PM_ITEM *p_pm_item,
                                      RTOS_ERR               *p_err);

static void FS_RAM_Disk_MediaRem(FS_MEDIA *p_media,
                                 RTOS_ERR *p_err);

static FS_BLK_DEV *FS_RAM_Disk_BlkDevAdd(FS_MEDIA *p_media,
                                         RTOS_ERR *p_err);

static void FS_RAM_Disk_BlkDevRem(FS_BLK_DEV *p_blk_dev,
                                  RTOS_ERR   *p_err);

static void FS_RAM_Disk_Open(FS_BLK_DEV *p_blk_dev,
                             RTOS_ERR   *p_err);

static void FS_RAM_Disk_Close(FS_BLK_DEV *p_blk_dev,
                              RTOS_ERR   *p_err);

static void FS_RAM_Disk_Rd(FS_BLK_DEV *p_blk_dev,
                           void       *p_dest,
                           FS_LB_NBR  start,
                           FS_LB_QTY  cnt,
                           RTOS_ERR   *p_err);

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_RAM_Disk_Wr(FS_BLK_DEV *p_blk_dev,
                           void       *p_src,
                           FS_LB_NBR  start,
                           FS_LB_QTY  cnt,
                           RTOS_ERR   *p_err);

static void FS_RAM_Disk_Sync(FS_BLK_DEV *p_blk_dev,
                             RTOS_ERR   *p_err);

static void FS_RAM_Disk_Trim(FS_BLK_DEV *p_blk_dev,
                             FS_LB_NBR  lb_nbr,
                             RTOS_ERR   *p_err);
#endif

static void FS_RAM_Disk_Query(FS_BLK_DEV      *p_blk_dev,
                              FS_BLK_DEV_INFO *p_info,
                              RTOS_ERR        *p_err);

static CPU_SIZE_T FS_RAM_Disk_AlignReqGet(FS_MEDIA *p_media,
                                          RTOS_ERR *p_err);

static CPU_BOOLEAN FS_RAM_Disk_IsConn(FS_MEDIA *p_media);

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

const FS_MEDIA_API FS_RAM_Disk_MediaApi = {
  .Add = FS_RAM_Disk_MediaAdd,
  .Rem = FS_RAM_Disk_MediaRem,
  .AlignReqGet = FS_RAM_Disk_AlignReqGet,
  .IsConn = FS_RAM_Disk_IsConn
};

const FS_BLK_DEV_API FS_RAM_Disk_BlkDevApi = {
  .Add = FS_RAM_Disk_BlkDevAdd,
  .Rem = FS_RAM_Disk_BlkDevRem,
  .Open = FS_RAM_Disk_Open,
  .Close = FS_RAM_Disk_Close,
  .Rd = FS_RAM_Disk_Rd,
#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
  .Wr = FS_RAM_Disk_Wr,
  .Sync = FS_RAM_Disk_Sync,
  .Trim = FS_RAM_Disk_Trim,
#endif
  .Query = FS_RAM_Disk_Query,
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               FS_RAM_Disk_Add()
 *
 * @brief    Add a RAM disk instance.
 *
 * @param    name    Media name to be assigned to the created RAM disk.
 *
 * @param    p_cfg   Pointer to a RAM disk configuration structure.
 *
 * @param    p_err   Pointer to variable that will receive the return error code(s) from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_NOT_INIT
 *                       - RTOS_ERR_SEG_OVF
 *******************************************************************************************************/
void FS_RAM_Disk_Add(const CPU_CHAR        *name,
                     const FS_RAM_DISK_CFG *p_cfg,
                     RTOS_ERR              *p_err)
{
  FS_RAM_DISK *p_ram_disk;
  CORE_DECLARE_IRQ_STATE;

  //                                                               Check that Storage layer is initialized.
  CORE_ENTER_ATOMIC();
  if (!FSBlkDev_Data.IsInit) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_INIT);
    CORE_EXIT_ATOMIC();
    return;
  }
  CORE_EXIT_ATOMIC();

  p_ram_disk = FS_RAM_Disk_Create(p_cfg, FSMedia_InitCfgPtr->MemSegPtr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  FS_RAM_Disk_Register(name, p_ram_disk, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  FS_RAM_Disk_Connect(p_ram_disk);
}

/****************************************************************************************************//**
 *                                           FS_RAM_Disk_Create()
 *
 * @brief    Create a RAM disk.
 *
 * @param    name    Name of the RAM disk.
 *
 * @param    p_seg   Pointer to a memory segment.
 *
 * @param    p_err   Pointer to variable that will receive the return error code(s) from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_SEG_OVF
 *******************************************************************************************************/
FS_RAM_DISK *FS_RAM_Disk_Create(const FS_RAM_DISK_CFG *p_cfg,
                                MEM_SEG               *p_seg,
                                RTOS_ERR              *p_err)
{
  FS_RAM_DISK *p_ram_disk;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NULL);
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  RTOS_ASSERT_DBG_ERR_SET(FSBlkDev_Data.IsInit, *p_err, RTOS_ERR_NOT_INIT, DEF_NULL)

  RTOS_ASSERT_DBG_ERR_SET((p_cfg != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_NULL);
  RTOS_ASSERT_DBG_ERR_SET(FS_UTIL_IS_PWR2(p_cfg->LbSize), *p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);

  p_ram_disk = (FS_RAM_DISK *)Mem_SegAlloc("FS - RAM disk instance",
                                           p_seg,
                                           sizeof(FS_RAM_DISK),
                                           p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  p_ram_disk->LbCnt = p_cfg->LbCnt;
  p_ram_disk->LbSize = p_cfg->LbSize;
  p_ram_disk->IsConn = DEF_NO;
  p_ram_disk->Name[0] = '\0';

  if (p_cfg->DiskPtr != DEF_NULL) {
    p_ram_disk->DiskPtr = p_cfg->DiskPtr;
  } else {
    p_ram_disk->DiskPtr = Mem_SegAlloc("FS - RAM Disk mem region",
                                       p_seg,
                                       p_cfg->LbCnt * p_cfg->LbSize,
                                       p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (DEF_NULL);
    }
  }

  return (p_ram_disk);
}

/****************************************************************************************************//**
 *                                           FS_RAM_Disk_Register()
 *
 * @brief    Register a RAM disk in the Platform Manager.
 *
 * @param    name        Pointer to a name identifying a RAM disk.
 *
 * @param    p_ram_disk  Pointer to a RAM disk information structure.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
void FS_RAM_Disk_Register(const CPU_CHAR *name,
                          FS_RAM_DISK    *p_ram_disk,
                          RTOS_ERR       *p_err)
{
  Str_Copy_N(p_ram_disk->Name, name, FS_RAM_DISK_MEDIA_NAME_MAX_LEN);

  p_ram_disk->MediaPmItem.PmItem.StrID = p_ram_disk->Name;
  p_ram_disk->MediaPmItem.PmItem.Type = PLATFORM_MGR_ITEM_TYPE_HW_INFO_FS_RAM_DISK;
  p_ram_disk->MediaPmItem.MediaApiPtr = &FS_RAM_Disk_MediaApi;

  (void *)FSMedia_Add(&p_ram_disk->MediaPmItem, p_err);
}

/****************************************************************************************************//**
 *                                           FS_RAM_Disk_Unregister()
 *
 * @brief    Unregister a RAM disk.
 *
 * @param    p_ram_disk  Pointer to a RAM disk information structure.
 *******************************************************************************************************/
void FS_RAM_Disk_Unregister(FS_RAM_DISK *p_ram_disk)
{
  FSMedia_Rem((FS_MEDIA *)p_ram_disk);
}

/****************************************************************************************************//**
 *                                           FS_RAM_Disk_Connect()
 *
 * @brief    Indicate RAM disk as connected.
 *
 * @param    p_ram_disk  Pointer to a RAM disk information structure.
 *******************************************************************************************************/
void FS_RAM_Disk_Connect(FS_RAM_DISK *p_ram_disk)
{
  p_ram_disk->IsConn = DEF_YES;
}

/****************************************************************************************************//**
 *                                           FS_RAM_Disk_Disconnect()
 *
 * @brief    Indicate RAM disk as disconnected.
 *
 * @param    p_ram_disk  Pointer to a RAM disk information structure.
 *******************************************************************************************************/
void FS_RAM_Disk_Disconnect(FS_RAM_DISK *p_ram_disk)
{
  p_ram_disk->IsConn = DEF_NO;
}

/****************************************************************************************************//**
 *                                           FS_RAM_Disk_NameGet()
 *
 * @brief    Get name associated to a RAM disk.
 *
 * @param    p_ram_disk  Pointer to a RAM disk information structure.
 *******************************************************************************************************/
const CPU_CHAR *FS_RAM_Disk_NameGet(FS_RAM_DISK *p_ram_disk)
{
  return (p_ram_disk->MediaPmItem.PmItem.StrID);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                       DRIVER INTERFACE FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           FS_RAM_Disk_MediaAdd()
 *
 * @brief    Add a RAM disk instance.
 *
 * @param    p_pm_item   Pointer to a media platform manager item.
 *
 * @param    p_err       Error pointer.
 *
 * @return   Pointer to the added media.
 *******************************************************************************************************/
static FS_MEDIA *FS_RAM_Disk_MediaAdd(const FS_MEDIA_PM_ITEM *p_pm_item,
                                      RTOS_ERR               *p_err)
{
  PlatformMgrItemAdd(&p_pm_item->PmItem, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  return ((FS_MEDIA *)CONTAINER_OF(p_pm_item, FS_RAM_DISK, MediaPmItem));
}

/****************************************************************************************************//**
 *                                           FS_RAM_Disk_MediaRem()
 *
 * @brief    Remove a RAM disk media instance.
 *
 * @param    p_media     Pointer to a RAM disk instance.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_RAM_Disk_MediaRem(FS_MEDIA *p_media,
                                 RTOS_ERR *p_err)
{
  PlatformMgrItemRem(&p_media->PmItemPtr->PmItem, p_err);
}

/****************************************************************************************************//**
 *                                           FS_RAM_Disk_BlkDevAdd()
 *
 * @brief    Add a RAM disk block device.
 *
 * @param    p_media     Pointer to a RAM disk media instance.
 *
 * @param    p_err       Error pointer.
 *
 * @return   Pointer to block device.
 *******************************************************************************************************/
static FS_BLK_DEV *FS_RAM_Disk_BlkDevAdd(FS_MEDIA *p_media,
                                         RTOS_ERR *p_err)
{
  PP_UNUSED_PARAM(p_err);

  return (&((FS_RAM_DISK *)p_media)->BlkDev);
}

/****************************************************************************************************//**
 *                                           FS_RAM_Disk_BlkDevRem()
 *
 * @brief    Remove a RAM disk block device.
 *
 * @param    p_blk_dev   Pointer to a RAM disk block device.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_RAM_Disk_BlkDevRem(FS_BLK_DEV *p_blk_dev,
                                  RTOS_ERR   *p_err)
{
  PP_UNUSED_PARAM(p_err);
  PP_UNUSED_PARAM(p_blk_dev);
}

/****************************************************************************************************//**
 *                                           FS_RAM_Disk_Open()
 *
 * @brief    Open a RAM disk.
 *
 * @param    p_blk_dev   Pointer to a RAM disk block device.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_RAM_Disk_Open(FS_BLK_DEV *p_blk_dev,
                             RTOS_ERR   *p_err)
{
  FS_RAM_DISK *p_ram_disk_data;

  p_ram_disk_data = (FS_RAM_DISK *)p_blk_dev->MediaPtr;
  if (!p_ram_disk_data->IsConn) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
  }
}

/****************************************************************************************************//**
 *                                           FS_RAM_Disk_Close()
 *
 * @brief    Close a RAM disk.
 *
 * @param    p_blk_dev   Pointer to a RAM disk block device.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_RAM_Disk_Close(FS_BLK_DEV *p_blk_dev,
                              RTOS_ERR   *p_err)
{
  FS_RAM_DISK *p_ram_disk_data;

  p_ram_disk_data = (FS_RAM_DISK *)p_blk_dev->MediaPtr;
  if (!p_ram_disk_data->IsConn) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
  }
}

/****************************************************************************************************//**
 *                                               FS_RAM_Disk_Rd()
 *
 * @brief    Read blocks from a RAM disk.
 *
 * @param    p_blk_dev   Pointer to a RAM disk.
 *
 * @param    p_dest      Pointer to a destination buffer.
 *
 * @param    start       Start logical block number.
 *
 * @param    cnt         Number of logical block to read.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_RAM_Disk_Rd(FS_BLK_DEV *p_blk_dev,
                           void       *p_dest,
                           FS_LB_NBR  start,
                           FS_LB_QTY  cnt,
                           RTOS_ERR   *p_err)
{
  FS_RAM_DISK *p_ram_disk;
  void        *p_disk;

  PP_UNUSED_PARAM(p_err);

  p_ram_disk = (FS_RAM_DISK *)p_blk_dev->MediaPtr;
  if (!p_ram_disk->IsConn) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
  }

  p_disk = (void *)((CPU_INT08U *)(p_ram_disk->DiskPtr) + (start << p_blk_dev->LbSizeLog2));

  Mem_Copy(p_dest, p_disk, cnt << p_blk_dev->LbSizeLog2);
}

/****************************************************************************************************//**
 *                                               FS_RAM_Disk_Wr()
 *
 * @brief    Write blocks to a RAM disk.
 *
 * @param    p_blk_dev   Pointer to a RAM disk.
 *
 * @param    p_src       Pointer to a source buffer.
 *
 * @param    start       Start logical block number.
 *
 * @param    cnt         Logical block count.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_RAM_Disk_Wr(FS_BLK_DEV *p_blk_dev,
                           void       *p_src,
                           FS_LB_NBR  start,
                           FS_LB_QTY  cnt,
                           RTOS_ERR   *p_err)
{
  FS_RAM_DISK *p_ram_disk;
  void        *p_disk;

  PP_UNUSED_PARAM(p_err);

  p_ram_disk = (FS_RAM_DISK *)p_blk_dev->MediaPtr;
  if (!p_ram_disk->IsConn) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
  }

  p_disk = (void *)((CPU_INT08U *)(p_ram_disk->DiskPtr) + (start << p_blk_dev->LbSizeLog2));

  Mem_Copy(p_disk, p_src, cnt << p_blk_dev->LbSizeLog2);
}
#endif

/****************************************************************************************************//**
 *                                           FS_RAM_Disk_Query()
 *
 * @brief    Get information about a RAM disk.
 *
 * @param    p_blk_dev   Pointer to a RAM disk.
 *
 * @param    p_info      Pointer to structure that will receive device information.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_RAM_Disk_Query(FS_BLK_DEV      *p_blk_dev,
                              FS_BLK_DEV_INFO *p_info,
                              RTOS_ERR        *p_err)
{
  FS_RAM_DISK *p_ram_disk;

  PP_UNUSED_PARAM(p_err);

  p_ram_disk = (FS_RAM_DISK *)p_blk_dev->MediaPtr;
  if (!p_ram_disk->IsConn) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
  }

  p_info->LbSizeLog2 = FSUtil_Log2(p_ram_disk->LbSize);
  p_info->LbCnt = p_ram_disk->LbCnt;
  p_info->Fixed = DEF_YES;
}

/****************************************************************************************************//**
 *                                           FS_RAM_Disk_StateGet()
 *
 * @brief    Get RAM disk state.
 *
 * @param    p_blk_dev   Pointer to a RAM disk.
 *
 * @return   RAM disk state.
 *******************************************************************************************************/
static CPU_BOOLEAN FS_RAM_Disk_IsConn(FS_MEDIA *p_media)
{
  FS_RAM_DISK *p_ram_disk_data;

  p_ram_disk_data = (FS_RAM_DISK *)p_media;

  return (p_ram_disk_data->IsConn);
}

/****************************************************************************************************//**
 *                                           FS_RAM_Disk_Sync()
 *
 * @brief    Sync RAM disk.
 *
 * @param    p_blk_dev   Pointer to a RAM disk.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_RAM_Disk_Sync(FS_BLK_DEV *p_blk_dev,
                             RTOS_ERR   *p_err)
{
  FS_RAM_DISK *p_ram_disk_data;

  p_ram_disk_data = (FS_RAM_DISK *)p_blk_dev->MediaPtr;
  if (!p_ram_disk_data->IsConn) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
  }
}
#endif

/****************************************************************************************************//**
 *                                           FS_RAM_Disk_Trim()
 *
 * @brief    Trim a RAM disk.
 *
 * @param    p_blk_dev   Pointer to a RAM disk.
 *
 * @param    lb_nbr      Logical block number of the block to be trimmed.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_RAM_Disk_Trim(FS_BLK_DEV *p_blk_dev,
                             FS_LB_NBR  lb_nbr,
                             RTOS_ERR   *p_err)
{
  FS_RAM_DISK *p_ram_disk_data;

  PP_UNUSED_PARAM(lb_nbr);

  p_ram_disk_data = (FS_RAM_DISK *)p_blk_dev->MediaPtr;
  if (!p_ram_disk_data->IsConn) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
  }
}
#endif

/****************************************************************************************************//**
 *                                           FS_RAM_Disk_AlignReqGet()
 *
 * @brief    Get alignment requirement.
 *
 * @param    p_media     Pointer to a RAM disk media.
 *
 * @param    p_err       Error pointer.
 *
 * @return   Alignment requirement.
 *******************************************************************************************************/
static CPU_SIZE_T FS_RAM_Disk_AlignReqGet(FS_MEDIA *p_media,
                                          RTOS_ERR *p_err)
{
  PP_UNUSED_PARAM(p_media);
  PP_UNUSED_PARAM(p_err);

  return (sizeof(CPU_ALIGN));
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_FS_STORAGE_RAM_DISK_AVAIL
