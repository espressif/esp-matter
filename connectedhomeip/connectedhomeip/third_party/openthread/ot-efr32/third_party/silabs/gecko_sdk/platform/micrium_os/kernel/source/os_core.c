/***************************************************************************//**
 * @file
 * @brief Kernel - Core Functions
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
#include  "../include/os.h"
#include  "os_priv.h"

#include  <em_core.h>

#if ((OS_CFG_TICK_EN == DEF_ENABLED) || (OS_CFG_SCHED_ROUND_ROBIN_EN == DEF_ENABLED))
#include  <sl_sleeptimer.h>
#endif

#include  <common/include/rtos_prio.h>
#include  <common/include/toolchains.h>

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const CPU_CHAR *os_core__c = "$Id: $";
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  DEF_INT_OS_TICK_MAX_VAL                (UINT64_C(0x100000000))

/********************************************************************************************************
 *                                       DEFAULT RUNTIME CONFIGURATION
 *******************************************************************************************************/

// Default Stacks, Pool Size, Stack Limit and Tasks.
#define  OS_INIT_CFG_DFLT                   { \
    .ISR =                                    \
    {                                         \
      .StkBasePtr = DEF_NULL,                 \
      .StkSize = 256u                         \
    },                                        \
    .MsgPoolSize = 100u,                      \
    .TaskStkLimit = 10u,                      \
    .StatTaskCfg =                            \
    {                                         \
      .StkBasePtr = DEF_NULL,                 \
      .StkSize = 256u,                        \
      .Prio = KERNEL_STAT_TASK_PRIO_DFLT,     \
      .RateHz = 10u                           \
    },                                        \
    .TmrTaskCfg =                             \
    {                                         \
      .StkBasePtr = DEF_NULL,                 \
      .StkSize = 256u,                        \
      .Prio = KERNEL_TMR_TASK_PRIO_DFLT,      \
      .RateHz = 10u                           \
    },                                        \
    .MemSeg = DEF_NULL,                       \
    .TickRate = 1000u                         \
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  OS_CFG_COMPAT_INIT
#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
// Kernel configuration.
const OS_INIT_CFG  OS_InitCfgDflt = OS_INIT_CFG_DFLT;
static OS_INIT_CFG OS_InitCfg = OS_INIT_CFG_DFLT;
#else
// Kernel configuration.
extern const OS_INIT_CFG OS_InitCfg;
#endif
#endif

#if  (OS_CFG_SCHED_ROUND_ROBIN_EN == DEF_ENABLED)
sl_sleeptimer_timer_handle_t OSRoundRobinTimer;
OS_TCB                       *OSRoundRobinCurTCB;
#endif

#if (OS_CFG_TICK_EN == DEF_ENABLED)
CPU_INT32U OSDelayMaxMilli = 0u;
OS_TICK OSDelayMaxTick = 0u;
CPU_INT32U OS_SleeptimerFrequency_Hz = 0u;
#endif

#if (OS_CFG_ERRNO_EN == 1)
// Errno Variable
int micriumos_errno;
#endif

/*
 *********************************************************************************************************
 *                                        HOOK WEAK REFERENCES
 *********************************************************************************************************
 */

/*****************************************************************************************************//**
 *                                               OSIdleEnterHook()
 *
 * @brief    Allows performing other operations during an Idle enter.
 *
 * @note     (1) Allow the MCU to be put in sleep mode.
 *******************************************************************************************************/
__WEAK void OSIdleEnterHook(void);

/*****************************************************************************************************//**
 *                                               OSIdleExitHook()
 *
 * @brief    Allows performing other operations during an Idle exit.
 *******************************************************************************************************/
__WEAK void OSIdleExitHook(void);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           OS_ConfigureISRStk()
 *
 * @brief    Configure the stack used for ISRs, if available.
 *
 * @param    p_stk_base_ptr  Pointer to the base of the buffer used as the stack.
 *
 * @param    stk_size        Size of the stack, in CPU_STK elements.
 *
 * @note     (1) This function is optional. If it is called, it must be called before OSInit().
 *               If it is not called, default values will be used.
 *******************************************************************************************************/
#if (!defined(OS_CFG_COMPAT_INIT) \
  && (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED))
void OS_ConfigureISRStk(CPU_STK      *p_stk_base_ptr,
                        CPU_STK_SIZE stk_size)
{
  RTOS_ASSERT_CRITICAL((OSInitialized == DEF_FALSE), RTOS_ERR_ALREADY_INIT,; );

  OS_InitCfg.ISR.StkBasePtr = p_stk_base_ptr;
  OS_InitCfg.ISR.StkSize = stk_size;
}
#endif

/****************************************************************************************************//**
 *                                           OS_ConfigureMemSeg()
 *
 * @brief    Configure the memory segment used by the Kernel.
 *
 * @param    p_mem_seg   Pointer to the memory segment in which the kernel data will be allocated.
 *
 * @note     (1) This function is optional. If it is called, it must be called before OSInit().
 *               If it is not called, default values will be used.
 *******************************************************************************************************/
#if (!defined(OS_CFG_COMPAT_INIT) \
  && (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED))
void OS_ConfigureMemSeg(MEM_SEG *p_mem_seg)
{
  RTOS_ASSERT_CRITICAL((OSInitialized == DEF_FALSE), RTOS_ERR_ALREADY_INIT,; );

  OS_InitCfg.MemSeg = p_mem_seg;
}
#endif

/****************************************************************************************************//**
 *                                           OS_ConfigureMsgPoolSize()
 *
 * @brief    Configure the Kernel message pool size.
 *
 * @param    msg_pool_size   Number of messages the kernel will manage. Shared between task message
 *                           queues and regular message queues.
 *
 * @note     (1) This function is optional. If it is called, it must be called before OSInit().
 *               If it is not called, default values will be used.
 *******************************************************************************************************/
#if (!defined(OS_CFG_COMPAT_INIT) \
  && (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED))
void OS_ConfigureMsgPoolSize(OS_MSG_SIZE msg_pool_size)
{
  RTOS_ASSERT_CRITICAL((OSInitialized == DEF_FALSE), RTOS_ERR_ALREADY_INIT,; );

  OS_InitCfg.MsgPoolSize = msg_pool_size;
}
#endif

/****************************************************************************************************//**
 *                                           OS_ConfigureStkLimit()
 *
 * @brief    Configure the application stack limit.
 *
 * @param    task_stk_limit  Stack limit in percentage to empty.
 *
 * @note     (1) This function is optional. If it is called, it must be called before OSInit().
 *               If it is not called, default values will be used.
 *******************************************************************************************************/
#if (!defined(OS_CFG_COMPAT_INIT) \
  && (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED))
void OS_ConfigureStkLimit(CPU_STK_SIZE task_stk_limit)
{
  RTOS_ASSERT_CRITICAL((OSInitialized == DEF_FALSE), RTOS_ERR_ALREADY_INIT,; );

  OS_InitCfg.TaskStkLimit = task_stk_limit;
}
#endif

/*****************************************************************************************************//**
 *                                           OS_ConfigureStatTask()
 *
 * @brief    If enabled, configure the Statistics Task.
 *
 * @param    p_stat_task_cfg     Pointer to the Statistics Task configuration.
 *
 * @note     (1) This function is optional. If it is called, it must be called before OSInit().
 *               If it is not called, default values will be used.
 *******************************************************************************************************/
#if (!defined(OS_CFG_COMPAT_INIT)                           \
  && (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED) \
  && (OS_CFG_STAT_TASK_EN == DEF_ENABLED))
void OS_ConfigureStatTask(OS_TASK_CFG *p_stat_task_cfg)
{
  RTOS_ASSERT_CRITICAL((OSInitialized == DEF_FALSE), RTOS_ERR_ALREADY_INIT,; );

  OS_ASSERT_DBG_NO_ERR((p_stat_task_cfg != DEF_NULL), RTOS_ERR_NULL_PTR,; );

  OS_InitCfg.StatTaskCfg = *p_stat_task_cfg;
}
#endif

/*****************************************************************************************************//**
 *                                           OS_ConfigureTmrTask()
 *
 * @brief    If enabled, configure the Timer Management Task.
 *
 * @param    p_tmr_task_cfg  Pointer to the Timer Management Task configuration.
 *
 * @note     (1) This function is optional. If it is called, it must be called before OSInit().
 *               If it is not called, default values will be used.
 *******************************************************************************************************/
#if (!defined(OS_CFG_COMPAT_INIT)                           \
  && (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED) \
  && (OS_CFG_TMR_EN == DEF_ENABLED))
void OS_ConfigureTmrTask(OS_TASK_CFG *p_tmr_task_cfg)
{
  RTOS_ASSERT_CRITICAL((OSInitialized == DEF_FALSE), RTOS_ERR_ALREADY_INIT,; );

  OS_ASSERT_DBG_NO_ERR((p_tmr_task_cfg != DEF_NULL), RTOS_ERR_NULL_PTR,; );

  OS_InitCfg.TmrTaskCfg = *p_tmr_task_cfg;
}
#endif

/*****************************************************************************************************//**
 *                                          OS_ConfigureTickRate()
 *
 * @brief    If enabled, configure the OS tick rate.
 *
 * @param    tick_rate  Tick rate, in hertz.
 *
 * @note     (1) This function is optional. If it is called, it must be called before OSInit().
 *               If it is not called, default values will be used.
 *******************************************************************************************************/
#if (!defined(OS_CFG_COMPAT_INIT)                           \
  && (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED) \
  && (OS_CFG_TICK_EN == DEF_ENABLED)) // TODO re-add compat
void OS_ConfigureTickRate(OS_RATE_HZ tick_rate)
{
  RTOS_ASSERT_CRITICAL((OSInitialized == DEF_FALSE), RTOS_ERR_ALREADY_INIT,; );

  OS_ASSERT_DBG_NO_ERR((tick_rate != 0), RTOS_ERR_INVALID_ARG,; );

  OS_InitCfg.TickRate = tick_rate;
}
#endif

/****************************************************************************************************//**
 *                                                   OSInit()
 *
 * @brief    Initializes the internals of the Kernel and MUST be called before creating any Kernel
 *           object and before calling OSStart().
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error
 *                   code(s) from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                       - RTOS_ERR_SEG_OVF
 *                       - RTOS_ERR_INVALID_CFG
 *
 * @note     (1) This function MUST be called AFTER Common's Mem_Init().
 *******************************************************************************************************/
