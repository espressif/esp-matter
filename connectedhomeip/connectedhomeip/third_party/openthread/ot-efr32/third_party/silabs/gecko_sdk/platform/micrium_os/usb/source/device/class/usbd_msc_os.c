/***************************************************************************//**
 * @file
 * @brief USB Device Operating System Layer - Micrium Kal
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

#if (defined(RTOS_MODULE_USB_DEV_MSC_AVAIL))

#if (!defined(RTOS_MODULE_USB_DEV_AVAIL))

#error USB Device MSC class requires USB Device Core. Make sure it is part of your project and that \
  RTOS_MODULE_USB_DEV_AVAIL is defined in rtos_description.h.

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#define    MICRIUM_SOURCE
#include  <em_core.h>

#include  <common/source/kal/kal_priv.h>

#include  <usb/include/device/usbd_msc.h>
#include  <usb/source/device/class/usbd_msc_priv.h>

#include  <common/include/rtos_err.h>
#include  <common/source/rtos/rtos_utils_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                               (USBD, CLASS, MSC, OS)
#define  RTOS_MODULE_CUR                            RTOS_CFG_MODULE_USBD

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef struct usbd_msc_os {
  KAL_TASK_HANDLE *TaskHandleTbl;                               // Table of task handle.
  KAL_SEM_HANDLE  *SemHandleTbl;                                // Table of semaphore.
  KAL_SEM_HANDLE  *EnumSignalTbl;                               // Table of enum complete signals.
} USBD_MSC_OS;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static USBD_MSC_OS *USBD_MSC_OS_Ptr = DEF_NULL;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void USBD_MSC_OS_Task(void *p_arg);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           USBD_MSC_OS_Init()
 *
 * @brief    Initialize MSC OS interface.
 *
 * @param    p_qty_cfg   Pointer to MSC class configuration structure.
 *
 * @param    p_mem_seg   Pointer to memory segment to use for data allocation.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void USBD_MSC_OS_Init(const USBD_MSC_QTY_CFG *p_qty_cfg,
                      MEM_SEG                *p_mem_seg,
                      RTOS_ERR               *p_err)
{
  CPU_BOOLEAN is_avail;
  CPU_INT08U  class_nbr;

  USBD_MSC_OS_Ptr = (USBD_MSC_OS *)Mem_SegAlloc("USBD - MSC OS root struct",
                                                p_mem_seg,
                                                sizeof(USBD_MSC_OS),
                                                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               Make sure every feature needed is avail.
  is_avail = KAL_FeatureQuery(KAL_FEATURE_TASK_CREATE, KAL_OPT_CREATE_NONE);
  is_avail &= KAL_FeatureQuery(KAL_FEATURE_SEM_CREATE, KAL_OPT_CREATE_NONE);
  is_avail &= KAL_FeatureQuery(KAL_FEATURE_SEM_PEND, KAL_OPT_PEND_BLOCKING);
  is_avail &= KAL_FeatureQuery(KAL_FEATURE_SEM_POST, KAL_OPT_POST_NONE);
  is_avail &= KAL_FeatureQuery(KAL_FEATURE_SEM_DEL, KAL_OPT_ABORT_NONE);
  if (is_avail != DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_AVAIL);
    return;
  }

  USBD_MSC_OS_Ptr->TaskHandleTbl = (KAL_TASK_HANDLE *)Mem_SegAlloc("USBD - MSC task handle tbl",
                                                                   p_mem_seg,
                                                                   sizeof(KAL_TASK_HANDLE) * p_qty_cfg->ClassInstanceQty,
                                                                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBD_MSC_OS_Ptr->SemHandleTbl = (KAL_SEM_HANDLE *)Mem_SegAlloc("USBD - MSC Sem handle tbl",
                                                                 p_mem_seg,
                                                                 sizeof(KAL_SEM_HANDLE) * p_qty_cfg->ClassInstanceQty,
                                                                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBD_MSC_OS_Ptr->EnumSignalTbl = (KAL_SEM_HANDLE *)Mem_SegAlloc("USBD - MSC Connect sem tbl",
                                                                  p_mem_seg,
                                                                  sizeof(KAL_SEM_HANDLE) * p_qty_cfg->ClassInstanceQty,
                                                                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  for (class_nbr = 0u; class_nbr < p_qty_cfg->ClassInstanceQty; class_nbr++) {
    USBD_MSC_OS_Ptr->TaskHandleTbl[class_nbr] = KAL_TaskHandleNull;

    //                                                             Create sem for signal used for MSC comm.
    USBD_MSC_OS_Ptr->SemHandleTbl[class_nbr] = KAL_SemCreate("USBD - MSC Task Sem",
                                                             DEF_NULL,
                                                             p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    //                                                             Create sem for signal used for MSC enum.
    USBD_MSC_OS_Ptr->EnumSignalTbl[class_nbr] = KAL_SemCreate("USBD - MSC Connect Sem",
                                                              DEF_NULL,
                                                              p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
  }
}

/****************************************************************************************************//**
 *                                           USBD_MSC_OS_TaskCreate()
 *
 * @brief    Initialize MSC OS interface.
 *
 * @param    p_msc_task_cfg  Pointer to structure that contains configurations for msc task.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this
 *                           function.
 *******************************************************************************************************/
