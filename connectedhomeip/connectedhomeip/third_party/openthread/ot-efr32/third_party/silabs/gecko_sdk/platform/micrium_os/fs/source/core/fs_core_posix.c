/***************************************************************************//**
 * @file
 * @brief File System - Core Posix Operations
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
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include <rtos_description.h>

#if (defined(RTOS_MODULE_FS_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------------------ FS ------------------------
#include  <fs_core_cfg.h>

#if (FS_CORE_CFG_POSIX_EN == DEF_ENABLED)

#include  <fs/include/fs_core_posix.h>
#include  <fs/include/fs_core_dir.h>
#include  <fs/include/fs_core_file.h>
#include  <fs/include/fs_core_working_dir.h>

#include  <fs/source/core/fs_core_file_priv.h>
#include  <fs/source/core/fs_core_dir_priv.h>
#include  <fs/source/core/fs_core_priv.h>
#include  <fs/source/core/fs_core_posix_priv.h>

#include  <fs/source/shared/cleanup/cleanup_mgmt_priv.h>

//                                                                 ----------------------- EXT ------------------------
#include  <common/include/kal.h>
#include  <common/include/rtos_err.h>
#include  <common/include/lib_utils.h>

#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/logging/logging_priv.h>

#include  <common/include/kal.h>

#include  <stdint.h>
#include  <stddef.h>
#include  <stdbool.h>
#include  "sl_sleeptimer.h"

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  RTOS_MODULE_CUR    RTOS_CFG_MODULE_FS
#define  LOG_DFLT_CH        (FS, CORE, POSIX)

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static KAL_TASK_REG_HANDLE FSPosix_ErrnoTaskRegHandle;

static MEM_DYN_POOL FSPosix_FileHandlePool;
#if (FS_CORE_CFG_DIR_EN == DEF_ENABLED)
static MEM_DYN_POOL FSPosix_DirHandlePool;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static int FSPosix_ErrConv(RTOS_ERR_CODE err_code);

static void FSPosix_ErrSet(RTOS_ERR err);

static RTOS_ERR_CODE FSPosix_ErrGet(void);

static FS_FLAGS FSPosix_FileModeParse(CPU_CHAR   *str_mode,
                                      CPU_SIZE_T str_len);

#ifdef FS_CORE_CFG_POSIX_PUTCHAR
static int FSPosix_PutCharWrapper(int  c,
                                  void *p_arg);
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL CONFIGURATION ERRORS
 ********************************************************************************************************
 *******************************************************************************************************/

#if (CPU_CFG_DATA_SIZE < CPU_CFG_ADDR_SIZE)
#error "CPU data size less than CPU address size. Posix function fileno() not usable."
#endif

/********************************************************************************************************
 ********************************************************************************************************
   GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       WORKING DIRECTORY FUNCTIONS
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               fs_chdir()
 *
 * @brief    Set the working directory for the current task.
 *
 * @param    path_dir    String that specifies EITHER the absolute working directory path to set OR
 *                       a relative path that will be applied to the current working directory.
 *
 * @return    0, if no error occurs.
 *           -1, otherwise.
 *******************************************************************************************************/

#if (FS_CORE_CFG_TASK_WORKING_DIR_EN == DEF_ENABLED)
int fs_chdir(const char *path_dir)
{
  FS_WRK_DIR_HANDLE cur_wrk_dir_handle;
  RTOS_ERR          err;

  RTOS_ASSERT_DBG((path_dir != DEF_NULL), RTOS_ERR_ASSERT_DBG_FAIL, -1);

  RTOS_ERR_SET(err, RTOS_ERR_NONE);

  cur_wrk_dir_handle = FS_CHAR_IS_PATH_SEP(*path_dir) ? FSWrkDir_NullHandle
                       : FSWrkDir_Get();

  FSWrkDir_TaskBind(cur_wrk_dir_handle, (CPU_CHAR *)path_dir, &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    FSPosix_ErrSet(err);
    return (-1);
  }

  return (0);
}
#endif

/****************************************************************************************************//**
 *                                               fs_getcwd()
 *
 * @brief    Get the working directory for the current task.
 *
 * @param    path_dir    String buffer that will receive the working directory path.
 *
 * @param    size        Size of string buffer.
 *
 * @return   Pointer to working directory path, if no error occurs.
 *           Pointer to NULL,                   otherwise.
 *******************************************************************************************************/

#if (FS_CORE_CFG_TASK_WORKING_DIR_EN == DEF_ENABLED)
char *fs_getcwd(char      *path_dir,
                fs_size_t size)
{
  FS_WRK_DIR_HANDLE wrk_dir_handle;
  RTOS_ERR          err;

  RTOS_ASSERT_DBG((path_dir != DEF_NULL), RTOS_ERR_ASSERT_DBG_FAIL, DEF_NULL);

  RTOS_ERR_SET(err, RTOS_ERR_NONE);

  wrk_dir_handle = FSWrkDir_Get();
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    FSPosix_ErrSet(err);
    return (DEF_NULL);
  }

  FSWrkDir_PathGet(wrk_dir_handle, path_dir, size, &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    FSPosix_ErrSet(err);
    return (DEF_NULL);
  }

  return (path_dir);
}
#endif

/********************************************************************************************************
 *                                           DIRECTORY FUNCTIONS
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               fs_opendir()
 *
 * @brief    Open a directory.
 *
 * @param    name_full   Name of the directory.
 *
 * @return   Pointer to a directory, if NO errors.
 *           Pointer to NULL,        otherwise.
 *******************************************************************************************************/

#if (FS_CORE_CFG_DIR_EN == DEF_ENABLED)
FS_DIR *fs_opendir(const char *name_full)
{
  FS_DIR_HANDLE     *p_dir_handle;
  FS_WRK_DIR_HANDLE cur_wrk_dir_handle;
  RTOS_ERR          err;

  RTOS_ASSERT_DBG((name_full != DEF_NULL), RTOS_ERR_ASSERT_DBG_FAIL, DEF_NULL);

  p_dir_handle = (FS_DIR_HANDLE *)Mem_DynPoolBlkGet(&FSPosix_DirHandlePool, &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

#if (FS_CORE_CFG_TASK_WORKING_DIR_EN == DEF_ENABLED)
  cur_wrk_dir_handle = FS_CHAR_IS_PATH_SEP(*name_full) ? FSWrkDir_NullHandle
                       : FSWrkDir_Get();
#else
  cur_wrk_dir_handle = FSWrkDir_NullHandle;
#endif

  *p_dir_handle = FSDir_Open(cur_wrk_dir_handle,
                             name_full,
                             FS_DIR_ACCESS_MODE_NONE,
                             &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    RTOS_ERR err_tmp;
    RTOS_ERR_SET(err_tmp, RTOS_ERR_NONE);
    Mem_DynPoolBlkFree(&FSPosix_DirHandlePool,
                       p_dir_handle,
                       &err_tmp);
    RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(err_tmp) == RTOS_ERR_NONE,
                         RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_NULL);
    FSPosix_ErrSet(err);
    return (DEF_NULL);
  }

  return (p_dir_handle);
}
#endif

/****************************************************************************************************//**
 *                                               fs_closedir()
 *
 * @brief    Close and free a directory.
 *
 * @param    p_dir   Pointer to a directory.
 *
 * @return    0, if directory is successfully closed.
 *           -1, if any error was encountered.
 *
 * @note     (1) After a directory is closed, the application MUST cease from accessing its directory
 *               pointer.  This could cause file system corruption, since this handle may be re-used
 *               for a different directory.
 *******************************************************************************************************/

#if (FS_CORE_CFG_DIR_EN == DEF_ENABLED)
int fs_closedir(FS_DIR *p_dir)
{
  RTOS_ERR err;

  RTOS_ASSERT_DBG((p_dir != DEF_NULL), RTOS_ERR_ASSERT_DBG_FAIL, -1);

  FSDir_Close(*p_dir, &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    FSPosix_ErrSet(err);
    return (-1);
  }

  Mem_DynPoolBlkFree(&FSPosix_DirHandlePool,
                     (void *)p_dir,
                     &err);
  RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL, -1);

  return (0);
}
#endif

/****************************************************************************************************//**
 *                                               fs_readdir_r()
 *
 * @brief    Read a directory entry from a directory.
 *
 * @param    p_dir           Pointer to a directory.
 *
 * @param    p_dir_entry     Pointer to variable that will receive directory entry information.
 *
 * @param    pp_result       Pointer to variable that will receive :
 *                           (a) ... 'p_dir_entry'    if NO error occurs AND directory does not encounter  EOF.
 *                           (b) ...  pointer to NULL if an error occurs OR  directory          encounters EOF.
 *
 * @return   1, if an error occurs.
 *           0, if no error occurs.
 *
 * @note     (1) IEEE Std 1003.1, 2004 Edition, Section 'readdir() : DESCRIPTION' states that :
 *               - (a) "The 'readdir()' function shall not return directory entries containing empty names.
 *                       If entries for dot or dot-dot exist, one entry shall be returned for dot and one
 *                       entry shall be returned for dot-dot; otherwise, they shall not be returned."
 *               - (b) "If a file is removed from or added to the directory after the most recent call
 *                       to 'opendir()' or 'rewinddir()', whether a subsequent call to 'readdir()' returns
 *                       an entry for that file is unspecified."
 *
 * @note     (2) IEEE Std 1003.1, 2004 Edition, Section 'readdir() : RETURN VALUE' states that "[i]f
 *               successful, the 'readdir_r()' function shall return zero; otherwise, an error shall
 *               be returned to indicate the error".
 *******************************************************************************************************/
