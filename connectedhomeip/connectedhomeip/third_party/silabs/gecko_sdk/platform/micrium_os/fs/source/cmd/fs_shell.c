/***************************************************************************//**
 * @file
 * @brief File System Shell Commands
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

#if (defined(RTOS_MODULE_FS_AVAIL) \
  && defined(RTOS_MODULE_COMMON_SHELL_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <fs/include/fs_core_partition.h>
#include  <fs/include/fs_core_dir.h>
#include  <fs/include/fs_core_entry.h>
#include  <fs/include/fs_core_file.h>
#include  <fs/include/fs_core_vol.h>
#include  <fs/include/fs_core_working_dir.h>
#include  <fs/include/fs_fat.h>
#include  <fs/source/core/fs_core_vol_priv.h>
#include  <fs/source/storage/fs_blk_dev_priv.h>

#include  <common/include/rtos_utils.h>
#include  <common/include/lib_utils.h>
#include  <common/include/lib_str.h>
#include  <common/include/shell.h>

#include  <common/source/preprocessor/preprocessor_priv.h>
#include  <common/source/logging/logging_priv.h>
#include  <common/source/rtos/rtos_utils_priv.h>

#include  <sl_sleeptimer.h>
#include  <sl_status.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  RTOS_MODULE_CUR                        RTOS_CFG_MODULE_FS
#define  LOG_DFLT_CH                            (FS, SHELL)

#define  FS_SHELL_STR_HELP                      (CPU_CHAR *)"-h"
#define  FS_SHELL_GLOBAL_BUF_MAX_SIZE           512u
#define  FS_SHELL_LINE_BUF_MAX_SIZE             500u
#define  FS_SHELL_MEDIA_NAME_MAX_LEN            30u

#define  UNIX_EPOCH_YR_START                    1970u                           ///< Unix epoch starts = 1970-01-01 00:00:00 UTC.
#define  UNIX_EPOCH_YR_END                      2106u                           ///<            ends   = 2105-12-31 23:59:59 UTC.

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef struct fs_shell_output_func_cb_data {
  SHELL_OUT_FNCT ShellOutFunc;
  void           *OutOptPtr;
} FS_SHELL_OUTPUT_FUNC_CB_DATA;

typedef struct fs_shell_data {
  CPU_INT08U        *BufPtr;
  CPU_CHAR          *LineBufPtr;
  CPU_SIZE_T        LineBufCurIx;
  FS_WRK_DIR_HANDLE WorkingDirHandle;
} FS_SHELL_DATA;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static FS_SHELL_DATA FSShell_Data = {
  .BufPtr = DEF_NULL,
  .LineBufPtr = DEF_NULL,
  .LineBufCurIx = 0u,
  .WorkingDirHandle = { DEF_NULL, 0u }
};

#if (FS_CORE_CFG_DIR_EN == DEF_ENABLED)
static const CPU_CHAR *FSShell_Month_Name[] = {
  "jan", "feb", "mar", "apr", "may", "jun",
  "jul", "aug", "sep", "oct", "nov", "dec"
};
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL MACROS
 ********************************************************************************************************
 *******************************************************************************************************/

#define  FS_SHELL_PRINTF(out_func, out_func_arg, ...)                             \
  {                                                                               \
    FS_SHELL_OUTPUT_FUNC_CB_DATA _out_cb_data = { out_func, out_func_arg };       \
    (void)Str_Printf(FSShell_PrintfOutputCb, (void *)&_out_cb_data, __VA_ARGS__); \
    FSShell_PrintfOutputCb('\0', (void *)&_out_cb_data);                          \
  }

#if (RTOS_CFG_ERR_STR_EN == DEF_ENABLED)
#define  FS_SHELL_ERR_PRINT(out_func, out_func_arg, err)                                                                                    \
  {                                                                                                                                         \
    FS_SHELL_OUTPUT_FUNC_CB_DATA _out_cb_data = { out_func, out_func_arg };                                                                 \
    (void)Str_Printf(FSShell_PrintfOutputCb, (void *)&_out_cb_data, "Failed with error %s.\r\n", RTOS_ERR_STR_GET(RTOS_ERR_CODE_GET(err))); \
    FSShell_PrintfOutputCb('\0', (void *)&_out_cb_data);                                                                                    \
  }
#else
#define  FS_SHELL_ERR_PRINT(out_func, out_func_arg, err)                                               \
  {                                                                                                    \
    FS_SHELL_OUTPUT_FUNC_CB_DATA _out_cb_data = { out_func, out_func_arg };                            \
    (void)Str_Printf(FSShell_PrintfOutputCb, (void *)&_out_cb_data, "Failed with error %u.\r\n", err); \
    FSShell_PrintfOutputCb('\0', (void *)&_out_cb_data);                                               \
  }
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static CPU_INT16S FSShell_EntryRemove(CPU_INT16U      argc,
                                      CPU_CHAR        *argv[],
                                      SHELL_OUT_FNCT  out_fnct,
                                      SHELL_CMD_PARAM *p_cmd_param,
                                      FS_FLAGS        entry_type);
#endif

static void FSShell_VolInfoDisplay(FS_VOL_HANDLE   vol_handle,
                                   SHELL_CMD_PARAM *p_cmd_param,
                                   SHELL_OUT_FNCT  out_fnct,
                                   RTOS_ERR        *p_err);

static int FSShell_PrintfOutputCb(int  c,
                                  void *p_arg);

/********************************************************************************************************
 *                                           SHELL COMMANDS
 *******************************************************************************************************/

static CPU_INT16S FSShell_cat(CPU_INT16U      argc,
                              CPU_CHAR        *argv[],
                              SHELL_OUT_FNCT  out_fnct,
                              SHELL_CMD_PARAM *p_cmd_param);

static CPU_INT16S FSShell_cd(CPU_INT16U      argc,
                             CPU_CHAR        *argv[],
                             SHELL_OUT_FNCT  out_fnct,
                             SHELL_CMD_PARAM *p_cmd_param);

#if ((FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED) \
  && (FS_CORE_CFG_FILE_COPY_EN == DEF_ENABLED))
static CPU_INT16S FSShell_cp(CPU_INT16U      argc,
                             CPU_CHAR        *argv[],
                             SHELL_OUT_FNCT  out_fnct,
                             SHELL_CMD_PARAM *p_cmd_param);
#endif

static CPU_INT16S FSShell_date(CPU_INT16U      argc,
                               CPU_CHAR        *argv[],
                               SHELL_OUT_FNCT  out_fnct,
                               SHELL_CMD_PARAM *p_cmd_param);

static CPU_INT16S FSShell_df(CPU_INT16U      argc,
                             CPU_CHAR        *argv[],
                             SHELL_OUT_FNCT  out_fnct,
                             SHELL_CMD_PARAM *p_cmd_param);

static CPU_INT16S FSShell_lsblk(CPU_INT16U      argc,
                                CPU_CHAR        *argv[],
                                SHELL_OUT_FNCT  out_fnct,
                                SHELL_CMD_PARAM *p_cmd_param);

static CPU_INT16S FSShell_ls(CPU_INT16U      argc,
                             CPU_CHAR        *argv[],
                             SHELL_OUT_FNCT  out_fnct,
                             SHELL_CMD_PARAM *p_cmd_param);

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static CPU_INT16S FSShell_mkdir(CPU_INT16U      argc,
                                CPU_CHAR        *argv[],
                                SHELL_OUT_FNCT  out_fnct,
                                SHELL_CMD_PARAM *p_cmd_param);

static CPU_INT16S FSShell_mkfs(CPU_INT16U      argc,
                               CPU_CHAR        *argv[],
                               SHELL_OUT_FNCT  out_fnct,
                               SHELL_CMD_PARAM *p_cmd_param);
#endif

static CPU_INT16S FSShell_mount(CPU_INT16U      argc,
                                CPU_CHAR        *argv[],
                                SHELL_OUT_FNCT  out_fnct,
                                SHELL_CMD_PARAM *p_cmd_param);

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static CPU_INT16S FSShell_mv(CPU_INT16U      argc,
                             CPU_CHAR        *argv[],
                             SHELL_OUT_FNCT  out_fnct,
                             SHELL_CMD_PARAM *p_cmd_param);
#endif

static CPU_INT16S FSShell_od(CPU_INT16U      argc,
                             CPU_CHAR        *argv[],
                             SHELL_OUT_FNCT  out_fnct,
                             SHELL_CMD_PARAM *p_cmd_param);

static CPU_INT16S FSShell_pwd(CPU_INT16U      argc,
                              CPU_CHAR        *argv[],
                              SHELL_OUT_FNCT  out_fnct,
                              SHELL_CMD_PARAM *p_cmd_param);

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static CPU_INT16S FSShell_rm(CPU_INT16U      argc,
                             CPU_CHAR        *argv[],
                             SHELL_OUT_FNCT  out_fnct,
                             SHELL_CMD_PARAM *p_cmd_param);

static CPU_INT16S FSShell_rmdir(CPU_INT16U      argc,
                                CPU_CHAR        *argv[],
                                SHELL_OUT_FNCT  out_fnct,
                                SHELL_CMD_PARAM *p_cmd_param);

static CPU_INT16S FSShell_touch(CPU_INT16U      argc,
                                CPU_CHAR        *argv[],
                                SHELL_OUT_FNCT  out_fnct,
                                SHELL_CMD_PARAM *p_cmd_param);
#endif

static CPU_INT16S FSShell_umount(CPU_INT16U      argc,
                                 CPU_CHAR        *argv[],
                                 SHELL_OUT_FNCT  out_fnct,
                                 SHELL_CMD_PARAM *p_cmd_param);

static CPU_INT16S FSShell_wc(CPU_INT16U      argc,
                             CPU_CHAR        *argv[],
                             SHELL_OUT_FNCT  out_fnct,
                             SHELL_CMD_PARAM *p_cmd_param);

/********************************************************************************************************
 *                                           SHELL COMMAND TABLE
 *******************************************************************************************************/

static SHELL_CMD FSShell_CmdTbl[] = {
  { "fs_cat", FSShell_cat    },

  { "fs_cd", FSShell_cd     },

#if ((FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED) \
  && (FS_CORE_CFG_FILE_COPY_EN == DEF_ENABLED))
  { "fs_cp", FSShell_cp     },
