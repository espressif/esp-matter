/***************************************************************************//**
 * @file
 * @brief USB Device - USB Hid Class Operating System Layer - Micrium Ucos-Iii
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

#if (defined(RTOS_MODULE_USB_DEV_HID_AVAIL))

#if (!defined(RTOS_MODULE_USB_DEV_AVAIL))

#error USB Device HID class requires USB Device Core. Make sure it is part of your project and that \
  RTOS_MODULE_USB_DEV_AVAIL is defined in rtos_description.h.

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#define    MICRIUM_SOURCE
#include  <em_core.h>

#include  <usb/include/device/usbd_core.h>
#include  <usb/include/device/usbd_hid.h>

#include  <usb/source/device/class/usbd_hid_os_priv.h>
#include  <usb/source/device/class/usbd_hid_priv.h>

#include  <common/source/kal/kal_priv.h>
#include  <common/include/rtos_err.h>
#include  <common/include/rtos_prio.h>
#include  <common/source/rtos/rtos_utils_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                               (USBD, CLASS, HID, OS)
#define  RTOS_MODULE_CUR                            RTOS_CFG_MODULE_USBD

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef struct usbd_hid_os {
  KAL_SEM_HANDLE  *InputDataSemHandleTbl;
  KAL_LOCK_HANDLE *InputLockHandleTbl;
  KAL_LOCK_HANDLE *TxLockHandleTbl;
  KAL_LOCK_HANDLE *OutputLockHandleTbl;
  KAL_SEM_HANDLE  *OutputDataSemHandleTbl;
  KAL_TASK_HANDLE TmrTaskHandle;
} USBD_HID_OS;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static USBD_HID_OS *USBD_HID_OS_Ptr = DEF_NULL;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void USBD_HID_OS_TmrTask(void *p_arg);

/****************************************************************************************************//**
 *                                           USBD_HID_OS_Init()
 *
 * @brief    Initialize HID OS interface.
 *
 * @param    p_mem_seg           Pointer to memory segmeent from where to allocate internal data.
 *
 * @param    p_cfg               Pointer to HID class configuration structure.
 *
 * @param    stk_size_elements   Size of timer task's stack, in quantity of elements.
 *
 * @param    p_stk               Pointer to base of timer task's stack.
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void USBD_HID_OS_Init(MEM_SEG          *p_mem_seg,
                      USBD_HID_QTY_CFG *p_cfg,
                      CPU_INT32U       stk_size_elements,
                      void             *p_stk,
                      RTOS_ERR         *p_err)
{
  CPU_BOOLEAN      is_avail;
  CPU_INT08U       class_nbr;
  KAL_TICK_RATE_HZ tick_rate;
  USBD_HID_OS      *p_hid_os;
  CORE_DECLARE_IRQ_STATE;

  //                                                               ---- VALIDATE HID REPORT TIMER/OS CONFIGURATION ----
  tick_rate = KAL_TickRateGet();
  if (tick_rate < 250u) {                                       // If OS ticker frequency < HID report timer task ...
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_CFG);                 // ... frequency, return error.
    return;
  }

  //                                                               Make sure every feature needed is avail.
  is_avail = KAL_FeatureQuery(KAL_FEATURE_TASK_CREATE, KAL_OPT_CREATE_NONE);
  is_avail &= KAL_FeatureQuery(KAL_FEATURE_LOCK_CREATE, KAL_OPT_CREATE_NONE);
  is_avail &= KAL_FeatureQuery(KAL_FEATURE_LOCK_ACQUIRE, KAL_OPT_PEND_BLOCKING);
  is_avail &= KAL_FeatureQuery(KAL_FEATURE_LOCK_RELEASE, KAL_OPT_POST_NONE);
  is_avail &= KAL_FeatureQuery(KAL_FEATURE_SEM_CREATE, KAL_OPT_CREATE_NONE);
  is_avail &= KAL_FeatureQuery(KAL_FEATURE_SEM_PEND, KAL_OPT_PEND_BLOCKING);
  is_avail &= KAL_FeatureQuery(KAL_FEATURE_SEM_POST, KAL_OPT_POST_NONE);
  is_avail &= KAL_FeatureQuery(KAL_FEATURE_SEM_ABORT, KAL_OPT_ABORT_NONE);
  if (is_avail != DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_AVAIL);
    return;
  }

  p_hid_os = (USBD_HID_OS *)Mem_SegAlloc("USBD - HID OS root struct",
                                         p_mem_seg,
                                         sizeof(USBD_HID_OS),
                                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_hid_os->TmrTaskHandle = KAL_TaskHandleNull;

  CORE_ENTER_ATOMIC();
  USBD_HID_OS_Ptr = p_hid_os;
  CORE_EXIT_ATOMIC();

  USBD_HID_OS_Ptr->InputDataSemHandleTbl = (KAL_SEM_HANDLE *)Mem_SegAlloc("USBD - HID OS Input data sem tbl",
                                                                          p_mem_seg,
                                                                          sizeof(KAL_SEM_HANDLE) * p_cfg->ClassInstanceQty,
                                                                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBD_HID_OS_Ptr->InputLockHandleTbl = (KAL_LOCK_HANDLE *)Mem_SegAlloc("USBD - HID OS Input lock tbl",
                                                                        p_mem_seg,
                                                                        sizeof(KAL_LOCK_HANDLE) * p_cfg->ClassInstanceQty,
                                                                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBD_HID_OS_Ptr->TxLockHandleTbl = (KAL_LOCK_HANDLE *)Mem_SegAlloc("USBD - HID OS Tx lock tbl",
                                                                     p_mem_seg,
                                                                     sizeof(KAL_LOCK_HANDLE) * p_cfg->ClassInstanceQty,
                                                                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBD_HID_OS_Ptr->OutputLockHandleTbl = (KAL_LOCK_HANDLE *)Mem_SegAlloc("USBD - HID OS output lock tbl",
                                                                         p_mem_seg,
                                                                         sizeof(KAL_LOCK_HANDLE) * p_cfg->ClassInstanceQty,
                                                                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBD_HID_OS_Ptr->OutputDataSemHandleTbl = (KAL_SEM_HANDLE *)Mem_SegAlloc("USBD - HID OS output data sem tbl",
                                                                           p_mem_seg,
                                                                           sizeof(KAL_SEM_HANDLE) * p_cfg->ClassInstanceQty,
                                                                           p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  for (class_nbr = 0u; class_nbr < p_cfg->ClassInstanceQty; class_nbr++) {
    USBD_HID_OS_Ptr->TxLockHandleTbl[class_nbr] = KAL_LockCreate("USBD HID Tx Lock",
                                                                 DEF_NULL,
                                                                 p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    USBD_HID_OS_Ptr->OutputLockHandleTbl[class_nbr] = KAL_LockCreate("USBD HID Output Lock",
                                                                     DEF_NULL,
                                                                     p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    USBD_HID_OS_Ptr->OutputDataSemHandleTbl[class_nbr] = KAL_SemCreate("USBD HID Output Data Signal",
                                                                       DEF_NULL,
                                                                       p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    USBD_HID_OS_Ptr->InputLockHandleTbl[class_nbr] = KAL_LockCreate("USBD HID Input Lock",
                                                                    DEF_NULL,
                                                                    p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    USBD_HID_OS_Ptr->InputDataSemHandleTbl[class_nbr] = KAL_SemCreate("USBD HID Input Data Signal",
                                                                      DEF_NULL,
                                                                      p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
  }

  USBD_HID_OS_Ptr->TmrTaskHandle = KAL_TaskAlloc("USBD HID Timer Task",
                                                 (CPU_STK *)p_stk,
                                                 stk_size_elements,
                                                 DEF_NULL,
                                                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  KAL_TaskCreate(USBD_HID_OS_Ptr->TmrTaskHandle,
                 USBD_HID_OS_TmrTask,
                 DEF_NULL,
                 USBD_HID_TMR_TASK_PRIO_DFLT,
                 DEF_NULL,
                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
}

/****************************************************************************************************//**
 *                                       USBD_HID_OS_TmrTaskPrioSet()
 *
 * @brief    Sets priority of the HID timer task.
 *
 * @param    prio    Priority of the timer task.
 *
 * @param    p_err   Pointer to error variable.
 *******************************************************************************************************/