void OSInit(RTOS_ERR *p_err)
{
#ifdef  OS_CFG_COMPAT_INIT
#if (OS_CFG_ISR_STK_SIZE > 0u)
  CPU_STK      *p_stk;
  CPU_STK_SIZE size;
#endif
#else
  CPU_STK      *p_stk;
  CPU_STK_SIZE size;
#endif
#if (OS_CFG_TICK_EN == DEF_ENABLED)
  sl_status_t status;
#endif

  //                                                               Validate 'p_err'
  OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  //                                                               Allocate Objects and Stacks.
#ifndef OS_CFG_COMPAT_INIT
  //                                                               Allocate ISR Stack.
  if (OS_InitCfg.ISR.StkSize > 0u) {
    if (OS_InitCfg.ISR.StkBasePtr == DEF_NULL) {
      OSCfg_ISRStk = (CPU_STK *)Mem_SegAlloc("Kernel's ISR Stack",
                                             OS_InitCfg.MemSeg,
                                             OS_InitCfg.ISR.StkSize * sizeof(CPU_STK),
                                             p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return;
      }
    } else {
      OSCfg_ISRStk = OS_InitCfg.ISR.StkBasePtr;
    }

    OSCfg_ISRStkBasePtr = OSCfg_ISRStk;
    OSCfg_ISRStkSize = OS_InitCfg.ISR.StkSize;
    OSCfg_ISRStkSizeRAM = OS_InitCfg.ISR.StkSize * sizeof(CPU_STK);
    OSCfg_DataSizeRAM += OSCfg_ISRStkSizeRAM;
  }

#if (OS_MSG_EN == DEF_ENABLED)                                  // Message Queue.

  if (OS_InitCfg.MsgPoolSize > 0u) {
    OSCfg_MsgPool = (OS_MSG *)Mem_SegAlloc("Kernel's Msg Pool",
                                           OS_InitCfg.MemSeg,
                                           OS_InitCfg.MsgPoolSize * sizeof(OS_MSG),
                                           p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    OSCfg_MsgPoolBasePtr = OSCfg_MsgPool;
    OSCfg_MsgPoolSize = OS_InitCfg.MsgPoolSize;
    OSCfg_MsgPoolSizeRAM = OS_InitCfg.MsgPoolSize * sizeof(OS_MSG);
    OSCfg_DataSizeRAM += OSCfg_MsgPoolSizeRAM;
  }
#endif

#if (OS_CFG_STAT_TASK_EN == DEF_ENABLED)                        // Statistic Task's Stack.
  if (OS_InitCfg.StatTaskCfg.StkSize > 0u) {
    if (OS_InitCfg.StatTaskCfg.StkBasePtr == DEF_NULL) {
      OSCfg_StatTaskStk = (CPU_STK *)Mem_SegAlloc("Kernel's Stat Task Stack",
                                                  OS_InitCfg.MemSeg,
                                                  OS_InitCfg.StatTaskCfg.StkSize * sizeof(CPU_STK),
                                                  p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return;
      }
    } else {
      OSCfg_StatTaskStk = OS_InitCfg.StatTaskCfg.StkBasePtr;
    }

    OSCfg_StatTaskPrio = OS_InitCfg.StatTaskCfg.Prio;
    OSCfg_StatTaskRate_Hz = OS_InitCfg.StatTaskCfg.RateHz;
    OSCfg_StatTaskStkBasePtr = OSCfg_StatTaskStk;
    OSCfg_StatTaskStkLimit = ((OS_InitCfg.StatTaskCfg.StkSize * OS_InitCfg.TaskStkLimit) / 100u);
    OSCfg_StatTaskStkSize = OS_InitCfg.StatTaskCfg.StkSize;
    OSCfg_StatTaskStkSizeRAM = OS_InitCfg.StatTaskCfg.StkSize * sizeof(CPU_STK);
    OSCfg_DataSizeRAM += OSCfg_StatTaskStkSizeRAM;
  }
#endif

#if (OS_CFG_TMR_EN == DEF_ENABLED)                              // Timer Manager Task's Stack.
  if (OS_InitCfg.TmrTaskCfg.StkSize > 0u) {
    if (OS_InitCfg.TmrTaskCfg.StkBasePtr == DEF_NULL) {
      OSCfg_TmrTaskStk = (CPU_STK *)Mem_SegAlloc("Kernel's Timer Task Stack",
                                                 OS_InitCfg.MemSeg,
                                                 OS_InitCfg.TmrTaskCfg.StkSize * sizeof(CPU_STK),
                                                 p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return;
      }
    } else {
      OSCfg_TmrTaskStk = OS_InitCfg.TmrTaskCfg.StkBasePtr;
    }

    OSCfg_TmrTaskPrio = OS_InitCfg.TmrTaskCfg.Prio;
    OSCfg_TmrTaskStkBasePtr = OSCfg_TmrTaskStk;
    OSCfg_TmrTaskStkLimit = ((OS_InitCfg.TmrTaskCfg.StkSize * OS_InitCfg.TaskStkLimit) / 100u);
    OSCfg_TmrTaskStkSize = OS_InitCfg.TmrTaskCfg.StkSize;
    OSCfg_TmrTaskStkSizeRAM = OS_InitCfg.TmrTaskCfg.StkSize * sizeof(CPU_STK);
    OSCfg_TmrTaskRate_Hz = OS_InitCfg.TmrTaskCfg.RateHz;
    OSCfg_DataSizeRAM += OSCfg_TmrTaskStkSizeRAM;
  }
#endif

#if (OS_CFG_TICK_EN == DEF_ENABLED)
  OSCfg_TickRate_Hz = OS_InitCfg.TickRate;
#endif
#endif

  OSInitHook();                                                 // Call port specific initialization code

#if (OS_CFG_TICK_EN == DEF_ENABLED)
  status = sl_sleeptimer_init();
  if (status != SL_STATUS_OK) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
    return;
  }

  OS_SleeptimerFrequency_Hz = (CPU_INT32U)sl_sleeptimer_get_timer_frequency();

  OS_ASSERT_DBG_ERR_SET((OSCfg_TickRate_Hz <= OS_SleeptimerFrequency_Hz), *p_err, RTOS_ERR_INVALID_CFG,; )

  if (OSCfg_TickRate_Hz == OS_SleeptimerFrequency_Hz) {
    OSDelayMaxTick =  0u;                                       // There is no limit for delay ticks
  } else {
    OSDelayMaxTick = (DEF_INT_OS_TICK_MAX_VAL / OS_SleeptimerFrequency_Hz) * OSCfg_TickRate_Hz;
  }

  if (OSCfg_TickRate_Hz <= 1000u) {
    OSDelayMaxMilli =  0u;                                      // There is no limit for delay milliseconds
  } else {
    OSDelayMaxMilli = (DEF_INT_OS_TICK_MAX_VAL / OSCfg_TickRate_Hz) * 1000u;
  }
#endif

  OSRunning = OS_STATE_OS_STOPPED;                              // Indicate that multitasking not started

  OSSchedLockNestingCtr = 0u;                                   // Clear the scheduling lock counter

  OSTCBCurPtr = DEF_NULL;                                       // Initialize OS_TCB pointers to a known state
  OSTCBHighRdyPtr = DEF_NULL;

  OSPrioCur = 0u;                                               // Initialize priority variables to a known state
  OSPrioHighRdy = 0u;

#if (OS_CFG_SCHED_LOCK_TIME_MEAS_EN == DEF_ENABLED)
  OSSchedLockTimeBegin = 0u;
  OSSchedLockTimeMax = 0u;
  OSSchedLockTimeMaxCur = 0u;
#endif

#ifdef OS_SAFETY_CRITICAL_IEC61508
  OSSafetyCriticalStartFlag = DEF_FALSE;
#endif

#if (OS_CFG_SCHED_ROUND_ROBIN_EN == DEF_ENABLED)
  OSSchedRoundRobinEn = DEF_FALSE;
  OSSchedRoundRobinDfltTimeQuanta = OSTimeTickRateHzGet(p_err) / 10u;
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
#endif

  //                                                               Clear exception stack for stack checking.
#ifdef  OS_CFG_COMPAT_INIT
#if (OS_CFG_ISR_STK_SIZE > 0u)
  p_stk = OSCfg_ISRStkBasePtr;
  if (p_stk != DEF_NULL) {
    size = OSCfg_ISRStkSize;
    while (size > 0u) {
      size--;
      *p_stk = 0u;
      p_stk++;
    }
#if (OS_CFG_TASK_STK_REDZONE_EN == DEF_ENABLED)                 // Initialize Redzoned ISR stack
    OS_TaskStkRedzoneInit(OSCfg_ISRStkBasePtr, OSCfg_ISRStkSize);
#endif
  }
#endif
#else
  if (OSCfg_ISRStkSize > 0u) {
    p_stk = OSCfg_ISRStkBasePtr;
    if (p_stk != DEF_NULL) {
      size = OSCfg_ISRStkSize;
      while (size > 0u) {
        size--;
        *p_stk = 0u;
        p_stk++;
      }
#if (OS_CFG_TASK_STK_REDZONE_EN == DEF_ENABLED)                 // Initialize Redzoned ISR stack
      OS_TaskStkRedzoneInit(OSCfg_ISRStkBasePtr, OSCfg_ISRStkSize);
#endif
    }
  }
#endif

#if (OS_CFG_APP_HOOKS_EN == DEF_ENABLED)                        // Clear application hook pointers
#if (OS_CFG_TASK_STK_REDZONE_EN == DEF_ENABLED)
  OS_AppRedzoneHitHookPtr = DEF_NULL;
#endif
  OS_AppTaskCreateHookPtr = DEF_NULL;
  OS_AppTaskDelHookPtr = DEF_NULL;
  OS_AppTaskReturnHookPtr = DEF_NULL;

  OS_AppStatTaskHookPtr = DEF_NULL;
  OS_AppTaskSwHookPtr = DEF_NULL;
#endif

#if (OS_CFG_TASK_REG_TBL_SIZE > 0u)
  OSTaskRegNextAvailID = 0u;
#endif

  OS_PrioInit();                                                // Initialize the priority bitmap table

  OS_RdyListInit();                                             // Initialize the Ready List

#if (OS_CFG_FLAG_EN == DEF_ENABLED)                             // Initialize the Event Flag module
#if (OS_CFG_DBG_EN == DEF_ENABLED)
  OSFlagDbgListPtr = DEF_NULL;
  OSFlagQty = 0u;
#endif
#endif

#if (OS_MSG_EN == DEF_ENABLED)                                  // Initialize the free list of OS_MSGs
  if (OSCfg_MsgPoolSize > 0u) {
    OS_MsgPoolInit(p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
  }
#endif

#if (OS_CFG_MUTEX_EN == DEF_ENABLED)                            // Initialize the Mutex Manager module
#if (OS_CFG_DBG_EN == DEF_ENABLED)
  OSMutexDbgListPtr = DEF_NULL;
  OSMutexQty = 0u;
#endif
#endif

#if (OS_CFG_Q_EN == DEF_ENABLED)                                // Initialize the Message Queue Manager module
#if (OS_CFG_DBG_EN == DEF_ENABLED)
  OSQDbgListPtr = DEF_NULL;
  OSQQty = 0u;
#endif
#endif

#if (OS_CFG_SEM_EN == DEF_ENABLED)                              // Initialize the Semaphore Manager module
#if (OS_CFG_DBG_EN == DEF_ENABLED)
  OSSemDbgListPtr = DEF_NULL;
  OSSemQty = 0u;
#endif
#endif

#if defined(OS_CFG_TLS_TBL_SIZE) && (OS_CFG_TLS_TBL_SIZE > 0u)
  OS_TLS_Init(p_err);                                           // Initialize Task Local Storage, before creating tasks
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
#endif

  OS_TaskInit(p_err);                                           // Initialize the task manager
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

#if (OS_CFG_STAT_TASK_EN == DEF_ENABLED)                        // Initialize the Statistic Task
  OS_StatTaskInit(p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
#endif

#if (OS_CFG_TMR_EN == DEF_ENABLED)                              // Initialize the Timer Manager module
  OS_TmrInit(p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
#endif

#if (OS_CFG_DBG_EN == DEF_ENABLED)
  OS_Dbg_Init();
#endif

  OSCfg_Init();

  OSInitialized = DEF_TRUE;                                     // Kernel is initialized
}

/*****************************************************************************************************//**
 *                                               OSIntEnter()
 *
 * @brief    Used in an interrupt service routine (ISR) to trace that you are about to
 *           service an interrupt.
 *
 * @note     (1) You MUST invoke OSIntEnter() and OSIntExit() in pairs. In other words, for every call
 *               to OSIntEnter() at the beginning of the ISR you MUST have a call to OSIntExit() at
 *               the end of the ISR.
 *******************************************************************************************************/
void OSIntEnter(void)
{
  OS_TRACE_ISR_ENTER();
}

/****************************************************************************************************//**
 *                                               OSIntExit()
 *
 * @brief    Traces that you have completed servicing an ISR.
 *
 * @note     (1) You MUST invoke OSIntEnter() and OSIntExit() in pairs. In other words, for every call
 *               to OSIntEnter() at the beginning of the ISR, you MUST have a call to OSIntExit() at the
 *               end of the ISR.
 *******************************************************************************************************/
void OSIntExit(void)
{
#if (OS_CFG_TASK_STK_REDZONE_EN == DEF_ENABLED)
  CPU_BOOLEAN stk_status;

  //                                                               Verify ISR Stack
#ifdef  OS_CFG_COMPAT_INIT
#if (OS_CFG_ISR_STK_SIZE > 0u)
  stk_status = OS_TaskStkRedzoneChk(OSCfg_ISRStkBasePtr, OSCfg_ISRStkSize);
  if (stk_status != DEF_OK) {
    OSRedzoneHitHook(DEF_NULL);
  }
#endif
#else
  if (OSCfg_ISRStkSize > 0u) {
    stk_status = OS_TaskStkRedzoneChk(OSCfg_ISRStkBasePtr, OSCfg_ISRStkSize);
    if (stk_status != DEF_OK) {
      OSRedzoneHitHook(DEF_NULL);
    }
  }
#endif
#endif

  OS_TRACE_ISR_EXIT();
}

/****************************************************************************************************//**
 *                                                   OSSched()
 *
 * @brief    This function is called by other Kernel services to determine whether a new, high
 *           priority task has been made ready to run. This function is invoked by TASK level code and
 *           is not used to reschedule tasks from ISRs (see OSIntExit() for ISR rescheduling).
 *
 * @note     (1) Rescheduling is prevented when the scheduler is locked (see OSSchedLock()).
 *******************************************************************************************************/
void OSSched(void)
{
  OS_PRIO prio_prev;
#if (OS_CFG_TASK_STK_REDZONE_EN == DEF_ENABLED)
  CPU_BOOLEAN stk_status;
#endif
  CORE_DECLARE_IRQ_STATE;
  //                                                               Can't schedule when the kernel is stopped.
  OS_ASSERT_DBG_NO_ERR((OSRunning == OS_STATE_OS_RUNNING), RTOS_ERR_NOT_READY,; );

  if (OSSchedLockNestingCtr > 0u) {                             // Scheduler locked?
    return;                                                     // Yes
  }

  CORE_ENTER_ATOMIC();
  prio_prev     = OSPrioHighRdy;
  OSPrioHighRdy = OS_PrioGetHighest();                          // Find the highest priority ready
  if (OSPrioHighRdy != (OS_CFG_PRIO_MAX)) {                     // Are we going to idle?
    OSTCBHighRdyPtr = OSRdyList[OSPrioHighRdy].HeadPtr;         // No ... get highest priority task ready-to-run
    if (prio_prev == (OS_CFG_PRIO_MAX)) {                       // Were we previously in idle ?
#if OS_CFG_STAT_TASK_EN > 0u
      OS_StatIdleExitNotify();                                  // Yes
#endif
      OSIdleExitHook();
    }

    if (OSTCBHighRdyPtr == OSTCBCurPtr) {                       // Current task still the highest priority?
#if (OS_CFG_TASK_STK_REDZONE_EN == DEF_ENABLED)
      stk_status = OSTaskStkRedzoneChk(DEF_NULL);
      if (stk_status != DEF_OK) {
        OSRedzoneHitHook(OSTCBCurPtr);
      }
#endif
#if (OS_CFG_SCHED_ROUND_ROBIN_EN == DEF_ENABLED)
      if (OSSchedRoundRobinEn) {
        if (OSTCBHighRdyPtr->TimeQuantaCtr == 0u) {
          OS_SchedRoundRobinResetQuanta(OSTCBHighRdyPtr);
        }
        OS_SchedRoundRobinRestartTimer(OSTCBHighRdyPtr);
      }
#endif
      CORE_EXIT_ATOMIC();
      return;
    }

#if (OS_CFG_SCHED_ROUND_ROBIN_EN == DEF_ENABLED)
    if ((OSSchedRoundRobinEn)
        && ((OSRoundRobinCurTCB->TimeQuantaCtr != 0u))) {
      sl_status_t status;
      uint32_t time_remain;
      // Check if round robin timer is currently running, if yes store remaining time
      status = sl_sleeptimer_get_timer_time_remaining(&OSRoundRobinTimer, &time_remain);
      if (status == SL_STATUS_OK) {
        OSRoundRobinCurTCB->TimeQuantaCtr = time_remain;
        sl_sleeptimer_stop_timer(&OSRoundRobinTimer);
      }
      // Check if round robin timer expire while in scheduler
      if (OSRoundRobinCurTCB->TimeQuantaCtr == 0u) {
        // Move current task to tail
        OS_RdyListMoveHeadToTail(&OSRdyList[OSRoundRobinCurTCB->Prio]);
        OSTCBHighRdyPtr = OSRdyList[OSPrioHighRdy].HeadPtr;
      }
    }
#endif

#if (OS_CFG_TASK_PROFILE_EN == DEF_ENABLED)
    OSTCBHighRdyPtr->CtxSwCtr++;                                // Inc. # of context switches to this task
#endif
  } else {
    OSTCBHighRdyPtr = DEF_NULL;
  }

#if ((OS_CFG_TASK_PROFILE_EN == DEF_ENABLED) || (OS_CFG_DBG_EN == DEF_ENABLED))
  OSTaskCtxSwCtr++;                                             // Increment context switch counter
#endif

#if defined(OS_CFG_TLS_TBL_SIZE) && (OS_CFG_TLS_TBL_SIZE > 0u)
  OS_TLS_TaskSw();
#endif

  if (OSPrioHighRdy != (OS_CFG_PRIO_MAX)) {
#if OS_CFG_TASK_PROFILE_EN > 0u
    if ((OSTCBCurPtr != OSTCBHighRdyPtr)
        && (OSTCBCurPtr != DEF_NULL)) {
      OSTCBCurPtr->CyclesDelta = OS_TMR_GET_DIFF(OS_TS_GET(), OSTCBCurPtr->CyclesStart);
      OSTCBCurPtr->CyclesTotal += (OS_CYCLES)OSTCBCurPtr->CyclesDelta;
    }
    OSTCBHighRdyPtr->CyclesStart = OS_TS_GET();
#endif
  } else {
#if OS_CFG_STAT_TASK_EN > 0u
    OS_StatIdleEnterNotify();
#endif
#if OS_CFG_TASK_PROFILE_EN > 0u
    if (OSTCBCurPtr != DEF_NULL) {
      OSTCBCurPtr->CyclesStart = OS_TS_GET();
    }
#endif
    OS_TRACE_ON_IDLE_ENTER();
    OSIdleEnterHook();
  }

  OS_TASK_SW();                                                 // Perform a task level context switch
  CORE_EXIT_ATOMIC();

#ifdef OS_TASK_SW_SYNC
  OS_TASK_SW_SYNC();
#endif
}

/****************************************************************************************************//**
 *                                               OSSchedLock()
 *
 * @brief    Prevents rescheduling from taking place, allowing your application to prevent context
 *           switches until you are ready to permit context switching.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s)
 *                   from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_WOULD_OVF
 *                       - RTOS_ERR_NOT_READY
 *
 * @note     (1) You MUST invoke OSSchedLock() and OSSchedUnlock() in pairs. In other words, for every
 *               call to OSSchedLock(), you MUST have a call to OSSchedUnlock().
 *******************************************************************************************************/
void OSSchedLock(RTOS_ERR *p_err)
{
  CORE_DECLARE_IRQ_STATE;

  OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  //                                                               Not allowed to call from an ISR
  OS_ASSERT_DBG_ERR_SET((!CORE_InIrqContext()), *p_err, RTOS_ERR_ISR,; );

  //                                                               Make sure kernel is running.
  if (OSRunning != OS_STATE_OS_RUNNING) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
    return;
  }

  if (OSSchedLockNestingCtr >= 250u) {                          // Prevent OSSchedLockNestingCtr overflowing
    RTOS_ERR_SET(*p_err, RTOS_ERR_WOULD_OVF);
    return;
  }

  CORE_ENTER_ATOMIC();
  OSSchedLockNestingCtr++;                                      // Increment lock nesting level
#if (OS_CFG_SCHED_LOCK_TIME_MEAS_EN == DEF_ENABLED)
  OS_SchedLockTimeMeasStart();
#endif
  CORE_EXIT_ATOMIC();

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                               OSSchedUnlock()
 *
 * @brief    Re-allows rescheduling.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s)
 *                   from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_INVALID_STATE
 *                       - RTOS_ERR_OS_SCHED_LOCKED
 *
 * @note     (1) You MUST invoke OSSchedLock() and OSSchedUnlock() in pairs. In other words, for every
 *               call to OSSchedLock(), you MUST have a call to OSSchedUnlock().
 *******************************************************************************************************/
void OSSchedUnlock(RTOS_ERR *p_err)
{
  CORE_DECLARE_IRQ_STATE;

  OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  //                                                               Not allowed to call from an ISR
  OS_ASSERT_DBG_ERR_SET((!CORE_InIrqContext()), *p_err, RTOS_ERR_ISR,; );

  //                                                               Make sure kernel is running.
  if (OSRunning != OS_STATE_OS_RUNNING) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
    return;
  }

  if (OSSchedLockNestingCtr == 0u) {                            // See if the scheduler is locked
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    return;
  }

  CORE_ENTER_ATOMIC();
  OSSchedLockNestingCtr--;                                      // Decrement lock nesting level
  if (OSSchedLockNestingCtr > 0u) {
    CORE_EXIT_ATOMIC();                                         // Scheduler is still locked
    RTOS_ERR_SET(*p_err, RTOS_ERR_OS_SCHED_LOCKED);
    return;
  }

#if (OS_CFG_SCHED_LOCK_TIME_MEAS_EN == DEF_ENABLED)
  OS_SchedLockTimeMeasStop();
#endif

  CORE_EXIT_ATOMIC();                                           // Scheduler should be re-enabled
  OSSched();                                                    // Run the scheduler
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                           OSSchedRoundRobinCfg()
 *
 * @brief    Changes the round-robin scheduling parameters.
 *
 * @param    en                  Determines if the round-robin will be used:
 *                                   - DEF_ENABLED     Round-robin scheduling is enabled.
 *                                   - DEF_DISABLED    Round-robin scheduling is disabled.
 *
 * @param    dflt_time_quanta    Default number of ticks between time slices.
 *                               A value of 0 assumes OSCfg_TickRate_Hz / 10.
 *
 * @param    p_err               Pointer to the variable that will receive one of the following
 *                               error code(s) from this function:
 *                                   - RTOS_ERR_NONE
 *******************************************************************************************************/
#if (OS_CFG_SCHED_ROUND_ROBIN_EN == DEF_ENABLED)
void OSSchedRoundRobinCfg(CPU_BOOLEAN en,
                          OS_TICK     dflt_time_quanta,
                          RTOS_ERR    *p_err)
{
  sl_status_t status;
  CORE_DECLARE_IRQ_STATE;

  OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  CORE_ENTER_ATOMIC();

  if (dflt_time_quanta > 0u) {
    OSSchedRoundRobinDfltTimeQuanta = dflt_time_quanta;
  } else {
    OSSchedRoundRobinDfltTimeQuanta = (OS_TICK)(OSTimeTickRateHzGet(p_err) / 10u);
  }

  if ((en != DEF_ENABLED)
      && (OSSchedRoundRobinEn == DEF_TRUE)) {
    OSSchedRoundRobinEn = DEF_FALSE;
    status = sl_sleeptimer_get_timer_time_remaining(&OSRoundRobinTimer,
                                                    &OSRoundRobinCurTCB->TimeQuantaCtr);
    if (status != SL_STATUS_OK) {
      OSRoundRobinCurTCB->TimeQuantaCtr = 0u;
    }

    sl_sleeptimer_stop_timer(&OSRoundRobinTimer);

    OSRoundRobinCurTCB = DEF_NULL;
  } else if ((en == DEF_ENABLED)
             && (OSSchedRoundRobinEn == DEF_FALSE)) {
    OSSchedRoundRobinEn = DEF_TRUE;
    OS_SchedRoundRobinResetQuanta(OSTCBCurPtr);
    OS_SchedRoundRobinRestartTimer(OSTCBCurPtr);
  }
  CORE_EXIT_ATOMIC();
}
#endif

/****************************************************************************************************//**
 *                                           OSSchedRoundRobinYield()
 *
 * @brief    Gives up the CPU when a task is finished its execution before its time slice expires.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s)
 *                   from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_NOT_AVAIL
 *                       - RTOS_ERR_NONE_WAITING
 *                       - RTOS_ERR_OS_SCHED_LOCKED
 *                       - RTOS_ERR_INVALID_STATE
 *
 * @note     (1) This function MUST be called from a task.
 *******************************************************************************************************/
#if (OS_CFG_SCHED_ROUND_ROBIN_EN == DEF_ENABLED)
void OSSchedRoundRobinYield(RTOS_ERR *p_err)
{
  OS_RDY_LIST *p_rdy_list;
  sl_status_t status;
  CORE_DECLARE_IRQ_STATE;

  OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  //                                                               Not allowed to call from an ISR
  OS_ASSERT_DBG_ERR_SET((!CORE_InIrqContext()), *p_err, RTOS_ERR_ISR,; );

  //                                                               Not allowed in atomic/critical sections
  OS_ASSERT_DBG_ERR_SET((!CORE_IrqIsDisabled()), *p_err, RTOS_ERR_INVALID_STATE,; );

  if (OSSchedLockNestingCtr > 0u) {                             // Can't yield if the scheduler is locked
    RTOS_ERR_SET(*p_err, RTOS_ERR_OS_SCHED_LOCKED);
    return;
  }

  if (OSSchedRoundRobinEn != DEF_TRUE) {                        // Make sure round-robin has been enabled
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_AVAIL);
    return;
  }

  CORE_ENTER_ATOMIC();
  p_rdy_list = &OSRdyList[OSPrioCur];                           // Can't yield if it's the only task at that priority
  if (p_rdy_list->HeadPtr == p_rdy_list->TailPtr) {
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE_WAITING);
    return;
  }

  status = sl_sleeptimer_stop_timer(&OSRoundRobinTimer);
  if (status != SL_STATUS_OK) {
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
    return;
  }

  p_rdy_list->HeadPtr->TimeQuantaCtr = 0u;                      // TimeQuantaCtr will be reset next time this task is scheduled
  OS_RdyListMoveHeadToTail(p_rdy_list);                         // Move current OS_TCB to the end of the list

  CORE_EXIT_ATOMIC();

  OSSched();                                                    // Run new task
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}
#endif