#if (FS_CORE_CFG_DIR_EN == DEF_ENABLED)
int fs_readdir_r(FS_DIR           *p_dir,
                 struct fs_dirent *p_dir_entry,
                 struct fs_dirent **pp_result)
{
  CPU_BOOLEAN eof;
  RTOS_ERR    err;

  RTOS_ASSERT_DBG((p_dir != DEF_NULL), RTOS_ERR_ASSERT_DBG_FAIL, 1);
  RTOS_ASSERT_DBG((p_dir_entry != DEF_NULL), RTOS_ERR_ASSERT_DBG_FAIL, 1);
  RTOS_ASSERT_DBG((pp_result != DEF_NULL), RTOS_ERR_ASSERT_DBG_FAIL, 1);

  eof = FSDir_Rd(*p_dir,
                 DEF_NULL,
                 p_dir_entry->d_name,
                 FILENAME_MAX,
                 &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    *pp_result = DEF_NULL;
    FSPosix_ErrSet(err);
    return (1);
  }

  *pp_result = eof ? DEF_NULL : p_dir_entry;

  return (0);
}
#endif

/********************************************************************************************************
 *                                           ENTRY FUNCTIONS
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               fs_mkdir()
 *
 * @brief    Create a directory.
 *
 * @param    name_full   Name of the directory.
 *
 * @return   -1, if an error occurs.
 *           0, if no error occurs.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
int fs_mkdir(const char *name_full)
{
  FS_WRK_DIR_HANDLE cur_wrk_dir_handle;
  RTOS_ERR          err;

  RTOS_ASSERT_DBG((name_full != DEF_NULL), RTOS_ERR_ASSERT_DBG_FAIL, -1);

  //                                                               Handle absolute vs relative path.
#if (FS_CORE_CFG_TASK_WORKING_DIR_EN == DEF_ENABLED)
  cur_wrk_dir_handle = FS_CHAR_IS_PATH_SEP(*name_full) ? FSWrkDir_NullHandle
                       : FSWrkDir_Get();
#else
  cur_wrk_dir_handle = FSWrkDir_NullHandle;
#endif

  FSEntry_Create(cur_wrk_dir_handle,
                 name_full,
                 FS_ENTRY_TYPE_DIR,
                 DEF_YES,
                 &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    FSPosix_ErrSet(err);
    return (-1);
  }

  return (0);
}
#endif

/****************************************************************************************************//**
 *                                               fs_remove()
 *
 * @brief    Delete a file or directory.
 *
 * @param    name_full   Name of the entry.
 *
 * @return    0, if the entry is     removed.
 *           -1, if the entry is NOT removed.
 *
 * @note     (1) IEEE Std 1003.1, 2004 Edition, Section 'remove() : DESCRIPTION' states that :
 *               - (a) "If 'path' does not name a directory, 'remove(path)' shall be equivalent to
 *                     'unlink(path)'."
 *               - (b) "If path names a directory, remove(path) shall be equivalent to rmdir(path)."
 *                   - (1) See 'fs_rmdir()  Note(s)'.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
int fs_remove(const char *name_full)
{
  FS_WRK_DIR_HANDLE cur_wrk_dir_handle;
  RTOS_ERR          err;

  RTOS_ASSERT_DBG((name_full != DEF_NULL), RTOS_ERR_ASSERT_DBG_FAIL, -1);

  //                                                               Handle absolute vs relative path.
#if (FS_CORE_CFG_TASK_WORKING_DIR_EN == DEF_ENABLED)
  cur_wrk_dir_handle = FS_CHAR_IS_PATH_SEP(*name_full) ? FSWrkDir_NullHandle
                       : FSWrkDir_Get();
#else
  cur_wrk_dir_handle = FSWrkDir_NullHandle;
#endif

  FSEntry_Del(cur_wrk_dir_handle,
              name_full,
              FS_ENTRY_TYPE_ANY,
              &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    FSPosix_ErrSet(err);
    return (-1);
  }

  return (0);
}
#endif

/****************************************************************************************************//**
 *                                               fs_rename()
 *
 * @brief    Rename a file or directory.
 *
 * @param    name_full_old   Old path of the entry.
 *
 * @param    name_full_new   New path of the entry.
 *
 * @return    0, if the entry is     renamed.
 *           -1, if the entry is NOT renamed.
 *
 * @note     (1) IEEE Std 1003.1, 2004 Edition, Section 'rename() : DESCRIPTION' states that :
 *               - (a) "If the 'old' argument and the 'new' argument resolve to the same existing file,
 *                     'rename()' shall return successfully and perform no other action."
 *               - (b) "If the 'old' argument points to the pathname of a file that is not a directory,
 *                     the 'new' argument shall not point to the pathname of a directory.  If the link
 *                     named by the 'new' argument exists, it shall be removed and 'old' renamed to
 *                     'new'."
 *               - (c) "If the 'old' argument points to the pathname of a directory, the 'new' argument
 *                     shall not point to the pathname of a file that is not a directory.  If the
 *                     directory named by the 'new' argument exists, it shall be removed and 'old'
 *                     renamed to 'new'."
 *                   - (1) "If 'new' names an existing directory, it shall be required to be an empty
 *                         directory."
 *               - (d) "The 'new' pathname shall not contain a path prefix that names 'old'."
 *
 * @note     (2) IEEE Std 1003.1, 2004 Edition, Section 'rename() : RETURN VALUE' states that "[u]pon
 *               successful completion, 'rename()' shall return 0; otherwise, -1 shall be returned".
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
int fs_rename(const char *name_full_old,
              const char *name_full_new)
{
  FS_WRK_DIR_HANDLE wrk_dir_handle_old;
  FS_WRK_DIR_HANDLE wrk_dir_handle_new;
  RTOS_ERR          err;

  RTOS_ASSERT_DBG((name_full_old != DEF_NULL), RTOS_ERR_ASSERT_DBG_FAIL, -1);
  RTOS_ASSERT_DBG((name_full_new != DEF_NULL), RTOS_ERR_ASSERT_DBG_FAIL, -1);

  //                                                               Handle absolute vs relative path.
#if (FS_CORE_CFG_TASK_WORKING_DIR_EN == DEF_ENABLED)
  wrk_dir_handle_old = FS_CHAR_IS_PATH_SEP(*name_full_old) ? FSWrkDir_NullHandle
                       : FSWrkDir_Get();

  wrk_dir_handle_new = FS_CHAR_IS_PATH_SEP(*name_full_new) ? FSWrkDir_NullHandle
                       : FSWrkDir_Get();
#else
  wrk_dir_handle_old = FSWrkDir_NullHandle;
  wrk_dir_handle_new = FSWrkDir_NullHandle;
#endif

  FSEntry_Rename(wrk_dir_handle_old,
                 name_full_old,
                 wrk_dir_handle_new,
                 name_full_new,
                 DEF_NO,
                 &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    FSPosix_ErrSet(err);
    return (-1);
  }

  return (0);
}
#endif

/****************************************************************************************************//**
 *                                               fs_rmdir()
 *
 * @brief    Delete a directory.
 *
 * @param    name_full   Name of the directory.
 *
 * @return    0, if the directory is     removed.
 *           -1, if the directory is NOT removed.
 *
 * @note     (1) IEEE Std 1003.1, 2004 Edition, Section 'rmdir() : DESCRIPTION' states that :
 *               - (a) "The 'rmdir()' function shall remove a directory whose name is given by path. The
 *                       directory shall be removed only if it is an empty directory."
 *               - (b) "If the directory is the root directory or the current working directory of any
 *                       process, it is unspecified whether the function succeeds, or whether it shall fail"
 *
 * @note     (2) IEEE Std 1003.1, 2004 Edition, Section 'rmdir() : RETURN VALUE' states that "[u]pon
 *               successful completion, the function 'rmdir()' shall return 0.  Otherwise, -1 shall
 *               be returned".
 *
 * @note     (3) The root directory CANNOT be removed.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
int fs_rmdir(const char *name_full)
{
  FS_WRK_DIR_HANDLE cur_wrk_dir_handle;
  RTOS_ERR          err;

  RTOS_ASSERT_DBG((name_full != DEF_NULL), RTOS_ERR_ASSERT_DBG_FAIL, -1);

  //                                                               Handle absolute vs relative path.
#if (FS_CORE_CFG_TASK_WORKING_DIR_EN == DEF_ENABLED)
  cur_wrk_dir_handle = FS_CHAR_IS_PATH_SEP(*name_full) ? FSWrkDir_NullHandle
                       : FSWrkDir_Get();
#else
  cur_wrk_dir_handle = FSWrkDir_NullHandle;
#endif

  FSEntry_Del(cur_wrk_dir_handle,
              name_full,
              FS_ENTRY_TYPE_DIR,
              &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    FSPosix_ErrSet(err);
    return (-1);
  }

  return (0);
}
#endif

/****************************************************************************************************//**
 *                                                   fs_stat()
 *
 * @brief    Get information about a file or directory.
 *
 * @param    name_full   Name of the entry.
 *
 * @param    p_info      Pointer to structure that will receive the entry information.
 *
 * @return    0, if the function succeeds.
 *           -1, otherwise.
 *******************************************************************************************************/