void USBD_HID_OS_TmrTaskPrioSet(RTOS_TASK_PRIO prio,
                                RTOS_ERR       *p_err)
{
  KAL_TASK_HANDLE task_handle;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  RTOS_ASSERT_CRITICAL((USBD_HID_OS_Ptr != DEF_NULL), RTOS_ERR_NOT_INIT,; );

  task_handle = USBD_HID_OS_Ptr->TmrTaskHandle;
  CORE_EXIT_ATOMIC();

  KAL_TaskPrioSet(task_handle,
                  prio,
                  p_err);
}

/****************************************************************************************************//**
 *                                           USBD_HID_OS_TmrTask()
 *
 * @brief    OS-dependent shell task to process periodic HID input reports.
 *
 * @param    p_arg   Pointer to task initialization argument.
 *
 * @note     (1) Assumes tick rate frequency is greater than or equal to 250 Hz.  Otherwise, timer task
 *               scheduling rate will NOT be correct.
 *
 * @note     (2) Timer task MUST delay without failure.
 *               - (a) Failure to delay timer task will prevent some HID report task(s)/operation(s) from
 *                     functioning correctly. Thus, timer task is assumed to be successfully delayed
 *                     since NO error handling could be performed to counteract failure.
 *******************************************************************************************************/
static void USBD_HID_OS_TmrTask(void *p_arg)
{
  KAL_TICK dly_tick;

  (void)&p_arg;

  dly_tick = KAL_TickRateGet() / 250u;                          // Dly task at 4 ms rate (see Note #1).

  while (DEF_ON) {
    KAL_DlyTick(dly_tick,
                KAL_OPT_DLY_PERIODIC);

    USBD_HID_Report_TmrTaskHandler();
  }
}

