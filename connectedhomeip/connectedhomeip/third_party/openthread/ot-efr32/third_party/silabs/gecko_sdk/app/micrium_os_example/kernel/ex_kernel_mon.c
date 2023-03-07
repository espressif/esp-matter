/***************************************************************************//**
 * @file
 * @brief Kernel Monitor Example
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
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <common/include/common.h>
#include  <kernel/include/os.h>

#include  <common/include/lib_def.h>
#include  <common/include/rtos_utils.h>
#include  <common/include/toolchains.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                  EXAMPLE CONFIGURATION REQUIREMENTS
 ********************************************************************************************************
 *******************************************************************************************************/

#if (OS_CFG_MON_EN != DEF_ENABLED)
#error "This example requires OS_CFG_MON_EN to be set to DEF_ENABLED"
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                               LOGGING
 *
 * Note(s) : (1) This example outputs information to the console via the function printf() via a macro
 *               called EX_TRACE(). This can be modified or disabled if printf() is not supported.
 *******************************************************************************************************/

#ifndef  EX_TRACE
#include  <stdio.h>
#define  EX_TRACE(...)                                      printf(__VA_ARGS__)
#endif

/********************************************************************************************************
 *                                         TASK'S CONFIGURATION
 *******************************************************************************************************/

//                                                                 FSM Task Configuration.
#define  EX_KERNEL_MON_FSM_TASK_STK_SIZE                 128u
#define  EX_KERNEL_MON_FSM_TASK_PRIO                      25u

//                                                                 State 1 Task Configuration.
#define  EX_KERNEL_MON_STATE1_TASK_STK_SIZE              128u
#define  EX_KERNEL_MON_STATE1_TASK_PRIO                   22u

//                                                                 State 2 Task Configuration.
#define  EX_KERNEL_MON_STATE2_TASK_STK_SIZE              128u
#define  EX_KERNEL_MON_STATE2_TASK_PRIO                   23u

//                                                                 State 3 Task Configuration.
#define  EX_KERNEL_MON_STATE3_TASK_STK_SIZE              128u
#define  EX_KERNEL_MON_STATE3_TASK_PRIO                   24u

/********************************************************************************************************
 *                                       EXAMPLE MONITOR SETTINGS
 *******************************************************************************************************/

//                                                                 Number of pend/posts before deletion.
#define  EX_KERNEL_MON_MAX_COUNT                           3u

//                                                                 Pend on State 1.
#define  EX_KERNEL_MON_PEND_ON_STATE1                   1337u
//                                                                 Pend on State 2.
#define  EX_KERNEL_MON_PEND_ON_STATE2                     42u
//                                                                 Pend on State 3.
#define  EX_KERNEL_MON_PEND_ON_STATE3               0xC0FFEEu

/********************************************************************************************************
 ********************************************************************************************************
 *                                        LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 FSM Task Data.
static CPU_STK Ex_KernelMonFSMTaskStk[EX_KERNEL_MON_FSM_TASK_STK_SIZE];
static OS_TCB  Ex_KernelMonFSMTaskTCB;

//                                                                 State 1 Task Data.
static CPU_STK Ex_KernelMonState1TaskStk[EX_KERNEL_MON_STATE1_TASK_STK_SIZE];
static OS_TCB  Ex_KernelMonState1TaskTCB;

//                                                                 State 2 Task Data.
static CPU_STK Ex_KernelMonState2TaskStk[EX_KERNEL_MON_STATE2_TASK_STK_SIZE];
static OS_TCB  Ex_KernelMonState2TaskTCB;

//                                                                 State 3 Task Data.
static CPU_STK Ex_KernelMonState3TaskStk[EX_KERNEL_MON_STATE3_TASK_STK_SIZE];
static OS_TCB  Ex_KernelMonState3TaskTCB;

//                                                                 Monitor.
static OS_MON Ex_KernelMonObj;

//                                                                 Example State.
static CPU_INT32U Ex_State;

//                                                                 Example execution counter.
static CPU_INT08U Ex_Done;

/********************************************************************************************************
 ********************************************************************************************************
 *                                      LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/
//                                                                 Task Prototypes.
static void Ex_KernelMonFSMTask(void *p_arg);
static void Ex_KernelMonState1Task(void *p_arg);
static void Ex_KernelMonState2Task(void *p_arg);
static void Ex_KernelMonState3Task(void *p_arg);

//                                                                 Monitor Eval (Pend).
static OS_MON_RES Ex_KernelMonPend(OS_MON *p_mon,
                                   void   *p_eval_data,
                                   void   *p_arg);

//                                                                 Monitor Enter (Post).
static OS_MON_RES Ex_KernelMonPost(OS_MON *p_mon,
                                   void   *p_arg);

/********************************************************************************************************
 ********************************************************************************************************
 *                                          LOCAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                            Ex_KernelMon()
 *
 * @brief  Illustrates the usage of Kernel monitors.
 *******************************************************************************************************/
