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
#include  <fs/source/core/fs_core_working_dir_priv.h>
#include  <fs/source/core/fs_core_entry_priv.h>
#include  <fs/include/fs_core_working_dir.h>
#include  <fs/source/core/fs_core_priv.h>
#include  <fs/source/shared/cleanup/cleanup_mgmt_priv.h>
#include  <fs/source/sys/fs_sys_priv.h>

//                                                                 ----------------------- EXT ------------------------
#include  <common/source/logging/logging_priv.h>
#include  <common/include/rtos_err.h>
#include  <common/include/kal.h>
#include  <cpu/include/cpu.h>

#include  <em_core.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH        (FS, WORKING, DIR)
#define  RTOS_MODULE_CUR    RTOS_CFG_MODULE_FS

/********************************************************************************************************
 ********************************************************************************************************
 *                                               VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

const FS_WRK_DIR_HANDLE FSWrkDir_NullHandle = { 0 };

SLIST_MEMBER *FSWrkDir_OpenListHeadPtr = DEF_NULL;

#if (FS_CORE_CFG_TASK_WORKING_DIR_EN == DEF_ENABLED)
static KAL_TASK_REG_HANDLE FS_WorkingDirPtrTaskRegHandle;
static KAL_TASK_REG_HANDLE FS_WorkingDirIdTaskRegHandle;
#endif

static MEM_DYN_POOL FSWrkDir_Pool;

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL MACROS
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 --------------- REF ACQUIRE/RELEASE ----------------
#define  FS_WRK_DIR_WITH(wrk_dir_handle, p_err)          FS_OBJ_WITH(wrk_dir_handle, p_err, FSWrkDir, RTOS_ERR_WRK_DIR_CLOSED)

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void FSWrkDir_OnNullRefCnt(FS_OBJ *p_obj);

static FS_OBJ_HANDLE FSWrkDir_ToObjHandle(FS_WRK_DIR_HANDLE wrk_dir_handle);

static FS_OBJ *FSWrkDir_ToObj(FS_WRK_DIR *p_wrk_dir);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               GLOBAL  FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               FSWrkDir_Open()
 *
 * @brief    Open a working directory.
 *
 * @param    wrk_dir_handle  Handle to the current working directory.
 *
 * @param    p_path          Pointer to a directory path relative to the given working directory.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_INIT
 *                               - RTOS_ERR_NOT_FOUND
 *                               - RTOS_ERR_INVALID_TYPE
 *                               - RTOS_ERR_ENTRY_MAX_DEPTH_EXCEEDED
 *                               - RTOS_ERR_WRK_DIR_CLOSED
 *                               - RTOS_ERR_VOL_CLOSED
 *                               - RTOS_ERR_VOL_CORRUPTED
 *                               - RTOS_ERR_BLK_DEV_CLOSED
 *                               - RTOS_ERR_BLK_DEV_CORRUPTED
 *                               - RTOS_ERR_IO
 *
 * @return   Handle to the opened working directory.
 *******************************************************************************************************/
