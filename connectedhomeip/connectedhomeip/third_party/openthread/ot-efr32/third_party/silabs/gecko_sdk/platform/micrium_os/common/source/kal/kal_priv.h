/***************************************************************************//**
 * @file
 * @brief Common - Kernel Abstraction Layer (Kal)
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
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _KAL_PRIV_H_
#define  _KAL_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>
#include  <cpu/include/cpu.h>

#include  <common/include/rtos_path.h>
#include  <rtos_cfg.h>

#include  <common/include/rtos_err.h>
#include  <common/include/lib_mem.h>
#include  <common/include/kal.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                                   DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                                   GENERAL
 *******************************************************************************************************/

#define  KAL_TIMEOUT_INFINITE                  0u

/********************************************************************************************************
 *                                       COMPILE TIME NULL HANDLES
 *******************************************************************************************************/

#define  KAL_OBJ_HANDLE_NULL                   { DEF_NULL }
#define  KAL_TASK_HANDLE_NULL                   KAL_OBJ_HANDLE_NULL
#define  KAL_LOCK_HANDLE_NULL                   KAL_OBJ_HANDLE_NULL
#define  KAL_SEM_HANDLE_NULL                    KAL_OBJ_HANDLE_NULL
#define  KAL_Q_HANDLE_NULL                      KAL_OBJ_HANDLE_NULL
#define  KAL_MON_HANDLE_NULL                    KAL_OBJ_HANDLE_NULL
#define  KAL_TASK_REG_HANDLE_NULL               KAL_OBJ_HANDLE_NULL

/********************************************************************************************************
 *                                               MON RES FLAGS
 *******************************************************************************************************/

#define  KAL_MON_RES_ALLOW                    DEF_BIT_NONE
#define  KAL_MON_RES_BLOCK                    DEF_BIT_00
#define  KAL_MON_RES_STOP_EVAL                DEF_BIT_01
#define  KAL_MON_RES_ACQUIRE                  DEF_BIT_02
#define  KAL_MON_RES_RELEASE                  DEF_BIT_03
#define  KAL_MON_RES_SUBSCRIBE                DEF_BIT_04
#define  KAL_MON_RES_UNSUBSCRIBE              DEF_BIT_05
#define  KAL_MON_RES_TRY_FAIL                 DEF_BIT_06

/********************************************************************************************************
 *                                                   OPTS
 *******************************************************************************************************/

#define  KAL_OPT_NONE                           DEF_BIT_NONE

/********************************************************************************************************
 *                                           CREATE OPTS (LOCK ONLY)
 *******************************************************************************************************/

#define  KAL_OPT_CREATE_NONE                    KAL_OPT_NONE

#define  KAL_OPT_CREATE_NON_REENTRANT           KAL_OPT_NONE    // Create non-re-entrant lock.
#define  KAL_OPT_CREATE_REENTRANT               DEF_BIT_00      // Create     re-entrant lock.

/********************************************************************************************************
 *                                       PEND OPTS (LOCK, SEM, Q)
 *******************************************************************************************************/

#define  KAL_OPT_PEND_NONE                      KAL_OPT_NONE

#define  KAL_OPT_PEND_BLOCKING                  KAL_OPT_NONE    // Block until rsrc is avail.
#define  KAL_OPT_PEND_NON_BLOCKING              DEF_BIT_00      // Don't block if rsrc is unavail.

/********************************************************************************************************
 *                                       POST OPTS (LOCK, SEM, Q)
 *******************************************************************************************************/

#define  KAL_OPT_POST_NONE                      KAL_OPT_NONE

/********************************************************************************************************
 *                                           ABORT OPTS (LOCK, SEM)
 *******************************************************************************************************/

#define  KAL_OPT_ABORT_NONE                     KAL_OPT_NONE

/********************************************************************************************************
 *                                       DELETE OPTS (LOCK, SEM, Q)
 *******************************************************************************************************/

#define  KAL_OPT_DEL_NONE                       KAL_OPT_NONE

/********************************************************************************************************
 *                                               MON OPTS
 *******************************************************************************************************/