int fs_stat(const char     *name_full,
            struct fs_stat *p_info)
{
  FS_ENTRY_INFO     info;
  fs_mode_t         mode;
  FS_WRK_DIR_HANDLE cur_wrk_dir_handle;
  RTOS_ERR          err;

  RTOS_ASSERT_DBG((name_full != DEF_NULL), RTOS_ERR_ASSERT_DBG_FAIL, -1);
  RTOS_ASSERT_DBG((p_info != DEF_NULL), RTOS_ERR_ASSERT_DBG_FAIL, -1);

  //                                                               Handle absolute vs relative path.
#if (FS_CORE_CFG_TASK_WORKING_DIR_EN == DEF_ENABLED)
  cur_wrk_dir_handle = FS_CHAR_IS_PATH_SEP(*name_full) ? FSWrkDir_NullHandle
                       : FSWrkDir_Get();
#else
  cur_wrk_dir_handle = FSWrkDir_NullHandle;
#endif

  FSEntry_Query(cur_wrk_dir_handle,
                name_full,
                &info,
                &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    FSPosix_ErrSet(err);
    return (-1);
  }

  mode = info.Attrib.IsDir ? S_IFDIR : S_IFREG;

  if (DEF_BIT_IS_SET(info.Attrib.Rd, FS_ENTRY_ATTRIB_RD) == DEF_YES) {
    mode |= S_IRUSR | S_IRGRP | S_IROTH;
  }

  if (DEF_BIT_IS_SET(info.Attrib.Wr, FS_ENTRY_ATTRIB_WR) == DEF_YES) {
    mode |= S_IWUSR | S_IWGRP | S_IWOTH;
  }

  p_info->st_dev = 0u;
  p_info->st_ino = 0u;
  p_info->st_mode = mode;
  p_info->st_nlink = 1u;
  p_info->st_uid = 0u;
  p_info->st_gid = 0u;
  p_info->st_size = (fs_off_t) info.Size;
  p_info->st_atime = (fs_time_t)-1;
  p_info->st_ctime = (fs_time_t) info.DateTimeCreate;
  p_info->st_mtime = (fs_time_t) info.DateTimeWr;
  p_info->st_blksize = (fs_blksize_t) info.BlkSize;
  p_info->st_blocks = (fs_blkcnt_t) info.BlkCnt;

  return (0);
}

/********************************************************************************************************
 *                                           FILE FUNCTIONS
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               fs_fopen()
 *
 * @brief    Open a file.
 *
 * @param    name_full   Name of the file.
 *
 * @param    str_mode    Access mode of the file (see Note #1a).
 *
 * @return   Pointer to a file, if NO errors.
 *           Pointer to NULL,   otherwise.
 *
 * @note     (1) IEEE Std 1003.1, 2004 Edition, Section 'fopen() : DESCRIPTION' states that :
 *               - (a) "If ['str_mode'] is one of the following, the file is open in the indicated mode." :
 *                   - "r or rb           Open file for reading."
 *                   - "w or wb           Truncate to zero length or create file for writing."
 *                   - "a or ab           Append; open and create file for writing at end-of-file."
 *                   - "r+ or rb+ or r+b  Open file for update (reading and writing)."
 *                   - "w+ or wb+ or w+b  Truncate to zero length or create file for update."
 *                   - "a+ or ab+ or a+b  Append; open or create for update, writing at end-of-file."
 *               - (b) "The character 'b' shall have no effect"
 *               - (c) "Opening a file with read mode ... shall fail if the file does not exist or
 *                       cannot be read"
 *               - (d) "Opening a file with append mode ... shall cause all subsequent writes to the
 *                       file to be forced to the then current end-of-file"
 *               - (e) "When a file is opened with update mode ... both input and output may be performed....
 *                       However, the application shall ensure that output is not directly followed by
 *                       input without an intervening call to 'fflush()' or to a file positioning function
 *                       ('fseek()', 'fsetpos()', or 'rewind()'), and input is not directly followed by output
 *                       without an intervening call to a file positioning function, unless the input
 *                       operation encounters end-of-file."
 *
 * @note     (2) IEEE Std 1003.1, 2004 Edition, Section 'fopen() : RETURN VALUE' states that "[u]pon
 *               successful completion 'fopen()' shall return a pointer to the object controlling the
 *               stream.  Otherwise a null pointer shall be returned'.
 *******************************************************************************************************/
FS_FILE *fs_fopen(const char *name_full,
                  const char *str_mode)
{
  FS_FILE_HANDLE    *p_file_handle;
  FS_WRK_DIR_HANDLE cur_wrk_dir_handle;
  FS_FLAGS          mode_flags;
  CPU_SIZE_T        len;
  RTOS_ERR          err = RTOS_ERR_INIT_CODE(RTOS_ERR_NONE);

  RTOS_ASSERT_DBG((name_full != DEF_NULL), RTOS_ERR_ASSERT_DBG_FAIL, DEF_NULL);

  len = Str_Len_N((char *)str_mode, 4u);

  RTOS_ASSERT_DBG((len <= 3u), RTOS_ERR_ASSERT_DBG_FAIL, DEF_NULL);
  RTOS_ASSERT_DBG((len > 1u), RTOS_ERR_ASSERT_DBG_FAIL, DEF_NULL);

  mode_flags = FSPosix_FileModeParse((CPU_CHAR *)str_mode, len);
  RTOS_ASSERT_DBG((mode_flags != FS_FILE_ACCESS_MODE_NONE), RTOS_ERR_ASSERT_DBG_FAIL, DEF_NULL);

  p_file_handle = (FS_FILE_HANDLE *)Mem_DynPoolBlkGet(&FSPosix_FileHandlePool, &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    FSPosix_ErrSet(err);
    return (DEF_NULL);
  }

  //                                                               Handle absolute vs relative path.
#if (FS_CORE_CFG_TASK_WORKING_DIR_EN == DEF_ENABLED)
  cur_wrk_dir_handle = FS_CHAR_IS_PATH_SEP(*name_full) ? FSWrkDir_NullHandle
                       : FSWrkDir_Get();
#else
  cur_wrk_dir_handle = FSWrkDir_NullHandle;
#endif

  *p_file_handle = FSFile_Open(cur_wrk_dir_handle,
                               name_full,
                               mode_flags,
                               &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    FSPosix_ErrSet(err);
    return (DEF_NULL);
  }

  return (p_file_handle);
}

/****************************************************************************************************//**
 *                                               fs_fclose()
 *
 * @brief    Close and free a file.
 *
 * @param    p_file  Pointer to a file.
 *
 * @return   0,   if the file was successfully closed.
 *           EOF, otherwise.
 *
 * @note     (1) After a file is closed, the application MUST cease from accessing its file pointer.
 *               This could cause file system corruption, since this handle may be re-used for a
 *               different file.
 *
 * @note     (2) If the most recent operation is output (write), all unwritten data is written
 *               to the file.
 *               - (a) Any buffer assigned with fs_setbuf() or fs_setvbuf() shall no longer be
 *                     accessed by the file system and may be re-used by the application.
 *******************************************************************************************************/
int fs_fclose(FS_FILE *p_file)
{
  RTOS_ERR err;

  RTOS_ASSERT_DBG((p_file != DEF_NULL), RTOS_ERR_ASSERT_DBG_FAIL, EOF);

  FSFile_Close(*p_file, &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    FSPosix_ErrSet(err);
    return (EOF);
  }

  Mem_DynPoolBlkFree(&FSPosix_FileHandlePool,
                     (void *)p_file,
                     &err);
  RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL, EOF);

  return (0);
}

/****************************************************************************************************//**
 *                                               fs_fread()
 *
 * @brief    Read from a file.
 *
 * @param    p_dest  Pointer to destination buffer.
 *
 * @param    size    Size of each item to read.
 *
 * @param    nitems  Number of items to read.
 *
 * @param    p_file  Pointer to a file.
 *
 * @return   Number of items read.
 *
 * @note     (1) IEEE Std 1003.1, 2004 Edition, Section 'fread() : DESCRIPTION' states that :
 *               - (a) "The 'fread()' function shall read into the array pointed to by 'ptr' up to 'nitems'
 *                       elements whose size is specified by 'size' in bytes"
 *               - (b) "The file position indicator for the stream ... shall be advanced by the number of
 *                     bytes successfully read"
 *
 * @note     (2) IEEE Std 1003.1, 2004 Edition, Section 'fread() : RETURN VALUE' states that "[u]pon
 *               completion, 'fread()' shall return the number of elements which is less than 'nitems'
 *               only if a read error or end-of-file is encountered".
 *
 * @note     (3) See 'fs_fopen()  Note #1e'.
 *
 * @note     (4) The file MUST have been opened in read or update (read/write) mode.
 *
 * @note     (5) If an error occurs while reading from the file, a value less than 'nitems' will
 *               be returned.  To determine whether the premature return was caused by reaching the
 *               end-of-file, the 'fs_feof()' function should be used :
 *               @verbatim
 *                   rtn = fs_fread(pbuf, 1, 1000, pfile);
 *                   if (rtn < 1000) {
 *                       eof = fs_feof();
 *                       if (eof != 0) {
 *                           // File has reached EOF
 *                       } else {
 *                           // Error has occurred
 *                       }
 *                   }
 *               @endverbatim
 * @note     (6) [INTERNAL] #### Check for multiplication overflow.
 *******************************************************************************************************/
