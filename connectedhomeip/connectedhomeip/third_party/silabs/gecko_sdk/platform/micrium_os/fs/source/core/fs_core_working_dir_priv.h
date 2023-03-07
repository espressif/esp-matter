/***************************************************************************//**
 * @file
 * @brief File System - Core Working Directory Operations
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

#ifndef  FS_CORE_WORKING_DIR_PRIV_H_
#define  FS_CORE_WORKING_DIR_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------------------ FS ------------------------
#include  <fs_core_cfg.h>
#include  <fs/source/shared/fs_obj_priv.h>
#include  <fs/source/core/fs_core_vol_priv.h>
#include  <fs/source/core/fs_core_entry_priv.h>
#include  <fs/source/core/fs_core_priv.h>
#include  <fs/source/core/fs_core_working_dir_priv.h>
#include  <fs/source/sys/fs_sys_priv.h>

//                                                                 ----------------------- EXT ------------------------
#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef struct fs_wrk_dir {
  FS_ENTRY_NODE_FIELDS
  CPU_INT08U Depth;
} FS_WRK_DIR;

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MACRO'S
 *
 * Note(s): (1) The browsing operation is thread safe: working directories may concurrently be opened /
 *               closed from other tasks.
 ********************************************************************************************************
 *******************************************************************************************************/
//                                                                 Iterate through opened working directories.
//                                                                 See Note #1.
#define  FS_WRK_DIR_FOR_EACH(cur_wrk_dir_handle)            FS_OBJ_FOR_EACH(cur_wrk_dir_handle, FSWrkDir, FS_WRK_DIR)

/********************************************************************************************************
 ********************************************************************************************************
 *                                       INTERNAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void FSWrkDir_Init(RTOS_ERR *p_err);

FS_WRK_DIR_HANDLE FSWrkDir_OpenFind(FS_VOL     *p_vol,
                                    FS_SYS_POS entry_pos);

FS_WRK_DIR_HANDLE FSWrkDir_FirstAcquire(void);

FS_WRK_DIR_HANDLE FSWrkDir_NextAcquire(FS_WRK_DIR_HANDLE wrk_dir_handle);

void FSWrkDir_Release(FS_WRK_DIR_HANDLE wrk_dir_handle);

/********************************************************************************************************
 ********************************************************************************************************
 *                                       CONFIGURATION ERRORS
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  FS_CORE_CFG_TASK_WORKING_DIR_EN
#error  "FS_CORE_CFG_TASK_WORKING_DIR_EN not #define'd in 'fs_core_cfg.h'. MUST be DEF_ENABLED or DEF_DISABLED."
#elif  ((FS_CORE_CFG_TASK_WORKING_DIR_EN != DEF_ENABLED) \
  && (FS_CORE_CFG_TASK_WORKING_DIR_EN != DEF_DISABLED))
#error  "FS_CORE_CFG_TASK_WORKING_DIR_EN illegally #define'd in 'fs_core_cfg.h'. MUST be DEF_ENABLED or DEF_DISABLED."
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