FS_WRK_DIR_HANDLE FSWrkDir_Open(FS_WRK_DIR_HANDLE wrk_dir_handle,
                                const CPU_CHAR    *p_path,
                                RTOS_ERR          *p_err)
{
  FS_VOL            *p_vol;
  FS_WRK_DIR        *p_new_wrk_dir;
  FS_SYS_POS        entry_pos_data;
  FS_WRK_DIR_HANDLE new_wrk_dir_handle;
  FS_ENTRY_INFO     entry_info;
  CPU_INT16S        rel_depth;
  FS_ENTRY_LOC_DATA entry_loc_data;
  CORE_DECLARE_IRQ_STATE;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, FSWrkDir_NullHandle);
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    //                                                             Check that Core layer is initialized.
    CORE_ENTER_ATOMIC();
    if (!FSCore_Data.IsInit) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_INIT);
      CORE_EXIT_ATOMIC();
      break;
    }
    CORE_EXIT_ATOMIC();

    new_wrk_dir_handle = FSWrkDir_NullHandle;

    BREAK_ON_ERR(FSEntry_VirtualPathResolve(wrk_dir_handle,
                                            (CPU_CHAR *)p_path,
                                            &entry_loc_data,
                                            p_err));

    if (!FS_VOL_HANDLE_IS_NULL(entry_loc_data.VolHandle)) {
      BREAK_ON_ERR(FS_VOL_WITH) (entry_loc_data.VolHandle, p_err) {
        p_vol = entry_loc_data.VolHandle.VolPtr;

        BREAK_ON_ERR((void)FSEntry_Lookup(&entry_loc_data,
                                          DEF_NULL,
                                          &entry_pos_data,
                                          &rel_depth,
                                          p_err));

        ASSERT_BREAK_LOG(entry_pos_data != p_vol->SysPtr->VoidEntryPos,
                         RTOS_ERR_NOT_FOUND, ("Directory not found."));

        ASSERT_BREAK((CPU_INT08U)(entry_loc_data.StartDepth + rel_depth) < 255u,
                     RTOS_ERR_ENTRY_MAX_DEPTH_EXCEEDED);

        ASSERT_BREAK(entry_loc_data.StartDepth + rel_depth > 0,
                     RTOS_ERR_VOL_CORRUPTED);

        //                                                         -------------------- CHK IF DIR --------------------
        BREAK_ON_ERR(p_vol->SysPtr->EntryQuery(p_vol,
                                               entry_pos_data,
                                               &entry_info,
                                               DEF_NULL,
                                               DEF_NULL,
                                               0u,
                                               p_err));

        ASSERT_BREAK_LOG(entry_info.Attrib.IsDir, RTOS_ERR_INVALID_TYPE,
                         ("A working directory may not be a file."));

        BREAK_ON_ERR(p_new_wrk_dir = (FS_WRK_DIR *)Mem_DynPoolBlkGet(&FSWrkDir_Pool, p_err));
        Mem_Clr(p_new_wrk_dir, sizeof(FS_WRK_DIR));

        ON_BREAK {
          Mem_DynPoolBlkFree(&FSWrkDir_Pool, (void *)p_new_wrk_dir, p_err);
          RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE,
                               RTOS_ERR_ASSERT_CRITICAL_FAIL, FSWrkDir_NullHandle);
          break;
        } WITH {
          p_new_wrk_dir->Id = FSObj_CurId++;
          p_new_wrk_dir->RefCnt = 1u;
          p_new_wrk_dir->IOCnt = 1u;
          p_new_wrk_dir->IsClosing = DEF_NO;
          p_new_wrk_dir->VolHandle.VolPtr = p_vol;
          p_new_wrk_dir->VolHandle.VolId = p_vol->Id;
          p_new_wrk_dir->SysPtr = p_vol->SysPtr;

          FS_OP_LOCK_WITH(&FSCore_Data.OpLock, FS_GLOBAL_OP_OPEN_ENTRY_LIST_WR) {
            SList_Push(&FSWrkDir_OpenListHeadPtr, &p_new_wrk_dir->OpenListMember);
          }

          p_new_wrk_dir->Pos = entry_pos_data;
          p_new_wrk_dir->Depth = entry_loc_data.StartDepth + rel_depth;

          new_wrk_dir_handle.WrkDirPtr = p_new_wrk_dir;
          new_wrk_dir_handle.WrkDirId = p_new_wrk_dir->Id;
        }
      }
    }
  } WITH_SCOPE_END

  return (new_wrk_dir_handle);
}

/****************************************************************************************************//**
 *                                               FSWrkDir_Close()
 *
 * @brief    Close a working directory.
 *
 * @param    working_dir_handle  Handle to the working directory to be closed.
 *
 * @param    p_err               Pointer to variable that will receive the return error code(s) from
 *                               this function:
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_WRK_DIR_CLOSED
 *******************************************************************************************************/
void FSWrkDir_Close(FS_WRK_DIR_HANDLE wrk_dir_handle,
                    RTOS_ERR          *p_err)
{
  FS_ENTRY_NODE_HANDLE entry_node_handle;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  if (!FS_WRK_DIR_HANDLE_IS_NULL(wrk_dir_handle)) {
    entry_node_handle.EntryNodePtr = (FS_ENTRY_NODE *)wrk_dir_handle.WrkDirPtr;
    entry_node_handle.EntryNodeId = wrk_dir_handle.WrkDirId;

    FSEntry_NodeClose(entry_node_handle, FSWrkDir_OnNullRefCnt, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_ENTRY_CLOSED) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_WRK_DIR_CLOSED);
    }
  }
}

