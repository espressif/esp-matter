/***************************************************************************//**
 * @file
 * @brief File System - Core Directory Operations
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

#if (FS_CORE_CFG_DIR_EN == DEF_ENABLED)
#include  <fs/source/core/fs_core_dir_priv.h>
#include  <fs/source/core/fs_core_working_dir_priv.h>
#include  <fs/source/core/fs_core_priv.h>
#include  <fs/source/shared/fs_obj_priv.h>
#include  <fs/source/sys/fs_sys_priv.h>
#include  <fs/source/core/fs_core_vol_priv.h>
#include  <fs/source/core/fs_core_op_priv.h>
#include  <fs/source/shared/cleanup/cleanup_mgmt_priv.h>

//                                                                 ----------------------- EXT ------------------------
#include  <cpu/include/cpu.h>
#include  <common/include/lib_utils.h>
#include  <common/include/rtos_err.h>

#include  <em_core.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH            (FS, CORE, DIR)
#define  RTOS_MODULE_CUR         RTOS_CFG_MODULE_FS

/********************************************************************************************************
 ********************************************************************************************************
 *                                               VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

const FS_DIR_HANDLE FSDir_NullHandle = { 0 };

static MEM_DYN_POOL FSDir_DescPool;
static SLIST_MEMBER *FSDir_OpenDescListHeadPtr = DEF_NULL;

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL MACROS
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------ REFERENCE ACQUIRE/RELEASE -------------
#define  FS_DIR_WITH(dir_handle, p_err)          FS_OBJ_WITH(dir_handle, p_err, FSDir_Desc, RTOS_ERR_ENTRY_CLOSED)

#define  FS_DIR_WITH_NO_IO(dir_handle, p_err)    FS_OBJ_WITH_NO_IO(dir_handle, p_err, FSDir_Desc, RTOS_ERR_ENTRY_CLOSED)

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void FSDir_Desc_OnNullRefCnt(FS_OBJ *p_obj);

static void FSDir_Node_OnNullRefCnt(FS_OBJ *p_obj);

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
static CPU_BOOLEAN FSDir_AccessModeIsValid(FS_FLAGS mode);
#endif

static FS_OBJ_HANDLE FSDir_Desc_ToObjHandle(FS_DIR_HANDLE dir_handle);

static FS_OBJ *FSDir_Desc_ToObj(FS_DIR_DESC *p_dir_desc);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               FSDir_Open()`
 *
 * @brief    Open a directory.
 *
 * @param    wrk_dir_handle  Handle to a working directory (see Note #1).
 *
 * @param    path            Path of the directory relative to the given working directory.
 *
 * @param    mode            Directory access mode; valid bit-wise OR of one or more of the following
 *                               - FS_DIR_ACCESS_MODE_EXCL       Directory will be opened if and only
 *                                                               if it does not already exist.
 *                               - FS_DIR_ACCESS_MODE_CREATE     Directory will be created, if necessary.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_INIT
 *                               - RTOS_ERR_NOT_FOUND
 *                               - RTOS_ERR_ALREADY_EXISTS
 *                               - RTOS_ERR_TYPE_INVALID
 *                               - RTOS_ERR_NAME_INVALID
 *                               - RTOS_ERR_PARENT_NOT_DIR
 *                               - RTOS_ERR_MAX_DEPTH_EXCEEDED
 *                               - RTOS_ERR_DIR_FULL
 *                               - RTOS_ERR_WRK_DIR_CLOSED
 *                               - RTOS_ERR_VOL_CLOSED
 *                               - RTOS_ERR_BLK_DEV_CLOSED
 *                               - RTOS_ERR_VOL_FULL
 *                               - RTOS_ERR_VOL_CORRUPTED
 *                               - RTOS_ERR_BLK_DEV_CORRUPTED
 *                               - RTOS_ERR_IO
 *
 * @return   Handle to the opened directory.
 *
 * @note     (1) If no working directory has been opened with FSWrkDir_Open(), you can pass a NULL
 *               working directory handle using the #define FS_WRK_DIR_NULL.
 *******************************************************************************************************/
