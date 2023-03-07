/***************************************************************************//**
 * @file
 * @brief Kernel Private
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
 *                                                   MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _OS_PRIV_H_
#define  _OS_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <common/include/lib_utils.h>
#include  <common/include/rtos_opt_def.h>
#include  <common/include/rtos_path.h>
#include  <common/include/toolchains.h>
#include  <rtos_cfg.h>

#include  <common/source/rtos/rtos_utils_priv.h>

#include  <kernel/include/os.h>
#include  <kernel/include/os_trace.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                                   DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (OS_ARG_CHK_EN == DEF_ENABLED)
#define  LOG_DFLT_CH                                   (KERNEL)

//                                                                 ----------------- OS DEBUG ASSERTS -----------------
#define  OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err_var, ret_val)                             \
  if ((p_err_var) == DEF_NULL) {                                                        \
    RTOS_ASSERT_ERR_CODE_FAIL_LOG(p_err_var == DEF_NULL, RTOS_ERR_ASSERT_ERR_PTR_NULL); \
    CPU_SW_EXCEPTION(ret_val);                                                          \
  }

#define  OS_ASSERT_DBG_NO_ERR(expr, err_code, ret_val) \
  if ((expr) == 0u) {                                  \
    RTOS_ASSERT_ERR_CODE_FAIL_LOG(expr, err_code);     \
    RTOS_ASSERT_DBG_FAILED_END_CALL(ret_val);          \
  }

#define  OS_ASSERT_DBG_FAIL_EXEC(err_var, err_code, ret_val)  \
  RTOS_ASSERT_ERR_CODE_FAIL_LOG(Assert Dbg Failed, err_code); \
  RTOS_ERR_SET((err_var), (err_code));                        \
  RTOS_ASSERT_DBG_FAILED_END_CALL(ret_val);

#define  OS_ASSERT_DBG_ERR_SET(expr, err_var, err_code, ret_val) \
  if ((expr) == 0u) {                                            \
    RTOS_ASSERT_ERR_CODE_FAIL_LOG(expr, err_code);               \
    RTOS_ERR_SET((err_var), (err_code));                         \
    RTOS_ASSERT_DBG_FAILED_END_CALL(ret_val);                    \
  }
#else
#define  OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, ret_val)             PP_UNUSED_PARAM(p_err)

#define  OS_ASSERT_DBG_NO_ERR(expr, err_code, ret_val)

#define  OS_ASSERT_DBG_FAIL_EXEC(err_var, err_code, ret_val)

#define  OS_ASSERT_DBG_ERR_SET(expr, err_var, err_code, ret_val)
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                               EVENT FLAGS
 *******************************************************************************************************/

#if (OS_CFG_FLAG_EN == DEF_ENABLED)
void OS_FlagClr(OS_FLAG_GRP *p_grp);

void OS_FlagBlock(OS_FLAG_GRP *p_grp,
                  OS_FLAGS    flags,
                  OS_OPT      opt,
                  OS_TICK     timeout);

#if (OS_CFG_DBG_EN == DEF_ENABLED)
void OS_FlagDbgListAdd(OS_FLAG_GRP *p_grp);

void OS_FlagDbgListRemove(OS_FLAG_GRP *p_grp);
#endif

void OS_FlagTaskRdy(OS_TCB   *p_tcb,
                    OS_FLAGS flags_rdy,
                    CPU_TS   ts);
#endif

/********************************************************************************************************
 *                                               MONITORS
 *******************************************************************************************************/

#if (OS_CFG_MON_EN == DEF_ENABLED)
void OS_MonClr(OS_MON *p_mon);

#if (OS_CFG_DBG_EN == DEF_ENABLED)
void OS_MonDbgListAdd(OS_MON *p_mon);

void OS_MonDbgListRemove(OS_MON *p_mon);
#endif
#endif

/********************************************************************************************************
 *                                           MESSAGE MANAGEMENT
 *******************************************************************************************************/

void OS_MsgPoolInit(RTOS_ERR *p_err);

