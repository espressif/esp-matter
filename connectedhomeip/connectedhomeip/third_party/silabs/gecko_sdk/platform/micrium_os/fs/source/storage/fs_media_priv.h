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
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  FS_MEDIA_PRIV_H_
#define  FS_MEDIA_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------------------ FS ------------------------
#include  <fs_storage_cfg.h>
#include  <fs/source/shared/cleanup/cleanup_mgmt_priv.h>
#include  <fs/include/fs_media.h>
#include  <fs/source/shared/fs_obj_priv.h>

//                                                                 ----------------------- EXT ------------------------
#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>
#include  <common/include/platform_mgr.h>
#include  <common/source/collections/slist_priv.h>
#include  <common/source/op_lock/op_lock_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef struct fs_media_pm_item FS_MEDIA_PM_ITEM;

typedef struct fs_media_data {
#if (FS_STORAGE_CFG_MEDIA_POLL_TASK_EN == DEF_ENABLED)
  CPU_INT32U      MediaPollPeriodMs;
  KAL_TASK_HANDLE MediaPollTaskHandle;
#endif
  CPU_BOOLEAN     IsInit;
  CPU_INT16U      CurId;
  SLIST_MEMBER    *MediaListHeadPtr;
  KAL_LOCK_HANDLE MediaListLockHandle;
  CPU_SIZE_T      FixedMediaCnt;
} FS_MEDIA_DATA;

typedef struct fs_media {
  FS_OBJ_FIELDS
  KAL_LOCK_HANDLE        LockHandle;
  CPU_BOOLEAN            IsConn;
  const FS_MEDIA_PM_ITEM *PmItemPtr;
} FS_MEDIA;

typedef struct fs_media_pm_item_get_pred_data {
  CPU_SIZE_T CurIx;
  CPU_SIZE_T TargetIx;
} FS_MEDIA_PM_ITEM_GET_PRED_DATA;
//                                                                 *INDENT-OFF*
typedef struct fs_media_api {
  FS_MEDIA *(*Add)(const FS_MEDIA_PM_ITEM *p_media_pm_item,
                   RTOS_ERR               *p_err);

  void (*Rem)(FS_MEDIA *p_media,
              RTOS_ERR *p_err);

  CPU_SIZE_T (*AlignReqGet)(FS_MEDIA *p_media,
                            RTOS_ERR *p_err);

  CPU_BOOLEAN (*IsConn)(FS_MEDIA *p_media);                                                                
} FS_MEDIA_API;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

extern const FS_MEDIA_INIT_CFG *FSMedia_InitCfgPtr;

extern FS_MEDIA_DATA FSMedia_Data;
//                                                                 *INDENT-ON*
/********************************************************************************************************
 ********************************************************************************************************
 *                                               MACRO'S
 ********************************************************************************************************
 *******************************************************************************************************/

#define  FS_MEDIA_LOCK_WITH(p_media)            \
  if (1) {                                      \
    FSMedia_Lock(p_media);                      \
    goto PP_UNIQUE_LABEL(fs_media_lock_enter);  \
  } else PP_UNIQUE_LABEL(fs_media_lock_enter) : \
    ON_EXIT {                                   \
    FSMedia_Unlock(p_media);                    \
  } WITH

#define  FS_MEDIA_WITH_NO_IO(media_handle, p_err)            FS_OBJ_WITH_NO_IO(media_handle, p_err, FSMedia, RTOS_ERR_IO)

#define  SPI_SLAVE_WITH(spi_handle, p_err)             \
  if (1) {                                             \
    SPI_SlaveSel(spi_handle, 0u, SPI_OPT_NONE, p_err); \
    if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {  \
      goto PP_UNIQUE_LABEL(spi_slave_with_enter);      \
    }                                                  \
  } else PP_UNIQUE_LABEL(spi_slave_with_enter) :       \
    ON_EXIT {                                          \
    SPI_SlaveDesel(spi_handle, p_err);                 \
  } WITH

#define  FS_MEDIA_FOR_EACH(media_handle)               FS_OBJ_FOR_EACH(media_handle, FSMedia, FS_MEDIA)

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

FS_MEDIA *FSMedia_Add(const FS_MEDIA_PM_ITEM *p_media_pm_item,
                      RTOS_ERR               *p_err);

void FSMedia_Rem(FS_MEDIA *p_media);

void FSMedia_Lock(FS_MEDIA *p_media);

CPU_BOOLEAN FSMedia_TryLock(FS_MEDIA *p_media);

void FSMedia_Unlock(FS_MEDIA *p_media);

void FSMedia_Conn(FS_MEDIA *p_media);

void FSMedia_Disconn(FS_MEDIA *p_media);

FS_OBJ_HANDLE FSMedia_ToObjHandle(FS_MEDIA_HANDLE media_handle);

FS_OBJ *FSMedia_ToObj(FS_MEDIA *p_media);

void FSMedia_OnNullRefCnt(FS_OBJ *p_obj);

FS_MEDIA_HANDLE FSMedia_FirstAcquire(void);

void FSMedia_Release(FS_MEDIA_HANDLE media_handle);

FS_MEDIA_HANDLE FSMedia_NextAcquire(FS_MEDIA_HANDLE media_handle);

#if (FS_STORAGE_CFG_MEDIA_POLL_TASK_EN == DEF_ENABLED)
void FSMedia_PollTaskPrioSet(RTOS_TASK_PRIO prio,
                             RTOS_ERR       *p_err);

void FSMedia_PollTaskPeriodSet(CPU_INT32U period_ms,
                               RTOS_ERR   *p_err);
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
