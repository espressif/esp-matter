/***************************************************************************//**
 * @file
 * @brief USB Device Real-Time Kernel Layer - Kal
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

#if (defined(RTOS_MODULE_USB_DEV_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <em_core.h>

#include  <usb/include/device/usbd_core.h>
#include  <usb/source/device/core/usbd_core_priv.h>
#include  <common/source/kal/kal_priv.h>
#include  <common/include/rtos_err.h>
#include  <common/source/rtos/rtos_utils_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                               (USBD, OS)
#define  RTOS_MODULE_CUR                            RTOS_CFG_MODULE_USBD

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef struct usbd_os {
  KAL_TASK_HANDLE *DevTaskHandleTbl;

  KAL_Q_HANDLE    *CoreEventQHandleTbl;

  KAL_SEM_HANDLE  **EP_SemHandleTbl;
  KAL_LOCK_HANDLE **EP_LockHandleTbl;
} USBD_OS;

static USBD_OS *USBD_OS_Ptr;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void USBD_OS_CoreTask(void *p_arg);

/****************************************************************************************************//**
 *                                               USBD_OS_Init()
 *
 * @brief    Initialize OS interface.
 *
 * @param    p_cfg       Pointer to USBD configuration structure.
 *
 * @param    p_mem_seg   Pointer to memory segment that will be used for allocation.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void USBD_OS_Init(USBD_QTY_CFG *p_cfg,
                  MEM_SEG      *p_mem_seg,
                  RTOS_ERR     *p_err)
{
  CPU_INT08U  dev_nbr;
  CPU_BOOLEAN is_avail;

  //                                                               Make sure every feature needed is avail.
  is_avail = KAL_FeatureQuery(KAL_FEATURE_TASK_CREATE, KAL_OPT_CREATE_NONE);
  is_avail &= KAL_FeatureQuery(KAL_FEATURE_LOCK_CREATE, KAL_OPT_CREATE_NONE);
  is_avail &= KAL_FeatureQuery(KAL_FEATURE_LOCK_ACQUIRE, KAL_OPT_PEND_BLOCKING);
  is_avail &= KAL_FeatureQuery(KAL_FEATURE_LOCK_RELEASE, KAL_OPT_POST_NONE);
  is_avail &= KAL_FeatureQuery(KAL_FEATURE_LOCK_DEL, KAL_OPT_DEL_NONE);
  is_avail &= KAL_FeatureQuery(KAL_FEATURE_SEM_CREATE, KAL_OPT_CREATE_NONE);
  is_avail &= KAL_FeatureQuery(KAL_FEATURE_SEM_PEND, KAL_OPT_PEND_BLOCKING);
  is_avail &= KAL_FeatureQuery(KAL_FEATURE_SEM_POST, KAL_OPT_POST_NONE);
  is_avail &= KAL_FeatureQuery(KAL_FEATURE_SEM_ABORT, KAL_OPT_ABORT_NONE);
  is_avail &= KAL_FeatureQuery(KAL_FEATURE_SEM_DEL, KAL_OPT_DEL_NONE);
  is_avail &= KAL_FeatureQuery(KAL_FEATURE_Q_CREATE, KAL_OPT_CREATE_NONE);
  is_avail &= KAL_FeatureQuery(KAL_FEATURE_Q_PEND, KAL_OPT_PEND_BLOCKING);
  is_avail &= KAL_FeatureQuery(KAL_FEATURE_Q_POST, KAL_OPT_POST_NONE);
  is_avail &= KAL_FeatureQuery(KAL_FEATURE_DLY, KAL_OPT_DLY);
  is_avail &= KAL_FeatureQuery(KAL_FEATURE_PEND_TIMEOUT, KAL_OPT_NONE);
  if (is_avail != DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_AVAIL);
    return;
  }

  USBD_OS_Ptr = (USBD_OS *)Mem_SegAlloc("USBD - OS root struct",
                                        p_mem_seg,
                                        sizeof(USBD_OS),
                                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBD_OS_Ptr->DevTaskHandleTbl = (KAL_TASK_HANDLE *)Mem_SegAlloc("USBD - Dev task handle tbl",
                                                                  p_mem_seg,
                                                                  sizeof(KAL_TASK_HANDLE) * p_cfg->DevQty,
                                                                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBD_OS_Ptr->EP_SemHandleTbl = (KAL_SEM_HANDLE **)Mem_SegAlloc("USBD - OS EP sem handle table",
                                                                 p_mem_seg,
                                                                 sizeof(KAL_SEM_HANDLE *) * p_cfg->DevQty,
                                                                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBD_OS_Ptr->EP_LockHandleTbl = (KAL_LOCK_HANDLE **)Mem_SegAlloc("USBD - OS EP lock handle table",
                                                                   p_mem_seg,
                                                                   sizeof(KAL_LOCK_HANDLE *) * p_cfg->DevQty,
                                                                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBD_OS_Ptr->CoreEventQHandleTbl = (KAL_Q_HANDLE *)Mem_SegAlloc("USBD - Core event Q handle tbl",
                                                                  p_mem_seg,
                                                                  sizeof(KAL_Q_HANDLE) * p_cfg->DevQty,
                                                                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  for (dev_nbr = 0u; dev_nbr < p_cfg->DevQty; dev_nbr++) {
    USBD_OS_Ptr->DevTaskHandleTbl[dev_nbr] = KAL_TaskHandleNull;

    USBD_OS_Ptr->EP_SemHandleTbl[dev_nbr] = (KAL_SEM_HANDLE *)Mem_SegAlloc("USBD - OS EP sem handle table",
                                                                           p_mem_seg,
                                                                           sizeof(KAL_SEM_HANDLE) * p_cfg->EP_OpenQty,
                                                                           p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    USBD_OS_Ptr->EP_LockHandleTbl[dev_nbr] = (KAL_LOCK_HANDLE *)Mem_SegAlloc("USBD - OS EP lock handle table",
                                                                             p_mem_seg,
                                                                             sizeof(KAL_LOCK_HANDLE) * p_cfg->EP_OpenQty,
                                                                             p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
  }

  return;
}

/****************************************************************************************************//**
 *                                           USBD_OS_TaskCreate()
 *
 * @brief    Create task and queue for task to pend on.
 *
 * @param    p_task_cfg  Pointer to task configuration structure.
 *
 * @param    dev_nbr     0 if only a task is needed, device number associated with task, if one task
 *                       per device is created.
 *
 * @param    event_qty   Quantity of core events.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void USBD_OS_TaskCreate(RTOS_TASK_CFG *p_task_cfg,
                        CPU_INT08U    dev_nbr,
                        CPU_INT16U    event_qty,
                        RTOS_ERR      *p_err)
{
  KAL_Q_HANDLE    q_handle;
  KAL_TASK_HANDLE task_handle;
  CORE_DECLARE_IRQ_STATE;

  q_handle = KAL_QCreate("USB Core Task Event Q",
                         event_qty,
                         DEF_NULL,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBD_OS_Ptr->CoreEventQHandleTbl[dev_nbr] = q_handle;

  task_handle = KAL_TaskAlloc("USBD Core Task",
                              p_task_cfg->StkPtr,
                              p_task_cfg->StkSizeElements,
                              DEF_NULL,
                              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  KAL_TaskCreate(task_handle,
                 USBD_OS_CoreTask,
                 (void *)(CPU_ADDR)dev_nbr,
                 p_task_cfg->Prio,
                 DEF_NULL,
                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  CORE_ENTER_ATOMIC();
  USBD_OS_Ptr->DevTaskHandleTbl[dev_nbr] = task_handle;
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                           USBD_OS_DevTaskPrioSet()
 *
 * @brief    Sets priority of the given devcie's task.
 *
 * @param    dev_nbr     Device number.
 *
 * @param    prio        Priority of the device's task.
 *
 * @param    p_err       Pointer to error variable.
 *******************************************************************************************************/