void USBD_MSC_OS_TaskCreate(CPU_INT08U    class_nbr,
                            RTOS_TASK_CFG *p_msc_task_cfg,
                            RTOS_ERR      *p_err)
{
  void            *p_arg = (void *)(CPU_ADDR)class_nbr;
  KAL_TASK_HANDLE task_handle;
  CORE_DECLARE_IRQ_STATE;

  task_handle = KAL_TaskAlloc("USBD MSC Task",
                              p_msc_task_cfg->StkPtr,
                              p_msc_task_cfg->StkSizeElements,
                              DEF_NULL,
                              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  KAL_TaskCreate(task_handle,
                 USBD_MSC_OS_Task,
                 p_arg,
                 p_msc_task_cfg->Prio,
                 DEF_NULL,
                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  CORE_ENTER_ATOMIC();
  USBD_MSC_OS_Ptr->TaskHandleTbl[class_nbr] = task_handle;
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                           USBD_MSC_OS_TaskPrioSet()
 *
 * @brief    Sets priority of the given MSC instance task.
 *
 * @param    class_nbr   MSC instance number.
 *
 * @param    prio        Priority of the MSC instance task.
 *
 * @param    p_err       Pointer to error variable.
 *******************************************************************************************************/
void USBD_MSC_OS_TaskPrioSet(CPU_INT08U     class_nbr,
                             RTOS_TASK_PRIO prio,
                             RTOS_ERR       *p_err)
{
  KAL_TASK_HANDLE task_handle;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  task_handle = USBD_MSC_OS_Ptr->TaskHandleTbl[class_nbr];
  CORE_EXIT_ATOMIC();

  KAL_TaskPrioSet(task_handle,
                  prio,
                  p_err);
}

/****************************************************************************************************//**
 *                                       USBD_MSC_OS_CommSignalPost()
 *
 * @brief    Post a semaphore used for MSC communication.
 *
 * @param    class_nbr   MSC instance class number
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void USBD_MSC_OS_CommSignalPost(CPU_INT08U class_nbr,
                                RTOS_ERR   *p_err)
{
  KAL_SemPost(USBD_MSC_OS_Ptr->SemHandleTbl[class_nbr],
              KAL_OPT_POST_NONE,
              p_err);
}

/****************************************************************************************************//**
 *                                       USBD_MSC_OS_CommSignalPend()
 *
 * @brief    Wait on a semaphore to become available for MSC communication.
 *
 * @param    class_nbr   MSC instance class number
 *
 * @param    timeout_ms  Timeout in milliseconds.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void USBD_MSC_OS_CommSignalPend(CPU_INT08U class_nbr,
                                CPU_INT32U timeout_ms,
                                RTOS_ERR   *p_err)
{
  KAL_SemPend(USBD_MSC_OS_Ptr->SemHandleTbl[class_nbr],
              KAL_OPT_PEND_BLOCKING,
              timeout_ms,
              p_err);
}

/****************************************************************************************************//**
 *                                       USBD_MSC_OS_CommSignalDel()
 *
 * @brief    Delete a semaphore if no tasks are waiting on it for MSC communication.
 *
 * @param    class_nbr   MSC instance class number
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
#if 0
void USBD_MSC_OS_CommSignalDel(CPU_INT08U class_nbr)
{
  KAL_SemDel(USBD_MSC_OS_Ptr->SemHandleTbl[class_nbr]);
}
#endif

/****************************************************************************************************//**
 *                                       USBD_MSC_OS_EnumSignalPost()
 *
 * @brief    Post a semaphore for MSC enumeration process.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void USBD_MSC_OS_EnumSignalPost(CPU_INT08U class_nbr,
                                RTOS_ERR   *p_err)
{
  KAL_SemPost(USBD_MSC_OS_Ptr->EnumSignalTbl[class_nbr],
              KAL_OPT_POST_NONE,
              p_err);
}

/****************************************************************************************************//**
 *                                       USBD_MSC_OS_EnumSignalPend()
 *
 * @brief    Wait on a semaphore to become available for MSC enumeration process.
 *
 * @param    timeout_ms  Timeout in milliseconds.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void USBD_MSC_OS_EnumSignalPend(CPU_INT08U class_nbr,
                                CPU_INT32U timeout_ms,
                                RTOS_ERR   *p_err)
{
  KAL_SemPend(USBD_MSC_OS_Ptr->EnumSignalTbl[class_nbr],
              KAL_OPT_PEND_BLOCKING,
              timeout_ms,
              p_err);
}

/****************************************************************************************************//**
 *                                           USBD_MSC_OS_Task()
 *
 * @brief    OS-dependent shell task to process MSC task
 *
 * @param    p_arg   Pointer to task initialization argument (required by uC/OS-II).
 *******************************************************************************************************/
static void USBD_MSC_OS_Task(void *p_arg)
{
  CPU_INT08U class_nbr = (CPU_INT08U)(CPU_ADDR)p_arg;

  USBD_MSC_TaskHandler(class_nbr);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_USB_DEV_MSC_AVAIL))