#endif

  { "fs_date", FSShell_date   },

  { "fs_df", FSShell_df     },

  { "fs_lsblk", FSShell_lsblk     },

  { "fs_ls", FSShell_ls     },

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
  { "fs_mkdir", FSShell_mkdir  },

  { "fs_mkfs", FSShell_mkfs   },
#endif

  { "fs_mount", FSShell_mount  },

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
  { "fs_mv", FSShell_mv     },
#endif

  { "fs_od", FSShell_od     },

  { "fs_pwd", FSShell_pwd    },

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
  { "fs_rm", FSShell_rm     },

  { "fs_rmdir", FSShell_rmdir  },

  { "fs_touch", FSShell_touch  },
#endif

  { "fs_umount", FSShell_umount },

  { "fs_wc", FSShell_wc     },

  { 0, 0              }
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               FSShell_Init()
 *
 * @brief    Initialize Shell for FS.
 *
 * @return   DEF_OK,   if file system shell commands were added.
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
void FSShell_Init(RTOS_ERR *p_err)
{
  FSShell_Data.BufPtr = (CPU_INT08U *)Mem_SegAlloc("FS - Shell cmd buffer",
                                                   FSCore_InitCfg.MemSegPtr,
                                                   (FS_SHELL_GLOBAL_BUF_MAX_SIZE + 1u),
                                                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  FSShell_Data.LineBufPtr = (CPU_CHAR *)Mem_SegAlloc("FS - Shell line buffer",
                                                     FSCore_InitCfg.MemSegPtr,
                                                     FS_SHELL_LINE_BUF_MAX_SIZE,
                                                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  Shell_CmdTblAdd((CPU_CHAR *)"fs", FSShell_CmdTbl, p_err);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               FSShell_cat()
 *
 * @brief    Print a file to the terminal output.
 *
 * @param    argc            The number of arguments.
 *
 * @param    argv            Array of arguments.
 *
 * @param    out_fnct        The output function.
 *
 * @param    p_cmd_param     Pointer to the command parameters.
 *
 * @return   RTOS_ERR_NONE,  if NO error(s).
 *           SHELL_EXEC_ERR, otherwise.
 *
 * @note     (1) Usage(s)    : fs_cat [file]
 *               - (a) Argument(s) : file      Path of file to print to terminal output.
 *               - (b) Output      : File contents, in the ASCII character set.  Non-printable/non-space
 *                                   characters are transmitted as full stops ("periods", character code
 *                                   46).  For a more convenient display of binary files use 'fs_od'.
 *******************************************************************************************************/
static CPU_INT16S FSShell_cat(CPU_INT16U      argc,
                              CPU_CHAR        *argv[],
                              SHELL_OUT_FNCT  out_fnct,
                              SHELL_CMD_PARAM *p_cmd_param)
{
  CPU_CHAR       *p_path;
  void           *p_opt;
  FS_FILE_HANDLE file_handle;
  CPU_SIZE_T     file_rd_len;
  CPU_SIZE_T     i;
  RTOS_ERR       err;

  RTOS_ERR_SET(err, RTOS_ERR_NONE);
  p_opt = p_cmd_param->OutputOptPtr;

  //                                                               ------------------ CHK ARGUMENTS -------------------
  if (argc != 2u) {
    FS_SHELL_PRINTF(out_fnct, p_opt, "fs_cat: usage: fs_cat [file]");
    return (SHELL_EXEC_ERR);
  }

  if (Str_Cmp_N(argv[1], FS_SHELL_STR_HELP, 3u) == 0) {
    FS_SHELL_PRINTF(out_fnct, p_opt, "fs_cat: usage: fs_cat [file]\r\n");
    FS_SHELL_PRINTF(out_fnct, p_opt, "               Print [file] contents to terminal output.\r\n");
    return (SHELL_EXEC_ERR_NONE);
  }

  p_path = argv[1];

  file_handle = FSFile_Open(FSShell_Data.WorkingDirHandle,
                            p_path,
                            FS_FILE_ACCESS_MODE_RD,
                            &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    FS_SHELL_ERR_PRINT(out_fnct, p_opt, err);
    return (SHELL_EXEC_ERR);
  }

  //                                                               -------------------- PRINT FILE --------------------
  do {
    file_rd_len = FSFile_Rd(file_handle,
                            FSShell_Data.BufPtr,
                            FS_SHELL_GLOBAL_BUF_MAX_SIZE,
                            &err);
    if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
      FS_SHELL_ERR_PRINT(out_fnct, p_opt, err);
      return (SHELL_EXEC_ERR);
    }

    FSShell_Data.BufPtr[file_rd_len] = '\0';
    if (file_rd_len > 0u) {
      for (i = 0u; i < file_rd_len; i++) {
        if (!(ASCII_IS_PRINT(FSShell_Data.BufPtr[i]))
            && !(ASCII_IS_SPACE(FSShell_Data.BufPtr[i]))) {
          FSShell_Data.BufPtr[i] = (CPU_CHAR)ASCII_CHAR_FULL_STOP;
        }
      }
      (void)out_fnct((CPU_CHAR *)FSShell_Data.BufPtr,
                     file_rd_len,
                     p_opt);
    }
  } while (file_rd_len > 0u);
  //                                                               ------------------- CLOSE & RTN --------------------
  FSFile_Close(file_handle, &err);                              // Close src file.
  FS_SHELL_PRINTF(out_fnct, p_opt, "\r\n\r\n");
  return (RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                               FSShell_cd()
 *
 * @brief    Change the working directory.
 *
 * @param    argc            The number of arguments.
 *
 * @param    argv            Array of arguments.
 *
 * @param    out_fnct        The output function.
 *
 * @param    p_cmd_param     Pointer to the command parameters.
 *
 * @return   RTOS_ERR_NONE,  if NO error(s).
 *           SHELL_EXEC_ERR, otherwise.
 *
 * @note     (1) Usage(s)    : fs_cd [dir]
 *           - (a) Argument(s) : dir       Absolute directory path.
 *                                               OR
 *                                         Path relative to current working directory.
 *           - (b) Output      : none.
 *
 * @note     (2) The new working directory is formed in three steps :
 *           - (a) If 'dir' begins with the path separator character (slash, '\'), it will
 *                 be interpreted as an absolute directory path on the current volume.  The
 *                 preliminary working directory path is formed by the concatenation of the
 *                 current volume name & 'dir'.
 *               - (1) Otherwise, if 'dir' begins with a volume name, it will be interpreted
 *                     as an absolute directory path & will become the preliminary working directory.
 *               - (2) Otherwise, the preliminary working directory path is formed by the concatenation
 *                     of the current working directory, a path separator character & 'dir'.
 *           - (b) The preliminary working directory is then resolved, from the first to last path
 *                 component :
 *               - (1) If the component is a 'dot' component, it is removed.
 *               - (2) If the component is a 'dot dot' component, & the preliminary working
 *                     directory path is not a root directory, the previous path component is
 *                     removed.  In any case, the 'dot dot' component is removed.
 *               - (3) Trailing path separator characters are removed, & multiple consecutive path
 *                     separator characters are replaced by a single path separator character.
 *           - (c) The volume is examined to determine whether the preliminary working directory
 *                 exists.  If it does, it becomes the new working directory.  Otherwise, an error
 *                 is output, & the working directory is unchanged.
 *******************************************************************************************************/
static CPU_INT16S FSShell_cd(CPU_INT16U      argc,
                             CPU_CHAR        *argv[],
                             SHELL_OUT_FNCT  out_fnct,
                             SHELL_CMD_PARAM *p_cmd_param)
{
  CPU_CHAR          *p_path;
  void              *p_opt;
  FS_WRK_DIR_HANDLE next_wrk_dir_handle;
  RTOS_ERR          err;

  RTOS_ERR_SET(err, RTOS_ERR_NONE);
  p_opt = p_cmd_param->OutputOptPtr;

  //                                                               ------------------ CHK ARGUMENTS -------------------
  if (argc != 2u) {
    FS_SHELL_PRINTF(out_fnct, p_opt, "fs_cd: usage: fs_cd [dir]\r\n");
    return (SHELL_EXEC_ERR);
  }

  if (Str_Cmp_N(argv[1], FS_SHELL_STR_HELP, 3u) == 0) {
    FS_SHELL_PRINTF(out_fnct, p_opt, "fs_cd: usage: fs_cd [dir]\r\n");
    FS_SHELL_PRINTF(out_fnct, p_opt, "              Change the working directory to [dir].\r\n");
    return (SHELL_EXEC_ERR_NONE);
  }

  p_path = argv[1];

  if ((*p_path == '\\') || (*p_path == '/')) {
    FSShell_Data.WorkingDirHandle = FSWrkDir_NullHandle;
  }

  next_wrk_dir_handle = FSWrkDir_Open(FSShell_Data.WorkingDirHandle,
                                      p_path,
                                      &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    FS_SHELL_ERR_PRINT(out_fnct, p_opt, err);
    return (SHELL_EXEC_ERR);
  }

  if (!FS_WRK_DIR_HANDLE_IS_NULL(FSShell_Data.WorkingDirHandle)) {
    FSWrkDir_Close(FSShell_Data.WorkingDirHandle,
                   &err);
    if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
      FS_SHELL_ERR_PRINT(out_fnct, p_opt, err);
      return (SHELL_EXEC_ERR);
    }
  }

  FSShell_Data.WorkingDirHandle = next_wrk_dir_handle;

  return (SHELL_EXEC_ERR_NONE);
}

/****************************************************************************************************//**
 *                                               FSShell_cp()
 *
 * @brief    Copy a file.
 *
 * @param    argc            The number of arguments.
 *
 * @param    argv            Array of arguments.
 *
 * @param    out_fnct        The output function.
 *
 * @param    p_cmd_param     Pointer to the command parameters.
 *
 * @return   RTOS_ERR_NONE,  if NO error(s).
 *           SHELL_EXEC_ERR, otherwise.
 *
 * @note     (1) Usage(s)    : fs_cp [source_file] [dest_file]
 *               - (a) Argument(s) : source_file   Source file path.
 *                                   dest_file     Destination file path.
 *                                   dest_dir      Destination directory path.
 *               - (b) Output      : none.
 *
 * @note     (2) In the first form of this command, neither argument may be an existing directory.
 *               The contents of 'source_file' will be copied to a file named 'dest_file' located
 *               in the same directory as 'source_file'.
 *               - (a) In the second form of this command, the first argument must not be an existing
 *                     directory and the second argument must be an existing directory.  The contents of
 *                     'source_file' will be copied to a file with name formed by concatenating
 *                     'dest_dir', a path separator character and the final component of 'source_file'.
 *******************************************************************************************************/

#if ((FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED) \
  && (FS_CORE_CFG_FILE_COPY_EN == DEF_ENABLED))
static CPU_INT16S FSShell_cp(CPU_INT16U      argc,
                             CPU_CHAR        *argv[],
                             SHELL_OUT_FNCT  out_fnct,
                             SHELL_CMD_PARAM *p_cmd_param)
{
  CPU_CHAR *p_path_src;
  CPU_CHAR *p_path_dest;
  void     *p_opt;
  RTOS_ERR err;

  RTOS_ERR_SET(err, RTOS_ERR_NONE);
  p_opt = p_cmd_param->OutputOptPtr;

  //                                                               ------------------ CHK ARGUMENTS -------------------
  if (argc == 2u) {
    if (Str_Cmp_N(argv[1], FS_SHELL_STR_HELP, 3u) == 0) {
      FS_SHELL_PRINTF(out_fnct, p_opt, "fs_cp: usage: fs_cp [source_file] [dest_file]\r\n");
      FS_SHELL_PRINTF(out_fnct, p_opt, "              fs_cp [source_file] [dest_dir]\r\n");
      FS_SHELL_PRINTF(out_fnct, p_opt, "              Copy [source_file] to [dest_file] or copy [source_file] into [dest_dir].");
      return (RTOS_ERR_NONE);
    }
  }

  if (argc != 3u) {
    FS_SHELL_PRINTF(out_fnct, p_opt, "fs_cp: usage: fs_cp [source_file] [dest_file]\r\n");
    FS_SHELL_PRINTF(out_fnct, p_opt, "              fs_cp [source_file] [dest_dir]\r\n");
    return (SHELL_EXEC_ERR);
  }

  p_path_src = argv[1];
  p_path_dest = argv[2];

  FSFile_Copy(FSShell_Data.WorkingDirHandle,
              p_path_src,
              FSShell_Data.WorkingDirHandle,
              p_path_dest,
              DEF_YES,
              &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    FS_SHELL_ERR_PRINT(out_fnct, p_opt, err);
    return (SHELL_EXEC_ERR);
  }

  return (SHELL_EXEC_ERR_NONE);
}
#endif

/****************************************************************************************************//**
 *                                               FSShell_date()
 *
 * @brief    Write the date & time to terminal output, or set the system date & time.
 *
 * @param    argc            The number of arguments.
 *
 * @param    argv            Array of arguments.
 *
 * @param    out_fnct        The output function.
 *
 * @param    p_cmd_param     Pointer to the command parameters.
 *
 * @return   RTOS_ERR_NONE,  if NO error(s).
 *           SHELL_EXEC_ERR, otherwise.
 *
 * @note     (1) Usage(s)    : fs_date
 *                             fs_date  [time]
 *               - (a) Argument(s) : time      If specified, time to set, in the form 'mmddhhmmccyy'
 *                                                   where the 1st mm   is the month  (1-12);
 *                                                       the     dd   is the day    (1-29, 30 or 31);
 *                                                       the     hh   is the hour   (0-23)
 *                                                       the 2nd mm   is the minute (0-59)
 *                                                       the     ccyy is the year   (1970 or larger)
 *               - (b) Output      : If no argument, date & time.
 *******************************************************************************************************/
static CPU_INT16S FSShell_date(CPU_INT16U      argc,
                               CPU_CHAR        *argv[],
                               SHELL_OUT_FNCT  out_fnct,
                               SHELL_CMD_PARAM *p_cmd_param)
{
  void                 *p_opt;
  CPU_BOOLEAN          dig;
  CPU_SIZE_T           i;
  CPU_SIZE_T           len;
  CPU_CHAR             out_str[FS_TIME_STR_MIN_LEN];
  sl_sleeptimer_date_t date;
  CPU_INT16U           year;
  sl_status_t          status;

  p_opt = p_cmd_param->OutputOptPtr;

  //                                                               ------------------ CHK ARGUMENTS -------------------
  if (argc == 2u) {
    if (Str_Cmp_N(argv[1], FS_SHELL_STR_HELP, 3u) == 0) {
      FS_SHELL_PRINTF(out_fnct, p_opt, "fs_date: usage: fs_date\r\n                fs_date mmddhhmmccyy\r\n");
      FS_SHELL_PRINTF(out_fnct, p_opt, "                Write the date & time to terminal output, or set the system date & time.\r\n");
      return (SHELL_EXEC_ERR_NONE);
    }
  }

  if ((argc != 1u) && (argc != 2u)) {
    FS_SHELL_PRINTF(out_fnct, p_opt, "fs_date: usage: fs_date\r\n                fs_date mmddhhmmccyy\r\n");
    return (SHELL_EXEC_ERR);
  }

  if (argc == 1u) {                                             // ------------------- GET DATE/TIME ------------------
    status = sl_sleeptimer_get_datetime(&date);
    if (status == SL_STATUS_OK) {
      // Convert date/time to str.
      sl_sleeptimer_convert_date_to_str(out_str, sizeof(out_str), (const uint8_t *)FS_TIME_FMT, &date);
    }

    if (status == SL_STATUS_OK) {                   // Wr date/time.
      FS_SHELL_PRINTF(out_fnct, p_opt, "%s\r\n", out_str);
    } else {
      FS_SHELL_PRINTF(out_fnct, p_opt, "Time could not be gotten.\r\n")
    }
  } else {                                                      // ------------------- SET DATE/TIME ------------------
    CPU_BOOLEAN  ok;

    len = Str_Len_N(argv[1], 13u);
    ok = DEF_FAIL;
    if (len == 12u) {
      ok = DEF_OK;
      for (i = 0u; i < 12u; i++) {
        dig = ASCII_IS_DIG(argv[1][i]);
        if (dig == DEF_NO) {
          ok = DEF_FAIL;
        }
      }
    }

    if (ok) {
      date.month = ((CPU_INT08U)argv[1][1]  - ASCII_CHAR_DIGIT_ZERO) + (((CPU_INT08U)argv[1][0]  - ASCII_CHAR_DIGIT_ZERO) * DEF_NBR_BASE_DEC);
      date.month_day = ((CPU_INT08U)argv[1][3]  - ASCII_CHAR_DIGIT_ZERO) + (((CPU_INT08U)argv[1][2]  - ASCII_CHAR_DIGIT_ZERO) * DEF_NBR_BASE_DEC);
      date.hour = ((CPU_INT08U)argv[1][5]  - ASCII_CHAR_DIGIT_ZERO) + (((CPU_INT08U)argv[1][4]  - ASCII_CHAR_DIGIT_ZERO) * DEF_NBR_BASE_DEC);
      date.min = ((CPU_INT08U)argv[1][7]  - ASCII_CHAR_DIGIT_ZERO) + (((CPU_INT08U)argv[1][6]  - ASCII_CHAR_DIGIT_ZERO) * DEF_NBR_BASE_DEC);

      year = ((CPU_INT16U)argv[1][9]  - ASCII_CHAR_DIGIT_ZERO) + (((CPU_INT16U)argv[1][8]  - ASCII_CHAR_DIGIT_ZERO) * DEF_NBR_BASE_DEC);
      year *= DEF_NBR_BASE_DEC;
      year *= DEF_NBR_BASE_DEC;
      year += ((CPU_INT16U)argv[1][11] - ASCII_CHAR_DIGIT_ZERO) + (((CPU_INT16U)argv[1][10] - ASCII_CHAR_DIGIT_ZERO) * DEF_NBR_BASE_DEC);
      if ((year < UNIX_EPOCH_YR_START) || (year > UNIX_EPOCH_YR_END)) {
        ok = DEF_FAIL;
      } else {
        date.year = year;
      }
    }

    if (!ok) {
      FS_SHELL_PRINTF(out_fnct, p_opt, "fs_date: usage: fs_date\r\n                fs_date mmddhhmmccyy\r\n");
      return (SHELL_EXEC_ERR);
    }

    status = sl_sleeptimer_set_datetime(&date);
    if (status != SL_STATUS_OK) {
      FS_SHELL_PRINTF(out_fnct, p_opt, "Time could not be set.\r\n");
    }
  }

  return (RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                               FSShell_df()
 *
 * @brief    Report disk free space.
 *
 * @param    argc            The number of arguments.
 *
 * @param    argv            Array of arguments.
 *
 * @param    out_fnct        The output function.
 *
 * @param    p_cmd_param     Pointer to the command parameters.
 *
 * @return   RTOS_ERR_NONE,  if NO error(s).
 *           SHELL_EXEC_ERR, otherwise.
 *
 * @note     (1)  Usage(s)    : fs_df
 *                            fs_df  [vol]
 *               - (a) Argument(s) : vol       If specified, volume on which to report free space.
 *               - (b) Output      : Name, total space, free space & used space of volume(s).
 *******************************************************************************************************/
static CPU_INT16S FSShell_df(CPU_INT16U      argc,
                             CPU_CHAR        *argv[],
                             SHELL_OUT_FNCT  out_fnct,
                             SHELL_CMD_PARAM *p_cmd_param)
{
  FS_VOL_HANDLE vol_handle;
  RTOS_ERR      err;
  void          *p_opt;

  RTOS_ERR_SET(err, RTOS_ERR_NONE);
  p_opt = p_cmd_param->OutputOptPtr;

  //                                                               ------------------ CHK ARGUMENTS -------------------
  if (argc == 2u) {
    if (Str_Cmp_N(argv[1], FS_SHELL_STR_HELP, 3u) == 0) {
      FS_SHELL_PRINTF(out_fnct, p_opt, "fs_df: usage: fs_df {[vol]}\r\n");
      FS_SHELL_PRINTF(out_fnct, p_opt, "              Report disk free space.");
      return (SHELL_EXEC_ERR_NONE);
    }
  }

  if ((argc != 1u) && (argc != 2u)) {
    FS_SHELL_PRINTF(out_fnct, p_opt, "fs_df: usage: fs_df {[vol]}\r\n");
    return (SHELL_EXEC_ERR);
  }

  //                                                               --------------------- DISP HDR ---------------------
  FS_SHELL_PRINTF(out_fnct, p_opt, "%.-20s%.-12s%.-12s%.-12s%.-12s",
                  "Filesystem", "512-blocks", "Used", "Available", "Mounted on\r\n");

  if (argc == 2u) {                                             // -------------- DISP INFO ABOUT ONE VOL -------------
    CPU_CHAR *vol_name;

    vol_name = argv[1];

    vol_handle = FSVol_Get(vol_name);
    if (FS_VOL_HANDLE_IS_NULL(vol_handle)) {
      FS_SHELL_ERR_PRINT(out_fnct, p_opt, err);
      return (SHELL_EXEC_ERR);
    }

    FSShell_VolInfoDisplay(vol_handle,
                           p_cmd_param,
                           out_fnct,
                           &err);
  } else {                                                      // ------------- DISP INFO ABOUT ALL VOLS -------------
    FS_VOL_FOR_EACH(vol_handle) {
      FSShell_VolInfoDisplay(vol_handle,
                             p_cmd_param,
                             out_fnct,
                             &err);
      if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
        break;
      }
    }
    if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
      return (SHELL_EXEC_ERR);
    }
  }

  FS_SHELL_PRINTF(out_fnct, p_opt, "\r\n");
  return (SHELL_EXEC_ERR_NONE);
}

/****************************************************************************************************//**
 *                                               FSShell_lsblk()
 *
 * @brief    List all open block devices and display information about each partition (e.g. number of
 *           logical blocks and their size) composing the block device.
 *
 * @param    argc            The number of arguments.
 *
 * @param    argv            Array of arguments.
 *
 * @param    out_fnct        The output function.
 *
 * @param    p_cmd_param     Pointer to the command parameters.
 *
 * @return   RTOS_ERR_NONE,  if NO error(s).
 *           SHELL_EXEC_ERR, otherwise.
 *******************************************************************************************************/
static CPU_INT16S FSShell_lsblk(CPU_INT16U      argc,
                                CPU_CHAR        *argv[],
                                SHELL_OUT_FNCT  out_fnct,
                                SHELL_CMD_PARAM *p_cmd_param)
{
  void              *p_opt;
  FS_BLK_DEV_HANDLE blk_dev_handle;
  CPU_CHAR          media_name[FS_SHELL_MEDIA_NAME_MAX_LEN];
  FS_LB_SIZE        lb_size;
  FS_LB_QTY         lb_cnt;
  CPU_SIZE_T        media_name_len;
  FS_PARTITION_NBR  partition_ix;
  FS_PARTITION_NBR  partition_cnt;
  FS_PARTITION_INFO partition_info;
  RTOS_ERR          err;

  p_opt = p_cmd_param->OutputOptPtr;
  RTOS_ERR_SET(err, RTOS_ERR_NONE);

  //                                                               ------------------ CHK ARGUMENTS -------------------
  if (argc == 2u) {
    if (Str_Cmp_N(argv[1], FS_SHELL_STR_HELP, 3u) == 0) {
      FS_SHELL_PRINTF(out_fnct, p_opt, "fs_df: usage: fs_df {[vol]}\r\n");
      FS_SHELL_PRINTF(out_fnct, p_opt, "              Report disk free space.");
      return (SHELL_EXEC_ERR_NONE);
    }
  }

  if (argc != 1u) {
    FS_SHELL_PRINTF(out_fnct, p_opt, "fs_df: usage: fs_df {[vol]}\r\n");
    return (SHELL_EXEC_ERR);
  }

  FS_BLK_DEV_FOR_EACH(blk_dev_handle) {
    lb_size = FSBlkDev_LbSizeGet(blk_dev_handle, &err);
    if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
      FS_SHELL_ERR_PRINT(out_fnct, p_opt, err);
      return (SHELL_EXEC_ERR);
    }

    lb_cnt = FSBlkDev_LbCntGet(blk_dev_handle, &err);
    if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
      FS_SHELL_ERR_PRINT(out_fnct, p_opt, err);
      return (SHELL_EXEC_ERR);
    }

    FSMedia_NameGet(FSBlkDev_MediaGet(blk_dev_handle),
                    media_name,
                    sizeof(media_name),
                    &err);
    if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
      FS_SHELL_ERR_PRINT(out_fnct, p_opt, err);
      return (SHELL_EXEC_ERR);
    }

    media_name_len = Str_Len(media_name);

    FS_SHELL_PRINTF(out_fnct, p_opt, "%.-30sLogical blocks\r\n", "Media name");
    FS_SHELL_PRINTF(out_fnct, p_opt, "%.-30s%u\r\n", media_name, lb_cnt);

#if (FS_CORE_CFG_PARTITION_EN == DEF_ENABLED)
    partition_cnt = FSPartition_CntGet(blk_dev_handle, &err);
#else
    partition_cnt = 1u;
#endif
    FS_SHELL_PRINTF(out_fnct, p_opt, "%.-30sSize (bytes)\r\n", "Partition number");

    for (partition_ix = 1u; partition_ix <= partition_cnt; partition_ix++) {
      FSPartition_Query(blk_dev_handle,
                        partition_ix,
                        &partition_info,
                        &err);
      if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
        FS_SHELL_PRINTF(out_fnct, p_opt, "Failed to query partition.\r\n");
        FS_SHELL_ERR_PRINT(out_fnct, p_opt, err);
        return (SHELL_EXEC_ERR);
      }

      FS_SHELL_PRINTF(out_fnct, p_opt, "\\--%s:%u%.*s%u\r\n", media_name, partition_ix,
                      30 - media_name_len - 5, "", partition_info.SecCnt * lb_size);
    }
  }

  return (SHELL_EXEC_ERR_NONE);
}