#define  KAL_OPT_MON_NONE                       KAL_OPT_NONE

#define  KAL_OPT_MON_NO_SCHED                   DEF_BIT_00

/********************************************************************************************************
 *                                               TMR OPTS
 *******************************************************************************************************/

#define  KAL_OPT_TMR_NONE                       KAL_OPT_NONE

#define  KAL_OPT_TMR_ONE_SHOT                   KAL_OPT_NONE    // One-shot tmr,   callback called only once.
#define  KAL_OPT_TMR_PERIODIC                   DEF_BIT_00      // Periodic timer, callback called periodically.

/********************************************************************************************************
 *                                               DLY OPTS
 *******************************************************************************************************/

#define  KAL_OPT_DLY_NONE                       KAL_OPT_NONE

#define  KAL_OPT_DLY                            KAL_OPT_NONE    // 'Normal' delay.
#define  KAL_OPT_DLY_PERIODIC                   DEF_BIT_00      // Periodic delay.

/********************************************************************************************************
 *                                               TASK UUID
 *******************************************************************************************************/

#define  KAL_TASK_UUID_NONE                     0u

/********************************************************************************************************
 *                                               TASK UUID
 *******************************************************************************************************/

#ifdef  RTOS_MODULE_KERNEL_OS2_AVAIL
#include  <os_cfg.h>
#define  KAL_TASK_PRIO_MIN                      1u
#define  KAL_TASK_PRIO_MAX                      OS_LOWEST_PRIO
#elif defined(RTOS_MODULE_KERNEL_OS3_AVAIL)
#include  <os_cfg.h>
#define  KAL_TASK_PRIO_MIN                      1u
#define  KAL_TASK_PRIO_MAX                      OS_CFG_PRIO_MAX
#endif

/********************************************************************************************************
 *                                       LANGUAGE SUPPORT DEFINES
 *******************************************************************************************************/

#ifdef  __cplusplus
#define  KAL_CPP_EXT  extern
#else
#define  KAL_CPP_EXT
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                                   GENERAL
 *******************************************************************************************************/

#if (CPU_CFG_ADDR_SIZE < 4u)
typedef CPU_INT32U KAL_TASK_REG;
#else
typedef CPU_ADDR KAL_TASK_REG;
#endif

typedef CPU_INT32U KAL_TICK;
typedef CPU_INT32U KAL_TICK_RATE_HZ;

typedef CPU_INT16U KAL_MSG_QTY;

typedef CPU_INT08U KAL_OPT;

typedef CPU_INT08U KAL_MON_RES;

typedef CPU_ADDR KAL_TASK_UUID;

typedef CPU_INT16U KAL_CPU_USAGE;

/********************************************************************************************************
 *                                               OBJ HANDLES
 *******************************************************************************************************/

typedef struct kal_task_handle {                                // -------------------- TASK HANDLE -------------------
  void *TaskObjPtr;
} KAL_TASK_HANDLE;

typedef struct kal_lock_handle {                                // -------------------- LOCK HANDLE -------------------
  void *LockObjPtr;
} KAL_LOCK_HANDLE;

typedef struct kal_sem_handle {                                 // -------------------- SEM HANDLE --------------------
  void *SemObjPtr;
} KAL_SEM_HANDLE;

typedef struct kal_q_handle {                                   // --------------------- Q HANDLE ---------------------
  void *QObjPtr;
} KAL_Q_HANDLE;

typedef struct kal_mon_handle {
  void *MonObjPtr;
} KAL_MON_HANDLE;

typedef struct kal_tmr_handle {                                 // -------------------- TMR HANDLE --------------------
  void *TmrObjPtr;
} KAL_TMR_HANDLE;

typedef struct kal_task_reg_handle {                            // ------------------ TASK REG HANDLE -----------------
  void *TaskRegObjPtr;
} KAL_TASK_REG_HANDLE;

/********************************************************************************************************
 *                                               CFG STRUCTS
 *******************************************************************************************************/

