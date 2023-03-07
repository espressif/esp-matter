/***************************************************************************//**
 * @file
 * @brief Common - Kernel Abstraction Layer (Kal) - Micrium Os Kernel
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

#if defined(RTOS_MODULE_KERNEL_AVAIL)

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <common/include/lib_def.h>
#include  <common/include/kal.h>
#include  <common/include/rtos_opt_def.h>
#include  <common/include/rtos_path.h>
#include  <rtos_cfg.h>
#include  <rtos_description.h>

#include  <os_cfg.h>
#ifdef  OS_CFG_COMPAT_INIT
#include  <os_cfg_app.h>
#endif

#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/kal/kal_priv.h>
#include  <common/include/lib_math.h>
#include  <common/include/lib_mem.h>
#include  <common/include/toolchains.h>

#include  <cpu/include/cpu.h>

#if (defined(RTOS_MODULE_KERNEL_OS3_AVAIL))
#include  <os.h>
#elif (defined(RTOS_MODULE_KERNEL_AVAIL))
#include  <kernel/include/os.h>
#endif

#include  <common/source/common/common_priv.h>
#include  <common/source/rtos/rtos_utils_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  KAL_CFG_TASK_STK_SIZE_PCT_FULL    90u

#define  KAL_INIT_STATUS_NONE               0u
#define  KAL_INIT_STATUS_OK                 1u
#define  KAL_INIT_STATUS_FAIL               2u

#define  LOG_DFLT_CH                       (COMMON, KAL)
#define  RTOS_MODULE_CUR                    RTOS_CFG_MODULE_COMMON

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

KAL_CPP_EXT const KAL_TASK_HANDLE     KAL_TaskHandleNull = KAL_OBJ_HANDLE_NULL;
KAL_CPP_EXT const KAL_LOCK_HANDLE     KAL_LockHandleNull = KAL_OBJ_HANDLE_NULL;
KAL_CPP_EXT const KAL_SEM_HANDLE      KAL_SemHandleNull = KAL_OBJ_HANDLE_NULL;
KAL_CPP_EXT const KAL_TMR_HANDLE      KAL_TmrHandleNull = KAL_OBJ_HANDLE_NULL;
KAL_CPP_EXT const KAL_Q_HANDLE        KAL_QHandleNull = KAL_OBJ_HANDLE_NULL;
KAL_CPP_EXT const KAL_MON_HANDLE      KAL_MonHandleNull = KAL_OBJ_HANDLE_NULL;
KAL_CPP_EXT const KAL_TASK_REG_HANDLE KAL_TaskRegHandleNull = KAL_OBJ_HANDLE_NULL;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------------ KAL TASK TYPE -------------------
typedef struct kal_task {
  OS_TCB         TCB;                                           // TCB for OS-III.

  CPU_STK        *StkBasePtr;                                   // Task stack base ptr.
  CPU_INT32U     StkSizeElements;                               // Task stack size (in CPU_STK elements).
#if (OS_CFG_DBG_EN == DEF_ENABLED)
  const CPU_CHAR *NamePtr;                                      // Task name string.
#endif
} KAL_TASK;

//                                                                 ------------------- KAL LOCK TYPE ------------------
#if (OS_CFG_MUTEX_EN == DEF_ENABLED)
typedef struct kal_lock {
  OS_MUTEX   Mutex;                                             // OS-III mutex obj.
  CPU_INT08U OptFlags;                                          // Opt flags associated with this lock.
} KAL_LOCK;
#endif

//                                                                 ------------------- KAL MON TYPE -------------------
#if (OS_CFG_MON_EN == DEF_ENABLED)
typedef struct kal_mon_arg_wrapper {
  void *DataPtr;                                                // Ptr to arg passed when calling KAL_MonOp().

  //                                                               Callback fnct called when entering KAL_MonOp().
  KAL_MON_RES (*EnterFnct)(void *p_mon_data, void *p_op_data);
  //                                                               Callback fnct called when evaluating monitor.
  KAL_MON_RES (*EvalFnct)(void *p_mon_data, void *p_eval_op_data, void *p_scan_op_data);
} KAL_MON_DATA_WRAPPER;
#endif

//                                                                 ------------------- KAL TMR TYPE -------------------
#if (OS_CFG_TMR_EN == DEF_ENABLED)
typedef struct kal_tmr {
  OS_TMR Tmr;                                                   // OS-III tmr obj.

  void (*CallbackFnct)(void *p_arg);                            // Tmr registered callback fnct.
  void   *CallbackArg;                                          // Arg to pass to callback fnct.
} KAL_TMR;
#endif

//                                                                 ---------------- KAL TASK REG TYPE -----------------
#if (OS_CFG_TASK_REG_TBL_SIZE > 0u)
typedef struct kal_task_reg_data {
  OS_REG_ID Id;                                                 // Id of the task reg.
} KAL_TASK_REG_DATA;
#endif

//                                                                 -------------- KAL INTERNAL DATA TYPE --------------
typedef struct kal_data {
#if (OS_CFG_MUTEX_EN == DEF_ENABLED)
  MEM_DYN_POOL MutexPool;                                       // Dyn mem pool used to alloc mutex.
#endif

#if (OS_CFG_SEM_EN == DEF_ENABLED)
  MEM_DYN_POOL SemPool;                                         // Dyn mem pool used to alloc sems.
#endif

#if (OS_CFG_MON_EN == DEF_ENABLED)
  MEM_DYN_POOL MonPool;                                         // Dyn mem pool used to alloc mons.
#endif

#if (OS_CFG_TMR_EN == DEF_ENABLED)
  MEM_DYN_POOL TmrPool;                                         // Dyn mem pool used to alloc tmrs.
#endif

#if (OS_CFG_TASK_REG_TBL_SIZE > 0u)
  MEM_DYN_POOL TaskRegPool;                                     // Dyn mem pool used to alloc task regs.
#endif
} KAL_DATA;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static KAL_DATA *KAL_DataPtr = DEF_NULL;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void KAL_Init(RTOS_ERR *p_err);

#if (OS_CFG_TMR_EN == DEF_ENABLED)
static void KAL_TmrFnctWrapper(void *p_tmr_os,
                               void *p_arg);
#endif

#if (OS_CFG_MON_EN == DEF_ENABLED)
static OS_MON_RES KAL_MonEnterFnct(OS_MON *p_mon,
                                   void   *p_op_data);

static OS_MON_RES KAL_MonEvalFnct(OS_MON *p_mon,
                                  void   *p_eval_op_data,
                                  void   *p_scan_op_data);
#endif

static KAL_TICK KAL_msToTicks(CPU_INT32U ms);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       KAL CORE API FUNCTIONS
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           KAL_FeatureQuery()
 *
 * @brief    Check if specified feature is available.
 *
 * @param    feature     Feature to query.
 *
 * @param    opt         Option associated with the feature requested.
 *
 * @return   DEF_YES, if feature is available.
 *           DEF_NO, otherwise.
 *******************************************************************************************************/
CPU_BOOLEAN KAL_FeatureQuery(KAL_FEATURE feature,
                             KAL_OPT     opt)
{
  CPU_BOOLEAN is_en;

  PP_UNUSED_PARAM(opt);

  is_en = DEF_NO;

  switch (feature) {
    case KAL_FEATURE_TASK_CREATE:                               // ---------------------- TASKS -----------------------
      is_en = DEF_YES;
      break;

    case KAL_FEATURE_TASK_DEL:
             #if ((OS_CFG_TASK_DEL_EN == DEF_ENABLED) \
      && !defined(OS_SAFETY_CRITICAL_IEC61508))

      is_en = DEF_YES;
             #endif
      break;

#if (OS_CFG_MUTEX_EN == DEF_ENABLED)                            // ----------------------- LOCKS ----------------------
    case KAL_FEATURE_LOCK_CREATE:
    case KAL_FEATURE_LOCK_ACQUIRE:
    case KAL_FEATURE_LOCK_RELEASE:
      is_en = DEF_YES;
      break;

    case KAL_FEATURE_LOCK_DEL:
             #if (!defined(OS_SAFETY_CRITICAL_IEC61508))
      is_en = DEF_YES;
             #endif
      break;
#endif // OS_CFG_MUTEX_EN

#if (OS_CFG_SEM_EN == DEF_ENABLED)                              // ----------------------- SEMS -----------------------
    case KAL_FEATURE_SEM_CREATE:
    case KAL_FEATURE_SEM_PEND:
    case KAL_FEATURE_SEM_POST:
      is_en = DEF_YES;
      break;

    case KAL_FEATURE_SEM_ABORT:
      is_en = DEF_YES;
      break;

    case KAL_FEATURE_SEM_SET:
      is_en = DEF_YES;
      break;

    case KAL_FEATURE_SEM_DEL:
             #if (!defined(OS_SAFETY_CRITICAL_IEC61508))
      is_en = DEF_YES;
             #endif
      break;
#endif // OS_CFG_SEM_EN

    case KAL_FEATURE_TMR:                                       // ----------------------- TMRS -----------------------
             #if (OS_CFG_TMR_EN == DEF_ENABLED)
      is_en = DEF_YES;
             #endif
      break;

    case KAL_FEATURE_Q_CREATE:                                  // ---------------------- QUEUES ----------------------
    case KAL_FEATURE_Q_PEND:
    case KAL_FEATURE_Q_POST:
             #if (OS_CFG_Q_EN == DEF_ENABLED)
      is_en = DEF_YES;
             #endif
      break;

#if (OS_CFG_MON_EN == DEF_ENABLED)                              // ---------------------- MONITORS ---------------------
    case KAL_FEATURE_MON:
      is_en = DEF_YES;
      break;

    case KAL_FEATURE_MON_DEL:
             #if (!defined(OS_SAFETY_CRITICAL_IEC61508))
      is_en = DEF_YES;
             #endif
      break;
#endif

    case KAL_FEATURE_DLY:                                       // ----------------------- DLYS -----------------------
      is_en = DEF_YES;
      break;

    case KAL_FEATURE_PEND_TIMEOUT:                              // ------------------- PEND TIMEOUT -------------------
             #if (OS_CFG_TICK_EN == DEF_ENABLED)
      is_en = DEF_YES;
             #endif
      break;

    case KAL_FEATURE_TICK_GET:                                  // ------------------- TICK CTR INFO ------------------
             #if (OS_CFG_TICK_EN == DEF_ENABLED)
      is_en = DEF_YES;
             #endif
      break;

    case KAL_FEATURE_TASK_REG:                                  // ------------------- TASK STORAGE -------------------
             #if (OS_CFG_TASK_REG_TBL_SIZE > 0u)
      is_en = DEF_YES;
             #endif
      break;

    case KAL_FEATURE_CPU_USAGE_GET:                             // ------------------ CPU USAGE INFO ------------------
             #if ((OS_CFG_TASK_PROFILE_EN == DEF_ENABLED) \
      && (OS_CFG_DBG_EN == DEF_ENABLED)                   \
      && (OS_CFG_STAT_TASK_EN == DEF_ENABLED))
      is_en = DEF_YES;
             #endif
      break;

    default:
      break;
  }

  return (is_en);
}

