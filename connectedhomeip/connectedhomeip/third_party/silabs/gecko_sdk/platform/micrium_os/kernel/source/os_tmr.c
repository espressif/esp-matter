/***************************************************************************//**
 * @file
 * @brief Kernel - Timer Management
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
const CPU_CHAR *os_tmr__c = "$Id: $";
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (OS_CFG_TMR_EN == DEF_ENABLED)

static void OS_TmrLock(void);
static void OS_TmrUnlock(void);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               OSTmrCreate()
 *
 * @brief    Called by your application code to create a timer.
 *
 * @param    p_tmr           Pointer to the timer to create. Your application is responsible
 *                           for allocating storage for the timer.
 *
 * @param    p_name          Pointer to an ASCII string that names the timer (useful for debugging)
 *
 * @param    dly             Initial delay.
 *                           If the timer is configured for ONE-SHOT mode, this is the timeout used.
 *                           If the timer is configured for PERIODIC mode, this is the first timeout
 *                           to wait for before the timer starts entering periodic mode.
 *
 * @param    period          The 'period' being repeated for the timer.
 *                           If you specified 'OS_OPT_TMR_PERIODIC' as an option, when the timer
 *                           expires, it will automatically restart with the same period.
 *
 * @param    opt             Specifies either:
 *                               - OS_OPT_TMR_ONE_SHOT     The timer counts down only once.
 *                               - OS_OPT_TMR_PERIODIC     The timer counts down and then reloads itself.
 *
 * @param    p_callback      Pointer to a callback function that will be called when the timer
 *                           expires. The callback function must be declared as follows:
 *                           @verbatim
 *                           void  MyCallback (OS_TMR  *p_tmr, void  *p_arg);
 *                           @endverbatim
 *                           @param    p_callback_arg  Pointer to an argument that is passed to the callback function when it is
 *                           called.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error code(s)
 *                           from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *
 * @note     (1) This function only creates the timer. In other words, the timer is not started when
 *               created. To start the timer, call OSTmrStart().
 *******************************************************************************************************/
void OSTmrCreate(OS_TMR              *p_tmr,
                 CPU_CHAR            *p_name,
                 OS_TICK             dly,
                 OS_TICK             period,
                 OS_OPT              opt,
                 OS_TMR_CALLBACK_PTR p_callback,
                 void                *p_callback_arg,
                 RTOS_ERR            *p_err)
{
  OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

#ifdef OS_SAFETY_CRITICAL_IEC61508
  if (OSSafetyCriticalStartFlag == DEF_TRUE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_OS_ILLEGAL_RUN_TIME);
    return;
  }