typedef struct kal_task_ext_cfg {                               // ------------------- TASK EXT CFG -------------------
  CPU_INT32U Rsvd;                                              // Rsvd for future use.
} KAL_TASK_EXT_CFG;

typedef struct kal_lock_ext_cfg {                               // ------------------- LOCK EXT CFG -------------------
  KAL_OPT Opt;                                                  // Opt passed to LockCreate() funct.
} KAL_LOCK_EXT_CFG;

typedef struct kal_sem_ext_cfg {                                // ------------------- SEM EXT CFG --------------------
  CPU_INT32U Rsvd;                                              // Rsvd for future use.
} KAL_SEM_EXT_CFG;

typedef struct kal_q_ext_cfg {                                  // -------------------- Q EXT CFG ---------------------
  CPU_INT32U Rsvd;                                              // Rsvd for future use.
} KAL_Q_EXT_CFG;

typedef struct kal_mon_ext_cfg {                                // ------------------- MON EXT CFG --------------------
  CPU_INT32U Rsvd;                                              // Rsvd for future use.
} KAL_MON_EXT_CFG;

typedef struct kal_tmr_ext_cfg {                                // ------------------- TMR EXT CFG --------------------
  KAL_OPT Opt;                                                  // Opt passed to TmrCreate() funct.
} KAL_TMR_EXT_CFG;

typedef struct kal_task_reg_ext_cfg {                           // ----------------- TASK REG EXT CFG -----------------
  CPU_INT32U Rsvd;                                              // Rsvd for future use.
} KAL_TASK_REG_EXT_CFG;

/********************************************************************************************************
 *                                               KAL FEATURES
 *******************************************************************************************************/

typedef enum kal_feature {
  KAL_FEATURE_TASK_CREATE = 0u,                                 // Task creation.
  KAL_FEATURE_TASK_DEL,                                         // Task del.

  KAL_FEATURE_LOCK_CREATE,                                      // Lock create, acquire and release.
  KAL_FEATURE_LOCK_ACQUIRE,                                     // Lock pend.
  KAL_FEATURE_LOCK_RELEASE,                                     // Lock post.
  KAL_FEATURE_LOCK_DEL,                                         // Lock del.

  KAL_FEATURE_SEM_CREATE,                                       // Sem creation.
  KAL_FEATURE_SEM_PEND,                                         // Sem pend.
  KAL_FEATURE_SEM_POST,                                         // Sem post.
  KAL_FEATURE_SEM_ABORT,                                        // Sem pend abort.
  KAL_FEATURE_SEM_SET,                                          // Sem set cnt.
  KAL_FEATURE_SEM_DEL,                                          // Sem del.

  KAL_FEATURE_TMR,                                              // Tmr creation and exec.

  KAL_FEATURE_Q_CREATE,                                         // Q creation.
  KAL_FEATURE_Q_POST,                                           // Q post.
  KAL_FEATURE_Q_PEND,                                           // Q pend.

  KAL_FEATURE_DLY,                                              // Dly in both ms and ticks.

  KAL_FEATURE_PEND_TIMEOUT,                                     // Pend with timeout

  KAL_FEATURE_TICK_GET,                                         // Get OS tick val.

  KAL_FEATURE_TASK_REG,                                         // Task storage creation, get and set.

  KAL_FEATURE_MON,                                              // Mon creation and exec.
  KAL_FEATURE_MON_DEL,                                          // Mon deletion.

  KAL_FEATURE_CPU_USAGE_GET                                     // Get OS overall CPU usage.
} KAL_FEATURE;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------------- NULL HANDLES -------------------
extern const KAL_TASK_HANDLE     KAL_TaskHandleNull;
extern const KAL_LOCK_HANDLE     KAL_LockHandleNull;
extern const KAL_SEM_HANDLE      KAL_SemHandleNull;
extern const KAL_Q_HANDLE        KAL_QHandleNull;
extern const KAL_MON_HANDLE      KAL_MonHandleNull;
extern const KAL_TASK_REG_HANDLE KAL_TaskRegHandleNull;

