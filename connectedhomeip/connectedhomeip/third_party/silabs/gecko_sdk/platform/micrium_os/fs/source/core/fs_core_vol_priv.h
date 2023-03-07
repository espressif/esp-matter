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
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  FS_CORE_VOL_PRIV_H_
#define  FS_CORE_VOL_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------------------ FS ------------------------
#include  <fs_core_cfg.h>
#include  <fs/include/fs_core_vol.h>
#include  <fs/source/storage/fs_blk_dev_priv.h>
#include  <fs/source/shared/fs_obj_priv.h>
#include  <fs/source/core/fs_core_cache_priv.h>
#include  <fs/source/sys/fs_sys_priv.h>

//                                                                 ----------------------- EXT ------------------------
#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>
#include  <common/source/collections/slist_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                                   DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ----------------- VOL STATUS FLAGS -----------------
#define  FS_VOL_STATUS_ACCESS_MODE_NONE        DEF_BIT_NONE
#define  FS_VOL_STATUS_ACCESS_MODE_RD_ONLY     DEF_BIT_00

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 --------------------- VOL DESC ---------------------
typedef struct fs_vol {
  FS_OBJ_FIELDS
#if (FS_CORE_CFG_THREAD_SAFETY_EN == DEF_ENABLED)
  OP_LOCK OpLock;
#endif
  FS_PARTITION_NBR PartitionNbr;
  FS_LB_NBR             PartitionStart;
  FS_LB_QTY             PartitionSize;
  FS_BLK_DEV_HANDLE     BlkDevHandle;
  const FS_SYS_API      *SysPtr;
  CPU_INT32U            AccessMode : 1;
  CPU_INT32U            AutoSync : 1;
  CPU_CHAR              Name[FS_CORE_CFG_MAX_VOL_NAME_LEN + 1u];
#if (FS_CORE_CFG_CTR_STAT_EN == DEF_ENABLED)
  FS_CTR                StatRdSecCtr;
  FS_CTR                StatWrSecCtr;
#endif
  FS_CACHE_BLK_DEV_DATA *CacheBlkDevDataPtr;
} FS_VOL;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

extern SLIST_MEMBER *FSVol_OpenListHeadPtr;

/********************************************************************************************************
 ********************************************************************************************************
 *                                                   MACRO'S
 *
 * Note(s): (1) The browsing operation is thread safe: volumes may concurrently be opened / closed from
 *               other tasks.
 ********************************************************************************************************
 *******************************************************************************************************/

#define  FS_VOL_HANDLE_IS_VALID(h)               (!FS_VOL_HANDLE_IS_NULL(h) && ((h).VolId == (h).VolPtr->Id))

//                                                                 Iterate through opened volumes (see Note #1).
#define  FS_VOL_FOR_EACH(cur_vol_handle)         FS_OBJ_FOR_EACH(cur_vol_handle, FSVol, FS_VOL)

//                                                                 --------------- REF ACQUIRE/RELEASE ----------------
#define  FS_VOL_WITH(vol_handle, p_err)          FS_OBJ_WITH(vol_handle, p_err, FSVol, RTOS_ERR_VOL_CLOSED)

#define  FS_VOL_WITH_NO_IO(vol_handle, p_err)    FS_OBJ_WITH_NO_IO(vol_handle, p_err, FSVol, RTOS_ERR_VOL_CLOSED)

//                                                                 ------------ CACHE BLK ACQUIRE/RELEASE -------------

#define  FS_VOL_CACHE_BLK_WR(p_vol, sec_nbr, lb_type, prev_wr_job_handle, pp_buf, p_wr_job_handle, p_err) \
  FS_VOL_CACHE_BLK_ADD(p_vol, sec_nbr, lb_type, FS_CACHE_BLK_GET_MODE_WR, prev_wr_job_handle, pp_buf, p_wr_job_handle, p_err)

#define  FS_VOL_CACHE_BLK_RW(p_vol, sec_nbr, lb_type, prev_wr_job_handle, pp_buf, p_wr_job_handle, p_err) \
  FS_VOL_CACHE_BLK_ADD(p_vol, sec_nbr, lb_type, FS_CACHE_BLK_GET_MODE_RW, prev_wr_job_handle, pp_buf, p_wr_job_handle, p_err)