#endif

  //                                                               Not allowed to call from an ISR
  OS_ASSERT_DBG_ERR_SET((!CORE_InIrqContext()), *p_err, RTOS_ERR_ISR,; );

  //                                                               Validate 'p_tmr'
  OS_ASSERT_DBG_ERR_SET((p_tmr != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

#if (OS_ARG_CHK_EN == DEF_ENABLED)
  switch (opt) {                                                // Validate 'opt'
    case OS_OPT_TMR_PERIODIC:
      OS_ASSERT_DBG_ERR_SET((period > 0u), *p_err, RTOS_ERR_INVALID_ARG,; );

      //                                                           No point in a periodic timer without a callback
      OS_ASSERT_DBG_ERR_SET((p_callback != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
      break;

    case OS_OPT_TMR_ONE_SHOT:
      OS_ASSERT_DBG_ERR_SET((dly > 0u), *p_err, RTOS_ERR_INVALID_ARG,; );
      break;

    default:
      OS_ASSERT_DBG_FAIL_EXEC(*p_err, RTOS_ERR_INVALID_ARG,; );
  }
#endif

  if (OSRunning == OS_STATE_OS_RUNNING) {                       // Only lock when the kernel is running
    OS_TmrLock();
  }

  *p_tmr = (OS_TMR){ 0 };                                       // Initialize the timer fields
  p_tmr->State = OS_TMR_STATE_STOPPED;
#if (OS_OBJ_TYPE_REQ == DEF_ENABLED)
  p_tmr->Type = OS_OBJ_TYPE_TMR;
#endif
#if (OS_CFG_DBG_EN == DEF_ENABLED)
  p_tmr->NamePtr = p_name;
#else
  (void)&p_name;
#endif
  p_tmr->Dly = dly;
  p_tmr->Period = period;
  p_tmr->Opt = opt;
  p_tmr->CallbackPtr = p_callback;
  p_tmr->CallbackPtrArg = p_callback_arg;

#if (OS_CFG_DBG_EN == DEF_ENABLED)
  OS_TmrDbgListAdd(p_tmr);
#endif
#if (OS_CFG_DBG_EN == DEF_ENABLED)
  OSTmrQty++;                                                   // Keep track of the number of timers created
#endif

  if (OSRunning == OS_STATE_OS_RUNNING) {
    OS_TmrUnlock();
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/*****************************************************************************************************//**
 *                                               OSTmrDel()
 *
 * @brief    Called by your application code to delete a timer.
 *
 * @param    p_tmr   Pointer to the timer to stop and delete.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s)
 *                   from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                       - RTOS_ERR_NOT_INIT
 *                       - RTOS_ERR_NOT_READY
 *
 * @return   == DEF_TRUE     If the timer was deleted.
 *           == DEF_FALSE    If the timer was not deleted or upon an error.
 *******************************************************************************************************/
CPU_BOOLEAN OSTmrDel(OS_TMR   *p_tmr,
                     RTOS_ERR *p_err)
{
  CPU_BOOLEAN success = DEF_FALSE;

  OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_FALSE);

#ifdef OS_SAFETY_CRITICAL_IEC61508
  if (OSSafetyCriticalStartFlag == DEF_TRUE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_OS_ILLEGAL_RUN_TIME);
    return (DEF_FALSE);
  }
#endif

  //                                                               Not allowed to call from an ISR
  OS_ASSERT_DBG_ERR_SET((!CORE_InIrqContext()), *p_err, RTOS_ERR_ISR, DEF_FALSE);

  //                                                               Validate 'p_tmr'
  OS_ASSERT_DBG_ERR_SET((p_tmr != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_FALSE);

  //                                                               Validate object type
  OS_ASSERT_DBG_ERR_SET((p_tmr->Type == OS_OBJ_TYPE_TMR), *p_err, RTOS_ERR_INVALID_TYPE, DEF_FALSE);

  //                                                               Make sure kernel is running.
  if (OSRunning != OS_STATE_OS_RUNNING) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
    return (DEF_FALSE);
  }

  OS_TmrLock();

#if (OS_CFG_DBG_EN == DEF_ENABLED)
  OS_TmrDbgListRemove(p_tmr);
#endif

  switch (p_tmr->State) {
    case OS_TMR_STATE_RUNNING:
      OS_TmrUnlink(p_tmr);                                      // Remove from the list
      OS_TmrClr(p_tmr);
#if (OS_CFG_DBG_EN == DEF_ENABLED)
      OSTmrQty--;                                               // One less timer
#endif
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      success = DEF_TRUE;
      break;

    case OS_TMR_STATE_STOPPED:                                  // Timer has not started or ...
    case OS_TMR_STATE_COMPLETED:                                // ... timer has completed the ONE-SHOT time
      OS_TmrClr(p_tmr);                                         // Clear timer fields
#if (OS_CFG_DBG_EN == DEF_ENABLED)
      OSTmrQty--;                                               // One less timer
#endif
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      success = DEF_TRUE;
      break;

    case OS_TMR_STATE_UNUSED:                                   // Already deleted
      RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_INIT);
      success = DEF_FALSE;
      break;

    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_OS, DEF_FALSE);
      break;
  }

  OS_TmrUnlock();

  return (success);
}

/*****************************************************************************************************//**
 *                                               OSTmrRemainGet()
 *
 * @brief    Called to get the number of ticks before a timer times out.
 *
 * @param    p_tmr   Pointer to the timer to obtain the remaining time from.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s)
 *                   from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_NOT_INIT
 *                       - RTOS_ERR_NOT_READY
 *
 * @return   The time remaining for the timer to expire. The time represents 'timer' increments. In
 *           other words, if OS_TmrTask() is signaled every 1/10 of a second, the returned value
 *           represents the number of 1/10 of a second remaining before the timer expires.
 *******************************************************************************************************/
OS_TICK OSTmrRemainGet(OS_TMR   *p_tmr,
                       RTOS_ERR *p_err)
{
  OS_TICK remain;

  OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  //                                                               Not allowed to call from an ISR
  OS_ASSERT_DBG_ERR_SET((!CORE_InIrqContext()), *p_err, RTOS_ERR_ISR, 0u);

  //                                                               Validate 'p_tmr'
  OS_ASSERT_DBG_ERR_SET((p_tmr != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, 0u);

  //                                                               Validate object type
  OS_ASSERT_DBG_ERR_SET((p_tmr->Type == OS_OBJ_TYPE_TMR), *p_err, RTOS_ERR_INVALID_TYPE, 0u);

  //                                                               Make sure kernel is running.
  if (OSRunning != OS_STATE_OS_RUNNING) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
    return (0u);
  }

  OS_TmrLock();
  remain = 0u;
  switch (p_tmr->State) {
    case OS_TMR_STATE_RUNNING:
      remain = p_tmr->Remain;
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      break;

    case OS_TMR_STATE_STOPPED:                                  // It's assumed that the timer has not started yet
      if (p_tmr->Opt == OS_OPT_TMR_PERIODIC) {
        if (p_tmr->Dly == 0u) {
          remain = p_tmr->Period;
        } else {
          remain = p_tmr->Dly;
        }
      } else {
        remain = p_tmr->Dly;
      }
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      break;

    case OS_TMR_STATE_COMPLETED:                                // Only ONE-SHOT that timed out can be in this state
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      break;

    case OS_TMR_STATE_UNUSED:
      RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_INIT);
      break;

    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_OS, 0u);
      break;
  }

  OS_TmrUnlock();

  return (remain);
}

