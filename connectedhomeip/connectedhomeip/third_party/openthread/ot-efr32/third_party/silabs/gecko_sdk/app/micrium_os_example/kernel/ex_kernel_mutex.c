/***************************************************************************//**
 * @file
 * @brief Kernel Mutex Example
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

//                                                                 Pend Task Configuration.
#define  EX_KERNEL_MUTEX_PEND_TASK_STK_SIZE              128u
#define  EX_KERNEL_MUTEX_PEND_TASK_PRIO                   22u

/********************************************************************************************************
 *                                        EXAMPLE MUTEX SETTINGS
 *******************************************************************************************************/

//                                                                 Number of pend/posts before deletion.
#define  EX_KERNEL_MUTEX_MAX_COUNT                         1u

/********************************************************************************************************
 ********************************************************************************************************
 *                                        LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 Pend Task Data.
static CPU_STK Ex_KernelMutexPendTaskStk[EX_KERNEL_MUTEX_PEND_TASK_STK_SIZE];
static OS_TCB  Ex_KernelMutexPendTaskTCB;

//                                                                 Mutex.
static OS_MUTEX Ex_KernelMutexObj;

//                                                                 Example execution counter.
static CPU_INT08U Ex_Done;

/********************************************************************************************************
 ********************************************************************************************************
 *                                      LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void Ex_KernelMutexPendTask(void *p_arg);

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
 *                                           Ex_KernelMutex()
 *
 * @brief  Illustrates the usage of Kernel mutexes.
 *******************************************************************************************************/
void Ex_KernelMutex(void)
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

  OSTaskCreate(&Ex_KernelMutexPendTaskTCB,                      // Create Pend Task.
               "Ex Kernel Mutex Pend Task",
               Ex_KernelMutexPendTask,
               0,
               EX_KERNEL_MUTEX_PEND_TASK_PRIO,
               &Ex_KernelMutexPendTaskStk[0],
               (EX_KERNEL_MUTEX_PEND_TASK_STK_SIZE / 10u),
               EX_KERNEL_MUTEX_PEND_TASK_STK_SIZE,
               0,
               0,
               0,
               (OS_OPT_TASK_STK_CLR),
               &err);
  //                                                               Check error code.
  APP_RTOS_ASSERT_DBG((err.Code == RTOS_ERR_NONE),; );

  //                                                               Ex_KernelMutexPendTask will execute.

  //                                                               Delay Task execution for
  OSTimeDly(500,                                                // 500 OS Ticks
            OS_OPT_TIME_DLY,                                    // from now.
            &err);

  //                                                               Ex_KernelMutexPendTask has blocked, execution
  //                                                               returns here.

  //                                                               Try to acquire mutex, the Pend Task will resume.
  OSMutexPend(&Ex_KernelMutexObj,                               // Pointer to the example mutex.
              0,                                                // Wait for infinity.
              OS_OPT_PEND_BLOCKING,                             // Task will block.
              DEF_NULL,                                         // Timestamp is not used.
              &err);
  if (err.Code == RTOS_ERR_NONE) {                              // Check error code.
                                                                // Ex_KernelMutexPendTask released the mutex,
                                                                // Ex_KernelMutex now has it.
    Ex_Done = 1u;

    EX_TRACE("Example Kernel Mutex: Ex_KernelMutex got the mutex, now releasing it.\r\n");

    //                                                             Release mutex.
    OSMutexPost(&Ex_KernelMutexObj,                             // Pointer to the example mutex.
                OS_OPT_POST_1,                                  // Only wake up highest-priority task.
                &err);
    //                                                             Check error code.
    APP_RTOS_ASSERT_DBG((err.Code == RTOS_ERR_NONE),; );
  } else {
    EX_TRACE("Example Kernel Mutex: unable to pend on mutex. OSMutexPend() returned with %i\r\n",
             err.Code);
    RTOS_ASSERT_CRITICAL_FAILED_END_CALL(0u);
  }

  //                                                               Delete Mutex.
  qty = OSMutexDel(&Ex_KernelMutexObj,                          // Pointer to the example evnt flag group.
                   OS_OPT_DEL_NO_PEND,                          // Option: delete if 0 tasks are pending.
                   &err);
  //                                                               Check error code.
  APP_RTOS_ASSERT_DBG((err.Code == RTOS_ERR_NONE),; );
  PP_UNUSED_PARAM(qty);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                        Ex_KernelMutexPendTask()
 *
 * @brief  This task pends on the mutex.
 *
 * @param  p_arg  Unused parameter
 *******************************************************************************************************/
static void Ex_KernelMutexPendTask(void *p_arg)
{
  RTOS_ERR err;

  //                                                               Prevent compiler warning.
  PP_UNUSED_PARAM(p_arg);

  //                                                               Task body.
  while (DEF_ON) {
    if (Ex_Done < EX_KERNEL_MUTEX_MAX_COUNT) {
      //                                                           Acquire mutex.
      OSMutexPend(&Ex_KernelMutexObj,                           // Pointer to the example mutex.
                  0,                                            // Wait for infinity.
                  OS_OPT_PEND_BLOCKING,                         // Task will block.
                  DEF_NULL,                                     // Timestamp is not used.
                  &err);
      if (err.Code == RTOS_ERR_NONE) {                          // Check error code.
        EX_TRACE("Example Kernel Mutex: Ex_KernelMutexPendTask got the mutex, now releasing it.\r\n");

        //                                                         Ex_KernelMutex will resume execution.

        //                                                         Delay Task execution for
        OSTimeDly(500,                                          // 500 OS Ticks
                  OS_OPT_TIME_DLY,                              // from now.
                  &err);
        //                                                         Check error code.
        APP_RTOS_ASSERT_DBG((err.Code == RTOS_ERR_NONE),; );

        //                                                         Ex_KernelMutex will resume execution.

        //                                                         Release mutex.
        OSMutexPost(&Ex_KernelMutexObj,                         // Pointer to the example mutex.
                    OS_OPT_POST_1,                              // Only wake up highest-priority task.
                    &err);

        //                                                         Check error code.
        APP_RTOS_ASSERT_DBG((err.Code == RTOS_ERR_NONE),; );
      } else {
        EX_TRACE("Example Kernel Mutex: unable to pend on mutex. OSMutexPend() returned with %i\r\n",
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
