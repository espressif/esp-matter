/***************************************************************************//**
 * @file
 * @brief File System - Core Generic Object Operations
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

//                                                                 ----------------------- CFG ------------------------
#include  <fs_core_cfg.h>

//                                                                 ------------------------ FS ------------------------
#include  <fs/source/shared/cleanup/cleanup_mgmt_priv.h>
#include  <fs/source/shared/fs_obj_priv.h>
#include  <fs/source/sys/fs_sys_priv.h>
#include  <fs/source/core/fs_core_priv.h>

//                                                                 ----------------------- EXT ------------------------
#include  <common/source/logging/logging_priv.h>
#include  <common/include/lib_utils.h>

#include  <em_core.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  RTOS_MODULE_CUR     RTOS_CFG_MODULE_FS
#define  LOG_DFLT_CH         (FS, CORE, OBJ)

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef struct fs_obj_release_data {
  FS_OBJ      *ObjPtr;
  CPU_BOOLEAN Block;
} FS_OBJ_IO_RELEASE_DATA;

/********************************************************************************************************
 ********************************************************************************************************
 *                                               VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

FS_OBJ_ID           FSObj_CurId;
const FS_OBJ_HANDLE FSObj_NullHandle = { 0 };

static KAL_MON_HANDLE FSObj_IOReleaseMonHandle;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static KAL_MON_RES FSObj_IOReleaseOnEnter(void *p_mon_data,
                                          void *p_data);

static KAL_MON_RES FSObj_IOReleaseOnEval(void *p_mon_data,
                                         void *p_eval_op_data,
                                         void *p_scan_op_data);

static void FSObj_IOReleaseImpl(FS_OBJ      *p_obj,
                                CPU_BOOLEAN block);

static CPU_BOOLEAN FSObj_IOAcquireImpl(FS_OBJ *p_obj);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               FSObj_Init()
 *
 * @brief    Initialize the file system object sub-module.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/
void FSObj_Init(RTOS_ERR *p_err)
{
  FSObj_IOReleaseMonHandle = KAL_MonCreate(DEF_NULL, DEF_NULL, DEF_NULL, DEF_NULL, p_err);
}

/****************************************************************************************************//**
 *                                           FSObj_RefAcquire()
 *
 * @brief   Acquire (i.e. increment the reference count) a file system object.
 *
 * @param   obj_handle  Handle to a file system core object.
 *
 * @note    (1) An handle is invalid when an object is closed. If an invalid handle is detected, the
 *              application must first re-open the object prior to perform any operations on this object.
 *
 *          (2) The object ID contained in the current handle must match the referenced object ID.
 *******************************************************************************************************/
CPU_BOOLEAN FSObj_RefAcquire(FS_OBJ_HANDLE obj_handle)
{
  CORE_DECLARE_IRQ_STATE;

  if (obj_handle.ObjPtr == DEF_NULL) {                          // Ensure an object is associated to this handle.
    return (DEF_NO);
  }

  CORE_ENTER_ATOMIC();
  if (obj_handle.ObjPtr->RefCnt == 0u) {                        // Detect an invalid handle (see Note #1).
    CORE_EXIT_ATOMIC();
    return (DEF_NO);
  }

  if (obj_handle.ObjPtr->Id != obj_handle.ObjId) {              // Detect an invalid handle (see Note #2).
    CORE_EXIT_ATOMIC();
    return (DEF_NO);
  }

  obj_handle.ObjPtr->RefCnt++;
  CORE_EXIT_ATOMIC();

  return (DEF_YES);
}

/****************************************************************************************************//**
 *                                           FSObj_RefRelease()
 *
 * @brief    Release a file system object.
 *
 * @param    p_obj               Pointer to a generic file system core object.
 *
 * @param    on_null_ref_cnt     On-null reference count callback.
 *******************************************************************************************************/
void FSObj_RefRelease(FS_OBJ                    *p_obj,
                      FS_OBJ_ON_NULL_REF_CNT_CB on_null_ref_cnt)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  if (p_obj->RefCnt == 0u) {                                    // Ensure ref cnt not null. At least one acquire...
                                                                // ...operation must be done before releasing object ref
    CORE_EXIT_ATOMIC();
    LOG_ERR(("FS_Obj_Release(): Unbalanced reference acquiring/releasing."));
    RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }
  p_obj->RefCnt--;
  if (p_obj->RefCnt == 0u) {                                    // If last reference count release, ...
    CORE_EXIT_ATOMIC();                                         // ...call the object null ref callback to perform...
    on_null_ref_cnt(p_obj);                                     // ...any cleanup operations.
    return;
  }

  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                       FSObj_FirstRefAcquireLocked()
 *
 * @brief    Acquire first object in the given list.
 *
 * @param    p_list_head     Pointer to a list head.
 *
 * @return   Handle to the acquired object.
 *
 * @note     (1) The object is acquired so that it cannot disappear before the next object is acquired
 *               through FSObj_NextRefAcquireLocked().
 *
 * @note     (2) The object acquisition cannot fail since it is done while the caller is holding the
 *               object list lock.
 *******************************************************************************************************/
