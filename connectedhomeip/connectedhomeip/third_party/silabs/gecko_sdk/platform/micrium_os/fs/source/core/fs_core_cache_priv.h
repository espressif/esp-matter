/***************************************************************************//**
 * @file
 * @brief File System - Core Cache Operations
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

#ifndef  FS_CORE_CACHE_PRIV_H_
#define  FS_CORE_CACHE_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------------------ FS ------------------------
#include  <fs_core_cfg.h>
#include  <fs/include/fs_core_cache.h>
#include  <fs/source/core/fs_core_priv.h>
#include  <fs/source/core/fs_core_list_priv.h>
#include  <fs/source/core/fs_core_job_sched_priv.h>
#include  <fs/source/core/fs_core_buddy_alloc_priv.h>
#include  <fs/source/storage/fs_blk_dev_priv.h>

//                                                                 ----------------------- EXT ------------------------
#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>
#include  <common/include/lib_mem.h>
#include  <common/source/preprocessor/preprocessor_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  FS_CACHE_BLK_GET_MODE_NONE              DEF_BIT_NONE
#define  FS_CACHE_BLK_GET_MODE_RD                DEF_BIT_00
#define  FS_CACHE_BLK_GET_MODE_WR                DEF_BIT_01
#define  FS_CACHE_BLK_GET_MODE_RW               (FS_CACHE_BLK_GET_MODE_RD | FS_CACHE_BLK_GET_MODE_WR)

#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
#define  FS_CACHE_WR_JOB_HANDLE_INIT       JOB_SCHED_JOB_HANDLE_INIT
#else
#define  FS_CACHE_WR_JOB_HANDLE_INIT       (CPU_DATA)-1
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef CPU_INT08U FS_CACHE_BLK_NBR;

typedef CPU_INT08U FS_CACHE_BLK_STATUS;

typedef void (*FS_CACHE_WR_CB) (FS_BLK_DEV_HANDLE blk_dev_handle,
                                CPU_INT08U        *p_blk,
                                FS_LB_NBR         lb_nbr,
                                RTOS_ERR          *p_err);

#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
typedef JOB_SCHED_JOB_HANDLE FS_CACHE_WR_JOB_HANDLE;
#else
typedef CPU_DATA FS_CACHE_WR_JOB_HANDLE;
#endif

typedef struct fs_cache_cfg_internal {
  CPU_SIZE_T     Align;
  CPU_SIZE_T     MinBlkCnt;
  FS_LB_SIZE     MinLbSize;
  FS_LB_SIZE     MaxLbSize;
  CPU_SIZE_T     ExtraStubWrJobCnt;
  CPU_SIZE_T     EdgeToNodeRatio;
  FS_FLAGS       Mode;
  MEM_SEG        *MetaDataMemSegPtr;
  MEM_SEG        *BlkMemSegPtr;
  FS_CACHE_WR_CB WrFunc;
} FS_CACHE_CFG_INTERNAL;

#if (FS_CACHE_DBG_EN == DEF_ENABLED)
typedef struct fs_cache_graph_node_dbg_data {
  CPU_INT32U  Id;
  FS_LB_NBR   LbNbr;
  CPU_INT08U  LbType;
  CPU_INT16U  HitCnt;
  CPU_BOOLEAN IsWr;
} FS_CACHE_GRAPH_NODE_DBG_DATA;
#endif

typedef struct fs_cache_blk_desc {
  FS_BLK_DEV_HANDLE            BlkDevHandle;
  FS_LB_NBR                    LbNbr;
  CPU_BOOLEAN                  IsDirty;
#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
  FS_CACHE_WR_JOB_HANDLE       WrJobHandle;
#endif
#if (FS_CACHE_DBG_EN == DEF_ENABLED)
  FS_CACHE_GRAPH_NODE_DBG_DATA DbgData;
#endif
} FS_CACHE_BLK_DESC;

#if (FS_CORE_CFG_CTR_STAT_EN == DEF_ENABLED)
typedef struct fs_cache_stat {
  FS_CTR SecFlushCnt;
  FS_CTR HitCnt;
  FS_CTR MissCnt;
  FS_CTR RdRwCnt;
} FS_CACHE_STAT;
#endif

typedef CPU_BOOLEAN (*FS_CACHE_FLUSH_PREDICATE) (FS_CACHE          *p_cache,
                                                 FS_CACHE_BLK_DESC *p_blk_desc,
                                                 void              *p_arg);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

extern const FS_CACHE_WR_JOB_HANDLE FSCache_VoidWrJobHandle;

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MACRO'S
 ********************************************************************************************************
 *******************************************************************************************************/

