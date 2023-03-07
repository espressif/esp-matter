/***************************************************************************//**
 * @file
 * @brief Kernel Flag Example
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

//                                                                 PostA Task Configuration.
#define  EX_KERNEL_FLAG_POSTA_TASK_STK_SIZE              128u
#define  EX_KERNEL_FLAG_POSTA_TASK_PRIO                   23u

//                                                                 PostB Task Configuration.
#define  EX_KERNEL_FLAG_POSTB_TASK_STK_SIZE              128u
#define  EX_KERNEL_FLAG_POSTB_TASK_PRIO                   24u

/********************************************************************************************************
 *                                         EXAMPLE FLAG SETTINGS
 *******************************************************************************************************/

//                                                                 Example Flags.
#define  EX_KERNEL_FLAG_A                                  (1u << 0)
#define  EX_KERNEL_FLAG_B                                  (1u << 1)
#define  EX_KERNEL_FLAG_ALL                                (EX_KERNEL_FLAG_A | EX_KERNEL_FLAG_B)
//                                                                 Number of Pend/Post cycles before deletion.
#define  EX_KERNEL_FLAG_MAX_COUNT                           2u

/********************************************************************************************************
 ********************************************************************************************************
 *                                        LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 Pend Task Data.
static CPU_STK Ex_KernelFlagPendTaskStk[EX_KERNEL_FLAG_PEND_TASK_STK_SIZE];
static OS_TCB  Ex_KernelFlagPendTaskTCB;

//                                                                 PostA Task Data.
static CPU_STK Ex_KernelFlagPostATaskStk[EX_KERNEL_FLAG_POSTA_TASK_STK_SIZE];
static OS_TCB  Ex_KernelFlagPostATaskTCB;

//                                                                 PostB Task Data.
static CPU_STK Ex_KernelFlagPostBTaskStk[EX_KERNEL_FLAG_POSTB_TASK_STK_SIZE];
static OS_TCB  Ex_KernelFlagPostBTaskTCB;

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
static void Ex_KernelFlagPostATask(void *p_arg);
static void Ex_KernelFlagPostBTask(void *p_arg);

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
void Ex_KernelFlag(void)
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

  OSTaskCreate(&Ex_KernelFlagPostATaskTCB,                       // Create PostA Task.
               "Ex Kernel Flags PostA Task",
               Ex_KernelFlagPostATask,
               0,
               EX_KERNEL_FLAG_POSTA_TASK_PRIO,
               &Ex_KernelFlagPostATaskStk[0],
               (EX_KERNEL_FLAG_POSTA_TASK_STK_SIZE / 10u),
               EX_KERNEL_FLAG_POSTA_TASK_STK_SIZE,
               0,
               0,
               0,
               (OS_OPT_TASK_STK_CLR),
               &err);
  //                                                               Check error code.
  APP_RTOS_ASSERT_DBG((err.Code == RTOS_ERR_NONE),; );

  OSTaskCreate(&Ex_KernelFlagPostBTaskTCB,                       // Create PostB Task.
               "Ex Kernel Flags PostB Task",
               Ex_KernelFlagPostBTask,
               0,
               EX_KERNEL_FLAG_POSTB_TASK_PRIO,
               &Ex_KernelFlagPostBTaskStk[0],
               (EX_KERNEL_FLAG_POSTB_TASK_STK_SIZE / 10u),
               EX_KERNEL_FLAG_POSTB_TASK_STK_SIZE,
               0,
               0,
               0,
               (OS_OPT_TASK_STK_CLR),
               &err);
  //                                                               Check error code.
  APP_RTOS_ASSERT_DBG((err.Code == RTOS_ERR_NONE),; );

  //                                                               Delay Task execution for
  OSTimeDly(5000,                                               // 5000 OS Ticks
            OS_OPT_TIME_DLY,                                    // from now.
            &err);
  //                                                               Check error code.
  APP_RTOS_ASSERT_DBG((err.Code == RTOS_ERR_NONE),; );

  //                                                               The three tasks had time to execute multiple
  //                                                               Pend/Post cycles.

  //                                                               Delete Event Flag Group.
  qty = OSFlagDel(&Ex_KernelFlagGroup,                          // Pointer to the example event flag group.
                  OS_OPT_DEL_NO_PEND,                           // Option: delete if 0 tasks are pending.
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
      //                                                           Wait until all flags are set.
      flags = OSFlagPend(&Ex_KernelFlagGroup,                   // Pointer to the example event flag group.
                         EX_KERNEL_FLAG_ALL,                    // Flag bitmask to match.
                         0,                                     // Wait for infinity.
                         OS_OPT_PEND_FLAG_SET_ALL               // Wait until all flags are set and
                         | OS_OPT_PEND_BLOCKING                 // task will block and
                         | OS_OPT_PEND_FLAG_CONSUME,            // function will clear the flags.
                         DEF_NULL,                              // Timestamp is not used.
                         &err);
      if (err.Code != RTOS_ERR_NONE) {                          // Check error code.
        EX_TRACE("Example Kernel Flags: unable to pend on event flag. OSFlagPend() returned with %i\r\n",
                 err.Code);
        RTOS_ASSERT_CRITICAL_FAILED_END_CALL(; );
      } else {
        flags = OSFlagPendGetFlagsRdy(&err);
        EX_TRACE("Example Kernel Flags: Ex_KernelFlagPendTask was awakened by the two flags: %u.\r\n", flags);
        Ex_Done++;
      }
    } else {
      //                                                           Delay Task execution for
      OSTimeDly(5000,                                           // 5000 OS Ticks
                OS_OPT_TIME_DLY,                                // from now.
                &err);
      //                                                           Check error code.
      APP_RTOS_ASSERT_DBG((err.Code == RTOS_ERR_NONE),; );
    }

    PP_UNUSED_PARAM(flags);
  }
}

/****************************************************************************************************//**
 *                                       Ex_KernelFlagPostATask()
 *
 * @brief  This task posts the event flag 'A' to the event flag group.
 *
 * @param  p_arg  Unused parameter
 *******************************************************************************************************/