/********************************************************************************************************
 *                                           TASK API FUNCTIONS
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               KAL_TaskAlloc()
 *
 * @brief    Allocate a task object and its stack.
 *
 * @param    p_name      Pointer to name of the task.
 *
 * @param    p_stk_base  Pointer to start of task stack. If NULL, the stack will be allocated from
 *                       the KAL memory segment.
 *
 * @param    stk_size    Size (in CPU_STK elements) of the task stack.
 *
 * @param    p_cfg       Pointer to KAL task configuration structure.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_SEG_OVF
 *
 * @return   Allocated task's handle.
 *
 * @note     (1) This function will initialize KAL if KAL has not yet been initialized.
 *******************************************************************************************************/
KAL_TASK_HANDLE KAL_TaskAlloc(const CPU_CHAR   *p_name,
                              CPU_STK          *p_stk_base,
                              CPU_STK_SIZE     stk_size,
                              KAL_TASK_EXT_CFG *p_cfg,
                              RTOS_ERR         *p_err)
{
  KAL_TASK_HANDLE handle = KAL_TaskHandleNull;
  KAL_TASK        *p_task;
  CPU_ADDR        stk_addr;
  CPU_ADDR        stk_addr_aligned;
  CPU_SIZE_T      actual_stk_size;
  MEM_SEG         *p_mem_seg;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, handle);

  RTOS_ASSERT_DBG_ERR_SET((stk_size >= OS_CFG_STK_SIZE_MIN), *p_err, RTOS_ERR_INVALID_ARG, handle);

  //                                                               Make sure no unsupported cfg recv.
  RTOS_ASSERT_DBG_ERR_SET((p_cfg == DEF_NULL), *p_err, RTOS_ERR_NOT_SUPPORTED, handle);

  if (KAL_DataPtr == DEF_NULL) {
    KAL_Init(p_err);                                            // KAL initialization (see Note #1).
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (handle);
    }
  }

  p_mem_seg = Common_MemSegPtrGet();

  if (p_stk_base == DEF_NULL) {                                 // Must alloc task stk on mem seg.
    stk_addr_aligned = (CPU_ADDR)Mem_SegAllocExt("KAL task stk",
                                                 p_mem_seg,
                                                 (stk_size * sizeof(CPU_STK)),
                                                 CPU_CFG_STK_ALIGN_BYTES,
                                                 DEF_NULL,
                                                 p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      LOG_DBG(("KAL_TaskAlloc failed to allocate task stack."));
      return (handle);
    }
    actual_stk_size = stk_size;
  } else {
    //                                                             Align stk ptr, if needed.
    stk_addr = (CPU_ADDR)p_stk_base;
    stk_addr_aligned = MATH_ROUND_INC_UP_PWR2(stk_addr, CPU_CFG_STK_ALIGN_BYTES);
    actual_stk_size = (((stk_size * sizeof(CPU_STK)) - (stk_addr_aligned - stk_addr)) / sizeof(CPU_STK));
  }

  p_task = (KAL_TASK *)Mem_SegAlloc("KAL task",
                                    p_mem_seg,
                                    sizeof(KAL_TASK),
                                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_DBG(("KAL_TaskAlloc failed to allocate KAL_TASK data object."));
    return (handle);
  }

  p_task->StkBasePtr = (CPU_STK *)stk_addr_aligned;
  p_task->StkSizeElements = actual_stk_size;
#if (OS_CFG_DBG_EN == DEF_ENABLED)
  p_task->NamePtr = p_name;
#else
  PP_UNUSED_PARAM(p_name);
#endif
  handle.TaskObjPtr = (void *)p_task;

  LOG_VRB(("KAL_TaskAlloc call successful."));

  return (handle);
}

/****************************************************************************************************//**
 *                                               KAL_TaskCreate()
 *
 * @brief    Create and start a task.
 *
 * @param    task_handle     Handle of the task to create.
 *
 * @param    p_fnct          Pointer to task function.
 *
 * @param    p_task_arg      Pointer to argument that will be passed to task function (can be DEF_NULL).
 *
 * @param    prio            Task priority.
 *
 * @param    p_cfg           Pointer to KAL task configuration structure.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *
 * @note     (1) The task must be allocated prior to this call using KAL_TaskAlloc().
 *******************************************************************************************************/
void KAL_TaskCreate(KAL_TASK_HANDLE  task_handle,
                    void (*p_fnct)(void *p_arg),
                    void             *p_task_arg,
                    KAL_TASK_PRIO    prio,
                    KAL_TASK_EXT_CFG *p_cfg,
                    RTOS_ERR         *p_err)
{
  KAL_TASK     *p_task;
  CPU_STK_SIZE stk_limit;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((p_fnct != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET((KAL_TASK_HANDLE_IS_NULL(task_handle) != DEF_YES), *p_err, RTOS_ERR_NULL_PTR,; );

  //                                                               Make sure no unsupported cfg recv.
  RTOS_ASSERT_DBG_ERR_SET((p_cfg == DEF_NULL), *p_err, RTOS_ERR_NOT_SUPPORTED,; );

  p_task = (KAL_TASK *)task_handle.TaskObjPtr;
  stk_limit = (p_task->StkSizeElements * (100u - KAL_CFG_TASK_STK_SIZE_PCT_FULL)) / 100u;

  OSTaskCreate(&p_task->TCB,
#if (OS_CFG_DBG_EN == DEF_ENABLED)
               (CPU_CHAR *)p_task->NamePtr,
#else
               DEF_NULL,
#endif
               p_fnct,
               p_task_arg,
               prio,
               p_task->StkBasePtr,
               stk_limit,
               p_task->StkSizeElements,
               0u,
               0u,
               DEF_NULL,
               (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               p_err);
  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
    LOG_DBG(("Successfully created task ", (s)p_task->NamePtr, " with priority ", (u)prio));
  } else {
    LOG_DBG(("Call to OSTaskCreate failed with err: ", RTOS_ERR_LOG_ARG_GET(*p_err)));
  }

  return;
}

/****************************************************************************************************//**
 *                                               KAL_TaskPrioSet()
 *
 * @brief    Change the priority of a task.
 *
 * @param    task_handle     Handle of the task to change the priority.
 *
 * @param    prio            Task priority.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_INVALID_ARG
 *******************************************************************************************************/
void KAL_TaskPrioSet(KAL_TASK_HANDLE task_handle,
                     KAL_TASK_PRIO   prio,
                     RTOS_ERR        *p_err)
{
  OS_TCB *p_tcb;

  RTOS_ASSERT_DBG((KAL_TASK_HANDLE_IS_NULL(task_handle) != DEF_YES), RTOS_ERR_NULL_PTR,; );

  p_tcb = &((KAL_TASK *)task_handle.TaskObjPtr)->TCB;           // Get TCB from task handle provided.

  OSTaskChangePrio(p_tcb,
                   prio,
                   p_err);

  return;
}

/****************************************************************************************************//**
 *                                               KAL_TaskDel()
 *
 * @brief    Delete a task.
 *
 * @param    task_handle     Handle of the task to delete.
 *******************************************************************************************************/
void KAL_TaskDel(KAL_TASK_HANDLE task_handle)
{
  RTOS_ASSERT_DBG((KAL_TASK_HANDLE_IS_NULL(task_handle) != DEF_YES), RTOS_ERR_NULL_PTR,; );

    #if ((OS_CFG_TASK_DEL_EN == DEF_ENABLED) \
  && !defined(OS_SAFETY_CRITICAL_IEC61508))
  {
    OS_TCB   *p_tcb;
    RTOS_ERR err;

    p_tcb = &((KAL_TASK *)task_handle.TaskObjPtr)->TCB;         // Get TCB from task handle provided.

    OSTaskDel(p_tcb,
              &err);
    //                                                             OSTaskDel does not return another err in this case.
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

    return;
  }
    #else
  RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_NOT_AVAIL,; );
    #endif
}

/****************************************************************************************************//**
 *                                           KAL_TaskUUID_Get()
 *
 * @brief    Get unique universal identificator (UUID) for current task.
 *
 * @note     (1) This function can only be used to obtain the UUID of the task that is currently
 *               running.
 *
 * @note     (2) This function can be used to obtain the UUID of a task that has not been created by
 *               KAL.
 *******************************************************************************************************/
KAL_TASK_UUID KAL_TaskUUID_Get(void)
{
  KAL_TASK_UUID task_uid;

  task_uid = (KAL_TASK_UUID)OSTCBCurPtr;

  return (task_uid);
}

/********************************************************************************************************
 *                                           LOCK API FUNCTIONS
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               KAL_LockCreate()
 *
 * @brief    Create a lock, which is unlocked by default.
 *
 * @param    p_name  Pointer to name of the lock.
 *
 * @param    p_cfg   Pointer to KAL lock configuration structure.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s) from
 *                   this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_NOT_AVAIL
 *                       - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                       - RTOS_ERR_POOL_EMPTY
 *                       - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                       - RTOS_ERR_SEG_OVF
 *
 * @return   Created lock handle.
 *
 * @note     (1) This function will initialize KAL if KAL has not yet been initialized.
 *******************************************************************************************************/
