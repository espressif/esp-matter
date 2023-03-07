/***************************************************************************//**
 * @file
 * @brief OS CPU Port File
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

/****************************************************************************************************//**
 * @note     (1) This driver targets the following:
 *               Core      : ARMv6M Cortex-M
 *               Toolchain : ARMCC Compiler
 *******************************************************************************************************/

#define   OS_CPU_GLOBALS

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const CPU_CHAR *os_cpu_c__c = "$Id: $";
#endif

/********************************************************************************************************
 *                                             INCLUDE FILES
 *******************************************************************************************************/

#include  <em_core.h>

#include  <kernel/include/os.h>
#include  <kernel/source/os_priv.h>

#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif
#ifdef SL_CATALOG_POWER_MANAGER_PRESENT
#include <sl_power_manager.h>
#endif

#ifdef __cplusplus
extern  "C" {
#endif

/********************************************************************************************************
 *                                             GLOBAL VARIABLES
 *******************************************************************************************************/
#if (OS_CFG_ERRNO_EN == 1)
extern int micriumos_errno;
#endif

/*****************************************************************************************************//**
 *                                         OSIdleContextPowerManagerHook
 *
 * @brief    This function allows to customize how the sleep mode is entered. However,
 *           sl_power_manager_sleep() MUST ultimately be called WITH the scheduler locked.
 *
 * @note     This function is for internal use only.
 *******************************************************************************************************/
__WEAK void OSIdleContextPowerManagerHook(void)
{
#ifdef SL_CATALOG_POWER_MANAGER_PRESENT
  RTOS_ERR err;

  OSSchedLock(&err);
  OS_ASSERT_DBG_NO_ERR(RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  sl_power_manager_sleep();
  OSSchedUnlock(&err);
  OS_ASSERT_DBG_NO_ERR(RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
#endif
}

/*****************************************************************************************************//**
 *                                             OSIdleContext
 *
 * @brief    This function handles idling. It should never return.
 *           Being called from an idle context, it means there is no context save.
 *           Extra care must be taken as no states will be saved and any set of instructions with side
 *           effects shall be performed in a critical section.
 *           This function is called using the isr stack.
 *******************************************************************************************************/
void OSIdleContext(void)
{
#ifdef SL_CATALOG_POWER_MANAGER_PRESENT
  OSIdleContextPowerManagerHook();
#else
  while (1) {
    __NOP();
  }
#endif
}

/*****************************************************************************************************//**
 *                                       OS INITIALIZATION HOOK
 *
 * @brief    This function is called by OSInit() at the beginning of OSInit().
 *******************************************************************************************************/
void OSInitHook(void)
{
  //                                                               8-byte align the ISR stack.
  OS_CPU_ExceptStkBase = (CPU_STK *)&OSCfg_ISRStkBasePtr[OSCfg_ISRStkSize - 1u];
  OS_CPU_ExceptStkBase = (CPU_STK *)((CPU_STK)(OS_CPU_ExceptStkBase) & 0xFFFFFFF8);
}

/****************************************************************************************************//**
 *                                           REDZONE HIT HOOK
 *
 * @brief    This function is called when a task's stack overflowed.
 *
 * @param    p_tcb        Pointer to the task control block of the offending task. NULL if ISR.
 *******************************************************************************************************/
#if (OS_CFG_TASK_STK_REDZONE_EN == DEF_ENABLED)
void OSRedzoneHitHook(OS_TCB *p_tcb)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
#if OS_CFG_APP_HOOKS_EN > 0u
  if (OS_AppRedzoneHitHookPtr != (OS_APP_HOOK_TCB)0) {
    (*OS_AppRedzoneHitHookPtr)(p_tcb);
  } else {
    CPU_SW_EXCEPTION(; );
  }
#else
  (void)p_tcb;                                                  // Prevent compiler warning
  CPU_SW_EXCEPTION(; );
#endif
  CORE_EXIT_ATOMIC();
}
#endif

/****************************************************************************************************//**
 *                                         STATISTIC TASK HOOK
 *
 * @brief    This function is called every second by uC/OS-III's statistics task.  This allows your
 *           application to add functionality to the statistics task.
 *******************************************************************************************************/
void OSStatTaskHook(void)
{
#if OS_CFG_APP_HOOKS_EN > 0u
  if (OS_AppStatTaskHookPtr != (OS_APP_HOOK_VOID)0) {
    (*OS_AppStatTaskHookPtr)();
  }
#endif
}

/****************************************************************************************************//**
 *                                          TASK CREATION HOOK
 *
 * @brief    This function is called when a task is created.
 *
 * @param    p_tcb        Pointer to the task control block of the task being created.
 *******************************************************************************************************/
void OSTaskCreateHook(OS_TCB *p_tcb)
{
#if OS_CFG_APP_HOOKS_EN > 0u
  if (OS_AppTaskCreateHookPtr != (OS_APP_HOOK_TCB)0) {
    (*OS_AppTaskCreateHookPtr)(p_tcb);
  }
#else
  (void)p_tcb;                                                  // Prevent compiler warning
#endif
}

/****************************************************************************************************//**
 *                                           TASK DELETION HOOK
 *
 * @brief    This function is called when a task is deleted.
 *
 * @param    p_tcb        Pointer to the task control block of the task being deleted.
 *******************************************************************************************************/
void OSTaskDelHook(OS_TCB *p_tcb)
{
#if OS_CFG_APP_HOOKS_EN > 0u
  if (OS_AppTaskDelHookPtr != (OS_APP_HOOK_TCB)0) {
    (*OS_AppTaskDelHookPtr)(p_tcb);
  }
#else
  (void)p_tcb;                                                  // Prevent compiler warning
#endif
}

/****************************************************************************************************//**
 *                                            TASK RETURN HOOK
 *
 * @brief    This function is called if a task accidentally returns.  In other words, a task should
 *              either be an infinite loop or delete itself when done.
 *
 * @param    p_tcb        Pointer to the task control block of the task that is returning.
 *******************************************************************************************************/
void OSTaskReturnHook(OS_TCB *p_tcb)
{
#if OS_CFG_APP_HOOKS_EN > 0u
  if (OS_AppTaskReturnHookPtr != (OS_APP_HOOK_TCB)0) {
    (*OS_AppTaskReturnHookPtr)(p_tcb);
  }
#else
  (void)p_tcb;                                                  // Prevent compiler warning
#endif
}

/****************************************************************************************************//**
 *                                        INITIALIZE A TASK'S STACK
 *
 * @brief    This function is called by either OSTaskCreate() or OSTaskCreateExt() to initialize the
 *              stack frame of the task being created.  This function is highly processor specific.
 *
 * @param    p_task       Pointer to the task entry point address.
 *
 * @param    p_arg        Pointer to a user supplied data area that will be passed to the task
 *                        when the task first executes.
 *
 * @param    p_stk_base   Pointer to the base address of the stack.
 *
 * @param    stk_size     Size of the stack, in number of CPU_STK elements.
 *
 * @param    opt          Options used to alter the behavior of OS_Task_StkInit().
 *                        (see OS.H for OS_TASK_OPT_xxx).
 *
 * @return   Always returns the location of the new top-of-stack once the processor registers have
 *              been placed on the stack in the proper order.
 *
 * @note     (1) Interrupts are enabled when task starts executing.
 *
 * @note     (2) All tasks run in Thread mode, using process stack.
 *
 * @note     (3) The SP must be 8-byte aligned in conforming to the Procedure Call Standard for the ARM architecture
 *               - (a) Section 2.1 of the  ABI for the ARM Architecture Advisory Note. SP must be 8-byte aligned
 *                     on entry to AAPCS-Conforming functions states :
 *                     The Procedure Call Standard for the ARM Architecture [AAPCS] requires primitive
 *                     data types to be naturally aligned according to their sizes (for size = 1, 2, 4, 8 bytes).
 *                     Doing otherwise creates more problems than it solves.
 *                     In return for preserving the natural alignment of data, conforming code is permitted
 *                     to rely on that alignment. To support aligning data allocated on the stack, the stack
 *                     pointer (SP) is required to be 8-byte aligned on entry to a conforming function. In
 *                     practice this requirement is met if:
 *                   - (1) At each call site, the current size of the calling function's stack frame is a multiple of 8 bytes.
 *                         This places an obligation on compilers and assembly language programmers.
 *                   - (2) SP is a multiple of 8 when control first enters a program.
 *                         This places an obligation on authors of low level OS, RTOS, and runtime library
 *                         code to align SP at all points at which control first enters
 *                         a body of (AAPCS-conforming) code.
 *                   In turn, this requires the value of SP to be aligned to 0 modulo 8:
 *                   - (3) By exception handlers, before calling AAPCS-conforming code.
 *                   - (4) By OS/RTOS/run-time system code, before giving control to an application.
 *               - (b) Section 2.3.1 corrective steps from the the SP must be 8-byte aligned on entry
 *                     to AAPCS-conforming functions advisory note also states.
 *                     " This requirement extends to operating systems and run-time code for all architecture versions
 *                     prior to ARMV7 and to the A, R and M architecture profiles thereafter.
 *                   - (1) Even if the SP 8-byte aligment is not a requirement for the ARMv7M profile, the stack is aligned
 *                         to 8-byte boundaries to support legacy execution enviroments.
 *               - (c) Section 5.2.1.2 from the Procedure Call Standard for the ARM
 *                     architecture states :  "The stack must also conform to the following
 *                     constraint at a public interface:
 *                   - (1) SP mod 8 = 0. The stack must be double-word aligned"
 *               - (d) From the ARM Technical Support Knowledge Base. 8 Byte stack aligment.
 *                     "8 byte stack alignment is a requirement of the ARM Architecture Procedure
 *                     Call Standard [AAPCS]. This specifies that functions must maintain an 8 byte
 *                     aligned stack address (e.g. 0x00, 0x08, 0x10, 0x18, 0x20) on all external
 *                     interfaces. In practice this requirement is met if:
 *                   - (1) At each external interface, the current stack pointer
 *                         is a multiple of 8 bytes.
 *                   - (2) Your OS maintains 8 byte stack alignment on its external interfaces
 *                         e.g. on task switches"
 *
 * @note     (4) Exception Return Behavior(EXEC_RETURN)
 *               0xFFFFFFF1     Return to Thread mode. Exception return gets state from the Main Stack.
 *                           On Return execution uses the Main Stack.
 *
 *               0xFFFFFFF9     Return to Thread mode. Exception return gets state from the Main Stack.
 *                           On Return execution uses the Main Stack.
 *
 *               0xFFFFFFFD     Return to Thread mode. Exception return gets state from the Process Stack.
 *                           On Return execution uses the Process Stack.
 *******************************************************************************************************/
CPU_STK *OSTaskStkInit(OS_TASK_PTR  p_task,
                       void         *p_arg,
                       CPU_STK      *p_stk_base,
                       CPU_STK      *p_stk_limit,
                       CPU_STK_SIZE stk_size,
                       OS_OPT       opt)
{
  CPU_STK *p_stk;

  (void)opt;                                                    // 'opt' is not used, prevent warning

  p_stk = &p_stk_base[stk_size];                                // Load stack pointer
                                                                // Align the stack to 8-bytes.
  p_stk = (CPU_STK *)((CPU_STK)(p_stk) & 0xFFFFFFF8u);
  //                                                               Registers stacked as if auto-saved on exception
  *(--p_stk) = (CPU_STK)0x01000000u;                            // xPSR
  *(--p_stk) = (CPU_STK)p_task;                                 // Entry Point
  *(--p_stk) = (CPU_STK)OS_TaskReturn;                          // R14 (LR)
  *(--p_stk) = (CPU_STK)0x12121212u;                            // R12
  *(--p_stk) = (CPU_STK)0x03030303u;                            // R3
  *(--p_stk) = (CPU_STK)0x02020202u;                            // R2
  *(--p_stk) = (CPU_STK)p_stk_limit;                            // R1
  *(--p_stk) = (CPU_STK)p_arg;                                  // R0 : argument
                                                                // Remaining registers saved on process stack
  *(--p_stk) = (CPU_STK)0xFFFFFFFDuL;                           // R14: EXEC_RETURN; See Note 4
  *(--p_stk) = (CPU_STK)0x11111111uL;                           // R11
  *(--p_stk) = (CPU_STK)0x10101010uL;                           // R10
  *(--p_stk) = (CPU_STK)0x09090909uL;                           // R9
  *(--p_stk) = (CPU_STK)0x08080808uL;                           // R8
  *(--p_stk) = (CPU_STK)0x07070707uL;                           // R7
  *(--p_stk) = (CPU_STK)0x06060606uL;                           // R6
  *(--p_stk) = (CPU_STK)0x05050505uL;                           // R5
  *(--p_stk) = (CPU_STK)0x04040404uL;                           // R4

  return (p_stk);
}

/****************************************************************************************************//**
 *                                           TASK SWITCH HOOK
 *
 * @brief    This function is called when a task switch is performed.  This allows you to perform other
 *              operations during a context switch.
 *
 * @note     (1) Interrupts are disabled during this call.
 *
 * @note     (2) It is assumed that the global pointer 'OSTCBHighRdyPtr' points to the TCB of the task
 *                 that will be 'switched in' (i.e. the highest priority task) and, 'OSTCBCurPtr' points
 *                 to the task being switched out (i.e. the preempted task).
 *******************************************************************************************************/
void OSTaskSwHook(void)
{
#if (OS_CFG_TASK_STK_REDZONE_EN == DEF_ENABLED)
  CPU_BOOLEAN stk_status;
#endif

#if (OS_CFG_SCHED_ROUND_ROBIN_EN == DEF_ENABLED)
  if (OSTCBHighRdyPtr != DEF_NULL && OSSchedRoundRobinEn) {
    if (OSTCBHighRdyPtr->TimeQuantaCtr == 0u) {
      OS_SchedRoundRobinResetQuanta(OSTCBHighRdyPtr);
    }
    OS_SchedRoundRobinRestartTimer(OSTCBHighRdyPtr);
  }
#endif

#if OS_CFG_APP_HOOKS_EN > 0u
  if (OS_AppTaskSwHookPtr != (OS_APP_HOOK_VOID)0) {
    (*OS_AppTaskSwHookPtr)();
  }
#endif

  OS_TRACE_TASK_SWITCHED_IN(OSTCBHighRdyPtr);

#ifdef  CPU_CFG_INT_DIS_MEAS_EN
  if (OSTCBCurPtr != DEF_NULL) {
    int_dis_time = CPU_IntDisMeasMaxCurReset();                 // Keep track of per-task interrupt disable time
    if (OSTCBCurPtr->IntDisTimeMax < int_dis_time) {
      OSTCBCurPtr->IntDisTimeMax = int_dis_time;
    }
  }
#endif

#if OS_CFG_SCHED_LOCK_TIME_MEAS_EN > 0u
  //                                                               Keep track of per-task scheduler lock time
  if ((OSTCBCurPtr != DEF_NULL)
      && (OSTCBCurPtr->SchedLockTimeMax < OSSchedLockTimeMaxCur)) {
    OSTCBCurPtr->SchedLockTimeMax = OSSchedLockTimeMaxCur;
  }
  OSSchedLockTimeMaxCur = (CPU_TS)0;                            // Reset the per-task value
#endif

#if (OS_CFG_TASK_STK_REDZONE_EN == DEF_ENABLED)
  //                                                               Check if stack overflowed.
  if (OSTCBCurPtr != DEF_NULL) {
    stk_status = OSTaskStkRedzoneChk(DEF_NULL);
    if (stk_status != DEF_OK) {
      OSRedzoneHitHook(OSTCBCurPtr);
    }
  }
#endif

#if (OS_CFG_ERRNO_EN == 1)
  if (OSTCBCurPtr != DEF_NULL) {
    OSTCBCurPtr->local_errno = micriumos_errno;
  }
  if (OSTCBHighRdyPtr != DEF_NULL) {
    micriumos_errno = OSTCBHighRdyPtr->local_errno;
  }
#endif
}

/*****************************************************************************************************//**
 *                                             OSCanReturnToSleep()
 *
 * @brief    Allows performing other operations during an Idle check.
 *
 * @return   True, if the system can sleep after the interrupt.
 *           False, otherwise.
 *******************************************************************************************************/
__WEAK bool OSCanReturnToSleep(void)
{
  return (OSIsIdle());
}

/***************************************************************************//**
 * Check if the MCU can sleep after an interrupt.
 *
 * @return  True, if the system can sleep after the interrupt.
 *          False, otherwise.
 ******************************************************************************/
bool sl_power_manager_sleep_on_isr_exit(void)
{
  return (OSCanReturnToSleep());
}

/***************************************************************************//**
 * Check if sleep must be cancelled
 *
 * @return  True, if the system can go to sleep.
 *          False, otherwise.
 ******************************************************************************/
bool sl_power_manager_is_ok_to_sleep(void)
{
  return (OSIsIdle());
}

#ifdef __cplusplus
}
#endif