#define  FS_CACHE_BLK_WR(p_cache_blk_dev_data, lb_nbr, lb_type, prev_wr_job_handle, pp_buf, p_wr_job_handle, p_err) \
  FS_CACHE_BLK_ADD(p_cache_blk_dev_data, lb_nbr, lb_type, FS_CACHE_BLK_GET_MODE_WR, prev_wr_job_handle, pp_buf, p_wr_job_handle, p_err)

#define  FS_CACHE_BLK_RW(p_cache_blk_dev_data, lb_nbr, lb_type, prev_wr_job_handle, pp_buf, p_wr_job_handle, p_err) \
  FS_CACHE_BLK_ADD(p_cache_blk_dev_data, lb_nbr, lb_type, FS_CACHE_BLK_GET_MODE_RW, prev_wr_job_handle, pp_buf, p_wr_job_handle, p_err)

#define  FS_CACHE_BLK_RD(p_cache_blk_dev_data, lb_nbr, lb_type, pp_buf, p_err) \
  FS_CACHE_BLK_ADD(p_cache_blk_dev_data, lb_nbr, lb_type, FS_CACHE_BLK_GET_MODE_RD, JobSched_VoidJobHandle, pp_buf, DEF_NULL, p_err)

#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
#define  FS_CACHE_BLK_ADD(p_blk_dev_data, lb_nbr, lb_type, mode, prev_wr_job_handle, pp_buf, p_wr_job_handle, p_err) \
  if (1) {                                                                                                           \
    *(pp_buf) = FSCache_BlkAcquireOrdered(p_blk_dev_data,                                                            \
                                          lb_nbr,                                                                    \
                                          lb_type,                                                                   \
                                          mode,                                                                      \
                                          prev_wr_job_handle,                                                        \
                                          p_wr_job_handle,                                                           \
                                          p_err);                                                                    \
    if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {                                                                \
      goto PP_UNIQUE_LABEL(fs_dev_cache_blk_add_enter);                                                              \
    }                                                                                                                \
  } else PP_UNIQUE_LABEL(fs_dev_cache_blk_add_enter) :                                                               \
    ON_EXIT {                                                                                                        \
    FSCache_BlkRelease(p_blk_dev_data, *(pp_buf));                                                                   \
  } WITH
#else
#define  FS_CACHE_BLK_ADD(p_cache_blk_dev_data, lb_nbr, lb_type, mode, prev_wr_job_handle, pp_buf, p_wr_job_handle, p_err) \
  if (1) {                                                                                                                 \
    *(pp_buf) = FSCache_BlkAcquire(p_cache_blk_dev_data,                                                                   \
                                   lb_nbr,                                                                                 \
                                   lb_type,                                                                                \
                                   mode,                                                                                   \
                                   p_err);                                                                                 \
    if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {                                                                      \
      goto PP_UNIQUE_LABEL(fs_dev_cache_blk_add_enter);                                                                    \
    }                                                                                                                      \
  } else PP_UNIQUE_LABEL(fs_dev_cache_blk_add_enter) :                                                                     \
    ON_EXIT {                                                                                                              \
    FSCache_BlkRelease(p_cache_blk_dev_data, *(pp_buf));                                                                   \
  } WITH
#endif

//                                                                 ------------------- LOCK/UNLOCK --------------------
#define  FS_CACHE_LOCK_WITH(p_cache)                \
  if (1) {                                          \
    FSCache_Lock(p_cache);                          \
    goto PP_UNIQUE_LABEL(fs_cache_blk_with_enter);  \
  } else PP_UNIQUE_LABEL(fs_cache_blk_with_enter) : \
    ON_EXIT {                                       \
    FSCache_Unlock(p_cache);                        \
  } WITH

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