/****************************************************************************************************//**
 *                                                   OSStart()
 *
 * @brief    Starts the multitasking process which lets the Kernel manage the tasks that you created.
 *           Before you can call OSStart(), you MUST have called OSInit() and you MUST have created
 *           at least one application task.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s)
 *                   from this function:
 *                       - RTOS_ERR_NONE
 *
 * @note     (1) OSStartHighRdy() MUST:
 *               - (a) Call OSTaskSwHook().
 *               - (b) Load the context of the task pointed to by OSTCBHighRdyPtr.
 *               - (c) Execute the task.
 *
 * @note     (2) OSStart() is not supposed to return. If it does, that would be considered a fatal error.
 *******************************************************************************************************/
void OSStart(RTOS_ERR *p_err)
{
  OS_OBJ_QTY kernel_task_cnt;

  OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  OS_ASSERT_DBG_ERR_SET((OSInitialized == DEF_TRUE), *p_err, RTOS_ERR_NOT_INIT,; );

  kernel_task_cnt = 0u;                                         // Calculate the number of kernel tasks
#if (OS_CFG_STAT_TASK_EN == DEF_ENABLED)
  kernel_task_cnt++;
#endif
#if (OS_CFG_TMR_EN == DEF_ENABLED)
  kernel_task_cnt++;
#endif

  //                                                               Make sure at least one application task is created
  OS_ASSERT_DBG_ERR_SET((OSTaskQty > kernel_task_cnt), *p_err, RTOS_ERR_INVALID_CFG,; );

  //                                                               Make sure kernel is not already running
  OS_ASSERT_DBG_ERR_SET((OSRunning == OS_STATE_OS_STOPPED), *p_err, RTOS_ERR_INVALID_STATE,; );

  //                                                               Not allowed in atomic/critical sections
  OS_ASSERT_DBG_ERR_SET((!CORE_IrqIsDisabled()), *p_err, RTOS_ERR_INVALID_STATE,; );

  OSPrioHighRdy = OS_PrioGetHighest();                          // Find the highest priority
  OSPrioCur = OSPrioHighRdy;
  OSTCBHighRdyPtr = OSRdyList[OSPrioHighRdy].HeadPtr;
  OSTCBCurPtr = OSTCBHighRdyPtr;
#ifdef OS_SAFETY_CRITICAL_IEC61508
  OSSafetyCriticalStartFlag = DEF_TRUE;                         // Prevent creation of additional kernel objects
#endif
  OSRunning = OS_STATE_OS_RUNNING;
#if OS_CFG_STAT_TASK_EN > 0u
  OS_StatTickInit();
#endif
  OSStartHighRdy();                                             // Execute target specific code to start task
  RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_OS,; );                      // OSStart() is not supposed to return
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                       DEPRECATED GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               OSVersion()
 *
 * @brief    Returns the version number of the Kernel. The returned value is the Kernel's version
 *           number multiplied by 10000. In other words, version 3.01.02 would be returned as 30102.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s)
 *                   from this function:
 *                       - RTOS_ERR_NONE
 *
 * @return   The version number of the Kernel multiplied by 10000.
 *
 * @note     (1) This function is DEPRECATED and will be removed in a future version of this product.
 *               Instead, use RTOS_Version() or RTOS_VERSION.
 * @deprecated
 *******************************************************************************************************/