fs_size_t fs_fread(void      *p_dest,
                   fs_size_t size,
                   fs_size_t nitems,
                   FS_FILE   *p_file)
{
  CPU_SIZE_T size_tot;
  CPU_SIZE_T size_rd;
  fs_size_t  size_rd_items;
  RTOS_ERR   err;

  RTOS_ASSERT_DBG((p_dest != DEF_NULL), RTOS_ERR_ASSERT_DBG_FAIL, 0);
  RTOS_ASSERT_DBG((p_file != DEF_NULL), RTOS_ERR_ASSERT_DBG_FAIL, 0);

  size_tot = size * nitems;                                     // See Note #6.
  if (size_tot == 0u) {
    return (0u);
  }

  size_rd = FSFile_Rd(*p_file, p_dest, size_tot, &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    FSPosix_ErrSet(err);
    return (0u);
  }

  size_rd_items = size_rd / size;

  return (size_rd_items);
}

/****************************************************************************************************//**
 *                                               fs_fwrite()
 *
 * @brief    Write to a file.
 *
 * @param    p_src   Pointer to source buffer.
 *
 * @param    size    Size of each item to write.
 *
 * @param    nitems  Number of items to write.
 *
 * @param    p_file  Pointer to a file.
 *
 * @return   Number of items written.
 *
 * @note     (1) IEEE Std 1003.1, 2004 Edition, Section 'fwrite() : DESCRIPTION' states that :
 *               - (a) "The 'fwrite()' function shall write, from the array pointed to by 'ptr', up to
 *                     'nitems' elements whose size is specified by 'size', to the stream pointed to by
 *                     'stream'"
 *               - (b) "The file position indicator for the stream ... shall be advanced by the number of
 *                       bytes successfully written"
 *
 * @note     (2) IEEE Std 1003.1, 2004 Edition, Section 'fwrite() : RETURN VALUE' states that
 *               "'fwrite()' shall return the number of elements successfully written, which may be
 *               less than 'nitems' if a write error is encountered".
 *
 * @note     (3) See 'fs_fopen()  Notes #1d & #1e'.
 *
 * @note     (4) The file MUST have been opened in write or update (read/write) mode.
 *
 * @note     (5) [INTERNAL] #### Check for multiplication overflow.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
fs_size_t fs_fwrite(const void *p_src,
                    fs_size_t  size,
                    fs_size_t  nitems,
                    FS_FILE    *p_file)
{
  CPU_SIZE_T size_tot;
  CPU_SIZE_T size_wr;
  fs_size_t  size_wr_items;
  RTOS_ERR   err;

  RTOS_ASSERT_DBG((p_src != DEF_NULL), RTOS_ERR_ASSERT_DBG_FAIL, 0);
  RTOS_ASSERT_DBG((p_file != DEF_NULL), RTOS_ERR_ASSERT_DBG_FAIL, 0);

  size_tot = size * nitems;                                     // See Note #5.
  if (size_tot == 0u) {
    return (0u);
  }

  size_wr = FSFile_Wr(*p_file,
                      (void *)p_src,
                      size_tot,
                      &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    FSPosix_ErrSet(err);
    return (0u);
  }

  size_wr_items = (fs_size_t)size_wr / size;

  return (size_wr_items);
}
#endif

/****************************************************************************************************//**
 *                                               fs_ftruncate()
 *
 * @brief    Truncate a file.
 *
 * @param    p_file  Pointer to a file.
 *
 * @param    size    Length of file after truncation.
 *
 * @return    0, if the function succeeds.
 *           -1, otherwise.
 *
 * @note     (1) IEEE Std 1003.1, 2004 Edition, Section 'ftruncate() : DESCRIPTION' states that :
 *               - (a) "If 'fildes' is not a valid file descriptor open for writing, the 'ftruncate()'
 *                       function shall fail."
 *               - (b) "[The] 'ftruncate()' function shall cause the size of the file to be truncated to
 *                       'length'."
 *                   - (1) "If the size of the file previously exceeded length, the extra data shall no
 *                           longer be available to reads on the file."
 *                   - (2) "If the file previously was smaller than this size, 'ftruncate' shall either
 *                           increase the size of the file or fail."  This implementation increases the
 *                           size of the file.
 *
 * @note     (2) IEEE Std 1003.1, 2004 Edition, Section 'ftruncate() : DESCRIPTION' states that [u]pon
 *               successful completion, 'ftruncate()' shall return 0; otherwise -1 shall be returned
 *               and 'errno' set to indicate the error".
 *
 * @note     (3) If the file position indicator before the call to 'fs_ftruncate()' lay in the
 *               extra data destroyed by the function, then the file position will be set to the
 *               end-of-file.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
int fs_ftruncate(int      file_desc,
                 fs_off_t size)
{
  FS_FILE_HANDLE file_handle;
  RTOS_ERR       err;

  RTOS_ASSERT_DBG((file_desc != DEF_NULL), RTOS_ERR_ASSERT_DBG_FAIL, -1);

  file_handle = *((FS_FILE_HANDLE *)file_desc);

  FSFile_Truncate(file_handle, size, &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    FSPosix_ErrSet(err);
    return (-1);
  }

  return (0);
}
#endif

/****************************************************************************************************//**
 *                                                   fs_feof()
 *
 * @brief    Test EOF (End Of File) indicator on a file.
 *
 * @param    p_file  Pointer to a file.
 *
 * @return   0,              if EOF indicator is NOT set or if an error occurred.
 *           Non-zero value, if EOF indicator is     set.
 *
 * @note     (1) The return value from this function should ALWAYS be tested against 0 :
 *               @verbatim
 *                   rtn = fs_feof(pfile);
 *                   if (rtn == 0) {
 *                       // EOF indicator is NOT set
 *                   } else {
 *                       // EOF indicator is     set
 *                   }
 *               @endverbatim
 * @note     (2) If the end-of-file indicator is set, that is fs_feof() returns a non-zero value,
 *               fs_clearerr() can be used to clear that indicator.
 *******************************************************************************************************/
int fs_feof(FS_FILE *p_file)
{
  CPU_BOOLEAN is_eof;
  int         rtn;
  RTOS_ERR    err;

  RTOS_ASSERT_DBG((p_file != DEF_NULL), RTOS_ERR_ASSERT_DBG_FAIL, -1);

  is_eof = FSFile_IsEOF(*p_file, &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    FSPosix_ErrSet(err);
    return (-1);
  }

  rtn = is_eof ? 1 : 0;

  return (rtn);
}

/****************************************************************************************************//**
 *                                               fs_ferror()
 *
 * @brief    Test error indicator on a file.
 *
 * @param    p_file  Pointer to a file.
 *
 * @return   0,              if error indicator is NOT set or if an error occurred.
 *           Non-zero value, if error indicator is     set.
 *
 * @note     (1) The return value from this function should ALWAYS be tested against 0 :
 *               @verbatim
 *                   rtn = fs_ferror(pfile);
 *                   if (rtn == 0) {
 *                       // Error indicator is NOT set
 *                   } else {
 *                       // Error indicator is     set
 *                   }
 *               @endverbatim
 * @note     (2) If the error indicator is set, that is fs_ferror() returns a non-zero value,
 *               fs_clearerr() can be used to clear that indicator.
 *******************************************************************************************************/
int fs_ferror(FS_FILE *p_file)
{
  CPU_BOOLEAN is_err;
  int         rtn;
  RTOS_ERR    err;

  RTOS_ASSERT_DBG((p_file != DEF_NULL), RTOS_ERR_ASSERT_DBG_FAIL, EBADF);

  is_err = FSFile_IsErr(*p_file, &err);
  rtn = is_err ? STDIO_ERR : 0;

  return (rtn);
}

/****************************************************************************************************//**
 *                                               fs_clearerr()
 *
 * @brief    Clear EOF (End Of File) and error indicators on a file.
 *
 * @param    p_file  Pointer to a file.
 *******************************************************************************************************/
void fs_clearerr(FS_FILE *p_file)
{
  RTOS_ERR err;

  RTOS_ASSERT_DBG(p_file != DEF_NULL, RTOS_ERR_ASSERT_DBG_FAIL,; );
  FSFile_ErrClr(*p_file, &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    FSPosix_ErrSet(err);
    return;
  }
}

/****************************************************************************************************//**
 *                                               fs_fgetpos()
 *
 * @brief    Get file position indicator.
 *
 * @param    p_file  Pointer to a file.
 *
 * @param    p_pos   Pointer to variable that will receive the file position indicator.
 *
 * @return   0,              if no error occurs.
 *           Non-zero value, otherwise.
 *
 * @note     (1) The return value should be tested against 0 :
 *               @verbatim
 *                   rtn = fs_fgetpos(pfile, &pos);
 *                   if (rtn == 0) {
 *                       // No error occurred.
 *                   } else {
 *                       // Handle error.
 *                   }
 *               @endverbatim
 * @note     (2) The value placed in 'p_pos' should be passed to fs_fsetpos() to reposition the file
 *               to its position at the time when this function was called.
 *******************************************************************************************************/
int fs_fgetpos(FS_FILE   *p_file,
               fs_fpos_t *p_pos)
{
  RTOS_ERR err;

  RTOS_ASSERT_DBG((p_file != DEF_NULL), RTOS_ERR_ASSERT_DBG_FAIL, -1);

  *p_pos = FSFile_PosGet(*p_file, &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    FSPosix_ErrSet(err);
    return (-1);
  }

  return (0);
}

