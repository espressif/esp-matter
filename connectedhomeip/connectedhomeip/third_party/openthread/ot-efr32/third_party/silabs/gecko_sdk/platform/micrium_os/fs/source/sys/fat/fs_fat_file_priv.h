/***************************************************************************//**
 * @file
 * @brief File System - Fat File Operations
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

#ifndef  FS_FAT_FILE_PRIV_H_
#define  FS_FAT_FILE_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ----------------------- CORE -----------------------
#include  <fs/source/core/fs_core_file_priv.h>
#include  <fs/source/shared/fs_utils_priv.h>
#include  <common/include/rtos_err.h>

//                                                                 ----------------------- FAT ------------------------
#include  <fs/source/sys/fat/fs_fat_entry_priv.h>

//                                                                 ----------------------- EXT ------------------------
#include  <cpu/include/cpu.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  FS_FAT_FILE_ALLOC_MODE_NONE       DEF_BIT_NONE
#define  FS_FAT_FILE_ALLOC_MODE_EXTEND     DEF_BIT_00
#define  FS_FAT_FILE_ALLOC_MODE_KEEP_SIZE  DEF_BIT_01

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef struct fs_fat_file_desc {
  FS_FILE_DESC    FileDesc;
  FS_FAT_CLUS_NBR CurClusIx;
  FS_FAT_CLUS_NBR CurClus;
  CPU_INT32U      ShrinkSeqNo;
} FS_FAT_FILE_DESC;

typedef struct fs_fat_file_node {
  FS_FILE_NODE           FileNode;
  FS_FAT_CLUS_NBR        FirstClusNbr;
  FS_FAT_SEC_BYTE_POS    EntryEndPos;
  CPU_INT32U             ShrinkSeqNo;
#if (FS_CORE_CFG_ORDERED_WR_EN == DEF_ENABLED)
  FS_CACHE_WR_JOB_HANDLE EntryWrJobHandle;
  FS_CACHE_WR_JOB_HANDLE DataWrJobHandle;
#endif
} FS_FAT_FILE_NODE;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void FS_FAT_FileModuleInit(RTOS_ERR *p_err);

FS_FILE_NODE *FS_FAT_FileNodeAlloc(RTOS_ERR *p_err);

void FS_FAT_FileNodeFree(FS_FILE_NODE *p_file_node);

FS_FILE_DESC *FS_FAT_FileDescAlloc(RTOS_ERR *p_err);

void FS_FAT_FileDescFree(FS_FILE_DESC *p_fat_file_desc);

void FS_FAT_FileNodeInit(FS_FILE_NODE *p_dir_node,
                         FS_SYS_POS   entry_sys_pos,
                         RTOS_ERR     *p_err);

void FS_FAT_FileDescInit(FS_FILE_DESC *p_file_desc,
                         RTOS_ERR     *p_err);

/********************************************************************************************************
 *                                   SYSTEM DRIVER FUNCTION PROTOTYPES
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
void FS_FAT_FileTruncate(FS_VOL       *p_vol,
                         FS_FILE_NODE *p_file_node,
                         FS_SYS_POS   entry_pos_data,
                         FS_FILE_SIZE new_size,
                         RTOS_ERR     *p_err);

void FS_FAT_FileExtend(FS_VOL       *p_vol,
                       FS_SYS_POS   entry_pos_data,
                       FS_FILE_SIZE new_size,
                       RTOS_ERR     *p_err);
#endif

CPU_SIZE_T FS_FAT_FileRd(FS_FILE_DESC *p_file_desc,
                         CPU_SIZE_T   pos,
                         CPU_INT08U   *p_dest,
                         CPU_SIZE_T   size,
                         RTOS_ERR     *p_err);

CPU_SIZE_T FS_FAT_FileWr(FS_FILE_DESC *p_file_desc,
                         CPU_SIZE_T   pos,
                         CPU_INT08U   *p_src,
                         CPU_SIZE_T   size,
                         RTOS_ERR     *p_err);

void FS_FAT_FileSync(FS_FILE_NODE *p_file_node,
                     RTOS_ERR     *p_err);

FS_FAT_CLUS_NBR FS_FAT_File_ClusQtyGet(FS_FILE_HANDLE file_handle,
                                       RTOS_ERR       *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