KAL_LOCK_HANDLE KAL_LockCreate(const CPU_CHAR   *p_name,
                               KAL_LOCK_EXT_CFG *p_cfg,
                               RTOS_ERR         *p_err)
{
  KAL_LOCK_HANDLE handle = KAL_LockHandleNull;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, handle);

  if (KAL_DataPtr == DEF_NULL) {
    KAL_Init(p_err);                                            // KAL initialization (see Note #1).
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return(handle);
    }
  }

    #if (OS_CFG_MUTEX_EN == DEF_ENABLED)
  {
    KAL_LOCK   *p_kal_lock;
    CPU_INT08U opt_flags;

    p_kal_lock = (KAL_LOCK *)Mem_DynPoolBlkGet(&KAL_DataPtr->MutexPool,
                                               p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (handle);
    }

    OSMutexCreate(&p_kal_lock->Mutex,
                  (CPU_CHAR *)p_name,
                  p_err);
    if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
      if ((p_cfg != DEF_NULL)
          && (DEF_BIT_IS_SET(p_cfg->Opt, KAL_OPT_CREATE_REENTRANT) == DEF_YES)) {
        opt_flags = KAL_OPT_CREATE_REENTRANT;
      } else {
        opt_flags = KAL_OPT_CREATE_NON_REENTRANT;
      }
      p_kal_lock->OptFlags = opt_flags;
      handle.LockObjPtr = (void *)p_kal_lock;
      LOG_VRB(("KAL_LockCreate call successful."));
    } else {
      RTOS_ERR local_err;

      LOG_DBG(("Call to OSMutexCreate failed with err: ", RTOS_ERR_LOG_ARG_GET(*p_err)));

      Mem_DynPoolBlkFree(&KAL_DataPtr->MutexPool,
                         (void *)p_kal_lock,
                         &local_err);
      RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, handle);
    }

    return (handle);
  }
    #else
  PP_UNUSED_PARAM(p_name);
  PP_UNUSED_PARAM(p_cfg);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_AVAIL);

  return (handle);
    #endif
}

/****************************************************************************************************//**
 *                                               KAL_LockAcquire()
 *
 * @brief    Acquire a lock.
 *
 * @param    lock_handle     Handle of the lock to acquire.
 *
 * @param    opt             Options available:
 *                               - KAL_OPT_PEND_NONE:          block until timeout expires or lock is available.
 *                               - KAL_OPT_PEND_BLOCKING:      block until timeout expires or lock is available.
 *                               - KAL_OPT_PEND_NON_BLOCKING:  return immediately even if lock is not available.
 *
 * @param    timeout_ms      Timeout, in milliseconds. A value of 0 will never timeout.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_OS_OBJ_DEL
 *                               - RTOS_ERR_WOULD_BLOCK
 *                               - RTOS_ERR_IS_OWNER
 *                               - RTOS_ERR_OS_SCHED_LOCKED
 *                               - RTOS_ERR_ABORT
 *                               - RTOS_ERR_TIMEOUT
 *******************************************************************************************************/
void KAL_LockAcquire(KAL_LOCK_HANDLE lock_handle,
                     KAL_OPT         opt,
                     CPU_INT32U      timeout_ms,
                     RTOS_ERR        *p_err)
{
  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((KAL_LOCK_HANDLE_IS_NULL(lock_handle) == DEF_NO), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET((DEF_BIT_IS_SET_ANY(opt, (KAL_OPT)(~(KAL_OPT_PEND_NONE | KAL_OPT_PEND_NON_BLOCKING))) == DEF_NO), *p_err, RTOS_ERR_INVALID_ARG,; );

    #if (OS_CFG_MUTEX_EN == DEF_ENABLED)
  {
    KAL_LOCK   *p_kal_lock;
    CPU_INT32U timeout_ticks;
    OS_OPT     opt_os;

    if (timeout_ms != KAL_TIMEOUT_INFINITE) {
      timeout_ticks = KAL_msToTicks(timeout_ms);
    } else {
      timeout_ticks = 0u;
    }

    opt_os = OS_OPT_NONE;
    if (DEF_BIT_IS_CLR(opt, KAL_OPT_PEND_NON_BLOCKING) == DEF_YES) {
      opt_os |= OS_OPT_PEND_BLOCKING;
    } else {
      opt_os |= OS_OPT_PEND_NON_BLOCKING;
    }

    p_kal_lock = (KAL_LOCK *)lock_handle.LockObjPtr;
    OSMutexPend(&p_kal_lock->Mutex,
                timeout_ticks,
                opt_os,
                DEF_NULL,
                p_err);
    if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_IS_OWNER) {
      if (DEF_BIT_IS_SET(p_kal_lock->OptFlags, KAL_OPT_CREATE_REENTRANT) == DEF_YES) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      } else {
        RTOS_ERR local_err;

        LOG_DBG(("KAL_LockAcquire failed with re-entrancy error."));
        OSMutexPost(&p_kal_lock->Mutex,                         // Post mutex to decrement nesting ctr.
                    OS_OPT_POST_NONE,
                    &local_err);
        RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_IS_OWNER), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
      }
    } else if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      LOG_DBG(("Call to OSMutexPend failed with err: ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    }

    return;
  }
    #else
  PP_UNUSED_PARAM(lock_handle);
  PP_UNUSED_PARAM(opt);
  PP_UNUSED_PARAM(timeout_ms);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_AVAIL);

  return;
    #endif
}

/****************************************************************************************************//**
 *                                               KAL_LockRelease()
 *
 * @brief    Release a lock.
 *
 * @param    lock_handle     Handle of the lock to release.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_OWNERSHIP
 *                               - RTOS_ERR_IS_OWNER
 *******************************************************************************************************/
void KAL_LockRelease(KAL_LOCK_HANDLE lock_handle,
                     RTOS_ERR        *p_err)
{
  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((KAL_LOCK_HANDLE_IS_NULL(lock_handle) == DEF_NO), *p_err, RTOS_ERR_NULL_PTR,; );

    #if (OS_CFG_MUTEX_EN == DEF_ENABLED)
  {
    KAL_LOCK *p_kal_lock;

    p_kal_lock = (KAL_LOCK *)lock_handle.LockObjPtr;
    OSMutexPost(&p_kal_lock->Mutex,
                OS_OPT_POST_NONE,
                p_err);
    if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_IS_OWNER) {
      if (DEF_BIT_IS_SET(p_kal_lock->OptFlags, KAL_OPT_CREATE_REENTRANT) == DEF_YES) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      } else {
        LOG_DBG(("KAL_LockRelease failed with re-entrancy error."));
      }
    } else if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      LOG_DBG(("Call to OSMutexPost failed with err: ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    }

    return;
  }
    #else
  PP_UNUSED_PARAM(lock_handle);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_AVAIL);

  return;
    #endif
}

/****************************************************************************************************//**
 *                                               KAL_LockDel()
 *
 * @brief    Delete a lock.
 *
 * @param    lock_handle     Handle of the lock to delete.
 *******************************************************************************************************/
void KAL_LockDel(KAL_LOCK_HANDLE lock_handle)
{
  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG((KAL_LOCK_HANDLE_IS_NULL(lock_handle) == DEF_NO), RTOS_ERR_NULL_PTR,; );

    #if ((OS_CFG_MUTEX_EN == DEF_ENABLED) \
  && !defined(OS_SAFETY_CRITICAL_IEC61508))                     // Mutex is avail. Not safety critical.
  {
    KAL_LOCK *p_kal_lock;
    RTOS_ERR err;

    p_kal_lock = (KAL_LOCK *)lock_handle.LockObjPtr;

    (void)OSMutexDel(&p_kal_lock->Mutex,
                     OS_OPT_DEL_ALWAYS,
                     &err);
    //                                                             OSMutexDel does not return another err in this case.
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

    Mem_DynPoolBlkFree(&KAL_DataPtr->MutexPool,
                       (void *)p_kal_lock,
                       &err);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

    return;
  }
    #else //                                                       Mutex or mutex del is not avail.
  PP_UNUSED_PARAM(lock_handle);

  RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_NOT_AVAIL,; );
    #endif
}

/********************************************************************************************************
 *                                           SEM API FUNCTIONS
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               KAL_SemCreate()
 *
 * @brief    Create a semaphore, with a count of 0.
 *
 * @param    p_name  Pointer to name of the semaphore.
 *
 * @param    p_cfg   Pointer to KAL semaphore configuration structure.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s) from
 *                   this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_NOT_AVAIL
 *                       - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                       - RTOS_ERR_POOL_EMPTY
 *                       - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                       - RTOS_ERR_SEG_OVF
 *
 * @return   Created semaphore's handle.
 *
 * @note     (1) This function will initialize KAL if KAL has not yet been initialized.
 *******************************************************************************************************/
KAL_SEM_HANDLE KAL_SemCreate(const CPU_CHAR  *p_name,
                             KAL_SEM_EXT_CFG *p_cfg,
                             RTOS_ERR        *p_err)
{
  KAL_SEM_HANDLE handle = KAL_SemHandleNull;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, handle);

  //                                                               Make sure no unsupported cfg recv.
  RTOS_ASSERT_DBG_ERR_SET((p_cfg == DEF_NULL), *p_err, RTOS_ERR_NOT_SUPPORTED, handle);

  if (KAL_DataPtr == DEF_NULL) {
    KAL_Init(p_err);                                            // KAL initialization (see Note #1).
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return(handle);
    }
  }

    #if (OS_CFG_SEM_EN == DEF_ENABLED)
  {
    OS_SEM *p_sem;

    p_sem = (OS_SEM *)Mem_DynPoolBlkGet(&KAL_DataPtr->SemPool,
                                        p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (handle);
    }

    OSSemCreate(p_sem,
                (CPU_CHAR *)p_name,
                0u,
                p_err);
    if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
      handle.SemObjPtr = (void *)p_sem;
    } else {
      RTOS_ERR local_err;

      LOG_DBG(("Call to OSSemCreate failed with err: ", RTOS_ERR_LOG_ARG_GET(*p_err)));

      Mem_DynPoolBlkFree(&KAL_DataPtr->SemPool,
                         (void *)p_sem,
                         &local_err);
      RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, handle);
    }

    return (handle);
  }
    #else
  PP_UNUSED_PARAM(p_name);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_AVAIL);

  return (handle);
    #endif
}