/****************************************************************************************************//**
 *                                               fs_fsetpos()
 *
 * @brief    Set file position indicator.
 *
 * @param    p_file  Pointer to a file.
 *
 * @param    p_pos   Pointer to variable holding the file position.
 *
 * @return   0,              if the function succeeds.
 *           Non-zero value, otherwise.
 *
 * @note     (1) The return value should be tested against 0 :
 *               @verbatim
 *                   rtn = fs_fsetpos(pfile, &pos);
 *                   if (rtn == 0) {
 *                       // No error occurred.
 *                   } else {
 *                       // Handle error.
 *                   }
 *               @endverbatim
 * @note     (2) IEEE Std 1003.1, 2004 Edition, Section 'fsetpos() : DESCRIPTION' states that :
 *               - (a) "If a read or write error occurs, the error indicator for the stream is set"
 *               - (b) "The 'fsetpos()' function shall set the file position and state indicators for
 *                       the stream pointed to by stream according to the value of the object pointed to
 *                       by 'pos', which the application shall ensure is a value obtained from an earlier
 *                       call to 'fgetpos()' on the same stream."
 *
 * @note     (3) IEEE Std 1003.1, 2004 Edition, Section 'fsetpos() : RETURN VALUE' states that "[t]he
 *               'fsetpos()' function shall return 0 if it succeeds; otherwise, it shall return a
 *               non-zero value".
 *
 * @note     (4) No attempt is made to verify that the value stored in 'p_pos' was returned from
 *               'fs_fgetpos()'.
 *
 * @note     (5) See also 'fs_fseek()  Note #1d'.
 *******************************************************************************************************/
int fs_fsetpos(FS_FILE         *p_file,
               const fs_fpos_t *p_pos)
{
  RTOS_ERR err;

  RTOS_ASSERT_DBG((p_file != DEF_NULL), RTOS_ERR_ASSERT_DBG_FAIL, -1);

  FSFile_PosSet(*p_file,
                *p_pos,
                FS_FILE_ORIGIN_START,
                &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    FSPosix_ErrSet(err);
    return (-1);
  }

  return (0);
}

/****************************************************************************************************//**
 *                                               fs_fseek()
 *
 * @brief    Set file position indicator.
 *
 * @param    p_file  Pointer to a file.
 *
 * @param    offset  Offset from file position specified by 'origin'.
 *
 * @param    origin  Reference position for offset :
 *                   SEEK_SET    Offset is from the beginning of the file.
 *                   SEEK_CUR    Offset is from current file position.
 *                   SEEK_END    Offset is from the end       of the file.
 *
 * @return    0, if the function succeeds.
 *           -1, otherwise.
 *
 * @note     (1) IEEE Std 1003.1, 2004 Edition, Section 'fread() : DESCRIPTION' states that :
 *               - (a) "If a read or write error occurs, the error indicator for the stream shall be set"
 *               - (b) "The new position measured in bytes from the beginning of the file, shall be
 *                       obtained by adding 'offset' to the position specified by 'whence'. The specified
 *                       point is ..."
 *                   - (1) "... the beginning of the file                        for SEEK_SET"
 *                   - (2) "... the current value of the file-position indicator for SEEK_CUR"
 *                   - (3) "... end-of-file                                      for SEEK_END"
 *               - (c) "A successful call to 'fseek()' shall clear the end-of-file indicator"
 *               - (d) "The 'fseek()' function shall allow the file-position indicator to be set beyond
 *                       the end of existing data in the file.  If data is later written at this point,
 *                       subsequent reads of data in the gap shall return bytes with the value 0 until
 *                       data is actually written into the gap."
 *
 * @note     (2) IEEE Std 1003.1, 2004 Edition, Section 'fread() : RETURN VALUE' states that "[t]he
 *               'fseek()' and 'fseeko()' functions shall return 0 if they succeeds.  Otherwise, they
 *               shall return -1".
 *
 * @note     (3) If the file position indicator is set beyond the file's current data, the file MUST
 *               be opened in write or read/write mode.
 *******************************************************************************************************/
int fs_fseek(FS_FILE  *p_file,
             long int offset,
             int      origin)
{
  RTOS_ERR err;

  RTOS_ASSERT_DBG((p_file != DEF_NULL), RTOS_ERR_ASSERT_DBG_FAIL, -1);

  FSFile_PosSet(*p_file,
                offset,
                origin,
                &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    FSPosix_ErrSet(err);
    return (-1);
  }

  return (0);
}

/****************************************************************************************************//**
 *                                               fs_ftell()
 *
 * @brief    Get file position indicator.
 *
 * @param    p_file  Pointer to a file.
 *
 * @return   The current file position, if the function succeeds.
 *           -1,                        otherwise.
 *
 * @note     (1) IEEE Std 1003.1, 2004 Edition, Section 'ftell() : RETURN VALUE' states that :
 *               - (a) "Upon successful completion, 'ftell()' and 'ftello()' shall return the current
 *                       value of the file-position indicator for the stream measured in bytes from the
 *                       beginning of the file."
 *               - (b) "Otherwise, 'ftell()' and 'ftello()' shall return -1, cast to 'long' and 'off_t'
 *                       respectively, and set errno to indicate the error."
 *
 * @note     (2) [INTERNAL] #### Check for overflow in cast.
 *******************************************************************************************************/
long int fs_ftell(FS_FILE *p_file)
{
  FS_FILE_SIZE pos;
  RTOS_ERR     err;

  RTOS_ASSERT_DBG((p_file != DEF_NULL), RTOS_ERR_ASSERT_DBG_FAIL, -1);

  pos = FSFile_PosGet(*p_file, &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    FSPosix_ErrSet(err);
    return ((long int)-1);
  }

  return ((long int)pos);                                       // See Note #2.
}

/****************************************************************************************************//**
 *                                               fs_rewind()
 *
 * @brief    Reset file position indicator of a file.
 *
 * @param    p_file  Pointer to a file.
 *
 * @note     (1) IEEE Std 1003.1, 2004 Edition, Section 'rewind() : DESCRIPTION' states that :
 *               "[T]he call 'rewind(stream)' shall be equivalent to '(void)fseek(stream, 0L, SEEK_SET)'
 *               except that 'rewind()' shall also clear the error indicator."
 *******************************************************************************************************/
void fs_rewind(FS_FILE *p_file)
{
  fs_clearerr(p_file);
  (void)fs_fseek(p_file, 0, SEEK_SET);
}

/****************************************************************************************************//**
 *                                               fs_fileno()
 *
 * @brief    Get file descriptor integer associated to file.
 *
 * @param    p_file  Pointer to a file.
 *
 * @return   File descriptor integer, if NO errors.
 *           -1,                      otherwise.
 *******************************************************************************************************/
int fs_fileno(FS_FILE *p_file)
{
  RTOS_ASSERT_DBG((p_file != DEF_NULL), RTOS_ERR_ASSERT_DBG_FAIL, -1);
  return ((int)p_file);
}

/****************************************************************************************************//**
 *                                               fs_fstat()
 *
 * @brief    Get information about a file.
 *
 * @param    p_file  Pointer to a file.
 *
 * @param    p_info  Pointer to structure that will receive the file information.
 *
 * @return    0, if the function succeeds.
 *           -1, otherwise.
 *******************************************************************************************************/
int fs_fstat(int            file_desc,
             struct fs_stat *p_info)
{
  FS_FILE_HANDLE file_handle;
  FS_ENTRY_INFO  info;
  fs_mode_t      mode;
  RTOS_ERR       err;

  RTOS_ASSERT_DBG((file_desc != DEF_NULL), RTOS_ERR_ASSERT_DBG_FAIL, -1);

  file_handle = *((FS_FILE_HANDLE *)file_desc);

  FSFile_Query(file_handle,
               &info,
               &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    FSPosix_ErrSet(err);
    return (-1);
  }

  mode = info.Attrib.IsDir ? S_IFDIR : S_IFREG;

  if (DEF_BIT_IS_SET(info.Attrib.Rd, FS_ENTRY_ATTRIB_RD) == DEF_YES) {
    mode |= S_IRUSR | S_IRGRP | S_IROTH;
  }

  if (DEF_BIT_IS_SET(info.Attrib.Wr, FS_ENTRY_ATTRIB_WR) == DEF_YES) {
    mode |= S_IWUSR | S_IWGRP | S_IWOTH;
  }

  p_info->st_dev = 0u;
  p_info->st_ino = 0u;
  p_info->st_mode = mode;
  p_info->st_nlink = 1u;
  p_info->st_uid = 0u;
  p_info->st_gid = 0u;
  p_info->st_size = (fs_off_t) info.Size;
  p_info->st_atime = (fs_time_t)-1;
  p_info->st_mtime = (fs_time_t) info.DateTimeWr;
  p_info->st_ctime = (fs_time_t) info.DateTimeCreate;
  p_info->st_blksize = (fs_blksize_t) info.BlkSize;
  p_info->st_blocks = (fs_blkcnt_t) info.BlkCnt;

  return (0);
}