/*****************************************************************************************************//**
 *                                               OSTmrSet()
 *
 * @brief    Called by your application code to set a timer.
 *
 * @param    p_tmr           Pointer to the timer to set.
 *
 * @param    dly             Initial delay.
 *                           If the timer is configured for ONE-SHOT mode, this is the timeout used.
 *                           If the timer is configured for PERIODIC mode, this is the first timeout
 *                           to wait for before the timer starts entering periodic mode.
 *
 * @param    period          The 'period' being repeated for the timer.
 *                           If you specified 'OS_OPT_TMR_PERIODIC' as an option, when the timer
 *                           expires, it will automatically restart with the same period.
 *
 * @param    p_callback      Pointer to a callback function that will be called when the timer
 *                           expires. The callback function must be declared as follows:
 *                           void  MyCallback (OS_TMR  *p_tmr, void  *p_arg);
 *
 * @param    p_callback_arg  Pointer to an argument that is passed to the callback function when it is
 *                           called.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error code(s)
 *                           from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_READY
 *
 * @note     (1) This function can be called on a running timer. The change to the delay and period
 *               will only take effect after the current period or delay has passed. Change to the
 *               callback will take effect immediately.
 *******************************************************************************************************/
void OSTmrSet(OS_TMR              *p_tmr,
              OS_TICK             dly,
              OS_TICK             period,
              OS_TMR_CALLBACK_PTR p_callback,
              void                *p_callback_arg,
              RTOS_ERR            *p_err)
{
  OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  //                                                               Not allowed to call from an ISR
  OS_ASSERT_DBG_ERR_SET((!CORE_InIrqContext()), *p_err, RTOS_ERR_ISR,; );

  //                                                               Validate 'p_tmr'
  OS_ASSERT_DBG_ERR_SET((p_tmr != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  //                                                               Validate object type
  OS_ASSERT_DBG_ERR_SET((p_tmr->Type == OS_OBJ_TYPE_TMR), *p_err, RTOS_ERR_INVALID_TYPE,; );

  //                                                               Make sure kernel is running.
  if (OSRunning != OS_STATE_OS_RUNNING) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
    return;
  }

#if (OS_ARG_CHK_EN == DEF_ENABLED)
  switch (p_tmr->Opt) {
    case OS_OPT_TMR_PERIODIC:
      OS_ASSERT_DBG_ERR_SET((period > 0u), *p_err, RTOS_ERR_INVALID_ARG,; );

      // No point in a periodic timer without a callback
      OS_ASSERT_DBG_ERR_SET((p_callback != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
      break;

    case OS_OPT_TMR_ONE_SHOT:
      OS_ASSERT_DBG_ERR_SET((dly > 0u), *p_err, RTOS_ERR_INVALID_ARG,; );
      break;

    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_INVALID_ARG,; );
  }
#endif

  OS_TmrLock();

  p_tmr->Dly = dly;
  p_tmr->Period = period;
  p_tmr->CallbackPtr = p_callback;
  p_tmr->CallbackPtrArg = p_callback_arg;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  OS_TmrUnlock();
}

/*****************************************************************************************************//**
 *                                               OSTmrStart()
 *
 * @brief    Called by your application code to start a timer.
 *
 * @param    p_tmr   Pointer to the timer to start.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s) from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_NOT_INIT
 *                       - RTOS_ERR_NOT_READY
 *
 * @return   DEF_TRUE     If the timer was started.
 *           DEF_FALSE    If the timer was not started or upon an error.
 *
 * @note     (1) When starting/restarting a timer, regardless if it is in PERIODIC or ONE-SHOT mode,
 *               the timer is linked to the timer list with the OS_OPT_LINK_DLY option. This option
 *               sets the initial expiration time for the timer. For timers in PERIODIC mode,
 *               subsequent expiration times are handled by the OS_TmrTask().
 *******************************************************************************************************/
CPU_BOOLEAN OSTmrStart(OS_TMR   *p_tmr,
                       RTOS_ERR *p_err)
{
  OS_TMR      *p_next;
  CPU_BOOLEAN success;

  OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_FALSE);

  //                                                               Not allowed to call from an ISR
  OS_ASSERT_DBG_ERR_SET((!CORE_InIrqContext()), *p_err, RTOS_ERR_ISR, DEF_FALSE);

  //                                                               Validate 'p_tmr'
  OS_ASSERT_DBG_ERR_SET((p_tmr != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_FALSE);

  //                                                               Validate object type
  OS_ASSERT_DBG_ERR_SET((p_tmr->Type == OS_OBJ_TYPE_TMR), *p_err, RTOS_ERR_INVALID_TYPE, DEF_FALSE);

  //                                                               Make sure kernel is running.
  if (OSRunning != OS_STATE_OS_RUNNING) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
    return (DEF_FALSE);
  }

  OS_TmrLock();
  success = DEF_FALSE;
  switch (p_tmr->State) {
    case OS_TMR_STATE_RUNNING:                                  // Restart the timer
      if (p_tmr->Dly == 0u) {
        p_tmr->Remain = p_tmr->Period;
      } else {
        p_tmr->Remain = p_tmr->Dly;
      }
      success = DEF_TRUE;
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      break;

    case OS_TMR_STATE_STOPPED:                                  // Start the timer
    case OS_TMR_STATE_COMPLETED:
      p_tmr->State = OS_TMR_STATE_RUNNING;
      if (p_tmr->Dly == 0u) {
        p_tmr->Remain = p_tmr->Period;
      } else {
        p_tmr->Remain = p_tmr->Dly;
      }
      if (OSTmrListPtr == DEF_NULL) {                           // Link into timer list
        p_tmr->NextPtr = DEF_NULL;                              // This is the first timer in the list
        p_tmr->PrevPtr = DEF_NULL;
        OSTmrListPtr = p_tmr;
#if (OS_CFG_DBG_EN == DEF_ENABLED)
        OSTmrListEntries = 1u;
#endif
      } else {
        p_next = OSTmrListPtr;                                  // Insert at the beginning of the list
        p_tmr->NextPtr = OSTmrListPtr;
        p_tmr->PrevPtr = DEF_NULL;
        p_next->PrevPtr = p_tmr;
        OSTmrListPtr = p_tmr;
#if (OS_CFG_DBG_EN == DEF_ENABLED)
        OSTmrListEntries++;
#endif
      }
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      success = DEF_TRUE;
      break;

    case OS_TMR_STATE_UNUSED:                                   // Timer not created
      RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_INIT);
      success = DEF_FALSE;
      break;

    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_OS, DEF_FALSE);
  }

  OS_TmrUnlock();

  return (success);
}

