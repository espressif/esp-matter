/***************************************************************************//**
 * @file
 * @brief File System Example
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                     DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include <rtos_description.h>

#if (defined(RTOS_MODULE_FS_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <ex_description.h>

#include  "ex_fs_entry_path.h"
#include  "ex_fs_utils.h"
#include  "ex_fs.h"
#include  <fs_core_cfg.h>

#include  <common/include/rtos_utils.h>
#include  <common/include/lib_str.h>

#include  <fs/include/fs_core_file.h>
#include  <fs/include/fs_core_dir.h>
#include  <fs/include/fs_core_working_dir.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL DEFINES
 *
 * Note(s) : (1) This configuration enables more messages displayed when executing the Entry Path example.
 *
 *               DEF_ENABLED     More messages displayed.
 *               DEF_DISABLED    Messages kept to a minimum.
 *
 *               For instance, when enabled, the full entry path will be displayed for each directory
 *               entry read with FSDir_Rd(). If the media read contains plenty of files and folders in the
 *               root directory, the output console displaying messages may be a bit flooded.
 *               By default, the configuration is disabled to keep messages to a minimum.
 ********************************************************************************************************
 *******************************************************************************************************/
//                                                                 See Note #1.
#define  EX_CFG_FS_ENTRY_PATH_MORE_INFO_EN          DEF_DISABLED

#define  EX_FS_ENTRY_PATH_DIR_QTY                   6u
#define  EX_FS_ENTRY_PATH_FILE_QTY                  3u
#define  EX_FS_ENTRY_PATH_MAX_PATH_LEN              100u
#define  EX_FS_MAX_ENTRY_NAME_LEN                   50u

/********************************************************************************************************
 *                                               LOGGING
 *
 * Note(s) : (1) This example outputs information to the console via the function printf() via a macro
 *               called EX_TRACE(). This can be modified or disabled if printf() is not supported.
 *******************************************************************************************************/

