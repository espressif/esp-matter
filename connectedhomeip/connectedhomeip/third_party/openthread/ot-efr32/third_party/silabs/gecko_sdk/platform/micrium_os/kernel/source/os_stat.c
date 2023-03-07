/***************************************************************************//**
 * @file
 * @brief Kernel - Statistics Module
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
const CPU_CHAR *os_stat__c = "$Id: $";
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                            GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

CPU_INT16U OSIdleCPUUsage = 0u;

/********************************************************************************************************
 ********************************************************************************************************
 *                                         LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (OS_CFG_STAT_TASK_EN == DEF_ENABLED)
static CPU_TS OSIdleTotalTick = 0u;
static CPU_TS OSStatStartTick = 0u;
static CPU_TS OSIdleStartTick = 0u;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/*****************************************************************************************************//**
 *                                               OSStatReset()
 *
 * @brief    Called by your application to reset the statistics.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s)
 *                   from this function:
 *                       - RTOS_ERR_NONE
 *******************************************************************************************************/
void OSStatReset(RTOS_ERR *p_err)
{
#if (OS_CFG_DBG_EN == DEF_ENABLED)
  OS_TCB *p_tcb;
#if (OS_MSG_EN == DEF_ENABLED)
  OS_MSG_Q *p_msg_q;
#endif
#if (OS_CFG_Q_EN == DEF_ENABLED)
  OS_Q *p_q;
#endif
#endif
  CORE_DECLARE_IRQ_STATE;

  OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  CORE_ENTER_ATOMIC();
#if (OS_CFG_STAT_TASK_EN == DEF_ENABLED)
  OSStatTaskCPUUsageMax = 0u;
#if (OS_CFG_TS_EN == DEF_ENABLED)
  OSStatTaskTimeMax = 0u;
#endif
#endif

#if (OS_CFG_TMR_EN == DEF_ENABLED)
#if (OS_CFG_TS_EN == DEF_ENABLED)
  OSTmrTaskTimeMax = 0u;
#endif
#endif

#if (OS_CFG_SCHED_LOCK_TIME_MEAS_EN == DEF_ENABLED)
  OSSchedLockTimeMax = 0u;                                      // Reset the maximum scheduler lock time
#endif

#if ((OS_MSG_EN == DEF_ENABLED) && (OS_CFG_DBG_EN == DEF_ENABLED))
  OSMsgPool.NbrUsedMax = 0u;
#endif
  CORE_EXIT_ATOMIC();

#if (OS_CFG_DBG_EN == DEF_ENABLED)
  CORE_ENTER_ATOMIC();
  p_tcb = OSTaskDbgListPtr;
  CORE_EXIT_ATOMIC();
  while (p_tcb != DEF_NULL) {                                   // Reset per-Task statistics
    CORE_ENTER_ATOMIC();

#if (OS_CFG_SCHED_LOCK_TIME_MEAS_EN == DEF_ENABLED)
    p_tcb->SchedLockTimeMax = 0u;
#endif

#if (OS_CFG_TASK_PROFILE_EN == DEF_ENABLED)
#if (OS_CFG_TASK_Q_EN == DEF_ENABLED)
    p_tcb->MsgQPendTimeMax = 0u;
#endif
    p_tcb->SemPendTimeMax = 0u;
    p_tcb->CtxSwCtr = 0u;
    p_tcb->CPUUsage = 0u;
    p_tcb->CPUUsageMax = 0u;
    p_tcb->CyclesTotal = 0u;
    p_tcb->CyclesTotalPrev = 0u;
#if (OS_CFG_TS_EN == DEF_ENABLED)
    p_tcb->CyclesStart = OS_TS_GET();
#endif
#endif

#if (OS_CFG_TASK_Q_EN == DEF_ENABLED)
    p_msg_q = &p_tcb->MsgQ;
    p_msg_q->NbrEntriesMax = 0u;
#endif
    p_tcb = p_tcb->DbgNextPtr;
    CORE_EXIT_ATOMIC();
  }
#endif

#if (OS_CFG_Q_EN == DEF_ENABLED) && (OS_CFG_DBG_EN == DEF_ENABLED)
  CORE_ENTER_ATOMIC();
  p_q = OSQDbgListPtr;
  CORE_EXIT_ATOMIC();
  while (p_q != DEF_NULL) {                                     // Reset message queues statistics
    CORE_ENTER_ATOMIC();
    p_msg_q = &p_q->MsgQ;
    p_msg_q->NbrEntriesMax = 0u;
    p_q = p_q->DbgNextPtr;
    CORE_EXIT_ATOMIC();
  }
#endif

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               OS_StatTask()
 *
 * @brief    This task is internal to the Kernel and is used to compute some statistics about the
 *           multitasking environment. Specifically, OS_StatTask() computes the CPU usage.
 *
 * @param    p_arg   Argument passed to the task when the task is created (unused).
 *
 * @note     (1) You can disable this task by setting the configuration #define OS_CFG_STAT_TASK_EN
 *               to 0.
 *
 * @note     (2) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
void OS_StatTask(void *p_arg)
{
#if (OS_CFG_DBG_EN == DEF_ENABLED)
#if (OS_CFG_TASK_PROFILE_EN == DEF_ENABLED)
  OS_CYCLES cycles_total;
#endif
  OS_TCB *p_tcb;
#endif
  CPU_BOOLEAN start_flag = DEF_NO;
  OS_CPU_USAGE usage;
  CPU_TS ts_stat_end;
  OS_CYCLES cycles_idle_total;
  OS_TICK  ctr_max;
  OS_TICK  ctr_mult;
  OS_TICK  ctr_div;
  RTOS_ERR err;
  OS_TICK  dly;
#if (OS_CFG_TS_EN == DEF_ENABLED)
  CPU_TS ts_start;
  CPU_TS ts_end;
#endif
  OS_RATE_HZ tick_rate;
  CORE_DECLARE_IRQ_STATE;

  (void)p_arg;                                                  // Prevent compiler warning for not using 'p_arg'

  OSStatReset(&err);                                            // Reset statistics

  dly       = (OS_TICK)0;                                       // Compute statistic task sleep delay
  tick_rate = OSTimeTickRateHzGet(&err);
  RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE, RTOS_ERR_FAIL,; );

  if (tick_rate > OSCfg_StatTaskRate_Hz) {
    dly = (tick_rate / OSCfg_StatTaskRate_Hz);
  }
  if (dly == 0u) {
    dly = (tick_rate / 10u);
  }

  while (DEF_ON) {
#if (OS_CFG_TS_EN == DEF_ENABLED)
    ts_start = OS_TS_GET();
#endif

    ts_stat_end = OS_TMR_GET_DIFF(OS_TS_GET(), OSStatStartTick);// Get execution time between two stats calculation.

    CORE_ENTER_ATOMIC();
    cycles_idle_total = OSIdleTotalTick;
    OSIdleTotalTick = 0u;
    CORE_EXIT_ATOMIC();

    //                                                             ------------ INDIVIDUAL TASK CPU USAGE -------------
    //                                                             'cycles_total' scaling ...
    if (ts_stat_end < 400000u) {                                // 1 to       400,000
      ctr_mult = 10000u;
      ctr_div = 1u;
    } else if (ts_stat_end < 4000000u) {                        // 400,000 to     4,000,000
      ctr_mult = 1000u;
      ctr_div = 10u;
    } else if (ts_stat_end < 40000000u) {                       // 4,000,000 to    40,000,000
      ctr_mult = 100u;
      ctr_div = 100u;
    } else if (ts_stat_end < 400000000u) {                      // 40,000,000 to   400,000,000
      ctr_mult = 10u;
      ctr_div = 1000u;
    } else {                                                    // 400,000,000 and up
      ctr_mult = 1u;
      ctr_div = 10000u;
    }
    ctr_max = ts_stat_end / ctr_div;

    usage = (OS_CPU_USAGE)(ctr_mult * cycles_idle_total / ctr_max);
    if (usage > 10000u) {
      usage = 10000u;
    }

    OSIdleCPUUsage = usage;

    OSStatTaskCPUUsage = 10000u - usage;
    if ((OSStatTaskCPUUsageMax < OSStatTaskCPUUsage)
        && start_flag) {
      OSStatTaskCPUUsageMax = OSStatTaskCPUUsage;
    }

#if (OS_CFG_DBG_EN == DEF_ENABLED)
#if (OS_CFG_TASK_PROFILE_EN == DEF_ENABLED)

    CORE_ENTER_ATOMIC();
    p_tcb = OSTaskDbgListPtr;
    CORE_EXIT_ATOMIC();

    while (p_tcb != DEF_NULL) {                                 // ---------------- TOTAL CYCLES COUNT ----------------
      CORE_ENTER_ATOMIC();
      p_tcb->CyclesTotalPrev = p_tcb->CyclesTotal;              // Save accumulated # cycles into a temp variable
      p_tcb->CyclesTotal = 0u;                                  // Reset total cycles for task for next run
      CORE_EXIT_ATOMIC();

      cycles_total += p_tcb->CyclesTotalPrev;                   // Perform sum of all task # cycles

      CORE_ENTER_ATOMIC();
      p_tcb = p_tcb->DbgNextPtr;
      CORE_EXIT_ATOMIC();
    }
#endif

    CORE_ENTER_ATOMIC();
    p_tcb = OSTaskDbgListPtr;
    CORE_EXIT_ATOMIC();

    while (p_tcb != DEF_NULL) {
#if (OS_CFG_TASK_PROFILE_EN == DEF_ENABLED)                     // Compute execution time of each task
      usage = (OS_CPU_USAGE)(ctr_mult * p_tcb->CyclesTotalPrev / ctr_max);
      if (usage > 10000u) {
        usage = 10000u;
      }

      p_tcb->CPUUsage = usage;
      if (p_tcb->CPUUsageMax < usage) {                         // Detect peak CPU usage
        p_tcb->CPUUsageMax = usage;
      }
#endif

#if (OS_CFG_STAT_TASK_STK_CHK_EN == DEF_ENABLED)
      OSTaskStkChk(p_tcb,                                       // Compute stack usage of active tasks only
                   &p_tcb->StkFree,
                   &p_tcb->StkUsed,
                   &err);
#endif

      CORE_ENTER_ATOMIC();
      p_tcb = p_tcb->DbgNextPtr;
      CORE_EXIT_ATOMIC();
    }
#endif

    if (OSStatResetFlag == DEF_TRUE) {                          // Check if need to reset statistics
      OSStatResetFlag = DEF_FALSE;
      OSStatReset(&err);
    }

    OSStatTaskHook();                                           // Invoke user definable hook

    OS_StatTickInit();
    start_flag = DEF_YES;

#if (OS_CFG_TS_EN == DEF_ENABLED)
    ts_end = OS_TS_GET() - ts_start;                            // Measure execution time of statistic task
    if (OSStatTaskTimeMax < ts_end) {
      OSStatTaskTimeMax = ts_end;
    }
#endif

    OSTimeDly(dly,
              OS_OPT_TIME_DLY,
              &err);
  }
}

