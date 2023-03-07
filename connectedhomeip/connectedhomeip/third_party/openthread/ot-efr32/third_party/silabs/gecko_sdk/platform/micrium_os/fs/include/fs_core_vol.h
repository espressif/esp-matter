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

/****************************************************************************************************//**
 * @addtogroup FS_CORE
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  FS_CORE_VOL_H_
#define  FS_CORE_VOL_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------------------ FS ------------------------
#include  <fs_core_cfg.h>
#include  <fs/include/fs_core.h>
#include  <fs/include/fs_blk_dev.h>
#include  <fs/include/fs_obj.h>

//                                                                 ----------------------- EXT ------------------------
#include  <common/include/rtos_err.h>
#include  <cpu/include/cpu.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 Volume access mode.
#define  FS_VOL_OPT_DFLT                  DEF_BIT_NONE
#define  FS_VOL_OPT_ACCESS_MODE_RD_ONLY   DEF_BIT_00
#define  FS_VOL_OPT_AUTO_SYNC             DEF_BIT_01

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MACROS
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 Verify whether a volume handle is null.
#define  FS_VOL_HANDLE_IS_NULL(h)                 (((h).VolPtr == DEF_NULL) && ((h).VolId == 0u))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 Null volume handle.
#define  FS_VOL_NULL                              FSVol_NullHandle

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------------ VOLUME HANDLE -------------------
typedef struct fs_vol_handle {
  struct fs_vol *VolPtr;
  FS_OBJ_ID     VolId;
} FS_VOL_HANDLE;

//                                                                 ------------------- VOLUME INFO --------------------
typedef struct fs_vol_info {
  FS_LB_QTY BadSecCnt;                                          ///< Number of bad sectors.
  FS_LB_QTY FreeSecCnt;                                         ///< Number of free sectors.
  FS_LB_QTY UsedSecCnt;                                         ///< Number of used sectors.
  FS_LB_QTY TotSecCnt;                                          ///< Total number of sectors.
  CPU_CHAR  *SysTypeStrPtr;                                     ///< String identifying file system type.
} FS_VOL_INFO;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 *
 * Note(s) : (1) The application should use the 'FS_VOL_NULL' macro instead of the 'FSVol_NullHandle'
 *               global variable.
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 Null volume handle (see Note #1).
extern const FS_VOL_HANDLE FSVol_NullHandle;

/********************************************************************************************************
 ********************************************************************************************************
   PUBLIC FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

FS_VOL_HANDLE FSVol_Open(FS_BLK_DEV_HANDLE blk_dev_handle,
                         FS_PARTITION_NBR  partition_nbr,
                         const CPU_CHAR    *vol_name,
                         FS_FLAGS          access_mode,
                         RTOS_ERR          *p_err);

void FSVol_Close(FS_VOL_HANDLE vol_handle,
                 RTOS_ERR      *p_err);

void FSVol_CloseAssociated(FS_BLK_DEV_HANDLE blk_dev_handle,
                           FS_PARTITION_NBR  partition_nbr,
                           RTOS_ERR          *p_err);

void FSVol_LabelGet(FS_VOL_HANDLE vol_handle,
                    CPU_CHAR      *p_label,
                    CPU_SIZE_T    label_size,
                    RTOS_ERR      *p_err);

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FSVol_LabelSet(FS_VOL_HANDLE  vol_handle,
                    const CPU_CHAR *p_label,
                    RTOS_ERR       *p_err);

void FSVol_Sync(FS_VOL_HANDLE vol_handle,
                RTOS_ERR      *p_err);
#endif

void FSVol_Query(FS_VOL_HANDLE vol_handle,
                 FS_VOL_INFO   *p_vol_info,
                 void          *p_sys_info,
                 RTOS_ERR      *p_err);

FS_PARTITION_NBR FSVol_PartitionNbrGet(FS_VOL_HANDLE vol_handle,
                                       RTOS_ERR      *p_err);

FS_BLK_DEV_HANDLE FSVol_BlkDevGet(FS_VOL_HANDLE vol_handle);

void FSVol_NameGet(FS_VOL_HANDLE vol_handle,
                   CPU_CHAR      *p_buf,
                   CPU_SIZE_T    buf_size,
                   RTOS_ERR      *p_err);

FS_VOL_HANDLE FSVol_Get(const CPU_CHAR *p_vol_name);

#ifdef __cplusplus
}
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                          MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
