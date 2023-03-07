/***************************************************************************//**
 * @file
 * @brief Kernel - Monitor Management
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
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_KERNEL_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  MICRIUM_SOURCE
#include "../include/os.h"
#include "os_priv.h"

#include  <em_core.h>

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const CPU_CHAR *os_mon__c = "$Id: $";
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

#if (OS_CFG_MON_EN == DEF_ENABLED)
/****************************************************************************************************//**
 *                                               OSMonCreate()
 *
 * @brief    Creates a monitor.
 *
 * @param    p_mon       Pointer to the monitor to initialize. Your application is responsible for
 *                       allocating storage space for the monitor.
 *
 * @param    p_name      Pointer to the name to assign to this monitor.
 *
 * @param    p_mon_data  Pointer to the monitor's global data.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *******************************************************************************************************/
void OSMonCreate(OS_MON   *p_mon,
                 CPU_CHAR *p_name,
                 void     *p_mon_data,
                 RTOS_ERR *p_err)
{
  CORE_DECLARE_IRQ_STATE;

  OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

#ifdef OS_SAFETY_CRITICAL_IEC61508
  if (OSSafetyCriticalStartFlag == DEF_TRUE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_OS_ILLEGAL_RUN_TIME);
    return;
  }
#endif

  //                                                               Not allowed to call from an ISR
  OS_ASSERT_DBG_ERR_SET((!CORE_InIrqContext()), *p_err, RTOS_ERR_ISR,; );

  //                                                               Validate 'p_mon'
  OS_ASSERT_DBG_ERR_SET((p_mon != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  CORE_ENTER_ATOMIC();

  *p_mon = (OS_MON){ 0 };
#if (OS_OBJ_TYPE_REQ == DEF_ENABLED)
  p_mon->Type = OS_OBJ_TYPE_MON;                                // Mark the data structure as a monitor
#endif

#if (OS_CFG_DBG_EN == DEF_ENABLED)
  p_mon->NamePtr = p_name;                                      // Save the name of the monitor
#else
  (void)&p_name;
#endif

  OS_PendListInit(&p_mon->PendList);

#if (OS_CFG_DBG_EN == DEF_ENABLED)
  OS_MonDbgListAdd(p_mon);
  OSMonQty++;
#endif

  if (p_mon_data != DEF_NULL) {
    p_mon->MonDataPtr = p_mon_data;
  }

  CORE_EXIT_ATOMIC();

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                               OSMonDel()
 *
 * @brief    This function deletes a monitor.
 *
 * @param    p_mon   Pointer to the monitor to delete.
 *
 * @param    opt     Determines delete options as follows:
 *                       - OS_OPT_DEL_NO_PEND  Deletes the monitor ONLY if there are no tasks pending.
 *                       - OS_OPT_DEL_ALWAYS   Deletes the monitor even if there are tasks waiting.
 *                   In this case, all pending tasks will be ready.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s)
 *                   from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                       - RTOS_ERR_OS_TASK_WAITING
 *                       - RTOS_ERR_NOT_READY
 *
 * @return   == 0    If there were no tasks waiting on the monitor, or upon error.
 *           >  0    If one or more tasks are waiting on the monitor are now ready and informed.
 *
 * @note     (1) Use this function with care. Tasks that would normally expect the presence of
 *               the monitor MUST check the return code of OSMonOp().
 *
 * @note     (2) Because ALL tasks pending on the monitor will be ready, be careful in applications
 *               where the monitor is used for mutual exclusion because the resource(s) will no longer
 *               be guarded by the monitor.
 *******************************************************************************************************/
OS_OBJ_QTY OSMonDel(OS_MON   *p_mon,
                    OS_OPT   opt,
                    RTOS_ERR *p_err)
{
  OS_OBJ_QTY   nbr_tasks;
  OS_PEND_LIST *p_pend_list;
  OS_TCB       *p_tcb;
  CORE_DECLARE_IRQ_STATE;

  OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

#ifdef OS_SAFETY_CRITICAL_IEC61508
  if (OSSafetyCriticalStartFlag == DEF_TRUE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_OS_ILLEGAL_RUN_TIME);
    return (0u);
  }
#endif

  //                                                               Not allowed to call from an ISR
  OS_ASSERT_DBG_ERR_SET((!CORE_InIrqContext()), *p_err, RTOS_ERR_ISR, 0u);

  //                                                               Make sure kernel is running.
  if (OSRunning != OS_STATE_OS_RUNNING) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
    return (0u);
  }

  //                                                               Validate 'p_mon'
  OS_ASSERT_DBG_ERR_SET((p_mon != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, 0u);

  //                                                               Validate object type
  OS_ASSERT_DBG_ERR_SET((p_mon->Type == OS_OBJ_TYPE_MON), *p_err, RTOS_ERR_INVALID_TYPE, 0u);

  CORE_ENTER_ATOMIC();
  p_pend_list = &p_mon->PendList;
  nbr_tasks = 0u;
  switch (opt) {
    case OS_OPT_DEL_NO_PEND:                                    // Delete monitor only if no task waiting
      if (p_pend_list->HeadPtr == DEF_NULL) {
 #if (OS_CFG_DBG_EN == DEF_ENABLED)
        OS_MonDbgListRemove(p_mon);
        OSMonQty--;
 #endif
        OS_MonClr(p_mon);
        CORE_EXIT_ATOMIC();
        RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      } else {
        CORE_EXIT_ATOMIC();
        RTOS_ERR_SET(*p_err, RTOS_ERR_OS_TASK_WAITING);
      }
      break;

    case OS_OPT_DEL_ALWAYS:                                     // Always delete the monitor
      while (p_pend_list->HeadPtr != DEF_NULL) {                // Remove all tasks on the pend list
        p_tcb = p_pend_list->HeadPtr;
        OS_PendAbort(p_tcb,
                     0,
                     OS_STATUS_PEND_DEL);
        nbr_tasks++;
      }
 #if (OS_CFG_DBG_EN == DEF_ENABLED)
      OS_MonDbgListRemove(p_mon);
      OSMonQty--;
 #endif
      OS_MonClr(p_mon);
      CORE_EXIT_ATOMIC();
      OSSched();                                                // Find highest priority task ready to run
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      break;

    default:
      CORE_EXIT_ATOMIC();
      OS_ASSERT_DBG_FAIL_EXEC(*p_err, RTOS_ERR_INVALID_ARG, 0u);
  }
  return (nbr_tasks);
}