FS_OBJ_HANDLE FSObj_FirstRefAcquireLocked(SLIST_MEMBER *p_list_head)
{
  FS_OBJ        *p_obj;
  FS_OBJ_HANDLE obj_handle;

  if (p_list_head != DEF_NULL) {
    p_obj = CONTAINER_OF(p_list_head, FS_OBJ, OpenListMember);
    obj_handle.ObjPtr = p_obj;
    obj_handle.ObjId = p_obj->Id;
    //                                                             See Note #1 & #2.
    (void)FSObj_RefAcquire(obj_handle);
  } else {
    obj_handle = FSObj_NullHandle;
  }

  return (obj_handle);
}

/****************************************************************************************************//**
 *                                       FSObj_NextRefAcquireLocked()
 *
 * @brief    Release the given object and acquire the next one in the open list.
 *
 * @param    cur_obj_handle  Handle to the current file system core object.
 *
 * @return   Handle to the next (acquired) object.
 *
 * @note     (1) The next object is acquired until the next call to FSObj_NextRefAcquireLocked().
 *
 * @note     (2) The object acquisition cannot fail as it is done while holding the object list lock.
 *******************************************************************************************************/
FS_OBJ_HANDLE FSObj_NextRefAcquireLocked(FS_OBJ_HANDLE cur_obj_handle)
{
  SLIST_MEMBER  *p_next_member;
  FS_OBJ        *p_next_obj;
  FS_OBJ_HANDLE next_obj_handle;

  p_next_member = cur_obj_handle.ObjPtr->OpenListMember.p_next;
  if (p_next_member != DEF_NULL) {
    p_next_obj = CONTAINER_OF(p_next_member, FS_OBJ, OpenListMember);

    next_obj_handle.ObjPtr = p_next_obj;
    next_obj_handle.ObjId = p_next_obj->Id;

    //                                                             See Note #1 & 2.
    FSObj_RefAcquire(next_obj_handle);
  } else {
    next_obj_handle = FSObj_NullHandle;
  }

  return (next_obj_handle);
}

/****************************************************************************************************//**
 *                                               FSObj_IOAcquire()
 *
 * @brief    Acquire an object for IO.
 *
 * @param    obj_handle          Handle to the object to be acquired.
 *
 * @param    on_null_ref_cnt     Callback to be executed on null reference count (if io acquisition
 *                               fails).
 *
 * @return   DEF_YES, is object successfully acquired.
 *           DEF_NO,  otherwise.
 *******************************************************************************************************/
CPU_BOOLEAN FSObj_IOAcquire(FS_OBJ_HANDLE             obj_handle,
                            FS_OBJ_ON_NULL_REF_CNT_CB on_null_ref_cnt)
{
  if (FSObj_RefAcquire(obj_handle)) {
    if (FSObj_IOAcquireImpl(obj_handle.ObjPtr)) {
      FSObj_RefRelease(obj_handle.ObjPtr, on_null_ref_cnt);
      return (DEF_YES);
    }
    FSObj_RefRelease(obj_handle.ObjPtr, on_null_ref_cnt);
  }

  return (DEF_NO);
}

/********************************************************************************************************
 *                                           FSObj_IORelease()
 *
 * Description : Relase object from previous IO acquisition.
 *
 * Argument(s) : p_obj               Pointer to the object to be released.
 *
 * Return(s)   : none.
 * Note(s)     : none.
 *******************************************************************************************************/
void FSObj_IORelease(FS_OBJ *p_obj)
{
  FSObj_IOReleaseImpl(p_obj, DEF_NO);
}

/****************************************************************************************************//**
 *                                               FSObj_IOEndWait()
 *
 * @brief    Wait for all IO operations on this object to be over.
 *
 * @param    p_obj   Pointer to the object to wait for IO end.
 *******************************************************************************************************/
void FSObj_IOEndWait(FS_OBJ *p_obj)
{
  FSObj_IOReleaseImpl(p_obj, DEF_YES);
}

/****************************************************************************************************//**
 *                                           FSObj_IsClosingTestSet()
 *
 * @brief    Set the closing flag and return its previous state.
 *
 * @param    p_obj   Pointer to the object to test and set closing flag.
 *
 * @return   DEF_YES, if the closing flag was set.
 *           DEF_NO,  otherwise.
 *******************************************************************************************************/