#define  FS_VOL_CACHE_BLK_RD(p_vol, sec_nbr, lb_type, pp_buf, p_err) \
  FS_VOL_CACHE_BLK_ADD(p_vol, sec_nbr, lb_type, FS_CACHE_BLK_GET_MODE_RD, FSCache_VoidWrJobHandle, pp_buf, DEF_NULL, p_err)

#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
#define  FS_VOL_CACHE_BLK_ADD(p_vol, sec_nbr, sec_type, mode, prev_wr_job_handle, pp_buf, p_wr_job_handle, p_err) \
  if (1) {                                                                                                        \
    *(pp_buf) = FSCache_BlkAcquireOrdered((p_vol)->CacheBlkDevDataPtr,                                            \
                                          (p_vol)->PartitionStart + sec_nbr,                                      \
                                          sec_type,                                                               \
                                          mode,                                                                   \
                                          prev_wr_job_handle,                                                     \
                                          p_wr_job_handle,                                                        \
                                          p_err);                                                                 \
    if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {                                                             \
      goto PP_UNIQUE_LABEL(fs_vol_cache_blk_add_enter);                                                           \
    }                                                                                                             \
  } else PP_UNIQUE_LABEL(fs_vol_cache_blk_add_enter) :                                                            \
    ON_EXIT {                                                                                                     \
    FSCache_BlkRelease((p_vol)->CacheBlkDevDataPtr, *(pp_buf));                                                   \
  } WITH
#else
#define  FS_VOL_CACHE_BLK_ADD(p_vol, sec_nbr, sec_type, mode, prev_wr_job_handle, pp_buf, p_wr_job_handle, p_err) \
  if (1) {                                                                                                        \
    *(pp_buf) = FSCache_BlkAcquire((p_vol)->CacheBlkDevDataPtr,                                                   \
                                   (p_vol)->PartitionStart + sec_nbr,                                             \
                                   sec_type,                                                                      \
                                   mode,                                                                          \
                                   p_err);                                                                        \
    if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {                                                             \
      goto PP_UNIQUE_LABEL(fs_vol_cache_blk_add_enter);                                                           \
    }                                                                                                             \
  } else PP_UNIQUE_LABEL(fs_vol_cache_blk_add_enter) :                                                            \
    ON_EXIT {                                                                                                     \
    FSCache_BlkRelease((p_vol)->CacheBlkDevDataPtr, *(pp_buf));                                                   \
  } WITH
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void FSVol_ModuleInit(void);

void FSVol_ReleaseInternal(FS_VOL *p_vol);

FS_VOL_HANDLE FSVol_GetLocked(CPU_CHAR *path);

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FSVol_Trim(FS_VOL    *p_vol,
                FS_LB_NBR start,
                FS_LB_QTY cnt,
                RTOS_ERR  *p_err);

void FSVol_Fmt(FS_BLK_DEV_HANDLE blk_dev_handle,
               FS_PARTITION_NBR  partition_nbr,
               FS_SYS_TYPE       sys_type,
               void              *p_fs_cfg,
               RTOS_ERR          *p_err);
#endif

FS_OBJ_HANDLE FSVol_ToObjHandle(FS_VOL_HANDLE vol_handle);

FS_OBJ *FSVol_ToObj(FS_VOL *p_vol);

void FSVol_OnNullRefCnt(FS_OBJ *p_obj);

FS_VOL_HANDLE FSVol_FirstAcquire(void);

FS_VOL_HANDLE FSVol_NextAcquire(FS_VOL_HANDLE vol_handle);

void FSVol_Release(FS_VOL_HANDLE vol_handle);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           CONFIGURATION ERRORS
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  FS_CORE_CFG_MAX_VOL_NAME_LEN
#error  "FS_CORE_CFG_MAX_VOL_NAME_LEN not #define'd in 'fs_core_cfg.h'. MUST be >= 1."
#elif   (FS_CORE_CFG_MAX_VOL_NAME_LEN < 1u)
#error  "FS_CORE_CFG_MAX_VOL_NAME_LEN illegally #define'd in 'fs_core_cfg.h'. MUST be >= 1."
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