/****************************************************************************************************//**
 *                                               FSShell_ls()
 *
 * @brief    List directory contents.
 *
 * @param    argc            The number of arguments.
 *
 * @param    argv            Array of arguments.
 *
 * @param    out_fnct        The output function.
 *
 * @param    p_cmd_param     Pointer to the command parameters.
 *
 * @return   RTOS_ERR_NONE,  if NO error(s).
 *           SHELL_EXEC_ERR, otherwise.
 *
 * @note     (1) Usage(s)    : fs_ls
 *               - (a) Argument(s) : none.
 *               - (b) Output      : List of directory contents.
 *
 * @note     (2) The output resembles the output from the standard UNIX command 'ls -l'.
 *******************************************************************************************************/
static CPU_INT16S FSShell_ls(CPU_INT16U      argc,
                             CPU_CHAR        *argv[],
                             SHELL_OUT_FNCT  out_fnct,
                             SHELL_CMD_PARAM *p_cmd_param)
{
  void *p_opt;

  p_opt = p_cmd_param->OutputOptPtr;

  //                                                               ------------------ CHK ARGUMENTS -------------------
  if (argc == 2u) {
    if (Str_Cmp_N(argv[1], FS_SHELL_STR_HELP, 3u) == 0) {
      FS_SHELL_PRINTF(out_fnct, p_opt, "fs_ls: usage: fs_ls\r\n");
      FS_SHELL_PRINTF(out_fnct, p_opt, "              List information about files in the current directory.\r\n");
      return (SHELL_EXEC_ERR_NONE);
    }
  }

  if (argc != 1u) {
    FS_SHELL_PRINTF(out_fnct, p_opt, "fs_ls: usage: fs_ls\r\n");
    return (SHELL_EXEC_ERR);
  }

  //                                                               ---------------- LIST DIR CONTENTS -----------------
  //                                                               0000000000011111111122222222223333333333
  //                                                               0123456789012345678901234567890123456789
  //                                                               File str: "drw-rw-rw- 12345 Jul 20 2008 07:41 a.pdf"

#if (FS_CORE_CFG_DIR_EN == DEF_ENABLED)
  {
    FS_DIR_HANDLE dir_handle;
    sl_status_t status;
    RTOS_ERR             err;
    FS_ENTRY_INFO        entry_info;
    sl_sleeptimer_date_t date;
    CPU_BOOLEAN          eof;

    RTOS_ERR_SET(err, RTOS_ERR_NONE);

    dir_handle = FSDir_Open(FSShell_Data.WorkingDirHandle,
                            DEF_NULL,
                            FS_DIR_ACCESS_MODE_NONE,
                            &err);
    if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
      FS_SHELL_PRINTF(out_fnct, p_opt, "Cannot open directory.\r\n");
      FS_SHELL_ERR_PRINT(out_fnct, p_opt, err);
      return (SHELL_EXEC_ERR);
    }

    eof = DEF_NO;
    while (!eof) {
      eof = FSDir_Rd(dir_handle,
                     &entry_info,
                     (CPU_CHAR *)FSShell_Data.BufPtr,
                     FS_SHELL_GLOBAL_BUF_MAX_SIZE,
                     &err);
      if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
        FS_SHELL_PRINTF(out_fnct, p_opt, "Cannot read directory.\r\n");
        FS_SHELL_ERR_PRINT(out_fnct, p_opt, err);
        break;
      }

      if (!eof) {
        FS_SHELL_PRINTF(out_fnct, p_opt, "%s%s %d", entry_info.Attrib.IsDir ? "d"   : "-",
                        entry_info.Attrib.Wr    ? "www" : "---",
                        entry_info.Attrib.IsDir ? 0u    : entry_info.Size);

        if (entry_info.DateTimeWr != FS_TIME_TS_INVALID) {
          status = sl_sleeptimer_convert_time_to_date(entry_info.DateTimeWr, 0, &date);
          if (status == SL_STATUS_OK) {
            FS_SHELL_PRINTF(out_fnct, p_opt, " %s %u %u %u:%u", FSShell_Month_Name[date.month - 1u],
                            date.month_day, date.year, date.hour, date.min);
          } else {
            FS_SHELL_PRINTF(out_fnct, p_opt, "                    ");
          }
        }
        FS_SHELL_PRINTF(out_fnct, p_opt, " %s \r\n", FSShell_Data.BufPtr);
      }
    }

    FS_SHELL_PRINTF(out_fnct, p_opt, "\r\n");

    FSDir_Close(dir_handle, &err);
    if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
      return (SHELL_EXEC_ERR);
    }

    return (SHELL_EXEC_ERR_NONE);
  }