/*****************************************************************************************************//**
 *                                               OSTmrStateGet()
 *
 * @brief    Called to determine what state the timer is in:
 *           - OS_TMR_STATE_UNUSED     The timer has not been created.
 *           - OS_TMR_STATE_STOPPED    The timer has been created but has not been started or has
 *                                     been stopped.
 *           - OS_TMR_STATE_COMPLETED  The timer is in ONE-SHOT mode and has completed it's timeout.
 *           - OS_TMR_SATE_RUNNING     The timer is currently running.
 *
 * @param    p_tmr   Pointer to the timer.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s)
 *                   from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_NOT_READY
 *
 * @return   The current state of the timer (see description).
 *******************************************************************************************************/
OS_STATE OSTmrStateGet(OS_TMR   *p_tmr,
                       RTOS_ERR *p_err)
{
  OS_STATE state;

  OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, OS_TMR_STATE_UNUSED);

  //                                                               Not allowed to call from an ISR
  OS_ASSERT_DBG_ERR_SET((!CORE_InIrqContext()), *p_err, RTOS_ERR_ISR, OS_TMR_STATE_UNUSED);

  //                                                               Validate 'p_tmr'
  OS_ASSERT_DBG_ERR_SET((p_tmr != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, OS_TMR_STATE_UNUSED);

  //                                                               Validate object type
  OS_ASSERT_DBG_ERR_SET((p_tmr->Type == OS_OBJ_TYPE_TMR), *p_err, RTOS_ERR_INVALID_TYPE, OS_TMR_STATE_UNUSED);

  //                                                               Make sure kernel is running.
  if (OSRunning != OS_STATE_OS_RUNNING) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
    return (OS_TMR_STATE_UNUSED);
  }

  OS_TmrLock();

  state = p_tmr->State;
  switch (state) {
    case OS_TMR_STATE_UNUSED:
    case OS_TMR_STATE_STOPPED:
    case OS_TMR_STATE_COMPLETED:
    case OS_TMR_STATE_RUNNING:
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      break;

    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_OS, 0u);
      break;
  }

  OS_TmrUnlock();

  return (state);
}