/****************************************************************************************************//**
 *                                               KAL_SemPend()
 *
 * @brief    Pend on a semaphore.
 *
 * @param    sem_handle  Handle of the semaphore to pend on.
 *
 * @param    opt         Options available:
 *                           - KAL_OPT_PEND_NONE:          block until timeout expires or semaphore is available.
 *                           - KAL_OPT_PEND_BLOCKING:      block until timeout expires or semaphore is available.
 *                           - KAL_OPT_PEND_NON_BLOCKING:  return immediately even if semaphore is not available.
 *
 * @param    timeout_ms  Timeout, in milliseconds. A value of 0 will never timeout.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_OS_OBJ_DEL
 *                           - RTOS_ERR_WOULD_BLOCK
 *                           - RTOS_ERR_OS_SCHED_LOCKED
 *                           - RTOS_ERR_ABORT
 *                           - RTOS_ERR_TIMEOUT
 *******************************************************************************************************/
void KAL_SemPend(KAL_SEM_HANDLE sem_handle,
                 KAL_OPT        opt,
                 CPU_INT32U     timeout_ms,
                 RTOS_ERR       *p_err)
{
  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((KAL_SEM_HANDLE_IS_NULL(sem_handle) == DEF_NO), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET((DEF_BIT_IS_SET_ANY(opt, (KAL_OPT)(~(KAL_OPT_PEND_NONE | KAL_OPT_PEND_NON_BLOCKING))) == DEF_NO), *p_err, RTOS_ERR_INVALID_ARG,; );

    #if (OS_CFG_SEM_EN == DEF_ENABLED)
  {
    CPU_INT32U timeout_ticks;
    OS_OPT     opt_os;

    if (timeout_ms != KAL_TIMEOUT_INFINITE) {
      timeout_ticks = KAL_msToTicks(timeout_ms);
    } else {
      timeout_ticks = 0u;
    }

    opt_os = OS_OPT_NONE;
    if (DEF_BIT_IS_CLR(opt, KAL_OPT_PEND_NON_BLOCKING) == DEF_YES) {
      opt_os |= OS_OPT_PEND_BLOCKING;
    } else {
      opt_os |= OS_OPT_PEND_NON_BLOCKING;
    }

    OSSemPend((OS_SEM *)sem_handle.SemObjPtr,
              timeout_ticks,
              opt_os,
              DEF_NULL,
              p_err);

    return;
  }
    #else
  PP_UNUSED_PARAM(sem_handle);
  PP_UNUSED_PARAM(opt);
  PP_UNUSED_PARAM(timeout_ms);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_AVAIL);

  return;
    #endif
}

/****************************************************************************************************//**
 *                                               KAL_SemPost()
 *
 * @brief    Post a semaphore.
 *
 * @param    sem_handle  Handle of the semaphore to post.
 *
 * @param    opt         Options available:
 *                       KAL_OPT_POST_NONE:     wake only the highest priority task pending on semaphore.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_WOULD_OVF
 *******************************************************************************************************/
void KAL_SemPost(KAL_SEM_HANDLE sem_handle,
                 KAL_OPT        opt,
                 RTOS_ERR       *p_err)
{
  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((KAL_SEM_HANDLE_IS_NULL(sem_handle) == DEF_NO), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET((opt == KAL_OPT_POST_NONE), *p_err, RTOS_ERR_INVALID_ARG,; );

#if (OS_CFG_SEM_EN == DEF_ENABLED)
  OSSemPost((OS_SEM *)sem_handle.SemObjPtr,
            OS_OPT_POST_1,
            p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_DBG(("Call to OSSemPost failed with err: ", RTOS_ERR_LOG_ARG_GET(*p_err)));
  }

  return;
#else
  PP_UNUSED_PARAM(sem_handle);
  PP_UNUSED_PARAM(opt);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_AVAIL);

  return;
#endif
}

/****************************************************************************************************//**
 *                                           KAL_SemPendAbort()
 *
 * @brief    Abort given semaphore and resume all the tasks pending on it.
 *
 * @param    sem_handle  Handle of the semaphore to abort.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_NONE_WAITING
 *******************************************************************************************************/
void KAL_SemPendAbort(KAL_SEM_HANDLE sem_handle,
                      RTOS_ERR       *p_err)
{
  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((KAL_SEM_HANDLE_IS_NULL(sem_handle) == DEF_NO), *p_err, RTOS_ERR_NULL_PTR,; );

#if (OS_CFG_SEM_EN == DEF_ENABLED)
  {
    (void)OSSemPendAbort((OS_SEM *)sem_handle.SemObjPtr,
                         OS_OPT_PEND_ABORT_ALL,
                         p_err);
    if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE_WAITING) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    } else if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      LOG_DBG(("Call to OSSemPendAbort failed with err: ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    }

    return;
  }
#else //                                                           Sems or sems pend abort is not avail.
  PP_UNUSED_PARAM(sem_handle);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_AVAIL);

  return;
#endif
}

/****************************************************************************************************//**
 *                                               KAL_SemSet()
 *
 * @brief    Set value of semaphore.
 *
 * @param    sem_handle  Handle of the semaphore to set.
 *
 * @param    cnt         Count value to set semaphore.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_OS_TASK_WAITING
 *******************************************************************************************************/
void KAL_SemSet(KAL_SEM_HANDLE sem_handle,
                CPU_INT08U     cnt,
                RTOS_ERR       *p_err)
{
  //                                                               ------------------ VALIDATE ARGS -------------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((KAL_SEM_HANDLE_IS_NULL(sem_handle) == DEF_NO), *p_err, RTOS_ERR_NULL_PTR,; );

#if (OS_CFG_SEM_EN == DEF_ENABLED)
  {
    OSSemSet((OS_SEM *)sem_handle.SemObjPtr,
             cnt,
             p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      LOG_DBG(("Call to OSSemSet failed with err: ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    }

    return;
  }
#else //                                                           Sems or sems set is not avail.
  PP_UNUSED_PARAM(sem_handle);
  PP_UNUSED_PARAM(cnt);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_AVAIL);

  return;
#endif
}

/****************************************************************************************************//**
 *                                               KAL_SemDel()
 *
 * @brief    Delete a semaphore.
 *
 * @param    sem_handle  Handle of the semaphore to delete.
 *******************************************************************************************************/
void KAL_SemDel(KAL_SEM_HANDLE sem_handle)
{
  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG((KAL_SEM_HANDLE_IS_NULL(sem_handle) == DEF_NO), RTOS_ERR_NULL_PTR,; );

    #if ((OS_CFG_SEM_EN == DEF_ENABLED) \
  && !defined(OS_SAFETY_CRITICAL_IEC61508))
  {
    OS_SEM   *p_sem;
    RTOS_ERR err;

    p_sem = (OS_SEM *)sem_handle.SemObjPtr;

    (void)OSSemDel(p_sem,
                   OS_OPT_DEL_ALWAYS,
                   &err);
    //                                                             OSSemDel should not return another err in this case.
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

    Mem_DynPoolBlkFree(&KAL_DataPtr->SemPool,
                       (void *)p_sem,
                       &err);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

    return;
  }
    #else //                                                       Sems or sems del is not avail.
  PP_UNUSED_PARAM(sem_handle);

  RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_NOT_AVAIL,; );
    #endif
}

/********************************************************************************************************
 *                                           TMR API FUNCTIONS
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               KAL_TmrCreate()
 *
 * @brief    Create a single-shot timer.
 *
 * @param    p_name          Pointer to name of the timer.
 *
 * @param    p_callback      Pointer to the callback function that will be called on completion of timer.
 *
 * @param    p_callback_arg  Argument passed to callback function.
 *
 * @param    interval_ms     If timer is 'one-shot', delay  used by the timer, in milliseconds.
 *                           If timer is 'periodic', period used by the timer, in milliseconds.
 *
 * @param    p_cfg           Pointer to KAL timer configuration structure.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                               - RTOS_ERR_POOL_EMPTY
 *                               - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                               - RTOS_ERR_SEG_OVF
 *
 * @return   Created timer handle.
 *
 * @note     (1) This function will initialize KAL if KAL has not yet been initialized.
 *******************************************************************************************************/
KAL_TMR_HANDLE KAL_TmrCreate(const CPU_CHAR  *p_name,
                             void (*p_callback)(void *p_arg),
                             void            *p_callback_arg,
                             CPU_INT32U      interval_ms,
                             KAL_TMR_EXT_CFG *p_cfg,
                             RTOS_ERR        *p_err)
{
  KAL_TMR_HANDLE handle = KAL_TmrHandleNull;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, handle);
  //                                                               Check if OS is initialized.
  RTOS_ASSERT_DBG_ERR_SET((OSInitialized == DEF_TRUE), *p_err, RTOS_ERR_NOT_INIT, handle);

  //                                                               Validate callback fnct ptr.
  RTOS_ASSERT_DBG_ERR_SET((p_callback != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, handle);

  //                                                               Validate time.
  RTOS_ASSERT_DBG_ERR_SET((interval_ms != 0u), *p_err, RTOS_ERR_INVALID_ARG, handle);

  if (p_cfg != DEF_NULL) {
    RTOS_ASSERT_DBG_ERR_SET((DEF_BIT_IS_SET_ANY(p_cfg->Opt, (KAL_OPT)(~(KAL_OPT_TMR_NONE | KAL_OPT_TMR_PERIODIC))) == DEF_NO), *p_err, RTOS_ERR_INVALID_ARG, handle);
  }

  if (KAL_DataPtr == DEF_NULL) {
    KAL_Init(p_err);                                            // KAL initialization (see Note #1).
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return(handle);
    }
  }

    #if (OS_CFG_TMR_EN == DEF_ENABLED)
  {
    KAL_TMR    *p_tmr;
    CPU_INT32U tmr_dly;
    CPU_INT32U tmr_period;
    OS_OPT     opt_os;
    KAL_TICK   ticks;
    #ifdef  OS_CFG_COMPAT_INIT
        #if  ((OS_CFG_TMR_TASK_RATE_HZ >= 1000u) \
    && (OS_CFG_TMR_TASK_RATE_HZ % 1000u == 0u))
    const CPU_INT08U mult = OS_CFG_TMR_TASK_RATE_HZ / 1000u;
        #endif
        #endif

    p_tmr = (KAL_TMR *)Mem_DynPoolBlkGet(&KAL_DataPtr->TmrPool,
                                         p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (handle);
    }

    p_tmr->CallbackFnct = p_callback;
    p_tmr->CallbackArg = p_callback_arg;
    //                                                             Calc nbr of tmr ticks (rounded up nearest int).