#else
  FS_SHELL_PRINTF(out_fnct, p_opt, "You must defined FS_CORE_CFG_DIR_EN to DEF_ENABLED.\r\n");
  return (SHELL_EXEC_ERR);
#endif
}

/****************************************************************************************************//**
 *                                               FSShell_mkdir()
 *
 * @brief    Make a directory.
 *
 * @param    argc            The number of arguments.
 *
 * @param    argv            Array of arguments.
 *
 * @param    out_fnct        The output function.
 *
 * @param    p_cmd_param     Pointer to the command parameters.
 *
 * @return   RTOS_ERR_NONE,  if NO error(s).
 *           SHELL_EXEC_ERR, otherwise.
 *
 * @note     (1) Usage(s)    : fs_mkdir [dir]
 *               - (a) Argument(s) : dir       Directory path.
 *               - (b) Output      : none.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static CPU_INT16S FSShell_mkdir(CPU_INT16U      argc,
                                CPU_CHAR        *argv[],
                                SHELL_OUT_FNCT  out_fnct,
                                SHELL_CMD_PARAM *p_cmd_param)
{
  CPU_CHAR *p_path;
  void     *p_opt;
  RTOS_ERR err;

  RTOS_ERR_SET(err, RTOS_ERR_NONE);
  p_opt = p_cmd_param->OutputOptPtr;

  //                                                               ------------------ CHK ARGUMENTS -------------------
  if (argc != 2u) {
    FS_SHELL_PRINTF(out_fnct, p_opt, "fs_mkdir: usage: fs_mkdir [dir]\r\n");
    return (SHELL_EXEC_ERR);
  }

  if (Str_Cmp_N(argv[1], FS_SHELL_STR_HELP, 3u) == 0) {
    FS_SHELL_PRINTF(out_fnct, p_opt, "fs_mkdir: usage: fs_mkdir [dir]");
    FS_SHELL_PRINTF(out_fnct, p_opt, "                 Create [dir], if it does not already exist.");
    return (RTOS_ERR_NONE);
  }

  p_path = argv[1];

  //                                                               --------------------- MAKE DIR ---------------------
  FSEntry_Create(FSShell_Data.WorkingDirHandle,
                 p_path,
                 FS_ENTRY_TYPE_DIR,
                 DEF_YES,
                 &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    FS_SHELL_ERR_PRINT(out_fnct, p_opt, err);
    return (SHELL_EXEC_ERR);
  }

  return (SHELL_EXEC_ERR_NONE);
}
#endif

/****************************************************************************************************//**
 *                                               FSShell_mkfs()
 *
 * @brief    Format a volume.
 *
 * @param    argc            The number of arguments.
 *
 * @param    argv            Array of arguments.
 *
 * @param    out_fnct        The output function.
 *
 * @param    p_cmd_param     Pointer to the command parameters.
 *
 * @return   RTOS_ERR_NONE,  if NO error(s).
 *           SHELL_EXEC_ERR, otherwise.
 *
 * @note     (1) Usage(s)    : fs_mkfs [dev] [part]
 *               - (a) Argument(s) : dev     Device name.
 *                                   part    Partition number (> 0)
 *               - (b) Output      : none.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static CPU_INT16S FSShell_mkfs(CPU_INT16U      argc,
                               CPU_CHAR        *argv[],
                               SHELL_OUT_FNCT  out_fnct,
                               SHELL_CMD_PARAM *p_cmd_param)
{
  CPU_CHAR          *p_media_name;
  CPU_CHAR          *p_partition_name;
  void              *p_opt;
  FS_BLK_DEV_HANDLE blk_dev_handle;
  FS_MEDIA_HANDLE   media_handle;
  FS_PARTITION_NBR  partition_nbr;
  RTOS_ERR          err;

  RTOS_ERR_SET(err, RTOS_ERR_NONE);
  p_opt = p_cmd_param->OutputOptPtr;

  //                                                               ------------------ CHK ARGUMENTS -------------------
  if (argc != 3u) {
    FS_SHELL_PRINTF(out_fnct, p_opt, "fs_mkfs: usage: fs_mkfs {[vol]}");
    return (SHELL_EXEC_ERR);
  }

  if (Str_Cmp_N(argv[1], FS_SHELL_STR_HELP, 3u) == 0) {
    FS_SHELL_PRINTF(out_fnct, p_opt, "fs_mkfs: usage: fs_mkfs {[vol]}");
    FS_SHELL_PRINTF(out_fnct, p_opt, "                Format [vol].");
    return (SHELL_EXEC_ERR_NONE);
  }

  p_media_name = argv[1];
  p_partition_name = argv[2];

  partition_nbr = Str_ParseNbr_Int32S(p_partition_name,
                                      DEF_NULL,
                                      DEF_NBR_BASE_DEC);

  media_handle = FSMedia_Get(p_media_name);

  if (FS_MEDIA_HANDLE_IS_NULL(media_handle)) {
    FS_SHELL_ERR_PRINT(out_fnct, p_opt, "Device name not found.\r\n");
    return (SHELL_EXEC_ERR);
  }

  blk_dev_handle = FSBlkDev_Get(media_handle);

  FS_FAT_Fmt(blk_dev_handle,
             partition_nbr,
             DEF_NULL,
             &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    FS_SHELL_PRINTF(out_fnct, p_opt, "Error while formatting.\r\n");
    FS_SHELL_ERR_PRINT(out_fnct, p_opt, err);
    return (SHELL_EXEC_ERR);
  }

  return (RTOS_ERR_NONE);
}
#endif

/****************************************************************************************************//**
 *                                               FSShell_mount()
 *
 * @brief    Mount volume.
 *
 * @param    argc            The number of arguments.
 *
 * @param    argv            Array of arguments.
 *
 * @param    out_fnct        The output function.
 *
 * @param    p_cmd_param     Pointer to the command parameters.
 *
 * @return   RTOS_ERR_NONE,  if NO error(s).
 *           SHELL_EXEC_ERR, otherwise.
 *
 * @note     (1) Usage(s)    : fs_mount  [dev] [par] [vol]
 *               - (a) Argument(s) : dev       Device to mount.
 *                                   par       Partition number
 *                                   vol       Name which will be given to volume.
 *               - (b) Output      : none.
 *******************************************************************************************************/