void USBD_OS_DevTaskPrioSet(CPU_INT08U     dev_nbr,
                            RTOS_TASK_PRIO prio,
                            RTOS_ERR       *p_err)
{
  KAL_TASK_HANDLE task_handle;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  task_handle = USBD_OS_Ptr->DevTaskHandleTbl[dev_nbr];
  CORE_EXIT_ATOMIC();

  KAL_TaskPrioSet(task_handle,
                  prio,
                  p_err);
}

/****************************************************************************************************//**
 *                                           USBD_OS_CoreTask()
 *
 * @brief    OS-dependent shell task to process USB core events.
 *
 * @param    p_arg   Pointer to task initialization argument (required by uC/OS-II).
 *
 *           USBD_OS_Init().
 *******************************************************************************************************/
static void USBD_OS_CoreTask(void *p_arg)
{
  CPU_INT08U dev_nbr = (CPU_INT08U)(CPU_ADDR)p_arg;

  while (DEF_ON) {
    USBD_CoreTaskHandler(dev_nbr);
  }
}

/****************************************************************************************************//**
 *                                           USBD_OS_SignalCreate()
 *
 * @brief    Create an OS signal.
 *
 * @param    dev_nbr     Device number.
 *
 * @param    -------     Argument validated by the caller(s).
 *
 * @param    ep_ix       Endpoint index.
 *
 * @param    -----       Argument validated by the caller(s).
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void USBD_OS_EP_SignalCreate(CPU_INT08U dev_nbr,
                             CPU_INT08U ep_ix,
                             RTOS_ERR   *p_err)
{
  KAL_SEM_HANDLE sem_handle;

  sem_handle = KAL_SemCreate("USB-Device EP Signal",
                             DEF_NULL,
                             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
    USBD_OS_Ptr->EP_SemHandleTbl[dev_nbr][ep_ix] = sem_handle;
  }
}

/****************************************************************************************************//**
 *                                           USBD_OS_SignalDel()
 *
 * @brief    Delete an OS signal.
 *
 * @param    dev_nbr     Device number.
 *
 * @param    -------     Argument validated by the caller(s).
 *
 * @param    ep_ix       Endpoint index.
 *
 * @param    -------     Argument validated by the caller(s).
 *******************************************************************************************************/