CPU_INT16U OSVersion(RTOS_ERR *p_err)
{
  OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  return (OS_VERSION);
}

/***************************************************************************//**
 * This function return the state of the kernel.
 *
 * @return   True, if the kernel is idle,
 *           False, otherwise.
 ******************************************************************************/
CPU_BOOLEAN OSIsIdle(void)
{
  if (OS_PrioGetHighest() != OS_CFG_PRIO_MAX ) {
    return DEF_NO;
  } else {
    return DEF_YES;
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/*****************************************************************************************************//**
 *                                               OS_Pend()
 *
 * @brief    This function is called to place a task in the blocked state waiting for an event to occur.
 *           This function exists because it is common to a number of OSxxxPend() services.
 *
 * @param    p_obj       Pointer to the object to pend on. If there are no object used to pend
 *                       on then the caller must pass a NULL pointer.
 *
 * @param    pending_on  Specifies what the task will be pending on:
 *                           - OS_TASK_PEND_ON_FLAG
 *                           - OS_TASK_PEND_ON_TASK_Q      <- No object (pending for a message sent to
 *                                                            the task)
 *                           - OS_TASK_PEND_ON_MUTEX
 *                           - OS_TASK_PEND_ON_Q
 *                           - OS_TASK_PEND_ON_SEM
 *                           - OS_TASK_PEND_ON_TASK_SEM    <- No object (pending on a signal   sent to
 *                                                            the task)
 *
 * @param    timeout     Amount of time the task will wait for the event to occur.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
void OS_Pend(OS_PEND_OBJ *p_obj,
             OS_STATE    pending_on,
             OS_TICK     timeout)
{
  OS_PEND_LIST *p_pend_list;

  OSTCBCurPtr->PendOn = pending_on;                             // Resource not available, wait until it is
  OSTCBCurPtr->PendStatus = OS_STATUS_PEND_OK;

#if (OS_CFG_SCHED_ROUND_ROBIN_EN == DEF_ENABLED)
  if (OSSchedRoundRobinEn) {
    OSRoundRobinCurTCB->TimeQuantaCtr = 0u;                     // Time quanta counter will be reset later.
    (void)sl_sleeptimer_stop_timer(&OSRoundRobinTimer);
  }
#endif

  OS_TaskBlock(OSTCBCurPtr,                                     // Block the task and add it to the tick list if needed
               timeout);

  if (p_obj != DEF_NULL) {                                      // Add the current task to the pend list ...
    p_pend_list = &p_obj->PendList;                             // ... if there is an object to pend on
    OSTCBCurPtr->PendObjPtr = p_obj;                            // Save the pointer to the object pending on
    OS_PendListInsertPrio(p_pend_list,                          // Insert in the pend list in priority order
                          OSTCBCurPtr);
  } else {
    OSTCBCurPtr->PendObjPtr = DEF_NULL;                         // If no object being pended on, clear the pend object
  }

#if (OS_CFG_DBG_EN == DEF_ENABLED)
  OS_PendDbgNameAdd(p_obj,
                    OSTCBCurPtr);
#endif
}

/****************************************************************************************************//**
 *                                               OS_PendAbort()
 *
 * @brief    This function is called by the OSxxxPendAbort() and OSxxxDel() functions to cancel pending
 *           on an event.
 *
 * @param    p_tcb   Pointer to the OS_TCB of the task that we'll abort the pend for.
 *
 * @param    ts      Timestamp as to when the pend was cancelled.
 *
 * @param    reason  Indicates how the task was readied:
 *                       - OS_STATUS_PEND_DEL      Object pended on was deleted.
 *                       - OS_STATUS_PEND_ABORT    Pend was aborted.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
void OS_PendAbort(OS_TCB    *p_tcb,
                  CPU_TS    ts,
                  OS_STATUS reason)
{
#if (OS_CFG_TICK_EN == DEF_ENABLED)
  sl_status_t status;
#endif
#if (OS_CFG_TS_EN == DEF_DISABLED)
  (void)ts;                                                     // Prevent compiler warning for not using 'ts'
#endif

  switch (p_tcb->TaskState) {
    case OS_TASK_STATE_PEND:
    case OS_TASK_STATE_PEND_TIMEOUT:
#if (OS_MSG_EN == DEF_ENABLED)
      p_tcb->MsgPtr = DEF_NULL;
      p_tcb->MsgSize = 0u;
#endif
#if (OS_CFG_TS_EN == DEF_ENABLED)
      p_tcb->TS = ts;
#endif
      OS_PendListRemove(p_tcb);                                 // Remove task from the pend list

#if (OS_CFG_TICK_EN == DEF_ENABLED)
      if (p_tcb->TaskState == OS_TASK_STATE_PEND_TIMEOUT) {
        status = sl_sleeptimer_stop_timer(&p_tcb->TimerHandle);
        RTOS_ASSERT_CRITICAL((status == SL_STATUS_OK), RTOS_ERR_FAIL,; );
      }
#endif
      OS_RdyListInsert(p_tcb);                                  // Insert the task in the ready list
      p_tcb->TaskState = OS_TASK_STATE_RDY;                     // Task will be ready
      p_tcb->PendStatus = reason;                               // Indicate how the task became ready
      p_tcb->PendOn = OS_TASK_PEND_ON_NOTHING;                  // Indicate no longer pending
      break;

    case OS_TASK_STATE_PEND_SUSPENDED:
    case OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED:
#if (OS_MSG_EN == DEF_ENABLED)
      p_tcb->MsgPtr = DEF_NULL;
      p_tcb->MsgSize = 0u;
#endif
#if (OS_CFG_TS_EN == DEF_ENABLED)
      p_tcb->TS = ts;
#endif
      OS_PendListRemove(p_tcb);                                 // Remove task from the pend list

#if (OS_CFG_TICK_EN == DEF_ENABLED)
      if (p_tcb->TaskState == OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED) {
        status = sl_sleeptimer_stop_timer(&p_tcb->TimerHandle);
        RTOS_ASSERT_CRITICAL((status == SL_STATUS_OK), RTOS_ERR_FAIL,; );
      }
#endif
      p_tcb->TaskState = OS_TASK_STATE_SUSPENDED;               // Task needs to remain suspended
      p_tcb->PendStatus = reason;                               // Indicate how the task became ready
      p_tcb->PendOn = OS_TASK_PEND_ON_NOTHING;                  // Indicate no longer pending
      break;

    case OS_TASK_STATE_RDY:                                     // Cannot cancel a pend when a task is in these states.
    case OS_TASK_STATE_DLY:
    case OS_TASK_STATE_SUSPENDED:
    case OS_TASK_STATE_DLY_SUSPENDED:
      break;

    case OS_TASK_STATE_DEL:
    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_OS,; );
  }
}

/****************************************************************************************************//**
 *                                           OS_PendDbgNameAdd()
 *
 * @brief    Add pointers to ASCII 'names' of objects so they can easily be displayed using a Kernel
 *           aware tool.
 *
 * @param    p_obj   Pointer to the object being pended on.
 *
 * @param    p_tcb   Pointer to the OS_TCB of the task pending on the object.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
#if (OS_CFG_DBG_EN == DEF_ENABLED)
void OS_PendDbgNameAdd(OS_PEND_OBJ *p_obj,
                       OS_TCB      *p_tcb)
{
  OS_PEND_LIST *p_pend_list;
  OS_TCB       *p_tcb1;

  if (p_obj != DEF_NULL) {
    p_tcb->DbgNamePtr = p_obj->NamePtr;                         // Task pending on this object ... save name in TCB
    p_pend_list = &p_obj->PendList;                             // Find name of HP task pending on this object ...
    p_tcb1 = p_pend_list->HeadPtr;
    p_obj->DbgNamePtr = p_tcb1->NamePtr;                        // ... Save in object
  } else {
    switch (p_tcb->PendOn) {
      case OS_TASK_PEND_ON_TASK_Q:
        p_tcb->DbgNamePtr = (CPU_CHAR *)((void *)"Task Q");
        break;

      case OS_TASK_PEND_ON_TASK_SEM:
        p_tcb->DbgNamePtr = (CPU_CHAR *)((void *)"Task Sem");
        break;

      default:
        p_tcb->DbgNamePtr = (CPU_CHAR *)((void *)" ");
        break;
    }
  }
}

/****************************************************************************************************//**
 *                                           OS_PendDbgNameRemove()
 *
 * @brief    Remove pointers to ASCII 'names' of objects so they can easily be displayed using a Kernel
 *           aware tool.
 *
 * @param    p_obj   Pointer to the object being pended on.
 *
 * @param    p_tcb   Pointer to the OS_TCB of the task pending on the object.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
void OS_PendDbgNameRemove(OS_PEND_OBJ *p_obj,
                          OS_TCB      *p_tcb)
{
  OS_PEND_LIST *p_pend_list;
  OS_TCB       *p_tcb1;

  p_tcb->DbgNamePtr = (CPU_CHAR *)((void *)" ");                // Remove name of object pended on for readied task

  if (p_obj != DEF_NULL) {
    p_pend_list = &p_obj->PendList;
    p_tcb1 = p_pend_list->HeadPtr;
    if (p_tcb1 != DEF_NULL) {                                   // Find name of HP task pending on this object ...
      p_obj->DbgNamePtr = p_tcb1->NamePtr;                      // ... Save in object
    } else {
      p_obj->DbgNamePtr = (CPU_CHAR *)((void *)" ");            // Or no other task is pending on object
    }
  }
}
#endif

/****************************************************************************************************//**
 *                                           OS_PendListChangePrio()
 *
 * @brief    This function is called to change the position of a task waiting in a pend list. The
 *           strategy used is to remove the task from the pend list and add it again using its changed
 *           priority.
 *
 * @param    p_tcb   Pointer to the TCB of the task to move.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *
 * @note     (2) It's assumed that the TCB contains the NEW priority in its .Prio field.
 *******************************************************************************************************/
void OS_PendListChangePrio(OS_TCB *p_tcb)
{
  OS_PEND_LIST *p_pend_list;
  OS_PEND_OBJ  *p_obj;

  p_obj = p_tcb->PendObjPtr;                                    // Get pointer to pend list
  p_pend_list = &p_obj->PendList;

  if (p_pend_list->HeadPtr->PendNextPtr != DEF_NULL) {          // Only move if multiple entries in the list
    OS_PendListRemove(p_tcb);                                   // Remove entry from current position
    p_tcb->PendObjPtr = p_obj;
    OS_PendListInsertPrio(p_pend_list,                          // INSERT it back in the list
                          p_tcb);
  }
}

/****************************************************************************************************//**
 *                                               OS_PendListInit()
 *
 * @brief    This function is called to initialize the fields of an OS_PEND_LIST.
 *
 * @param    p_pend_list     Pointer to an OS_PEND_LIST.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
void OS_PendListInit(OS_PEND_LIST *p_pend_list)
{
  p_pend_list->HeadPtr = DEF_NULL;
  p_pend_list->TailPtr = DEF_NULL;
#if (OS_CFG_DBG_EN == DEF_ENABLED)
  p_pend_list->NbrEntries = 0u;
#endif
}

/****************************************************************************************************//**
 *                                           OS_PendListInsertPrio()
 *
 * @brief    This function is called to place an OS_TCB entry in a linked list based on its priority.
 *           The highest priority being placed at the head of the list. The TCB is assumed to contain
 *           the priority of the task in its .Prio field.
 *           @verbatim
 *           CASE 0: Insert in an empty list.
 *
 *               OS_PEND_LIST
 *               +---------------+
 *               | TailPtr       |-> 0
 *               +---------------+
 *               | HeadPtr       |-> 0
 *               +---------------+
 *               | NbrEntries=0  |
 *               +---------------+
 *
 *           CASE 1: Insert BEFORE or AFTER an OS_TCB
 *
 *               OS_PEND_LIST
 *               +--------------+      OS_TCB
 *               | TailPtr      |-+--> +--------------+
 *               +--------------+ |    | PendNextPtr  |->0
 *               | HeadPtr      |-/    +--------------+
 *               +--------------+   0<-| PendPrevPtr  |
 *               | NbrEntries=1 |      +--------------+
 *               +--------------+      |              |
 *                                     +--------------+
 *                                     |              |
 *                                     +--------------+
 *
 *               OS_PEND_LIST
 *               +--------------+
 *               | TailPtr      |---------------------------------------------+
 *               +--------------+      OS_TCB               OS_TCB            |    OS_TCB
 *               | HeadPtr      |----> +--------------+     +--------------+  +-> +--------------+
 *               +--------------+      | PendNextPtr  |<----| PendNextPtr  | .... | PendNextPtr  |->0
 *               | NbrEntries=N |      +--------------+     +--------------+      +--------------+
 *               +--------------+   0<-| PendPrevPtr  |<----| PendPrevPtr  | .... | PendPrevPtr  |
 *                                     +--------------+     +--------------+      +--------------+
 *                                     |              |     |              |      |              |
 *                                     +--------------+     +--------------+      +--------------+
 *                                     |              |     |              |      |              |
 *                                     +--------------+     +--------------+      +--------------+
 *           @endverbatim
 *
 * @param    p_pend_list     Pointer to the OS_PEND_LIST where the OS_TCB entry will be inserted.
 *
 * @param    p_tcb           The OS_TCB to insert in the list.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
void OS_PendListInsertPrio(OS_PEND_LIST *p_pend_list,
                           OS_TCB       *p_tcb)
{
  OS_PRIO prio;
  OS_TCB  *p_tcb_next;

  prio = p_tcb->Prio;                                           // Obtain the priority of the task to insert

  if (p_pend_list->HeadPtr == DEF_NULL) {                       // CASE 0: Insert when there are no entries
#if (OS_CFG_DBG_EN == DEF_ENABLED)
    p_pend_list->NbrEntries = 1u;                               // This is the first entry
#endif
    p_tcb->PendNextPtr = DEF_NULL;                              // No other OS_TCBs in the list
    p_tcb->PendPrevPtr = DEF_NULL;
    p_pend_list->HeadPtr = p_tcb;
    p_pend_list->TailPtr = p_tcb;
  } else {
#if (OS_CFG_DBG_EN == DEF_ENABLED)
    p_pend_list->NbrEntries++;                                  // CASE 1: One more OS_TCBs in the list
#endif
    p_tcb_next = p_pend_list->HeadPtr;
    while (p_tcb_next != DEF_NULL) {                            // Find the position where to insert
      if (prio < p_tcb_next->Prio) {
        break;                                                  // Found! ... insert BEFORE current
      } else {
        p_tcb_next = p_tcb_next->PendNextPtr;                   // Not Found, follow the list
      }
    }
    if (p_tcb_next == DEF_NULL) {                               // TCB to insert is lowest in priority
      p_tcb->PendNextPtr = DEF_NULL;                            // ... insert at the tail.
      p_tcb->PendPrevPtr = p_pend_list->TailPtr;
      p_tcb->PendPrevPtr->PendNextPtr = p_tcb;
      p_pend_list->TailPtr = p_tcb;
    } else {
      if (p_tcb_next->PendPrevPtr == DEF_NULL) {                // Is new TCB highest priority?
        p_tcb->PendNextPtr = p_tcb_next;                        // Yes, insert as new Head of list
        p_tcb->PendPrevPtr = DEF_NULL;
        p_tcb_next->PendPrevPtr = p_tcb;
        p_pend_list->HeadPtr = p_tcb;
      } else {                                                  // No, insert in between two entries
        p_tcb->PendNextPtr = p_tcb_next;
        p_tcb->PendPrevPtr = p_tcb_next->PendPrevPtr;
        p_tcb->PendPrevPtr->PendNextPtr = p_tcb;
        p_tcb_next->PendPrevPtr = p_tcb;
      }
    }
  }
}

/****************************************************************************************************//**
 *                                           OS_PendListRemove()
 *
 * @brief    This function is called to remove a task from a pend list knowing its TCB.
 *           @verbatim
 *           CASE 0: OS_PEND_LIST list is empty, nothing to do.
 *
 *           CASE 1: Only 1 OS_TCB in the list.
 *
 *               OS_PEND_LIST
 *               +--------------+      OS_TCB
 *               | TailPtr      |-+--> +--------------+
 *               +--------------+ |    | PendNextPtr  |->0
 *               | HeadPtr      |-/    +--------------+
 *               +--------------+   0<-| PendPrevPtr  |
 *               | NbrEntries=1 |      +--------------+
 *               +--------------+      |              |
 *                                     +--------------+
 *                                     |              |
 *                                     +--------------+
 *
 *           CASE N: Two or more OS_TCBs in the list.
 *
 *               OS_PEND_LIST
 *               +--------------+
 *               | TailPtr      |---------------------------------------------+
 *               +--------------+      OS_TCB               OS_TCB            |   OS_TCB
 *               | HeadPtr      |----> +--------------+     +--------------+  +-> +--------------+
 *               +--------------+      | PendNextPtr  |<----| PendNextPtr  | .... | PendNextPtr  |->0
 *               | NbrEntries=N |      +--------------+     +--------------+      +--------------+
 *               +--------------+   0<-| PendPrevPtr  |<----| PendPrevPtr  | .... | PendPrevPtr  |
 *                                     +--------------+     +--------------+      +--------------+
 *                                     |              |     |              |      |              |
 *                                     +--------------+     +--------------+      +--------------+
 *                                     |              |     |              |      |              |
 *                                     +--------------+     +--------------+      +--------------+
 *           @endverbatim
 * @param    p_tcb   Pointer to the TCB of the task to remove from the pend list.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
void OS_PendListRemove(OS_TCB *p_tcb)
{
  OS_PEND_LIST *p_pend_list;
  OS_TCB       *p_next;
  OS_TCB       *p_prev;

  if (p_tcb->PendObjPtr != DEF_NULL) {                          // Only remove if object has a pend list.
    p_pend_list = &p_tcb->PendObjPtr->PendList;                 // Get pointer to pend list

    // Remove TCB from the pend list.
    if (p_pend_list->HeadPtr->PendNextPtr == DEF_NULL) {
      p_pend_list->HeadPtr = DEF_NULL;                          // Only one entry in the pend list
      p_pend_list->TailPtr = DEF_NULL;
    } else if (p_tcb->PendPrevPtr == DEF_NULL) {                // See if entry is at the head of the list
      p_next = p_tcb->PendNextPtr;                              // Yes
      p_next->PendPrevPtr = DEF_NULL;
      p_pend_list->HeadPtr = p_next;
    } else if (p_tcb->PendNextPtr == DEF_NULL) {                // See if entry is at the tail of the list
      p_prev = p_tcb->PendPrevPtr;                              // Yes
      p_prev->PendNextPtr = DEF_NULL;
      p_pend_list->TailPtr = p_prev;
    } else {
      p_prev = p_tcb->PendPrevPtr;                              // Remove from inside the list
      p_next = p_tcb->PendNextPtr;
      p_prev->PendNextPtr = p_next;
      p_next->PendPrevPtr = p_prev;
    }
#if (OS_CFG_DBG_EN == DEF_ENABLED)
    p_pend_list->NbrEntries--;                                  // One less entry in the list
#endif
    p_tcb->PendNextPtr = DEF_NULL;
    p_tcb->PendPrevPtr = DEF_NULL;
    p_tcb->PendObjPtr = DEF_NULL;
  }
}

/****************************************************************************************************//**
 *                                                   OS_Post()
 *
 * @brief    This function is called to post to a task. This function exist because it is common to a
 *           number of OSxxxPost() services.
 *
 * @param    p_obj       Pointer to the object being posted to. If there are no object posted to
 *                       then the caller must pass a NULL pointer.
 *
 * @param    p_tcb       Pointer to the OS_TCB that will receive the 'post'.
 *
 * @param    p_void      If we are posting a message to a task, this is the message that the task
 *                       will receive.
 *
 * @param    msg_size    If we are posting a message to a task, this is the size of the message.
 *
 * @param    ts          The timestamp as to when the post occurred.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
void OS_Post(OS_PEND_OBJ *p_obj,
             OS_TCB      *p_tcb,
             void        *p_void,
             OS_MSG_SIZE msg_size,
             CPU_TS      ts)
{
#if (OS_CFG_TICK_EN == DEF_ENABLED)
  sl_status_t status;
#endif

#if (OS_CFG_TS_EN == DEF_DISABLED)
  (void)ts;                                                     // Prevent compiler warning for not using 'ts'
#endif
#if (OS_MSG_EN == DEF_DISABLED)
  (void)msg_size;                                               // Prevent compiler warning for not using 'msg_size'
#endif
#if (OS_MSG_EN == DEF_DISABLED)
  (void)p_void;                                                 // Prevent compiler warning for not using 'p_void'
#endif

  switch (p_tcb->TaskState) {
    case OS_TASK_STATE_RDY:                                     // Cannot Post a task that is ready
    case OS_TASK_STATE_DLY:                                     // Cannot Post a task that is delayed
    case OS_TASK_STATE_SUSPENDED:                               // Cannot Post a suspended task
    case OS_TASK_STATE_DLY_SUSPENDED:                           // Cannot Post a suspended task that was also dly'd
      break;

    case OS_TASK_STATE_PEND:
    case OS_TASK_STATE_PEND_TIMEOUT:
#if (OS_MSG_EN == DEF_ENABLED)
      p_tcb->MsgPtr = p_void;                                   // Deposit message in OS_TCB of task waiting
      p_tcb->MsgSize = msg_size;                                // ... assuming posting a message
#endif
#if (OS_CFG_TS_EN == DEF_ENABLED)
      p_tcb->TS = ts;
#endif
      if (p_obj != DEF_NULL) {
        OS_PendListRemove(p_tcb);                               // Remove task from pend list
      }
#if (OS_CFG_DBG_EN == DEF_ENABLED)
      OS_PendDbgNameRemove(p_obj,
                           p_tcb);
#endif
#if (OS_CFG_TICK_EN == DEF_ENABLED)
      if (p_tcb->TaskState == OS_TASK_STATE_PEND_TIMEOUT) {
        status = sl_sleeptimer_stop_timer(&p_tcb->TimerHandle);
        RTOS_ASSERT_CRITICAL((status == SL_STATUS_OK), RTOS_ERR_FAIL,; );
      }
#endif
      OS_RdyListInsert(p_tcb);                                  // Insert the task in the ready list
      p_tcb->TaskState = OS_TASK_STATE_RDY;
      p_tcb->PendStatus = OS_STATUS_PEND_OK;                    // Clear pend status
      p_tcb->PendOn = OS_TASK_PEND_ON_NOTHING;                  // Indicate no longer pending
      break;

    case OS_TASK_STATE_PEND_SUSPENDED:
    case OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED:
#if (OS_MSG_EN == DEF_ENABLED)
      p_tcb->MsgPtr = p_void;                                   // Deposit message in OS_TCB of task waiting
      p_tcb->MsgSize = msg_size;                                // ... assuming posting a message
#endif
#if (OS_CFG_TS_EN == DEF_ENABLED)
      p_tcb->TS = ts;
#endif
      if (p_obj != DEF_NULL) {
        OS_PendListRemove(p_tcb);                               // Remove from pend list
      }
#if (OS_CFG_DBG_EN == DEF_ENABLED)
      OS_PendDbgNameRemove(p_obj,
                           p_tcb);
#endif
#if (OS_CFG_TICK_EN == DEF_ENABLED)
      if (p_tcb->TaskState == OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED) {
        status = sl_sleeptimer_stop_timer(&p_tcb->TimerHandle);
        RTOS_ASSERT_CRITICAL((status == SL_STATUS_OK), RTOS_ERR_FAIL,; );
      }
#endif
      p_tcb->TaskState = OS_TASK_STATE_SUSPENDED;
      p_tcb->PendStatus = OS_STATUS_PEND_OK;                    // Clear pend status
      p_tcb->PendOn = OS_TASK_PEND_ON_NOTHING;                  // Indicate no longer pending
      break;

    case OS_TASK_STATE_DEL:
#if (OS_CFG_TASK_DEL_EN == DEF_ENABLED)
      break;
#endif

    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_OS,; );
  }
}

/****************************************************************************************************//**
 *                                               OS_RdyListInit()
 *
 * @brief    This function is called by OSInit() to initialize the ready list. The ready list contains
 *           a list of all the tasks that are ready to run. The list is actually an array of OS_RDY_LIST.
 *           An OS_RDY_LIST contains three fields. The number of OS_TCBs in the list (i.e. .NbrEntries),
 *           a pointer to the first OS_TCB in the OS_RDY_LIST (i.e. .HeadPtr) and a pointer to the last
 *           OS_TCB in the OS_RDY_LIST (i.e. .TailPtr).
 *           @n
 *           OS_TCBs are doubly linked in the OS_RDY_LIST and each OS_TCB points back to the OS_RDY_LIST
 *           it belongs to.
 *           @n
 *           'OS_RDY_LIST  OSRdyTbl[OS_CFG_PRIO_MAX]'  looks like this once initialized:
 *           @verbatim
 *                                   +---------------+--------------+
 *                                   |               | TailPtr      |-----> 0
 *                              [0]  | NbrEntries=0  +--------------+
 *                                   |               | HeadPtr      |-----> 0
 *                                   +---------------+--------------+
 *                                   |               | TailPtr      |-----> 0
 *                              [1]  | NbrEntries=0  +--------------+
 *                                   |               | HeadPtr      |-----> 0
 *                                   +---------------+--------------+
 *                                           :              :
 *                                           :              :
 *                                           :              :
 *                                   +---------------+--------------+
 *                                   |               | TailPtr      |-----> 0
 *               [OS_CFG_PRIO_MAX-1]  | NbrEntries=0  +--------------+
 *                                   |               | HeadPtr      |-----> 0
 *                                   +---------------+--------------+
 *           @endverbatim
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
void OS_RdyListInit(void)
{
  CPU_INT32U  i;
  OS_RDY_LIST *p_rdy_list;

  for (i = 0u; i < OS_CFG_PRIO_MAX; i++) {                      // Initialize the array of OS_RDY_LIST at each priority
    p_rdy_list = &OSRdyList[i];
#if (OS_CFG_DBG_EN == DEF_ENABLED)
    p_rdy_list->NbrEntries = 0u;
#endif
    p_rdy_list->HeadPtr = DEF_NULL;
    p_rdy_list->TailPtr = DEF_NULL;
  }
}

/****************************************************************************************************//**
 *                                           OS_RdyListInsert()
 *
 * @brief    This function is called to insert a TCB in the ready list.
 *
 *           The TCB is inserted at the tail of the list if the priority of the TCB is the same as the
 *           priority of the current task. The TCB is inserted at the head of the list if not.
 *
 * @param    p_tcb   Pointer to the TCB to insert into the ready list.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
void OS_RdyListInsert(OS_TCB *p_tcb)
{
  OS_PrioInsert(p_tcb->Prio);
  if (p_tcb->Prio == OSPrioCur) {                               // Are we readying a task at the same prio?
    OS_RdyListInsertTail(p_tcb);                                // Yes, insert readied task at the end of the list
  } else {
    OS_RdyListInsertHead(p_tcb);                                // No, insert readied task at the beginning of the list
  }

  OS_TRACE_TASK_READY(p_tcb);
}

/****************************************************************************************************//**
 *                                           OS_RdyListInsertHead()
 *
 * @brief    This function is called to place an OS_TCB at the beginning of a linked list as follows:
 *
 *           CASE 0: Insert in an empty list.
 *
 *               OS_RDY_LIST
 *               +--------------+
 *               | TailPtr      |-> 0
 *               +--------------+
 *               | HeadPtr      |-> 0
 *               +--------------+
 *               | NbrEntries=0 |
 *               +--------------+
 *
 *           CASE 1: Insert BEFORE the current head of list
 *
 *               OS_RDY_LIST
 *               +--------------+      OS_TCB
 *               | TailPtr      |-+--> +------------+
 *               +--------------+ |    | NextPtr    |->0
 *               | HeadPtr      |-/    +------------+
 *               +--------------+   0<-| PrevPtr    |
 *               | NbrEntries=1 |      +------------+
 *               +--------------+      :            :
 *                                     :            :
 *                                     +------------+
 *
 *               OS_RDY_LIST
 *               +--------------+
 *               | TailPtr      |-----------------------------------------+
 *               +--------------+      OS_TCB             OS_TCB          |   OS_TCB
 *               | HeadPtr      |----> +------------+     +------------+  +-> +------------+
 *               +--------------+      | NextPtr    |---->| NextPtr    | .... | NextPtr    |->0
 *               | NbrEntries=N |      +------------+     +------------+      +------------+
 *               +--------------+   0<-| PrevPtr    |<----| PrevPtr    | .... | PrevPtr    |
 *                                     +------------+     +------------+      +------------+
 *                                     :            :     :            :      :            :
 *                                     :            :     :            :      :            :
 *                                     +------------+     +------------+      +------------+
 *
 * @param    p_tcb   Pointer to the TCB to insert into the ready list.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
void OS_RdyListInsertHead(OS_TCB *p_tcb)
{
  OS_RDY_LIST *p_rdy_list;
  OS_TCB      *p_tcb2;

  p_rdy_list = &OSRdyList[p_tcb->Prio];
  if (p_rdy_list->HeadPtr == DEF_NULL) {                        // CASE 0: Insert when there are no entries
#if (OS_CFG_DBG_EN == DEF_ENABLED)
    p_rdy_list->NbrEntries = 1u;                                // This is the first entry
#endif
    p_tcb->NextPtr = DEF_NULL;                                  // No other OS_TCBs in the list
    p_tcb->PrevPtr = DEF_NULL;
    p_rdy_list->HeadPtr = p_tcb;                                // Both list pointers point to this OS_TCB
    p_rdy_list->TailPtr = p_tcb;
  } else {                                                      // CASE 1: Insert BEFORE the current head of list
#if (OS_CFG_DBG_EN == DEF_ENABLED)
    p_rdy_list->NbrEntries++;                                   // One more OS_TCB in the list
#endif
    p_tcb->NextPtr = p_rdy_list->HeadPtr;                       // Adjust new OS_TCBs links
    p_tcb->PrevPtr = DEF_NULL;
    p_tcb2 = p_rdy_list->HeadPtr;                               // Adjust old head of list's links
    p_tcb2->PrevPtr = p_tcb;
    p_rdy_list->HeadPtr = p_tcb;
  }
}

/****************************************************************************************************//**
 *                                           OS_RdyListInsertTail()
 *
 * @brief    This function is called to place an OS_TCB at the end of a linked list as follows:
 *
 *           CASE 0: Insert in an empty list.
 *
 *               OS_RDY_LIST
 *               +--------------+
 *               | TailPtr      |-> 0
 *               +--------------+
 *               | HeadPtr      |-> 0
 *               +--------------+
 *               | NbrEntries=0 |
 *               +--------------+
 *
 *           CASE 1: Insert AFTER the current tail of list
 *
 *               OS_RDY_LIST
 *               +--------------+      OS_TCB
 *               | TailPtr      |-+--> +------------+
 *               +--------------+ |    | NextPtr    |->0
 *               | HeadPtr      |-/    +------------+
 *               +--------------+   0<-| PrevPtr    |
 *               | NbrEntries=1 |      +------------+
 *               +--------------+      :            :
 *                                     :            :
 *                                     +------------+
 *
 *               OS_RDY_LIST
 *               +--------------+
 *               | TailPtr      |-----------------------------------------+
 *               +--------------+      OS_TCB             OS_TCB          |   OS_TCB
 *               | HeadPtr      |----> +------------+     +------------+  +-> +------------+
 *               +--------------+      | NextPtr    |---->| NextPtr    | .... | NextPtr    |->0
 *               | NbrEntries=N |      +------------+     +------------+      +------------+
 *               +--------------+   0<-| PrevPtr    |<----| PrevPtr    | .... | PrevPtr    |
 *                                     +------------+     +------------+      +------------+
 *                                     :            :     :            :      :            :
 *                                     :            :     :            :      :            :
 *                                     +------------+     +------------+      +------------+
 *
 * @param    p_tcb   Pointer to the TCB to insert into the ready list.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
void OS_RdyListInsertTail(OS_TCB *p_tcb)
{
  OS_RDY_LIST *p_rdy_list;
  OS_TCB      *p_tcb2;

  p_rdy_list = &OSRdyList[p_tcb->Prio];
  if (p_rdy_list->HeadPtr == DEF_NULL) {                        // CASE 0: Insert when there are no entries
#if (OS_CFG_DBG_EN == DEF_ENABLED)
    p_rdy_list->NbrEntries = 1u;                                // This is the first entry
#endif
    p_tcb->NextPtr = DEF_NULL;                                  // No other OS_TCBs in the list
    p_tcb->PrevPtr = DEF_NULL;
    p_rdy_list->HeadPtr = p_tcb;                                // Both list pointers point to this OS_TCB
    p_rdy_list->TailPtr = p_tcb;
  } else {                                                      // CASE 1: Insert AFTER the current tail of list
#if (OS_CFG_DBG_EN == DEF_ENABLED)
    p_rdy_list->NbrEntries++;                                   // One more OS_TCB in the list
#endif
    p_tcb->NextPtr = DEF_NULL;                                  // Adjust new OS_TCBs links
    p_tcb2 = p_rdy_list->TailPtr;
    p_tcb->PrevPtr = p_tcb2;
    p_tcb2->NextPtr = p_tcb;                                    // Adjust old tail of list's links
    p_rdy_list->TailPtr = p_tcb;
  }
}

/****************************************************************************************************//**
 *                                       OS_RdyListMoveHeadToTail()
 *
 * @brief    This function is called to move the current head of a list to the tail of the list.
 *
 *           CASE 0: TCB list is empty, nothing to do.
 *
 *           CASE 1: Only 1 OS_TCB  in the list, nothing to do.
 *
 *           CASE 2: Only 2 OS_TCBs in the list.
 *
 *               OS_RDY_LIST
 *               +--------------+
 *               | TailPtr      |----------------------+
 *               +--------------+      OS_TCB          |   OS_TCB
 *               | HeadPtr      |----> +------------+  +-> +------------+
 *               +--------------+      | NextPtr    |----> | NextPtr    |->0
 *               | NbrEntries=2 |      +------------+      +------------+
 *               +--------------+   0<-| PrevPtr    | <----| PrevPtr    |
 *                                     +------------+      +------------+
 *                                     :            :      :            :
 *                                     :            :      :            :
 *                                     +------------+      +------------+
 *
 *           CASE N: More than 2 OS_TCBs in the list.
 *
 *               OS_RDY_LIST
 *               +--------------+
 *               | TailPtr      |-----------------------------------------+
 *               +--------------+      OS_TCB             OS_TCB          |   OS_TCB
 *               | HeadPtr      |----> +------------+     +------------+  +-> +------------+
 *               +--------------+      | NextPtr    |---->| NextPtr    | .... | NextPtr    |->0
 *               | NbrEntries=N |      +------------+     +------------+      +------------+
 *               +--------------+   0<-| PrevPtr    |<----| PrevPtr    | .... | PrevPtr    |
 *                                     +------------+     +------------+      +------------+
 *                                     :            :     :            :      :            :
 *                                     :            :     :            :      :            :
 *                                     +------------+     +------------+      +------------+
 *
 * @param    p_list  Pointer to the OS_RDY_LIST where the OS_TCB will be moved.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
void OS_RdyListMoveHeadToTail(OS_RDY_LIST *p_rdy_list)
{
  OS_TCB *p_tcb1;
  OS_TCB *p_tcb2;
  OS_TCB *p_tcb3;

  if (p_rdy_list->HeadPtr != p_rdy_list->TailPtr) {
    if (p_rdy_list->HeadPtr->NextPtr == p_rdy_list->TailPtr) {  // SWAP the TCBs
      p_tcb1 = p_rdy_list->HeadPtr;                             // Point to current head
      p_tcb2 = p_rdy_list->TailPtr;                             // Point to current tail
      p_tcb1->PrevPtr = p_tcb2;
      p_tcb1->NextPtr = DEF_NULL;
      p_tcb2->PrevPtr = DEF_NULL;
      p_tcb2->NextPtr = p_tcb1;
      p_rdy_list->HeadPtr = p_tcb2;
      p_rdy_list->TailPtr = p_tcb1;
    } else {
      p_tcb1 = p_rdy_list->HeadPtr;                             // Point to current head
      p_tcb2 = p_rdy_list->TailPtr;                             // Point to current tail
      p_tcb3 = p_tcb1->NextPtr;                                 // Point to new list head
      p_tcb3->PrevPtr = DEF_NULL;                               // Adjust back    link of new list head
      p_tcb1->NextPtr = DEF_NULL;                               // Adjust forward link of new list tail
      p_tcb1->PrevPtr = p_tcb2;                                 // Adjust back    link of new list tail
      p_tcb2->NextPtr = p_tcb1;                                 // Adjust forward link of old list tail
      p_rdy_list->HeadPtr = p_tcb3;                             // Adjust new list head and tail pointers
      p_rdy_list->TailPtr = p_tcb1;
    }
  }
}

/****************************************************************************************************//**
 *                                           OS_RdyListRemove()
 *
 * @brief    This function is called to remove an OS_TCB from an OS_RDY_LIST knowing the address of the
 *           OS_TCB to remove.
 *
 *           CASE 0: TCB list is empty, nothing to do.
 *
 *           CASE 1: Only 1 OS_TCBs in the list.
 *
 *               OS_RDY_LIST
 *               +--------------+      OS_TCB
 *               | TailPtr      |-+--> +------------+
 *               +--------------+ |    | NextPtr    |->0
 *               | HeadPtr      |-/    +------------+
 *               +--------------+   0<-| PrevPtr    |
 *               | NbrEntries=1 |      +------------+
 *               +--------------+      :            :
 *                                     :            :
 *                                     +------------+
 *
 *           CASE N: Two or more OS_TCBs in the list.
 *
 *               OS_RDY_LIST
 *               +--------------+
 *               | TailPtr      |-----------------------------------------+
 *               +--------------+    OS_TCB               OS_TCB          |   OS_TCB
 *               | HeadPtr      |----> +------------+     +------------+  +-> +------------+
 *               +--------------+      | NextPtr    |---->| NextPtr    | .... | NextPtr    |->0
 *               | NbrEntries=N |      +------------+     +------------+      +------------+
 *               +--------------+   0<-| PrevPtr    |<----| PrevPtr    | .... | PrevPtr    |
 *                                     +------------+     +------------+      +------------+
 *                                     :            :     :            :      :            :
 *                                     :            :     :            :      :            :
 *                                     +------------+     +------------+      +------------+
 *
 * @param    p_tcb   Pointer to the OS_TCB to remove.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
void OS_RdyListRemove(OS_TCB *p_tcb)
{
  OS_RDY_LIST *p_rdy_list;
  OS_TCB      *p_tcb1;
  OS_TCB      *p_tcb2;

  p_rdy_list = &OSRdyList[p_tcb->Prio];
  p_tcb1 = p_tcb->PrevPtr;                                      // Point to next and previous OS_TCB in the list
  p_tcb2 = p_tcb->NextPtr;
  if (p_tcb1 == DEF_NULL) {                                     // Was the OS_TCB to remove at the head?
    if (p_tcb2 == DEF_NULL) {                                   // Yes, was it the only OS_TCB?
#if (OS_CFG_DBG_EN == DEF_ENABLED)
      p_rdy_list->NbrEntries = 0u;                              // Yes, no more entries
#endif
      p_rdy_list->HeadPtr = DEF_NULL;
      p_rdy_list->TailPtr = DEF_NULL;
      OS_PrioRemove(p_tcb->Prio);
    } else {
#if (OS_CFG_DBG_EN == DEF_ENABLED)
      p_rdy_list->NbrEntries--;                                 // No, one less entry
#endif
      p_tcb2->PrevPtr = DEF_NULL;                               // adjust back link of new list head
      p_rdy_list->HeadPtr = p_tcb2;                             // adjust OS_RDY_LIST's new head
    }
  } else {
#if (OS_CFG_DBG_EN == DEF_ENABLED)
    p_rdy_list->NbrEntries--;                                   // No, one less entry
#endif
    p_tcb1->NextPtr = p_tcb2;
    if (p_tcb2 == DEF_NULL) {
      p_rdy_list->TailPtr = p_tcb1;                             // Removing the TCB at the tail, adj the tail ptr
    } else {
      p_tcb2->PrevPtr = p_tcb1;
    }
  }
  p_tcb->PrevPtr = DEF_NULL;
  p_tcb->NextPtr = DEF_NULL;

  OS_TRACE_TASK_SUSPENDED(p_tcb);
}

/****************************************************************************************************//**
 *                                       OS_SchedLockTimeMeasStart()
 *
 * @brief    Start measuring the time the scheduler is locked.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *
 * @note     (2) It's assumed that this function is called when interrupts are disabled.
 *
 * @note     (3) We are reading the CPU_TS_TmrRd() directly even if this is a 16-bit timer. The reason
 *               is that we don't expect to have the scheduler locked for 65536 counts even at the rate
 *               the TS timer is updated. In other words, locking the scheduler for longer than 65536
 *               counts would not be a good thing for a real-time system.
 *******************************************************************************************************/
#if (OS_CFG_SCHED_LOCK_TIME_MEAS_EN == DEF_ENABLED)
void OS_SchedLockTimeMeasStart(void)
{
  if (OSSchedLockNestingCtr == 1u) {
    OSSchedLockTimeBegin = CPU_TS_TmrRd();
  }
}

/****************************************************************************************************//**
 *                                       OS_SchedLockTimeMeasStop()
 *
 * @brief    Stop measuring the time the scheduler is locked and update the current and max locked
 *           times.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *
 * @note     (2) It's assumed that this function is called when interrupts are disabled.
 *
 * @note     (3) We are reading the CPU_TS_TmrRd() directly even if this is a 16-bit timer. The reason
 *               is that we don't expect to have the scheduler locked for 65536 counts even at the rate
 *               the TS timer is updated. In other words, locking the scheduler for longer than 65536
 *               counts would not be a good thing for a real-time system.
 *******************************************************************************************************/
void OS_SchedLockTimeMeasStop(void)
{
  CPU_TS_TMR delta;

  if (OSSchedLockNestingCtr == 0u) {                            // Make sure we fully un-nested scheduler lock
    delta = CPU_TS_TmrRd()                                      // Compute the delta time between begin and end
            - OSSchedLockTimeBegin;
    if (OSSchedLockTimeMax < delta) {                           // Detect peak value
      OSSchedLockTimeMax = delta;
    }
    if (OSSchedLockTimeMaxCur < delta) {                        // Detect peak value (for resettable value)
      OSSchedLockTimeMaxCur = delta;
    }
  }
}
#endif

/****************************************************************************************************//**
 *                                           OS_SchedRoundRobin()
 *
 * @brief    This function is a callback for the task timer when operating in Round-Robin mode. It updates
 *           the RdyList and reschedules the task.
 *
 * @param    handle   Handle to timer that expired.
 *
 * @param    data     TCB of the task that owns this timer
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
#if (OS_CFG_SCHED_ROUND_ROBIN_EN == DEF_ENABLED)
void OS_SchedRoundRobin(sl_sleeptimer_timer_handle_t *handle,
                        void                         *data)
{
  OS_TCB *p_tcb = (OS_TCB *)data;
  CORE_DECLARE_IRQ_STATE;

  PP_UNUSED_PARAM(handle);

  if (OSSchedRoundRobinEn != DEF_TRUE) {                        // Make sure round-robin has been enabled
    return;
  }

  CORE_ENTER_ATOMIC();
  p_tcb->TimeQuantaCtr = 0u;
  OS_RdyListMoveHeadToTail(&OSRdyList[OSRoundRobinCurTCB->Prio]);
  CORE_EXIT_ATOMIC();

  OSSched();
}

/*****************************************************************************************************//**
 *                                          OS_SchedRoundRobinRestartTimer()
 *
 * @brief    Restart the Round-Robin timer, saving the TimeQuanta remaining if needed.
 *
 * @param    p_tcb     pointer to the TCB of the new task to round-robin
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *
 * @note     (2) Due to the sleeptimer's behavior and it blocking interrupts, we cannot rely on the
 *               OSTCBCurPtr as being 100% accurate. OSRoundRobinCurTCB is used instead as the current
 *               Task for the RoundRobin timer.
 *******************************************************************************************************/
void OS_SchedRoundRobinRestartTimer(OS_TCB* p_tcb)
{
  if (p_tcb == DEF_NULL) {
    return;
  }

  if (OSSchedRoundRobinEn != DEF_TRUE) {                        // Make sure round-robin has been enabled
    return;
  }

  sl_sleeptimer_start_timer(&OSRoundRobinTimer,
                            p_tcb->TimeQuantaCtr,
                            OS_SchedRoundRobin,
                            (void *)p_tcb,
                            0u,
                            0u);

  OSRoundRobinCurTCB = p_tcb;
}

/*****************************************************************************************************//**
 *                                          OS_SchedRoundRobinResetQuanta()
 *
 * @brief    Resets a task's TimeQuantaCtr
 *
 * @param    p_tcb     pointer to the TCB of the task
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
void OS_SchedRoundRobinResetQuanta(OS_TCB *p_tcb)
{
  if (p_tcb == DEF_NULL) {
    return;
  }

  if (OSSchedRoundRobinEn != DEF_TRUE) {                        // Make sure round-robin has been enabled
    return;
  }

  if (p_tcb->TimeQuanta == 0u) {                                // See if we need to use the default time slice
    p_tcb->TimeQuantaCtr = (uint64_t)(((uint64_t)OSSchedRoundRobinDfltTimeQuanta * (uint64_t)sl_sleeptimer_get_timer_frequency()) + (OSCfg_TickRate_Hz - 1u)) / OSCfg_TickRate_Hz;
  } else {
    p_tcb->TimeQuantaCtr = (uint64_t)(((uint64_t)p_tcb->TimeQuanta * (uint64_t)sl_sleeptimer_get_timer_frequency()) + (OSCfg_TickRate_Hz - 1u)) / OSCfg_TickRate_Hz;
  }
}

#endif

/****************************************************************************************************//**
 *                                               OS_TaskBlock()
 *
 * @brief    This function is called to remove a task from the ready list and also insert it in the
 *           timer tick list if the specified timeout is non-zero.
 *
 * @param    p_tcb       Pointer to the OS_TCB of the task block.
 *
 * @param    timeout     The desired timeout.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
void OS_TaskBlock(OS_TCB  *p_tcb,
                  OS_TICK timeout)
{
  if (timeout > 0u) {                                           // Add task to tick list if timeout non zero            */
#if (OS_CFG_TICK_EN == DEF_ENABLED)
    sl_status_t status;

    uint32_t delay = (uint64_t)(((uint64_t)timeout * (uint64_t)sl_sleeptimer_get_timer_frequency()) + ((uint64_t)OSCfg_TickRate_Hz - 1u)) / OSCfg_TickRate_Hz;
    status = sl_sleeptimer_start_timer(&p_tcb->TimerHandle,
                                       delay,
                                       OS_TimerCallback,
                                       (void *)p_tcb,
                                       0u,
                                       0u);
    RTOS_ASSERT_CRITICAL(status == SL_STATUS_OK, RTOS_ERR_FAIL,; );

    p_tcb->TaskState = OS_TASK_STATE_PEND_TIMEOUT;
#else
    RTOS_ASSERT_CRITICAL(DEF_FALSE, RTOS_ERR_FAIL,; );
#endif
  } else {
    p_tcb->TaskState = OS_TASK_STATE_PEND;
  }

  OS_RdyListRemove(p_tcb);
}

