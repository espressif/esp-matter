/***************************************************************************//**
 * @file
 * @brief Kernel Flag Pend Abort Example
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

#if (OS_CFG_FLAG_EN != DEF_ENABLED)
#error "This example requires OS_CFG_FLAG_EN to be set to DEF_ENABLED"
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
#define  EX_KERNEL_FLAG_PEND_TASK_STK_SIZE               128u
#define  EX_KERNEL_FLAG_PEND_TASK_PRIO                    22u

/********************************************************************************************************
 *                                         EXAMPLE FLAG SETTINGS
 *******************************************************************************************************/

//                                                                 Example Flags.
#define  EX_KERNEL_FLAG_A                                  (1u << 0)
#define  EX_KERNEL_FLAG_B                                  (1u << 1)
#define  EX_KERNEL_FLAG_ALL                                (EX_KERNEL_FLAG_A | EX_KERNEL_FLAG_B)
#define  EX_KERNEL_FLAG_MAX_COUNT                           1u

/********************************************************************************************************
 ********************************************************************************************************
 *                                        LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 Pend Task Data.
static CPU_STK Ex_KernelFlagPendTaskStk[EX_KERNEL_FLAG_PEND_TASK_STK_SIZE];
static OS_TCB  Ex_KernelFlagPendTaskTCB;

//                                                                 Event Flag Group.
static OS_FLAG_GRP Ex_KernelFlagGroup;

//                                                                 Example execution counter.
static CPU_INT08U Ex_Done;

/********************************************************************************************************
 ********************************************************************************************************
 *                                      LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void Ex_KernelFlagPendTask(void *p_arg);

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
 *                                            Ex_KernelFlag()
 *
 * @brief  Illustrates the usage of Kernel event flags.
 *******************************************************************************************************/
void Ex_KernelFlagPendAbort(void)
{
  RTOS_ERR   err;
  OS_OBJ_QTY qty;

  //                                                               Initialize Example.
  Ex_Done = 0u;

  //                                                               Create Event Flag Group.
  OSFlagCreate(&Ex_KernelFlagGroup,                             // Pointer to the example event flag group.
               "Ex Kernel Flags",                               // Name used for debugging.
               0,                                               // Initial flags, all cleared.
               &err);
  //                                                               Check error code.
  APP_RTOS_ASSERT_DBG((err.Code == RTOS_ERR_NONE),; );

  OSTaskCreate(&Ex_KernelFlagPendTaskTCB,                       // Create Pend Task.
               "Ex Kernel Flags Pend Task",
               Ex_KernelFlagPendTask,
               0,
               EX_KERNEL_FLAG_PEND_TASK_PRIO,
               &Ex_KernelFlagPendTaskStk[0],
               (EX_KERNEL_FLAG_PEND_TASK_STK_SIZE / 10u),
               EX_KERNEL_FLAG_PEND_TASK_STK_SIZE,
               0,
               0,
               0,
               (OS_OPT_TASK_STK_CLR),
               &err);
  //                                                               Check error code.
  APP_RTOS_ASSERT_DBG((err.Code == RTOS_ERR_NONE),; );

  //                                                               The Pend task will execute.

  //                                                               Delay Task execution for
  OSTimeDly(500,                                                // 500 OS Ticks
            OS_OPT_TIME_DLY,                                    // from now.
            &err);
  //                                                               Check error code.
  APP_RTOS_ASSERT_DBG((err.Code == RTOS_ERR_NONE),; );

  //                                                               The Pend task is blocked, execution returns here.

  //                                                               Abort the pend on the event flag.
  qty = OSFlagPendAbort(&Ex_KernelFlagGroup,                    // Pointer to the example event flag group.
                        OS_OPT_PEND_ABORT_ALL,                  // Abort all waiting tasks.
                        &err);
  if (err.Code == RTOS_ERR_NONE) {                              // Check error code.
    if (qty == 1) {
      Ex_Done = 1u;
      EX_TRACE("Example Kernel Flags Pend Abort: Ex_KernelFlagPendAbort successfully aborted the pend.\r\n");
    } else {
      EX_TRACE("Example Kernel Flags Pend Abort: wrong number of tasks aborted. OSFlagPendAbort() aborted %u tasks\r\n",
               qty);
      RTOS_ASSERT_CRITICAL_FAILED_END_CALL(; );
    }
  } else {
    EX_TRACE("Example Kernel Flags Pend Abort: error on pend abort. OSFlagPendAbort() returned with %i\r\n",
             err.Code);
    RTOS_ASSERT_CRITICAL_FAILED_END_CALL(; );
  }

  //                                                               Delete Event Flag Group.
  qty = OSFlagDel(&Ex_KernelFlagGroup,                          // Pointer to the example event flag group.
                  OS_OPT_DEL_NO_PEND,                           // Option: delete if 0 tasks are pending.
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
 *                                        Ex_KernelFlagPendTask()
 *
 * @brief  This task pends on the event flag group waiting for two flags to be set.
 *
 * @param  p_arg  Unused parameter
 *******************************************************************************************************/
static void Ex_KernelFlagPendTask(void *p_arg)
{
  RTOS_ERR err;
  OS_FLAGS flags;

  //                                                               Prevent compiler warning.
  PP_UNUSED_PARAM(p_arg);

  //                                                               Task body.
  while (DEF_ON) {
    if (Ex_Done < EX_KERNEL_FLAG_MAX_COUNT) {
      EX_TRACE("Example Kernel Flags Pend Abort: pend task pending on event flag.\r\n");
      //                                                           Wait until all flags are set.
      flags = OSFlagPend(&Ex_KernelFlagGroup,                   // Pointer to the example event flag group.
                         EX_KERNEL_FLAG_ALL,                    // Flag bitmask to match.
                         0,                                     // Wait for infinity.
                         OS_OPT_PEND_FLAG_SET_ALL               // Wait until all flags are set and
                         | OS_OPT_PEND_BLOCKING                 // task will block and
                         | OS_OPT_PEND_FLAG_CONSUME,            // function will clear the flags.
                         DEF_NULL,                              // Timestamp is not used.
                         &err);
      if (err.Code != RTOS_ERR_ABORT) {                         // Check error code.
        EX_TRACE("Example Kernel Flags Pend Abort: error on pend abort. OSFlagPend() returned with %i\r\n",
                 err.Code);
        RTOS_ASSERT_CRITICAL_FAILED_END_CALL(; );
      }
      PP_UNUSED_PARAM(flags);
    } else {
      //                                                           Delay Task execution for
      OSTimeDly(5000,                                           // 5000 OS Ticks
                OS_OPT_TIME_DLY,                                // from now.
                &err);
      //                                                           Check error code.
      APP_RTOS_ASSERT_DBG((err.Code == RTOS_ERR_NONE),; );
    }
  }
}