/********************************************************************************************************
 ********************************************************************************************************
 *                                                   MACROS
 ********************************************************************************************************
 *******************************************************************************************************/

#define  KAL_TASK_HANDLE_IS_NULL(task_handle)            (((task_handle).TaskObjPtr == KAL_TaskHandleNull.TaskObjPtr)       ? DEF_YES : DEF_NO)
#define  KAL_LOCK_HANDLE_IS_NULL(lock_handle)            (((lock_handle).LockObjPtr == KAL_LockHandleNull.LockObjPtr)       ? DEF_YES : DEF_NO)
#define  KAL_SEM_HANDLE_IS_NULL(sem_handle)              (((sem_handle).SemObjPtr == KAL_SemHandleNull.SemObjPtr)         ? DEF_YES : DEF_NO)
#define  KAL_Q_HANDLE_IS_NULL(q_handle)                  (((q_handle).QObjPtr == KAL_QHandleNull.QObjPtr)             ? DEF_YES : DEF_NO)
#define  KAL_MON_HANDLE_IS_NULL(mon_handle)              (((mon_handle).MonObjPtr == KAL_MonHandleNull.MonObjPtr)         ? DEF_YES : DEF_NO)
#define  KAL_TMR_HANDLE_IS_NULL(tmr_handle)              (((tmr_handle).TmrObjPtr == KAL_TmrHandleNull.TmrObjPtr)         ? DEF_YES : DEF_NO)
#define  KAL_TASK_REG_HANDLE_IS_NULL(task_reg_handle)    (((task_reg_handle).TaskRegObjPtr == KAL_TaskRegHandleNull.TaskRegObjPtr) ? DEF_YES : DEF_NO)

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                                   CORE
 *******************************************************************************************************/

CPU_BOOLEAN KAL_FeatureQuery(KAL_FEATURE feature,
                             KAL_OPT     opt);

/********************************************************************************************************
 *                                                   TASKS
 *******************************************************************************************************/

KAL_TASK_HANDLE KAL_TaskAlloc(const CPU_CHAR   *p_name,
                              CPU_STK          *p_stk_base,
                              CPU_STK_SIZE     stk_size,
                              KAL_TASK_EXT_CFG *p_cfg,
                              RTOS_ERR         *p_err);

void KAL_TaskCreate(KAL_TASK_HANDLE  task_handle,
                    void (*p_fnct)(void *p_arg),
                    void             *p_task_arg,
                    KAL_TASK_PRIO    prio,
                    KAL_TASK_EXT_CFG *p_cfg,
                    RTOS_ERR         *p_err);

void KAL_TaskPrioSet(KAL_TASK_HANDLE task_handle,
                     KAL_TASK_PRIO   prio,
                     RTOS_ERR        *p_err);

void KAL_TaskDel(KAL_TASK_HANDLE task_handle);

KAL_TASK_UUID KAL_TaskUUID_Get(void);

/********************************************************************************************************
 *                                                   LOCKS
 *******************************************************************************************************/

KAL_LOCK_HANDLE KAL_LockCreate(const CPU_CHAR   *p_name,
                               KAL_LOCK_EXT_CFG *p_cfg,
                               RTOS_ERR         *p_err);

void KAL_LockAcquire(KAL_LOCK_HANDLE lock_handle,
                     KAL_OPT         opt,
                     CPU_INT32U      timeout,
                     RTOS_ERR        *p_err);

void KAL_LockRelease(KAL_LOCK_HANDLE lock_handle,
                     RTOS_ERR        *p_err);

void KAL_LockDel(KAL_LOCK_HANDLE lock_handle);

/********************************************************************************************************
 *                                                   SEMS
 *******************************************************************************************************/

KAL_SEM_HANDLE KAL_SemCreate(const CPU_CHAR  *p_name,
                             KAL_SEM_EXT_CFG *p_cfg,
                             RTOS_ERR        *p_err);

void KAL_SemPend(KAL_SEM_HANDLE sem_handle,
                 KAL_OPT        opt,
                 CPU_INT32U     timeout,
                 RTOS_ERR       *p_err);

