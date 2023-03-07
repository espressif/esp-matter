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

#ifndef  FS_BLK_DEV_PRIV_H_
#define  FS_BLK_DEV_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------------------ FS ------------------------
#include  <fs_storage_cfg.h>
#include  <fs/source/shared/cleanup/cleanup_mgmt_priv.h>
#include  <fs/include/fs_blk_dev.h>
#include  <fs/source/storage/fs_media_priv.h>
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

typedef CPU_INT16U FS_BLK_DEV_CTR;

typedef struct fs_blk_dev_api FS_BLK_DEV_API;

typedef struct fs_blk_dev_data {
  CPU_BOOLEAN     IsInit;
  CPU_INT16U      BlkDevId;
  SLIST_MEMBER    *BlkDevListHeadPtr;
  KAL_LOCK_HANDLE BlkDevListLockHandle;
} FS_BLK_DEV_DATA;

typedef struct fs_blk_dev {
  FS_OBJ_FIELDS
  const FS_BLK_DEV_API *BlkDevApiPtr;
  FS_MEDIA             *MediaPtr;
  CPU_INT16U           MediaId;
  FS_LB_QTY            LbCnt;
  CPU_INT08U           LbSizeLog2;
  KAL_LOCK_HANDLE      LockHandle;
  FS_BLK_DEV_ON_CLOSE  OnClose;
#if (FS_STORAGE_CFG_CTR_STAT_EN == DEF_ENABLED)
  CPU_INT32U           StatRdCtr;
  CPU_INT32U           StatWrCtr;
#endif
} FS_BLK_DEV;

typedef struct fs_blk_dev_info {
  FS_LB_NBR   LbCnt;
  CPU_INT08U  LbSizeLog2;
  CPU_BOOLEAN Fixed;
} FS_BLK_DEV_INFO;

struct fs_blk_dev_api {
  FS_BLK_DEV *(*Add)  (FS_MEDIA *p_media,
                       RTOS_ERR *p_err);

  void       (*Rem)  (FS_BLK_DEV *p_blk_dev,
                      RTOS_ERR   *p_err);

  void       (*Open) (FS_BLK_DEV *p_blk_dev,
                      RTOS_ERR   *p_err);

  void       (*Close)(FS_BLK_DEV *p_blk_dev,
                      RTOS_ERR   *p_err);

  void       (*Rd)   (FS_BLK_DEV *p_blk_dev,
                      void       *p_dest,
                      FS_LB_NBR  lb_nbr_start,
                      FS_LB_QTY  lb_cnt,
                      RTOS_ERR   *p_err);

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
  void (*Wr)(FS_BLK_DEV *p_blk_dev,
             void *p_src,
             FS_LB_NBR lb_nbr_start,
             FS_LB_QTY lb_cnt,
             RTOS_ERR *p_err);

  void (*Sync) (FS_BLK_DEV *p_blk_dev,
                RTOS_ERR   *p_err);

  void (*Trim) (FS_BLK_DEV *p_blk_dev,
                FS_LB_NBR  lb_nbr,
                RTOS_ERR   *p_err);
#endif

  void (*Query)(FS_BLK_DEV *p_blk_dev,
                FS_BLK_DEV_INFO *p_blk_dev_info,
                RTOS_ERR *p_err);
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

extern FS_BLK_DEV_DATA FSBlkDev_Data;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void FSBlkDev_Init(RTOS_ERR *p_err);

void FSBlkDev_OnNullRefCnt(FS_OBJ *p_obj);

FS_OBJ_HANDLE FSBlkDev_ToObjHandle(FS_BLK_DEV_HANDLE blk_dev_handle);

FS_OBJ *FSBlkDev_ToObj(FS_BLK_DEV *p_blk_dev);

FS_BLK_DEV_HANDLE FSBlkDev_FirstAcquire(void);

FS_BLK_DEV_HANDLE FSBlkDev_NextAcquire(FS_BLK_DEV_HANDLE blk_dev_handle);

void FSBlkDev_Release(FS_BLK_DEV_HANDLE blk_dev_handle);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MACRO'S
 *
 * Note(s): (1) The browsing operation is thread safe: block devices may concurrently be opened / closed
 *               from other tasks.
 ********************************************************************************************************
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_CTR_STAT_EN == DEF_ENABLED)                 // ----------------- STAT CTR MACRO'S -----------------
#define  FS_BLK_DEV_CTR_STAT_INC(stat_ctr)                  FS_CTR_INC(stat_ctr)
#define  FS_BLK_DEV_CTR_STAT_ADD(stat_ctr, val)             FS_CTR_ADD((stat_ctr), (val))
#else
#define  FS_BLK_DEV_CTR_STAT_INC(stat_ctr)
#define  FS_BLK_DEV_CTR_STAT_ADD(stat_ctr, val)
#endif

#if (FS_STORAGE_CFG_CTR_ERR_EN == DEF_ENABLED)                  // ------------------ ERR CTR MACRO'S -----------------
#define  FS_BLK_DEV_CTR_ERR_INC(err_ctr)                    FS_CTR_INC(err_ctr)
#else
#define  FS_BLK_DEV_CTR_ERR_INC(err_ctr)
#endif

#define  FS_BLK_DEV_WITH(blk_dev_handle, p_err)             FS_OBJ_WITH(blk_dev_handle, p_err, FSBlkDev, RTOS_ERR_BLK_DEV_CLOSED)

#define  FS_BLK_DEV_WITH_NO_IO(blk_dev_handle, p_err)       FS_OBJ_WITH_NO_IO(blk_dev_handle, p_err, FSBlkDev, RTOS_ERR_BLK_DEV_CLOSED)

//                                                                 Iterate through opened block devices (see Note #1).
#define  FS_BLK_DEV_FOR_EACH(cur_blk_dev_handle)            FS_OBJ_FOR_EACH(cur_blk_dev_handle, FSBlkDev, FS_BLK_DEV)

/********************************************************************************************************
 ********************************************************************************************************
 *                                           CONFIGURATION ERRORS
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  FS_STORAGE_CFG_DBG_WR_VERIFY_EN
#error  "FS_STORAGE_CFG_DBG_WR_VERIFY_EN not #define'd in 'fs_storage_cfg.h'. MUST be DEF_ENABLED or DEF_DISABLED."
#elif  ((FS_STORAGE_CFG_DBG_WR_VERIFY_EN != DEF_DISABLED) \
  && (FS_STORAGE_CFG_DBG_WR_VERIFY_EN != DEF_ENABLED))
#error  "FS_STORAGE_CFG_DBG_WR_VERIFY_EN illegally #define'd in 'fs_storage_cfg.h'. MUST be DEF_ENABLED or DEF_DISABLED."
#endif

#ifndef  FS_STORAGE_CFG_RD_ONLY_EN
#error  "FS_STORAGE_CFG_RD_ONLY_EN not #define'd in 'fs_storage_cfg.h'. MUST be DEF_ENABLED or DEF_DISABLED."
#elif  ((FS_STORAGE_CFG_RD_ONLY_EN != DEF_ENABLED) \
  && (FS_STORAGE_CFG_RD_ONLY_EN != DEF_DISABLED))
#error  "FS_STORAGE_CFG_RD_ONLY_EN illegally #define'd in 'fs_storage_cfg.h'. MUST be DEF_ENABLED or DEF_DISABLED."
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