/*****************************************************************************************************//**
 *                                               OSTmrStop()
 *
 * @brief    Called by your application code to stop a timer.
 *
 * @param    p_tmr           Pointer to the timer to stop.
 *
 * @param    opt             Allows you to specify an option to this functions which can be:
 *                               - OS_OPT_TMR_NONE             Stop the timer.
 *                               - OS_OPT_TMR_CALLBACK         Stop the timer and execute the callback
 *                           function, pass it the callback argument
 *                           specified when the timer was created.
 *                               - OS_OPT_TMR_CALLBACK_ARG     Stop the timer and execute the callback
 *                           function, pass it the callback argument
 *                           specified in THIS function call.
 *
 * @param    p_callback_arg  Pointer to a 'new' callback argument that can be passed to the
 *                           callback function instead of the timer's callback argument. In other words,
 *                           use 'callback_arg' passed in THIS function INSTEAD of p_tmr->OSTmrCallbackArg
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error code(s)
 *                           from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_INIT
 *                               - RTOS_ERR_INVALID_ARG
 *                               - RTOS_ERR_INVALID_STATE
 *                               - RTOS_ERR_NOT_READY
 *
 * @return   DEF_TRUE     If we stop the timer (if the timer is already stopped, it returns as
 *                       DEF_TRUE).
 *           DEF_FALSE    If the timer is not stopped.
 *******************************************************************************************************/