OS_MSG_QTY OS_MsgQFreeAll(OS_MSG_Q *p_msg_q);

void *OS_MsgQGet(OS_MSG_Q    *p_msg_q,
                 OS_MSG_SIZE *p_msg_size,
                 CPU_TS      *p_ts,
                 RTOS_ERR    *p_err);

void OS_MsgQInit(OS_MSG_Q   *p_msg_q,
                 OS_MSG_QTY size);

void OS_MsgQPut(OS_MSG_Q    *p_msg_q,
                void        *p_void,
                OS_MSG_SIZE msg_size,
                OS_OPT      opt,
                CPU_TS      ts,
                RTOS_ERR    *p_err);

/********************************************************************************************************
 *                                       MUTUAL EXCLUSION SEMAPHORES
 *******************************************************************************************************/

#if (OS_CFG_MUTEX_EN == DEF_ENABLED)
void OS_MutexClr(OS_MUTEX *p_mutex);

#if (OS_CFG_DBG_EN == DEF_ENABLED)
void OS_MutexDbgListAdd(OS_MUTEX *p_mutex);

void OS_MutexDbgListRemove(OS_MUTEX *p_mutex);
#endif

void OS_MutexGrpAdd(OS_TCB   *p_tcb,
                    OS_MUTEX *p_mutex);

void OS_MutexGrpRemove(OS_TCB   *p_tcb,
                       OS_MUTEX *p_mutex);

OS_PRIO OS_MutexGrpPrioFindHighest(OS_TCB *p_tcb);

void OS_MutexGrpPostAll(OS_TCB *p_tcb);
#endif

/********************************************************************************************************
 *                                           PEND/POST MANAGEMENT
 *******************************************************************************************************/

void OS_Pend(OS_PEND_OBJ *p_obj,
             OS_STATE    pending_on,
             OS_TICK     timeout);

void OS_PendAbort(OS_TCB    *p_tcb,
                  CPU_TS    ts,
                  OS_STATUS reason);

void OS_Post(OS_PEND_OBJ *p_obj,
             OS_TCB      *p_tcb,
             void        *p_void,
             OS_MSG_SIZE msg_size,
             CPU_TS      ts);

/********************************************************************************************************
 *                                           PEND LIST MANAGEMENT
 *******************************************************************************************************/

#if (OS_CFG_DBG_EN == DEF_ENABLED)
void OS_PendDbgNameAdd(OS_PEND_OBJ *p_obj,
                       OS_TCB      *p_tcb);

void OS_PendDbgNameRemove(OS_PEND_OBJ *p_obj,
                          OS_TCB      *p_tcb);
#endif

void OS_PendListInit(OS_PEND_LIST *p_pend_list);

void OS_PendListInsertPrio(OS_PEND_LIST *p_pend_list,
                           OS_TCB       *p_tcb);

void OS_PendListChangePrio(OS_TCB *p_tcb);

void OS_PendListRemove(OS_TCB *p_tcb);

/********************************************************************************************************
 *                                           PRIORITY MANAGEMENT
 *******************************************************************************************************/

void OS_PrioInit(void);

void OS_PrioInsert(OS_PRIO prio);

void OS_PrioRemove(OS_PRIO prio);

OS_PRIO OS_PrioGetHighest(void);

/********************************************************************************************************
 *                                               MESSAGE QUEUES
 *******************************************************************************************************/

#if (OS_CFG_Q_EN == DEF_ENABLED)
void OS_QClr(OS_Q *p_q);

#if (OS_CFG_DBG_EN == DEF_ENABLED)
void OS_QDbgListAdd(OS_Q *p_q);

void OS_QDbgListRemove(OS_Q *p_q);
#endif
#endif

/********************************************************************************************************
 *                                           READY LIST MANAGEMENT
 *******************************************************************************************************/

void OS_RdyListInit(void);

void OS_RdyListInsert(OS_TCB *p_tcb);

void OS_RdyListInsertHead(OS_TCB *p_tcb);