CPU_BOOLEAN FSObj_IsClosingTestSet(FS_OBJ *p_obj)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  if (p_obj->IsClosing) {
    CORE_EXIT_ATOMIC();
    return (DEF_YES);
  }
  p_obj->IsClosing = DEF_YES;
  CORE_EXIT_ATOMIC();

  return (DEF_NO);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           FSObj_IOReleaseOnEnter()
 *
 * @brief    Callback to be executed upon entering the IO release monitor.
 *
 * @param    p_mon_data  Pointer to application-specific monitor data.
 *
 * @param    p_data      Pointer to caller's data.
 *
 * @return   Whether to start/stop pending list evaluation and allow/block the caller.
 *******************************************************************************************************/
static KAL_MON_RES FSObj_IOReleaseOnEnter(void *p_mon_data,
                                          void *p_data)
{
  FS_OBJ_IO_RELEASE_DATA *p_release_data;

  PP_UNUSED_PARAM(p_mon_data);

  p_release_data = (FS_OBJ_IO_RELEASE_DATA *)p_data;

  RTOS_ASSERT_CRITICAL(p_release_data->ObjPtr->IOCnt > 0u,
                       RTOS_ERR_ASSERT_CRITICAL_FAIL, KAL_MON_RES_STOP_EVAL);

  if (p_release_data->Block) {
    if (p_release_data->ObjPtr->IOCnt == 1u) {
      p_release_data->ObjPtr->IOCnt = 0u;
      return (KAL_MON_RES_ALLOW | KAL_MON_RES_STOP_EVAL);
    }
    return (KAL_MON_RES_BLOCK | KAL_MON_RES_STOP_EVAL);
  }
  p_release_data->ObjPtr->IOCnt--;

  return (KAL_MON_RES_ALLOW);
}

/****************************************************************************************************//**
 *                                           FSObj_IOReleaseOnEval()
 *
 * @brief    Callback to be executed upon evaluating the IO release monitor.
 *
 * @param    p_mon_data      Pointer to application-specific monitor data.
 *
 * @param    p_eval_op_data  Pointer to pending task data.
 *
 * @param    p_scan_op_data  Pointer to caller data.
 *
 * @return   Whether to start/stop pending list evaluation and allow/block the evaluated task.
 *******************************************************************************************************/
static KAL_MON_RES FSObj_IOReleaseOnEval(void *p_mon_data,
                                         void *p_eval_op_data,
                                         void *p_scan_op_data)
{
  FS_OBJ_IO_RELEASE_DATA *p_eval_release_data;

  PP_UNUSED_PARAM(p_mon_data);
  PP_UNUSED_PARAM(p_scan_op_data);

  p_eval_release_data = (FS_OBJ_IO_RELEASE_DATA *)p_eval_op_data;

  if (p_eval_release_data->ObjPtr->IOCnt == 1u) {
    RTOS_ASSERT_CRITICAL(p_eval_release_data->Block,
                         RTOS_ERR_INVALID_STATE, KAL_MON_RES_STOP_EVAL);
    p_eval_release_data->ObjPtr->IOCnt--;
    return (KAL_MON_RES_ALLOW);
  }
  return (KAL_MON_RES_BLOCK);
}

/****************************************************************************************************//**
 *                                           FSObj_IOReleaseImpl()
 *
 * @brief    IO release implementation.
 *
 * @param    p_obj   Pointer to an object.
 *
 * @param    block   Whether to block if IO operations are ongoing.
 *******************************************************************************************************/
static void FSObj_IOReleaseImpl(FS_OBJ      *p_obj,
                                CPU_BOOLEAN block)
{
  FS_OBJ_IO_RELEASE_DATA io_release_data;
  RTOS_ERR               err;

  RTOS_ERR_SET(err, RTOS_ERR_NONE);

  io_release_data.Block = block;
  io_release_data.ObjPtr = p_obj;

  KAL_MonOp(FSObj_IOReleaseMonHandle,
            (void *)&io_release_data,
            FSObj_IOReleaseOnEnter,
            FSObj_IOReleaseOnEval,
            KAL_OPT_NONE,
            0u,
            &err);
}

/****************************************************************************************************//**
 *                                           FSObj_IOAcquireImpl()
 *
 * @brief    IO acquisition callback.
 *
 * @param    p_obj   Pointer to an object.
 *
 * @return   DEF_YES, if acquisition was successful.
 *           DEF_NO, otherwise
 *******************************************************************************************************/
static CPU_BOOLEAN FSObj_IOAcquireImpl(FS_OBJ *p_obj)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  if (p_obj->IOCnt == 0u) {
    CORE_EXIT_ATOMIC();
    return (DEF_NO);
  }

  p_obj->IOCnt++;
  CORE_EXIT_ATOMIC();

  return (DEF_YES);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_FS_AVAIL