/****************************************************************************************************//**
 *                                               fs_flockfile()
 *
 * @brief    Acquire task ownership of a file.
 *
 * @param    p_file  Pointer to a file.
 *
 * @note     (1) IEEE Std 1003.1, 2004 Edition, Section 'flockfile(), ftrylockfile(), funlockfile() :
 *               DESCRIPTION' states that :
 *               - (a) "The 'flockfile()' function shall acquire thread ownership of a (FILE *) object."
 *               - (b) "The functions shall behave as if there is a lock count associated with each
 *                       (FILE *) object."
 *                   - (1) "The (FILE *) object is unlocked when the count is zero."
 *                   - (2) "When the count is positive, a single thread owns the (FILE *) object."
 *                   - (3) "When the 'flockfile()' function is called, if the count is zero or if the
 *                         count is positive and the caller owns the (FILE *), the count shall be
 *                         incremented.  Otherwise, the calling thread shall be suspended, waiting for
 *                         the count to return to zero."
 *                   - (4) "Each call to 'funlockfile()' shall decrement the count."
 *                   - (5) "This allows matching calls to 'flockfile()' (or successful calls to 'ftrylockfile()')
 *                         and 'funlockfile()' to be nested."
 *******************************************************************************************************/

#if (FS_CORE_CFG_FILE_LOCK_EN == DEF_ENABLED)
void fs_flockfile(FS_FILE *p_file)
{
  RTOS_ERR err;

  RTOS_ASSERT_DBG((p_file != DEF_NULL), RTOS_ERR_ASSERT_DBG_FAIL,; );

  FSFile_Lock(*p_file, &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    FSPosix_ErrSet(err);
  }
}
#endif

/****************************************************************************************************//**
 *                                               fs_ftrylockfile()
 *
 * @brief    Acquire task ownership of a file (if available).
 *
 * @param    p_file  Pointer to a file.
 *
 * @return   0,              if no error occurs and the file lock is acquired.
 *           Non-zero value, otherwise.
 *
 * @note     (1) IEEE Std 1003.1, 2004 Edition, Section 'flockfile(), ftrylockfile(), funlockfile() :
 *               DESCRIPTION' states that :
 *               - (a) See 'fs_flockfile()  Note(s)'.
 *               - (b) "The 'ftrylockfile() function shall acquire for a thread ownership of a (FILE *)
 *                     object if the object is available; 'ftrylockfile()' is a non-blocking version of
 *                     'flockfile()'."
 *
 * @note     (2) IEEE Std 1003.1, 2004 Edition, Section 'flockfile(), ftrylockfile(), funlockfile() :
 *               RETURN VALUES' states that "[t]he 'ftrylockfile()' function shall return zero for
 *               success and non-zero to indicate that the lock cannot be acquired".
 *******************************************************************************************************/

#if (FS_CORE_CFG_FILE_LOCK_EN == DEF_ENABLED)
int fs_ftrylockfile(FS_FILE *p_file)
{
  RTOS_ERR err;

  RTOS_ASSERT_DBG((p_file != DEF_NULL), RTOS_ERR_ASSERT_DBG_FAIL, -1);

  FSFile_TryLock(*p_file, &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    FSPosix_ErrSet(err);
    return (-1);
  }

  return (0);
}
#endif

/****************************************************************************************************//**
 *                                               fs_funlockfile()
 *
 * @brief    Release task ownership of a file.
 *
 * @param    p_file  Pointer to a file.
 *
 * @note     (1) IEEE Std 1003.1, 2004 Edition, Section 'flockfile(), ftrylockfile(), funlockfile() :
 *               DESCRIPTION' states that :
 *               - (a) See 'fs_flockfile()  Note(s)'.
 *******************************************************************************************************/

 #if (FS_CORE_CFG_FILE_LOCK_EN == DEF_ENABLED)
void fs_funlockfile(FS_FILE *p_file)
{
  RTOS_ASSERT_DBG((p_file != DEF_NULL), RTOS_ERR_ASSERT_DBG_FAIL,; );
  FSFile_Unlock(*p_file);
}
#endif

/****************************************************************************************************//**
 *                                               fs_setbuf()
 *
 * @brief    Assign buffer to a file.
 *
 * @param    p_file  Pointer to a file.
 *
 * @param    p_buf   Pointer to buffer.
 *
 * @note     (1) IEEE Std 1003.1, 2004 Edition, Section 'setbuf() : DESCRIPTION' states that :
 *               "Except that it returns no value, the function call: 'setbuf(stream, buf)'
 *               shall be equivalent to: 'setvbuf(stream, buf, _IOFBF, BUFSIZ)' if 'buf' is not a
 *               null pointer"
 *
 * @note     (2) See 'fs_setvbuf()  Note(s)'.
 *******************************************************************************************************/

#if (FS_CORE_CFG_FILE_BUF_EN == DEF_ENABLED)
void fs_setbuf(FS_FILE *p_file,
               char    *p_buf)
{
  (void)fs_setvbuf(p_file,
                   p_buf,
                   _IOFBF,
                   BUFSIZ);
}
#endif

/****************************************************************************************************//**
 *                                               fs_setvbuf()
 *
 * @brief    Assign buffer to a file.
 *
 * @param    p_file  Pointer to a file.
 *
 * @param    p_buf   Pointer to buffer.
 *
 * @param    mode    Buffer mode.
 *                   _IOFBF    Data buffered for reads & writes.
 *                   _IONBR    Data unbuffered for reads & writes.
 *
 * @param    size    Size of buffer, in octets.
 *
 * @return   -1, if an error occurs.
 *           0, if no error occurs.
 *
 * @note     (1) IEEE Std 1003.1, 2004 Edition, Section 'setvbuf() : DESCRIPTION' states that :
 *               - (a) "The setvbuf() function may be used after the stream pointed to by stream is
 *                       associated with an open file but before any other operation (other than an
 *                       unsuccessful call to setvbuf()) is performed on the stream."
 *               - (b) "The argument 'mode' determines how 'stream' will be buffered ... "
 *                   - (1) ... FS__IOFBF "causes input/output to be fully buffered".
 *                   - (2) ... FS__IONBF "causes input/output to be unbuffered".
 *                   - (3) No equivalent to '_IOLBF' is supported.
 *               - (c) "If 'buf' is not a null pointer, the array it points to may be used instead of a
 *                       buffer allocated by the 'setvbuf' function and the argument 'size' specifies the
 *                       size of the array ...."  This implementation REQUIRES that 'buf' not be a null
 *                       pointer; the array 'buf' points to will always be used.
 *               - (d) The function "returns zero on success, or nonzero if an invalid value is given
 *                       for 'mode' or if the request cannot be honored".
 *
 * @note     (2) 'size' MUST be more than or equal to the size of one sector and will be rounded DOWN
 *               to the size of a number of full sectors.
 *
 * @note     (3) Once a buffer is assigned to a file, a new buffer may not be assigned nor may the
 *               assigned buffer be removed.  To change the buffer, the file should be closed &
 *               re-opened.
 *
 * @note     (4) Upon power loss, any data stored in file buffers will be lost.
 *******************************************************************************************************/

#if (FS_CORE_CFG_FILE_BUF_EN == DEF_ENABLED)
int fs_setvbuf(FS_FILE   *p_file,
               char      *p_buf,
               int       mode,
               fs_size_t size)
{
  RTOS_ERR err;

  RTOS_ASSERT_DBG((p_file != DEF_NULL), RTOS_ERR_ASSERT_DBG_FAIL, -1);
  RTOS_ASSERT_DBG((p_buf != DEF_NULL), RTOS_ERR_ASSERT_DBG_FAIL, -1);

  FSFile_BufAssign(*p_file,
                   p_buf,
                   mode,
                   size,
                   &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    FSPosix_ErrSet(err);
    return (-1);
  }

  return (0);
}
#endif

/****************************************************************************************************//**
 *                                               fs_fflush()
 *
 * @brief    Flush buffer contents to file.
 *
 * @param    p_file  Pointer to a file.
 *
 * @return   0,   if flushing succeeds.
 *           EOF, otherwise.
 *
 * @note     (1) IEEE Std 1003.1, 2004 Edition, Section 'fflush() : DESCRIPTION' states that :
 *               - (a) "If 'stream' points to an output stream or an update stream in which the most
 *                       recent operation was not input, 'fflush()' shall cause any unwritten data for
 *                       that stream to be written to the file."
 *               - (b) "If 'stream' is a null pointer, the 'fflush' function performs this flushing
 *                       action on all streams ...."  #### Currently unimplemented.
 *               - (c) "Upon successful completion, fflush() shall return 0; otherwise, it shall set the
 *                       error indicator for the stream, return EOF."
 *
 * @note     (2) IEEE Std 1003.1, 2004 Edition, Section 'fflush()' defines no behavior for an input
 *               stream or update stream in which the most recent operation was input.
 *
 *           - (a) In this implementation, if the most recent operation is input, fs_fflush()
 *                   clears all buffered input data.
 *******************************************************************************************************/

#if (FS_CORE_CFG_FILE_BUF_EN == DEF_ENABLED)
int fs_fflush(FS_FILE *p_file)
{
  RTOS_ERR err;

  RTOS_ASSERT_DBG((p_file != DEF_NULL), RTOS_ERR_ASSERT_DBG_FAIL, EOF);

  FSFile_BufFlush(*p_file, &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    FSPosix_ErrSet(err);
    return (EOF);
  }

  return (0);
}
#endif