void OS_RdyListInsertTail(OS_TCB *p_tcb);

void OS_RdyListMoveHeadToTail(OS_RDY_LIST *p_rdy_list);

void OS_RdyListRemove(OS_TCB *p_tcb);

/********************************************************************************************************
 *                                               SCHEDULING
 *******************************************************************************************************/

#if (OS_CFG_SCHED_LOCK_TIME_MEAS_EN == DEF_ENABLED)
void OS_SchedLockTimeMeasStart(void);

void OS_SchedLockTimeMeasStop(void);
#endif

#if (OS_CFG_SCHED_ROUND_ROBIN_EN == DEF_ENABLED)
void OS_SchedRoundRobin(sl_sleeptimer_timer_handle_t *handle,
                        void                         *data);

void OS_SchedRoundRobinResetQuanta(OS_TCB *p_tcb);

void OS_SchedRoundRobinRestartTimer(OS_TCB *p_tcb);
#endif

/********************************************************************************************************
 *                                               SEMAPHORES
 *******************************************************************************************************/

#if (OS_CFG_SEM_EN == DEF_ENABLED)
void OS_SemClr(OS_SEM *p_sem);

#if (OS_CFG_DBG_EN == DEF_ENABLED)
void OS_SemDbgListAdd(OS_SEM *p_sem);

void OS_SemDbgListRemove(OS_SEM *p_sem);
#endif
#endif

/********************************************************************************************************
 *                                               TASK MANAGEMENT
 *******************************************************************************************************/

void OS_TaskBlock(OS_TCB  *p_tcb,
                  OS_TICK timeout);

#if (OS_CFG_TICK_EN == DEF_ENABLED)
void OS_TimerCallback(sl_sleeptimer_timer_handle_t  *handle,
                      void                          *data);
#endif

#if (OS_CFG_DBG_EN == DEF_ENABLED)
void OS_TaskDbgListAdd(OS_TCB *p_tcb);

void OS_TaskDbgListRemove(OS_TCB *p_tcb);
#endif

void OS_TaskInit(RTOS_ERR *p_err);

void OS_TaskInitTCB(OS_TCB *p_tcb);

#if (OS_CFG_TASK_STK_REDZONE_EN == DEF_ENABLED)
CPU_BOOLEAN OS_TaskStkRedzoneChk(CPU_STK      *p_base,
                                 CPU_STK_SIZE stk_size);

void OS_TaskStkRedzoneInit(CPU_STK      *p_base,
                           CPU_STK_SIZE stk_size);
#endif

void OS_TaskChangePrio(OS_TCB  *p_tcb,
                       OS_PRIO prio_new);

/********************************************************************************************************
 *                                           TIMER MANAGEMENT
 *******************************************************************************************************/

#if (OS_CFG_TMR_EN == DEF_ENABLED)
void OS_TmrClr(OS_TMR *p_tmr);

#if (OS_CFG_DBG_EN == DEF_ENABLED)
void OS_TmrDbgListAdd(OS_TMR *p_tmr);

void OS_TmrDbgListRemove(OS_TMR *p_tmr);
#endif

void OS_TmrInit(RTOS_ERR *p_err);

void OS_TmrLink(OS_TMR *p_tmr,
                OS_OPT opt);

void OS_TmrUnlink(OS_TMR *p_tmr);

void OS_TmrTask(void *p_arg);
#endif

/********************************************************************************************************
 *                                               MISCELLANEOUS
 *******************************************************************************************************/

void OSCfg_Init(void);

#if (OS_CFG_DBG_EN == DEF_ENABLED)
void OS_Dbg_Init(void);
#endif

#if (OS_CFG_STAT_TASK_EN == DEF_ENABLED)
void OS_StatTask(void *p_arg);
#endif

void OS_StatTaskInit(RTOS_ERR *p_err);

void OS_StatTickInit(void);

void OS_StatIdleEnterNotify(void);

void OS_StatIdleExitNotify(void);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
