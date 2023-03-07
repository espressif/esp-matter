/***************************************************************************//**
 * @file
 * @brief Kernel Mutex Pend Abort Example
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

#if (OS_CFG_MUTEX_EN != DEF_ENABLED)
#error "This example requires OS_CFG_MUTEX_EN to be set to DEF_ENABLED"
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

//                                                                 PendA Task Configuration.
#define  EX_KERNEL_MUTEX_PENDA_TASK_STK_SIZE              128u
#define  EX_KERNEL_MUTEX_PENDA_TASK_PRIO                   22u

//                                                                 PendB Task Configuration.
#define  EX_KERNEL_MUTEX_PENDB_TASK_STK_SIZE              128u
#define  EX_KERNEL_MUTEX_PENDB_TASK_PRIO                   23u

/********************************************************************************************************
 *                                        EXAMPLE MUTEX SETTINGS
 *******************************************************************************************************/

#define  EX_KERNEL_MUTEX_MAX_COUNT                         1u

/********************************************************************************************************
 ********************************************************************************************************
 *                                        LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 PendA Task Data.
static CPU_STK Ex_KernelMutexPendATaskStk[EX_KERNEL_MUTEX_PENDA_TASK_STK_SIZE];
static OS_TCB  Ex_KernelMutexPendATaskTCB;

//                                                                 PendB Task Data.
static CPU_STK Ex_KernelMutexPendBTaskStk[EX_KERNEL_MUTEX_PENDB_TASK_STK_SIZE];
static OS_TCB  Ex_KernelMutexPendBTaskTCB;

//                                                                 Mutex.
static OS_MUTEX Ex_KernelMutexObj;

//                                                                 Example execution counter.
static CPU_INT08U Ex_Done;

/********************************************************************************************************
 ********************************************************************************************************
 *                                      LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void Ex_KernelMutexPendATask(void *p_arg);
static void Ex_KernelMutexPendBTask(void *p_arg);

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
 *                                       Ex_KernelMutexPendAbort()
 *
 * @brief  Illustrates the usage of the pend abort feature of Kernel mutexes.
 *******************************************************************************************************/