#if (OS_CFG_TICK_EN == DEF_ENABLED)
/*****************************************************************************************************//**
 *                                             OS_TimerCallback()
 *
 * @brief    Function called when a timer expires.
 *
 * @param    handle Handle to timer that expired.
 *
 * @param    data   Pointer caller specific data.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
void OS_TimerCallback(sl_sleeptimer_timer_handle_t *handle,
                      void                         *data)
{
  OS_TCB  *p_tcb = (OS_TCB *)data;
#if (OS_CFG_MUTEX_EN == DEF_ENABLED)
  OS_TCB  *p_tcb_owner;
  OS_PRIO  prio_new;
#endif
  PP_UNUSED_PARAM(handle);
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  switch (p_tcb->TaskState) {
    case OS_TASK_STATE_PEND_TIMEOUT:
    case OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED:
#if (OS_CFG_MUTEX_EN == DEF_ENABLED)
      p_tcb_owner = DEF_NULL;
      if (p_tcb->PendOn == OS_TASK_PEND_ON_MUTEX) {
        p_tcb_owner = ((OS_MUTEX *)p_tcb->PendObjPtr)->OwnerTCBPtr;
      }
#endif

#if (OS_MSG_EN == DEF_ENABLED)
      p_tcb->MsgPtr = DEF_NULL;
      p_tcb->MsgSize = 0u;
#endif
#if (OS_CFG_TS_EN == DEF_ENABLED)
      p_tcb->TS = OS_TS_GET();
#endif
      OS_PendListRemove(p_tcb);                                 /* Remove task from pend list                           */
      if (p_tcb->TaskState == OS_TASK_STATE_PEND_TIMEOUT) {
        OS_RdyListInsert(p_tcb);                                /* Insert the task in the ready list                    */
        p_tcb->TaskState = OS_TASK_STATE_RDY;
      } else if (p_tcb->TaskState == OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED) {
        p_tcb->TaskState = OS_TASK_STATE_SUSPENDED;
      }
      p_tcb->PendStatus = OS_STATUS_PEND_TIMEOUT;               /* Indicate pend timed out                              */
      p_tcb->PendOn = OS_TASK_PEND_ON_NOTHING;                  /* Indicate no longer pending                           */