#ifdef  OS_CFG_COMPAT_INIT
        #if  ((OS_CFG_TMR_TASK_RATE_HZ >= 1000u) \
    && (OS_CFG_TMR_TASK_RATE_HZ % 1000u == 0u))                 // Optimize calc if possible for often used vals.
    ticks = interval_ms * mult;
        #elif (OS_CFG_TMR_TASK_RATE_HZ == 100u)
    ticks = ((interval_ms +  9u) /  10u);
        #elif (OS_CFG_TMR_TASK_RATE_HZ == 10u)
    ticks = ((interval_ms + 99u) / 100u);
        #else //                                                   General formula.
    ticks = (((interval_ms * OS_CFG_TMR_TASK_RATE_HZ)  + 1000u - 1u) / 1000u);
        #endif
#else
    ticks = (((interval_ms * OSCfg_TmrTaskRate_Hz)  + 1000u - 1u) / 1000u);
#endif
    //                                                             Tmr is 'periodic'.
    if ((p_cfg != DEF_NULL)
        && (DEF_BIT_IS_SET(p_cfg->Opt, KAL_OPT_TMR_PERIODIC) == DEF_YES)) {
      opt_os = OS_OPT_TMR_PERIODIC;
      tmr_dly = 0u;
      tmr_period = ticks;
    } else {
      opt_os = OS_OPT_TMR_ONE_SHOT;                             // Tmr is 'one-shot'.
      tmr_dly = ticks;
      tmr_period = 0u;
    }

    OSTmrCreate(&p_tmr->Tmr,                                    // Create tmr obj.
                (CPU_CHAR *)p_name,
                tmr_dly,
                tmr_period,
                opt_os,
                KAL_TmrFnctWrapper,
                (void *)p_tmr,
                p_err);
    if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
      handle.TmrObjPtr = p_tmr;
      LOG_VRB(("KAL_TmrCreate call successful."));
    } else {
      RTOS_ERR local_err;

      LOG_DBG(("Call to OSTmrCreate failed with err: ", RTOS_ERR_LOG_ARG_GET(*p_err)));

      Mem_DynPoolBlkFree(&KAL_DataPtr->TmrPool,
                         p_tmr,
                         &local_err);
      RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, handle);
    }

    return (handle);
  }
    #else
  PP_UNUSED_PARAM(p_name);
  PP_UNUSED_PARAM(p_callback);
  PP_UNUSED_PARAM(p_callback_arg);
  PP_UNUSED_PARAM(interval_ms);
  PP_UNUSED_PARAM(p_cfg);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_AVAIL);

  return (handle);
    #endif
}

/****************************************************************************************************//**
 *                                               KAL_TmrStart()
 *
 * @brief    Start timer.
 *
 * @param    tmr_handle  Handle of timer to start.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_NOT_INIT
 *******************************************************************************************************/
void KAL_TmrStart(KAL_TMR_HANDLE tmr_handle,
                  RTOS_ERR       *p_err)
{
  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((KAL_TMR_HANDLE_IS_NULL(tmr_handle) == DEF_NO), *p_err, RTOS_ERR_NULL_PTR,; );

    #if (OS_CFG_TMR_EN == DEF_ENABLED)
  {
    KAL_TMR *p_tmr;

    p_tmr = (KAL_TMR *)tmr_handle.TmrObjPtr;

    OSTmrStart(&p_tmr->Tmr,
               p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      LOG_DBG(("Call to OSTmrStart failed with err: ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    }

    return;
  }
    #else
  PP_UNUSED_PARAM(tmr_handle);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_AVAIL);

  return;
    #endif
}

/********************************************************************************************************
 *                                           Q API FUNCTIONS
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               KAL_QCreate()
 *
 * @brief    Create an empty queue.
 *
 * @param    p_name          Pointer to name of the queue.
 *
 * @param    max_msg_qty     Maximum number of message contained in the queue.
 *
 * @param    p_cfg           Pointer to KAL queue configuration structure.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                               - RTOS_ERR_SEG_OVF
 *
 * @return   Created queue handle.
 *
 * @note     (1) This function will initialize KAL if KAL has not yet been initialized.
 *******************************************************************************************************/
KAL_Q_HANDLE KAL_QCreate(const CPU_CHAR *p_name,
                         KAL_MSG_QTY    max_msg_qty,
                         KAL_Q_EXT_CFG  *p_cfg,
                         RTOS_ERR       *p_err)
{
  KAL_Q_HANDLE handle = KAL_QHandleNull;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, handle);

  //                                                               Make sure no unsupported cfg recv.
  RTOS_ASSERT_DBG_ERR_SET((p_cfg == DEF_NULL), *p_err, RTOS_ERR_NOT_SUPPORTED, handle);

  if (KAL_DataPtr == DEF_NULL) {
    KAL_Init(p_err);                                            // KAL initialization (see Note #1).
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return(handle);
    }
  }

    #if (OS_CFG_Q_EN == DEF_ENABLED)
  {
    MEM_SEG *p_mem_seg;
    OS_Q    *p_q;

    p_mem_seg = Common_MemSegPtrGet();

    p_q = (OS_Q *)Mem_SegAlloc("KAL Q",
                               p_mem_seg,
                               sizeof(OS_Q),
                               p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (handle);
    }

    OSQCreate(p_q,
              (CPU_CHAR *)p_name,
              max_msg_qty,
              p_err);
    if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
      handle.QObjPtr = (void *)p_q;
      LOG_VRB(("KAL_QCreate call successful."));
    } else {
      LOG_DBG(("Call to OSQCreate failed with err: ", RTOS_ERR_LOG_ARG_GET(*p_err), ". Memory allocated for Q is lost."));
    }

    return (handle);
  }
    #else
  PP_UNUSED_PARAM(p_name);
  PP_UNUSED_PARAM(max_msg_qty);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_AVAIL);

  return (handle);
    #endif
}

/****************************************************************************************************//**
 *                                               KAL_QPend()
 *
 * @brief    Pend/get first message of queue.
 *
 * @param    q_handle    Handle of the queue to pend on.
 *
 * @param    opt         Options available:
 *                           - KAL_OPT_PEND_NONE:          block until timeout expires or message is available.
 *                           - KAL_OPT_PEND_BLOCKING:      block until timeout expires or message is available.
 *                           - KAL_OPT_PEND_NON_BLOCKING:  return immediately with or without message.
 *
 * @param    timeout_ms  Timeout, in milliseconds. A value of 0 will never timeout.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_OS_OBJ_DEL
 *                           - RTOS_ERR_NOT_FOUND
 *                           - RTOS_ERR_WOULD_BLOCK
 *                           - RTOS_ERR_OS_SCHED_LOCKED
 *                           - RTOS_ERR_ABORT
 *                           - RTOS_ERR_TIMEOUT
 *
 * @return   Pointer to message obtained, if any, if no error.
 *           Null pointer, otherwise.
 *******************************************************************************************************/