static CPU_INT16S FSShell_mount(CPU_INT16U      argc,
                                CPU_CHAR        *argv[],
                                SHELL_OUT_FNCT  out_fnct,
                                SHELL_CMD_PARAM *p_cmd_param)
{
  CPU_CHAR          *p_partition_name;
  CPU_CHAR          *p_vol_name;
  CPU_CHAR          *p_media_name;
  void              *p_opt;
  FS_PARTITION_NBR  partition_nbr;
  FS_BLK_DEV_HANDLE blk_dev_handle;
  FS_MEDIA_HANDLE   media_handle;
  RTOS_ERR          err;

  RTOS_ERR_SET(err, RTOS_ERR_NONE);
  p_opt = p_cmd_param->OutputOptPtr;
  //                                                               ------------------ CHK ARGUMENTS -------------------
  if (argc == 2u) {
    if (Str_Cmp_N(argv[1], FS_SHELL_STR_HELP, 3u) == 0) {
      FS_SHELL_PRINTF(out_fnct, p_opt, "fs_mount: usage: fs_mount [Dev] [par] [vol]\r\n");
      FS_SHELL_PRINTF(out_fnct, p_opt, "                 Mount [dev] as [vol].\r\n");
      return (RTOS_ERR_NONE);
    }
  }

  if (argc != 4u) {
    FS_SHELL_PRINTF(out_fnct, p_opt, "fs_mount: usage: fs_mount [dev] [par] [vol]\r\n");
    return (SHELL_EXEC_ERR);
  }

  p_media_name = argv[1u];
  p_partition_name = argv[2u];
  p_vol_name = argv[3u];

  partition_nbr = Str_ParseNbr_Int32U(p_partition_name,
                                      DEF_NULL,
                                      DEF_NBR_BASE_DEC);

  //                                                               --------------------- OPEN VOL ---------------------
  if (partition_nbr < 1u) {
    FS_SHELL_ERR_PRINT(out_fnct, p_opt, err);
    return (SHELL_EXEC_ERR);
  }

  media_handle = FSMedia_Get(p_media_name);

  if (FS_MEDIA_HANDLE_IS_NULL(media_handle)) {
    FS_SHELL_ERR_PRINT(out_fnct, p_opt, "Device name not found.\r\n");
    return (SHELL_EXEC_ERR);
  }

  blk_dev_handle = FSBlkDev_Get(media_handle);

  FSVol_Open(blk_dev_handle,
             partition_nbr,
             p_vol_name,
             FS_VOL_OPT_DFLT,
             &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    FS_SHELL_ERR_PRINT(out_fnct, p_opt, err);
    return (SHELL_EXEC_ERR);
  }
  //                                                               Initialize global wkr dir handle to root.

  FSShell_Data.WorkingDirHandle = FSWrkDir_Open(FS_WRK_DIR_NULL,
                                                p_vol_name,
                                                &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    FS_SHELL_ERR_PRINT(out_fnct, p_opt, err);
    return (SHELL_EXEC_ERR);
  }

  return (SHELL_EXEC_ERR_NONE);
}