/****************************************************************************************************//**
 *                                           FSWrkDir_TaskBind()
 *
 * @brief    Bind a working directory to the current task.
 *
 * @param    wrk_dir_handle  Handle to a working directory.
 *
 * @param    p_path          Pointer to a directory path relative to the given working directory.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_FOUND
 *                               - RTOS_ERR_INVALID_TYPE
 *                               - RTOS_ERR_ENTRY_MAX_DEPTH_EXCEEDED
 *                               - RTOS_ERR_WRK_DIR_CLOSED
 *                               - RTOS_ERR_VOL_CLOSED
 *                               - RTOS_ERR_VOL_CORRUPTED
 *                               - RTOS_ERR_BLK_DEV_CLOSED
 *                               - RTOS_ERR_BLK_DEV_CORRUPTED
 *                               - RTOS_ERR_IO
 *******************************************************************************************************/
#if (FS_CORE_CFG_TASK_WORKING_DIR_EN == DEF_ENABLED)
void FSWrkDir_TaskBind(FS_WRK_DIR_HANDLE wrk_dir_handle,
                       const CPU_CHAR    *p_path,
                       RTOS_ERR          *p_err)
{
  FS_WRK_DIR_HANDLE old_wrk_dir_handle;
  FS_WRK_DIR_HANDLE new_wrk_dir_handle;
  RTOS_ERR          err_tmp;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    RTOS_ERR_SET(err_tmp, RTOS_ERR_NONE);

    old_wrk_dir_handle = FSWrkDir_Get();

    BREAK_ON_ERR(new_wrk_dir_handle = FSWrkDir_Open(wrk_dir_handle,
                                                    p_path,
                                                    p_err));

    FSWrkDir_Close(old_wrk_dir_handle, &err_tmp);
    ASSERT_BREAK((RTOS_ERR_CODE_GET(err_tmp) == RTOS_ERR_NONE)
                 || (RTOS_ERR_CODE_GET(err_tmp) == RTOS_ERR_WRK_DIR_CLOSED),
                 RTOS_ERR_CODE_GET(err_tmp));

    BREAK_ON_ERR(KAL_TaskRegSet(KAL_TaskHandleNull,
                                FS_WorkingDirPtrTaskRegHandle,
                                (KAL_TASK_REG)new_wrk_dir_handle.WrkDirPtr,
                                p_err));

#if (FS_CORE_CFG_THREAD_SAFETY_EN == DEF_ENABLED)
    BREAK_ON_ERR(KAL_TaskRegSet(KAL_TaskHandleNull,
                                FS_WorkingDirIdTaskRegHandle,
                                (KAL_TASK_REG)new_wrk_dir_handle.WrkDirId,
                                p_err));
#endif
  } WITH_SCOPE_END
}
#endif

/****************************************************************************************************//**
 *                                           FSWrkDir_TaskUnbind()
 *
 * @brief    Unbind a working directory from the current task.
 *
 * @param    p_err   Pointer to variable that will receive the return error code(s) from this function:
 *                       - RTOS_ERR_NONE
 *******************************************************************************************************/

#if (FS_CORE_CFG_TASK_WORKING_DIR_EN == DEF_ENABLED)
void FSWrkDir_TaskUnbind(RTOS_ERR *p_err)
{
  FS_WRK_DIR_HANDLE old_wrk_dir_handle;
  RTOS_ERR          err_tmp = RTOS_ERR_INIT_CODE(RTOS_ERR_NONE);

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    old_wrk_dir_handle = FSWrkDir_Get();

    FSWrkDir_Close(old_wrk_dir_handle, &err_tmp);
    ASSERT_BREAK((RTOS_ERR_CODE_GET(err_tmp) == RTOS_ERR_NONE)
                 || (RTOS_ERR_CODE_GET(err_tmp) == RTOS_ERR_WRK_DIR_CLOSED),
                 RTOS_ERR_CODE_GET(err_tmp));

    BREAK_ON_ERR(KAL_TaskRegSet(KAL_TaskHandleNull,
                                FS_WorkingDirPtrTaskRegHandle,
                                (KAL_TASK_REG)FSWrkDir_NullHandle.WrkDirPtr,
                                p_err));

#if (FS_CORE_CFG_THREAD_SAFETY_EN == DEF_ENABLED)
    BREAK_ON_ERR(KAL_TaskRegSet(KAL_TaskHandleNull,
                                FS_WorkingDirIdTaskRegHandle,
                                (KAL_TASK_REG)FSWrkDir_NullHandle.WrkDirId,
                                p_err));
#endif
  } WITH_SCOPE_END
}
#endif