FS_DIR_HANDLE FSDir_Open(FS_WRK_DIR_HANDLE wrk_dir_handle,
                         const CPU_CHAR    *path,
                         FS_FLAGS          mode,
                         RTOS_ERR          *p_err)
{
  FS_DIR_DESC          *p_dir_desc = DEF_NULL;
  FS_DIR_NODE          *p_dir_node;
  FS_ENTRY_NODE_HANDLE entry_node_handle;
  FS_DIR_HANDLE        dir_handle;
  CORE_DECLARE_IRQ_STATE;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, FSDir_NullHandle);
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    //                                                             Check that Core layer is initialized.
    CORE_ENTER_ATOMIC();
    if (!FSCore_Data.IsInit) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_INIT);
      CORE_EXIT_ATOMIC();
      break;
    }
    CORE_EXIT_ATOMIC();

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
    RTOS_ASSERT_DBG_ERR_SET(FSDir_AccessModeIsValid(mode), *p_err,
                            RTOS_ERR_INVALID_ARG, FSDir_NullHandle);
#endif

    dir_handle = FSDir_NullHandle;

    BREAK_ON_ERR(entry_node_handle = FSEntry_NodeOpen(wrk_dir_handle,
                                                      (CPU_CHAR *)path,
                                                      mode,
                                                      FS_ENTRY_TYPE_DIR,
                                                      p_err));
    ON_BREAK {
      RTOS_ERR err_tmp;
      RTOS_ERR_SET(err_tmp, RTOS_ERR_NONE);
      FSEntry_NodeClose(entry_node_handle, FSDir_Node_OnNullRefCnt, &err_tmp);
      break;
    } WITH {
      if (FS_ENTRY_NODE_HANDLE_IS_NULL(entry_node_handle)) {
        BREAK_ON_ERR(p_dir_desc = (FS_DIR_DESC *)Mem_DynPoolBlkGet(&FSDir_DescPool, p_err));
        Mem_Clr(p_dir_desc, sizeof(FS_DIR_DESC));

        p_dir_desc->Id = FSObj_CurId++;
        p_dir_desc->RefCnt = 1u;
        p_dir_desc->IOCnt = 1u;
        p_dir_desc->IsClosing = DEF_NO;
        p_dir_desc->CurPosData = 0u;
        p_dir_desc->DirNodePtr = DEF_NULL;
      } else {
        p_dir_node = (FS_DIR_NODE *)entry_node_handle.EntryNodePtr;

        BREAK_ON_ERR(FS_VOL_WITH) (p_dir_node->VolHandle, p_err) {
          BREAK_ON_ERR(p_dir_desc = p_dir_node->SysPtr->DirDescAlloc(p_err));

          ON_BREAK {
            p_dir_node->SysPtr->DirDescFree(p_dir_desc);
            break;
          } WITH {
            p_dir_desc->DirNodePtr = p_dir_node;
            p_dir_desc->Id = FSObj_CurId++;
            p_dir_desc->IsClosing = DEF_NO;
            p_dir_desc->RefCnt = 1u;
            p_dir_desc->IOCnt = 1u;
            p_dir_desc->CurPosData = p_dir_node->SysPtr->VoidEntryPos;

            BREAK_ON_ERR(p_dir_node->SysPtr->DirDescInit(p_dir_desc, p_err));
          }
        }
      }

      FS_OP_LOCK_WITH(&FSCore_Data.OpLock, FS_GLOBAL_OP_OPEN_ENTRY_LIST_WR) {
        SList_Push(&FSDir_OpenDescListHeadPtr, &p_dir_desc->OpenListMember);
      }
    }

    dir_handle.DirDescPtr = p_dir_desc;
    dir_handle.DirDescId = p_dir_desc->Id;
  } WITH_SCOPE_END

  return (dir_handle);
}

/****************************************************************************************************//**
 *                                               FSDir_Close()
 *
 * @brief    Close a directory.
 *
 * @param    dir_handle  Handle to a directory.
 *
 * @param    p_err       Pointer to variable that will receive the return error code(s) from this
 *                       function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_ENTRY_CLOSED
 *******************************************************************************************************/
