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

/****************************************************************************************************//**
 * @addtogroup FS_STORAGE
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  FS_MEDIA_H_
#define  FS_MEDIA_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------------------ FS ------------------------
#include  <fs_storage_cfg.h>
#include  <fs/include/fs_obj.h>

//                                                                 ----------------------- EXT ------------------------
#include  <common/include/lib_mem.h>
#include  <common/include/rtos_err.h>
#include  <common/include/rtos_types.h>
#include  <cpu/include/cpu.h>
#include  <common/include/platform_mgr.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MACROS
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 Verify whether a media handle is null.
#define  FS_MEDIA_HANDLE_IS_NULL(media_handle)         ((media_handle.MediaPtr == DEF_NULL) \
                                                        && (media_handle.MediaId == 0u))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef CPU_INT32U FS_LB_QTY;

typedef FS_LB_QTY FS_LB_NBR;
typedef CPU_INT32U FS_LB_SIZE;

typedef struct fs_media_handle {
  struct fs_media *MediaPtr;
  FS_OBJ_ID       MediaId;
} FS_MEDIA_HANDLE;

#if (FS_STORAGE_CFG_MEDIA_POLL_TASK_EN == DEF_ENABLED)
typedef struct fs_media_poll_task_cfg {
  CPU_INT32U StkSizeElements;
  void       *StkPtr;
} FS_MEDIA_POLL_TASK_CFG;
#endif

typedef void (*FS_MEDIA_CONN_CB) (FS_MEDIA_HANDLE media_handle);

typedef struct fs_media_init_cfg {
#if (FS_STORAGE_CFG_MEDIA_POLL_TASK_EN == DEF_ENABLED)
  CPU_INT32U       PollTaskStkSizeElements;
  void             *PollTaskStkPtr;
#endif
  FS_MEDIA_CONN_CB OnConn;
  FS_MEDIA_CONN_CB OnDisconn;
  MEM_SEG          *MemSegPtr;
  CPU_SIZE_T       MaxSCSILuCnt;
} FS_MEDIA_INIT_CFG;

struct fs_media_pm_item {
  PLATFORM_MGR_ITEM         PmItem;
  const struct fs_media_api *MediaApiPtr;
};

typedef enum fs_media_type {
  FS_MEDIA_TYPE_GENERIC = PLATFORM_MGR_ITEM_TYPE_GENERIC,
  FS_MEDIA_TYPE_RAM_DISK = PLATFORM_MGR_ITEM_TYPE_HW_INFO_FS_RAM_DISK,
  FS_MEDIA_TYPE_VDI = PLATFORM_MGR_ITEM_TYPE_HW_INFO_FS_VDI,
  FS_MEDIA_TYPE_NOR = PLATFORM_MGR_ITEM_TYPE_HW_INFO_FS_NOR,
  FS_MEDIA_TYPE_NAND = PLATFORM_MGR_ITEM_TYPE_HW_INFO_FS_NAND,
  FS_MEDIA_TYPE_SD_SPI = PLATFORM_MGR_ITEM_TYPE_HW_INFO_FS_SD_SPI,
  FS_MEDIA_TYPE_SD_CARD = PLATFORM_MGR_ITEM_TYPE_HW_INFO_FS_SD_CARD,
  FS_MEDIA_TYPE_SCSI = PLATFORM_MGR_ITEM_TYPE_HW_INFO_FS_SCSI,
} FS_MEDIA_TYPE;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

extern const FS_MEDIA_HANDLE FSMedia_NullHandle;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

void FSMedia_Init(const FS_MEDIA_INIT_CFG *p_media_cfg,
                  RTOS_ERR                *p_err);

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FSMedia_LowFmt(FS_MEDIA_HANDLE media_handle,
                    RTOS_ERR        *p_err);
#endif

CPU_SIZE_T FSMedia_MaxCntGet(void);

CPU_SIZE_T FSMedia_AlignReqGet(FS_MEDIA_HANDLE media_handle,
                               RTOS_ERR        *p_err);

CPU_BOOLEAN FSMedia_IsConn(FS_MEDIA_HANDLE media_handle);

FS_MEDIA_TYPE FSMedia_TypeGet(FS_MEDIA_HANDLE media_handle,
                              RTOS_ERR        *p_err);

void FSMedia_NameGet(FS_MEDIA_HANDLE media_handle,
                     CPU_CHAR        *p_buf,
                     CPU_SIZE_T      buf_size,
                     RTOS_ERR        *p_err);

const CPU_CHAR *FSMedia_TypeStrGet(FS_MEDIA_TYPE type);

FS_MEDIA_HANDLE FSMedia_Get(const CPU_CHAR *p_name);

FS_MEDIA_HANDLE FSMedia_FirstGet(void);

#ifdef __cplusplus
}
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                       MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