/****************************************************************************************************//**
 *                                               FSWrkDir_Get()
 *
 * @brief    Get working directory bound to the current task.
 *
 * @return   Working directory handle.
 *******************************************************************************************************/

#if (FS_CORE_CFG_TASK_WORKING_DIR_EN == DEF_ENABLED)
FS_WRK_DIR_HANDLE FSWrkDir_Get(void)
{
  FS_WRK_DIR_HANDLE wrk_dir_handle;
  KAL_TASK_REG      task_reg_val;
  RTOS_ERR          err;

  RTOS_ERR_SET(err, RTOS_ERR_NONE);

  task_reg_val = KAL_TaskRegGet(KAL_TaskHandleNull,
                                FS_WorkingDirPtrTaskRegHandle,
                                &err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE),
                       RTOS_ERR_ASSERT_CRITICAL_FAIL, FSWrkDir_NullHandle);

  wrk_dir_handle.WrkDirPtr = (FS_WRK_DIR *)task_reg_val;

  task_reg_val = KAL_TaskRegGet(KAL_TaskHandleNull,
                                FS_WorkingDirIdTaskRegHandle,
                                &err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE),
                       RTOS_ERR_ASSERT_CRITICAL_FAIL, FSWrkDir_NullHandle);

  wrk_dir_handle.WrkDirId = (FS_ID)task_reg_val;

  return (wrk_dir_handle);
}
#endif

/****************************************************************************************************//**
 *                                           FSWrkDir_PathGet()
 *
 * @brief    Get the absolute path of a working directory.
 *
 * @param    wrk_dir_handle  Handle to a working directory.
 *
 * @param    p_buf           Pointer to a buffer that will receive the path.
 *
 * @param    buf_size        Size of the provided buffer.
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
 * @return   The total number of characters in the path.
 *******************************************************************************************************/
CPU_SIZE_T FSWrkDir_PathGet(FS_WRK_DIR_HANDLE wrk_dir_handle,
                            CPU_CHAR          *p_buf,
                            CPU_SIZE_T        buf_size,
                            RTOS_ERR          *p_err)
{
  CPU_SIZE_T path_len;

  WITH_SCOPE_BEGIN(p_err) {
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    path_len = 0u;
    if (FS_WRK_DIR_HANDLE_IS_NULL(wrk_dir_handle)) {
      path_len = 1u;
      if (p_buf != DEF_NULL) {
        ASSERT_BREAK(buf_size >= 2u, RTOS_ERR_WOULD_OVF);
        p_buf[0] = '/';
        p_buf[1] = '\0';
      }
    } else {
      BREAK_ON_ERR(FS_WRK_DIR_WITH) (wrk_dir_handle, p_err) {
        BREAK_ON_ERR(path_len = FSEntry_PathGetInternal((FS_ENTRY_NODE *)wrk_dir_handle.WrkDirPtr,
                                                        FS_ENTRY_TYPE_WRK_DIR,
                                                        p_buf,
                                                        buf_size,
                                                        p_err));
      }
    }
  } WITH_SCOPE_END

  return (path_len);
}

/****************************************************************************************************//**
 *                                               FSWrkDir_VolGet()
 *
 * @brief    Get volume handle associated to given working directory.
 *
 * @param    wrk_dir_handle  Handle to a working directory.
 *
 * @return   Parent volume handle or NULL handle if working directory closed.
 *******************************************************************************************************/