void *KAL_QPend(KAL_Q_HANDLE q_handle,
                KAL_OPT      opt,
                CPU_INT32U   timeout_ms,
                RTOS_ERR     *p_err)
{
  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NULL);

  RTOS_ASSERT_DBG_ERR_SET((KAL_Q_HANDLE_IS_NULL(q_handle) == DEF_NO), *p_err, RTOS_ERR_NULL_PTR, DEF_NULL);

  RTOS_ASSERT_DBG_ERR_SET((DEF_BIT_IS_SET_ANY(opt, (KAL_OPT)(~(KAL_OPT_PEND_NONE | KAL_OPT_PEND_NON_BLOCKING))) == DEF_NO), *p_err, RTOS_ERR_INVALID_ARG, DEF_NULL);

    #if (OS_CFG_Q_EN == DEF_ENABLED)
  {
    void        *p_msg;
    CPU_INT32U  timeout_ticks;
    OS_MSG_SIZE msg_size;
    OS_OPT      opt_os;

    if (timeout_ms != KAL_TIMEOUT_INFINITE) {
      timeout_ticks = KAL_msToTicks(timeout_ms);
    } else {
      timeout_ticks = 0u;
    }

    opt_os = OS_OPT_NONE;
    if (DEF_BIT_IS_CLR(opt, KAL_OPT_PEND_NON_BLOCKING) == DEF_YES) {
      opt_os |= OS_OPT_PEND_BLOCKING;
    } else {
      opt_os |= OS_OPT_PEND_NON_BLOCKING;
    }

    p_msg = OSQPend((OS_Q *)q_handle.QObjPtr,
                    timeout_ticks,
                    opt_os,
                    &msg_size,
                    DEF_NULL,
                    p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      LOG_DBG(("Call to OSQPend failed with err: ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    }

    return (p_msg);
  }
    #else
  PP_UNUSED_PARAM(q_handle);
  PP_UNUSED_PARAM(opt);
  PP_UNUSED_PARAM(timeout_ms);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_AVAIL);

  return (DEF_NULL);
    #endif
}

/****************************************************************************************************//**
 *                                               KAL_QPost()
 *
 * @brief    Post message on queue.
 *
 * @param    q_handle    Handle of the queue on which to post message.
 *
 * @param    p_msg       Pointer to message to post.
 *
 * @param    opt         Options available:
 *                       KAL_OPT_POST_NONE:     wake only the highest priority task pending on queue.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_WOULD_OVF
 *                           - RTOS_ERR_NO_MORE_RSRC
 *******************************************************************************************************/
void KAL_QPost(KAL_Q_HANDLE q_handle,
               void         *p_msg,
               KAL_OPT      opt,
               RTOS_ERR     *p_err)
{
  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((KAL_Q_HANDLE_IS_NULL(q_handle) == DEF_NO), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET((opt == KAL_OPT_POST_NONE), *p_err, RTOS_ERR_INVALID_ARG,; );

    #if (OS_CFG_Q_EN == DEF_ENABLED)                            // Qs are available.
  {
    OSQPost((OS_Q *)q_handle.QObjPtr,
            p_msg,
            0u,
            OS_OPT_POST_1,
            p_err);
    if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
      LOG_DBG(("Call to OSQPost failed with err: ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    }

    return;
  }
    #else
  PP_UNUSED_PARAM(q_handle);
  PP_UNUSED_PARAM(p_msg);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_AVAIL);

  return;
    #endif
}

/********************************************************************************************************
 *                                           MON API FUNCTIONS
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               KAL_MonCreate()
 *
 * @brief    Create a monitor.
 *
 * @param    p_name      Pointer to name of the monitor.
 *
 * @param    p_mon_obj   Pointer to monitor object, if any. If DEF_NULL, monitor object will be
 *                       allocated from monitor pool by KAL.
 *
 * @param    p_mon_data  Pointer to data that will be passed to callback functions.
 *
 * @param    p_cfg       Pointer to KAL monitor configuration structure. Reserved for future use, must
 *                       be DEF_NULL.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                           - RTOS_ERR_POOL_EMPTY
 *                           - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                           - RTOS_ERR_SEG_OVF
 *
 * @return   Created monitor handle.
 *
 * @note     (1) This function will initialize KAL if KAL has not yet been initialized.
 *******************************************************************************************************/
KAL_MON_HANDLE KAL_MonCreate(const CPU_CHAR  *p_name,
                             void            *p_mon_obj,
                             void            *p_mon_data,
                             KAL_MON_EXT_CFG *p_cfg,
                             RTOS_ERR        *p_err)
{
  KAL_MON_HANDLE handle = KAL_MonHandleNull;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, handle);

  //                                                               Make sure no unsupported cfg recv.
  RTOS_ASSERT_DBG_ERR_SET((p_cfg == DEF_NULL), *p_err, RTOS_ERR_NOT_SUPPORTED, handle);

  if (KAL_DataPtr == DEF_NULL) {
    KAL_Init(p_err);                                            // KAL initialization (see Note #1).
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return(handle);
    }
  }

    #if (OS_CFG_MON_EN == DEF_ENABLED)
  {
    void *p_mon_obj_local;

    if (p_mon_obj == DEF_NULL) {                                // Allocate mon obj if req'd.
      p_mon_obj_local = Mem_DynPoolBlkGet(&KAL_DataPtr->MonPool,
                                          p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        LOG_DBG(("KAL_MonCreate: failed to allocate mon data."));
        return (handle);
      }
    } else {
      p_mon_obj_local = p_mon_obj;
    }

    OSMonCreate((OS_MON *)  p_mon_obj_local,
                (CPU_CHAR *)p_name,
                p_mon_data,
                p_err);
    if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
      handle.MonObjPtr = p_mon_obj_local;
      LOG_VRB(("KAL_MonCreate call successful."));
    } else {
      LOG_DBG(("Call to OSMonCreate failed with err: ", RTOS_ERR_LOG_ARG_GET(*p_err)));

      if (p_mon_obj == DEF_NULL) {
        RTOS_ERR local_err;

        Mem_DynPoolBlkFree(&KAL_DataPtr->MonPool,
                           p_mon_obj_local,
                           &local_err);
        RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, handle);
      }
    }

    return (handle);
  }
    #else
  PP_UNUSED_PARAM(p_name);
  PP_UNUSED_PARAM(p_mon_obj);
  PP_UNUSED_PARAM(p_mon_data);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_AVAIL);

  return (handle);
    #endif
}

/****************************************************************************************************//**
 *                                               KAL_MonOp()
 *
 * @brief    Execute an operation on a monitor.
 *
 * @param    mon_handle      Handle of the monitor on which operation is executed.
 *
 * @param    p_op_data       Pointer to arg that will be passed to callback functions.
 *
 * @param    on_enter_fnct   Function that will be called on entering the monitor.
 *
 * @param    on_eval_fnct    Function that will be called when evaluating if this task can run.
 *
 * @param    opt             Options for call.
 *                               - KAL_OPT_MON_NONE            No option.
 *                               - KAL_OPT_MON_NO_SCHED        Scheduler will not be called.
 *
 * @param    timeout_ms      Timeout, in milliseconds. A value of KAL_TIMEOUT_INFINITE will never timeout.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_OS_OBJ_DEL
 *                               - RTOS_ERR_ABORT
 *                               - RTOS_ERR_TIMEOUT
 *******************************************************************************************************/
void KAL_MonOp(KAL_MON_HANDLE mon_handle,
               void *p_op_data,
               KAL_MON_RES (*on_enter_fnct)(void *p_mon_data, void *p_op_data),
               KAL_MON_RES (*on_eval_fnct)(void *p_mon_data, void *p_eval_op_data, void *p_scan_op_data),
               KAL_OPT opt,
               KAL_TICK timeout_ms,
               RTOS_ERR *p_err)
{
  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((KAL_MON_HANDLE_IS_NULL(mon_handle) == DEF_NO), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET((DEF_BIT_IS_SET_ANY(opt, (KAL_OPT)(~(KAL_OPT_MON_NONE | KAL_OPT_MON_NO_SCHED))) == DEF_NO), *p_err, RTOS_ERR_INVALID_ARG,; );

    #if (OS_CFG_MON_EN == DEF_ENABLED)
  {
    OS_OPT               opt_os = OS_OPT_POST_NONE;
    KAL_MON_DATA_WRAPPER arg_wrapper;
    CPU_INT32U           timeout_ticks;
    OS_MON_ON_ENTER_PTR  on_enter_local;
    OS_MON_ON_EVAL_PTR   on_eval_local;

    arg_wrapper.DataPtr = p_op_data;
    arg_wrapper.EnterFnct = on_enter_fnct;
    arg_wrapper.EvalFnct = on_eval_fnct;

    if (timeout_ms == KAL_TIMEOUT_INFINITE) {
      timeout_ticks = 0u;
    } else {
      timeout_ticks = KAL_msToTicks(timeout_ms);
    }

    if (DEF_BIT_IS_SET(opt, KAL_OPT_MON_NO_SCHED) == DEF_YES) {
      DEF_BIT_SET(opt_os, OS_OPT_POST_NO_SCHED);
    }

    if (on_enter_fnct != DEF_NULL) {
      on_enter_local = KAL_MonEnterFnct;
    } else {
      on_enter_local = DEF_NULL;
    }

    if (on_eval_fnct != DEF_NULL) {
      on_eval_local = KAL_MonEvalFnct;
    } else {
      on_eval_local = DEF_NULL;
    }

    OSMonOp((OS_MON *)mon_handle.MonObjPtr,
            timeout_ticks,
            &arg_wrapper,
            on_enter_local,
            on_eval_local,
            opt_os,
            p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      LOG_DBG(("Call to OSMonOp failed with err: ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    }

    return;
  }
    #else
  PP_UNUSED_PARAM(mon_handle);
  PP_UNUSED_PARAM(p_op_data);
  PP_UNUSED_PARAM(on_enter_fnct);
  PP_UNUSED_PARAM(on_eval_fnct);
  PP_UNUSED_PARAM(opt);
  PP_UNUSED_PARAM(timeout_ms);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_AVAIL);

  return;
    #endif
}

/****************************************************************************************************//**
 *                                               KAL_MonDel()
 *
 * @brief    Delete a monitor.
 *
 * @param    mon_handle  Handle of the monitor to delete.
 *******************************************************************************************************/
void KAL_MonDel(KAL_MON_HANDLE mon_handle)
{
  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG((KAL_MON_HANDLE_IS_NULL(mon_handle) == DEF_NO), RTOS_ERR_NULL_PTR,; );

    #if ((OS_CFG_MON_EN == DEF_ENABLED) \
  && !defined(OS_SAFETY_CRITICAL_IEC61508))
  {
    RTOS_ERR err;

    (void)OSMonDel((OS_MON *)mon_handle.MonObjPtr,
                   OS_OPT_DEL_ALWAYS,
                   &err);
    //                                                             OSMonDel should not return another err in this case.
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

    Mem_DynPoolBlkFree(&KAL_DataPtr->MonPool,
                       mon_handle.MonObjPtr,
                       &err);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

    return;
  }
    #else
  PP_UNUSED_PARAM(mon_handle);

  RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_NOT_AVAIL,; );
    #endif
}

/********************************************************************************************************
 *                                           DLY API FUNCTIONS
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                                   KAL_Dly()
 *
 * @brief    Delay current task (in milliseconds).
 *
 * @param    dly_ms  Delay value, in milliseconds.
 *******************************************************************************************************/
void KAL_Dly(CPU_INT32U dly_ms)
{
#if (OS_CFG_TICK_EN == DEF_ENABLED)
  CPU_INT32U dly_ticks;
  RTOS_ERR   err;

  dly_ticks = KAL_msToTicks(dly_ms);
  if (dly_ticks == 0u) {
    return;
  }

  OSTimeDly(dly_ticks,
            OS_OPT_TIME_DLY,
            &err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  return;
#else
  PP_UNUSED_PARAM(dly_ms);

  RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_NOT_AVAIL,; );
#endif
}

/****************************************************************************************************//**
 *                                               KAL_DlyTick()
 *
 * @brief    Delay current task (in ticks).
 *
 * @param    dly_ticks   Delay value, in ticks.
 *
 * @param    opt         Options available:
 *                           - KAL_OPT_DLY_NONE:       apply a 'normal' delay.
 *                           - KAL_OPT_DLY:            apply a 'normal' delay.
 *                           - KAL_OPT_DLY_PERIODIC:   apply a periodic delay.
 *
 * @note     (1) When used in PERIODIC mode, it is possible that starving the task calling KAL_DlyTick
 *               leads to an error if delay has already passed. This should be considered a design
 *               flaw more than a critical error. If used in normal DLY mode, there is no reason why
 *               an error could be returned so it is a critical assert instead of a debug one.
 *******************************************************************************************************/
void KAL_DlyTick(KAL_TICK dly_ticks,
                 KAL_OPT  opt)
{
#if (OS_CFG_TICK_EN == DEF_ENABLED)
  OS_OPT   opt_os;
  RTOS_ERR err;

  if (dly_ticks == 0u) {
    return;
  }

  if (DEF_BIT_IS_SET(opt, KAL_OPT_DLY_PERIODIC) == DEF_YES) {
    opt_os = OS_OPT_TIME_PERIODIC;
  } else {
    opt_os = OS_OPT_TIME_DLY;
  }

  OSTimeDly(dly_ticks,
            opt_os,
            &err);

  if (DEF_BIT_IS_SET(opt, KAL_OPT_DLY_PERIODIC) == DEF_YES) {   // See Note #1.
    RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_DBG_FAIL,; );
  } else {
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }

  return;
#else
  PP_UNUSED_PARAM(dly_ticks);
  PP_UNUSED_PARAM(opt);

  RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_NOT_AVAIL,; );
