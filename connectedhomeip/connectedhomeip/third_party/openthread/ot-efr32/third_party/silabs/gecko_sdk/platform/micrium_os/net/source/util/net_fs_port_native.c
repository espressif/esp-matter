/***************************************************************************//**
 * @file
 * @brief Network File System Port Layer - Micrium Os File System
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

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_NET_AVAIL) \
  && defined(RTOS_MODULE_FS_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <em_core.h>
#include  <sl_sleeptimer.h>
#include  <sl_status.h>

#include  <common/source/logging/logging_priv.h>
#include  <common/include/rtos_utils.h>
#include  <common/source/rtos/rtos_utils_priv.h>

#include  <net/source/tcpip/net_priv.h>
#include  <net/include/net_fs.h>

#include  <fs/include/fs_core.h>
#include  <fs/include/fs_core_entry.h>
#include  <fs/include/fs_core_file.h>
#include  <fs/include/fs_core_dir.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <fs_core_cfg.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                           CONFIGURATION ERROR
 ********************************************************************************************************
 *******************************************************************************************************/

#if (FS_CORE_CFG_DIR_EN != DEF_ENABLED)
#error  FS_CORE_CFG_DIR_EN not #defined in fs_core_cfg.h [MUST be DEF_ENABLED ]
#endif

#if (FS_CORE_CFG_TASK_WORKING_DIR_EN != DEF_ENABLED)
#error  FS_CORE_CFG_TASK_WORKING_DIR_EN not #defined in fs_core_cfg.h [MUST be DEF_ENABLED ]
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                        (NET, HTTP)
#define  RTOS_MODULE_CUR                     RTOS_CFG_MODULE_NET

#define  NET_FS_FILE_BUF_SIZE                512u

#define  NET_FS_MAX_PATH                      2048u

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef  struct  net_fs_dir_handle {
  FS_DIR_HANDLE DirHandle;
} NET_FS_DIR_HANDLE;

typedef  struct  net_fs_file_handle {
  FS_FILE_HANDLE FileHandle;
  CPU_CHAR       *Filename;
} NET_FS_FILE_HANDLE;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static CPU_CHAR NetFS_CfgPathGetSepChar(void);

//                                                                 --------------- DIRECTORY FUNCTIONS ----------------
static void *NetFS_DirOpen(CPU_CHAR *p_name);

static void NetFS_DirClose(void *p_dir);

static CPU_BOOLEAN NetFS_DirRd(void         *p_dir,
                               NET_FS_ENTRY *p_entry);

//                                                                 ----------------- ENTRY FUNCTIONS ------------------
static CPU_BOOLEAN NetFS_EntryCreate(CPU_CHAR    *p_name,
                                     CPU_BOOLEAN dir);

static CPU_BOOLEAN NetFS_EntryDel(CPU_CHAR    *p_name,
                                  CPU_BOOLEAN file);

static CPU_BOOLEAN NetFS_EntryRename(CPU_CHAR *p_name_old,
                                     CPU_CHAR *p_name_new);

static CPU_BOOLEAN NetFS_EntryTimeSet(CPU_CHAR         *p_name,
                                      NET_FS_DATE_TIME *p_time);

//                                                                 ------------------ FILE FUNCTIONS ------------------
static void *NetFS_FileOpen(CPU_CHAR           *p_name,
                            NET_FS_FILE_MODE   mode,
                            NET_FS_FILE_ACCESS access);

static void NetFS_FileClose(void *p_file);

static CPU_BOOLEAN NetFS_FileRd(void       *p_file,
                                void       *p_dest,
                                CPU_SIZE_T size,
                                CPU_SIZE_T *p_size_rd);

static CPU_BOOLEAN NetFS_FileWr(void       *p_file,
                                void       *p_src,
                                CPU_SIZE_T size,
                                CPU_SIZE_T *p_size_wr);

static CPU_BOOLEAN NetFS_FilePosSet(void       *p_file,
                                    CPU_INT32S offset,
                                    CPU_INT08U origin);

static CPU_BOOLEAN NetFS_FileSizeGet(void       *p_file,
                                     CPU_INT32U *p_size);

static CPU_BOOLEAN NetFS_FileDateTimeCreateGet(void             *p_file,
                                               NET_FS_DATE_TIME *p_time);

static CPU_BOOLEAN NetFS_WorkingFolderGet(CPU_CHAR   *p_path,
                                          CPU_SIZE_T path_len_max);

static CPU_BOOLEAN NetFS_WorkingFolderSet(CPU_CHAR *p_path);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           FILE SYSTEM API
 *
 * Note(s) : (1) File system API structures are used by network applications during calls.  This API structure
 *               allows network application to call specific file system functions via function pointer instead
 *               of by name.  This enables the network application suite to compile & operate with multiple
 *               file system.
 *
 *           (2) In most cases, the API structure provided below SHOULD suffice for most network application
 *               exactly as is with the exception that the API structure's name which MUST be unique &
 *               SHOULD clearly identify the file system being implemented.  For example, the Micrium file system
 *               V4's API structure should be named NetFS_API_FS_V4[].
 *
 *               The API structure MUST also be externally declared in the File system port header file
 *               ('net_fs_&&&.h') with the exact same name & type.
 *******************************************************************************************************/