/********************************************************************************************************
 *                                           TIME FUNCTIONS
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               fs_asctime_r()
 *
 * @brief    Convert date/time to string.
 *
 * @param    p_time      Pointer to date/time to format.
 *
 * @param    p_str_time  String buffer that will receive the date/time string (see Note #1).
 *
 * @return   Pointer to date/time string, if NO errors.
 *           Pointer to NULL,             otherwise.
 *
 * @note     (1) String buffer MUST be at least 26 characters long.  Buffer overruns MUST be prevented
 *               by caller.
 *
 * @note     (2) IEEE Std 1003.1, 2004 Edition, Section 'asctime() : DESCRIPTION' states that :
 *               - (a) "The 'asctime()' function shall convert the broken-down time in the structure
 *                       pointed to by 'timeptr' into a string in the form:
 *                       @verbatim
 *                       Sun Sep 16 01:03:52 1973\n\0
 *                       @endverbatim
 *******************************************************************************************************/
char *fs_asctime_r(const struct fs_tm *p_time,
                   char               *p_str_time)
{
  sl_sleeptimer_date_t date;
  uint32_t             result;

  RTOS_ASSERT_DBG((p_time != DEF_NULL), RTOS_ERR_ASSERT_DBG_FAIL, DEF_NULL);
  RTOS_ASSERT_DBG((p_str_time != DEF_NULL), RTOS_ERR_ASSERT_DBG_FAIL, DEF_NULL);

  //                                                               --------------- FMT DATE/TIME --------------
  date.sec = (CPU_INT08U)p_time->tm_sec;
  date.min = (CPU_INT08U)p_time->tm_min;
  date.hour = (CPU_INT08U)p_time->tm_hour;
  date.month_day = (CPU_INT08U)p_time->tm_mday;
  date.month = (CPU_INT08U)p_time->tm_mon;
  date.year = (CPU_INT08U)p_time->tm_year;
  date.day_of_week = (CPU_INT08U)p_time->tm_wday;

  result = sl_sleeptimer_convert_date_to_str(p_str_time, FS_TIME_STR_MIN_LEN, (const uint8_t *)FS_TIME_FMT, &date);
  if (result == 0) {
    RTOS_ERR err;
    RTOS_ERR_SET(err, RTOS_ERR_TIME_INVALID);
    FSPosix_ErrSet(err);
    return (DEF_NULL);
  }

  return (p_str_time);
}

/****************************************************************************************************//**
 *                                               fs_ctime_r()
 *
 * @brief    Convert timestamp to string.
 *
 * @param    p_ts        Pointer to timestamp to format.
 *
 * @param    p_str_time  String buffer that will receive the timestamp string (see Note #1).
 *
 * @return   Pointer to timestamp buffer, if NO errors.
 *           Pointer to NULL,             otherwise.
 *
 * @note     (1) The timestamp buffer MUST be at least 26 characters long.  buffer overruns MUST be
 *               prevented by caller.
 *
 * @note     (2) IEEE Std 1003.1, 2004 Edition, Section 'ctime() : DESCRIPTION' states that :
 *               - (a) 'ctime' "shall be equivalent to:  'asctime(localtime(clock))'".
 *******************************************************************************************************/
char *fs_ctime_r(const fs_time_t *p_ts,
                 char            *p_str_time)
{
  uint32_t             ts;
  sl_sleeptimer_date_t date;
  sl_status_t          status;
  uint32_t             result;

  RTOS_ASSERT_DBG((p_ts != DEF_NULL), RTOS_ERR_ASSERT_DBG_FAIL, DEF_NULL);
  RTOS_ASSERT_DBG((p_str_time != DEF_NULL), RTOS_ERR_ASSERT_DBG_FAIL, DEF_NULL);

  //                                                               ------------------- FMT DATE/TIME ------------------
  ts = (uint32_t)*p_ts;
  status = sl_sleeptimer_convert_time_to_date(ts, 0, &date);
  if (status != SL_STATUS_OK) {
    RTOS_ERR err;
    RTOS_ERR_SET(err, RTOS_ERR_TIME_INVALID);
    FSPosix_ErrSet(err);
    return (DEF_NULL);
  }

  result = sl_sleeptimer_convert_date_to_str(p_str_time, FS_TIME_STR_MIN_LEN, (const uint8_t *)FS_TIME_FMT, &date);
  if (result == 0) {
    RTOS_ERR err;
    RTOS_ERR_SET(err, RTOS_ERR_TIME_INVALID);
    FSPosix_ErrSet(err);
    return (DEF_NULL);
  }

  return (p_str_time);
}

/****************************************************************************************************//**
 *                                               fs_localtime_r()
 *
 * @brief    Convert timestamp to date/time.
 *
 * @param    p_ts    Pointer to timestamp to convert.
 *
 * @param    p_time  Pointer to variable that will receive the date/time.
 *
 * @return   Pointer to date/time, if NO errors.
 *           Pointer to NULL,      otherwise.
 *
 * @note     (1) IEEE Std 1003.1, 2004 Edition, Section '4.14 Seconds Since the Epoch()' states that
 *
 *               - (a) "If the year is <1970 or the value is negative, the relationship is undefined.
 *                       If the year is >=1970  and the value is non-negative, the value is related to
 *                       coordinated universal time name according to the C-language expression, where
 *                       tm_sec, tm_min, tm_hour, tm_yday, and tm_year are all integer types:
 *                       @verbatim
 *                       tm_sec + tm_min*60 + tm_hour*3600 + tm_yday*86400 +
 *                       (tm_year-70)*31536000 + ((tm_year-69)/4)*86400 -
 *                       ((tm_year-1)/100)*86400 + ((tm_year+299)/400)*86400"
 *                       @endverbatim
 *               - (b) "The relationship between the actual time of day and the current value for
 *                       seconds since the Epoch is unspecified."
 *
 * @note     (2) The expression for the time value can be rewritten :
 *                   @verbatim
 *                   time_val = tm_sec + 60 * (tm_min  +
 *                                       60 * (tm_hour +
 *                                       24 * (tm_yday + ((tm_year-69)/4) - ((tm_year-1)/100) + ((tm_year+299)/400) +
 *                                       365 * (tm_year - 70))))
 *                   @endverbatim
 *******************************************************************************************************/
struct fs_tm *fs_localtime_r(const fs_time_t *p_ts,
                             struct fs_tm    *p_time)
{
  sl_sleeptimer_date_t stime;
  sl_status_t          status;

  RTOS_ASSERT_DBG((p_ts != DEF_NULL), RTOS_ERR_ASSERT_DBG_FAIL, DEF_NULL);
  RTOS_ASSERT_DBG((p_time != DEF_NULL), RTOS_ERR_ASSERT_DBG_FAIL, DEF_NULL);

  //                                                               ------------ CALC BROKEN-DOWN DATE/TIME ------------
  status = sl_sleeptimer_convert_time_to_date(*p_ts, 0, &stime);
  if (status != SL_STATUS_OK) {
    RTOS_ERR err;
    RTOS_ERR_SET(err, RTOS_ERR_TIME_INVALID);
    FSPosix_ErrSet(err);
    return (DEF_NULL);
  }

  p_time->tm_sec = (int)stime.sec;
  p_time->tm_min = (int)stime.min;
  p_time->tm_hour = (int)stime.hour;
  p_time->tm_mday = (int)stime.month_day;
  p_time->tm_mon = (int)stime.month;
  p_time->tm_year = (int)stime.year;
  p_time->tm_wday = (int)stime.day_of_week;
  p_time->tm_yday = (int)stime.day_of_year;
  p_time->tm_isdst = 0;

  return (p_time);
}

/****************************************************************************************************//**
 *                                               fs_mktime()
 *
 * @brief    Convert date/time to timestamp.
 *
 * @param    p_time  Pointer to date/time to convert.
 *
 * @return   Time value,    if NO errors.
 *           (fs_time_t)-1, otherwise.
 *
 * @note     (1) See 'fs_localtime_r()  Note #1'.
 *
 * @note     (2) IEEE Std 1003.1, 2004 Edition, Section 'mktime() : DESCRIPTION' states that :
 *               - (a) "The 'mktime()' function shall convert the broken-down time, expressed as local
 *                     time, in the structure pointed to by 'timeptr', into a time since the Epoch"
 *               - (b) "The original values of 'tm_wday' and 'tm_yday' components of the structure are
 *                     ignored, and the original values of the other components are not restricted to
 *                     the ranges described in <time.h>" (see also Note #3)
 *               - (c) "Upon successful completion, the values of the 'tm_wday' and 'tm_yday' components
 *                     of the structure shall be set appropriately, and the other components set to
 *                     represent the specified time since the Epoch, but with their values forced to the
 *                     ranges indicated in the <time.h> entry"
 *
 * @note     (3) Even though strict range checking is NOT performed, the broken-down date/time
 *               components are restricted to positive values, and the month value MUST be between
 *               0 & 11 (otherwise, the day of year cannot be determined).
 *******************************************************************************************************/