void Ex_KernelMon(void)
{
  RTOS_ERR err;

  //                                                               Initialize Example.
  Ex_Done = 0u;

  OSTaskCreate(&Ex_KernelMonFSMTaskTCB,                         // Create FSM Task.
               "Ex Kernel Monitor FSM Task",
               Ex_KernelMonFSMTask,
               0,
               EX_KERNEL_MON_FSM_TASK_PRIO,
               &Ex_KernelMonFSMTaskStk[0],
               (EX_KERNEL_MON_FSM_TASK_STK_SIZE / 10u),
               EX_KERNEL_MON_FSM_TASK_STK_SIZE,
               0,
               0,
               0,
               (OS_OPT_TASK_STK_CLR),
               &err);
  //                                                               Check error code.
  APP_RTOS_ASSERT_DBG((err.Code == RTOS_ERR_NONE),; );

  //                                                               The FSM Task will execute.

  //                                                               Delay Task execution for
  OSTimeDly(5000,                                               // 5000 OS Ticks
            OS_OPT_TIME_DLY,                                    // from now.
            &err);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                        Ex_KernelMonFSMTask()
 *
 * @brief  This task simulates an FSM that changes state. Each state has a task attached to it.
 *
 * @param  p_arg  Unused parameter
 *******************************************************************************************************/
static void Ex_KernelMonFSMTask(void *p_arg)
{
  RTOS_ERR   err;
  OS_OBJ_QTY qty;

  //                                                               Prevent compiler warning.
  PP_UNUSED_PARAM(p_arg);

  //                                                               Create Monitor.
  OSMonCreate(&Ex_KernelMonObj,                                 // Pointer to example monitor.
              "App Monitor",                                    // Name used for debugging.
              (void *)&Ex_State,                                // Global monitor data: the State.
              &err);
  //                                                               Check error code.
  APP_RTOS_ASSERT_DBG((err.Code == RTOS_ERR_NONE),; );

  OSTaskCreate(&Ex_KernelMonState1TaskTCB,                      // Create State 1 Task.
               "Ex Kernel Monitor State 1 Task",
               Ex_KernelMonState1Task,
               0,
               EX_KERNEL_MON_STATE1_TASK_PRIO,
               &Ex_KernelMonState1TaskStk[0],
               (EX_KERNEL_MON_STATE1_TASK_STK_SIZE / 10u),
               EX_KERNEL_MON_STATE1_TASK_STK_SIZE,
               0,
               0,
               0,
               (OS_OPT_TASK_STK_CLR),
               &err);
  //                                                               Check error code.
  APP_RTOS_ASSERT_DBG((err.Code == RTOS_ERR_NONE),; );

  OSTaskCreate(&Ex_KernelMonState2TaskTCB,                      // Create State 2 Task.
               "Ex Kernel Monitor State 2 Task",
               Ex_KernelMonState2Task,
               0,
               EX_KERNEL_MON_STATE2_TASK_PRIO,
               &Ex_KernelMonState2TaskStk[0],
               (EX_KERNEL_MON_STATE2_TASK_STK_SIZE / 10u),
               EX_KERNEL_MON_STATE2_TASK_STK_SIZE,
               0,
               0,
               0,
               (OS_OPT_TASK_STK_CLR),
               &err);
  //                                                               Check error code.
  APP_RTOS_ASSERT_DBG((err.Code == RTOS_ERR_NONE),; );

  OSTaskCreate(&Ex_KernelMonState3TaskTCB,                      // Create State 3 Task.
               "Ex Kernel Monitor State 3 Task",
               Ex_KernelMonState3Task,
               0,
               EX_KERNEL_MON_STATE3_TASK_PRIO,
               &Ex_KernelMonState3TaskStk[0],
               (EX_KERNEL_MON_STATE3_TASK_STK_SIZE / 10u),
               EX_KERNEL_MON_STATE3_TASK_STK_SIZE,
               0,
               0,
               0,
               (OS_OPT_TASK_STK_CLR),
               &err);
  //                                                               Check error code.
  APP_RTOS_ASSERT_DBG((err.Code == RTOS_ERR_NONE),; );

  //                                                               All State Tasks have executed and are blocked.

  EX_TRACE("Example Kernel Monitor: Changing FSM from 0 to 3.\r\n");
  //                                                               Wake up State 3 Task.
  OSMonOp(&Ex_KernelMonObj,                                     // Pointer to the example monitor.
          0,                                                    // Timeout: none, this is a post operation.
          (void *)EX_KERNEL_MON_PEND_ON_STATE3,                 // Change state to new value.
          &Ex_KernelMonPost,                                    // Function used on entering the monitor.
          DEF_NULL,                                             // Function used for evaluation.
          0,                                                    // Option: none.
          &err);
  //                                                               Check error code.
  APP_RTOS_ASSERT_DBG((err.Code == RTOS_ERR_NONE),; );
  Ex_Done++;

  EX_TRACE("Example Kernel Monitor: Changing FSM from 3 to 1.\r\n");
  //                                                               Wake up State 1 Task.
  OSMonOp(&Ex_KernelMonObj,                                     // Pointer to the example monitor.
          0,                                                    // Timeout: none, this is a post operation.
          (void *)EX_KERNEL_MON_PEND_ON_STATE1,                 // Change state to new value.
          &Ex_KernelMonPost,                                    // Function used on entering the monitor.
          DEF_NULL,                                             // Function used for evaluation.
          0,                                                    // Option: none.
          &err);
  //                                                               Check error code.
  APP_RTOS_ASSERT_DBG((err.Code == RTOS_ERR_NONE),; );
  Ex_Done++;

  EX_TRACE("Example Kernel Monitor: Changing FSM from 1 to 2.\r\n");
  //                                                               Wake up State 2 Task.
  OSMonOp(&Ex_KernelMonObj,                                     // Pointer to the example monitor.
          0,                                                    // Timeout: none, this is a post operation.
          (void *)EX_KERNEL_MON_PEND_ON_STATE2,                 // Change state to new value.
          &Ex_KernelMonPost,                                    // Function used on entering the monitor.
          DEF_NULL,                                             // Function used for evaluation.
          0,                                                    // Option: none.
          &err);
  //                                                               Check error code.
  APP_RTOS_ASSERT_DBG((err.Code == RTOS_ERR_NONE),; );
  Ex_Done++;

  //                                                               All State Tasks have executed, delete the monitor.

  //                                                               Delete Monitor.
  qty = OSMonDel(&Ex_KernelMonObj,                              // Pointer to the example monitor.
                 OS_OPT_DEL_NO_PEND,                            // Option: delete if 0 tasks are pending.
                 &err);
  //                                                               Check error code.
  APP_RTOS_ASSERT_DBG((err.Code == RTOS_ERR_NONE),; );
  PP_UNUSED_PARAM(qty);

  while (DEF_ON) {
    //                                                             Delay Task execution for
    OSTimeDly(5000,                                             // 5000 OS Ticks
              OS_OPT_TIME_DLY,                                  // from now.
              &err);
    //                                                             Check error code.
    APP_RTOS_ASSERT_DBG((err.Code == RTOS_ERR_NONE),; );
  }
}

/****************************************************************************************************//**
 *                                       Ex_KernelMonState1Task()
 *
 * @brief  This task pends until the global state variable is equal to the required state.
 *
 * @param  p_arg  Unused parameter
 *******************************************************************************************************/
static void Ex_KernelMonState1Task(void *p_arg)
{
  RTOS_ERR err;

  //                                                               Prevent compiler warning.
  PP_UNUSED_PARAM(p_arg);

  //                                                               Task body.
  while (DEF_ON) {
    if (Ex_Done < EX_KERNEL_MON_MAX_COUNT) {
      //                                                           Wait for Value 1.
      OSMonOp(&Ex_KernelMonObj,                                 // Pointer to the example monitor.
              500,                                              // Timeout: 500 OS Ticks.
              (void *)EX_KERNEL_MON_PEND_ON_STATE1,             // Wait for this state.
              DEF_NULL,                                         // Function used on entering the monitor.
              &Ex_KernelMonPend,                                // Function used for evaluation.
              0,                                                // Option: none.
              &err);
      if (err.Code == RTOS_ERR_NONE) {                          // Check error code.
        EX_TRACE("Example Kernel Monitor: FSM is now in State 1.\r\n");
      } else {
        EX_TRACE("Example Kernel Monitor: error on monitor pend. OSMonOp() returned with %i\r\n",
                 err.Code);
        RTOS_ASSERT_CRITICAL_FAILED_END_CALL(0u);
      }
    }
    //                                                             Delay Task execution for
    OSTimeDly(5000,                                             // 5000 OS Ticks
              OS_OPT_TIME_DLY,                                  // from now.
              &err);
    //                                                             Check error code.
    APP_RTOS_ASSERT_DBG((err.Code == RTOS_ERR_NONE),; );
  }
}

/****************************************************************************************************//**
 *                                       Ex_KernelMonState2Task()
 *
 * @brief  This task pends until the global state variable is equal to the required state.
 *
 * @param  p_arg  Unused parameter
 *******************************************************************************************************/
static void Ex_KernelMonState2Task(void *p_arg)
{
  RTOS_ERR err;

  //                                                               Prevent compiler warning.
  PP_UNUSED_PARAM(p_arg);

  //                                                               Task body.
  while (DEF_ON) {
    if (Ex_Done < EX_KERNEL_MON_MAX_COUNT) {
      //                                                           Wait for Value 2.
      OSMonOp(&Ex_KernelMonObj,                                 // Pointer to the example monitor.
              500,                                              // Timeout: 500 OS Ticks.
              (void *)EX_KERNEL_MON_PEND_ON_STATE2,             // Wait for this state.
              DEF_NULL,                                         // Function used on entering the monitor.
              &Ex_KernelMonPend,                                // Function used for evaluation.
              0,                                                // Option: none.
              &err);
      if (err.Code == RTOS_ERR_NONE) {                          // Check error code.
        EX_TRACE("Example Kernel Monitor: FSM is now in State 2.\r\n");
      } else {
        EX_TRACE("Example Kernel Monitor: error on monitor pend. OSMonOp() returned with %i\r\n",
                 err.Code);
        RTOS_ASSERT_CRITICAL_FAILED_END_CALL(; );
      }
    }
    //                                                             Delay Task execution for
    OSTimeDly(5000,                                             // 5000 OS Ticks
              OS_OPT_TIME_DLY,                                  // from now.
              &err);
    //                                                             Check error code.
    APP_RTOS_ASSERT_DBG((err.Code == RTOS_ERR_NONE),; );
  }
}

/****************************************************************************************************//**
 *                                       Ex_KernelMonState3Task()
 *
 * @brief  This task pends until the global state variable is equal to the required state.
 *
 * @param  p_arg  Unused parameter
 *******************************************************************************************************/
static void Ex_KernelMonState3Task(void *p_arg)
{
  RTOS_ERR err;

  //                                                               Prevent compiler warning.
  PP_UNUSED_PARAM(p_arg);

  //                                                               Task body.
  while (DEF_ON) {
    if (Ex_Done < EX_KERNEL_MON_MAX_COUNT) {
      //                                                           Wait for Value 3.
      OSMonOp(&Ex_KernelMonObj,                                 // Pointer to the example monitor.
              500,                                              // Timeout: 500 OS Ticks.
              (void *)EX_KERNEL_MON_PEND_ON_STATE3,             // Wait for this state.
              DEF_NULL,                                         // Function used on entering the monitor.
              &Ex_KernelMonPend,                                // Function used for evaluation.
              0,                                                // Option: none.
              &err);
      if (err.Code == RTOS_ERR_NONE) {                          // Check error code.
        EX_TRACE("Example Kernel Monitor: FSM is now in State 3.\r\n");
      } else {
        EX_TRACE("Example Kernel Monitor: error on monitor pend. OSMonOp() returned with %i\r\n",
                 err.Code);
        RTOS_ASSERT_CRITICAL_FAILED_END_CALL(; );
      }
    }
    //                                                             Delay Task execution for
    OSTimeDly(5000,                                             // 5000 OS Ticks
              OS_OPT_TIME_DLY,                                  // from now.
              &err);
    //                                                             Check error code.
    APP_RTOS_ASSERT_DBG((err.Code == RTOS_ERR_NONE),; );
  }
}

/****************************************************************************************************//**
 *                                        Ex_KernelMonPend()
 *
 * @brief   This function will cause a task to pend on a monitor until its value matches the one
 *          required.
 *
 * @param  p_mon           Pointer to the example monitor.
 *
 * @param  p_eval_data     Pointer to the argument passed to OSMonOp by the task.
 *
 * @param  p_arg           Pointer to the argument passed to OSMonOp by another task.
 *******************************************************************************************************/
static OS_MON_RES Ex_KernelMonPend(OS_MON *p_mon,
                                   void   *p_eval_data,
                                   void   *p_arg)
{
  CPU_INT32U *p_state;
  OS_MON_RES res;

  //                                                               Prevent compiler warning.
  PP_UNUSED_PARAM(p_arg);

  //                                                               Get State variable.
  p_state = (CPU_INT32U *)p_mon->MonDataPtr;

  //                                                               If new state matches, wake up the pending task.
  if (*p_state == (CPU_INT32U)p_eval_data) {
    res = OS_MON_RES_ALLOW;
  } else {
    res = OS_MON_RES_BLOCK;
  }

  return (res);
}

/****************************************************************************************************//**
 *                                        Ex_KernelMonPost()
 *
 * @brief  This function will update the state of a monitor. The pending tasks will then re-evaluate
 *         the state.
 *
 * @param  p_mon   Pointer to the example monitor.
 *
 * @param  p_arg   Pointer to the argument passed to OSMonOp by the task.
 *******************************************************************************************************/
static OS_MON_RES Ex_KernelMonPost(OS_MON *p_mon,
                                   void   *p_arg)
{
  CPU_INT32U *p_state;

  //                                                               Get State variable.
  p_state = (CPU_INT32U *)p_mon->MonDataPtr;
  //                                                               Change state.
  *p_state = (CPU_INT32U)p_arg;

  return (OS_MON_RES_ALLOW);
}