//                                                                 Net FS static API fnct ptrs :
const NET_FS_API NetFS_API_Native = {
  NetFS_CfgPathGetSepChar,                                              // Path sep char.
  NetFS_FileOpen,                                                       // Open
  NetFS_FileClose,                                                      // Close
  NetFS_FileRd,                                                         // Rd
  NetFS_FileWr,                                                         // Wr
  NetFS_FilePosSet,                                                     // Set position
  NetFS_FileSizeGet,                                                    // Get size
  NetFS_DirOpen,                                                        // Open  dir
  NetFS_DirClose,                                                       // Close dir
  NetFS_DirRd,                                                          // Rd    dir
  NetFS_EntryCreate,                                                    // Entry create
  NetFS_EntryDel,                                                       // Entry del
  NetFS_EntryRename,                                                    // Entry rename
  NetFS_EntryTimeSet,                                                   // Entry set time
  NetFS_FileDateTimeCreateGet,                                          // Create date time
  NetFS_WorkingFolderGet,                                               // Get working folder
  NetFS_WorkingFolderSet,                                               // Set working folder
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static MEM_DYN_POOL NetFS_DirHandlePool;
static MEM_DYN_POOL NetFS_FileHandlePool;

static CPU_BOOLEAN NetFS_DirHandlePoolInit = DEF_NO;
static CPU_BOOLEAN NetFS_FileHandlePoolInit = DEF_NO;

#if (FS_CORE_CFG_FILE_BUF_EN == DEF_ENABLED)
static CPU_DATA NetFS_FileBuf[NET_FS_FILE_BUF_SIZE / sizeof(CPU_DATA)];

static FS_FILE_HANDLE NetFS_BufFileHandle = { 0 };
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           NetFS_CfgPathGetSepChar()
 *
 * @brief    Get path separator character
 *
 * @return   separator charater.
 *
 *           Application.
 *******************************************************************************************************/
static CPU_CHAR NetFS_CfgPathGetSepChar(void)
{
  return ((CPU_CHAR)'/');
}

/****************************************************************************************************//**
 *                                               NetFS_DirOpen()
 *
 * @brief    Open a directory.
 *
 * @param    p_name  Name of the directory.
 *
 * @return   Pointer to a directory, if NO errors.
 *           Pointer to NULL,        otherwise.
 *
 *           Application.
 *******************************************************************************************************/
static void *NetFS_DirOpen(CPU_CHAR *p_name)
{
  NET_FS_DIR_HANDLE *p_handle = DEF_NULL;
#if  (FS_CORE_CFG_DIR_EN == DEF_ENABLED)
  FS_DIR_HANDLE dir_handle = FSDir_NullHandle;
  FS_FLAGS      fs_flags = 0;
  CPU_BOOLEAN   is_init = DEF_NO;
  RTOS_ERR      err;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG((p_name != DEF_NULL), RTOS_ERR_NULL_PTR, DEF_NULL);

  CORE_ENTER_ATOMIC();
  is_init = NetFS_DirHandlePoolInit;
  CORE_EXIT_ATOMIC();

  LOG_VRB(("NET-FS : Open Directory: ", (s)p_name));

  if (is_init == DEF_NO) {
    LOG_VRB(("NET-FS : Initialize Directory Handle"));
    Mem_DynPoolCreate("Dir Handle Pool",
                      &NetFS_DirHandlePool,
                      Net_CoreDataPtr->CoreMemSegPtr,
                      sizeof(NET_FS_DIR_HANDLE),
                      sizeof(CPU_ALIGN),
                      1,
                      LIB_MEM_BLK_QTY_UNLIMITED,
                      &err);
    RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_NULL);

    CORE_ENTER_ATOMIC();
    NetFS_DirHandlePoolInit = DEF_YES;
    CORE_EXIT_ATOMIC();
  }

  LOG_VRB(("NET-FS : Get Directory Handle"));
  p_handle = (NET_FS_DIR_HANDLE *)Mem_DynPoolBlkGet(&NetFS_DirHandlePool, &err);
  RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_NULL);

  fs_flags = FS_DIR_ACCESS_MODE_NONE | FS_DIR_ACCESS_MODE_CREATE;

  //                                                               -------------------- OPEN DIR ----------------------
  dir_handle = FSDir_Open(FS_WRK_DIR_CUR,
                          p_name,
                          fs_flags,
                          &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    LOG_VRB(("NET-FS : FSDir_Open Error: ", (s)RTOS_ERR_STR_GET(RTOS_ERR_CODE_GET(err))));
    goto exit_release;
  }

  p_handle->DirHandle = dir_handle;

  goto exit;

exit_release:
  Mem_DynPoolBlkFree(&NetFS_DirHandlePool, p_handle, &err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_NULL);
  p_handle = DEF_NULL;

exit:
#else
  PP_UNUSED_PARAM(p_name);                                      // Prevent 'variable unused' compiler warning.
  RTOS_DBG_FAIL_EXEC(RTOS_ERR_NOT_AVAIL, DEF_NULL);
#endif

  return (p_handle);
}

/****************************************************************************************************//**
 *                                               NetFS_DirClose()
 *
 * @brief    Close a directory.
 *
 * @param    p_dir   Pointer to a directory.
 *
 *           Application.
 *******************************************************************************************************/
