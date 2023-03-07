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

/****************************************************************************************************//**
 * @addtogroup FS_CORE
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  FS_CORE_CACHE_H_
#define  FS_CORE_CACHE_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include <fs/include/fs_blk_dev.h>
#include <common/include/rtos_err.h>
#include <common/include/lib_mem.h>
#include <cpu/include/cpu.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef struct fs_cache FS_CACHE;

typedef struct fs_cache_cfg {
  CPU_SIZE_T Align;                                             ///< Buffer alignment requirement.
  CPU_SIZE_T MinBlkCnt;                                         ///< Number of buffers available to cache module.
  CPU_SIZE_T MinLbSize;                                         ///< Minimum logical block size to consider.
  CPU_SIZE_T MaxLbSize;                                         ///< Maximum logical block size to consider.
  MEM_SEG    *BlkMemSegPtr;                                     ///< Memory segment where buffers will be allocated from.
} FS_CACHE_CFG;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

FS_CACHE *FSCache_Create(const FS_CACHE_CFG *p_cache_cfg,
                         RTOS_ERR           *p_err);

void FSCache_Assign(FS_BLK_DEV_HANDLE blk_dev_handle,
                    FS_CACHE          *p_cache,
                    RTOS_ERR          *p_err);

FS_CACHE *FSCache_DfltAssign(FS_BLK_DEV_HANDLE blk_dev_handle,
                             CPU_SIZE_T        cache_blk_cnt,
                             RTOS_ERR          *p_err);

FS_CACHE *FSCache_Get(FS_BLK_DEV_HANDLE blk_dev_handle);

FS_LB_SIZE FSCache_MinBlkSizeGet(FS_CACHE *p_cache);

FS_LB_SIZE FSCache_MaxBlkSizeGet(FS_CACHE *p_cache);

#ifdef __cplusplus
}
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                          MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
