/***************************************************************************//**
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#define  CMSIS_SOURCE
#include PLATFORM_HEADER
#include "hal.h"
#include "cmsis-rtos-ipc-config.h"
#include "stack/include/ember.h"
#include "cmsis-stack-ipc/cmsis-rtos-support.h"
#include <em_emu.h>
#include <em_core.h>

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif

#if defined(SL_CATALOG_CONNECT_DEBUG_PRINT_PRESENT)
#include "debug_print.h"
#endif

#if defined(SL_CATALOG_MICRIUMOS_KERNEL_PRESENT)
#include <kernel/include/os.h>

#if (OS_CFG_TASK_STK_REDZONE_EN == DEF_ENABLED)
void  App_OS_RedzoneHitHook (OS_TCB *p_tcb);
#endif
void  App_OS_StatTaskHook (void);
void  App_OS_TaskCreateHook (OS_TCB *p_tcb);
void  App_OS_TaskDelHook (OS_TCB *p_tcb);
void  App_OS_TaskReturnHook (OS_TCB *p_tcb);
void  App_OS_TaskSwHook (void);

/*
 ************************************************************************************************************************
 *                                              SET ALL APPLICATION HOOKS
 *
 * Description: Set ALL application hooks.
 *
 * Arguments  : none.
 *
 * Note(s)    : none
 ************************************************************************************************************************
 */
void  App_OS_SetAllHooks(void)
{
#if OS_CFG_APP_HOOKS_EN > 0u
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
#if (OS_CFG_TASK_STK_REDZONE_EN == DEF_ENABLED)
  OS_AppRedzoneHitHookPtr = App_OS_RedzoneHitHook;
#endif

  OS_AppStatTaskHookPtr   = App_OS_StatTaskHook;

  OS_AppTaskCreateHookPtr = App_OS_TaskCreateHook;

  OS_AppTaskDelHookPtr    = App_OS_TaskDelHook;

  OS_AppTaskReturnHookPtr = App_OS_TaskReturnHook;

  OS_AppTaskSwHookPtr     = App_OS_TaskSwHook;
  CORE_EXIT_ATOMIC();
#endif
}

/*
 ************************************************************************************************************************
 *                                             CLEAR ALL APPLICATION HOOKS
 *
 * Description: Clear ALL application hooks.
 *
 * Arguments  : none.
 *
 * Note(s)    : none
 ************************************************************************************************************************
 */
void  App_OS_ClrAllHooks(void)
{
#if OS_CFG_APP_HOOKS_EN > 0u
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
#if (OS_CFG_TASK_STK_REDZONE_EN == DEF_ENABLED)
  OS_AppRedzoneHitHookPtr = (OS_APP_HOOK_TCB)0;
#endif

  OS_AppStatTaskHookPtr   = (OS_APP_HOOK_VOID)0;

  OS_AppTaskCreateHookPtr = (OS_APP_HOOK_TCB)0;

  OS_AppTaskDelHookPtr    = (OS_APP_HOOK_TCB)0;

  OS_AppTaskReturnHookPtr = (OS_APP_HOOK_TCB)0;

  OS_AppTaskSwHookPtr     = (OS_APP_HOOK_VOID)0;
  CORE_EXIT_ATOMIC();
#endif
}

/*
 ************************************************************************************************************************
 *                                              APPLICATION IDLE TASK HOOK
 *
 * Description: This function is called by the idle task.  This hook has been added to allow you to do such things as
 *              STOP the CPU to conserve power.
 *
 * Arguments  : none
 *
 * Note(s)    : none
 ************************************************************************************************************************
 */
void  OSIdleEnterHook(void)
{
#ifndef DISABLE_WATCHDOG
  halResetWatchdog();
#endif
}

/*
 ************************************************************************************************************************
 *                                             APPLICATION REDZONE HIT HOOK
 *
 * Description: This function is called when a task's stack overflowed.
 *
 * Arguments  : p_tcb   is a pointer to the task control block of the offending task. NULL if ISR.
 *
 * Note(s)    : None.
 ************************************************************************************************************************
 */
#if (OS_CFG_TASK_STK_REDZONE_EN == DEF_ENABLED)
void  App_OS_RedzoneHitHook(OS_TCB *p_tcb)
{
  while (DEF_ON) {
#ifndef DISABLE_WATCHDOG
    halResetWatchdog();
#endif

#if defined(SL_CATALOG_CONNECT_DEBUG_PRINT_PRESENT)
#if (OS_CFG_DBG_EN == DEF_ENABLED)
    connect_core_debug_print("stack overflow: %s\n",
                             ((p_tcb) ? p_tcb->NamePtr : "isr"));
#else
    connect_core_debug_print("stack overflow: p_tcb=%4x\n", p_tcb);
#endif
#endif // SL_CATALOG_CONNECT_DEBUG_PRINT_PRESENT
  }
}
#endif

/*
 ************************************************************************************************************************
 *                                           APPLICATION STATISTIC TASK HOOK
 *
 * Description: This function is called every second by uC/OS-III's statistics task.  This allows your application to add
 *              functionality to the statistics task.
 *
 * Arguments  : none
 *
 * Note(s)    : none
 ************************************************************************************************************************
 */
void  App_OS_StatTaskHook(void)
{
}

/*
 ************************************************************************************************************************
 *                                            APPLICATION TASK CREATION HOOK
 *
 * Description: This function is called when a task is created.
 *
 * Arguments  : p_tcb   is a pointer to the task control block of the task being created.
 *
 * Note(s)    : none
 ************************************************************************************************************************
 */
void  App_OS_TaskCreateHook(OS_TCB *p_tcb)
{
  (void)&p_tcb;
}

/*
 ************************************************************************************************************************
 *                                            APPLICATION TASK DELETION HOOK
 *
 * Description: This function is called when a task is deleted.
 *
 * Arguments  : p_tcb   is a pointer to the task control block of the task being deleted.
 *
 * Note(s)    : none
 ************************************************************************************************************************
 */
void  App_OS_TaskDelHook(OS_TCB *p_tcb)
{
  (void)&p_tcb;
}

/*
 ************************************************************************************************************************
 *                                             APPLICATION TASK RETURN HOOK
 *
 * Description: This function is called if a task accidentally returns.  In other words, a task should either be an
 *              infinite loop or delete itself when done.
 *
 * Arguments  : p_tcb     is a pointer to the OS_TCB of the task that is returning.
 *
 * Note(s)    : none
 ************************************************************************************************************************
 */
void  App_OS_TaskReturnHook(OS_TCB *p_tcb)
{
  (void)&p_tcb;
}

/*
 ************************************************************************************************************************
 *                                             APPLICATION TASK SWITCH HOOK
 *
 * Description: This function is called when a task switch is performed.  This allows you to perform other operations
 *              during a context switch.
 *
 * Arguments  : none
 *
 * Note(s)    : 1) Interrupts are disabled during this call.
 *              2) It is assumed that the global pointer 'OSTCBHighRdyPtr' points to the TCB of the task that will be
 *                 'switched in' (i.e. the highest priority task) and, 'OSTCBCurPtr' points to the task being switched out
 *                 (i.e. the preempted task).
 ************************************************************************************************************************
 */
void  App_OS_TaskSwHook(void)
{
}

#endif // SL_CATALOG_MICRIUMOS_KERNEL_PRESENT