static void NetFS_DirClose(void *p_dir)
{
#if  (FS_CORE_CFG_DIR_EN == DEF_ENABLED)
  NET_FS_DIR_HANDLE *p_handle = DEF_NULL;
  RTOS_ERR          err;

  LOG_VRB(("NET-FS : Close Directory"));
  RTOS_ASSERT_DBG((p_dir != DEF_NULL), RTOS_ERR_NULL_PTR,; );

  p_handle = (NET_FS_DIR_HANDLE *)p_dir;

  //                                                               -------------------- CLOSE DIR ---------------------
  FSDir_Close(p_handle->DirHandle,
              &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    LOG_VRB(("NET-FS : FSDir_Close Error: ", (s)RTOS_ERR_STR_GET(RTOS_ERR_CODE_GET(err))));
  }

  RTOS_ERR_SET(err, RTOS_ERR_NONE);
  LOG_VRB(("NET-FS : Free Directory Handle"));
  Mem_DynPoolBlkFree(&NetFS_DirHandlePool, p_handle, &err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

#else
  PP_UNUSED_PARAM(p_dir);                                       // Prevent 'variable unused' compiler warning.
  RTOS_DBG_FAIL_EXEC(RTOS_ERR_NOT_AVAIL,; );
#endif
}

/****************************************************************************************************//**
 *                                               NetFS_DirRd()
 *
 * @brief    Read a directory entry from a directory.
 *
 * @param    p_dir       Pointer to a directory.
 *
 * @param    p_entry     Pointer to variable that will receive directory entry information.
 *
 * @return   DEF_OK,   if directory entry read.
 *           DEF_FAIL, otherwise.
 *
 *           Application.
 *******************************************************************************************************/
static CPU_BOOLEAN NetFS_DirRd(void         *p_dir,
                               NET_FS_ENTRY *p_entry)
{
#if  (FS_CORE_CFG_DIR_EN == DEF_ENABLED)
  NET_FS_DIR_HANDLE    *p_handle;
  CPU_INT16U           attrib;
  FS_ENTRY_INFO        entry_info;
  sl_sleeptimer_date_t date;
  RTOS_ERR             err;
  sl_status_t          status;

  RTOS_ASSERT_DBG((p_dir != DEF_NULL), RTOS_ERR_NULL_PTR, DEF_FAIL);
  RTOS_ASSERT_DBG((p_entry != DEF_NULL), RTOS_ERR_NULL_PTR, DEF_FAIL);
  RTOS_ASSERT_DBG((p_entry->NamePtr != DEF_NULL), RTOS_ERR_NULL_PTR, DEF_FAIL);

  p_handle = (NET_FS_DIR_HANDLE *)p_dir;

  LOG_VRB(("NET-FS : Read Directory"));

  //                                                               ---------------------- RD DIR ----------------------
  FSDir_Rd(p_handle->DirHandle,
           &entry_info,
           p_entry->NamePtr,
           p_entry->NameSize,
           &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    LOG_VRB(("NET-FS : FSDir_Rd Error: ", (s)RTOS_ERR_STR_GET(RTOS_ERR_CODE_GET(err))));
    return (DEF_FAIL);
  }

  attrib = DEF_BIT_NONE;
  if (entry_info.Attrib.Rd == DEF_YES) {
    attrib |= NET_FS_ENTRY_ATTRIB_RD;
  }
  if (entry_info.Attrib.Wr == DEF_YES) {
    attrib |= NET_FS_ENTRY_ATTRIB_WR;
  }
  if (entry_info.Attrib.Hidden == DEF_YES) {
    attrib |= NET_FS_ENTRY_ATTRIB_HIDDEN;
  }
  if (entry_info.Attrib.IsDir == DEF_YES) {
    attrib |= NET_FS_ENTRY_ATTRIB_DIR;
  }
  p_entry->Attrib = attrib;
  p_entry->Size = entry_info.Size;

  status = sl_sleeptimer_convert_time_to_date(entry_info.DateTimeCreate, 0, &date);
  if (status == SL_STATUS_OK) {
    p_entry->DateTimeCreate.Sec = date.sec;
    p_entry->DateTimeCreate.Min = date.min;
    p_entry->DateTimeCreate.Hr = date.hour;
    p_entry->DateTimeCreate.Day = date.month_day;
    p_entry->DateTimeCreate.Month = date.month;
    p_entry->DateTimeCreate.Yr = date.year;
  } else {
    p_entry->DateTimeCreate.Sec = 0u;
    p_entry->DateTimeCreate.Min = 0u;
    p_entry->DateTimeCreate.Hr = 0u;
    p_entry->DateTimeCreate.Day = 1u;
    p_entry->DateTimeCreate.Month = 1u;
    p_entry->DateTimeCreate.Yr = 0u;
  }

  return (DEF_OK);

#else
  PP_UNUSED_PARAM(p_dir);                                       // Prevent 'variable unused' compiler warning.
  PP_UNUSED_PARAM(p_entry);

  RTOS_DBG_FAIL_EXEC(RTOS_ERR_NOT_AVAIL, DEF_FAIL);
  return (DEF_FAIL);
#endif
}

/****************************************************************************************************//**
 *                                           NetFS_EntryCreate()
 *
 * @brief    Create a file or directory.
 *
 * @param    p_name  Name of the entry.
 *
 * @param    dir     Indicates whether the new entry shall be a directory :
 *                   DEF_YES, if the entry shall be a directory.
 *                   DEF_NO,  if the entry shall be a file.
 *
 * @return   DEF_OK,   if entry created.
 *           DEF_FAIL, otherwise.
 *
 *           Application.
 *******************************************************************************************************/
static CPU_BOOLEAN NetFS_EntryCreate(CPU_CHAR    *p_name,
                                     CPU_BOOLEAN dir)
{
#if (FS_CORE_CFG_RD_ONLY_EN != DEF_ENABLED)
  CPU_BOOLEAN ok = DEF_OK;
  FS_FLAGS    entry_type;
  RTOS_ERR    err;

  RTOS_ASSERT_DBG((p_name != DEF_NULL), RTOS_ERR_NULL_PTR, DEF_FAIL);

  //                                                               ----------------- CREATE FILE/DIR ------------------
  if (dir == DEF_YES) {
    entry_type = FS_ENTRY_TYPE_DIR;
    LOG_VRB(("NET-FS : Create Directory : ", (s)p_name));
  } else {
    entry_type = FS_ENTRY_TYPE_FILE;
    LOG_VRB(("NET-FS : Create File : ", (s)p_name));
  }

  FSEntry_Create(FS_WRK_DIR_CUR,
                 p_name,
                 entry_type,
                 DEF_YES,
                 &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    LOG_VRB(("NET-FS : FSEntry_Create() Error: ", (s)RTOS_ERR_STR_GET(RTOS_ERR_CODE_GET(err))));
    ok = DEF_FAIL;
  }

  return (ok);

#else
  PP_UNUSED_PARAM(p_name);                                      // Prevent 'variable unused' compiler warning.
  PP_UNUSED_PARAM(dir);

  RTOS_DBG_FAIL_EXEC(RTOS_ERR_NOT_AVAIL, DEF_FAIL);
  return (DEF_FAIL);
#endif
}

/****************************************************************************************************//**
 *                                               NetFS_EntryDel()
 *
 * @brief    Delete a file or directory.
 *
 * @param    p_name  Name of the entry.
 *
 * @param    file    Indicates whether the entry MAY be a file :
 *                   DEF_YES, if the entry MAY be a file.
 *                   DEF_NO,  if the entry is NOT a file.
 *
 * @return   DEF_OK,   if entry created.
 *           DEF_FAIL, otherwise.
 *
 *           Application.
 *******************************************************************************************************/
static CPU_BOOLEAN NetFS_EntryDel(CPU_CHAR    *p_name,
                                  CPU_BOOLEAN file)
{
#if (FS_CORE_CFG_RD_ONLY_EN != DEF_ENABLED)
  CPU_BOOLEAN ok = DEF_OK;
  FS_FLAGS    entry_type;
  RTOS_ERR    err;

  RTOS_ASSERT_DBG((p_name != DEF_NULL), RTOS_ERR_NULL_PTR, DEF_FAIL);

  //                                                               --------------------- DEL FILE ---------------------

  if (file == DEF_YES) {
    entry_type = FS_ENTRY_TYPE_ANY;
    LOG_VRB(("NET-FS : Delete File : ", (s)p_name));
  } else {
    entry_type = FS_ENTRY_TYPE_DIR;
    LOG_VRB(("NET-FS : Delete Directory : ", (s)p_name));
  }

  FSEntry_Del(FS_WRK_DIR_CUR,
              p_name,
              entry_type,
              &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    LOG_VRB(("NET-FS : FSEntry_Del() Error: ", (s)RTOS_ERR_STR_GET(RTOS_ERR_CODE_GET(err))));
    ok = DEF_FAIL;
  }

  return (ok);

#else
  PP_UNUSED_PARAM(p_name);                                      // Prevent 'variable unused' compiler warning.
  PP_UNUSED_PARAM(file);

  RTOS_DBG_FAIL_EXEC(RTOS_ERR_NOT_AVAIL, DEF_FAIL);
  return (DEF_FAIL);
#endif
}

/****************************************************************************************************//**
 *                                           NetFS_EntryRename()
 *
 * @brief    Rename a file or directory.
 *
 * @param    p_name_old  Old path of the entry.
 *
 * @param    p_name_new  New path of the entry.
 *
 * @return   DEF_OK,   if entry renamed.
 *           DEF_FAIL, otherwise.
 *
 *           Application.
 *******************************************************************************************************/
static CPU_BOOLEAN NetFS_EntryRename(CPU_CHAR *p_name_old,
                                     CPU_CHAR *p_name_new)
{
#if (FS_CORE_CFG_RD_ONLY_EN != DEF_ENABLED)
  CPU_BOOLEAN ok = DEF_OK;
  RTOS_ERR    err;

  RTOS_ASSERT_DBG((p_name_old != DEF_NULL), RTOS_ERR_NULL_PTR, DEF_FAIL);
  RTOS_ASSERT_DBG((p_name_new != DEF_NULL), RTOS_ERR_NULL_PTR, DEF_FAIL);

  LOG_VRB(("NET-FS : Rename Entry : Old Name= ", (s)p_name_old, " New Name=", (s)p_name_new));

  //                                                               ------------------- RENAME FILE --------------------
  FSEntry_Rename(FS_WRK_DIR_CUR,
                 p_name_old,
                 FS_WRK_DIR_CUR,
                 p_name_new,
                 DEF_YES,
                 &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    LOG_VRB(("NET-FS : FSEntry_Rename() Error: ", (s)RTOS_ERR_STR_GET(RTOS_ERR_CODE_GET(err))));
    ok = DEF_FAIL;
  }

  return (ok);

#else
  PP_UNUSED_PARAM(p_name_old);                                  // Prevent 'variable unused' compiler warning.
  PP_UNUSED_PARAM(p_name_new);

  RTOS_DBG_FAIL_EXEC(RTOS_ERR_NOT_AVAIL, DEF_FAIL);
  return (DEF_FAIL);
#endif
}

/****************************************************************************************************//**
 *                                           NetFS_EntryTimeSet()
 *
 * @brief    Set a file or directory's date/time.
 *
 * @param    p_name  Name of the entry.
 *
 * @param    p_time  Pointer to date/time.
 *
 * @return   DEF_OK,   if date/time set.
 *           DEF_FAIL, otherwise.
 *
 *           Application.
 *******************************************************************************************************/
static CPU_BOOLEAN NetFS_EntryTimeSet(CPU_CHAR         *p_name,
                                      NET_FS_DATE_TIME *p_time)
{
#if (FS_CORE_CFG_RD_ONLY_EN != DEF_ENABLED)
  CPU_BOOLEAN   ok = DEF_OK;
  sl_sleeptimer_date_t date;
  RTOS_ERR      err;

  RTOS_ASSERT_DBG((p_name != DEF_NULL), RTOS_ERR_NULL_PTR, DEF_FAIL);
  RTOS_ASSERT_DBG((p_time != DEF_NULL), RTOS_ERR_NULL_PTR, DEF_FAIL);

  LOG_VRB(("NET-FS : Set Entry time : ", (s)p_name));

  //                                                               ------------------ SET DATE/TIME -------------------
  date.sec = p_time->Sec;
  date.min = p_time->Min;
  date.hour = p_time->Hr;
  date.month_day = p_time->Day;
  date.month = p_time->Month;
  date.year = p_time->Yr;

  sl_fs_entry_time_set(FS_WRK_DIR_CUR,
                       p_name,
                       &date,
                       FS_DATE_TIME_ALL,
                       &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    LOG_VRB(("NET-FS : FSEntry_TimeSet() Error: ", (s)RTOS_ERR_STR_GET(RTOS_ERR_CODE_GET(err))));
    ok = DEF_FAIL;
  }

  return (ok);

#else
  PP_UNUSED_PARAM(p_name);                                      // Prevent 'variable unused' compiler warning.
  PP_UNUSED_PARAM(p_time);

  RTOS_DBG_FAIL_EXEC(RTOS_ERR_NOT_AVAIL, DEF_FAIL);
  return (DEF_FAIL);
#endif
}

/****************************************************************************************************//**
 *                                               NetFS_FileOpen()
 *
 * @brief    Open a file.
 *
 * @param    p_name  Name of the file.
 *
 * @param    mode    Mode of the file :
 *                   NET_FS_FILE_MODE_APPEND        Open existing file at end-of-file OR create new file.
 *                   NET_FS_FILE_MODE_CREATE        Create new file OR overwrite existing file.
 *                   NET_FS_FILE_MODE_CREATE_NEW    Create new file OR return error if file exists.
 *                   NET_FS_FILE_MODE_OPEN          Open existing file.
 *                   NET_FS_FILE_MODE_TRUNCATE      Truncate existing file to zero length.
 *
 * @param    access  Access rights of the file :
 *                   NET_FS_FILE_ACCESS_RD          Open file in read           mode.
 *                   NET_FS_FILE_ACCESS_RD_WR       Open file in read AND write mode.
 *                   NET_FS_FILE_ACCESS_WR          Open file in          write mode
 *
 * @return   Pointer to a file, if NO errors.
 *           Pointer to NULL,   otherwise.
 *
 *           Application.
 *******************************************************************************************************/
static void *NetFS_FileOpen(CPU_CHAR           *p_name,
                            NET_FS_FILE_MODE   mode,
                            NET_FS_FILE_ACCESS access)
{
  NET_FS_FILE_HANDLE *p_handle = DEF_NULL;
  FS_FILE_HANDLE     file_handle = FSFile_NullHandle;
  FS_FLAGS           mode_flags;
  CPU_BOOLEAN        is_init = DEF_NO;
  RTOS_ERR           err;
#if (FS_CORE_CFG_FILE_BUF_EN == DEF_ENABLED)
  CORE_DECLARE_IRQ_STATE;
#endif

  LOG_VRB(("NET-FS : Open File : ", (s)p_name));
  RTOS_ASSERT_DBG((p_name != DEF_NULL), RTOS_ERR_NULL_PTR, DEF_NULL);

  //                                                               -------------------- OPEN FILE ---------------------
  mode_flags = FS_FILE_ACCESS_MODE_NONE;

  switch (mode) {
    case NET_FS_FILE_MODE_APPEND:
      DEF_BIT_SET(mode_flags, FS_FILE_ACCESS_MODE_APPEND);
      DEF_BIT_SET(mode_flags, FS_FILE_ACCESS_MODE_CREATE);
      LOG_VRB(("NET-FS : Mode = append"));
      break;

    case NET_FS_FILE_MODE_CREATE:
      DEF_BIT_SET(mode_flags, FS_FILE_ACCESS_MODE_CREATE);
      DEF_BIT_SET(mode_flags, FS_FILE_ACCESS_MODE_TRUNCATE);
      LOG_VRB(("NET-FS : Mode = create"));
      break;

    case NET_FS_FILE_MODE_CREATE_NEW:
      DEF_BIT_SET(mode_flags, FS_FILE_ACCESS_MODE_CREATE);
      DEF_BIT_SET(mode_flags, FS_FILE_ACCESS_MODE_EXCL);
      LOG_VRB(("NET-FS : Mode = create new"));
      break;

    case NET_FS_FILE_MODE_OPEN:
      LOG_VRB(("NET-FS : Mode = open"));
      break;

    case NET_FS_FILE_MODE_TRUNCATE:
      LOG_VRB(("NET-FS : Mode = truncate"));
      DEF_BIT_SET(mode_flags, FS_FILE_ACCESS_MODE_TRUNCATE);
      break;

    default:
      goto exit;
  }

  switch (access) {
    case NET_FS_FILE_ACCESS_RD:
      DEF_BIT_SET(mode_flags, FS_FILE_ACCESS_MODE_RD);
      LOG_VRB(("NET-FS : Access = read"));
      break;

    case NET_FS_FILE_ACCESS_RD_WR:
      DEF_BIT_SET(mode_flags, FS_FILE_ACCESS_MODE_RD);
      DEF_BIT_SET(mode_flags, FS_FILE_ACCESS_MODE_WR);
      LOG_VRB(("NET-FS : Access = read-write"));
      break;

    case NET_FS_FILE_ACCESS_WR:
      DEF_BIT_SET(mode_flags, FS_FILE_ACCESS_MODE_WR);
      LOG_VRB(("NET-FS : Access = write"));
      break;

    default:
      goto exit;
  }

  CORE_ENTER_ATOMIC();
  is_init = NetFS_FileHandlePoolInit;
  CORE_EXIT_ATOMIC();

  if (is_init == DEF_NO) {
    LOG_VRB(("NET-FS : Initialize File handle pool"));
    Mem_DynPoolCreate("File Handle Pool",
                      &NetFS_FileHandlePool,
                      Net_CoreDataPtr->CoreMemSegPtr,
                      sizeof(NET_FS_FILE_HANDLE),
                      sizeof(CPU_ALIGN),
                      1,
                      LIB_MEM_BLK_QTY_UNLIMITED,
                      &err);
    if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
      goto exit;
    }

    CORE_ENTER_ATOMIC();
    NetFS_FileHandlePoolInit = DEF_YES;
    CORE_EXIT_ATOMIC();
  }

  LOG_VRB(("NET-FS : Get a File handle"));
  p_handle = (NET_FS_FILE_HANDLE *)Mem_DynPoolBlkGet(&NetFS_FileHandlePool, &err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_NULL);

  file_handle = FSFile_Open(FS_WRK_DIR_CUR,
                            p_name,
                            mode_flags,
                            &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    LOG_VRB(("NET-FS : FSFile_Open() Error: ", (s)RTOS_ERR_STR_GET(RTOS_ERR_CODE_GET(err))));
    goto exit_release;
  }

  p_handle->FileHandle = file_handle;
  p_handle->Filename = p_name;

#if (FS_CORE_CFG_FILE_BUF_EN == DEF_ENABLED)

  CORE_ENTER_ATOMIC();
  if (FS_FILE_HANDLE_IS_NULL(NetFS_BufFileHandle)) {
    NetFS_BufFileHandle = file_handle;
    CORE_EXIT_ATOMIC();

    FSFile_BufAssign(file_handle, &NetFS_FileBuf, FS_FILE_BUF_MODE_RD_WR, NET_FS_FILE_BUF_SIZE, &err);
    if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
      CORE_ENTER_ATOMIC();
      NetFS_BufFileHandle = FSFile_NullHandle;
      CORE_EXIT_ATOMIC();
    }
  } else {
    CORE_EXIT_ATOMIC();
  }