void USBD_OS_EP_SignalDel(CPU_INT08U dev_nbr,
                          CPU_INT08U ep_ix)
{
  KAL_SEM_HANDLE sem_handle;

  sem_handle = USBD_OS_Ptr->EP_SemHandleTbl[dev_nbr][ep_ix];

  KAL_SemDel(sem_handle);

  USBD_OS_Ptr->EP_SemHandleTbl[dev_nbr][ep_ix] = KAL_SemHandleNull;
}

/****************************************************************************************************//**
 *                                           USBD_OS_SignalPend()
 *
 * @brief    Wait for a signal to become available.
 *
 * @param    dev_nbr     Device number.
 *
 * @param    -------     Argument validated by the caller(s).
 *
 * @param    ep_ix       Endpoint index.
 *
 * @param    -------     Argument validated by the caller(s).
 *
 * @param    timeout_ms  Signal wait timeout in milliseconds.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void USBD_OS_EP_SignalPend(CPU_INT08U dev_nbr,
                           CPU_INT08U ep_ix,
                           CPU_INT16U timeout_ms,
                           RTOS_ERR   *p_err)
{
  KAL_SEM_HANDLE sem_handle;

  sem_handle = USBD_OS_Ptr->EP_SemHandleTbl[dev_nbr][ep_ix];

  KAL_SemPend(sem_handle,
              KAL_OPT_PEND_BLOCKING,
              timeout_ms,
              p_err);
}

/****************************************************************************************************//**
 *                                           USBD_OS_SignalAbort()
 *
 * @brief    Abort any wait operation on signal.
 *
 * @param    dev_nbr     Device number.
 *
 * @param    -------     Argument validated by the caller(s).
 *
 * @param    ep_ix       Endpoint index.
 *
 * @param    -------     Argument validated by the caller(s).
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void USBD_OS_EP_SignalAbort(CPU_INT08U dev_nbr,
                            CPU_INT08U ep_ix,
                            RTOS_ERR   *p_err)
{
  KAL_SEM_HANDLE sem_handle;

  sem_handle = USBD_OS_Ptr->EP_SemHandleTbl[dev_nbr][ep_ix];

  KAL_SemPendAbort(sem_handle,
                   p_err);
}

/****************************************************************************************************//**
 *                                           USBD_OS_SignalPost()
 *
 * @brief    Make a signal available.
 *
 * @param    dev_nbr     Device number.
 *
 * @param    -------     Argument validated by the caller(s).
 *
 * @param    ep_ix       Endpoint index.
 *
 * @param    -------     Argument validated by the caller(s).
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void USBD_OS_EP_SignalPost(CPU_INT08U dev_nbr,
                           CPU_INT08U ep_ix,
                           RTOS_ERR   *p_err)
{
  KAL_SEM_HANDLE sem_handle;

  sem_handle = USBD_OS_Ptr->EP_SemHandleTbl[dev_nbr][ep_ix];

  KAL_SemPost(sem_handle,
              KAL_OPT_POST_NONE,
              p_err);
}

/****************************************************************************************************//**
 *                                           USBD_OS_EP_LockCreate()
 *
 * @brief    Create an OS resource to use as an endpoint lock.
 *
 * @param    dev_nbr     Device number.
 *
 * @param    -------     Argument validated by the caller(s).
 *
 * @param    ep_ix       Endpoint index.
 *
 * @param    -------     Argument validated by the caller(s).
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void USBD_OS_EP_LockCreate(CPU_INT08U dev_nbr,
                           CPU_INT08U ep_ix,
                           RTOS_ERR   *p_err)
{
  KAL_LOCK_HANDLE lock_handle;

  lock_handle = KAL_LockCreate("USB-Device EP Mutex",
                               DEF_NULL,
                               p_err);
  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
    USBD_OS_Ptr->EP_LockHandleTbl[dev_nbr][ep_ix] = lock_handle;
  }
}

/****************************************************************************************************//**
 *                                           USBD_OS_EP_LockDel()
 *
 * @brief    Delete the OS resource used as an endpoint lock.
 *
 * @param    dev_nbr     Device number.
 *
 * @param    -------     Argument validated by the caller(s).
 *
 * @param    ep_ix       Endpoint index.
 *
 * @param    -------     Argument validated by the caller(s).
 *******************************************************************************************************/