/****************************************************************************************************//**
 *                                               FSShell_mv()
 *
 * @brief    Move files.
 *
 * @param    argc            The number of arguments.
 *
 * @param    argv            Array of arguments.
 *
 * @param    out_fnct        The output function.
 *
 * @param    p_cmd_param     Pointer to the command parameters.
 *
 * @return   RTOS_ERR_NONE,  if NO error(s).
 *           SHELL_EXEC_ERR, otherwise.
 *
 * @note     (1) Usage(s)    : fs_mv [source_entry] [dest_entry]
 *                                 fs_mv [source_entry] [dest_dir]
 *               - (a) Argument(s) : source_entry  Source entry path.
 *                                   dest_entry    Destination entry path.
 *                                   dest_dir      Destination directory path.
 *               - (b) Output      : none.
 *
 * @note     (2) In the first form of this command, the second argument must not be an existing
 *               directory.  The file 'source_entry' will be renamed 'dest_entry'.
 *               - (a) In the second form of this command, the second argument must be an existing
 *                     directory.  'source_entry' will be renamed to an entry with name formed by
 *                     concatenating 'dest_dir', a path separator character and the final component of
 *                     'source_entry'.
 *               - (b) In both forms, if 'source_entry' is a directory, the entire directory tree rooted
 *                     at 'source_entry' will be copied and then deleted.  Additionally, both
 *                     'source_entry' and 'dest_entry' or 'dest_dir' must specify locations on the same
 *                     volume.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static CPU_INT16S FSShell_mv(CPU_INT16U      argc,
                             CPU_CHAR        *argv[],
                             SHELL_OUT_FNCT  out_fnct,
                             SHELL_CMD_PARAM *p_cmd_param)
{
  CPU_CHAR *p_path_src;
  CPU_CHAR *p_path_dest;
  void     *p_opt;
  RTOS_ERR err;

  RTOS_ERR_SET(err, RTOS_ERR_NONE);
  p_opt = p_cmd_param->OutputOptPtr;

  //                                                               ------------------ CHK ARGUMENTS -------------------
  if (argc == 2u) {
    if (Str_Cmp_N(argv[1], FS_SHELL_STR_HELP, 3u) == 0) {
      FS_SHELL_PRINTF(out_fnct, p_opt, "fs_mv: usage: fs_mv [source] [dest]\r\n");
      FS_SHELL_PRINTF(out_fnct, p_opt, "              fs_mv [source] [dir]\r\n");
      return (SHELL_EXEC_ERR_NONE);
    }
  }

  if (argc != 3u) {
    FS_SHELL_PRINTF(out_fnct, p_opt, "fs_mv: usage: fs_mv [source] [dest]\r\n");
    return (SHELL_EXEC_ERR);
  }

  p_path_src = argv[1];
  p_path_dest = argv[2];

  //                                                               --------------------- MOVE FILE --------------------
  FSEntry_Rename(FSShell_Data.WorkingDirHandle,
                 p_path_src,
                 FSShell_Data.WorkingDirHandle,
                 p_path_dest,
                 DEF_NO,
                 &err);

  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    FS_SHELL_PRINTF(out_fnct, p_opt, "Could not move %s to %s.\r\n", p_path_src, p_path_dest);
    FS_SHELL_ERR_PRINT(out_fnct, p_opt, err);
    return (SHELL_EXEC_ERR);
  }

  return (SHELL_EXEC_ERR_NONE);
}
#endif

/****************************************************************************************************//**
 *                                               FSShell_od()
 *
 * @brief    Dump file contents to terminal output.
 *
 * @param    argc            The number of arguments.
 *
 * @param    argv            Array of arguments.
 *
 * @param    out_fnct        The output function.
 *
 * @param    p_cmd_param     Pointer to the command parameters.
 *
 * @return   RTOS_ERR_NONE,  if NO error(s).
 *           SHELL_EXEC_ERR, otherwise.
 *
 * @note     (1) Usage(s)    : fs_od [file]
 *               - (a) Argument(s) : file          Path of file to dump to terminal output.
 *               - (b) Output      : File contents, in hexadecimal form.
 *******************************************************************************************************/
static CPU_INT16S FSShell_od(CPU_INT16U      argc,
                             CPU_CHAR        *argv[],
                             SHELL_OUT_FNCT  out_fnct,
                             SHELL_CMD_PARAM *p_cmd_param)
{
  void           *p_opt;
  CPU_CHAR       *p_path;
  FS_FILE_HANDLE file_handle;
  CPU_SIZE_T     file_rd_len;
  RTOS_ERR       err;
  CPU_SIZE_T     i;
  CPU_SIZE_T     j;
  CPU_INT32U     nbr_byte_print;
  CPU_BOOLEAN    print;
  CPU_CHAR       print_buf[33];
  CPU_INT32U     print_acc;

  RTOS_ERR_SET(err, RTOS_ERR_NONE);
  p_opt = p_cmd_param->OutputOptPtr;

  //                                                               ------------------ CHK ARGUMENTS -------------------
  if (argc == 2u) {
    if (Str_Cmp_N(argv[1], FS_SHELL_STR_HELP, 3u) == 0) {
      FS_SHELL_PRINTF(out_fnct, p_opt, "fs_od: usage: fs_od [file]\r\n");
      FS_SHELL_PRINTF(out_fnct, p_opt, "              Dump [file] to standard output in specified format.\r\n");
      return (SHELL_EXEC_ERR_NONE);
    }
  }

  if (argc != 2u) {
    FS_SHELL_PRINTF(out_fnct, p_opt, "fs_od: usage: fs_od [file]\r\n");
    return (SHELL_EXEC_ERR);
  }

  p_path = argv[1];

  file_handle = FSFile_Open(FSShell_Data.WorkingDirHandle,
                            p_path,                             // Open file.
                            FS_FILE_ACCESS_MODE_RD,
                            &err);

  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {                // File not opened.
    FS_SHELL_PRINTF(out_fnct, p_opt, "Could not open file.\r\n");
    FS_SHELL_ERR_PRINT(out_fnct, p_opt, err);
    return (SHELL_EXEC_ERR);
  }

  nbr_byte_print = 0u;
  //                                                               -------------------- PRINT FILE --------------------
  do {
    file_rd_len = FSFile_Rd(file_handle,
                            FSShell_Data.BufPtr,
                            FS_SHELL_GLOBAL_BUF_MAX_SIZE,
                            &err);
    if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
      FS_SHELL_PRINTF(out_fnct, p_opt, "Could not read file.\r\n");
      FS_SHELL_ERR_PRINT(out_fnct, p_opt, err);
      return (SHELL_EXEC_ERR);
    }

    i = 0u;
    while (i < file_rd_len) {
      if ((nbr_byte_print % 32u) == 0u) {
        (void)Str_FmtNbr_Int32U(nbr_byte_print, 8u, DEF_NBR_BASE_HEX, (CPU_CHAR)ASCII_CHAR_DIGIT_ZERO, DEF_NO, DEF_NO, print_buf);
        print_buf[8] = (CPU_CHAR)ASCII_CHAR_SPACE;
        print_buf[9] = (CPU_CHAR)ASCII_CHAR_NULL;
        (void)out_fnct(print_buf, 9u, p_opt);
      }

      print_acc = FSShell_Data.BufPtr[i];
      if (file_rd_len - 1u > i) {
        print_acc += ((CPU_INT32U)FSShell_Data.BufPtr[i + 1u] << 8);
      }
      if (file_rd_len - 2u > i) {
        print_acc += ((CPU_INT32U)FSShell_Data.BufPtr[i + 2u] << 16);
      }
      if (file_rd_len - 3u > i) {
        print_acc += ((CPU_INT32U)FSShell_Data.BufPtr[i + 3u] << 24);
      }

      i += 4u;
      nbr_byte_print += 4u;

      (void)Str_FmtNbr_Int32U(print_acc, 8u, DEF_NBR_BASE_HEX, (CPU_CHAR)ASCII_CHAR_DIGIT_ZERO, DEF_NO, DEF_NO, print_buf);
      print_buf[8] = (CPU_CHAR)ASCII_CHAR_SPACE;
      print_buf[9] = (CPU_CHAR)ASCII_CHAR_NULL;
      (void)out_fnct(print_buf, 9u, p_opt);

      if ((i % 32u) == 0u) {
        (void)out_fnct((CPU_CHAR *)"     ", 5u, p_opt);
        for (j = 0u; j < 32u; j++) {
          print = ASCII_IS_PRINT(FSShell_Data.BufPtr[(i - 32u) + j]);
          if (print == DEF_YES) {
            print_buf[j] = (CPU_CHAR)FSShell_Data.BufPtr[(i - 32u) + j];
          } else {
            print_buf[j] = (CPU_CHAR)ASCII_CHAR_FULL_STOP;
          }
        }
        print_buf[32] = (CPU_CHAR)ASCII_CHAR_SPACE;
        (void)out_fnct(print_buf, 32u, p_opt);
        FS_SHELL_PRINTF(out_fnct, p_opt, "r\n");
      }
    }
  } while (file_rd_len > 0u);

  //                                                               ------------------- CLOSE & RTN --------------------
  FSFile_Close(file_handle, &err);                              // Close src file.
  FS_SHELL_PRINTF(out_fnct, p_opt, "r\n\r\n");
  return (SHELL_EXEC_ERR_NONE);
}

/****************************************************************************************************//**
 *                                               FSShell_pwd()
 *
 * @brief    Write the pathname of current working directory to the terminal output.
 *
 * @param    argc            The number of arguments.
 *
 * @param    argv            Array of arguments.
 *
 * @param    out_fnct        The output function.
 *
 * @param    p_cmd_param     Pointer to the command parameters.
 *
 * @return   RTOS_ERR_NONE,  if NO error(s).
 *           SHELL_EXEC_ERR, otherwise.
 *
 * @note     (1) Usage(s)    : fs_pwd
 *               - (a) Argument(s) : none.
 *               - (b) Output      : Pathname of current working directory.
 *******************************************************************************************************/
static CPU_INT16S FSShell_pwd(CPU_INT16U      argc,
                              CPU_CHAR        *argv[],
                              SHELL_OUT_FNCT  out_fnct,
                              SHELL_CMD_PARAM *p_cmd_param)
{
  void     *p_opt;
  RTOS_ERR err;

  RTOS_ERR_SET(err, RTOS_ERR_NONE);
  p_opt = p_cmd_param->OutputOptPtr;

  //                                                               ------------------ CHK ARGUMENTS -------------------
  if (argc != 1u) {
    FS_SHELL_PRINTF(out_fnct, p_opt, "fs_pwd: usage: fs_pwd\r\n");
    return (SHELL_EXEC_ERR);
  }

  if (argc == 2u) {
    if (Str_Cmp_N(argv[1], FS_SHELL_STR_HELP, 3u) == 0) {
      FS_SHELL_PRINTF(out_fnct, p_opt, "fs_pwd: usage: fs_pwd\r\n");
      FS_SHELL_PRINTF(out_fnct, p_opt, "               Print the current working directory.\r\n");
      return (SHELL_EXEC_ERR_NONE);
    }
  }

  FSWrkDir_PathGet(FSShell_Data.WorkingDirHandle,
                   (CPU_CHAR *)FSShell_Data.BufPtr,
                   FS_SHELL_GLOBAL_BUF_MAX_SIZE,
                   &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    FS_SHELL_ERR_PRINT(out_fnct, p_opt, err);
    return (SHELL_EXEC_ERR);
  }

  FS_SHELL_PRINTF(out_fnct, p_opt, "%s\r\n\r\n", FSShell_Data.BufPtr);

  return (SHELL_EXEC_ERR_NONE);
}