FS_CACHE_BLK_DEV_DATA *FSCache_BlkDevDataGet(FS_BLK_DEV_HANDLE blk_dev_handle);

FS_CACHE *FSCache_CreateInternal(FS_CACHE_CFG_INTERNAL *p_cache_cfg,
                                 RTOS_ERR              *p_err);

void FSCache_Flush(FS_CACHE                 *p_cache,
                   FS_CACHE_FLUSH_PREDICATE flush_pred,
                   void                     *p_flush_pred_data,
                   CPU_BOOLEAN              invalidate,
                   RTOS_ERR                 *p_err);

void FSCache_Sync(FS_CACHE          *p_cache,
                  FS_BLK_DEV_HANDLE blk_dev_handle,
                  RTOS_ERR          *p_err);

void FSCache_Invalidate(FS_CACHE          *p_cache,
                        FS_BLK_DEV_HANDLE blk_dev_handle,
                        RTOS_ERR          *p_err);

void FSCache_LbInvalidate(FS_CACHE          *p_cache,
                          FS_BLK_DEV_HANDLE blk_dev_handle,
                          FS_LB_NBR         lb_nbr,
                          RTOS_ERR          *p_err);

void FSCache_WrJobExec(FS_CACHE             *p_cache,
                       JOB_SCHED_JOB_HANDLE blk_handle,
                       CPU_BOOLEAN          invalidate,
                       RTOS_ERR             *p_err);

CPU_INT08U *FSCache_BlkAlloc(FS_CACHE   *p_cache,
                             CPU_INT08U blk_size_log2,
                             RTOS_ERR   *p_err);

void FSCache_BlkFree(FS_CACHE   *p_cache,
                     CPU_INT08U *p_buf);

FS_CACHE_WR_JOB_HANDLE FSCache_WrJobSet(FS_CACHE               *p_cache,
                                        FS_CACHE_WR_JOB_HANDLE src_handle,
                                        FS_CACHE_WR_JOB_HANDLE target_handle,
                                        FS_CACHE_BLK_DESC      *p_blk_desc,
                                        RTOS_ERR               *p_err);

FS_CACHE_WR_JOB_HANDLE FSCache_WrJobJoin(FS_CACHE               *p_cache,
                                         FS_CACHE_WR_JOB_HANDLE prev_wr_job_handle,
                                         FS_CACHE_WR_JOB_HANDLE wr_job_handle,
                                         RTOS_ERR               *p_err);

FS_CACHE_WR_JOB_HANDLE FSCache_WrJobAppend(FS_CACHE               *p_cache,
                                           FS_CACHE_WR_JOB_HANDLE prev_wr_job_handle,
                                           RTOS_ERR               *p_err);

CPU_BOOLEAN FSCache_StubWrJobRem(FS_CACHE               *p_cache,
                                 FS_CACHE_WR_JOB_HANDLE stub_job_handle);

#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
CPU_INT08U *FSCache_BlkAcquireOrdered(FS_CACHE_BLK_DEV_DATA  *p_blk_dev_data,
                                      FS_LB_NBR              lb_nbr,
                                      CPU_INT08U             lb_type,
                                      CPU_INT08U             mode,
                                      FS_CACHE_WR_JOB_HANDLE prev_wr_job_handle,
                                      FS_CACHE_WR_JOB_HANDLE *p_wr_job_handle,
                                      RTOS_ERR               *p_err);
#else
CPU_INT08U *FSCache_BlkAcquire(FS_CACHE_BLK_DEV_DATA *p_blk_dev_data,
                               FS_LB_NBR             lb_nbr,
                               CPU_INT08U            lb_type,
                               CPU_INT08U            mode,
                               RTOS_ERR              *p_err);
#endif

void FSCache_BlkRelease(FS_CACHE_BLK_DEV_DATA *p_blk_dev_data,
                        CPU_INT08U            *p_blk);

void FSCache_Lock(FS_CACHE *p_cache);

void FSCache_Unlock(FS_CACHE *p_cache);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