void Ex_KernelMutexPendAbort(void)
{
  RTOS_ERR   err;
  OS_OBJ_QTY qty;

  //                                                               Initialize Example.
  Ex_Done = 0u;

  //                                                               Create Mutex.
  OSMutexCreate(&Ex_KernelMutexObj,                             // Pointer to the example mutex.
                "Ex Kernel Mutex",                              // Name used for debugging.
                &err);
  //                                                               Check error code.
  APP_RTOS_ASSERT_DBG((err.Code == RTOS_ERR_NONE),; );

  OSTaskCreate(&Ex_KernelMutexPendATaskTCB,                     // Create PendA Task.
               "Ex Kernel Mutex PendA Task",
               Ex_KernelMutexPendATask,
               0,
               EX_KERNEL_MUTEX_PENDA_TASK_PRIO,
               &Ex_KernelMutexPendATaskStk[0],
               (EX_KERNEL_MUTEX_PENDA_TASK_STK_SIZE / 10u),
               EX_KERNEL_MUTEX_PENDA_TASK_STK_SIZE,
               0,
               0,
               0,
               (OS_OPT_TASK_STK_CLR),
               &err);
  //                                                               Check error code.
  APP_RTOS_ASSERT_DBG((err.Code == RTOS_ERR_NONE),; );

  OSTaskCreate(&Ex_KernelMutexPendBTaskTCB,                     // Create PendB Task.
               "Ex Kernel Mutex PendB Task",
               Ex_KernelMutexPendBTask,
               0,
               EX_KERNEL_MUTEX_PENDB_TASK_PRIO,
               &Ex_KernelMutexPendBTaskStk[0],
               (EX_KERNEL_MUTEX_PENDB_TASK_STK_SIZE / 10u),
               EX_KERNEL_MUTEX_PENDB_TASK_STK_SIZE,
               0,
               0,
               0,
               (OS_OPT_TASK_STK_CLR),
               &err);
  //                                                               Check error code.
  APP_RTOS_ASSERT_DBG((err.Code == RTOS_ERR_NONE),; );

  //                                                               The other tasks will block.

  //                                                               Acquire mutex.
  OSMutexPend(&Ex_KernelMutexObj,                               // Pointer to the example mutex.
              0,                                                // Wait for infinity.
              OS_OPT_PEND_BLOCKING,                             // Task will block.
              DEF_NULL,                                         // Timestamp is not used.
              &err);
  //                                                               Check error code.
  APP_RTOS_ASSERT_DBG((err.Code == RTOS_ERR_NONE),; );

  //                                                               Both Pend tasks will execute.

  //                                                               Delay Task execution for
  OSTimeDly(500,                                                // 500 OS Ticks
            OS_OPT_TIME_DLY,                                    // from now.
            &err);

  //                                                               Both pend tasks blocked, execution returns here.

  //                                                               Abort the pend on both tasks.
  qty = OSMutexPendAbort(&Ex_KernelMutexObj,                    // Pointer to the example mutex.
                         OS_OPT_PEND_ABORT_ALL,                 // Abort all waiting tasks.
                         &err);
  if (err.Code == RTOS_ERR_NONE) {                              // Check error code.
    if (qty == 2) {
      Ex_Done = 1u;
      EX_TRACE("Example Kernel Mutex Pend Abort: Ex_KernelMutexPendAbort successfully aborted both pends.\r\n");
    } else {
      EX_TRACE("Example Kernel Mutex Pend Abort: wrong number of tasks aborted. OSMutexPendAbort() aborted %u tasks\r\n",
               qty);
      RTOS_ASSERT_CRITICAL_FAILED_END_CALL(; );
    }
  } else {
    EX_TRACE("Example Kernel Mutex Pend Abort: error on pend abort. OSMutexPendAbort() returned with %i\r\n",
             err.Code);
    RTOS_ASSERT_CRITICAL_FAILED_END_CALL(; );
  }
  //                                                               Delete Mutex.
  qty = OSMutexDel(&Ex_KernelMutexObj,                          // Pointer to the example evnt flag group.
                   OS_OPT_DEL_NO_PEND,                          // Option: delete if 0 tasks are pending.
                   &err);
  //                                                               Check error code.
  APP_RTOS_ASSERT_DBG((err.Code == RTOS_ERR_NONE),; );
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       Ex_KernelMutexPendATask()
 *
 * @brief  This task pends on the mutex, it will be pend aborted.
 *
 * @param  p_arg  Unused parameter
 *******************************************************************************************************/
static void Ex_KernelMutexPendATask(void *p_arg)
{
  RTOS_ERR err;

  //                                                               Prevent compiler warning.
  PP_UNUSED_PARAM(p_arg);

  //                                                               Task body.
  while (DEF_ON) {
    if (Ex_Done < EX_KERNEL_MUTEX_MAX_COUNT) {
      EX_TRACE("Example Kernel Mutex Pend Abort: PendA Task pending on mutex.\r\n");
      //                                                           Acquire mutex.
      OSMutexPend(&Ex_KernelMutexObj,                           // Pointer to the example mutex.
                  0,                                            // Wait for infinity.
                  OS_OPT_PEND_BLOCKING,                         // Task will block.
                  DEF_NULL,                                     // Timestamp is not used.
                  &err);
      if (err.Code != RTOS_ERR_ABORT) {                         // Check error code.
        EX_TRACE("Example Kernel Mutex Pend Abort: error on pend abort. OSMutexPend() returned with %i\r\n",
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
 *                                       Ex_KernelMutexPendBTask()
 *
 * @brief  This task pends on the mutex, it will be pend aborted.
 *
 * @param  p_arg  Unused param
 *******************************************************************************************************/
static void Ex_KernelMutexPendBTask(void *p_arg)
{
  RTOS_ERR err;

  //                                                               Prevent compiler warning.
  PP_UNUSED_PARAM(p_arg);

  //                                                               Task body.
  while (DEF_ON) {
    if (Ex_Done < EX_KERNEL_MUTEX_MAX_COUNT) {
      EX_TRACE("Example Kernel Mutex Pend Abort: PendB Task pending on mutex.\r\n");
      //                                                           Acquire mutex.
      OSMutexPend(&Ex_KernelMutexObj,                           // Pointer to the example mutex.
                  0,                                            // Wait for infinity.
                  OS_OPT_PEND_BLOCKING,                         // Task will block.
                  DEF_NULL,                                     // Timestamp is not used.
                  &err);
      if (err.Code != RTOS_ERR_ABORT) {                         // Check error code.
        EX_TRACE("Example Kernel Mutex Pend Abort: error on pend abort. OSMutexPend() returned with %i\r\n",
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
