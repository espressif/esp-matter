/***************************************************************************//**
 * @file
 * @brief Kernel - ARM Cortex-M Port
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
 * @note     (1) This port targets the following:
 *                   Core      : ARMv7M and ARMv8M Cortex-M
 *                   Mode      : Thumb-2 ISA
 *                   Toolchain : IAR EWARM
 *
 * @note     (2) This port supports the ARM Cortex-M3, Cortex-M4, Cortex-M7 and Cortex-M33 architectures.
 *******************************************************************************************************/

#define   OS_CPU_GLOBALS

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const CPU_CHAR *os_cpu_c__c = "$Id: $";
#endif

/********************************************************************************************************
 *                                               INCLUDE FILES
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

/********************************************************************************************************
 *                                           FLOATING POINT DEFINES
 *******************************************************************************************************/

#define  CPU_REG_FP_FPCCR              (*((CPU_REG32 *)0xE000EF34uL)) // Floating-Point Context Control Reg.

//                                                                 Enabled FP lazy stacking and enable ..
//                                                                 ..automatic state saving.
#define  CPU_REG_FPCCR_LAZY_STK                        0xC0000000uL

/********************************************************************************************************
 *                                            INTERRUPT HANDLING
 *******************************************************************************************************/

#pragma optimize=speed
void OSDisableIRQ(void)
{
#if (CORE_ATOMIC_METHOD == CORE_ATOMIC_METHOD_BASEPRI)
  __set_BASEPRI(CORE_ATOMIC_BASE_PRIORITY_LEVEL << (8 - __NVIC_PRIO_BITS));
#else
  __disable_irq();
#endif
}

#pragma optimize=speed
void OSEnableIRQ(void)
{
#if (CORE_ATOMIC_METHOD == CORE_ATOMIC_METHOD_BASEPRI)
  __set_BASEPRI(0);
#else
  __enable_irq();
#endif
}

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
 *                                               OSInitHook()
 *
 * @brief    Called by OSInit() at the beginning of OSInit().
 *
 * @note     (1) When using hardware floating point, follow these steps during the reset handler:
 *                - (a) Set full access for CP10 and CP11 bits in CPACR register.
 *                - (b) Set bits ASPEN and LSPEN in FPCCR register.
 *******************************************************************************************************/
void OSInitHook(void)
{
#if (OS_CPU_ARM_FP_EN > 0u)
  CPU_INT32U reg_val;
#endif
  //                                                               8-byte align the ISR stack.
  OS_CPU_ExceptStkBase = (CPU_STK *)&OSCfg_ISRStkBasePtr[OSCfg_ISRStkSize - 1u];
  OS_CPU_ExceptStkBase = (CPU_STK *)((CPU_STK)(OS_CPU_ExceptStkBase) & 0xFFFFFFF8);

#if (OS_CPU_ARM_FP_EN > 0u)
  reg_val = CPU_REG_FP_FPCCR;                                   // Check the floating point mode.
  if ((reg_val & CPU_REG_FPCCR_LAZY_STK) != CPU_REG_FPCCR_LAZY_STK) {
    while (1u) {                                                // See Note (1).
      ;
    }
  }
#endif
}