/****************************************************************************************************//**
 *                                               FSShell_rm()
 *
 * @brief    Remove a directory entry, that is a file or a directory.
 *
 * @param    argc            The number of arguments.
 *
 * @param    argv            Array of arguments.
 *
 * @param    out_fnct        The output function.
 *
 * @param    p_cmd_param     Pointer to the command parameters.
 *
 * @return   RTOS_ERR_NONE,  if NO error(s).
 *           SHELL_EXEC_ERR, otherwise.
 *
 * @note     (1) Usage(s)    : fs_rm [entry]
 *               - (a) Argument(s) : File or directory path.
 *               - (b) Output      : none.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static CPU_INT16S FSShell_rm(CPU_INT16U      argc,
                             CPU_CHAR        *argv[],
                             SHELL_OUT_FNCT  out_fnct,
                             SHELL_CMD_PARAM *p_cmd_param)
{
  CPU_INT16S err_code;
  err_code = FSShell_EntryRemove(argc,
                                 argv,
                                 out_fnct,
                                 p_cmd_param,
                                 FS_ENTRY_TYPE_FILE);
  return (err_code);
}
#endif

/****************************************************************************************************//**
 *                                               FSShell_rmdir()
 *
 * @brief    Remove a directory.
 *
 * @param    argc            The number of arguments.
 *
 * @param    argv            Array of arguments.
 *
 * @param    out_fnct        The output function.
 *
 * @param    p_cmd_param     Pointer to the command parameters.
 *
 * @return   RTOS_ERR_NONE,  if NO error(s).
 *           SHELL_EXEC_ERR, otherwise.
 *
 * @note     (1) Usage(s)    : fs_rmdir [dir]
 *               - (a) Argument(s) : Directory path.
 *               - (b) Output      : none.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static CPU_INT16S FSShell_rmdir(CPU_INT16U      argc,
                                CPU_CHAR        *argv[],
                                SHELL_OUT_FNCT  out_fnct,
                                SHELL_CMD_PARAM *p_cmd_param)
{
  CPU_INT16S err_code;
  err_code = FSShell_EntryRemove(argc,
                                 argv,
                                 out_fnct,
                                 p_cmd_param,
                                 FS_ENTRY_TYPE_DIR);
  return (err_code);
}
#endif

/****************************************************************************************************//**
 *                                               FSShell_touch()
 *
 * @brief    Change file modification time.
 *
 * @param    argc            The number of arguments.
 *
 * @param    argv            Array of arguments.
 *
 * @param    out_fnct        The output function.
 *
 * @param    p_cmd_param     Pointer to the command parameters.
 *
 * @return   RTOS_ERR_NONE,  if NO error(s).
 *           SHELL_EXEC_ERR, otherwise.
 *
 * @note     (1) Usage(s)    : fs_touch [file]
 *               - (b) Argument(s) : File path.
 *               - (c) Output      : none.
 *
 * @note     (2) The file modification time is set to the current time.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static CPU_INT16S FSShell_touch(CPU_INT16U      argc,
                                CPU_CHAR        *argv[],
                                SHELL_OUT_FNCT  out_fnct,
                                SHELL_CMD_PARAM *p_cmd_param)
{
  CPU_CHAR             *p_path;
  void                 *p_opt;
  sl_sleeptimer_date_t date;
  RTOS_ERR             err;
  sl_status_t          status;

  RTOS_ERR_SET(err, RTOS_ERR_NONE);
  p_opt = p_cmd_param->OutputOptPtr;

  //                                                               ------------------ CHK ARGUMENTS -------------------
  if (argc != 2u) {
    FS_SHELL_PRINTF(out_fnct, p_opt, "fs_touch: usage: fs_touch [file]\r\n");
    return (SHELL_EXEC_ERR);
  }

  if (Str_Cmp_N(argv[1], FS_SHELL_STR_HELP, 3u) == 0) {
    FS_SHELL_PRINTF(out_fnct, p_opt, "fs_touch: usage: fs_touch [file]\r\n");
    FS_SHELL_PRINTF(out_fnct, p_opt, "                 Create file or, if file exists, update access and modification time.\r\n");
    return (SHELL_EXEC_ERR);
  }

  p_path = argv[1];

  //                                                               ------------------- CREATE FILE --------------------
  FSEntry_Create(FSShell_Data.WorkingDirHandle,
                 p_path,
                 FS_ENTRY_TYPE_FILE,
                 DEF_YES,
                 &err);
  if ((RTOS_ERR_CODE_GET(err) == RTOS_ERR_ALREADY_EXISTS)
      || (RTOS_ERR_CODE_GET(err) == RTOS_ERR_INVALID_TYPE)) {
    RTOS_ERR_SET(err, RTOS_ERR_NONE);

    //                                                             ------------------ CHNG FILE TIME ------------------
    status = sl_sleeptimer_get_datetime(&date);
    if (status != SL_STATUS_OK) {
      FS_SHELL_PRINTF(out_fnct, p_opt, "Time could not be gotten.\r\n");
      return (SHELL_EXEC_ERR);
    }

    sl_fs_entry_time_set(FSShell_Data.WorkingDirHandle,
                         p_path,
                         &date,
                         FS_DATE_TIME_MODIFY,
                         &err);
    if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
      FS_SHELL_PRINTF(out_fnct, p_opt, "Time could not be gotten.\r\n");
      FS_SHELL_ERR_PRINT(out_fnct, p_opt, err);
      return (SHELL_EXEC_ERR);
    }
  }

  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    FS_SHELL_ERR_PRINT(out_fnct, p_opt, err);
    return (SHELL_EXEC_ERR);
  }

  return (SHELL_EXEC_ERR_NONE);
}
#endif

/****************************************************************************************************//**
 *                                               FSShell_umount()
 *
 * @brief    Unmount volume.
 *
 * @param    argc            The number of arguments.
 *
 * @param    argv            Array of arguments.
 *
 * @param    out_fnct        The output function.
 *
 * @param    p_cmd_param     Pointer to the command parameters.
 *
 * @return   RTOS_ERR_NONE,  if NO error(s).
 *           SHELL_EXEC_ERR, otherwise.
 *
 * @note     (1) Usage(s)    : fs_umount [vol]
 *               - (a) Argument(s) : vol       Volume to unmount.
 *               - (b) Output      : none.
 *******************************************************************************************************/
static CPU_INT16S FSShell_umount(CPU_INT16U      argc,
                                 CPU_CHAR        *argv[],
                                 SHELL_OUT_FNCT  out_fnct,
                                 SHELL_CMD_PARAM *p_cmd_param)
{
  CPU_CHAR      *p_vol_name;
  void          *p_opt;
  FS_VOL_HANDLE vol_handle;
  RTOS_ERR      err;

  RTOS_ERR_SET(err, RTOS_ERR_NONE);
  p_opt = p_cmd_param->OutputOptPtr;

  //                                                               ------------------ CHK ARGUMENTS -------------------
  if (argc != 2u) {
    FS_SHELL_PRINTF(out_fnct, p_opt, "fs_umount: usage: fs_umount [vol]\r\n");
    return (SHELL_EXEC_ERR);
  }

  if (argc == 2u) {
    if (Str_Cmp_N(argv[1], FS_SHELL_STR_HELP, 3u) == 0) {
      FS_SHELL_PRINTF(out_fnct, p_opt, "fs_umount: usage: fs_umount [vol]\r\n");
      FS_SHELL_PRINTF(out_fnct, p_opt, "                  Unmount [vol].");
      return (RTOS_ERR_NONE);
    }
  }

  p_vol_name = argv[1u];

  vol_handle = FSVol_Get(p_vol_name);
  if (FS_VOL_HANDLE_IS_NULL(vol_handle)) {
    FS_SHELL_ERR_PRINT(out_fnct, p_opt, err);
    return (SHELL_EXEC_ERR);
  }

  //                                                               --------------------- CLOSE VOL --------------------
  FSVol_Close(vol_handle, &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    FS_SHELL_PRINTF(out_fnct, p_opt, "Could not umount volume %s.\r\n", p_vol_name);
    FS_SHELL_ERR_PRINT(out_fnct, p_opt, err);
    return (SHELL_EXEC_ERR);
  }

  return (SHELL_EXEC_ERR_NONE);
}

/****************************************************************************************************//**
 *                                               FSShell_wc()
 *
 * @brief    Determine the number of newlines, words and bytes in a file.
 *
 * @param    argc            The number of arguments.
 *
 * @param    argv            Array of arguments.
 *
 * @param    out_fnct        The output function.
 *
 * @param    p_cmd_param     Pointer to the command parameters.
 *
 * @return   RTOS_ERR_NONE,  if NO error(s).
 *           SHELL_EXEC_ERR, otherwise.
 *
 * @note     (1) Usage(s)    : fs_wc [file]
 *               - (a) Argument(s) : file          Path of file to examine.
 *               - (b) Output      : Number of newlines, words and bytes; equivalent to :
 *                                   @verbatim
 *                                       printf("%d %d %d %s", newline_cnt, word_cnt, byte_cnt, file);
 *                                   @endverbatim
 *******************************************************************************************************/