CPU_BOOLEAN OSTmrStop(OS_TMR   *p_tmr,
                      OS_OPT   opt,
                      void     *p_callback_arg,
                      RTOS_ERR *p_err)
{
  OS_TMR_CALLBACK_PTR p_fnct;
  CPU_BOOLEAN         success;

  OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_FALSE);             // Validate 'p_err'

  //                                                               Not allowed to call from an ISR
  OS_ASSERT_DBG_ERR_SET((!CORE_InIrqContext()), *p_err, RTOS_ERR_ISR, DEF_FALSE);

  //                                                               Validate 'p_tmr'
  OS_ASSERT_DBG_ERR_SET((p_tmr != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_FALSE);

  //                                                               Validate object type
  OS_ASSERT_DBG_ERR_SET((p_tmr->Type == OS_OBJ_TYPE_TMR), *p_err, RTOS_ERR_INVALID_TYPE, DEF_FALSE);

  //                                                               Make sure kernel is running.
  if (OSRunning != OS_STATE_OS_RUNNING) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
    return (DEF_FALSE);
  }

  OS_TmrLock();
  success = DEF_FALSE;
  switch (p_tmr->State) {
    case OS_TMR_STATE_RUNNING:
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      switch (opt) {
        case OS_OPT_TMR_CALLBACK:
          OS_TmrUnlink(p_tmr);                                  // Remove from timer list
          p_fnct = p_tmr->CallbackPtr;                          // Execute callback function ...
          if (p_fnct != DEF_NULL) {                             // ... if available
            (*p_fnct)((void *)p_tmr, p_tmr->CallbackPtrArg);    // Use callback arg when timer was created
          } else {
            RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
          }
          break;

        case OS_OPT_TMR_CALLBACK_ARG:
          OS_TmrUnlink(p_tmr);                                  // Remove from timer list
          p_fnct = p_tmr->CallbackPtr;                          // Execute callback function if available ...
          if (p_fnct != DEF_NULL) {
            (*p_fnct)((void *)p_tmr, p_callback_arg);           // .. using the 'callback_arg' provided in call
          } else {
            RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
          }
          break;

        case OS_OPT_TMR_NONE:
          OS_TmrUnlink(p_tmr);                                  // Remove from timer list
          break;

        default:
          OS_TmrUnlock();
          OS_ASSERT_DBG_FAIL_EXEC(*p_err, RTOS_ERR_INVALID_ARG, DEF_FALSE);
      }
      success = DEF_TRUE;
      break;

    case OS_TMR_STATE_COMPLETED:                                // Timer has already completed the ONE-SHOT or
    case OS_TMR_STATE_STOPPED:                                  // ... timer has not started yet.
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
      success = DEF_TRUE;
      break;

    case OS_TMR_STATE_UNUSED:                                   // Timer was not created
      RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_INIT);
      success = DEF_FALSE;
      break;

    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_OS, DEF_FALSE);
  }

  OS_TmrUnlock();

  return (success);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/*****************************************************************************************************//**
 *                                               OS_TmrClr()
 *
 * @brief    Called to clear all timer fields.
 *
 * @param    p_tmr   Pointer to the timer to clear.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
void OS_TmrClr(OS_TMR *p_tmr)
{
  p_tmr->State = OS_TMR_STATE_UNUSED;                           // Clear timer fields
#if (OS_OBJ_TYPE_REQ == DEF_ENABLED)
  p_tmr->Type = OS_OBJ_TYPE_NONE;
#endif
#if (OS_CFG_DBG_EN == DEF_ENABLED)
  p_tmr->NamePtr = (CPU_CHAR *)((void *)"?TMR");
#endif
  p_tmr->Dly = 0u;
  p_tmr->Remain = 0u;
  p_tmr->Period = 0u;
  p_tmr->Opt = 0u;
  p_tmr->CallbackPtr = DEF_NULL;
  p_tmr->CallbackPtrArg = DEF_NULL;
  p_tmr->NextPtr = DEF_NULL;
  p_tmr->PrevPtr = DEF_NULL;
}

/*****************************************************************************************************//**
 *                                           OS_TmrDbgListAdd()
 *
 * @brief    Add a timer to timer debug list.
 *
 * @param    p_tmr   Pointer to the timer to add.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
#if (OS_CFG_DBG_EN == DEF_ENABLED)
void OS_TmrDbgListAdd(OS_TMR *p_tmr)
{
  p_tmr->DbgPrevPtr = DEF_NULL;
  if (OSTmrDbgListPtr == DEF_NULL) {
    p_tmr->DbgNextPtr = DEF_NULL;
  } else {
    p_tmr->DbgNextPtr = OSTmrDbgListPtr;
    OSTmrDbgListPtr->DbgPrevPtr = p_tmr;
  }
  OSTmrDbgListPtr = p_tmr;
}

/*****************************************************************************************************//**
 *                                           OS_TmrDbgListRemove()
 *
 * @brief    Remove a timer to timer debug list.
 *
 * @param    p_tmr   Pointer to the timer to remove.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
void OS_TmrDbgListRemove(OS_TMR *p_tmr)
{
  OS_TMR *p_tmr_next;
  OS_TMR *p_tmr_prev;

  p_tmr_prev = p_tmr->DbgPrevPtr;
  p_tmr_next = p_tmr->DbgNextPtr;

  if (p_tmr_prev == DEF_NULL) {
    OSTmrDbgListPtr = p_tmr_next;
    if (p_tmr_next != DEF_NULL) {
      p_tmr_next->DbgPrevPtr = DEF_NULL;
    }
    p_tmr->DbgNextPtr = DEF_NULL;
  } else if (p_tmr_next == DEF_NULL) {
    p_tmr_prev->DbgNextPtr = DEF_NULL;
    p_tmr->DbgPrevPtr = DEF_NULL;
  } else {
    p_tmr_prev->DbgNextPtr = p_tmr_next;
    p_tmr_next->DbgPrevPtr = p_tmr_prev;
    p_tmr->DbgNextPtr = DEF_NULL;
    p_tmr->DbgPrevPtr = DEF_NULL;
  }
}
#endif

/*****************************************************************************************************//**
 *                                               OS_TmrInit()
 *
 * @brief    Called by OSInit() to initialize the timer manager module.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s) from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
void OS_TmrInit(RTOS_ERR *p_err)
{
  OS_RATE_HZ tick_rate;

#if (OS_CFG_DBG_EN == DEF_ENABLED)
  OSTmrDbgListPtr = DEF_NULL;
#endif

  OSTmrListPtr = DEF_NULL;                                      // Create an empty timer list
#if (OS_CFG_DBG_EN == DEF_ENABLED)
  OSTmrListEntries = 0u;
#endif

  tick_rate = OSTimeTickRateHzGet(p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if (OSCfg_TmrTaskRate_Hz > 0u) {
    OSTmrUpdateCnt = tick_rate / OSCfg_TmrTaskRate_Hz;
  } else {
    OSTmrUpdateCnt = tick_rate / 10u;
  }
  OSTmrUpdateCtr = OSTmrUpdateCnt;

  OSTmrTickCtr = 0u;

#if (OS_CFG_TS_EN == DEF_ENABLED)
  OSTmrTaskTimeMax = 0u;
#endif

#if (OS_CFG_MUTEX_EN == DEF_ENABLED)
  OSMutexCreate(&OSTmrMutex,                                    // Use a mutex to protect the timers
                (CPU_CHAR *)"OS Tmr Mutex",
                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
#endif

  //                                                               -------------- CREATE THE TIMER TASK ---------------
  OSTaskCreate(&OSTmrTaskTCB,
               (CPU_CHAR *)((void *)"Kernel's Timer Task"),
               OS_TmrTask,
               DEF_NULL,
               OSCfg_TmrTaskPrio,
               OSCfg_TmrTaskStkBasePtr,
               OSCfg_TmrTaskStkLimit,
               OSCfg_TmrTaskStkSize,
               0u,
               0u,
               DEF_NULL,
               (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_NO_TLS),
               p_err);
}

/*****************************************************************************************************//**
 *                                               OS_TmrUnlink()
 *
 * @brief    Called to remove the timer from the timer list.
 *
 * @param    p_tmr   Pointer to the timer to remove.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
void OS_TmrUnlink(OS_TMR *p_tmr)
{
  OS_TMR *p_tmr1;
  OS_TMR *p_tmr2;

  if (OSTmrListPtr == p_tmr) {                                  // See if timer to remove is at the beginning of list
    p_tmr1 = p_tmr->NextPtr;
    OSTmrListPtr = p_tmr1;
    if (p_tmr1 != DEF_NULL) {
      p_tmr1->PrevPtr = DEF_NULL;
    }
  } else {
    p_tmr1 = p_tmr->PrevPtr;                                    // Remove timer from somewhere in the list
    p_tmr2 = p_tmr->NextPtr;
    p_tmr1->NextPtr = p_tmr2;
    if (p_tmr2 != DEF_NULL) {
      p_tmr2->PrevPtr = p_tmr1;
    }
  }
  p_tmr->State = OS_TMR_STATE_STOPPED;
  p_tmr->NextPtr = DEF_NULL;
  p_tmr->PrevPtr = DEF_NULL;
#if (OS_CFG_DBG_EN == DEF_ENABLED)
  OSTmrListEntries--;
#endif
}

/*****************************************************************************************************//**
 *                                               OS_TmrTask()
 *
 * @brief    This task is created by OS_TmrInit().
 *
 * @param    p_arg   Argument passed to the task when the task is created (unused).
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
void OS_TmrTask(void *p_arg)
{
  RTOS_ERR            err;
  OS_TMR_CALLBACK_PTR p_fnct;
  OS_TMR              *p_tmr;
  OS_TMR              *p_tmr_next;
#if (OS_CFG_TS_EN == DEF_ENABLED)
  CPU_TS ts_start;
  CPU_TS ts_delta;
#endif

  (void)p_arg;                                                  // Not using 'p_arg', prevent compiler warning
  while (DEF_ON) {
    OSTimeDly(OSTmrUpdateCnt, OS_OPT_TIME_DLY, &err);
    (void)err;

    OS_TmrLock();
#if (OS_CFG_TS_EN == DEF_ENABLED)
    ts_start = OS_TS_GET();
#endif
    OSTmrTickCtr++;                                             // Increment the current time
    p_tmr = OSTmrListPtr;
    while (p_tmr != DEF_NULL) {                                 // Update all the timers in the list
      OSSchedLock(&err);
      (void)&err;
      p_tmr_next = p_tmr->NextPtr;
      p_tmr->Remain--;
      if (p_tmr->Remain == 0u) {
        if (p_tmr->Opt == OS_OPT_TMR_PERIODIC) {
          p_tmr->Remain = p_tmr->Period;                        // Reload the time remaining
        } else {
          OS_TmrUnlink(p_tmr);                                  // Remove from list
          p_tmr->State = OS_TMR_STATE_COMPLETED;                // Indicate that the timer has completed
        }
        p_fnct = p_tmr->CallbackPtr;                            // Execute callback function if available
        if (p_fnct != 0u) {
          (*p_fnct)((void *)p_tmr,
                    p_tmr->CallbackPtrArg);
        }
      }
      p_tmr = p_tmr_next;
      OSSchedUnlock(&err);
      (void)&err;
    }

#if (OS_CFG_TS_EN == DEF_ENABLED)
    ts_delta = OS_TS_GET() - ts_start;                          // Measure execution time of timer task
    if (OSTmrTaskTimeMax < ts_delta) {
      OSTmrTaskTimeMax = ts_delta;
    }
#endif

    OS_TmrUnlock();
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/*****************************************************************************************************//**
 *                                               OS_TmrLock()
 *
 * @brief    This function is used to lock the timer processing code. The preferred method is to use
 *           a mutex in order to avoid locking the scheduler and also, to avoid calling callback
 *           functions while the scheduler is locked.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
static void OS_TmrLock(void)
{
  RTOS_ERR err;
#if (OS_CFG_MUTEX_EN == DEF_ENABLED)
  CPU_TS ts;

  OSMutexPend(&OSTmrMutex,                                      // Use a mutex to protect the timers
              0u,
              OS_OPT_PEND_BLOCKING,
              &ts,
              &err);
#else
  OSSchedLock(&err);                                            // Lock the scheduler to protect the timers
#endif
  (void)&err;
}

/*****************************************************************************************************//**
 *                                               OS_TmrUnlock()
 *
 * @brief    This function is used to unlock the timer processing code. The preferred method is to use
 *           a mutex in order to avoid locking the scheduler and also, to avoid calling callback
 *           functions while the scheduler is locked.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
static void OS_TmrUnlock(void)
{
  RTOS_ERR err;

#if (OS_CFG_MUTEX_EN == DEF_ENABLED)
  OSMutexPost(&OSTmrMutex,                                      // Use a mutex to protect the timers
              OS_OPT_POST_NONE,
              &err);
#else
  OSSchedUnlock(&err);                                          // Lock the scheduler to protect the timers
#endif
  (void)&err;
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_KERNEL_AVAIL))