/****************************************************************************************************//**
 *                                           OSRedzoneHitHook()
 *
 * @brief    Called when a task's stack has overflowed.
 *
 * @param    p_tcb   Pointer to the TCB of the offending task. NULL if ISR.
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
 *                                               OSStatTaskHook()
 *
 * @brief    This function is called every second by the Kernel's statistics task.  This allows your
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
 *                                           OSTaskCreateHook()
 *
 * @brief    Called when a task is created.
 *
 * @param    p_tcb   Pointer to the TCB of the task being created.
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
 *                                               OSTaskDelHook()
 *
 * @brief    Called when a task is deleted.
 *
 * @param    p_tcb   Pointer to the TCB of the task being deleted.
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
 *                                           OSTaskReturnHook()
 *
 * @brief    Called if a task accidentally returns. In other words, a task should either be an infinite
 *           loop or delete itself when done.
 *
 * @param    p_tcb   Pointer to the TCB of the task that is returning.
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
 *                                               OSTaskStkInit()
 *
 * @brief    Initializes the stack frame of the task being created as if it had been already
 *           switched-out. This function is called by OSTaskCreate() and is highly processor specific.
 *
 * @param    p_task          Pointer to the task entry point address.
 *
 * @param    p_arg           Pointer to a user-supplied data area that will be passed to the task
 *                           when the task first executes.
 *
 * @param    p_stk_base      Pointer to the base address of the stack.
 *
 * @param    p_stk_limit     Pointer to the element to set as the 'watermark' limit of the stack.
 *
 * @param    stk_size        Size of the stack (measured as number of CPU_STK elements).
 *
 * @param    opt             Options used to alter the behavior of OSTaskStkInit().
 *                           See OS.H for OS_TASK_OPT_xxx.
 *
 * @return   Always returns the location of the new top-of-stack once the processor registers have
 *           been placed on the stack in the proper order.
 *
 * @note     (1) Interrupts are enabled when task starts executing.
 *
 * @note     (2) All tasks run in Thread mode, using process stack.
 *
 * @note     (3) There are two different stack frames depending on whether the Floating-Point(FP)
 *               co-processor is enabled or not.
 *               - (a) The stack frame shown in the diagram is used when the Co-processor Access Control
 *                     Register(CPACR) is disabling the Floating Point Unit. In this case, the FP
 *                     registers(S0- S31) & FP Status Control(FPSCR) register are not saved in the stack frame.
 *               - (b) The stack frame shown in the diagram is used when the Floating Point Unit is enabled,
 *                     that is, CP10 and CP11 field in CPACR are ones and FPCCR sets bits ASPEN and LSPEN to 1,
 *                     and also only when a FP instruction is used.
 *               - (c) When enabling the FPU through CPACR, make sure to set bits ASPEN and LSPEN in the
 *                     Floating-Point Context Control Register (FPCCR).
 *                       @verbatim
 *                                               +-------------+
 *                                               |             |
 *                                               +-------------+
 *                                               |             |
 *                                               +-------------+
 *                                               |    FPSCR    |
 *                                               +-------------+
 *                                               |     S15     |
 *                                               +-------------+
 *                                               |     S14     |
 *                                               +-------------+
 *                                               |     S13     |
 *                                               +-------------+
 *                                                       .
 *                                                       .
 *                                                       .
 *                                               +-------------+
 *                                               |     S2      |
 *                                               +-------------+
 *                                               |     S1      |
 *                         +-------------+       +-------------+
 *                         |             |       |     S0      |
 *                         +-------------+       +-------------+
 *                         |    xPSR     |       |    xPSR     |
 *                         +-------------+       +-------------+
 *                         | Return Addr |       | Return Addr |
 *                         +-------------+       +-------------+
 *                         |  LR(R14)    |       |   LR(R14)   |
 *                         +-------------+       +-------------+
 *                         |    R12      |       |     R12     |
 *                         +-------------+       +-------------+
 *                         |    R3       |       |     R3      |
 *                         +-------------+       +-------------+
 *                         |    R2       |       |     R2      |
 *                         +-------------+       +-------------+
 *                         |    R1       |       |     R1      |
 *                         +-------------+       +-------------+
 *                         |    R0       |       |     R0      |
 *                         +-------------+       +-------------+
 *                         | EXEC_RETURN |       |     S31     |
 *                         +-------------+       +-------------+
 *                         |    R11      |       |     S30     |
 *                         +-------------+       +-------------+
 *                         |    R10      |       |     S29     |
 *                         +-------------+       +-------------+
 *                         |    R9       |               .
 *                         +-------------+               .
 *                         |    R8       |               .
 *                         +-------------+       +-------------+
 *                         |    R7       |       |     S17     |
 *                         +-------------+       +-------------+
 *                         |    R6       |       |     S16     |
 *                         +-------------+       +-------------+
 *                         |    R5       |       | EXEC_RETURN |
 *                         +-------------+       +-------------+
 *                         |    R4       |       |     R11     |
 *                         +-------------+       +-------------+
 *                            - (a)              |     R10     |
 *                                               +-------------+
 *                                               |     R9      |
 *                                               +-------------+
 *                                               |     R8      |
 *                                               +-------------+
 *                                               |     R7      |
 *                                               +-------------+
 *                                               |     R6      |
 *                                               +-------------+
 *                                               |     R5      |
 *                                               +-------------+
 *                                               |     R4      |
 *                                               +-------------+
 *                       @endverbatim
 * @note     (4) The SP must be 8-byte aligned in conforming to the Procedure Call Standard for the ARM architecture
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
 *                     In turn, this requires the value of SP to be aligned to 0 modulo 8:
 *                   - (3) By exception handlers, before calling AAPCS-conforming code.
 *                   - (4) By OS/RTOS/run-time system code, before giving control to an application.
 *               - (b) Section 2.3.1 corrective steps from the the SP must be 8-byte aligned on entry
 *                     to AAPCS-conforming functions advisory note also states.
 *                     " This requirement extends to operating systems and run-time code for all architecture versions
 *                     prior to ARMV7 and to the A, R and M architecture profiles thereafter. Special considerations
 *                     associated with ARMV7M are discussed in section 2.3.3"
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
 * @note     (5) Exception Return Behavior(EXEC_RETURN)
 *               0xFFFFFFFD  Return to Thread mode, exception return uses non-floating point state
 *                           from the PSP and execution uses PSP after return.
 *                @n
 *               Non-floating point state is set at stack initialization. If FPU is enabled and a FP
 *               instruction is used, the state will change dynamically allowing to save the FP
 *               registers on-demand.
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

  *(--p_stk) = (CPU_STK)0xFFFFFFFDuL;                           // R14: EXEC_RETURN; See Note 5

  //                                                               Remaining registers saved on process stack
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
 *                                               OSTaskSwHook()
 *
 * @brief    Allows you to perform other operations during a context switch. This function is called
 *           when a task switch is performed.
 *
 * @note     (1) Interrupts are disabled during this call.
 *
 * @note     (2) It is assumed that the global pointer 'OSTCBHighRdyPtr' points to the TCB of the task
 *               that will be 'switched in' (i.e. the highest priority task) and, 'OSTCBCurPtr' points
 *               to the task being switched out (i.e. the preempted task).
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