#if (OS_CFG_MUTEX_EN == DEF_ENABLED)
      if (p_tcb_owner != DEF_NULL) {
        if ((p_tcb_owner->Prio != p_tcb_owner->BasePrio)
            && (p_tcb_owner->Prio == p_tcb->Prio)) {            /* Has the owner inherited a priority?                  */
          prio_new = OS_MutexGrpPrioFindHighest(p_tcb_owner);
          prio_new = prio_new > p_tcb_owner->BasePrio ? p_tcb_owner->BasePrio : prio_new;
          if (prio_new != p_tcb_owner->Prio) {
            OS_TaskChangePrio(p_tcb_owner, prio_new);
            OS_TRACE_MUTEX_TASK_PRIO_DISINHERIT(p_tcb_owner, p_tcb_owner->Prio);
          }
        }
      }
#endif
      break;

    case OS_TASK_STATE_DLY:
      p_tcb->TaskState = OS_TASK_STATE_RDY;
      OS_RdyListInsert(p_tcb);                                  /* Insert the task in the ready list                    */
      break;

    case OS_TASK_STATE_DLY_SUSPENDED:
      p_tcb->TaskState = OS_TASK_STATE_SUSPENDED;
      break;

    default:
      break;
  }
  CORE_EXIT_ATOMIC();

  OSSched();
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_KERNEL_AVAIL))
