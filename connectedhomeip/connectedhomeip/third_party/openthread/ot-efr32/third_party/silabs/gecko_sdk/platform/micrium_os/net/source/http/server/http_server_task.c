/***************************************************************************//**
 * @file
 * @brief Network - HTTP Server Task Module
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

#if (defined(RTOS_MODULE_NET_HTTP_SERVER_AVAIL))

#if (!defined(RTOS_MODULE_NET_AVAIL))
#error HTTP Server Module requires Network Core module. Make sure it is part of your project \
  and that RTOS_MODULE_NET_AVAIL is defined in rtos_description.h.
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  "http_server_priv.h"

#include  <net/include/http_server.h>

#include  <cpu/include/cpu.h>

#include  <common/source/kal/kal_priv.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/logging/logging_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                     (NET, HTTP)
#define  RTOS_MODULE_CUR                  RTOS_CFG_MODULE_NET

#define  HTTPs_TASK_STR_NAME_TASK                  "HTTP Instance"
#define  HTTPs_TASK_STR_NAME_LOCK                  "HTTP Instance Lock"
#define  HTTPs_TASK_STR_NAME_SEM_STOP_REQ          "HTTP Instance Stop req"
#define  HTTPs_TASK_STR_NAME_SEM_STOP_COMPLETED    "HTTP Instance Stop compl"
#define  HTTPs_TASK_STR_NAME_TMR                   "HTTP Conn Timeout"

#define  HTTPs_OS_LOCK_ACQUIRE_FAIL_DLY_MS                    5u

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void HTTPsTask_InstanceTask(void *p_data);

static void HTTPsTask_InstanceTaskHandler(HTTPs_INSTANCE *p_start_cfg);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           HTTPsTask_LockCreate()
 *
 * @brief    Create OS lock object.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @return   Pointer to the OS lock object, if no error(s)
 *           Null pointer,                  otherwise.
 *******************************************************************************************************/
KAL_LOCK_HANDLE HTTPsTask_LockCreate(RTOS_ERR *p_err)
{
  KAL_LOCK_HANDLE lock_handle;

  //                                                               ---------- ACQUIRE OS_LOCK_OBJ MEM SPACE -----------
  lock_handle = KAL_LockCreate((const CPU_CHAR *)HTTPs_TASK_STR_NAME_LOCK,
                               DEF_NULL,
                               p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_CODE_GET(*p_err));
    goto exit;
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

exit:
  return (lock_handle);
}

/****************************************************************************************************//**
 *                                           HTTPsTask_LockAcquire()
 *
 * @brief    Acquire exclusive resource access.
 *
 * @param    os_lock_obj     OS lock handle.
 *******************************************************************************************************/