/****************************************************************************************************//**
 *                                           USBD_HID_OS_InputLock()
 *
 * @brief    Lock class input report.
 *
 * @param    class_nbr   Class instance number.
 *
 * @param    ---------   Argument validated by the caller(s).
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void USBD_HID_OS_InputLock(CPU_INT08U class_nbr,
                           RTOS_ERR   *p_err)
{
  KAL_LockAcquire(USBD_HID_OS_Ptr->InputLockHandleTbl[class_nbr],
                  KAL_OPT_PEND_BLOCKING,
                  KAL_TIMEOUT_INFINITE,
                  p_err);
}

/****************************************************************************************************//**
 *                                           USBD_HID_OS_InputUnlock()
 *
 * @brief    Unlock class input report.
 *
 * @param    class_nbr   Class instance number.
 *
 * @param    ---------   Argument validated by the caller(s).
 *******************************************************************************************************/
void USBD_HID_OS_InputUnlock(CPU_INT08U class_nbr)
{
  RTOS_ERR err;

  KAL_LockRelease(USBD_HID_OS_Ptr->InputLockHandleTbl[class_nbr],
                  &err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), RTOS_ERR_OS,; );
}

/****************************************************************************************************//**
 *                                       USBD_HID_OS_OutputDataPendAbort()
 *
 * @brief    Abort class output report.
 *
 * @param    class_nbr   Class instance number.
 *
 * @param    ---------   Argument validated by the caller(s).
 *******************************************************************************************************/
void USBD_HID_OS_OutputDataPendAbort(CPU_INT08U class_nbr)
{
  RTOS_ERR err;

  KAL_SemPendAbort(USBD_HID_OS_Ptr->OutputDataSemHandleTbl[class_nbr],
                   &err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), RTOS_ERR_OS,; );
}

/****************************************************************************************************//**
 *                                       USBD_HID_OS_OutputDataPend()
 *
 * @brief    Wait for output report data transfer to complete.
 *
 * @param    class_nbr   Class instance number.
 *
 * @param    ---------   Argument validated by the caller(s).
 *
 * @param    timeout_ms  Signal wait timeout, in milliseconds.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void USBD_HID_OS_OutputDataPend(CPU_INT08U class_nbr,
                                CPU_INT16U timeout_ms,
                                RTOS_ERR   *p_err)
{
  KAL_SemPend(USBD_HID_OS_Ptr->OutputDataSemHandleTbl[class_nbr],
              KAL_OPT_PEND_BLOCKING,
              timeout_ms,
              p_err);
}

/****************************************************************************************************//**
 *                                       USBD_HID_OS_OutputDataPost()
 *
 * @brief    Signal that output report data is available.
 *
 * @param    class_nbr   Class instance number.
 *
 * @param    ---------   Argument validated by the caller(s).
 *******************************************************************************************************/
void USBD_HID_OS_OutputDataPost(CPU_INT08U class_nbr)
{
  RTOS_ERR err;

  KAL_SemPost(USBD_HID_OS_Ptr->OutputDataSemHandleTbl[class_nbr],
              KAL_OPT_POST_NONE,
              &err);
  PP_UNUSED_PARAM(err);
}