fs_time_t fs_mktime(struct fs_tm *p_time)
{
  sl_sleeptimer_date_t date;
  uint32_t             ts;
  sl_status_t          status;

  RTOS_ASSERT_DBG((p_time != DEF_NULL), RTOS_ERR_ASSERT_DBG_FAIL, DEF_NULL);

  //                                                               ----------------- COMPUTE TIME VAL -----------------
  date.sec = (uint8_t)p_time->tm_sec;
  date.min = (uint8_t)p_time->tm_min;
  date.hour = (uint8_t)p_time->tm_hour;
  date.month_day = (uint8_t)p_time->tm_mday;
  date.month = (uint8_t)p_time->tm_mon;
  date.year = (uint16_t)p_time->tm_year;

  status = sl_sleeptimer_convert_date_to_time(&date, &ts);
  if (status != SL_STATUS_OK) {
    RTOS_ERR err;
    RTOS_ERR_SET(err, RTOS_ERR_TIME_INVALID);
    FSPosix_ErrSet(err);
    return ((fs_time_t)-1);
  }

  return ((fs_time_t)ts);
}

/****************************************************************************************************//**
 *                                               fs_perror()
 *
 * @brief    Print POSIX error code along with user's output string.
 *
 * @param    p_err_desc  Pointer to the user output string.
 *******************************************************************************************************/

#ifdef FS_CORE_CFG_POSIX_PUTCHAR
void fs_perror(const char *p_err_desc)
{
  int err_posix;

  err_posix = _fs_errno();
  if (err_posix != 0) {
    Str_Printf(FSPosix_PutCharWrapper, DEF_NULL, "Error: %u (%s)\r\n", err_posix, p_err_desc);
  }
}
#endif

/****************************************************************************************************//**
 *                                               _fs_errno()
 *
 * @brief    Converts file system error code into Posix error code.
 *
 * @return   Posix error code.
 *******************************************************************************************************/

#ifdef FS_CORE_CFG_POSIX_PUTCHAR
int _fs_errno(void)
{
  RTOS_ERR_CODE err_code;
  int           err_posix;

  err_code = FSPosix_ErrGet();
  err_posix = FSPosix_ErrConv(err_code);

  return (err_posix);
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
   INTERNAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               FSPosix_Init()
 *
 * @brief    Initialize file system object management module.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/
void FSPosix_Init(RTOS_ERR *p_err)
{
  FSPosix_ErrnoTaskRegHandle = KAL_TaskRegCreate(DEF_NULL, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  Mem_DynPoolCreate("FS - Posix file handle pool",
                    &FSPosix_FileHandlePool,
                    FSCore_InitCfg.MemSegPtr,
                    sizeof(FS_FILE_HANDLE),
                    sizeof(CPU_ALIGN),
                    1u,
                    LIB_MEM_BLK_QTY_UNLIMITED,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

#if (FS_CORE_CFG_DIR_EN == DEF_ENABLED)
  Mem_DynPoolCreate("FS - Posix dir handle pool",
                    &FSPosix_DirHandlePool,
                    FSCore_InitCfg.MemSegPtr,
                    sizeof(FS_DIR_HANDLE),
                    sizeof(CPU_ALIGN),
                    1u,
                    LIB_MEM_BLK_QTY_UNLIMITED,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
#endif
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION DEFINITIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           FSPosix_FileModeParse()
 *
 * @brief    Parse file access mode string.
 *
 * @param    str_mode    Access mode of the file.
 *
 * @param    str_len     Length of the mode string.
 *
 * @return   Mode identifier, if mode string invalid.
 *           0,               otherwise.
 *******************************************************************************************************/
FS_FLAGS FSPosix_FileModeParse(CPU_CHAR   *str_mode,
                               CPU_SIZE_T str_len)
{
  FS_FLAGS    mode;
  CPU_BOOLEAN b_present;
  CPU_BOOLEAN plus_present;

  //                                                               -------------------- PARSE MODE --------------------
  switch (*str_mode) {                                          // Interpret first letter: rd, wr, append.
    case ASCII_CHAR_LATIN_LOWER_R:
      mode = FS_FILE_ACCESS_MODE_RD;
      break;

    case ASCII_CHAR_LATIN_LOWER_W:
      mode = FS_FILE_ACCESS_MODE_WR | FS_FILE_ACCESS_MODE_TRUNCATE | FS_FILE_ACCESS_MODE_CREATE;
      break;

    case ASCII_CHAR_LATIN_LOWER_A:
      mode = FS_FILE_ACCESS_MODE_WR | FS_FILE_ACCESS_MODE_CREATE   | FS_FILE_ACCESS_MODE_APPEND;
      break;

    default:
      return (FS_FILE_ACCESS_MODE_NONE);
  }
  str_mode++;
  str_len--;

  //                                                               Interpret remaining str: "", "+", "b", "b+", "+b".
  b_present = DEF_NO;
  plus_present = DEF_NO;

  while (str_len > 0u) {
    if (*str_mode == (CPU_CHAR)ASCII_CHAR_PLUS_SIGN) {
      if (plus_present == DEF_YES) {
        mode = FS_FILE_ACCESS_MODE_NONE;                        // Invalid mode.
        return (mode);
      }
      mode |= FS_FILE_ACCESS_MODE_RD | FS_FILE_ACCESS_MODE_WR;
      plus_present = DEF_YES;
    } else if (*str_mode == (CPU_CHAR)ASCII_CHAR_LATIN_LOWER_B) {
      if (b_present == DEF_YES) {
        mode = FS_FILE_ACCESS_MODE_NONE;                        // Invalid mode.
        return (mode);
      }
      b_present = DEF_YES;
    } else {
      mode = FS_FILE_ACCESS_MODE_NONE;                          // Invalid mode.
      return(mode);
    }
    str_mode++;
    str_len--;
  }

  return (mode);
}

/****************************************************************************************************//**
 *                                               FSPosix_ErrConv()
 *
 * @brief    Convert a native error code to a POSIX error code.
 *
 * @param    err_code    Native error code.
 *
 * @return   Converted error code.
 *******************************************************************************************************/
static int FSPosix_ErrConv(RTOS_ERR_CODE err_code)
{
  int err_posix;

  switch (err_code) {
    case RTOS_ERR_NONE:
      err_posix = 0u;
      break;
    case RTOS_ERR_NAME_INVALID:
      err_posix = EINVAL;
      break;
    case RTOS_ERR_DIR_FULL:
      err_posix = ENOSPC;
      break;
    case RTOS_ERR_ENTRY_ROOT_DIR:
      err_posix = EPERM;
      break;
    case RTOS_ERR_ENTRY_PARENT_NOT_DIR:
      err_posix = ENOTDIR;
      break;
    case RTOS_ERR_DIR_NOT_EMPTY:
      err_posix = ENOTEMPTY;
      break;
    case RTOS_ERR_NOT_FOUND:
      err_posix = ENOENT;
      break;
    case RTOS_ERR_ALREADY_EXISTS:
      err_posix = EEXIST;
      break;
    case RTOS_ERR_ENTRY_OPENED:
      err_posix = EBUSY;
      break;
    case RTOS_ERR_FILE_ACCESS_MODE_INVALID:
      err_posix = EACCES;
      break;
    case RTOS_ERR_SIZE_INVALID:
      err_posix = EINVAL;
      break;
    case RTOS_ERR_INVALID_STATE:
      err_posix = EPERM;
      break;
    case RTOS_ERR_INVALID_ARG:
      err_posix = EINVAL;
      break;
    case RTOS_ERR_ALLOC:
      err_posix = ENOMEM;
      break;
    case RTOS_ERR_FILE_ERR_STATE:
      err_posix = EIO;
      break;
    default:
      err_posix = ENOTDEF;
      break;
  }

  return (err_posix);
}

/****************************************************************************************************//**
 *                                               FSPosix_ErrSet()
 *
 * @brief    Store error in the errno task register.
 *
 * @param    err     Error code to be stored.
 *******************************************************************************************************/
static void FSPosix_ErrSet(RTOS_ERR err)
{
  RTOS_ERR err_tmp;

  RTOS_ERR_SET(err_tmp, RTOS_ERR_NONE);

  KAL_TaskRegSet(KAL_TaskHandleNull,
                 FSPosix_ErrnoTaskRegHandle,
                 (KAL_TASK_REG)RTOS_ERR_CODE_GET(err),
                 &err_tmp);

  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err_tmp) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}

/****************************************************************************************************//**
 *                                               FSPosix_ErrGet()
 *
 * @brief    Retrieve error from the errno task register.
 *
 * @return   Error code.
 *******************************************************************************************************/
static RTOS_ERR_CODE FSPosix_ErrGet(void)
{
  KAL_TASK_REG  task_reg_val;
  RTOS_ERR_CODE err_code;
  RTOS_ERR      err;

  RTOS_ERR_SET(err, RTOS_ERR_NONE);

  task_reg_val = KAL_TaskRegGet(KAL_TaskHandleNull,
                                FSPosix_ErrnoTaskRegHandle,
                                &err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, RTOS_ERR_ASSERT_CRITICAL_FAIL);

  err_code = (RTOS_ERR_CODE)task_reg_val;

  return (err_code);
}

/****************************************************************************************************//**
 *                                           FSPosix_PutCharWrapper()
 *
 * @brief    putchar() wrapper function.
 *
 * @param    c       Character to print.
 *
 * @param    p_arg   DEF_NULL
 *
 * @return   putchar()'s return value.
 *******************************************************************************************************/

#ifdef FS_CORE_CFG_POSIX_PUTCHAR
static int FSPosix_PutCharWrapper(int  c,
                                  void *p_arg)
{
  int ret;

  PP_UNUSED_PARAM(p_arg);

  ret = FS_CORE_CFG_POSIX_PUTCHAR(c);

  return (ret);
}
#endif

#endif // FS_CORE_CFG_POSIX_EN

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_FS_AVAIL