#endif

  goto exit;

exit_release:
  LOG_VRB(("NET-FS : Free File handle"));
  Mem_DynPoolBlkFree(&NetFS_FileHandlePool, p_handle, &err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_NULL);
  p_handle = DEF_NULL;

exit:
  return (p_handle);
}

/****************************************************************************************************//**
 *                                               NetFS_FileClose()
 *
 * @brief    Close a file.
 *
 * @param    p_file  Pointer to a file.
 *
 *           Application.
 *******************************************************************************************************/
static void NetFS_FileClose(void *p_file)
{
  NET_FS_FILE_HANDLE *p_handle = DEF_NULL;
  RTOS_ERR           err;
#if (FS_CORE_CFG_FILE_BUF_EN == DEF_ENABLED)
  CPU_BOOLEAN is_same;
  CORE_DECLARE_IRQ_STATE;
#endif

  RTOS_ASSERT_DBG((p_file != DEF_NULL), RTOS_ERR_NULL_PTR,; );

  p_handle = (NET_FS_FILE_HANDLE *)p_file;
  LOG_VRB(("NET-FS : Close File : ", (s)p_handle->Filename));

#if (FS_CORE_CFG_FILE_BUF_EN == DEF_ENABLED)
  CORE_ENTER_ATOMIC();
  is_same = Mem_Cmp(&NetFS_BufFileHandle, &p_handle->FileHandle, sizeof(FS_FILE_HANDLE));
  if (is_same == DEF_YES) {
    NetFS_BufFileHandle = FSFile_NullHandle;
  }
  CORE_EXIT_ATOMIC();
#endif

  RTOS_ERR_SET(err, RTOS_ERR_NONE);
  //                                                               -------------------- CLOSE FILE --------------------
  FSFile_Close(p_handle->FileHandle, &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    LOG_VRB(("NET-FS : FSFile_Close() Error: ", (s)RTOS_ERR_STR_GET(RTOS_ERR_CODE_GET(err))));
  }

  p_handle->FileHandle = FS_FILE_NULL;

  LOG_VRB(("NET-FS : Free File handle."));
  Mem_DynPoolBlkFree(&NetFS_FileHandlePool, p_handle, &err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}

/****************************************************************************************************//**
 *                                               NetFS_FileRd()
 *
 * @brief    Read from a file.
 *
 * @param    p_file      Pointer to a file.
 *
 * @param    p_dest      Pointer to destination buffer.
 *
 * @param    size        Number of octets to read.
 *
 * @param    p_size_rd   Pointer to variable that will receive the number of octets read.
 *
 * @return   DEF_OK,   if no error occurred during read (see Note #2).
 *           DEF_FAIL, otherwise.
 *
 *           Application.
 *
 * @note         (1) Pointers to variables that return values MUST be initialized PRIOR to all other
 *               validation or function handling in case of any error(s).
 *
 * @note         (2) If the read request could not be fulfilled because the EOF was reached, the return
 *               value should be 'DEF_OK'.  The application should compare the value in 'psize_rd' to
 *               the value passed to 'size' to detect an EOF reached condition.
 *******************************************************************************************************/
static CPU_BOOLEAN NetFS_FileRd(void       *p_file,
                                void       *p_dest,
                                CPU_SIZE_T size,
                                CPU_SIZE_T *p_size_rd)
{
  NET_FS_FILE_HANDLE *p_handle = DEF_NULL;
  CPU_BOOLEAN        ok = DEF_FAIL;
  CPU_SIZE_T         size_rd = 0;
  RTOS_ERR           err;

  RTOS_ASSERT_DBG((p_file != DEF_NULL), RTOS_ERR_NULL_PTR, DEF_FAIL);
  RTOS_ASSERT_DBG((p_size_rd != DEF_NULL), RTOS_ERR_NULL_PTR, DEF_FAIL);
  RTOS_ASSERT_DBG((p_dest != DEF_NULL), RTOS_ERR_NULL_PTR, DEF_FAIL);

  p_handle = (NET_FS_FILE_HANDLE *)p_file;
  LOG_VRB(("NET-FS : Read File : ", (s)p_handle->Filename));

  *p_size_rd = 0u;                                              // Init to dflt size for err (see Note #1).

  //                                                               --------------------- RD FILE ----------------------
  size_rd = FSFile_Rd(p_handle->FileHandle,
                      p_dest,
                      size,
                      &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    LOG_VRB(("NET-FS : FSFile_Rd() Error: ", (s)RTOS_ERR_STR_GET(RTOS_ERR_CODE_GET(err))));
    ok = DEF_FAIL;
  } else {
    LOG_VRB(("NET-FS : Bytes read : ", (u)size_rd));
    ok = DEF_OK;
  }

  *p_size_rd = size_rd;

  return (ok);
}

/****************************************************************************************************//**
 *                                               NetFS_FileWr()
 *
 * @brief    Write to a file.
 *
 * @param    p_file      Pointer to a file.
 *
 * @param    p_src       Pointer to source buffer.
 *
 * @param    size        Number of octets to write.
 *
 * @param    p_size_wr   Pointer to variable that will receive the number of octets written.
 *
 * @return   DEF_OK,   if no error occurred during write.
 *           DEF_FAIL, otherwise.
 *
 *           Application.
 *
 * @note         (1) Pointers to variables that return values MUST be initialized PRIOR to all other
 *               validation or function handling in case of any error(s).
 *******************************************************************************************************/
static CPU_BOOLEAN NetFS_FileWr(void       *p_file,
                                void       *p_src,
                                CPU_SIZE_T size,
                                CPU_SIZE_T *p_size_wr)
{
#if (FS_CORE_CFG_RD_ONLY_EN != DEF_ENABLED)
  NET_FS_FILE_HANDLE *p_handle = DEF_NULL;
  CPU_BOOLEAN        ok = DEF_OK;
  CPU_SIZE_T         size_wr;
  RTOS_ERR           err;
#endif

  RTOS_ASSERT_DBG((p_file != DEF_NULL), RTOS_ERR_NULL_PTR, DEF_FAIL);
  RTOS_ASSERT_DBG((p_src != DEF_NULL), RTOS_ERR_NULL_PTR, DEF_FAIL);
  RTOS_ASSERT_DBG((p_size_wr != DEF_NULL), RTOS_ERR_NULL_PTR, DEF_FAIL);

  *p_size_wr = 0u;                                              // Init to dflt size (see Note #1).

  //                                                               --------------------- WR FILE ----------------------
#if (FS_CORE_CFG_RD_ONLY_EN != DEF_ENABLED)

  p_handle = (NET_FS_FILE_HANDLE *)p_file;
  LOG_VRB(("NET-FS : File Name  : ", (s)p_handle->Filename));
  LOG_VRB(("NET-FS : Write Size : ", (u)size));
  size_wr = FSFile_Wr(p_handle->FileHandle,
                      p_src,
                      size,
                      &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    LOG_VRB(("NET-FS : FSFile_Wr() Error: ", (s)RTOS_ERR_STR_GET(RTOS_ERR_CODE_GET(err))));
    ok = DEF_FAIL;
  }

  *p_size_wr = size_wr;
  LOG_VRB(("NET-FS : Bytes wrote : ", (u)size_wr));

  return (ok);

#else
  PP_UNUSED_PARAM(p_file);                                      // Prevent 'variable unused' compiler warning.
  PP_UNUSED_PARAM(p_src);
  PP_UNUSED_PARAM(size);

  RTOS_DBG_FAIL_EXEC(RTOS_ERR_NOT_AVAIL, DEF_FAIL);
  return (DEF_FAIL);
#endif
}

/****************************************************************************************************//**
 *                                           NetFS_FilePosSet()
 *
 * @brief    Set file position indicator.
 *
 * @param    p_file  Pointer to a file.
 *
 * @param    offset  Offset from the file position specified by 'origin'.
 *
 * @param    origin  Reference position for offset :
 *                   NET_FS_SEEK_ORIGIN_START    Offset is from the beginning of the file.
 *                   NET_FS_SEEK_ORIGIN_CUR      Offset is from current file position.
 *                   NET_FS_SEEK_ORIGIN_END      Offset is from the end       of the file.
 *
 * @return   DEF_OK,   if file position set.
 *           DEF_FAIL, otherwise.
 *
 *           Application.
 *******************************************************************************************************/
static CPU_BOOLEAN NetFS_FilePosSet(void       *p_file,
                                    CPU_INT32S offset,
                                    CPU_INT08U origin)
{
  NET_FS_FILE_HANDLE *p_handle = DEF_NULL;
  CPU_BOOLEAN        ok = DEF_OK;
  FS_FLAGS           fs_flags;
  RTOS_ERR           err;

  RTOS_ASSERT_DBG((p_file != DEF_NULL), RTOS_ERR_NULL_PTR, DEF_FAIL);
  p_handle = (NET_FS_FILE_HANDLE *)p_file;
  LOG_VRB(("NET-FS : Set File Position: ", (s)p_handle->Filename));
  //                                                               ------------------ SET FILE POS --------------------
  switch (origin) {
    case NET_FS_SEEK_ORIGIN_START:
      LOG_VRB(("NET-FS : origin = Start"));
      fs_flags = FS_FILE_ORIGIN_START;
      break;

    case NET_FS_SEEK_ORIGIN_CUR:
      LOG_VRB(("NET-FS : origin = Current"));
      fs_flags = FS_FILE_ORIGIN_CUR;
      break;

    case NET_FS_SEEK_ORIGIN_END:
      LOG_VRB(("NET-FS : origin = End"));
      fs_flags = FS_FILE_ORIGIN_END;
      break;

    default:
      return (DEF_FAIL);
  }

  FSFile_PosSet(p_handle->FileHandle,
                offset,
                fs_flags,
                &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    LOG_VRB(("NET-FS : FSFile_PosSet() Error: ", (s)RTOS_ERR_STR_GET(RTOS_ERR_CODE_GET(err))));
    ok = DEF_FAIL;
  }

  return (ok);
}

/****************************************************************************************************//**
 *                                           NetFS_FileSizeGet()
 *
 * @brief    Get file size.
 *
 * @param    p_file  Pointer to a file.
 *
 * @param    p_size  Pointer to variable that will receive the file size.
 *
 * @return   DEF_OK,   if file size gotten.
 *           DEF_FAIL, otherwise.
 *
 *           Application.
 *
 * @note         (1) Pointers to variables that return values MUST be initialized PRIOR to all other
 *               validation or function handling in case of any error(s).
 *******************************************************************************************************/
static CPU_BOOLEAN NetFS_FileSizeGet(void       *p_file,
                                     CPU_INT32U *p_size)
{
  NET_FS_FILE_HANDLE *p_handle = DEF_NULL;
  FS_ENTRY_INFO      info;
  RTOS_ERR           err;

  RTOS_ASSERT_DBG((p_file != DEF_NULL), RTOS_ERR_NULL_PTR, DEF_FAIL);
  RTOS_ASSERT_DBG((p_size != DEF_NULL), RTOS_ERR_NULL_PTR, DEF_FAIL);

  *p_size = 0u;                                                 // Init to dflt size for err (see Note #1).

  p_handle = (NET_FS_FILE_HANDLE *)p_file;
  LOG_VRB(("NET-FS : Get File Size: ", (s)p_handle->Filename));

  //                                                               ------------------ GET FILE SIZE -------------------
  FSFile_Query(p_handle->FileHandle,
               &info,
               &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    LOG_VRB(("NET-FS : FSFile_Query() Error: ", (s)RTOS_ERR_STR_GET(RTOS_ERR_CODE_GET(err))));
    return (DEF_FAIL);
  }

  LOG_VRB(("NET-FS : File Size : ", (u)info.Size));
  *p_size = (CPU_INT32U)info.Size;

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                       NetFS_FileDateTimeCreateGet()
 *
 * @brief    Get file creation date/time.
 *
 * @param    p_file  Pointer to a file.
 *
 * @param    p_time  Pointer to variable that will receive the date/time :
 *                   Default/epoch date/time,        if any error(s);
 *                   Current       date/time,        otherwise.
 *
 * @return   DEF_OK,   if file creation date/time gotten.
 *           DEF_FAIL, otherwise.
 *
 *           Application.
 *
 * @note         (1) Pointers to variables that return values MUST be initialized PRIOR to all other
 *               validation or function handling in case of any error(s).
 *******************************************************************************************************/
static CPU_BOOLEAN NetFS_FileDateTimeCreateGet(void             *p_file,
                                               NET_FS_DATE_TIME *p_time)
{
  NET_FS_FILE_HANDLE   *p_handle = DEF_NULL;
  sl_sleeptimer_date_t date;
  sl_sleeptimer_timestamp_t time_ts_sec;
  FS_ENTRY_INFO        info;
  RTOS_ERR             err;
  sl_status_t          status;

  RTOS_ASSERT_DBG((p_file != DEF_NULL), RTOS_ERR_NULL_PTR, DEF_FAIL);
  RTOS_ASSERT_DBG((p_time != DEF_NULL), RTOS_ERR_NULL_PTR, DEF_FAIL);

  //                                                               Init to dflt date/time for err (see Note #1).
  p_time->Yr = 0u;
  p_time->Month = 0u;
  p_time->Day = 0u;
  p_time->Hr = 0u;
  p_time->Min = 0u;
  p_time->Sec = 0u;

  p_handle = (NET_FS_FILE_HANDLE *)p_file;
  LOG_VRB(("NET-FS : Get File time created: ", (s)p_handle->Filename));

  //                                                               ---------- GET FILE CREATION DATE/TIME -------------
  FSFile_Query(p_handle->FileHandle,
               &info,
               &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    LOG_VRB(("NET-FS : FSFile_Query() Error: ", (s)RTOS_ERR_STR_GET(RTOS_ERR_CODE_GET(err))));
    return (DEF_FAIL);
  }

  time_ts_sec = info.DateTimeCreate;

  //                                                               Convert clk timestamp to date/time structure.
  status = sl_sleeptimer_convert_time_to_date(time_ts_sec, 0, &date);
  if (status != SL_STATUS_OK) {
    LOG_VRB(("NET-FS : sl_sleeptimer_convert_time_to_date() Error: converting time failed"));
    return (DEF_FAIL);
  }
  //                                                               Set each creation date/time field to be rtn'd.
  p_time->Yr = date.year;
  p_time->Month = date.month;
  p_time->Day = date.month_day;
  p_time->Hr = date.hour;
  p_time->Min = date.min;
  p_time->Sec = date.sec;

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                           NetFS_WorkingFolderGet()
 *
 * @brief    Get current working folder.
 *
 * @param    p_path  Pointer to string that will receive the working folder
 *
 * @return   DEF_OK,   if p_path successfully copied.
 *
 *           DEF_FAIL, otherwise.
 *
 *           Application.
 *
 * @note         (1) Pointers to variables that return values MUST be initialized PRIOR to all other
 *               validation or function handling in case of any error(s).
 *******************************************************************************************************/
static CPU_BOOLEAN NetFS_WorkingFolderGet(CPU_CHAR   *p_path,
                                          CPU_SIZE_T path_len_max)
{
  RTOS_ERR err;

  RTOS_ASSERT_DBG((p_path != DEF_NULL), RTOS_ERR_NULL_PTR, DEF_FAIL);

  LOG_VRB(("NET-FS : Get current workspace: "));
  FSWrkDir_PathGet(FS_WRK_DIR_CUR,
                   p_path,
                   path_len_max,
                   &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    LOG_VRB(("NET-FS : FSWrkDir_PathGet() Error: ", (s)RTOS_ERR_STR_GET(RTOS_ERR_CODE_GET(err))));
    return (DEF_FAIL);
  }

  LOG_VRB(("NET-FS : Current workspace = ", (s)p_path));

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                           NetFS_WorkingFolderSet()
 *
 * @brief    Set current working folder.
 *
 * @param    p_path  String that specifies EITHER...
 *                   (a) the absolute working directory path to set;
 *                   (b) a relative path that will be applied to the current working directory.
 *
 * @return   DEF_OK,   if file creation date/time gotten.
 *           DEF_FAIL, otherwise.
 *
 *           Application.
 *
 * @note         (1) Pointers to variables that return values MUST be initialized PRIOR to all other
 *               validation or function handling in case of any error(s).
 *******************************************************************************************************/
static CPU_BOOLEAN NetFS_WorkingFolderSet(CPU_CHAR *p_path)
{
  RTOS_ERR err;

  RTOS_ASSERT_DBG((p_path != DEF_NULL), RTOS_ERR_NULL_PTR, DEF_FAIL);

  LOG_VRB(("NET-FS : Set workspace : ", (s)p_path));

  FSWrkDir_TaskBind(FS_WRK_DIR_NULL, p_path, &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    LOG_VRB(("NET-FS : FSWrkDir_TaskBind() Error: ", (s)RTOS_ERR_STR_GET(RTOS_ERR_CODE_GET(err))));
    return (DEF_FAIL);
  }

  return (DEF_OK);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_AVAIL && RTOS_MODULE_FS_AVAIL