/****************************************************************************************************//**
 *                                           USBD_HID_OS_OutputLock()
 *
 * @brief    Lock class output report.
 *
 * @param    class_nbr   Class instance number.
 *
 * @param    ---------   Argument validated by the caller(s).
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void USBD_HID_OS_OutputLock(CPU_INT08U class_nbr,
                            RTOS_ERR   *p_err)
{
  KAL_LockAcquire(USBD_HID_OS_Ptr->OutputLockHandleTbl[class_nbr],
                  KAL_OPT_PEND_BLOCKING,
                  KAL_TIMEOUT_INFINITE,
                  p_err);
}

/****************************************************************************************************//**
 *                                       USBD_HID_OS_OutputUnlock()
 *
 * @brief    Unlock class output report.
 *
 * @param    class_nbr   Class instance number.
 *
 * @param    ---------   Argument validated by the caller(s).
 *******************************************************************************************************/
void USBD_HID_OS_OutputUnlock(CPU_INT08U class_nbr)
{
  RTOS_ERR err;

  KAL_LockRelease(USBD_HID_OS_Ptr->OutputLockHandleTbl[class_nbr],
                  &err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), RTOS_ERR_OS,; );
}

/****************************************************************************************************//**
 *                                           USBD_HID_OS_TxLock()
 *
 * @brief    Lock class transmit.
 *
 * @param    class_nbr   Class instance number.
 *
 * @param    ---------   Argument validated by the caller(s).
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void USBD_HID_OS_TxLock(CPU_INT08U class_nbr,
                        RTOS_ERR   *p_err)
{
  KAL_LockAcquire(USBD_HID_OS_Ptr->TxLockHandleTbl[class_nbr],
                  KAL_OPT_PEND_BLOCKING,
                  KAL_TIMEOUT_INFINITE,
                  p_err);
}

/****************************************************************************************************//**
 *                                           USBD_HID_OS_TxUnlock()
 *
 * @brief    Unlock class transmit.
 *
 * @param    class_nbr   Class instance number.
 *
 * @param    ---------   Argument validated by the caller(s).
 *******************************************************************************************************/
void USBD_HID_OS_TxUnlock(CPU_INT08U class_nbr)
{
  RTOS_ERR err;

  KAL_LockRelease(USBD_HID_OS_Ptr->TxLockHandleTbl[class_nbr],
                  &err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), RTOS_ERR_OS,; );
}

/****************************************************************************************************//**
 *                                       USBD_HID_OS_InputDataPend()
 *
 * @brief    Wait for input report data transfer to complete.
 *
 * @param    class_nbr   Class instance number.
 *
 * @param    ---------   Argument validated by the caller(s).
 *
 * @param    timeout_ms  Signal wait timeout in milliseconds.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void USBD_HID_OS_InputDataPend(CPU_INT08U class_nbr,
                               CPU_INT16U timeout_ms,
                               RTOS_ERR   *p_err)
{
  KAL_SemPend(USBD_HID_OS_Ptr->InputDataSemHandleTbl[class_nbr],
              KAL_OPT_PEND_BLOCKING,
              timeout_ms,
              p_err);
}

/****************************************************************************************************//**
 *                                       USBD_HID_OS_InputDataPendAbort()
 *
 * @brief    Abort any operation on input report.
 *
 * @param    class_nbr   Class instance number.
 *
 * @param    ---------   Argument validated by the caller(s).
 *******************************************************************************************************/
void USBD_HID_OS_InputDataPendAbort(CPU_INT08U class_nbr)
{
  RTOS_ERR err;

  KAL_SemPendAbort(USBD_HID_OS_Ptr->InputDataSemHandleTbl[class_nbr],
                   &err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), RTOS_ERR_OS,; );
}

/****************************************************************************************************//**
 *                                       USBD_HID_OS_InputDataPost()
 *
 * @brief    Signal that input report data transfer has completed.
 *
 * @param    class_nbr   Class instance number.
 *
 * @param    ---------   Argument validated by the caller(s).
 *******************************************************************************************************/
void USBD_HID_OS_InputDataPost(CPU_INT08U class_nbr)
{
  RTOS_ERR err;

  KAL_SemPost(USBD_HID_OS_Ptr->InputDataSemHandleTbl[class_nbr],
              KAL_OPT_POST_NONE,
              &err);
  PP_UNUSED_PARAM(err);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_USB_DEV_HID_AVAIL))