void KAL_SemPost(KAL_SEM_HANDLE sem_handle,
                 KAL_OPT        opt,
                 RTOS_ERR       *p_err);

void KAL_SemPendAbort(KAL_SEM_HANDLE sem_handle,
                      RTOS_ERR       *p_err);

void KAL_SemSet(KAL_SEM_HANDLE sem_handle,
                CPU_INT08U     cnt,
                RTOS_ERR       *p_err);

void KAL_SemDel(KAL_SEM_HANDLE sem_handle);

/********************************************************************************************************
 *                                                   TMRS
 *******************************************************************************************************/

KAL_TMR_HANDLE KAL_TmrCreate(const CPU_CHAR  *p_name,
                             void (*p_callback)(void *p_arg),
                             void            *p_callback_arg,
                             CPU_INT32U      interval_ms,
                             KAL_TMR_EXT_CFG *p_cfg,
                             RTOS_ERR        *p_err);

void KAL_TmrStart(KAL_TMR_HANDLE tmr_handle,
                  RTOS_ERR       *p_err);

/********************************************************************************************************
 *                                                   QS
 *******************************************************************************************************/

KAL_Q_HANDLE KAL_QCreate(const CPU_CHAR *p_name,
                         KAL_MSG_QTY    max_msg_qty,
                         KAL_Q_EXT_CFG  *p_cfg,
                         RTOS_ERR       *p_err);

void *KAL_QPend(KAL_Q_HANDLE q_handle,
                KAL_OPT      opt,
                CPU_INT32U   timeout,
                RTOS_ERR     *p_err);

void KAL_QPost(KAL_Q_HANDLE q_handle,
               void         *p_msg,
               KAL_OPT      opt,
               RTOS_ERR     *p_err);

/********************************************************************************************************
 *                                                   MON
 *******************************************************************************************************/

KAL_MON_HANDLE KAL_MonCreate(const CPU_CHAR  *p_name,
                             void            *p_mon_obj,
                             void            *p_mon_data,
                             KAL_MON_EXT_CFG *p_cfg,
                             RTOS_ERR        *p_err);

void KAL_MonOp(KAL_MON_HANDLE mon_handle,
               void *p_op_data,
               KAL_MON_RES (*on_enter_fnct)(void *p_mon_data, void *p_op_data),
               KAL_MON_RES (*on_eval_fnct)(void *p_mon_data, void *p_eval_op_data, void *p_scan_op_data),
               KAL_OPT opt,
               KAL_TICK timeout_ms,
               RTOS_ERR *p_err);

void KAL_MonDel(KAL_MON_HANDLE mon_handle);

/********************************************************************************************************
 *                                                   DLYS
 *******************************************************************************************************/

void KAL_Dly(CPU_INT32U dly_ms);

void KAL_DlyTick(KAL_TICK dly_tick,
                 KAL_OPT  opt);

/********************************************************************************************************
 *                                               TASK REGS
 *******************************************************************************************************/

KAL_TASK_REG_HANDLE KAL_TaskRegCreate(KAL_TASK_REG_EXT_CFG *p_cfg,
                                      RTOS_ERR             *p_err);

KAL_TASK_REG KAL_TaskRegGet(KAL_TASK_HANDLE     task_handle,
                            KAL_TASK_REG_HANDLE task_reg,
                            RTOS_ERR            *p_err);

void KAL_TaskRegSet(KAL_TASK_HANDLE     task_handle,
                    KAL_TASK_REG_HANDLE task_reg_handle,
                    KAL_TASK_REG        val,
                    RTOS_ERR            *p_err);

/********************************************************************************************************
 *                                               TICK CTR
 *******************************************************************************************************/

KAL_TICK KAL_TickGet(RTOS_ERR *p_err);

KAL_TICK_RATE_HZ KAL_TickRateGet(void);

/********************************************************************************************************
 *                                               CPU USAGE
 *******************************************************************************************************/

KAL_CPU_USAGE KAL_CPU_UsageGet(void);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // End of kal private module include.