FS_VOL_HANDLE FSWrkDir_VolGet(FS_WRK_DIR_HANDLE wrk_dir_handle)
{
  FS_VOL_HANDLE vol_handle;
  RTOS_ERR      err;

  RTOS_ERR_SET(err, RTOS_ERR_NONE);

  vol_handle = FSVol_NullHandle;

  FS_WRK_DIR_WITH(wrk_dir_handle, &err) {
    vol_handle = wrk_dir_handle.WrkDirPtr->VolHandle;
  }

  PP_UNUSED_PARAM(err);

  return (vol_handle);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               FSWrkDir_Init()
 *
 * @brief    Initialize the working directory module.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/
void FSWrkDir_Init(RTOS_ERR *p_err)
{
  CPU_SIZE_T init_wrk_dir_cnt;
  CPU_SIZE_T max_wrk_dir_cnt;

  WITH_SCOPE_BEGIN(p_err) {
    init_wrk_dir_cnt = FSCore_InitCfg.MaxCoreObjCnt.WrkDirCnt == LIB_MEM_BLK_QTY_UNLIMITED ? 1u : FSCore_InitCfg.MaxCoreObjCnt.WrkDirCnt;
    max_wrk_dir_cnt = FSCore_InitCfg.MaxCoreObjCnt.WrkDirCnt;

    //                                                             ------------- WRK DIR TASK REG CREATE --------------
#if (FS_CORE_CFG_TASK_WORKING_DIR_EN == DEF_ENABLED)
    RTOS_ASSERT_DBG_ERR_SET(KAL_FeatureQuery(KAL_FEATURE_TASK_REG, KAL_OPT_NONE),
                            *p_err, RTOS_ERR_NOT_AVAIL,; );

    BREAK_ON_ERR(FS_WorkingDirPtrTaskRegHandle = KAL_TaskRegCreate(DEF_NULL, p_err));
#if (FS_CORE_CFG_THREAD_SAFETY_EN == DEF_ENABLED)
    BREAK_ON_ERR(FS_WorkingDirIdTaskRegHandle = KAL_TaskRegCreate(DEF_NULL, p_err));
#endif
#endif

    //                                                             ------------------- POOL CREATE --------------------
    BREAK_ON_ERR(Mem_DynPoolCreate("FS - working directory pool",
                                   &FSWrkDir_Pool,
                                   FSCore_InitCfg.MemSegPtr,
                                   sizeof(FS_WRK_DIR),
                                   sizeof(CPU_ALIGN),
                                   init_wrk_dir_cnt,
                                   max_wrk_dir_cnt,
                                   p_err));
  } WITH_SCOPE_END
}

/****************************************************************************************************//**
 *                                           FSWrkDir_OpenFind()
 *
 * @brief    Find an opened working directory.
 *
 * @param    p_vol       Pointer to a volume instance.
 *
 * @param    entry_pos   On-disk directory position.
 *
 * @return   Handle to the found working directory or FSWrkDir_NullHandle if working directory was
 *           not found.
 *******************************************************************************************************/
FS_WRK_DIR_HANDLE FSWrkDir_OpenFind(FS_VOL     *p_vol,
                                    FS_SYS_POS entry_pos)
{
  FS_WRK_DIR        *p_cur_wrk_dir;
  FS_WRK_DIR_HANDLE found_wrk_dir_handle = FSWrkDir_NullHandle;

  FS_OP_LOCK_WITH(&FSCore_Data.OpLock, FS_GLOBAL_OP_OPEN_ENTRY_LIST_RD) {
    SLIST_FOR_EACH_ENTRY(FSWrkDir_OpenListHeadPtr, p_cur_wrk_dir, FS_WRK_DIR, OpenListMember) {
      if ((p_cur_wrk_dir->Pos == entry_pos)
          && (p_cur_wrk_dir->VolHandle.VolPtr == p_vol)) {
        found_wrk_dir_handle.WrkDirPtr = p_cur_wrk_dir;
        found_wrk_dir_handle.WrkDirId = p_cur_wrk_dir->Id;
        break;
      }
    }
  }

  return (found_wrk_dir_handle);
}

/****************************************************************************************************//**
 *                                           FSWrkDir_FirstAcquire()
 *
 * @brief    Acquire first opened working directory.
 *
 * @return   Handle to the first opened working directory.
 *           'FS_WRK_DIR_NULL' if no working directory is opened.
 *******************************************************************************************************/
FS_WRK_DIR_HANDLE FSWrkDir_FirstAcquire(void)
{
  FS_OBJ_HANDLE     obj_handle;
  FS_WRK_DIR_HANDLE wrk_dir_handle;

  FS_OP_LOCK_WITH(&FSCore_Data.OpLock, FS_GLOBAL_OP_OPEN_ENTRY_LIST_RD) {
    obj_handle = FSObj_FirstRefAcquireLocked(FSWrkDir_OpenListHeadPtr);
  }

  wrk_dir_handle.WrkDirPtr = (FS_WRK_DIR *)obj_handle.ObjPtr;
  wrk_dir_handle.WrkDirId = obj_handle.ObjId;

  return (wrk_dir_handle);
}

/****************************************************************************************************//**
 *                                           FSWrkDir_NextGet()
 *
 * @brief    Get the next working directory.
 *
 * @param    wrk_dir_handle  Handle to the current working directory.
 *
 * @return   Handle to the next working directory.
 *           'FS_WRK_DIR_NULL' if no more working directory is opened.
 *******************************************************************************************************/
FS_WRK_DIR_HANDLE FSWrkDir_NextAcquire(FS_WRK_DIR_HANDLE wrk_dir_handle)
{
  FS_OBJ_HANDLE     cur_obj_handle;
  FS_OBJ_HANDLE     next_obj_handle;
  FS_WRK_DIR_HANDLE next_wrk_dir_handle;

  cur_obj_handle.ObjPtr = (FS_OBJ *)wrk_dir_handle.WrkDirPtr;
  cur_obj_handle.ObjId = wrk_dir_handle.WrkDirId;

  FS_OP_LOCK_WITH(&FSCore_Data.OpLock, FS_GLOBAL_OP_OPEN_ENTRY_LIST_RD) {
    next_obj_handle = FSObj_NextRefAcquireLocked(cur_obj_handle);
  }

  FSWrkDir_Release(wrk_dir_handle);

  next_wrk_dir_handle.WrkDirPtr = (FS_WRK_DIR *)next_obj_handle.ObjPtr;
  next_wrk_dir_handle.WrkDirId = next_obj_handle.ObjId;

  return (next_wrk_dir_handle);
}

/****************************************************************************************************//**
 *                                           FSWrkDir_Release()
 *
 * @brief    Release a working directory.
 *
 * @param    wrk_dir_handle  Handle to a working directory.
 *******************************************************************************************************/
void FSWrkDir_Release(FS_WRK_DIR_HANDLE wrk_dir_handle)
{
  FSObj_RefRelease(FSWrkDir_ToObj(wrk_dir_handle.WrkDirPtr), FSWrkDir_OnNullRefCnt);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           FSWrkDir_OnNullRefCnt()
 *
 * @brief    On null working directory reference count callback.
 *
 * @param    p_obj   Pointer to the null reference count working directory.
 *******************************************************************************************************/
static void FSWrkDir_OnNullRefCnt(FS_OBJ *p_obj)
{
  FS_WRK_DIR *p_wrk_dir;
  RTOS_ERR   err;

  p_wrk_dir = (FS_WRK_DIR *)p_obj;

  FS_OP_LOCK_WITH(&FSCore_Data.OpLock, FS_GLOBAL_OP_OPEN_ENTRY_LIST_WR) {
    SList_Rem(&FSWrkDir_OpenListHeadPtr, &p_wrk_dir->OpenListMember);
  }

  RTOS_ERR_SET(err, RTOS_ERR_NONE);
  Mem_DynPoolBlkFree(&FSWrkDir_Pool, (void *)p_wrk_dir, &err);
  RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}

/****************************************************************************************************//**
 *                                           FSWrkDir_ToObjHandle()
 *
 * @brief    Convert a working directory handle to an object handle.
 *
 * @param    wrk_dir_handle  Handle to a working directory.
 *
 * @return   Handle to an object.
 *******************************************************************************************************/
static FS_OBJ_HANDLE FSWrkDir_ToObjHandle(FS_WRK_DIR_HANDLE wrk_dir_handle)
{
  FS_OBJ_HANDLE obj_handle;

  obj_handle.ObjPtr = (FS_OBJ *)wrk_dir_handle.WrkDirPtr;
  obj_handle.ObjId = wrk_dir_handle.WrkDirId;

  return (obj_handle);
}

/****************************************************************************************************//**
 *                                               FSWrkDir_ToObj()
 *
 * @brief    Convert a working directory to an object.
 *
 * @param    p_wrk_dir   Pointer to a working directory.
 *
 * @return   Pointer to an object.
 *******************************************************************************************************/
static FS_OBJ *FSWrkDir_ToObj(FS_WRK_DIR *p_wrk_dir)
{
  return ((FS_OBJ *)p_wrk_dir);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_FS_AVAIL
