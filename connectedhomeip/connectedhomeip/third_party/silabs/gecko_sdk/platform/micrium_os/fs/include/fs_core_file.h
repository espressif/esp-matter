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

/****************************************************************************************************//**
 * @addtogroup FS_CORE
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  FS_CORE_FILE_H_
#define  FS_CORE_FILE_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------------------ FS ------------------------
#include  <fs_core_cfg.h>
#include  <fs/include/fs_core_entry.h>

//                                                                 ----------------------- EXT ------------------------
#include  <common/include/rtos_err.h>
#include  <cpu/include/cpu.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ----------------- NULL FILE HANDLE -----------------
#define  FS_FILE_NULL                        FSFile_NullHandle

//                                                                 -------------- FILE ACCESS MODE FLAGS --------------
#define  FS_FILE_ACCESS_MODE_NONE            DEF_BIT_NONE
#define  FS_FILE_ACCESS_MODE_EXCL            DEF_BIT_00
#define  FS_FILE_ACCESS_MODE_CREATE          DEF_BIT_01
#define  FS_FILE_ACCESS_MODE_RD              DEF_BIT_02
#define  FS_FILE_ACCESS_MODE_WR              DEF_BIT_03
#define  FS_FILE_ACCESS_MODE_TRUNCATE        DEF_BIT_04
#define  FS_FILE_ACCESS_MODE_APPEND          DEF_BIT_05
#define  FS_FILE_ACCESS_MODE_RDWR           (FS_FILE_ACCESS_MODE_RD | FS_FILE_ACCESS_MODE_WR)
#define  FS_FILE_ACCESS_MODE_ALL            (FS_FILE_ACCESS_MODE_EXCL       \
                                             | FS_FILE_ACCESS_MODE_CREATE   \
                                             | FS_FILE_ACCESS_MODE_RDWR     \
                                             | FS_FILE_ACCESS_MODE_TRUNCATE \
                                             | FS_FILE_ACCESS_MODE_APPEND)

//                                                                 -------------- FILE BUFFER MODE FLAGS --------------
#if (FS_CORE_CFG_FILE_BUF_EN == DEF_ENABLED)
#define  FS_FILE_BUF_MODE_NONE               DEF_BIT_NONE
#define  FS_FILE_BUF_MODE_RD                 DEF_BIT_00
#define  FS_FILE_BUF_MODE_WR                 DEF_BIT_01
#define  FS_FILE_BUF_MODE_RD_WR             (FS_FILE_BUF_MODE_RD | FS_FILE_BUF_MODE_WR)
#endif
//                                                                 ---------------- FILE POS SET FLAGS ----------------
#define  FS_FILE_ORIGIN_START                DEF_BIT_00
#define  FS_FILE_ORIGIN_CUR                  DEF_BIT_01
#define  FS_FILE_ORIGIN_END                  DEF_BIT_02

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------------- FILE HANDLE --------------------
typedef struct fs_file_handle {
  struct fs_file_desc *FileDescPtr;
  FS_OBJ_ID           FileDescId;
} FS_FILE_HANDLE;

//                                                                 ------------------- FILE OFFSET --------------------
typedef CPU_INT32S FS_FILE_OFFSET;

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MACROS
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 Verify whether a file handle is null.
#define  FS_FILE_HANDLE_IS_NULL(h)                    (((h).FileDescPtr == DEF_NULL) && ((h).FileDescId == 0u))

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 *
 * Note(s) : (1) The application should use the 'FS_FILE_NULL' macro instead of the 'FSFile_NullHandle'
 *               global variable.
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 Null file handle (see Note #1).
extern const FS_FILE_HANDLE FSFile_NullHandle;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

FS_FILE_HANDLE FSFile_Open(FS_WRK_DIR_HANDLE wrk_dir_handle,
                           const CPU_CHAR    *path,
                           FS_FLAGS          mode,
                           RTOS_ERR          *p_err);

void FSFile_Close(FS_FILE_HANDLE file_handle,
                  RTOS_ERR       *p_err);

CPU_SIZE_T FSFile_Rd(FS_FILE_HANDLE file_handle,
                     void           *p_dest,
                     CPU_SIZE_T     size,
                     RTOS_ERR       *p_err);

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
CPU_SIZE_T FSFile_Wr(FS_FILE_HANDLE file_handle,
                     const void     *p_src,
                     CPU_SIZE_T     size,
                     RTOS_ERR       *p_err);

void FSFile_Truncate(FS_FILE_HANDLE file_handle,
                     FS_FILE_SIZE   size,
                     RTOS_ERR       *p_err);

#if (FS_CORE_CFG_FILE_COPY_EN == DEF_ENABLED)
void FSFile_Copy(FS_WRK_DIR_HANDLE src_wrk_dir_handle,
                 const CPU_CHAR    *src_path,
                 FS_WRK_DIR_HANDLE dest_wrk_dir_handle,
                 const CPU_CHAR    *dest_path,
                 CPU_BOOLEAN       excl,
                 RTOS_ERR          *p_err);
#endif
#endif

FS_FILE_SIZE FSFile_PosGet(FS_FILE_HANDLE file_handle,
                           RTOS_ERR       *p_err);

void FSFile_PosSet(FS_FILE_HANDLE file_handle,
                   FS_FILE_OFFSET offset,
                   FS_FLAGS       origin,
                   RTOS_ERR       *p_err);

void FSFile_Query(FS_FILE_HANDLE file_handle,
                  FS_ENTRY_INFO  *p_entry_info,
                  RTOS_ERR       *p_err);

void FSFile_PathGet(FS_FILE_HANDLE file_handle,
                    CPU_CHAR       *p_buf,
                    CPU_SIZE_T     buf_len,
                    RTOS_ERR       *p_err);

CPU_BOOLEAN FSFile_IsEOF(FS_FILE_HANDLE file_handle,
                         RTOS_ERR       *p_err);

#if (FS_CORE_CFG_FILE_LOCK_EN == DEF_ENABLED)
void FSFile_TryLock(FS_FILE_HANDLE file_handle,
                    RTOS_ERR       *p_err);

void FSFile_Lock(FS_FILE_HANDLE file_handle,
                 RTOS_ERR       *p_err);

void FSFile_Unlock(FS_FILE_HANDLE file_handle);
#endif

void FSFile_ErrClr(FS_FILE_HANDLE file_handle,
                   RTOS_ERR       *p_err);

CPU_BOOLEAN FSFile_IsErr(FS_FILE_HANDLE file_handle,
                         RTOS_ERR       *p_err);

#if (FS_CORE_CFG_FILE_BUF_EN == DEF_ENABLED)
void FSFile_BufAssign(FS_FILE_HANDLE file_handle,
                      void           *p_buf,
                      FS_FLAGS       mode,
                      CPU_SIZE_T     size,
                      RTOS_ERR       *p_err);

void FSFile_BufFlush(FS_FILE_HANDLE file_handle,
                     RTOS_ERR       *p_err);
#endif

FS_VOL_HANDLE FSFile_VolGet(FS_FILE_HANDLE file_handle);

#ifdef __cplusplus
}
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                        MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