static void Ex_KernelFlagPostATask(void *p_arg)
{
  RTOS_ERR err;
  OS_FLAGS flags;

  //                                                               Prevent compiler warning.
  PP_UNUSED_PARAM(p_arg);

  //                                                               Task body.
  while (DEF_ON) {
    if (Ex_Done < EX_KERNEL_FLAG_MAX_COUNT) {
      //                                                           Set the event flag 'A'.
      flags = OSFlagPost(&Ex_KernelFlagGroup,                   // Pointer to the example event flag group.
                         EX_KERNEL_FLAG_A,                      // Example Flag A bitmask.
                         OS_OPT_POST_FLAG_SET,                  // Set the flag.
                         &err);
      if (err.Code != RTOS_ERR_NONE) {                          // Check error code.
        EX_TRACE("Example Kernel Flags: unable to post the event flag. OSFlagPost() returned with %i\r\n",
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
    PP_UNUSED_PARAM(flags);
  }
}

/****************************************************************************************************//**
 *                                       Ex_KernelFlagPostBTask()
 *
 * @brief  This task posts the event flag 'B' to the event flag group.
 *
 * @param  p_arg  Unused parameter
 *******************************************************************************************************/
static void Ex_KernelFlagPostBTask(void *p_arg)
{
  RTOS_ERR err;
  OS_FLAGS flags;

  //                                                               Prevent compiler warning.
  PP_UNUSED_PARAM(p_arg);

  //                                                               Task body.
  while (DEF_ON) {
    if (Ex_Done < EX_KERNEL_FLAG_MAX_COUNT) {
      //                                                           Set the event flag 'B'.
      flags = OSFlagPost(&Ex_KernelFlagGroup,                   // Pointer to the example event flag group.
                         EX_KERNEL_FLAG_B,                      // Example Flag B bitmask.
                         OS_OPT_POST_FLAG_SET,                  // Set the flag.
                         &err);
      if (err.Code != RTOS_ERR_NONE) {                          // Check error code.
        EX_TRACE("Example Kernel Flags: unable to post the event flag. OSFlagPost() returned with %i\r\n",
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
    PP_UNUSED_PARAM(flags);
  }
}