/****************************************************************************************************//**
 *                                               OS_StatTaskInit()
 *
 * @brief    This function is called by OSInit() to initialize the statistic task.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s) from this
 *                   function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
void OS_StatTaskInit(RTOS_ERR *p_err)
{
  OSStatTaskRdy = DEF_FALSE;                                    // Statistic task is not ready
  OSStatResetFlag = DEF_FALSE;

  //                                                               --------------- CREATE THE STAT TASK ---------------
  OSTaskCreate(&OSStatTaskTCB,
               (CPU_CHAR *)((void *)"Kernel's Stat Task"),
               OS_StatTask,
               DEF_NULL,
               OSCfg_StatTaskPrio,
               OSCfg_StatTaskStkBasePtr,
               OSCfg_StatTaskStkLimit,
               OSCfg_StatTaskStkSize,
               0u,
               0u,
               DEF_NULL,
               (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               p_err);
}

/*****************************************************************************************************//**
 *                                           OS_StatTickInit()
 *
 * @brief    This function initialize OSStatStartTick with the current tick count.
 *******************************************************************************************************/
void OS_StatTickInit(void)
{
  OSStatStartTick = OS_TS_GET();
}

/*****************************************************************************************************//**
 *                                        OS_StatIdleEnterNotify()
 *
 * @brief    This function initialize OSIdleStartTick with the current tick count.
 *******************************************************************************************************/
void OS_StatIdleEnterNotify(void)
{
  OSIdleStartTick = OS_TS_GET();
}

/*****************************************************************************************************//**
 *                                        OS_StatIdleExitNotify()
 *
 * @brief    This function adds the difference in tick between OSIdleStartTick and the current tick
 *           count to OSIdleTotalTick.
 *******************************************************************************************************/
void OS_StatIdleExitNotify(void)
{
  OSIdleTotalTick += OS_TMR_GET_DIFF(OS_TS_GET(), OSIdleStartTick);
}

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_KERNEL_AVAIL))