#endif
}

/********************************************************************************************************
 *                                       TASK REG API FUNCTIONS
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           KAL_TaskRegCreate()
 *
 * @brief    Create a task register.
 *
 * @param    p_cfg   Pointer to KAL task register configuration structure.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s) from
 *                   this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_NOT_AVAIL
 *                       - RTOS_ERR_POOL_EMPTY
 *                       - RTOS_ERR_NO_MORE_RSRC
 *                       - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                       - RTOS_ERR_SEG_OVF
 *
 * @return   Created task register's handle.
 *
 * @note     (1) This function will initialize KAL if KAL has not yet been initialized.
 *******************************************************************************************************/
KAL_TASK_REG_HANDLE KAL_TaskRegCreate(KAL_TASK_REG_EXT_CFG *p_cfg,
                                      RTOS_ERR             *p_err)
{
  KAL_TASK_REG_HANDLE handle = KAL_TaskRegHandleNull;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, handle);

  //                                                               Make sure no unsupported cfg recv.
  RTOS_ASSERT_DBG_ERR_SET((p_cfg == DEF_NULL), *p_err, RTOS_ERR_NOT_SUPPORTED, handle);

  if (KAL_DataPtr == DEF_NULL) {
    KAL_Init(p_err);                                            // KAL initialization (see Note #1).
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return(handle);
    }
  }

    #if (OS_CFG_TASK_REG_TBL_SIZE > 0u)
  {
    KAL_TASK_REG_DATA *p_task_reg;

    p_task_reg = (KAL_TASK_REG_DATA *)Mem_DynPoolBlkGet(&KAL_DataPtr->TaskRegPool,
                                                        p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (handle);
    }

    p_task_reg->Id = OSTaskRegGetID(p_err);
    if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
      handle.TaskRegObjPtr = (void *)p_task_reg;
      LOG_VRB(("KAL_TaskRegCreate call successful."));
    } else {
      RTOS_ERR local_err;

      LOG_DBG(("Call to OSTaskRegGetID failed with err: ", RTOS_ERR_LOG_ARG_GET(*p_err)));

      //                                                           Free rsrc to pool.
      Mem_DynPoolBlkFree(&KAL_DataPtr->TaskRegPool,
                         (void *)p_task_reg,
                         &local_err);
      RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, handle);
    }

    return (handle);
  }
    #else
  RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_AVAIL);

  return (handle);
    #endif
}

/****************************************************************************************************//**
 *                                               KAL_TaskRegGet()
 *
 * @brief    Get value from a task register.
 *
 * @param    task_handle         Handle of the task associated to the task register. Current task is used if NULL.
 *
 * @param    task_reg_handle     Handle of the task register to read.
 *
 * @param    p_err               Pointer to the variable that will receive one of the following error
 *                               code(s) from this function:
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_NOT_AVAIL
 *
 * @return   Value read from the task register, if no error.
 *           0, otherwise.
 *******************************************************************************************************/
KAL_TASK_REG KAL_TaskRegGet(KAL_TASK_HANDLE     task_handle,
                            KAL_TASK_REG_HANDLE task_reg_handle,
                            RTOS_ERR            *p_err)
{
  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  RTOS_ASSERT_DBG_ERR_SET((KAL_TASK_REG_HANDLE_IS_NULL(task_reg_handle) == DEF_NO), *p_err, RTOS_ERR_NULL_PTR, 0u);

    #if (OS_CFG_TASK_REG_TBL_SIZE > 0u)
  {
    KAL_TASK_REG_DATA *p_task_reg;
    OS_TCB            *p_task_tcb;
    KAL_TASK_REG      ret_val;

    p_task_reg = (KAL_TASK_REG_DATA *)task_reg_handle.TaskRegObjPtr;

    if (KAL_TASK_HANDLE_IS_NULL(task_handle) == DEF_YES) {
      p_task_tcb = OSTCBCurPtr;                                 // Use cur task if no task handle is provided.
    } else {
      //                                                           Get TCB from task handle provided.
      p_task_tcb = &((KAL_TASK *)task_handle.TaskObjPtr)->TCB;
    }

    ret_val = OSTaskRegGet(p_task_tcb,
                           p_task_reg->Id,
                           p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      LOG_DBG(("Call to OSTaskRegGet failed with err: ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    }

    return (ret_val);
  }
    #else
  PP_UNUSED_PARAM(task_handle);
  PP_UNUSED_PARAM(task_reg_handle);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_AVAIL);

  return (0u);
    #endif
}

/****************************************************************************************************//**
 *                                               KAL_TaskRegSet()
 *
 * @brief    Set value of task register.
 *
 * @param    task_handle         Handle of the task associated to the task register. Current task is used if NULL.
 *
 * @param    task_reg_handle     Handle of the task register to write to.
 *
 * @param    val                 Value to write in the task register.
 *
 * @param    p_err               Pointer to the variable that will receive one of the following error
 *                               code(s) from this function:
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_NOT_AVAIL
 *******************************************************************************************************/
void KAL_TaskRegSet(KAL_TASK_HANDLE     task_handle,
                    KAL_TASK_REG_HANDLE task_reg_handle,
                    KAL_TASK_REG        val,
                    RTOS_ERR            *p_err)
{
  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((KAL_TASK_REG_HANDLE_IS_NULL(task_reg_handle) == DEF_NO), *p_err, RTOS_ERR_NULL_PTR,; );

    #if (OS_CFG_TASK_REG_TBL_SIZE > 0u)
  {
    KAL_TASK_REG_DATA *p_task_reg;
    OS_TCB            *p_task_tcb;

    p_task_reg = (KAL_TASK_REG_DATA *)task_reg_handle.TaskRegObjPtr;

    if (KAL_TASK_HANDLE_IS_NULL(task_handle) == DEF_YES) {
      p_task_tcb = OSTCBCurPtr;                                 // Use cur task if no task handle is provided.
    } else {
      //                                                           Get TCB from task handle provided.
      p_task_tcb = &((KAL_TASK *)task_handle.TaskObjPtr)->TCB;
    }

    OSTaskRegSet(p_task_tcb,
                 p_task_reg->Id,
                 (OS_REG)val,
                 p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      LOG_DBG(("Call to OSTaskRegSet failed with err: ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    }

    return;
  }
    #else
  PP_UNUSED_PARAM(task_handle);
  PP_UNUSED_PARAM(task_reg_handle);
  PP_UNUSED_PARAM(val);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_AVAIL);

  return;
    #endif
}

/********************************************************************************************************
 *                                           TICK API FUNCTIONS
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               KAL_TickGet()
 *
 * @brief    Get value of OS' tick counter.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s) from
 *                   this function:
 *                       - RTOS_ERR_NONE
 *
 * @return   OS tick counter's value.
 *******************************************************************************************************/
KAL_TICK KAL_TickGet(RTOS_ERR *p_err)
{
#if (OS_CFG_TICK_EN == DEF_ENABLED)
  KAL_TICK tick_cnt;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

  tick_cnt = OSTimeGet(p_err);
  //                                                               OSTimeGet should never return another err.
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, 0u);

  return (tick_cnt);
#else
  PP_UNUSED_PARAM(p_err);

  RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_NOT_AVAIL, 0u);

  return (0u);
#endif
}

/****************************************************************************************************//**
 *                                               KAL_TickRateGet()
 *
 * @brief    Get value of OS' tick rate, in Hz.
 *
 * @return   OS tick rate's value.
 *******************************************************************************************************/
KAL_TICK_RATE_HZ KAL_TickRateGet(void)
{
#if (OS_CFG_TICK_EN == DEF_ENABLED)
#ifdef  OS_CFG_COMPAT_INIT
  return (OS_CFG_TICK_RATE_HZ);
#else
  KAL_TICK_RATE_HZ rate;
  RTOS_ERR err;

  rate = (KAL_TICK_RATE_HZ)OSTimeTickRateHzGet(&err);
  RTOS_ASSERT_CRITICAL(DEF_TRUE, RTOS_ERR_FAIL, 0u);

  return (rate);
#endif
#else
  return (0u);
#endif
}

/********************************************************************************************************
 *                                           CPU USAGE API FUNCTION
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           KAL_CPU_UsageGet()
 *
 * @brief    Get value of the CPU Usage.
 *
 * @return   CPU usage's percentage represented as an integer from 0 (0%) to 10000 (100%).
 *******************************************************************************************************/
KAL_CPU_USAGE KAL_CPU_UsageGet(void)
{
  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
#if ((OS_CFG_TASK_PROFILE_EN == DEF_ENABLED) \
  && (OS_CFG_DBG_EN == DEF_ENABLED)          \
  && (OS_CFG_STAT_TASK_EN == DEF_ENABLED))
  {
    KAL_CPU_USAGE cpu_usage;

    cpu_usage = OSStatTaskCPUUsage;

    return (cpu_usage);
  }
#else
  RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_NOT_AVAIL, 0u);

  return (0u);
#endif
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               KAL_Init()
 *
 * @brief    Initializes the Kernel Abstraction Layer.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s) from
 *                   this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                       - RTOS_ERR_SEG_OVF
 *
 * @note     (1) This function will be called by the first function KAL_XXXCreate() called by the
 *               application.
 *******************************************************************************************************/