void FSDir_Close(FS_DIR_HANDLE dir_handle,
                 RTOS_ERR      *p_err)
{
  FS_DIR_DESC *p_desc = DEF_NULL;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  RTOS_ASSERT_CRITICAL(!FS_DIR_HANDLE_IS_NULL(dir_handle), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

    FS_DIR_WITH_NO_IO(dir_handle, p_err) {
      p_desc = dir_handle.DirDescPtr;
      ASSERT_BREAK(!FSObj_IsClosingTestSet(FSDir_Desc_ToObj(p_desc)),
                   RTOS_ERR_ENTRY_CLOSED);
    }

    FSObj_IOEndWait(FSDir_Desc_ToObj(p_desc));

    FSObj_RefRelease(FSDir_Desc_ToObj(p_desc), FSDir_Desc_OnNullRefCnt);
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                               FSDir_Rd()
 *
 * @brief    Read a directory entry from a directory table.
 *
 * @param    dir_handle      Handle to a directory.
 *
 * @param    p_entry_info    Pointer to a structure that will receive the entry information.
 *
 * @param    p_buf           Pointer to a buffer that will receive the entry name.
 *
 * @param    buf_size        Size of the given buffer.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_ENTRY_CLOSED
 *                               - RTOS_ERR_VOL_CLOSED
 *                               - RTOS_ERR_VOL_CORRUPTED
 *                               - RTOS_ERR_BLK_DEV_CLOSED
 *                               - RTOS_ERR_BLK_DEV_CORRUPTED
 *                               - RTOS_ERR_IO
 *
 * @return   DEF_NO,  if there are other available entries.
 *           DEF_YES, if there are no more entry.
 *
 * @note     (1) Entries for "dot" (current directory) and "dot-dot" (parent directory) will be
 *               returned, if present.
 *
 * @note     (2) A subsequent call to FSDir_Rd() will return the next directory entry in the table
 *               until there is no more entry to read.
 *
 * @note     (3) The virtual root of the the file system designates the different opened volumes. It is
 *               not a regular directory table entry of type file or directory. It can be seen as a
 *               special entry of type volume.
 *               If a structure FS_ENTRY_INFO is provided by the application, the special volume entry
 *               is considered hidden and is identified as a directory, a sort of virtual directory.
 *******************************************************************************************************/
CPU_BOOLEAN FSDir_Rd(FS_DIR_HANDLE dir_handle,
                     FS_ENTRY_INFO *p_entry_info,
                     CPU_CHAR      *p_buf,
                     CPU_SIZE_T    buf_size,
                     RTOS_ERR      *p_err)
{
  FS_DIR_DESC *p_dir_desc;
  FS_VOL      *p_cur_vol;
  CPU_BOOLEAN eof;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_FAIL);
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    RTOS_ASSERT_DBG_ERR_SET(p_buf != DEF_NULL, *p_err,
                            RTOS_ERR_ASSERT_ERR_PTR_NULL, DEF_FAIL)

    eof = DEF_NO;
    BREAK_ON_ERR(FS_DIR_WITH) (dir_handle, p_err) {
      p_dir_desc = dir_handle.DirDescPtr;

      if (p_dir_desc->DirNodePtr == DEF_NULL) {                 // ------------- FILE SYSTEM VIRTUAL ROOT -------------
        CPU_SIZE_T cur_list_pos;

        cur_list_pos = 0u;
        eof = DEF_YES;

        BREAK_ON_ERR(FS_OP_LOCK_WITH) (&FSCore_Data.OpLock, FS_GLOBAL_OP_OPEN_VOL_LIST_RD) {
          //                                                       Browse open volume list to find virtual root entry.
          BREAK_ON_ERR(SLIST_FOR_EACH_ENTRY) (FSVol_OpenListHeadPtr, p_cur_vol, FS_VOL, OpenListMember) {
            if (cur_list_pos == p_dir_desc->CurPosData) {
              //                                                   Copy vol name to provided buffer.
              p_buf[buf_size - 1u] = '\0';
              Str_Copy_N(p_buf, p_cur_vol->Name, buf_size);
              ASSERT_BREAK(p_buf[buf_size - 1u] == '\0', RTOS_ERR_WOULD_OVF);
              p_dir_desc->CurPosData += 1u;

              if (p_entry_info != DEF_NULL) {                   // See Note #3.
                Mem_Set(p_entry_info, 0, sizeof(FS_ENTRY_INFO));

                p_entry_info->Attrib.IsDir = DEF_YES;
                p_entry_info->Attrib.Hidden = DEF_YES;
                p_entry_info->DevId = p_cur_vol->BlkDevHandle.BlkDevId;
              }

              eof = DEF_NO;
              break;
            }
          }
        }

        cur_list_pos += 1u;
      } else {                                                  // ------- REGULAR DIRECTORY (ROOT & NON-ROOT) --------
        FS_VOL_WITH(p_dir_desc->DirNodePtr->VolHandle, p_err) {
          //                                                       Use underlying file system to get info about entry.
          BREAK_ON_ERR(eof = p_dir_desc->DirNodePtr->SysPtr->DirRd(p_dir_desc,
                                                                   &p_dir_desc->CurPosData,
                                                                   p_entry_info,
                                                                   p_buf,
                                                                   buf_size,
                                                                   p_err));
        }
      }
    }
  } WITH_SCOPE_END

  return (eof);
}