/****************************************************************************************************//**
 *                                                   OSMonOp()
 *
 * @brief    Performs an operation on a monitor.
 *
 * @param    p_mon       Pointer to the monitor.
 *
 * @param    timeout     Optional timeout to be applied if the monitor blocks (pending).
 *
 * @param    p_arg       Argument of the monitor.
 *
 * @param    p_on_enter  Callback called at the entry of OSMonOp().
 *
 * @param    p_on_eval   Callback to be registered as the monitor's evaluation function.
 *
 * @param    opt         Possible option :
 *                           - OS_OPT_POST_NO_SCHED    Do not call the scheduler.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_OS_OBJ_DEL
 *                           - RTOS_ERR_ABORT
 *                           - RTOS_ERR_TIMEOUT
 *                           - RTOS_ERR_NOT_READY
 *                           - RTOS_ERR_INVALID_STATE
 *******************************************************************************************************/
void OSMonOp(OS_MON              *p_mon,
             OS_TICK             timeout,
             void                *p_arg,
             OS_MON_ON_ENTER_PTR p_on_enter,
             OS_MON_ON_EVAL_PTR  p_on_eval,
             OS_OPT              opt,
             RTOS_ERR            *p_err)
{
  CPU_INT32U   op_res;
  CPU_INT32U   mon_res;
  OS_PEND_LIST *p_pend_list;
  OS_TCB       *p_tcb;
  OS_TCB       *p_tcb_next;
  void         *p_eval_data;
  CPU_BOOLEAN  sched;
  CORE_DECLARE_IRQ_STATE;

  OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  //                                                               Make sure kernel is running.
  if (OSRunning != OS_STATE_OS_RUNNING) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
    return;
  }

  //                                                               Validate 'p_mon'
  OS_ASSERT_DBG_ERR_SET((p_mon != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  //                                                               Validate object type
  OS_ASSERT_DBG_ERR_SET((p_mon->Type == OS_OBJ_TYPE_MON), *p_err, RTOS_ERR_INVALID_TYPE,; );

  //                                                               Not allowed in atomic/critical sections
  OS_ASSERT_DBG_ERR_SET((!CORE_IrqIsDisabled()), *p_err, RTOS_ERR_INVALID_STATE,; );

  sched = DEF_NO;

  CORE_ENTER_ATOMIC();

  if (p_on_enter != DEF_NULL) {
    op_res = (*p_on_enter)(p_mon, p_arg);
  } else {
    op_res = OS_MON_RES_BLOCK | OS_MON_RES_STOP_EVAL;
  }

  if (DEF_BIT_IS_SET(op_res, OS_MON_RES_BLOCK) == DEF_YES) {
    OS_Pend((OS_PEND_OBJ *)(p_mon),                             // Block task pending on Condition Variable
            OS_TASK_PEND_ON_COND_VAR,
            timeout);

    sched = DEF_YES;
  }

  OSTCBCurPtr->MonData.p_eval_data = p_arg;
  OSTCBCurPtr->MonData.p_on_eval = p_on_eval;

  if (DEF_BIT_IS_CLR(op_res, OS_MON_RES_STOP_EVAL) == DEF_YES) {
    p_pend_list = &p_mon->PendList;
    if (p_pend_list->HeadPtr != DEF_NULL) {
      p_tcb = p_pend_list->HeadPtr;
      while (p_tcb != DEF_NULL) {
        p_tcb_next = p_tcb->PendNextPtr;

        p_on_eval = p_tcb->MonData.p_on_eval;
        p_eval_data = p_tcb->MonData.p_eval_data;

        if (p_on_eval != DEF_NULL) {
          mon_res = (*p_on_eval)(p_mon, p_eval_data, p_arg);
        } else {
          mon_res = OS_MON_RES_STOP_EVAL;
        }

        if (DEF_BIT_IS_CLR(mon_res, OS_MON_RES_BLOCK) == DEF_YES) {
          OS_Post((OS_PEND_OBJ *)(p_mon), p_tcb, DEF_NULL, 0u, 0u);
          if (DEF_BIT_IS_CLR(opt, OS_OPT_POST_NO_SCHED) == DEF_YES) {
            sched = DEF_YES;
          }
        }

        if (DEF_BIT_IS_SET(mon_res, OS_MON_RES_STOP_EVAL) == DEF_YES) {
          break;
        }

        p_tcb = p_tcb_next;
      }
    }
  }

  CORE_EXIT_ATOMIC();

  if (sched == DEF_YES) {
    OSSched();                                                  // Find the next highest priority task ready to run
  }

  if (DEF_BIT_IS_SET(op_res, OS_MON_RES_BLOCK) == DEF_YES) {
    OS_STATUS status;

    CORE_ENTER_ATOMIC();
    status = OSTCBCurPtr->PendStatus;
    CORE_EXIT_ATOMIC();

    switch (status) {
      case OS_STATUS_PEND_OK:                                   // We got the monitor
        RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
        break;

      case OS_STATUS_PEND_ABORT:                                // Indicate that we aborted
        RTOS_ERR_SET(*p_err, RTOS_ERR_ABORT);
        break;

      case OS_STATUS_PEND_TIMEOUT:                              // Indicate that we didn't get monitor within timeout
        RTOS_ERR_SET(*p_err, RTOS_ERR_TIMEOUT);
        break;

      case OS_STATUS_PEND_DEL:                                  // Indicate that object pended on has been deleted
        RTOS_ERR_SET(*p_err, RTOS_ERR_OS_OBJ_DEL);
        break;

      default:
        RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_OS,; );
    }
  } else {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               OS_MonClr()
 *
 * @brief    This function is called by OSMonDel() to clear the contents of the monitor.
 *
 * @param    p_mon   Pointer to the monitor to clear.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
void OS_MonClr(OS_MON *p_mon)
{
#if (OS_OBJ_TYPE_REQ == DEF_ENABLED)
  p_mon->Type = OS_OBJ_TYPE_NONE;                               // Mark the data structure as a NONE
#endif
  p_mon->MonDataPtr = DEF_NULL;
#if (OS_CFG_DBG_EN == DEF_ENABLED)
  p_mon->NamePtr = (CPU_CHAR *)((void *)"?MON");
#endif
  OS_PendListInit(&p_mon->PendList);                            // Initialize the waiting list
}

/****************************************************************************************************//**
 *                                           OS_MonDbgListAdd()
 *
 * @brief    Add a monitor to the monitor debug list.
 *
 * @param    p_mon   Pointer to the monitor to add.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/

#if (OS_CFG_DBG_EN == DEF_ENABLED)
void OS_MonDbgListAdd(OS_MON *p_mon)
{
  p_mon->DbgNamePtr = (CPU_CHAR *)((void *)" ");
  p_mon->DbgPrevPtr = DEF_NULL;
  if (OSMonDbgListPtr == DEF_NULL) {
    p_mon->DbgNextPtr = DEF_NULL;
  } else {
    p_mon->DbgNextPtr = OSMonDbgListPtr;
    OSMonDbgListPtr->DbgPrevPtr = p_mon;
  }
  OSMonDbgListPtr = p_mon;
}

/****************************************************************************************************//**
 *                                           OS_MonDbgListRemove()
 *
 * @brief    Remove a monitor from the monitor debug list.
 *
 * @param    p_mon   Pointer to the monitor to remove.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
void OS_MonDbgListRemove(OS_MON *p_mon)
{
  OS_MON *p_mon_next;
  OS_MON *p_mon_prev;

  p_mon_prev = p_mon->DbgPrevPtr;
  p_mon_next = p_mon->DbgNextPtr;

  if (p_mon_prev == DEF_NULL) {
    OSMonDbgListPtr = p_mon_next;
    if (p_mon_next != DEF_NULL) {
      p_mon_next->DbgPrevPtr = DEF_NULL;
    }
    p_mon->DbgNextPtr = DEF_NULL;
  } else if (p_mon_next == DEF_NULL) {
    p_mon_prev->DbgNextPtr = DEF_NULL;
    p_mon->DbgPrevPtr = DEF_NULL;
  } else {
    p_mon_prev->DbgNextPtr = p_mon_next;
    p_mon_next->DbgPrevPtr = p_mon_prev;
    p_mon->DbgNextPtr = DEF_NULL;
    p_mon->DbgPrevPtr = DEF_NULL;
  }
}
#endif
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_KERNEL_AVAIL))