#ifndef EX_TRACE
#include  <stdio.h>
#define  EX_TRACE(...)              printf(__VA_ARGS__)
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                          LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                      LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void Ex_FS_EntryPath_Exec(FS_VOL_HANDLE vol_handle,
                                 RTOS_ERR      *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                     LOCAL CONFIGURATION ERRORS
 ********************************************************************************************************
 *******************************************************************************************************/

#if (FS_CORE_CFG_UTF8_EN != DEF_ENABLED)
#error  "FS_CORE_CFG_UTF8_EN illegally #define'd in 'fs_core_cfg.h'. MUST be DEF_ENABLED."
#endif

#if (FS_FAT_CFG_LFN_EN != DEF_ENABLED)
#error  "FS_FAT_CFG_LFN_EN illegally #define'd in 'fs_core_cfg.h'. MUST be DEF_ENABLED."
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                            Ex_FS_EntryPath()
 *
 * @brief  Perform file path example.
 *******************************************************************************************************/
void Ex_FS_EntryPath(void)
{
  FS_MEDIA_HANDLE media_handle;
  FS_VOL_HANDLE   vol_handle;
  RTOS_ERR        err;

  media_handle = FSMedia_Get(EX_CFG_FS_ACTIVE_MEDIA_NAME);      // Get media handle.
  APP_RTOS_ASSERT_CRITICAL(!FS_MEDIA_HANDLE_IS_NULL(media_handle),; );
  EX_TRACE("FS Example: File entry path on '%s'...", EX_CFG_FS_ACTIVE_MEDIA_NAME);

  //                                                               Open device and volume.
  vol_handle = Ex_FS_VolOpen(media_handle,
                             "vol3",                            // Unique volume name.
                             &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
  //                                                               Execute file path example.
  Ex_FS_EntryPath_Exec(vol_handle,
                       &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  Ex_FS_VolClose(vol_handle, &err);                             // Close volume and device.
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  EX_TRACE("OK\n\r");
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                         Ex_FS_EntryPath_Exec()
 *
 * @brief  This example will do the following main steps:
 *           - (a) Create a predefined directory/file tree structure.
 *           - (b) Read each entry (sub-directory or file) of each directory of the predefined tree
 *                 structure using absolute paths.
 *           - (c) Create a working directory associated to a specific directory of the predefined
 *                 tree structure. Read each entry of the directory pointed by the working
 *                 directory (see Note #1).
 *           - (d) Create a working directory pointing to the virtual root directory. Read each
 *                 entry of virtual root directory (see Note #2).
 *
 * @param  vol_handle  Volume handle.
 *
 * @param  p_err       Pointer to variable that will receive the return error code from this
 *                     function.
 *
 * @note   (1) When using working directories, any path specified with a working directory handle
 *             are considered as relative. As opposed to absolute paths which use a NULL working
 *             directory handle.
 *
 * @note   (2) Virtual root directory refers to the media name level. Let's assume for instance the
 *             following generic path to a directory:
 *             @verbatim
 *             Virtual root directory
 *                |
 *                v
 *             "nand0/<vol-name>/<root-dir-name>/<sub-dir-name>/<sub-dir-name>/<sub-dir-name>/"
 *             @endverbatim
 *             The virtual root directory here is "nand0". Thus when reading the virtual directory
 *             with FSDir_Rd(), the entry obtained will be a volume name. The volume must have been
 *             open prior to reading the virtual root directory. Otherwise no entry is available
 *             from the virtual root directory. If several volumes have been open on the same media
 *             because this one is partitioned for example, several calls to FSDir_Rd() will
 *             return all the open volumes.
 *******************************************************************************************************/
void Ex_FS_EntryPath_Exec(FS_VOL_HANDLE vol_handle,
                          RTOS_ERR      *p_err)
{
  CPU_CHAR          **p_dir_tbl;
  CPU_CHAR          *p_path_buf;
  MEM_SEG_INFO      seg_info;
  CPU_SIZE_T        seg_rem_size;
  CPU_CHAR          vol_name[FS_CORE_CFG_MAX_VOL_NAME_LEN];
  CPU_CHAR          file_name_buf[EX_FS_MAX_ENTRY_NAME_LEN];
  CPU_CHAR          file_ix_str[3u];
  CPU_CHAR          entry_name[EX_FS_MAX_ENTRY_NAME_LEN];
  CPU_INT08U        i;
  CPU_INT08U        j;
  CPU_INT08U        file_ix;
  FS_DIR_HANDLE     dir_handle;
  FS_FILE_HANDLE    file_handle;
  FS_WRK_DIR_HANDLE wrk_dir_handle = FS_WRK_DIR_NULL;
  FS_ENTRY_INFO     entry_info;
  CPU_BOOLEAN       eof;
  CPU_BOOLEAN       path_display = EX_CFG_FS_ENTRY_PATH_MORE_INFO_EN;
  RTOS_ERR          local_err;

  //                                                               Ensure enough space to allocate internal resources.
  seg_rem_size = Mem_SegRemSizeGet(&Ex_FS_MemSeg, sizeof(CPU_ALIGN), &seg_info, &local_err);
  APP_RTOS_ASSERT_CRITICAL((local_err.Code == RTOS_ERR_NONE),; );
  APP_RTOS_ASSERT_CRITICAL((seg_rem_size > (EX_FS_ENTRY_PATH_MAX_PATH_LEN + (EX_FS_ENTRY_PATH_DIR_QTY * sizeof(CPU_CHAR *)))),; );
  //                                                               ------------------ INITIALIZATION ------------------
  //                                                               Allocate buffer for file & directory paths.
  p_path_buf = (CPU_CHAR *)Mem_SegAlloc("Ex - FS entry path path buf",
                                        &Ex_FS_MemSeg,
                                        EX_FS_ENTRY_PATH_MAX_PATH_LEN,
                                        p_err);
  if (p_err->Code != RTOS_ERR_NONE) {
    EX_TRACE("Ex_FS_EntryPath_Exec(): Error allocating path buffer w/ err %d\r\n", p_err->Code);
    goto end_mem_seg;
  }
  //                                                               Allocate buffer for directory table.
  p_dir_tbl = (CPU_CHAR **)Mem_SegAlloc("Ex - FS entry path dir tbl",
                                        &Ex_FS_MemSeg,
                                        (EX_FS_ENTRY_PATH_DIR_QTY * sizeof(CPU_CHAR *)),
                                        p_err);
  if (p_err->Code != RTOS_ERR_NONE) {
    EX_TRACE("Ex_FS_EntryPath_Exec(): Error allocating directory table w/ err %d\r\n", p_err->Code);
    goto end_mem_seg;
  }
  //                                                               Get volume name associated to handle.
  FSVol_NameGet(vol_handle,
                vol_name,
                FS_CORE_CFG_MAX_VOL_NAME_LEN,
                p_err);
  if (p_err->Code != RTOS_ERR_NONE) {
    EX_TRACE("Ex_FS_EntryPath_Exec(): Error getting volume name w/ err %d\r\n", p_err->Code);
    goto end_mem_seg;
  }

  //                                                               Create a predefined directory/file tree structure...
  //                                                               ...These directory path segments are appended...
  //                                                               ...starting from the volume name.
  p_dir_tbl[0u] = DEF_NULL;                                     // Root directory.
  p_dir_tbl[1u] = "dir10";
  p_dir_tbl[2u] = "dir10/subdir0";
  p_dir_tbl[3u] = "dir11";
  p_dir_tbl[4u] = "dir11/subdir0-Ã§Ã©Ã Ã€Ã‡Ã«Ã´";
  p_dir_tbl[5u] = "dir11/subdir0-Ã§Ã©Ã Ã€Ã‡Ã«Ã´/subsubdir0";

  file_ix = 0u;
  for (i = 0u; i < EX_FS_ENTRY_PATH_DIR_QTY; i++) {
    //                                                             Build absolute path to directory to create.
    Mem_Clr((void *)p_path_buf, EX_FS_ENTRY_PATH_MAX_PATH_LEN);
    Str_Copy(p_path_buf, vol_name);
    Str_Cat(p_path_buf, "/");
    Str_Cat(p_path_buf, p_dir_tbl[i]);

    dir_handle = FSDir_Open(FS_WRK_DIR_NULL,                    // No working directory used.
                            p_path_buf,                         // Buffer with absolute directory path.
                            FS_DIR_ACCESS_MODE_CREATE,          // Directory created if it does not exist.
                            p_err);
    if (p_err->Code != RTOS_ERR_NONE) {
      EX_TRACE("Ex_FS_EntryPath_Exec(): Error opening directory w/ err %d\r\n", p_err->Code);
      goto end_mem_seg;
    }

    for (j = 0u; j < EX_FS_ENTRY_PATH_FILE_QTY; j++) {          // In each pre-defined directory, create a few files.
      Mem_Clr((void *)p_path_buf, EX_FS_ENTRY_PATH_MAX_PATH_LEN);
      Mem_Clr((void *)file_name_buf, EX_FS_MAX_ENTRY_NAME_LEN);

      //                                                           Create file name with format file<XX>.txt (XX = nbr).
      Str_FmtNbr_Int32U(file_ix,
                        2u,
                        DEF_NBR_BASE_DEC,
                        '\0',                                   // Do not prepend lead character to string.
                        DEF_NO,
                        DEF_YES,                                // Append NULL character to end of string.
                        &file_ix_str[0]);
      //                                                           Build absolute file path.
      Str_Copy(p_path_buf, vol_name);
      Str_Cat(p_path_buf, "/");
      Str_Cat(p_path_buf, p_dir_tbl[i]);
      Str_Cat(p_path_buf, "/file");
      Str_Cat(p_path_buf, file_ix_str);
      Str_Cat(p_path_buf, ".txt");
      //                                                           Create empty file.
      file_handle = FSFile_Open(FS_WRK_DIR_NULL,                // NULL working directory means absolute path used.
                                p_path_buf,                     // Buffer containing full path to file.
                                FS_FILE_ACCESS_MODE_WR              // File can be read or written.
                                | FS_FILE_ACCESS_MODE_CREATE        // File created if it does not exist.
                                | FS_FILE_ACCESS_MODE_TRUNCATE,     // If file exists, its size is truncated to 0 bytes.
                                p_err);
      if (p_err->Code != RTOS_ERR_NONE) {
        EX_TRACE("Ex_FS_EntryPath_Exec(): Error opening file w/ err %d\r\n", p_err->Code);
        goto end_dir;
      }

      FSFile_Close(file_handle, &local_err);
      APP_RTOS_ASSERT_CRITICAL(local_err.Code == RTOS_ERR_NONE,; );
      file_ix++;
    }

    FSDir_Close(dir_handle, &local_err);
    APP_RTOS_ASSERT_CRITICAL(local_err.Code == RTOS_ERR_NONE,; );
  }
  //                                                               Create some files with special characters in the name
  Mem_Clr((void *)p_path_buf, EX_FS_ENTRY_PATH_MAX_PATH_LEN);
  Str_Copy(p_path_buf, vol_name);
  Str_Cat(p_path_buf, "/dir10/file-Ã§Ã Ã¨Ã©Ã‡Ã€ÃˆÃ‰.txt");       // File name with UTF-8 encoding.

  //                                                               Create empty file.
  file_handle = FSFile_Open(FS_WRK_DIR_NULL,                    // NULL working directory means absolute path used.
                            p_path_buf,                         // Buffer containing full path to file.
                            FS_FILE_ACCESS_MODE_WR              // File can be read or written.
                            | FS_FILE_ACCESS_MODE_CREATE        // File created if it does not exist.
                            | FS_FILE_ACCESS_MODE_TRUNCATE,     // If file exists, its size is truncated to 0 bytes.
                            p_err);
  if (p_err->Code != RTOS_ERR_NONE) {
    EX_TRACE("Ex_FS_EntryPath_Exec(): Error opening file w/ err %d\r\n", p_err->Code);
    goto end_mem_seg;
  }

  FSFile_Close(file_handle, &local_err);
  APP_RTOS_ASSERT_CRITICAL(local_err.Code == RTOS_ERR_NONE,; );

  Mem_Clr((void *)p_path_buf, EX_FS_ENTRY_PATH_MAX_PATH_LEN);
  Str_Copy(p_path_buf, vol_name);
  Str_Cat(p_path_buf, "/dir11/subdir0-Ã§Ã©Ã Ã€Ã‡Ã«Ã´/file-Ã¯Ã„Ã»Â¿.txt");   // File name with UTF-8 encoding.

  //                                                               Create empty file.
  file_handle = FSFile_Open(FS_WRK_DIR_NULL,                    // NULL working directory means absolute path used.
                            p_path_buf,                         // Buffer containing full path to file.
                            FS_FILE_ACCESS_MODE_WR              // File can be read or written.
                            | FS_FILE_ACCESS_MODE_CREATE        // File created if it does not exist.
                            | FS_FILE_ACCESS_MODE_TRUNCATE,     // If file exists, its size is truncated to 0 bytes.
                            p_err);
  if (p_err->Code != RTOS_ERR_NONE) {
    EX_TRACE("Ex_FS_EntryPath_Exec(): Error opening file w/ err %d\r\n", p_err->Code);
    goto end_mem_seg;
  }

  FSFile_Close(file_handle, &local_err);
  APP_RTOS_ASSERT_CRITICAL(local_err.Code == RTOS_ERR_NONE,; );

  //                                                               ---------------- EXAMPLE EXECUTION -----------------
  //                                                               1) ABSOLUTE PATH.
  //                                                               Browse the predefined tree structure.
  EX_TRACE("\r\n");
  EX_TRACE("Absolute path used.\r\n");

  file_ix = 0u;
  for (i = 0u; i < EX_FS_ENTRY_PATH_DIR_QTY; i++) {
    //                                                             Recreate full path to directory.
    Mem_Clr((void *)p_path_buf, EX_FS_ENTRY_PATH_MAX_PATH_LEN);
    Str_Copy(p_path_buf, vol_name);
    Str_Cat(p_path_buf, "/");
    Str_Cat(p_path_buf, p_dir_tbl[i]);
    //                                                             Open an existing directory.
    dir_handle = FSDir_Open(FS_WRK_DIR_NULL,                    // NULL working directory means absolute path used.
                            p_path_buf,                         // Buffer containing full path to directory.
                            FS_DIR_ACCESS_MODE_NONE,            // Directory must exist.
                            p_err);
    if (p_err->Code != RTOS_ERR_NONE) {
      EX_TRACE("Ex_FS_EntryPath_Exec(): Error opening directory w/ err %d\r\n", p_err->Code);
      goto end_mem_seg;
    }
    //                                                             Retrieve path associated to directory handle.
    FSDir_PathGet(dir_handle,                                   // Handle associated to open directory.
                  p_path_buf,                                   // Buffer receiving the path.
                  EX_FS_ENTRY_PATH_MAX_PATH_LEN,                // Buffer size in bytes.
                  p_err);
    if (p_err->Code != RTOS_ERR_NONE) {
      EX_TRACE("Ex_FS_EntryPath_Exec(): Error getting directory path w/ err %d\r\n", p_err->Code);
      goto end_dir;
    }

    eof = DEF_NO;
    //                                                             Read all entries in current directory until result...
    while (!eof) {                                              // ...indicates no more entry.
                                                                // Read 1 entry.
      eof = FSDir_Rd(dir_handle,                                // Handle associated to open directory.
                     &entry_info,                               // Structure receiving info about entry.
                     entry_name,                                // Buffer receiving entry name (directory or file).
                     EX_FS_MAX_ENTRY_NAME_LEN,                  // Buffer size in bytes.
                     p_err);
      if (p_err->Code != RTOS_ERR_NONE) {
        EX_TRACE("Ex_FS_EntryPath_Exec(): Error reading directory content w/ err %d\r\n", p_err->Code);
        goto end_dir;
      }
      if (!eof && path_display) {                               // Entry found, display full path.
        EX_TRACE("Full entry path: '%s%s%s'\r\n", p_path_buf, "/", entry_name);
      }
    }

    FSDir_Close(dir_handle, &local_err);
    APP_RTOS_ASSERT_CRITICAL(local_err.Code == RTOS_ERR_NONE,; );
  }
  //                                                               2) RELATIVE PATH USING WORKING DIRECTORY.
  EX_TRACE("Relative path used with working directory.\r\n");
  //                                                               Browse a sub-directory using working directory.
  //                                                               Create full path to sub-directory.
  Mem_Clr((void *)p_path_buf, EX_FS_ENTRY_PATH_MAX_PATH_LEN);
  Str_Copy(p_path_buf, vol_name);
  Str_Cat(p_path_buf, "/dir11/subdir0-Ã§Ã©Ã Ã€Ã‡Ã«Ã´");
  //                                                               Open a working directory to specified sub-directory.
  wrk_dir_handle = FSWrkDir_Open(FS_WRK_DIR_NULL,               // NULL working directory means absolute path used.
                                 p_path_buf,                    // Buffer containing full path to sub-directory.
                                 p_err);
  if (p_err->Code != RTOS_ERR_NONE) {
    EX_TRACE("Ex_FS_EntryPath_Exec(): Error opening working directory w/ err %d\r\n", p_err->Code);
    goto end_mem_seg;
  }
  //                                                               Open an existing directory.
  dir_handle = FSDir_Open(wrk_dir_handle,                       // Non-null working directory means relative path used.
                          DEF_NULL,                             // NULL path = directory pointed by working directory.
                          FS_DIR_ACCESS_MODE_NONE,              // Directory must exist.
                          p_err);
  if (p_err->Code != RTOS_ERR_NONE) {
    EX_TRACE("Ex_FS_EntryPath_Exec(): Error opening directory w/ err %d\r\n", p_err->Code);
    goto end_wrk_dir;
  }
  //                                                               Retrieve path associated to directory handle.
  FSDir_PathGet(dir_handle,                                     // Handle associated to open directory.
                p_path_buf,                                     // Buffer receiving the path.
                EX_FS_ENTRY_PATH_MAX_PATH_LEN,                  // Buffer size in bytes.
                p_err);
  if (p_err->Code != RTOS_ERR_NONE) {
    EX_TRACE("Ex_FS_EntryPath_Exec(): Error getting directory path w/ err %d\r\n", p_err->Code);
    goto end_dir;
  }

  eof = DEF_NO;
  //                                                               Read all entries in directory pointed by working...
  while (!eof) {                                                // ...directory until result indicates no more entry.
                                                                // Read 1 entry.
    eof = FSDir_Rd(dir_handle,                                  // Handle associated to open directory.
                   &entry_info,                                 // Structure receiving info about entry.
                   entry_name,                                  // Buffer receiving entry name (directory or file).
                   EX_FS_MAX_ENTRY_NAME_LEN,                    // Buffer size in bytes.
                   p_err);
    if (p_err->Code != RTOS_ERR_NONE) {
      EX_TRACE("Ex_FS_EntryPath_Exec(): Error reading directory content w/ err %d\r\n", p_err->Code);
      goto end_dir;
    }
    if (!eof && path_display) {                                 // Entry found, display full path.
      EX_TRACE("Full entry path: '%s%s%s'\r\n", p_path_buf, "/", entry_name);
    }
  }

  FSDir_Close(dir_handle, &local_err);
  APP_RTOS_ASSERT_CRITICAL(local_err.Code == RTOS_ERR_NONE,; );
  FSWrkDir_Close(wrk_dir_handle, &local_err);
  APP_RTOS_ASSERT_CRITICAL(local_err.Code == RTOS_ERR_NONE,; );

  //                                                               3) VIRTUAL ROOT USING WORKING DIRECTORY.
  EX_TRACE("Virtual root used with working directory.\r\n");    // See Note #2.

  //                                                               Open a working directory pointing to block device.
  wrk_dir_handle = FSWrkDir_Open(FS_WRK_DIR_NULL,               // NULL working directory means absolute path used.
                                 "../../../",                   // Go 3 levels up from the root directory.
                                 p_err);
  if (p_err->Code != RTOS_ERR_NONE) {
    EX_TRACE("Ex_FS_EntryPath_Exec(): Error opening working directory w/ err %d\r\n", p_err->Code);
    goto end_mem_seg;
  }
  //                                                               Open a virtual directory = block device level.
  dir_handle = FSDir_Open(wrk_dir_handle,                       // Non-null working directory means relative path used.
                          DEF_NULL,                             // NULL path = virtual directory pointed by working...
                                                                // ...directory.
                          FS_DIR_ACCESS_MODE_NONE,              // Directory must exist.
                          p_err);
  if (p_err->Code != RTOS_ERR_NONE) {
    EX_TRACE("Ex_FS_EntryPath_Exec(): Error opening directory w/ err %d\r\n", p_err->Code);
    goto end_wrk_dir;
  }

  eof = DEF_NO;
  //                                                               Read all entries in virtual root until result...
  while (!eof) {                                                // ...indicates no more entry.
                                                                // Read 1 entry.
    eof = FSDir_Rd(dir_handle,                                  // Handle associated to open virtual directory.
                   &entry_info,                                 // Structure receiving info about entry.
                   entry_name,                                  // Buffer receiving entry name (here open volume name).
                   EX_FS_MAX_ENTRY_NAME_LEN,                    // Buffer size in bytes.
                   p_err);
    if (p_err->Code != RTOS_ERR_NONE) {
      EX_TRACE("Ex_FS_EntryPath_Exec(): Error reading directory content w/ err %d\r\n", p_err->Code);
      goto end_dir;
    }
    if (!eof && path_display) {                                 // Entry found, display full path.
      EX_TRACE("Entry is virtual root: '%s'\r\n", entry_name);
    }
  }

end_dir:
  FSDir_Close(dir_handle, &local_err);
  APP_RTOS_ASSERT_CRITICAL(local_err.Code == RTOS_ERR_NONE,; );
end_wrk_dir:
  FSWrkDir_Close(wrk_dir_handle, &local_err);
  APP_RTOS_ASSERT_CRITICAL(local_err.Code == RTOS_ERR_NONE,; );
  //                                                               --------------------- CLOSING ----------------------
end_mem_seg:
  Mem_SegClr(&Ex_FS_MemSeg, &local_err);                        // Clear memory segment for next example.
  APP_RTOS_ASSERT_CRITICAL(local_err.Code == RTOS_ERR_NONE,; );
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_FS_AVAIL
