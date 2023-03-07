/***************************************************************************//**
 * @file
 * @brief File System - Core Operations
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
 * @defgroup FS File System API
 * @brief      File System API
 *
 * @defgroup FS_CORE File System Core API
 * @ingroup  FS
 * @brief      File System Core API
 *
 * @addtogroup FS_CORE
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  FS_CORE_H_
#define  FS_CORE_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------------------ FS ------------------------
#include  <fs_core_cfg.h>

//                                                                 ----------------------- EXT ------------------------
#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>
#include  <common/include/rtos_types.h>
#include  <common/include/lib_mem.h>

#include  <sl_sleeptimer.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  FS_TIME_FMT              "%c"
#ifndef  FS_TIME_STR_MIN_LEN
#define  FS_TIME_STR_MIN_LEN      30u
#endif

#define  FS_TIME_TS_INVALID       ((sl_sleeptimer_timestamp_t)-1)

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef CPU_INT32U FS_FLAGS;

typedef CPU_INT16U FS_QTY;

typedef CPU_INT16U FS_PARTITION_NBR;

typedef CPU_INT32U FS_FILE_SIZE;

typedef CPU_INT16U FS_FILE_NAME_LEN;

typedef CPU_INT32U FS_CTR;

typedef CPU_INT32U FS_ID;

typedef struct fs_core_cfg_max_obj_cnt {
  CPU_SIZE_T WrkDirCnt;
  CPU_SIZE_T RootDirDescCnt;
} FS_CORE_CFG_MAX_OBJ_CNT;

typedef struct fs_core_cfg_max_fat_obj_cnt {
  CPU_SIZE_T FileDescCnt;
  CPU_SIZE_T FileNodeCnt;
  CPU_SIZE_T DirDescCnt;
  CPU_SIZE_T DirNodeCnt;
  CPU_SIZE_T VolCnt;
} FS_CORE_CFG_MAX_FAT_OBJ_CNT;

typedef struct fs_core_init_cfg {
  FS_CORE_CFG_MAX_OBJ_CNT     MaxCoreObjCnt;
  FS_CORE_CFG_MAX_FAT_OBJ_CNT MaxFatObjCnt;
  MEM_SEG                     *MemSegPtr;
} FS_CORE_INIT_CFG;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
extern const FS_CORE_INIT_CFG FSCore_InitCfgDflt;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void FSCore_ConfigureMemSeg(MEM_SEG *p_seg);

void FSCore_ConfigureMaxObjCnt(FS_CORE_CFG_MAX_OBJ_CNT max_cnt);

void FSCore_ConfigureMaxFatObjCnt(FS_CORE_CFG_MAX_FAT_OBJ_CNT max_cnt);
#endif

void FSCore_Init(RTOS_ERR *p_err);

#ifdef __cplusplus
}
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                          MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