/****************************************************************************************************//**
 *                                               FSDir_Query()
 *
 * @brief    Get information about a directory.
 *
 * @param    dir_handle      Handle to a directory.
 *
 * @param    p_entry_info    Pointer to structure that will receive the entry information.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_ENTRY_CLOSED
 *                               - RTOS_ERR_VOL_CLOSED
 *                               - RTOS_ERR_VOL_CORRUPTED
 *                               - RTOS_ERR_BLK_DEV_CLOSED
 *                               - RTOS_ERR_BLK_DEV_CORRUPTED
 *                               - RTOS_ERR_IO
 *
 * @note     (1) If data is stored in the file buffer waiting to be written to the storage medium,
 *               the actual file size may need to be adjusted to account for that buffered data.
 *******************************************************************************************************/
void FSDir_Query(FS_DIR_HANDLE dir_handle,
                 FS_ENTRY_INFO *p_entry_info,
                 RTOS_ERR      *p_err)

{
  FS_DIR_NODE *p_dir_node;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    RTOS_ASSERT_DBG_ERR_SET(p_entry_info != DEF_NULL, *p_err, RTOS_ERR_ASSERT_ERR_PTR_NULL,; )

    FS_DIR_WITH(dir_handle, p_err) {
      p_dir_node = dir_handle.DirDescPtr->DirNodePtr;
      FS_VOL_WITH(p_dir_node->VolHandle, p_err) {
        BREAK_ON_ERR(p_dir_node->SysPtr->EntryQuery(p_dir_node->VolHandle.VolPtr,
                                                    p_dir_node->Pos,
                                                    p_entry_info,
                                                    DEF_NULL,
                                                    DEF_NULL,
                                                    0u,
                                                    p_err));
      }
    }
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                               FSDir_PathGet()
 *
 * @brief    Get absolute path to an opened file.
 *
 * @param    dir_handle  Handle to a directory.
 *
 * @param    p_buf       Pointer to a buffer that will receive the path.
 *
 * @param    buf_size    Size of the provided buffer.
 *
 * @param    p_err       Pointer to variable that will receive the return error code(s) from this
 *                       function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_WOULD_OVF
 *                           - RTOS_ERR_ENTRY_CLOSED
 *                           - RTOS_ERR_VOL_CLOSED
 *                           - RTOS_ERR_VOL_CORRUPTED
 *                           - RTOS_ERR_BLK_DEV_CLOSED
 *                           - RTOS_ERR_BLK_DEV_CORRUPTED
 *                           - RTOS_ERR_IO
 *******************************************************************************************************/
void FSDir_PathGet(FS_DIR_HANDLE dir_handle,
                   CPU_CHAR      *p_buf,
                   CPU_SIZE_T    buf_size,
                   RTOS_ERR      *p_err)
{
  FS_ENTRY_NODE *p_entry_node;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
    RTOS_ASSERT_DBG_ERR_SET(p_buf != DEF_NULL, *p_err, RTOS_ERR_NULL_PTR,; )

    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    BREAK_ON_ERR(FS_DIR_WITH) (dir_handle, p_err) {
      p_entry_node = (FS_ENTRY_NODE *)dir_handle.DirDescPtr->DirNodePtr;
      BREAK_ON_ERR(FSEntry_PathGetInternal(p_entry_node,
                                           FS_ENTRY_TYPE_DIR,
                                           p_buf,
                                           buf_size,
                                           p_err));
    }
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                               FSDir_VolGet()
 *
 * @brief    Get volume containing the given directory.
 *
 * @param    dir_handle  Handle to a directory.
 *
 * @return   Handle to the volume, if NO error(s).
 *           NULL handle,          otherwise.
 *******************************************************************************************************/
FS_VOL_HANDLE FSDir_VolGet(FS_DIR_HANDLE dir_handle)
{
  FS_VOL_HANDLE vol_handle;
  RTOS_ERR      err;

  RTOS_ERR_SET(err, RTOS_ERR_NONE);
  vol_handle = FSVol_NullHandle;

  FS_DIR_WITH(dir_handle, &err) {
    vol_handle = dir_handle.DirDescPtr->DirNodePtr->VolHandle;
  }

  PP_UNUSED_PARAM(err);

  return (vol_handle);
}

/****************************************************************************************************//**
 *                                           FSDir_FirstAcquire()
 *
 * @brief    Acquire the first opened directory.
 *
 * @return   Handle to the first opened directory or 'FS_DIR_NULL' if there is no opened directory.
 *******************************************************************************************************/
FS_DIR_HANDLE FSDir_FirstAcquire(void)
{
  FS_OBJ_HANDLE obj_handle;
  FS_DIR_HANDLE dir_handle;

  FS_OP_LOCK_WITH(&FSCore_Data.OpLock, FS_GLOBAL_OP_OPEN_ENTRY_LIST_RD) {
    obj_handle = FSObj_FirstRefAcquireLocked(FSDir_OpenDescListHeadPtr);
  }

  dir_handle.DirDescPtr = (FS_DIR_DESC *)obj_handle.ObjPtr;
  dir_handle.DirDescId = obj_handle.ObjId;

  return (dir_handle);
}

/****************************************************************************************************//**
 *                                           FSDir_NextAcquire()
 *
 * @brief    Acquire the next opened directory.
 *
 * @param    dir_handle  Handle to the current directory.
 *
 * @return   Handle to the next directory or 'FS_DIR_NULL' if there is no more opened diretory.
 *******************************************************************************************************/
FS_DIR_HANDLE FSDir_NextAcquire(FS_DIR_HANDLE dir_handle)
{
  FS_OBJ_HANDLE cur_obj_handle;
  FS_OBJ_HANDLE next_obj_handle;
  FS_DIR_HANDLE next_dir_handle;

  cur_obj_handle.ObjPtr = (FS_OBJ *)dir_handle.DirDescPtr;
  cur_obj_handle.ObjId = dir_handle.DirDescId;

  FS_OP_LOCK_WITH(&FSCore_Data.OpLock, FS_GLOBAL_OP_OPEN_ENTRY_LIST_RD) {
    next_obj_handle = FSObj_NextRefAcquireLocked(cur_obj_handle);
  }

  FSDir_Release(dir_handle);

  next_dir_handle.DirDescPtr = (FS_DIR_DESC *)next_obj_handle.ObjPtr;
  next_dir_handle.DirDescId = next_obj_handle.ObjId;

  return (next_dir_handle);
}

/****************************************************************************************************//**
 *                                               FSDir_Release()
 *
 * @brief    Release a directory.
 *
 * @param    dir_handle  Handle to a directory.
 *******************************************************************************************************/
void FSDir_Release(FS_DIR_HANDLE dir_handle)
{
  FSObj_RefRelease(FSDir_Desc_ToObj(dir_handle.DirDescPtr), FSDir_Desc_OnNullRefCnt);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           FSDir_ModuleInit()
 *
 * @brief    Initialize the directory module.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/
void FSDir_ModuleInit(RTOS_ERR *p_err)
{
  CPU_SIZE_T init_desc_cnt;
  CPU_SIZE_T max_desc_cnt;

  init_desc_cnt = FSCore_InitCfg.MaxCoreObjCnt.RootDirDescCnt == LIB_MEM_BLK_QTY_UNLIMITED ? 1u : FSCore_InitCfg.MaxCoreObjCnt.RootDirDescCnt;
  max_desc_cnt = FSCore_InitCfg.MaxCoreObjCnt.RootDirDescCnt;

  Mem_DynPoolCreate("FS - dir desc pool",
                    &FSDir_DescPool,
                    FSCore_InitCfg.MemSegPtr,
                    sizeof(FS_DIR_DESC),
                    sizeof(CPU_ALIGN),
                    init_desc_cnt,
                    max_desc_cnt,
                    p_err);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           FSDir_OnNullDescRefCnt()
 *
 * @brief    On directory descriptor null reference count callback.
 *
 * @param    p_obj   Pointer to a directory descriptor.
 *
 * @note     (1) The directory descriptor will be freed regardless of possible errors occurring while:
 *               - (a) performing any operation related to detaching from a parent object.
 *******************************************************************************************************/
static void FSDir_Desc_OnNullRefCnt(FS_OBJ *p_obj)
{
  FS_DIR_DESC *p_desc;

  p_desc = (FS_DIR_DESC *)p_obj;

  FS_OP_LOCK_WITH(&FSCore_Data.OpLock, FS_GLOBAL_OP_OPEN_ENTRY_LIST_WR) {
    SList_Rem(&FSDir_OpenDescListHeadPtr, &p_desc->OpenListMember);
  }

  if (p_desc->DirNodePtr != DEF_NULL) {
    FSObj_RefRelease((FS_OBJ *)p_desc->DirNodePtr, FSDir_Node_OnNullRefCnt);
    p_desc->DirNodePtr->SysPtr->DirDescFree(p_desc);
  } else {
    RTOS_ERR err = RTOS_ERR_INIT_CODE(RTOS_ERR_NONE);
    Mem_DynPoolBlkFree(&FSDir_DescPool, (void *)p_desc, &err);
    RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }
}

/****************************************************************************************************//**
 *                                       FSEntry_OnNullNodeRefCntCb()
 *
 * @brief    On directory node null reference count callback.
 *
 * @param    p_obj   Pointer to an entry descriptor.
 *
 * @note     (1) The entry node will be freed regardless of possible errors occurring while:
 *               - (a) synchronizing the file;
 *               - (b) performing any operation related to detaching from a parent object.
 *******************************************************************************************************/
static void FSDir_Node_OnNullRefCnt(FS_OBJ *p_obj)
{
  FS_DIR_NODE *p_node;

  p_node = (FS_DIR_NODE *)p_obj;
  FS_OP_LOCK_WITH(&FSCore_Data.OpLock, FS_GLOBAL_OP_OPEN_ENTRY_LIST_WR) {
    SList_Rem(&FSEntry_OpenListHeadPtr, &p_node->OpenListMember);
  }

  p_node->SysPtr->DirNodeFree(p_node);
}

/****************************************************************************************************//**
 *                                           FSDir_AccessModeIsValid()
 *
 * @brief    Check whether access mode flags combination is valid.
 *
 * @param    mode    Access mode flags.
 *
 * @return   DEF_YES, if flag combination is valid.
 *           DEF_NO, otherwise.
 *******************************************************************************************************/

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
static CPU_BOOLEAN FSDir_AccessModeIsValid(FS_FLAGS mode)
{
#if (FS_CORE_CFG_RD_ONLY_EN == DEF_ENABLED)
  if (DEF_BIT_IS_SET_ANY(mode, FS_DIR_ACCESS_MODE_CREATE | FS_DIR_ACCESS_MODE_EXCL)) {
    return (DEF_NO);
  }
#endif

  if (DEF_BIT_IS_SET(mode, FS_DIR_ACCESS_MODE_EXCL)
      && DEF_BIT_IS_CLR(mode, FS_DIR_ACCESS_MODE_CREATE)) {
    return (DEF_NO);                                            // If 'excl' flag is set, 'create' flag must also be set
  }

  if ((mode & FS_DIR_ACCESS_MODE_ALL) != mode) {
    return (DEF_NO);                                            // Inexistent flags have been set besides 'create' and 'excl'
  }

  return (DEF_YES);
}
#endif

/****************************************************************************************************//**
 *                                           FSDir_Desc_ToObjHandle()
 *
 * @brief    Convert directory handle to generic object handle.
 *
 * @param    dir_handle  Handle to a directory.
 *
 * @return   Handle to generic object.
 *******************************************************************************************************/
static FS_OBJ_HANDLE FSDir_Desc_ToObjHandle(FS_DIR_HANDLE dir_handle)
{
  FS_OBJ_HANDLE obj_handle;

  obj_handle.ObjPtr = (FS_OBJ *)dir_handle.DirDescPtr;
  obj_handle.ObjId = dir_handle.DirDescId;

  return (obj_handle);
}

/****************************************************************************************************//**
 *                                           FSDir_Desc_ToObj()
 *
 * @brief    Convert directory descriptor to associated structure object.
 *
 * @param    p_dir_desc  Pointer to directory descriptor.
 *
 * @return   Pointer to associated structure object.
 *******************************************************************************************************/
static FS_OBJ *FSDir_Desc_ToObj(FS_DIR_DESC *p_dir_desc)
{
  return ((FS_OBJ *)p_dir_desc);
}

#endif // FS_CORE_CFG_DIR_EN

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_FS_AVAIL