void HTTPsTask_LockAcquire(KAL_LOCK_HANDLE os_lock_obj)
{
  RTOS_ERR local_err;

  KAL_LockAcquire(os_lock_obj, KAL_OPT_PEND_BLOCKING, KAL_TIMEOUT_INFINITE, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}

/****************************************************************************************************//**
 *                                           HTTPsTask_LockRelease()
 *
 * @brief    Release exclusive resource access.
 *
 * @param    os_lock_obj     OS lock handle.
 *******************************************************************************************************/
void HTTPsTask_LockRelease(KAL_LOCK_HANDLE os_lock_obj)
{
  RTOS_ERR local_err;

  KAL_LockRelease(os_lock_obj, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}

/****************************************************************************************************//**
 *                                       HTTPsTask_InstanceObjInit()
 *
 * @brief    (1) Initialize OS objects:
 *               - (a) Validate remaining memory available
 *               - (b) Acquire memory space for task control block
 *               - (c) Acquire memory space for task stack
 *               - (d) Create semaphore for instance stop request
 *               - (e) Create semaphore for instance stop request completed
 *
 * @param    p_instance  Pointer to the instance.
 *
 * @param    ----------  Argument validated in HTTPs_InstanceStart().
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void HTTPsTask_InstanceObjInit(HTTPs_INSTANCE *p_instance,
                               RTOS_ERR       *p_err)
{
  HTTPs_OS_TASK_OBJ   *p_os_task_obj;
  const RTOS_TASK_CFG *p_task_cfg;

  p_task_cfg = p_instance->TaskCfgPtr;

  p_os_task_obj = HTTPsMem_InstanceTaskInit(p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  p_instance->OS_TaskObjPtr = p_os_task_obj;

  //                                                               ------- ACQUIRE TASK AND TASK STACK MEM SPACE ------
  p_os_task_obj->TaskHandle = KAL_TaskAlloc(HTTPs_TASK_STR_NAME_TASK,
                                            p_task_cfg->StkPtr,
                                            p_task_cfg->StkSizeElements,
                                            DEF_NULL,
                                            p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_CODE_GET(*p_err));
    goto exit;
  }

  //                                                               --------------- CREATE STOP REQ SEM ----------------
  p_os_task_obj->SemStopReq = KAL_SemCreate(HTTPs_TASK_STR_NAME_SEM_STOP_REQ,
                                            DEF_NULL,
                                            p_err);

  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_CODE_GET(*p_err));
    goto exit;
  }

  //                                                               ------------- CREATE STOP COMPLETE SEM -------------
  p_os_task_obj->SemStopCompleted = KAL_SemCreate(HTTPs_TASK_STR_NAME_SEM_STOP_COMPLETED,
                                                  DEF_NULL,
                                                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_CODE_GET(*p_err));
    goto exit;
  }

  p_os_task_obj->LockObj = HTTPsTask_LockCreate(p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_CODE_GET(*p_err));
    goto exit;
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

exit:
  return;
}

/****************************************************************************************************//**
 *                                       HTTPsTask_InstanceTaskCreate()
 *
 * @brief    Create and start instance HTTP server task.
 *
 * @param    p_instance  Pointer to the instance.
 *
 * @param    ----------  Argument validated in HTTPs_InstanceStart().
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void HTTPsTask_InstanceTaskCreate(HTTPs_INSTANCE *p_instance,
                                  RTOS_ERR       *p_err)
{
  HTTPs_OS_TASK_OBJ   *p_os_task_obj;
  const RTOS_TASK_CFG *p_task_cfg;

  p_os_task_obj = (HTTPs_OS_TASK_OBJ *)p_instance->OS_TaskObjPtr;
  p_task_cfg = p_instance->TaskCfgPtr;

  //                                                               Create HTTP server task.
  KAL_TaskCreate(p_os_task_obj->TaskHandle,
                 HTTPsTask_InstanceTask,
                 (void *)p_instance,
                 (CPU_INT08U)p_task_cfg->Prio,
                 DEF_NULL,
                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    p_instance->Started = DEF_NO;
    RTOS_ERR_SET(*p_err, RTOS_ERR_CODE_GET(*p_err));
    return;
  }

  p_instance->Started = DEF_YES;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                       HTTPsTask_InstanceTaskPrioSet()
 *
 * @brief    Sets the priority of the HTTP server instance task.
 *
 * @param    p_instance  Pointer to the instance.
 *
 * @param    ----------  Argument validated in HTTPs_InstanceTaskPrioSet().
 *
 * @param    prio        Priority of the HTTP server instance task.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_ARG
 *******************************************************************************************************/
void HTTPsTask_InstanceTaskPrioSet(HTTPs_INSTANCE *p_instance,
                                   KAL_TASK_PRIO  prio,
                                   RTOS_ERR       *p_err)
{
  HTTPs_OS_TASK_OBJ *p_os_task_obj = (HTTPs_OS_TASK_OBJ *)p_instance->OS_TaskObjPtr;

  KAL_TaskPrioSet(p_os_task_obj->TaskHandle, prio, p_err);
}

/****************************************************************************************************//**
 *                                       HTTPsTask_InstanceTaskDel()
 *
 * @brief    Stop and delete instance task.
 *
 * @param    p_instance  Pointer to the instance.
 *
 * @param    ----------  Argument validated in HTTPs_InstanceStart().
 *******************************************************************************************************/
void HTTPsTask_InstanceTaskDel(HTTPs_INSTANCE *p_instance)
{
  HTTPs_OS_TASK_OBJ *p_os_task_obj;

  p_instance->Started = DEF_NO;                                 // Stop Instance.

  p_os_task_obj = (HTTPs_OS_TASK_OBJ *)p_instance->OS_TaskObjPtr;

  KAL_TaskDel(p_os_task_obj->TaskHandle);
}

/****************************************************************************************************//**
 *                                       HTTPsTask_InstanceStopReqSignal()
 *
 * @brief    Signal that the instance must be stopped.
 *
 * @param    p_instance  Pointer to the instance.
 *
 * @param    ----------  Argument validated in HTTPs_InstanceStop().
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void HTTPsTask_InstanceStopReqSignal(HTTPs_INSTANCE *p_instance)
{
  HTTPs_OS_TASK_OBJ *p_os_task_obj;
  RTOS_ERR          local_err;

  p_os_task_obj = (HTTPs_OS_TASK_OBJ *)p_instance->OS_TaskObjPtr;

  KAL_SemPost(p_os_task_obj->SemStopReq,                        // Signal instance that stop is requested.
              KAL_OPT_POST_NONE,
              &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}

/****************************************************************************************************//**
 *                                   HTTPsTask_InstanceStopReqPending()
 *
 * @brief    Get stop request.
 *
 * @param    p_instance  Pointer to the instance.
 *
 * @param    ----------  Argument validated in HTTPs_InstanceStart().
 *
 * @return   DEF_YES, if stop is requested or a fatal error occurred.
 *           DEF_NO,  otherwise.
 *******************************************************************************************************/
CPU_BOOLEAN HTTPsTask_InstanceStopReqPending(HTTPs_INSTANCE *p_instance)
{
  HTTPs_OS_TASK_OBJ *p_os_task_obj;
  RTOS_ERR          local_err;

  p_os_task_obj = (HTTPs_OS_TASK_OBJ *)p_instance->OS_TaskObjPtr;

  KAL_SemPend(p_os_task_obj->SemStopReq,
              KAL_OPT_PEND_NON_BLOCKING,
              1u,
              &local_err);
  switch (RTOS_ERR_CODE_GET(local_err)) {
    case RTOS_ERR_NONE:                                         // Event occurred.
      return (DEF_YES);

    case RTOS_ERR_WOULD_BLOCK:
    case RTOS_ERR_TIMEOUT:
      return (DEF_NO);

    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_YES);
  }

  return (DEF_YES);
}

/****************************************************************************************************//**
 *                                   HTTPsTask_InstanceStopCompletedSignal()
 *
 * @brief    Signal that the stop request has been completed.
 *
 * @param    p_instance  Pointer to the instance.
 *
 * @param    ----------  Argument validated in HTTPs_InstanceStart().
 *******************************************************************************************************/
void HTTPsTask_InstanceStopCompletedSignal(HTTPs_INSTANCE *p_instance)
{
  HTTPs_OS_TASK_OBJ *p_os_task_obj;
  RTOS_ERR          local_err;

  p_os_task_obj = (HTTPs_OS_TASK_OBJ *)p_instance->OS_TaskObjPtr;

  KAL_SemPost(p_os_task_obj->SemStopCompleted,
              KAL_OPT_POST_NONE,
              &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}

/****************************************************************************************************//**
 *                                   HTTPsTask_InstanceStopCompletedPending()
 *
 * @brief    Wait until the stop request has been completed.
 *
 * @param    p_instance  Pointer to the instance structure variable.
 *
 * @param    ----------  Argument validated in HTTPs_InstanceStop().
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @param    -----       Argument validated in HTTPs_InstanceStop().
 *******************************************************************************************************/
void HTTPsTask_InstanceStopCompletedPending(HTTPs_INSTANCE *p_instance,
                                            RTOS_ERR       *p_err)
{
  HTTPs_OS_TASK_OBJ *p_os_task_obj;

  p_os_task_obj = (HTTPs_OS_TASK_OBJ *)p_instance->OS_TaskObjPtr;

  KAL_SemPend(p_os_task_obj->SemStopCompleted,
              KAL_OPT_PEND_BLOCKING,
              0u,                                               // Infinite timeout.
              p_err);
}

/****************************************************************************************************//**
 *                                           HTTPsTask_TimeDly_ms()
 *
 * @brief    Delay for specified time, in milliseconds.
 *
 * @param    time_dly_ms     Time delay value, in millisecond.
 *******************************************************************************************************/
void HTTPsTask_TimeDly_ms(CPU_INT32U time_dly_ms)
{
  KAL_Dly(time_dly_ms);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           HTTPs_InstanceTask()
 *
 * @brief    Shell task to schedule & run HTTP instance task handler.
 *               - (1) Shell task's primary purpose is to schedule & run HTTPs_InstanceTaskHandler() forever.
 *
 * @param    p_data  Pointer to task initialization (required by Micrium OS Kernel).
 *******************************************************************************************************/
static void HTTPsTask_InstanceTask(void *p_data)
{
  HTTPs_INSTANCE *p_instance;

  p_instance = (HTTPs_INSTANCE *)p_data;

  while (DEF_ON) {
    HTTPsTask_InstanceTaskHandler(p_instance);                  // Call HTTP server task handler.
  }
}

/****************************************************************************************************//**
 *                                       HTTPsTask_InstanceTaskHandler()
 *
 * @brief    HTTP server main loop.
 *
 * @param    p_instance  Pointer to the instance.
 *
 * @param    ----------  Argument validated in HTTPs_InstanceStart().
 *******************************************************************************************************/
static void HTTPsTask_InstanceTaskHandler(HTTPs_INSTANCE *p_instance)
{
  const HTTPs_CFG *p_cfg = p_instance->CfgPtr;
  NET_SOCK_QTY    sock_nbr_rdy = 0;
  CPU_BOOLEAN     close_pending = DEF_NO;
  CPU_BOOLEAN     closed = DEF_NO;
  CPU_BOOLEAN     accept;
#if (HTTPs_CFG_FS_PRESENT_EN == DEF_ENABLED)
  const NET_FS_API *p_fs_api;
  CPU_CHAR         *p_working_folder;
  CPU_BOOLEAN      result;
#endif
  HTTPs_INSTANCE_ERRS *p_ctr_err = DEF_NULL;

  HTTPs_SET_PTR_ERRS(p_ctr_err, p_instance);
  //                                                               ---------------- SET WORKING FOLDER ----------------
  switch (p_cfg->FS_Type) {
    case HTTPs_FS_TYPE_NONE:
      break;

    case HTTPs_FS_TYPE_STATIC:
      break;

    case HTTPs_FS_TYPE_DYN:
#if (HTTPs_CFG_FS_PRESENT_EN == DEF_ENABLED)
      p_fs_api = ((HTTPs_CFG_FS_DYN *)p_cfg->FS_CfgPtr)->FS_API_Ptr;
      p_working_folder = ((HTTPs_CFG_FS_DYN *)p_cfg->FS_CfgPtr)->WorkingFolderNamePtr;
      if (p_fs_api != DEF_NULL) {
        if ((p_fs_api->WorkingFolderSet != DEF_NULL)
            && (p_working_folder != DEF_NULL)) {
          Str_Copy_N(p_instance->FS_WorkingDirPtr, p_working_folder, p_instance->CfgPtr->PathLenMax);
          if (p_instance->FS_PathSepChar != HTTPs_PATH_SEP_CHAR_DFLT) {
            Str_Char_Replace_N(p_instance->FS_WorkingDirPtr,
                               HTTPs_PATH_SEP_CHAR_DFLT,
                               p_instance->FS_PathSepChar,
                               p_instance->CfgPtr->PathLenMax);
          }
          result = p_fs_api->WorkingFolderSet(p_instance->FS_WorkingDirPtr);
          if (result != DEF_OK) {
            HTTPs_ERR_INC(p_ctr_err->FS_ErrWorkingFolderInvalidCtr);
            RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; )
          }
        }
      }
#else
      HTTPs_ERR_INC(p_ctr_err->FS_ErrNoEnCtr);
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; )
#endif
      break;

    default:
      HTTPs_ERR_INC(p_ctr_err->FS_ErrTypeInvalidCtr);
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; )
  }

  PP_UNUSED_PARAM(p_ctr_err);

  while (DEF_ON) {
    //                                                             -------------------- DELAY TASK --------------------
    if (p_cfg->OS_TaskDly_ms > 0) {
      HTTPsTask_TimeDly_ms(p_cfg->OS_TaskDly_ms);
    }

    //                                                             -------------- ACQUIRE INSTANCE LOCK ---------------
    HTTPsTask_LockAcquire(p_instance->OS_TaskObjPtr->LockObj);

    //                                                             -------------- CHECK FOR ASYNC CLOSE ---------------
    if (close_pending == DEF_NO) {                              // If close NOT already signaled.
      close_pending = HTTPsTask_InstanceStopReqPending(p_instance);
    }

    if (close_pending == DEF_YES) {                             // If stop is requested.
      if (closed == DEF_NO) {                                   // If not closed.
                                                                // ---------------- CLOSE LISTEN SOCK -----------------
        switch (p_cfg->SockSel) {
          case HTTPs_SOCK_SEL_IPv4:
#ifdef   NET_IPv4_MODULE_EN
            HTTPsSock_ListenClose(p_instance, p_instance->SockListenID_IPv4);
#endif
            break;

          case HTTPs_SOCK_SEL_IPv6:
#ifdef   NET_IPv6_MODULE_EN
            HTTPsSock_ListenClose(p_instance, p_instance->SockListenID_IPv6);
#endif
            break;

          case HTTPs_SOCK_SEL_IPv4_IPv6:
#ifdef   NET_IPv4_MODULE_EN
            HTTPsSock_ListenClose(p_instance, p_instance->SockListenID_IPv4);
#endif
#ifdef   NET_IPv6_MODULE_EN
            HTTPsSock_ListenClose(p_instance, p_instance->SockListenID_IPv6);
#endif
            break;

          default:
            break;
        }
        //                                                         Close listen sock: discard incoming conn.
        closed = DEF_YES;
      } else if (p_instance->ConnActiveCtr == 0) {              // If no more active connection:
        HTTPsTask_InstanceStopCompletedSignal(p_instance);          // Signal that the stop is completed.
        HTTPsTask_LockRelease(p_instance->OS_TaskObjPtr->LockObj);         // Release instance lock.
        HTTPsTask_TimeDly_ms(500);                              // Add delay to permit context switch.
        return;
      }
    }

    //                                                             ------------------- SEL CONN RDY -------------------
    accept = !closed;
    HTTPsTask_LockRelease(p_instance->OS_TaskObjPtr->LockObj);
    sock_nbr_rdy = HTTPsSock_ConnSel(p_instance, accept);
    HTTPsTask_LockAcquire(p_instance->OS_TaskObjPtr->LockObj);

    //                                                             ----------------- PROCESS CONN RDY -----------------
    if (sock_nbr_rdy > 0) {
      HTTPsConn_Process(p_instance);
    }

    //                                                             -------------- RELEASE INSTANCE LOCK ---------------
    HTTPsTask_LockRelease(p_instance->OS_TaskObjPtr->LockObj);
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_HTTP_SERVER_AVAIL