static CPU_INT16S FSShell_wc(CPU_INT16U      argc,
                             CPU_CHAR        *argv[],
                             SHELL_OUT_FNCT  out_fnct,
                             SHELL_CMD_PARAM *p_cmd_param)
{
  CPU_CHAR       *p_path;
  void           *p_opt;
  CPU_INT32U     cnt_bytes;
  CPU_INT32U     cnt_newlines;
  CPU_INT32U     cnt_words;
  CPU_SIZE_T     file_rd_len;
  FS_FILE_HANDLE file_handle;
  CPU_SIZE_T     i;
  CPU_BOOLEAN    is_newline;
  CPU_BOOLEAN    is_space;
  CPU_BOOLEAN    is_word;
  RTOS_ERR       err;

  RTOS_ERR_SET(err, RTOS_ERR_NONE);
  p_opt = p_cmd_param->OutputOptPtr;

  //                                                               ------------------ CHK ARGUMENTS -------------------
  if (argc != 2u) {
    FS_SHELL_PRINTF(out_fnct, p_opt, "fs_wc: usage: fs_wc [file]\r\n");
    return (SHELL_EXEC_ERR);
  }

  if (Str_Cmp_N(argv[1], FS_SHELL_STR_HELP, 3u) == 0) {
    FS_SHELL_PRINTF(out_fnct, p_opt, "fs_wc: usage: fs_wc [file]\r\n");
    FS_SHELL_PRINTF(out_fnct, p_opt, "              Determine the number of newlines, words and bytes in [file].\r\n");
    return (SHELL_EXEC_ERR_NONE);
  }

  p_path = argv[1];

  //                                                               --------------------- OPEN FILE --------------------
  file_handle = FSFile_Open(FSShell_Data.WorkingDirHandle,
                            p_path,
                            FS_FILE_ACCESS_MODE_RD,
                            &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {                // File not opened.
    FS_SHELL_PRINTF(out_fnct, p_opt, "Could not open file.\r\n");
    FS_SHELL_ERR_PRINT(out_fnct, p_opt, err);
    return (SHELL_EXEC_ERR);
  }

  //                                                               ----------------- PERFORM ANALYSIS -----------------
  cnt_bytes = 0u;
  cnt_words = 0u;
  cnt_newlines = 0u;
  is_word = DEF_NO;
  do {
    file_rd_len = FSFile_Rd(file_handle,
                            (void *)FSShell_Data.BufPtr,
                            FS_SHELL_GLOBAL_BUF_MAX_SIZE,
                            &err);

    if (file_rd_len > 0u) {
      cnt_bytes += file_rd_len;

      for (i = 0u; i < file_rd_len; i++) {
        is_space = ASCII_IS_SPACE(FSShell_Data.BufPtr[i]);
        if (is_space == DEF_YES) {                              // If char is space        ...
          if (is_word == DEF_YES) {                             // ... chk if it ends word ...
            is_word = DEF_NO;
            cnt_words++;
          }
          //                                                       ... & chk if it is newline.
          is_newline = (FSShell_Data.BufPtr[i] == ASCII_CHAR_LF) ? DEF_YES : DEF_NO;
          if (is_newline == DEF_YES) {
            cnt_newlines++;
          }
        } else {                                                // Any non-space char begins or continues word.
          is_word = DEF_YES;
        }
      }
    }
  } while (file_rd_len > 0u);

  if (is_word == DEF_YES) {
    cnt_words++;
  }

  //                                                               -------------------- CLOSE & RTN -------------------
  (void)FSFile_Close(file_handle, &err);                        // Close file.

  FS_SHELL_PRINTF(out_fnct, p_opt, "%.-10u%.-10u%.-10u%s\r\n\r\n",
                  cnt_newlines, cnt_words, cnt_bytes, p_path);

  return (RTOS_ERR_NONE);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           FSShell_EntryRemove()
 *
 * @brief    Remove a directory entry, that is a file or a directory.
 *
 * @param    argc            The number of arguments.
 *
 * @param    argv            Array of arguments.
 *
 * @param    out_fnct        The output function.
 *
 * @param    p_cmd_param     Pointer to the command parameters.
 *
 * @param    entry_type      Entry type:
 *                               - FS_ENTRY_TYPE_FILE      File
 *                               - FS_ENTRY_TYPE_DIR       Directory
 *
 * @return   RTOS_ERR_NONE,  if NO error(s).
 *           SHELL_EXEC_ERR, otherwise.
 *******************************************************************************************************/

#if (FS_CORE_CFG_RD_ONLY_EN == DEF_DISABLED)
static CPU_INT16S FSShell_EntryRemove(CPU_INT16U      argc,
                                      CPU_CHAR        *argv[],
                                      SHELL_OUT_FNCT  out_fnct,
                                      SHELL_CMD_PARAM *p_cmd_param,
                                      FS_FLAGS        entry_type)
{
  CPU_CHAR *p_path;
  void     *p_opt;
  RTOS_ERR err;

  RTOS_ERR_SET(err, RTOS_ERR_NONE);
  p_opt = p_cmd_param->OutputOptPtr;

  //                                                               ------------------ CHK ARGUMENTS -------------------
  if (argc != 2u) {
    FS_SHELL_PRINTF(out_fnct, p_opt, "fs_rm: usage: fs_rm [%s]\r\n",
                    entry_type == FS_ENTRY_TYPE_FILE ? "file" : "dir");
    return (SHELL_EXEC_ERR);
  }

  if (Str_Cmp_N(argv[1], FS_SHELL_STR_HELP, 3u) == 0) {
    FS_SHELL_PRINTF(out_fnct, p_opt, "fs_rm: usage: fs_rm [%s]\r\n",
                    entry_type == FS_ENTRY_TYPE_FILE ? "file" : "dir");
    FS_SHELL_PRINTF(out_fnct, p_opt, "              Remove [%s].\r\n",
                    entry_type == FS_ENTRY_TYPE_FILE ? "file" : "dir");
    return (SHELL_EXEC_ERR_NONE);
  }

  p_path = argv[1];

  //                                                               -------------------- REMOVE FILE -------------------
  FSEntry_Del(FSShell_Data.WorkingDirHandle,
              p_path,
              entry_type,
              &err);

  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    FS_SHELL_PRINTF(out_fnct, p_opt, "Could not remove entry.\r\n");
    FS_SHELL_ERR_PRINT(out_fnct, p_opt, err);
    return (SHELL_EXEC_ERR);
  }

  return (SHELL_EXEC_ERR_NONE);
}
#endif

/****************************************************************************************************//**
 *                                           FSShell_VolInfoDisplay()
 *
 * @brief    Display information about a given volume.
 *
 * @param    vol_handle      Handle to a volume.
 *
 * @param    p_cmd_param     Pointer to the command parameters.
 *
 * @param    out_fnct        The output function.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *******************************************************************************************************/
static void FSShell_VolInfoDisplay(FS_VOL_HANDLE   vol_handle,
                                   SHELL_CMD_PARAM *p_cmd_param,
                                   SHELL_OUT_FNCT  out_fnct,
                                   RTOS_ERR        *p_err)
{
  void              *p_opt;
  FS_BLK_DEV_HANDLE blk_dev_handle;
  FS_PARTITION_NBR  partition_nbr;
  FS_LB_SIZE        lb_size;
  CPU_CHAR          name[FS_SHELL_MEDIA_NAME_MAX_LEN];
  FS_VOL_INFO       vol_info;
  CPU_SIZE_T        name_len;
  RTOS_ERR          err;

  p_opt = p_cmd_param->OutputOptPtr;

  blk_dev_handle = FSVol_BlkDevGet(vol_handle);

  lb_size = FSBlkDev_LbSizeGet(blk_dev_handle, &err);

  FSMedia_NameGet(FSBlkDev_MediaGet(blk_dev_handle),
                  name,
                  sizeof(name),
                  &err);

  partition_nbr = FSVol_PartitionNbrGet(vol_handle, p_err);

  FSVol_Query(vol_handle,
              &vol_info,
              DEF_NULL,
              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    FS_SHELL_PRINTF(out_fnct, p_opt, "Could not query vol.\r\n");
    FS_SHELL_ERR_PRINT(out_fnct, p_opt, *p_err);
    return;
  }

  name_len = Str_Len(name);

  FS_SHELL_PRINTF(out_fnct, p_opt, "%s:%u%.*s", name, partition_nbr, 20 - name_len - 2, "");
  FS_SHELL_PRINTF(out_fnct, p_opt, "%.-12u", vol_info.UsedSecCnt * (lb_size / 512u));
  FS_SHELL_PRINTF(out_fnct, p_opt, "%.-12u", vol_info.FreeSecCnt * (lb_size / 512u));
  FS_SHELL_PRINTF(out_fnct, p_opt, "%.-12u", vol_info.TotSecCnt  * (lb_size / 512u));

  FSVol_NameGet(vol_handle,
                name,
                FS_CORE_CFG_MAX_VOL_NAME_LEN,
                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    FS_SHELL_PRINTF(out_fnct, p_opt, "Could not get vol name.\r\n");
    FS_SHELL_ERR_PRINT(out_fnct, p_opt, *p_err);
    return;
  }

  FS_SHELL_PRINTF(out_fnct, p_opt, "%s\r\n", name)
}

/****************************************************************************************************//**
 *                                           FSShell_PrintfOutputCb()
 *
 * @brief    Callback for outputting a character.
 *
 * @param    c       Character to output.
 *
 * @param    p_arg   Pointer to argument passed to output function.
 *
 * @return   0, if NO error(s).
 *******************************************************************************************************/
static int FSShell_PrintfOutputCb(int c, void *p_arg)
{
  FS_SHELL_OUTPUT_FUNC_CB_DATA *p_out_data;
  p_out_data = (FS_SHELL_OUTPUT_FUNC_CB_DATA *)p_arg;

  FSShell_Data.LineBufPtr[FSShell_Data.LineBufCurIx] = (CPU_CHAR)c;
  FSShell_Data.LineBufCurIx += 1u;

  if (((FSShell_Data.LineBufCurIx + 1) == FS_SHELL_LINE_BUF_MAX_SIZE) || (c == '\0')) {
    //                                                             Some ShellOutFunc expect a null-terminator
    FSShell_Data.LineBufPtr[FSShell_Data.LineBufCurIx] = '\0';
    FSShell_Data.LineBufCurIx += 1u;

    p_out_data->ShellOutFunc(FSShell_Data.LineBufPtr,
                             FSShell_Data.LineBufCurIx,
                             p_out_data->OutOptPtr);
    FSShell_Data.LineBufCurIx = 0u;
  }

  return (0);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_FS_AVAIL && RTOS_MODULE_COMMON_SHELL_AVAIL
