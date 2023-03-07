/***************************************************************************//**
 * @file
 * @brief File System - Core File Operations
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

#ifndef  FS_CORE_FILE_PRIV_H_
#define  FS_CORE_FILE_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------------------ FS ------------------------
#include  <fs_core_cfg.h>
#include  <fs/include/fs_core_file.h>
#include  <fs/source/core/fs_core_entry_priv.h>
#include  <fs/source/core/fs_core_cache_priv.h>

//                                                                 ----------------------- EXT ------------------------
#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 Size/offset max values.
#define  FS_FILE_OFFSET_MIN                                  DEF_INT_32S_MIN_VAL
#define  FS_FILE_OFFSET_MAX                                  DEF_INT_32S_MAX_VAL
#define  FS_FILE_SIZE_MAX                                    DEF_INT_32U_MAX_VAL

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef CPU_INT16U FS_STATE;

//                                                                 File descriptor file buffer data.
typedef struct fs_file_desc_buf_data {
  void       *BufPtr;
  CPU_INT16U WrSeqNo;
  CPU_SIZE_T Pos;
  CPU_SIZE_T Size;
} FS_FILE_DESC_BUF_DATA;

//                                                                 File node file buffer data.
typedef struct fs_file_node_buf_data {
  CPU_INT16U WrSeqNo;
} FS_FILE_NODE_BUF_DATA;

//                                                                 File node.
typedef struct fs_file_node {
  FS_ENTRY_NODE_FIELDS
  FS_SYS_POS            ParentPosData;
  FS_FILE_SIZE          Size;
#if (FS_CORE_CFG_FILE_BUF_EN == DEF_ENABLED)
  FS_FILE_NODE_BUF_DATA BufData;
#endif
} FS_FILE_NODE;

//                                                                 File descriptor.
typedef struct fs_file_desc {
  FS_ENTRY_DESC_FIELDS(FS_FILE_NODE  * FileNodePtr)
  FS_FILE_SIZE Pos;
#if (FS_CORE_CFG_FILE_LOCK_EN == DEF_ENABLED)
  KAL_LOCK_HANDLE       FileLockHandle;
#endif
#if (FS_CORE_CFG_FILE_BUF_EN == DEF_ENABLED)
  FS_FILE_DESC_BUF_DATA BufData;
  CPU_INT32U            BufModeFlags : 2;
  CPU_INT32U            BufStatus : 2;
#endif
  CPU_INT32U            IsEOF : 1;
  CPU_INT32U            IsErr : 1;
  CPU_INT32U            IOStateFlags : 2;
  CPU_INT32U            RdOnly : 1;
  CPU_INT32U            Append : 1;
} FS_FILE_DESC;

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MACRO'S
 *
 * Note(s): (1) The browsing operation is thread safe: files may concurrently be opened / closed from other
 *               tasks.
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 Iterate through opened files (see Note #1).
#define  FS_FILE_FOR_EACH(file_handle)                 FS_OBJ_FOR_EACH(file_handle, FSFile, FS_FILE)

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FONCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (FS_CORE_CFG_FILE_COPY_EN == DEF_ENABLED)
void FSFile_CopyLockedAcquired(FS_ENTRY_LOC_DATA *p_src_entry_loc_data,
                               FS_ENTRY_LOC_DATA *p_dest_entry_loc_data,
                               CPU_BOOLEAN       excl,
                               RTOS_ERR          *p_err);
#endif

FS_FILE_HANDLE FSFile_FirstAcquire(void);

FS_FILE_HANDLE FSFile_NextAcquire(FS_FILE_HANDLE file_handle);

void FSFile_Release(FS_FILE_HANDLE file_handle);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           CONFIGURATION ERRORS
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  FS_CORE_CFG_FILE_BUF_EN
#error  "FS_CORE_CFG_FILE_BUF_EN not #define'd in 'fs_core_cfg.h'. MUST be DEF_ENABLED or DEF_DISABLED."
#elif  ((FS_CORE_CFG_FILE_BUF_EN != DEF_ENABLED) \
  && (FS_CORE_CFG_FILE_BUF_EN != DEF_DISABLED))
#error  "FS_CORE_CFG_FILE_BUF_EN illegally #define'd in 'fs_core_cfg.h'. MUST be DEF_ENABLED or DEF_DISABLED."
#endif

#ifndef  FS_CORE_CFG_FILE_LOCK_EN
#error  "FS_CORE_CFG_FILE_LOCK_EN not #define'd in 'fs_core_cfg.h'. MUST be DEF_ENABLED or DEF_DISABLED."
#elif  ((FS_CORE_CFG_FILE_LOCK_EN != DEF_ENABLED) \
  && (FS_CORE_CFG_FILE_LOCK_EN != DEF_DISABLED))
#error  "FS_CORE_CFG_FILE_LOCK_EN not #define'd in 'fs_core_cfg.h'. MUST be DEF_ENABLED or DEF_DISABLED."
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