void USBD_OS_EP_LockDel(CPU_INT08U dev_nbr,
                        CPU_INT08U ep_ix)
{
  KAL_LOCK_HANDLE lock_handle;

  lock_handle = USBD_OS_Ptr->EP_LockHandleTbl[dev_nbr][ep_ix];

  KAL_LockDel(lock_handle);

  USBD_OS_Ptr->EP_LockHandleTbl[dev_nbr][ep_ix] = KAL_LockHandleNull;
}

/****************************************************************************************************//**
 *                                           USBD_OS_EP_LockAcquire()
 *
 * @brief    Wait for an endpoint to become available and acquire its lock.
 *
 * @param    dev_nbr     Device number.
 *
 * @param    -------     Argument validated by the caller(s).
 *
 * @param    ep_ix       Endpoint index.
 *
 * @param    -------     Argument validated by the caller(s).
 *
 * @param    timeout_ms  Lock wait timeout in milliseconds.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void USBD_OS_EP_LockAcquire(CPU_INT08U dev_nbr,
                            CPU_INT08U ep_ix,
                            RTOS_ERR   *p_err)
{
  KAL_LOCK_HANDLE lock_handle;

  lock_handle = USBD_OS_Ptr->EP_LockHandleTbl[dev_nbr][ep_ix];
  if (KAL_LOCK_HANDLE_IS_NULL(lock_handle) == DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_HANDLE);
    return;
  }

  KAL_LockAcquire(lock_handle,
                  KAL_OPT_PEND_BLOCKING,
                  KAL_TIMEOUT_INFINITE,
                  p_err);
}

/****************************************************************************************************//**
 *                                           USBD_OS_EP_LockRelease()
 *
 * @brief    Release an endpoint lock.
 *
 * @param    dev_nbr     Device number.
 *
 * @param    -------     Argument validated by the caller(s).
 *
 * @param    ep_ix       Endpoint index.
 *
 * @param    -------     Argument validated by the caller(s).
 *******************************************************************************************************/
void USBD_OS_EP_LockRelease(CPU_INT08U dev_nbr,
                            CPU_INT08U ep_ix)
{
  KAL_LOCK_HANDLE lock_handle;
  RTOS_ERR        err;

  lock_handle = USBD_OS_Ptr->EP_LockHandleTbl[dev_nbr][ep_ix];
  if (KAL_LOCK_HANDLE_IS_NULL(lock_handle) == DEF_YES) {
    return;
  }

  KAL_LockRelease(lock_handle,
                  &err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), RTOS_ERR_OS,; );
}

/****************************************************************************************************//**
 *                                               USBD_OS_DlyMs()
 *
 * @brief    Delay a task for a certain time.
 *
 * @param    ms  Delay in milliseconds.
 *******************************************************************************************************/
void USBD_OS_DlyMs(CPU_INT32U ms)
{
  KAL_Dly(ms);
}

/****************************************************************************************************//**
 *                                           USBD_OS_CoreEventGet()
 *
 * @brief    Wait until a core event is ready.
 *
 * @param    timeout_ms  Timeout in milliseconds.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void *USBD_OS_CoreEventGet(CPU_INT08U dev_nbr,
                           RTOS_ERR   *p_err)
{
  KAL_Q_HANDLE q_handle;
  void         *p_msg;

  q_handle = USBD_OS_Ptr->CoreEventQHandleTbl[dev_nbr];

  p_msg = KAL_QPend(q_handle,
                    KAL_OPT_PEND_BLOCKING,
                    KAL_TIMEOUT_INFINITE,
                    p_err);

  return (p_msg);
}

/****************************************************************************************************//**
 *                                           USBD_OS_CoreEventPut()
 *
 * @brief    Queues core event.
 *
 * @param    p_event     Pointer to core event.
 *******************************************************************************************************/
void USBD_OS_CoreEventPut(CPU_INT08U dev_nbr,
                          void       *p_event)
{
  KAL_Q_HANDLE q_handle;
  RTOS_ERR     err;

  q_handle = USBD_OS_Ptr->CoreEventQHandleTbl[dev_nbr];

  KAL_QPost(q_handle,
            p_event,
            KAL_OPT_POST_NONE,
            &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    LOG_ERR(("USBD : Unable to post core event. Err = ", RTOS_ERR_LOG_ARG_GET(err)));
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_USB_DEV_AVAIL))