static void KAL_Init(RTOS_ERR *p_err)
{
  MEM_SEG *p_mem_seg;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  OSSchedLock(p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    OSSchedUnlock(p_err);
    return;
  }

  if (KAL_DataPtr == DEF_NULL) {
    p_mem_seg = Common_MemSegPtrGet();

    KAL_DataPtr = (KAL_DATA *)Mem_SegAlloc("KAL internal data",
                                           p_mem_seg,
                                           sizeof(KAL_DATA),
                                           p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      OSSchedUnlock(p_err);
      return;
    }

#if (OS_CFG_MUTEX_EN == DEF_ENABLED)
    Mem_DynPoolCreate("KAL mutex pool",
                      &KAL_DataPtr->MutexPool,
                      p_mem_seg,
                      sizeof(KAL_LOCK),
                      sizeof(CPU_ALIGN),
                      0u,
                      LIB_MEM_BLK_QTY_UNLIMITED,
                      p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      OSSchedUnlock(p_err);
      return;
    }
#endif

#if (OS_CFG_SEM_EN == DEF_ENABLED)
    Mem_DynPoolCreate("KAL sem pool",
                      &KAL_DataPtr->SemPool,
                      p_mem_seg,
                      sizeof(OS_SEM),
                      sizeof(CPU_ALIGN),
                      0u,
                      LIB_MEM_BLK_QTY_UNLIMITED,
                      p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      OSSchedUnlock(p_err);
      return;
    }
#endif

#if (OS_CFG_TMR_EN == DEF_ENABLED)
    Mem_DynPoolCreate("KAL tmr pool",
                      &KAL_DataPtr->TmrPool,
                      p_mem_seg,
                      sizeof(KAL_TMR),
                      sizeof(CPU_ALIGN),
                      0u,
                      LIB_MEM_BLK_QTY_UNLIMITED,
                      p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      OSSchedUnlock(p_err);
      return;
    }
#endif

#if (OS_CFG_TASK_REG_TBL_SIZE > 0u)
    Mem_DynPoolCreate("KAL task reg pool",
                      &KAL_DataPtr->TaskRegPool,
                      p_mem_seg,
                      sizeof(KAL_TASK_REG_DATA),
                      sizeof(CPU_ALIGN),
                      0u,
                      LIB_MEM_BLK_QTY_UNLIMITED,
                      p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      OSSchedUnlock(p_err);
      return;
    }
#endif

#if (OS_CFG_MON_EN == DEF_ENABLED)
    Mem_DynPoolCreate("KAL mon pool",
                      &KAL_DataPtr->MonPool,
                      p_mem_seg,
                      sizeof(OS_MON),
                      sizeof(CPU_ALIGN),
                      0u,
                      LIB_MEM_BLK_QTY_UNLIMITED,
                      p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      OSSchedUnlock(p_err);
      return;
    }
#endif
    LOG_VRB(("KAL_Init successful."));
  }

  OSSchedUnlock(p_err);
  return;
}

/****************************************************************************************************//**
 *                                           KAL_TmrFnctWrapper()
 *
 * @brief    Wrapper function for timer callback.
 *
 * @param    p_tmr_os    Pointer to OS timer object.
 *
 * @param    p_arg       Pointer to KAL timer object.
 *******************************************************************************************************/

#if (OS_CFG_TMR_EN == DEF_ENABLED)
static void KAL_TmrFnctWrapper(void *p_tmr_os,
                               void *p_arg)
{
  KAL_TMR *p_tmr;

  PP_UNUSED_PARAM(p_tmr_os);

  p_tmr = (KAL_TMR *)p_arg;
  p_tmr->CallbackFnct(p_tmr->CallbackArg);
}
#endif

/****************************************************************************************************//**
 *                                           KAL_MonConvertRes()
 *
 * @brief    Wrapper function for timer callback.
 *
 * @param    res     Monitor result in KAL format.
 *
 * @return   Converted monitor result value.
 *******************************************************************************************************/

#if (OS_CFG_MON_EN == DEF_ENABLED)
static OS_MON_RES KAL_MonConvertRes(KAL_MON_RES res)
{
  OS_MON_RES res_converted = 0u;

  res_converted |= (DEF_BIT_IS_SET(res, KAL_MON_RES_BLOCK) == DEF_YES) ? (OS_MON_RES_BLOCK)       : 0u;
  res_converted |= (DEF_BIT_IS_SET(res, KAL_MON_RES_STOP_EVAL) == DEF_YES) ? (OS_MON_RES_STOP_EVAL)   : 0u;
  res_converted |= (DEF_BIT_IS_SET(res, KAL_MON_RES_ACQUIRE) == DEF_YES) ? (OS_MON_RES_ACQUIRE)     : 0u;
  res_converted |= (DEF_BIT_IS_SET(res, KAL_MON_RES_RELEASE) == DEF_YES) ? (OS_MON_RES_RELEASE)     : 0u;
  res_converted |= (DEF_BIT_IS_SET(res, KAL_MON_RES_SUBSCRIBE) == DEF_YES) ? (OS_MON_RES_SUBSCRIBE)   : 0u;
  res_converted |= (DEF_BIT_IS_SET(res, KAL_MON_RES_UNSUBSCRIBE) == DEF_YES) ? (OS_MON_RES_UNSUBSCRIBE) : 0u;
  res_converted |= (DEF_BIT_IS_SET(res, KAL_MON_RES_TRY_FAIL) == DEF_YES) ? (OS_MON_RES_TRY_FAIL)    : 0u;

  return (res_converted);
}
#endif

/****************************************************************************************************//**
 *                                           KAL_MonEnterFnct()
 *
 * @brief    Wrapper function for monitor enter callback.
 *
 * @param    p_mon       Pointer to OS monitor object.
 *
 * @param    p_op_data   Argument passed to OSMonOp. In this case, wrapper for arguments passed by
 *                       caller of KAL_MonOp().
 *
 * @return   OS monitor result value.
 *******************************************************************************************************/

#if (OS_CFG_MON_EN == DEF_ENABLED)
static OS_MON_RES KAL_MonEnterFnct(OS_MON *p_mon,
                                   void   *p_op_data)
{
  KAL_MON_DATA_WRAPPER *p_arg_wrapper = (KAL_MON_DATA_WRAPPER *)p_op_data;
  KAL_MON_RES          res;

  res = p_arg_wrapper->EnterFnct(p_mon->MonDataPtr,
                                 p_arg_wrapper->DataPtr);
  return (KAL_MonConvertRes(res));
}
#endif

/****************************************************************************************************//**
 *                                               KAL_MonEvalFnct()
 *
 * @brief    Wrapper function for monitor evaluation callback.
 *
 * @param    p_mon           Pointer to OS monitor object.
 *
 * @param    p_eval_op_data  Argument passed to OSMonOp. In this case, wrapper for arguments passed by
 *                           previous caller of KAL_MonOp().
 *
 * @param    p_scan_op_data  Argument passed to OSMonOp. In this case, wrapper for arguments passed by
 *                           current caller of KAL_MonOp().
 *
 * @return   OS monitor result value.
 *******************************************************************************************************/

#if (OS_CFG_MON_EN == DEF_ENABLED)
static OS_MON_RES KAL_MonEvalFnct(OS_MON *p_mon,
                                  void   *p_eval_op_data,
                                  void   *p_scan_op_data)
{
  KAL_MON_DATA_WRAPPER *p_eval_op_data_wrapper = (KAL_MON_DATA_WRAPPER *)p_eval_op_data;
  KAL_MON_DATA_WRAPPER *p_scan_op_data_wrapper = (KAL_MON_DATA_WRAPPER *)p_scan_op_data;
  KAL_MON_RES          res;

  res = p_eval_op_data_wrapper->EvalFnct(p_mon->MonDataPtr,
                                         p_eval_op_data_wrapper->DataPtr,
                                         p_scan_op_data_wrapper->DataPtr);
  return (KAL_MonConvertRes(res));
}
#endif

/****************************************************************************************************//**
 *                                               KAL_msToTicks()
 *
 * @brief    Convert milliseconds value to tick value.
 *
 * @param    ms  Millisecond value to convert.
 *
 * @return   Number of ticks corresponding to the millisecond value, rounded up, if needed.
 *******************************************************************************************************/
// TODO: Get rid of this function and use ms variant of OS when available
static KAL_TICK KAL_msToTicks(CPU_INT32U ms)
{
  KAL_TICK ticks;

#if (OS_CFG_TICK_EN == DEF_ENABLED)
  KAL_TICK_RATE_HZ  tick_rate;

  tick_rate = KAL_TickRateGet();

  ticks = (((ms * tick_rate)  + 1000u - 1u) / 1000u);
#else
  PP_UNUSED_PARAM(ms);

  ticks = 0u;
#endif

  return (ticks);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // defined(RTOS_MODULE_KERNEL_AVAIL))
