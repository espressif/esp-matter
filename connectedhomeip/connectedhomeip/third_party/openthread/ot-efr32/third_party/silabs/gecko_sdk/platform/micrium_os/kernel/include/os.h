/***************************************************************************//**
 * @file
 * @brief Kernel
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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
 * @defgroup KERNEL Kernel API
 * @brief      Kernel API
 *
 * @defgroup KERNEL_CORE Kernel Core API
 * @ingroup  KERNEL
 * @brief      Kernel Core API
 *
 * @defgroup KERNEL_EVENT Kernel Event Flag API
 * @ingroup  KERNEL
 * @brief      Kernel Event Flag API
 *
 * @defgroup KERNEL_MEMMNG Kernel Memory Manager API
 * @ingroup  KERNEL
 * @brief      Kernel Memory Manager API
 *
 * @defgroup KERNEL_MSGQ Kernel Message Queue API
 * @ingroup  KERNEL
 * @brief      Kernel Message Queue API
 *
 * @defgroup KERNEL_MON Kernel Monitor API
 * @ingroup  KERNEL
 * @brief      Kernel Monitor API
 *
 * @defgroup KERNEL_MUTEX Kernel Mutex API
 * @ingroup  KERNEL
 * @brief      Kernel Mutex API
 *
 * @defgroup KERNEL_PORT Kernel Port Hooks API
 * @ingroup  KERNEL
 * @brief      Kernel Port Hooks API
 *
 * @defgroup KERNEL_SEM Kernel Semaphore API
 * @ingroup  KERNEL
 * @brief      Kernel Semaphore API
 *
 * @defgroup KERNEL_STAT Kernel Statistic API
 * @ingroup  KERNEL
 * @brief      Kernel Statistic API
 *
 * @defgroup KERNEL_TASKMGNT Kernel Task Management API
 * @ingroup  KERNEL
 * @brief      Kernel Task Management API
 *
 * @defgroup KERNEL_TIMEMGNT Kernel Time Management API
 * @ingroup  KERNEL
 * @brief      Kernel Time Management API
 *
 * @defgroup KERNEL_TIMER Kernel Timer API
 * @ingroup  KERNEL
 * @brief      Kernel Timer API
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                                   MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _OS_H_
#define  _OS_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>

#include  <common/include/lib_def.h>
#include  <common/include/lib_mem.h>
#include  <common/include/lib_math.h>
#include  <common/include/lib_utils.h>
#include  <common/include/toolchains.h>
#include  <common/include/rtos_err.h>
#include  <common/include/rtos_path.h>
#include  <common/include/rtos_opt_def.h>
#include  <common/include/rtos_version.h>
#include  <common/include/rtos_utils.h>

#include  <os_cfg.h>
#include  <rtos_description.h>

#ifdef  OS_CFG_COMPAT_INIT_EN
#if (OS_CFG_COMPAT_INIT_EN == DEF_ENABLED)
#define  OS_CFG_COMPAT_INIT
#endif
#endif

#ifdef OS_CFG_COMPAT_INIT
#include  <os_cfg_app.h>
#endif

#include  <rtos_cfg.h>

#include "os_type.h"
#include "os_port_sel.h"

#ifndef OS_CFG_TICK_EN
#define OS_CFG_TICK_EN OS_CFG_TASK_TICK_EN
#endif

#ifndef OS_CFG_ERRNO_EN
#define OS_CFG_ERRNO_EN  0
#endif

#if (OS_CFG_TICK_EN == DEF_ENABLED)
#include  <sl_sleeptimer.h>
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               TASK DEFAULT CONFIGURATION
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef OS_CFG_TASK_DEL_EN
#define  OS_CFG_TASK_DEL_EN                 DEF_ENABLED
#endif

#ifndef OS_CFG_TASK_SUSPEND_EN
#define  OS_CFG_TASK_SUSPEND_EN             DEF_ENABLED
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MISCELLANEOUS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                                   EXTERNS
 *******************************************************************************************************/

#ifdef   OS_GLOBALS
#define  OS_EXT
#else
#define  OS_EXT  extern
#endif

/********************************************************************************************************
 *                                                   CONST
 *******************************************************************************************************/

#ifdef  OS_CFG_COMPAT_INIT
#define  OS_CONST  const
#else
#define  OS_CONST
#endif

/********************************************************************************************************
 *                                           FEATURE USAGE & SIZE
 *******************************************************************************************************/

#define  OS_PRIO_TBL_SIZE          (((OS_CFG_PRIO_MAX - 1u) / DEF_INT_CPU_NBR_BITS) + 1u)

#define  OS_MSG_EN                 (((OS_CFG_TASK_Q_EN == DEF_ENABLED) || (OS_CFG_Q_EN == DEF_ENABLED)) ? DEF_ENABLED : DEF_DISABLED)

#define  OS_ARG_CHK_EN             (DEF_BIT_IS_SET_ANY(RTOS_CFG_ASSERT_DBG_ARG_CHK_EXT_MASK, (RTOS_CFG_MODULE_KERNEL)))

#define  OS_OBJ_TYPE_REQ           (((OS_CFG_DBG_EN == DEF_ENABLED) || (OS_ARG_CHK_EN == DEF_ENABLED)) \
                                    ? DEF_ENABLED : DEF_DISABLED)

/********************************************************************************************************
 ********************************************************************************************************
 *                                                   DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                           KERNEL VERSION NUMBER
 ********************************************************************************************************
 *******************************************************************************************************/

#define  OS_VERSION                              RTOS_VERSION   // Version of Micrium OS Kernel (Vx.yy.zz times 10000).

/********************************************************************************************************
 *                                               TASK STATUS
 *******************************************************************************************************/

//                                                                 ------------------- KERNEL STATES ------------------
#define  OS_STATE_OS_STOPPED                    (OS_STATE)(0u)
#define  OS_STATE_OS_RUNNING                    (OS_STATE)(1u)

//                                                                 -------------------- TASK STATES -------------------
#define  OS_TASK_STATE_BIT_DLY               (OS_STATE)(0x01u)  // /-------- SUSPENDED bit
#define  OS_TASK_STATE_BIT_PEND              (OS_STATE)(0x02u)  // | /-----  PEND      bit
#define  OS_TASK_STATE_BIT_SUSPENDED         (OS_STATE)(0x04u)  // | | /---  Delayed/Timeout bit
#define  OS_TASK_STATE_RDY                    (OS_STATE)(0u)    // 0 0 0     Ready
#define  OS_TASK_STATE_DLY                    (OS_STATE)(1u)    // 0 0 1     Delayed or Timeout
#define  OS_TASK_STATE_PEND                   (OS_STATE)(2u)    // 0 1 0     Pend
#define  OS_TASK_STATE_PEND_TIMEOUT           (OS_STATE)(3u)    // 0 1 1     Pend + Timeout
#define  OS_TASK_STATE_SUSPENDED              (OS_STATE)(4u)    // 1 0 0     Suspended
#define  OS_TASK_STATE_DLY_SUSPENDED          (OS_STATE)(5u)    // 1 0 1     Suspended + Delayed or Timeout
#define  OS_TASK_STATE_PEND_SUSPENDED         (OS_STATE)(6u)    // 1 1 0     Suspended + Pend
#define  OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED (OS_STATE)(7u)    // 1 1 1     Suspended + Pend + Timeout
#define  OS_TASK_STATE_DEL                    (OS_STATE)(255u)

//                                                                 ------------------ PENDING ON ... ------------------
#define  OS_TASK_PEND_ON_NOTHING              (OS_STATE)(0u)    // Pending on nothing
#define  OS_TASK_PEND_ON_FLAG                 (OS_STATE)(1u)    // Pending on event flag group
#define  OS_TASK_PEND_ON_TASK_Q               (OS_STATE)(2u)    // Pending on message to be sent to task
                                                                // 3 not used.
#define  OS_TASK_PEND_ON_MUTEX                (OS_STATE)(4u)    // Pending on mutual exclusion semaphore
#define  OS_TASK_PEND_ON_Q                    (OS_STATE)(5u)    // Pending on queue
#define  OS_TASK_PEND_ON_SEM                  (OS_STATE)(6u)    // Pending on semaphore
#define  OS_TASK_PEND_ON_TASK_SEM             (OS_STATE)(7u)    // Pending on signal  to be sent to task
#define  OS_TASK_PEND_ON_COND_VAR             (OS_STATE)(8u)    // Pending on condition variable

/********************************************************************************************************
 *                                           TASK PEND STATUS
 *                               (Status codes for OS_TCBs field .PendStatus)
 *******************************************************************************************************/

#define  OS_STATUS_PEND_OK                   (OS_STATUS)(0u)    // Pending status OK, !pending, or pending complete
#define  OS_STATUS_PEND_ABORT                (OS_STATUS)(1u)    // Pending aborted
#define  OS_STATUS_PEND_DEL                  (OS_STATUS)(2u)    // Pending object deleted
#define  OS_STATUS_PEND_TIMEOUT              (OS_STATUS)(3u)    // Pending timed out

/********************************************************************************************************
 *                                               OS OBJECT TYPES
 *
 * Note(s) : (1) OS_OBJ_TYPE_&&& #define values specifically chosen as ASCII representations of the kernel
 *               object types.  Memory displays of kernel objects will display the kernel object TYPEs with
 *               their chosen ASCII names.
 *******************************************************************************************************/

#define  OS_OBJ_TYPE_NONE                    (OS_OBJ_TYPE)CPU_TYPE_CREATE('N', 'O', 'N', 'E')
#define  OS_OBJ_TYPE_FLAG                    (OS_OBJ_TYPE)CPU_TYPE_CREATE('F', 'L', 'A', 'G')
#define  OS_OBJ_TYPE_MEM                     (OS_OBJ_TYPE)CPU_TYPE_CREATE('M', 'E', 'M', ' ')
#define  OS_OBJ_TYPE_MON                     (OS_OBJ_TYPE)CPU_TYPE_CREATE('M', 'O', 'N', ' ')
#define  OS_OBJ_TYPE_MUTEX                   (OS_OBJ_TYPE)CPU_TYPE_CREATE('M', 'U', 'T', 'X')
#define  OS_OBJ_TYPE_Q                       (OS_OBJ_TYPE)CPU_TYPE_CREATE('Q', 'U', 'E', 'U')
#define  OS_OBJ_TYPE_SEM                     (OS_OBJ_TYPE)CPU_TYPE_CREATE('S', 'E', 'M', 'A')
#define  OS_OBJ_TYPE_TASK_MSG                (OS_OBJ_TYPE)CPU_TYPE_CREATE('T', 'M', 'S', 'G')
#define  OS_OBJ_TYPE_TASK_RESUME             (OS_OBJ_TYPE)CPU_TYPE_CREATE('T', 'R', 'E', 'S')
#define  OS_OBJ_TYPE_TASK_SIGNAL             (OS_OBJ_TYPE)CPU_TYPE_CREATE('T', 'S', 'I', 'G')
#define  OS_OBJ_TYPE_TASK_SUSPEND            (OS_OBJ_TYPE)CPU_TYPE_CREATE('T', 'S', 'U', 'S')
#define  OS_OBJ_TYPE_TICK                    (OS_OBJ_TYPE)CPU_TYPE_CREATE('T', 'I', 'C', 'K')
#define  OS_OBJ_TYPE_TMR                     (OS_OBJ_TYPE)CPU_TYPE_CREATE('T', 'M', 'R', ' ')

/********************************************************************************************************
 *                                   POSSIBLE VALUES FOR 'opt' ARGUMENT
 *******************************************************************************************************/

#define  OS_OPT_NONE                         (OS_OPT)(0x0000u)

//                                                                 ------------------ DELETE OPTIONS ------------------
#define  OS_OPT_DEL_NO_PEND                  (OS_OPT)(0x0000u)
#define  OS_OPT_DEL_ALWAYS                   (OS_OPT)(0x0001u)

//                                                                 ------------------- PEND OPTIONS -------------------
#define  OS_OPT_PEND_FLAG_MASK               (OS_OPT)(0x000Fu)
#define  OS_OPT_PEND_FLAG_CLR_ALL            (OS_OPT)(0x0001u)  // Wait for ALL    the bits specified to be CLR
#define  OS_OPT_PEND_FLAG_CLR_AND            (OS_OPT)(0x0001u)

#define  OS_OPT_PEND_FLAG_CLR_ANY            (OS_OPT)(0x0002u)  // Wait for ANY of the bits specified to be CLR
#define  OS_OPT_PEND_FLAG_CLR_OR             (OS_OPT)(0x0002u)

#define  OS_OPT_PEND_FLAG_SET_ALL            (OS_OPT)(0x0004u)  // Wait for ALL    the bits specified to be SET
#define  OS_OPT_PEND_FLAG_SET_AND            (OS_OPT)(0x0004u)

#define  OS_OPT_PEND_FLAG_SET_ANY            (OS_OPT)(0x0008u)  // Wait for ANY of the bits specified to be SET
#define  OS_OPT_PEND_FLAG_SET_OR             (OS_OPT)(0x0008u)

#define  OS_OPT_PEND_FLAG_CONSUME            (OS_OPT)(0x0100u)  // Consume the flags if condition(s) satisfied

#define  OS_OPT_PEND_BLOCKING                (OS_OPT)(0x0000u)
#define  OS_OPT_PEND_NON_BLOCKING            (OS_OPT)(0x8000u)

//                                                                 ---------------- PEND ABORT OPTIONS ----------------
#define  OS_OPT_PEND_ABORT_1                 (OS_OPT)(0x0000u)  // Pend abort a single waiting task
#define  OS_OPT_PEND_ABORT_ALL               (OS_OPT)(0x0100u)  // Pend abort ALL tasks waiting

//                                                                 ------------------- POST OPTIONS -------------------
#define  OS_OPT_POST_NONE                    (OS_OPT)(0x0000u)  // No option selected

#define  OS_OPT_POST_FLAG_SET                (OS_OPT)(0x0000u)  // Post while setting  the flag group
#define  OS_OPT_POST_FLAG_CLR                (OS_OPT)(0x0001u)  // Post while clearing the flag group

#define  OS_OPT_POST_FIFO                    (OS_OPT)(0x0000u)  // Default is to post FIFO
#define  OS_OPT_POST_LIFO                    (OS_OPT)(0x0010u)  // Post to highest priority task waiting
#define  OS_OPT_POST_1                       (OS_OPT)(0x0000u)  // Post message to highest priority task waiting
#define  OS_OPT_POST_ALL                     (OS_OPT)(0x0200u)  // Broadcast message to ALL tasks waiting

#define  OS_OPT_POST_NO_SCHED                (OS_OPT)(0x8000u)  // Do not call the scheduler if this is selected

//                                                                 ------------------- TASK OPTIONS -------------------
#define  OS_OPT_TASK_NONE                    (OS_OPT)(0x0000u)  // No option selected
#define  OS_OPT_TASK_STK_CHK                 (OS_OPT)(0x0001u)  // Enable stack checking for the task
#define  OS_OPT_TASK_STK_CLR                 (OS_OPT)(0x0002u)  // Clear the stack when the task is create
#define  OS_OPT_TASK_SAVE_FP                 (OS_OPT)(0x0004u)  // Save the contents of any floating-point registers
#define  OS_OPT_TASK_NO_TLS                  (OS_OPT)(0x0008u)  // Specifies the task DOES NOT require TLS support

//                                                                 ------------------- TIME OPTIONS -------------------
#define  OS_OPT_TIME_DLY                    ((OS_OPT)DEF_BIT_NONE)
#define  OS_OPT_TIME_TIMEOUT                ((OS_OPT)DEF_BIT_01)
#define  OS_OPT_TIME_PERIODIC               ((OS_OPT)DEF_BIT_03)

#define  OS_OPT_TIME_HMSM_STRICT            ((OS_OPT)DEF_BIT_NONE)
#define  OS_OPT_TIME_HMSM_NON_STRICT        ((OS_OPT)DEF_BIT_04)

#define  OS_OPT_TIME_MASK                   ((OS_OPT)(OS_OPT_TIME_DLY       \
                                                      | OS_OPT_TIME_TIMEOUT \
                                                      | OS_OPT_TIME_PERIODIC))

#define  OS_OPT_TIME_OPTS_MASK                       (OS_OPT_TIME_DLY        \
                                                      | OS_OPT_TIME_TIMEOUT  \
                                                      | OS_OPT_TIME_PERIODIC \
                                                      | OS_OPT_TIME_HMSM_NON_STRICT)

//                                                                 ------------------- TIMER OPTIONS ------------------
#define  OS_OPT_TMR_NONE                          (OS_OPT)(0u)  // No option selected

#define  OS_OPT_TMR_ONE_SHOT                      (OS_OPT)(1u)  // Timer will not auto restart when it expires
#define  OS_OPT_TMR_PERIODIC                      (OS_OPT)(2u)  // Timer will     auto restart when it expires

#define  OS_OPT_TMR_CALLBACK                      (OS_OPT)(3u)  // OSTmrStop() option to call 'callback' w/ timer arg
#define  OS_OPT_TMR_CALLBACK_ARG                  (OS_OPT)(4u)  // OSTmrStop() option to call 'callback' w/ new   arg

/********************************************************************************************************
 *                                               TIMER STATES
 *******************************************************************************************************/

#define  OS_TMR_STATE_UNUSED                    (OS_STATE)(0u)
#define  OS_TMR_STATE_STOPPED                   (OS_STATE)(1u)
#define  OS_TMR_STATE_RUNNING                   (OS_STATE)(2u)
#define  OS_TMR_STATE_COMPLETED                 (OS_STATE)(3u)

/********************************************************************************************************
 *                                         TIMER GET DIFFERENCE TIME
 *******************************************************************************************************/

#if CPU_CFG_TS_TMR_SIZE == CPU_WORD_SIZE_32
#define  OS_TMR_GET_DIFF(A, B)           ((A) >= (B)) ? ((A) -(B)) : (DEF_INT_32U_MAX_VAL - (B) + (A))
#elif CPU_CFG_TS_TMR_SIZE == CPU_WORD_SIZE_64
#define  OS_TMR_GET_DIFF(A, B)           ((A) >= (B)) ? ((A) -(B)) : (DEF_INT_64U_MAX_VAL - (B) + (A))
#endif

/********************************************************************************************************
 *                                               PRIORITY
 *******************************************************************************************************/

//                                                                 Default priority to init task TCB
#define  OS_PRIO_INIT                       (OS_PRIO)(OS_CFG_PRIO_MAX)

/********************************************************************************************************
 *                                           TIMER TICK THRESHOLDS
 *******************************************************************************************************/

//                                                                 Threshold to init previous tick time
#define  OS_TICK_TH_INIT                    (OS_TICK)(DEF_BIT((sizeof(OS_TICK) * DEF_OCTET_NBR_BITS) - 1u))

//                                                                 Threshold to check if tick time already ready
#define  OS_TICK_TH_RDY                     (OS_TICK)(DEF_BIT_FIELD(((sizeof(OS_TICK) * DEF_OCTET_NBR_BITS) / 2u), \
                                                                    ((sizeof(OS_TICK) * DEF_OCTET_NBR_BITS) / 2u)))

/********************************************************************************************************
 *                                               MONITOR RESULTS
 *******************************************************************************************************/

#define  OS_MON_RES_ALLOW                   (OS_MON_RES)(0x0000u)
#define  OS_MON_RES_BLOCK                   (OS_MON_RES)(0x0001u)
#define  OS_MON_RES_STOP_EVAL               (OS_MON_RES)(0x0002u)
#define  OS_MON_RES_ACQUIRE                 (OS_MON_RES)(0x0004u)
#define  OS_MON_RES_RELEASE                 (OS_MON_RES)(0x0008u)
#define  OS_MON_RES_SUBSCRIBE               (OS_MON_RES)(0x0010u)
#define  OS_MON_RES_UNSUBSCRIBE             (OS_MON_RES)(0x0020u)
#define  OS_MON_RES_TRY_FAIL                (OS_MON_RES)(0x0040u)

/********************************************************************************************************
 *                                               STACK REDZONE
 *******************************************************************************************************/

#define  OS_STACK_CHECK_VAL                 0x5432DCBAABCD2345UL
#define  OS_STACK_CHECK_DEPTH               8u

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef struct os_init_cfg OS_INIT_CFG;
typedef struct os_task_cfg OS_TASK_CFG;
typedef struct os_stack_cfg OS_STACK_CFG;

typedef struct os_flag_grp OS_FLAG_GRP;

typedef struct os_msg OS_MSG;
typedef struct os_msg_pool OS_MSG_POOL;
typedef struct os_msg_q OS_MSG_Q;

typedef struct os_mutex OS_MUTEX;

typedef struct os_q OS_Q;

typedef struct os_mon OS_MON;
typedef struct os_mon_data OS_MON_DATA;

typedef struct os_sem OS_SEM;

typedef void (*OS_TASK_PTR)(void *p_arg);

typedef OS_MON_RES (*OS_MON_ON_ENTER_PTR)(OS_MON *p_mon, void *p_data);
typedef OS_MON_RES (*OS_MON_ON_EVAL_PTR)(OS_MON *p_mon, void *p_eval_data, void *p_scan_data);

typedef struct os_tcb OS_TCB;

#if defined(OS_CFG_TLS_TBL_SIZE) && (OS_CFG_TLS_TBL_SIZE > 0u)
typedef void *OS_TLS;

typedef CPU_DATA OS_TLS_ID;

typedef void (*OS_TLS_DESTRUCT_PTR)(OS_TCB    *p_tcb,
                                    OS_TLS_ID id,
                                    OS_TLS    value);
#endif

typedef struct os_rdy_list OS_RDY_LIST;

typedef struct os_tick_list OS_TICK_LIST;

typedef void (*OS_TMR_CALLBACK_PTR)(void *p_tmr, void *p_arg);
typedef struct os_tmr OS_TMR;

typedef struct os_pend_list OS_PEND_LIST;
typedef struct os_pend_obj OS_PEND_OBJ;

#if (OS_CFG_APP_HOOKS_EN == DEF_ENABLED)
typedef void (*OS_APP_HOOK_VOID)(void);
typedef void (*OS_APP_HOOK_TCB)(OS_TCB *p_tcb);
#endif

#if (OS_CFG_PRIO_MAX > DEF_INT_32U_MAX_VAL)
#error "OS_CFG_PRIO_MAX must fit in a 32-bit variable and cannot be higher than 4294967295."
#elif (!MATH_IS_PWR2(OS_CFG_PRIO_MAX))
#error "OS_CFG_PRIO_MAX must have a value that is a power of 2 (8, 16, 32, 64, etc)."
#elif (OS_CFG_PRIO_MAX > DEF_INT_16U_MAX_VAL)
typedef CPU_INT32U OS_PRIO;                                     ///< Priority of a task.
#elif (OS_CFG_PRIO_MAX > DEF_INT_08U_MAX_VAL)
typedef CPU_INT16U OS_PRIO;                                     ///< Priority of a task.
#else
typedef CPU_INT08U OS_PRIO;                                     ///< Priority of a task.
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA STRUCTURES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       KERNEL STACK CONFIGURATION
 *******************************************************************************************************/

struct os_stack_cfg {                                           // Kernel Stack Configuration
  CPU_STK      *StkBasePtr;
  CPU_STK_SIZE StkSize;
};

/********************************************************************************************************
 *                                       KERNEL TASK CONFIGURATION
 *******************************************************************************************************/

struct os_task_cfg {                                            // Kernel Task Configuration
  CPU_STK      *StkBasePtr;
  CPU_STK_SIZE StkSize;
  OS_PRIO      Prio;
  OS_RATE_HZ   RateHz;
};

/********************************************************************************************************
 *                                           RUNTIME CONFIGURATION
 *******************************************************************************************************/

struct os_init_cfg {                                            // Runtime Configuration
  OS_STACK_CFG         ISR;
  OS_MSG_SIZE          MsgPoolSize;
  CPU_STK_SIZE TaskStkLimit;
  OS_TASK_CFG  StatTaskCfg;
  OS_TASK_CFG  TmrTaskCfg;
  MEM_SEG      *MemSeg;
  OS_RATE_HZ           TickRate;
};

/********************************************************************************************************
 *                                               READY LIST
 *******************************************************************************************************/

struct os_rdy_list {
  OS_TCB     *HeadPtr;                                          ///< Pointer to task that will run at selected priority
  OS_TCB     *TailPtr;                                          ///< Pointer to last task          at selected priority
#if (OS_CFG_DBG_EN == DEF_ENABLED)
  OS_OBJ_QTY NbrEntries;                                        ///< Number of entries             at selected priority
#endif
};

/********************************************************************************************************
 *                                               PEND LIST
 *******************************************************************************************************/

struct os_pend_list {
  OS_TCB     *HeadPtr;
  OS_TCB     *TailPtr;
#if (OS_CFG_DBG_EN == DEF_ENABLED)
  OS_OBJ_QTY NbrEntries;
#endif
};

/********************************************************************************************************
 *                                               PEND OBJ
 *
 * Note(s) : (1) The 'os_pend_obj' structure data type is a subset for specific kernel objects' data types:
 *               'os_flag_grp', 'os_mutex', 'os_q', and 'os_sem'. Each specific kernel object data type MUST
 *               define ALL generic OS pend object parameters, synchronized in both the sequential order
 *               and data type of each parameter.
 *
 *               Thus, ANY modification to the sequential order or data types of OS pend object parameters
 *               MUST be appropriately synchronized between the generic OS pend object data type and ALL
 *               specific kernel objects' data types.
 *******************************************************************************************************/

struct os_pend_obj {
#if (OS_OBJ_TYPE_REQ == DEF_ENABLED)
  OS_OBJ_TYPE  Type;
#endif
#if (OS_CFG_DBG_EN == DEF_ENABLED)
  CPU_CHAR     *NamePtr;
#endif
  OS_PEND_LIST PendList;                                        ///< List of tasks pending on object
#if (OS_CFG_DBG_EN == DEF_ENABLED)
  void         *DbgPrevPtr;
  void         *DbgNextPtr;
  CPU_CHAR     *DbgNamePtr;
#endif
};

/********************************************************************************************************
 *                                               EVENT FLAGS
 *
 * Note(s) : (1) See 'PEND OBJ Note #1'.
 *******************************************************************************************************/

struct os_flag_grp {                                            // Event Flag Group
                                                                // ----------------- GENERIC  MEMBERS -----------------
#if (OS_OBJ_TYPE_REQ == DEF_ENABLED)
  OS_OBJ_TYPE  Type;                                            ///< Should be set to OS_OBJ_TYPE_FLAG
#endif
#if (OS_CFG_DBG_EN == DEF_ENABLED)
  CPU_CHAR     *NamePtr;                                        ///< Pointer to Event Flag Name (NUL terminated ASCII)
#endif
  OS_PEND_LIST PendList;                                        ///< List of tasks waiting on event flag group
#if (OS_CFG_DBG_EN == DEF_ENABLED)
  OS_FLAG_GRP  *DbgPrevPtr;
  OS_FLAG_GRP  *DbgNextPtr;
  CPU_CHAR     *DbgNamePtr;
#endif
  //                                                               ----------------- SPECIFIC MEMBERS -----------------
  OS_FLAGS     Flags;                                           ///< 8, 16 or 32 bit flags
#if (OS_CFG_TS_EN == DEF_ENABLED)
  CPU_TS       TS;                                              ///< Timestamp of when last post occurred
#endif
#if (OS_CFG_TRACE_EN == DEF_ENABLED)
  CPU_INT16U   FlagID;                                          ///< Unique ID for third-party debuggers and tracers.
#endif
};

/********************************************************************************************************
 *                                               MESSAGES
 *******************************************************************************************************/

struct os_msg {                                                 // MESSAGE CONTROL BLOCK
  OS_MSG      *NextPtr;                                         ///< Pointer to next message
  void        *MsgPtr;                                          ///< Actual message
  OS_MSG_SIZE MsgSize;                                          ///< Size of the message (in # bytes)
#if (OS_CFG_TS_EN == DEF_ENABLED)
  CPU_TS      MsgTS;                                            ///< Time stamp of when message was sent
#endif
};

struct os_msg_pool {                                            // MESSAGE POOL
  OS_MSG     *NextPtr;                                          ///< Pointer to next message
  OS_MSG_QTY NbrFree;                                           ///< Number of messages available from this pool
  OS_MSG_QTY NbrUsed;                                           ///< Current number of messages used
#if (OS_CFG_DBG_EN == DEF_ENABLED)
  OS_MSG_QTY NbrUsedMax;                                        ///< Peak number of messages used
#endif
};

struct os_msg_q {                                               // MESSAGE QUEUE
  OS_MSG     *InPtr;                                            ///< Pointer to next OS_MSG to be inserted  in   the queue
  OS_MSG     *OutPtr;                                           ///< Pointer to next OS_MSG to be extracted from the queue
  OS_MSG_QTY NbrEntriesSize;                                    ///< Maximum allowable number of entries in the queue
  OS_MSG_QTY NbrEntries;                                        ///< Current number of entries in the queue
#if (OS_CFG_DBG_EN == DEF_ENABLED)
  OS_MSG_QTY NbrEntriesMax;                                     ///< Peak number of entries in the queue
#endif
#if (OS_CFG_TRACE_EN == DEF_ENABLED)
  CPU_INT16U MsgQID;                                            ///< Unique ID for third-party debuggers and tracers.
#endif
};

/********************************************************************************************************
 *                                       MUTUAL EXCLUSION SEMAPHORES
 *
 * Note(s) : (1) See 'PEND OBJ Note #1'.
 *******************************************************************************************************/

struct os_mutex {
  //                                                               ----------------- GENERIC  MEMBERS -----------------
#if (OS_OBJ_TYPE_REQ == DEF_ENABLED)
  OS_OBJ_TYPE    Type;                                          ///< Should be set to OS_OBJ_TYPE_MUTEX
#endif
#if (OS_CFG_DBG_EN == DEF_ENABLED)
  CPU_CHAR       *NamePtr;                                      ///< Pointer to Mutex Name (NUL terminated ASCII)
#endif
  OS_PEND_LIST   PendList;                                      ///< List of tasks waiting on mutex
#if (OS_CFG_DBG_EN == DEF_ENABLED)
  OS_MUTEX       *DbgPrevPtr;
  OS_MUTEX       *DbgNextPtr;
  CPU_CHAR       *DbgNamePtr;
#endif
  //                                                               ----------------- SPECIFIC MEMBERS -----------------
  OS_MUTEX       *MutexGrpNextPtr;
  OS_TCB         *OwnerTCBPtr;
  OS_NESTING_CTR OwnerNestingCtr;                               ///< Mutex is available when the counter is 0
#if (OS_CFG_TS_EN == DEF_ENABLED)
  CPU_TS         TS;
#endif
#if (OS_CFG_TRACE_EN == DEF_ENABLED)
  CPU_INT16U     MutexID;                                       ///< Unique ID for third-party debuggers and tracers.
#endif
};

/********************************************************************************************************
 *                                               MESSAGE QUEUES
 *
 * Note(s) : (1) See 'PEND OBJ Note #1'.
 *******************************************************************************************************/

struct os_q {
  //                                                               ----------------- GENERIC  MEMBERS -----------------
#if (OS_OBJ_TYPE_REQ == DEF_ENABLED)
  OS_OBJ_TYPE  Type;                                            ///< Should be set to OS_OBJ_TYPE_Q
#endif
#if (OS_CFG_DBG_EN == DEF_ENABLED)
  CPU_CHAR     *NamePtr;                                        ///< Pointer to Message Queue Name (NUL terminated ASCII)
#endif
  OS_PEND_LIST PendList;                                        ///< List of tasks waiting on message queue
#if (OS_CFG_DBG_EN == DEF_ENABLED)
  OS_Q         *DbgPrevPtr;
  OS_Q         *DbgNextPtr;
  CPU_CHAR     *DbgNamePtr;
#endif
  //                                                               ----------------- SPECIFIC MEMBERS -----------------
  OS_MSG_Q     MsgQ;                                            ///< List of messages
};

/********************************************************************************************************
 *                                               SEMAPHORES
 *
 * Note(s) : (1) See 'PEND OBJ Note #1'.
 *******************************************************************************************************/

struct os_sem {
  //                                                               ----------------- GENERIC  MEMBERS -----------------
#if (OS_OBJ_TYPE_REQ == DEF_ENABLED)
  OS_OBJ_TYPE  Type;                                            ///< Should be set to OS_OBJ_TYPE_SEM
#endif
#if (OS_CFG_DBG_EN == DEF_ENABLED)
  CPU_CHAR     *NamePtr;                                        ///< Pointer to Semaphore Name (NUL terminated ASCII)
#endif
  OS_PEND_LIST PendList;                                        ///< List of tasks waiting on semaphore
#if (OS_CFG_DBG_EN == DEF_ENABLED)
  OS_SEM       *DbgPrevPtr;
  OS_SEM       *DbgNextPtr;
  CPU_CHAR     *DbgNamePtr;
#endif
  //                                                               ----------------- SPECIFIC MEMBERS -----------------
  OS_SEM_CTR   Ctr;
#if (OS_CFG_TS_EN == DEF_ENABLED)
  CPU_TS       TS;
#endif
#if (OS_CFG_TRACE_EN == DEF_ENABLED)
  CPU_INT16U   SemID;                                           ///< Unique ID for third-party debuggers and tracers.
#endif
};

/********************************************************************************************************
 *                                               MONITORS
 *
 * Note(s) : (1) See 'PEND OBJ Note #1'.
 *******************************************************************************************************/

struct os_mon {                                                 // Monitor
                                                                // ----------------- GENERIC  MEMBERS -----------------
#if (OS_OBJ_TYPE_REQ == DEF_ENABLED)
  OS_OBJ_TYPE  Type;                                            ///< Should be set to OS_OBJ_TYPE_MON
#endif
#if (OS_CFG_DBG_EN == DEF_ENABLED)
  CPU_CHAR     *NamePtr;                                        ///< Pointer to Monitor Name (NUL terminated ASCII)
#endif
  OS_PEND_LIST PendList;                                        ///< List of tasks waiting on monitor
#if (OS_CFG_DBG_EN == DEF_ENABLED)
  OS_MON       *DbgPrevPtr;
  OS_MON       *DbgNextPtr;
  CPU_CHAR     *DbgNamePtr;
#endif
  //                                                               ----------------- SPECIFIC MEMBERS -----------------
  void         *MonDataPtr;
};

struct os_mon_data {                                            // MONITOR EVALUATOR
  void               *p_eval_data;
  OS_MON_ON_EVAL_PTR p_on_eval;
};

/********************************************************************************************************
 *                                           TASK CONTROL BLOCK
 * Note(s) : (1) The position in the os_tcb structure of the three following pointers should not be
 *               modified. Access by offset are done in the port during context switches.
 *******************************************************************************************************/

struct os_tcb {
  //                                                               See note #1
  CPU_STK      *StkPtr;                                         ///< Pointer to current top of stack,
  void         *ExtPtr;                                         ///< Pointer to user definable data for TCB extension
  CPU_STK      *StkLimitPtr;                                    ///< Pointer used to set stack 'watermark' limit
#if (OS_CFG_DBG_EN == DEF_ENABLED)
  CPU_CHAR     *NamePtr;                                        ///< Pointer to task name
#endif

  OS_TCB       *NextPtr;                                        ///< Pointer to next     TCB in the TCB list
  OS_TCB       *PrevPtr;                                        ///< Pointer to previous TCB in the TCB list
  CPU_STK      *StkBasePtr;                                     ///< Pointer to base address of stack

#if defined(OS_CFG_TLS_TBL_SIZE) && (OS_CFG_TLS_TBL_SIZE > 0u)
  OS_TLS TLS_Tbl[OS_CFG_TLS_TBL_SIZE];
#endif

#if (OS_CFG_DBG_EN == DEF_ENABLED)
  OS_TASK_PTR TaskEntryAddr;                                    ///< Pointer to task entry point address
  void        *TaskEntryArg;                                    ///< Argument passed to task when it was created
#endif

  OS_TCB      *PendNextPtr;                                     ///< Pointer to next     TCB in pend list.
  OS_TCB      *PendPrevPtr;                                     ///< Pointer to previous TCB in pend list.
  OS_PEND_OBJ *PendObjPtr;                                      ///< Pointer to object pended on.
  OS_STATE    PendOn;                                           ///< Indicates what task is pending on
  OS_STATUS   PendStatus;                                       ///< Pend status

  OS_STATE    TaskState;                                        ///< See OS_TASK_STATE_xxx
  OS_PRIO     Prio;                                             ///< Task priority (0 == highest)
#if (OS_CFG_MUTEX_EN == DEF_ENABLED)
  OS_PRIO     BasePrio;                                         ///< Base priority (Not inherited)
  OS_MUTEX    *MutexGrpHeadPtr;                                 ///< Owned mutex group head pointer
#endif

  CPU_STK_SIZE StkSize;                                         ///< Size of task stack (in number of stack elements)

  OS_OPT       Opt;                                             ///< Task options as passed by OSTaskCreate()

#if (OS_CFG_TS_EN == DEF_ENABLED)
  CPU_TS       TS;                                              ///< Timestamp
#endif

#if (OS_CFG_TRACE_EN == DEF_ENABLED)
  CPU_INT16U SemID;                                             ///< Unique ID for third-party debuggers and tracers.
#endif

  OS_SEM_CTR SemCtr;                                            ///< Task specific semaphore counter

  //                                                               DELAY / TIMEOUT
#if (OS_CFG_TICK_EN == DEF_ENABLED)
  CPU_BOOLEAN                  IsTickCtrPrevValid;              ///< Flag that indicates if TickCtrPrev is valid
  OS_TICK                      TickCtrPrev;                     ///< Used by OSTimeDlyXX() in PERIODIC mode
  sl_sleeptimer_timer_handle_t TimerHandle;                     ///< Handle to timer
#endif

#if (OS_CFG_SCHED_ROUND_ROBIN_EN == DEF_ENABLED)
  OS_TICK TimeQuanta;
  OS_TICK TimeQuantaCtr;
#endif

#if (OS_MSG_EN == DEF_ENABLED)
  void        *MsgPtr;                                          ///< Message received
  OS_MSG_SIZE MsgSize;
#endif

#if (OS_CFG_TASK_Q_EN == DEF_ENABLED)
  OS_MSG_Q MsgQ;                                                ///< Message queue associated with task
#if (OS_CFG_TASK_PROFILE_EN == DEF_ENABLED)
  CPU_TS   MsgQPendTime;                                        ///< Time it took for signal to be received
  CPU_TS   MsgQPendTimeMax;                                     ///< Max amount of time it took for signal to be received
#endif
#endif

#if (OS_CFG_TASK_REG_TBL_SIZE > 0u)
  OS_REG RegTbl[OS_CFG_TASK_REG_TBL_SIZE];                      ///< Task specific registers
#endif

#if (OS_CFG_FLAG_EN == DEF_ENABLED)
  OS_FLAGS FlagsPend;                                           ///< Event flag(s) to wait on
  OS_FLAGS FlagsRdy;                                            ///< Event flags that made task ready to run
  OS_OPT   FlagsOpt;                                            ///< Options (See OS_OPT_FLAG_xxx)
#endif

#if (OS_CFG_MON_EN == DEF_ENABLED)
  OS_MON_DATA MonData;                                          ///< Monitor data associated with task
#endif

#if (OS_CFG_TASK_SUSPEND_EN == DEF_ENABLED)
  OS_NESTING_CTR SuspendCtr;                                    ///< Nesting counter for OSTaskSuspend()
#endif

#if (OS_CFG_TASK_PROFILE_EN == DEF_ENABLED)
  OS_CPU_USAGE  CPUUsage;                                       ///< CPU Usage of task (0.00-100.00%)
  OS_CPU_USAGE  CPUUsageMax;                                    ///< CPU Usage of task (0.00-100.00%) - Peak
  OS_CTX_SW_CTR CtxSwCtr;                                       ///< Number of time the task was switched in
  CPU_TS        CyclesDelta;                                    ///< value of OS_TS_GET() - .CyclesStart
  CPU_TS        CyclesStart;                                    ///< Cycle counter value at start of task resumption
  OS_CYCLES     CyclesTotal;                                    ///< Total number of # of cycles the task has been running
  OS_CYCLES     CyclesTotalPrev;                                ///< Snapshot of previous # of cycles

  CPU_TS        SemPendTime;                                    ///< Time it took for signal to be received
  CPU_TS        SemPendTimeMax;                                 ///< Max amount of time it took for signal to be received
#endif

#if (OS_CFG_STAT_TASK_STK_CHK_EN == DEF_ENABLED)
  CPU_STK_SIZE StkUsed;                                         ///< Number of stack elements used from the stack
  CPU_STK_SIZE StkFree;                                         ///< Number of stack elements free on   the stack
#endif

#if (OS_CFG_SCHED_LOCK_TIME_MEAS_EN == DEF_ENABLED)
  CPU_TS SchedLockTimeMax;                                      ///< Maximum scheduler lock time
#endif

#if (OS_CFG_DBG_EN == DEF_ENABLED)
  OS_TCB   *DbgPrevPtr;
  OS_TCB   *DbgNextPtr;
  CPU_CHAR *DbgNamePtr;
#endif

#if (OS_CFG_TRACE_EN == DEF_ENABLED)
  CPU_INT16U TaskID;                                            ///< Unique ID for third-party debuggers and tracers.
#endif

#if (OS_CFG_ERRNO_EN == 1)
  int local_errno;
#endif
};

/********************************************************************************************************
 *                                               TICK LIST
 *******************************************************************************************************/

struct os_tick_list {
  OS_TCB     *TCB_Ptr;                                          ///< Pointer to list of tasks in tick list
#if (OS_CFG_DBG_EN == DEF_ENABLED)
  OS_OBJ_QTY NbrEntries;                                        ///< Current number of entries in the tick list
  OS_OBJ_QTY NbrUpdated;                                        ///< Number of entries updated
#endif
};

/********************************************************************************************************
 *                                                   TIMER
 *******************************************************************************************************/

#if (OS_CFG_TMR_EN == DEF_ENABLED)
struct os_tmr {
#if (OS_OBJ_TYPE_REQ == DEF_ENABLED)
  OS_OBJ_TYPE         Type;
#endif
#if (OS_CFG_DBG_EN == DEF_ENABLED)
  CPU_CHAR            *NamePtr;                                 ///< Name to give the timer
#endif
  OS_TMR_CALLBACK_PTR CallbackPtr;                              ///< Function to call when timer expires
  void                *CallbackPtrArg;                          ///< Argument to pass to function when timer expires
  OS_TMR              *NextPtr;                                 ///< Double link list pointers
  OS_TMR              *PrevPtr;
  OS_TICK             Remain;                                   ///< Amount of time remaining before timer expires
  OS_TICK             Dly;                                      ///< Delay before start of repeat
  OS_TICK             Period;                                   ///< Period to repeat timer
  OS_OPT              Opt;                                      ///< Options (see OS_OPT_TMR_xxx)
  OS_STATE            State;
#if (OS_CFG_DBG_EN == DEF_ENABLED)
  OS_TMR              *DbgPrevPtr;
  OS_TMR              *DbgNextPtr;
#endif
};
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 APPLICATION HOOKS --------------------------
#if (OS_CFG_APP_HOOKS_EN == DEF_ENABLED)
#if (OS_CFG_TASK_STK_REDZONE_EN == DEF_ENABLED)
OS_EXT OS_APP_HOOK_TCB OS_AppRedzoneHitHookPtr;
#endif
OS_EXT OS_APP_HOOK_TCB OS_AppTaskCreateHookPtr;
OS_EXT OS_APP_HOOK_TCB OS_AppTaskDelHookPtr;
OS_EXT OS_APP_HOOK_TCB OS_AppTaskReturnHookPtr;

OS_EXT OS_APP_HOOK_VOID OS_AppStatTaskHookPtr;
OS_EXT OS_APP_HOOK_VOID OS_AppTaskSwHookPtr;
#endif
//                                                                 MISCELLANEOUS ------------------------------

OS_EXT OS_STATE OSRunning;                                      ///< Flag indicating the kernel is running
OS_EXT OS_STATE OSInitialized;                                  ///< Flag indicating the kernel is initialized

//                                                                 FLAGS --------------------------------------
#if (OS_CFG_FLAG_EN == DEF_ENABLED)
#if (OS_CFG_DBG_EN == DEF_ENABLED)
OS_EXT OS_FLAG_GRP *OSFlagDbgListPtr;
OS_EXT OS_OBJ_QTY  OSFlagQty;
#endif
#endif

//                                                                 OS_MSG POOL --------------------------------
#if (OS_MSG_EN == DEF_ENABLED)
OS_EXT OS_MSG_POOL OSMsgPool;                                   ///< Pool of OS_MSG
#endif

//                                                                 MUTEX MANAGEMENT ---------------------------
#if (OS_CFG_MUTEX_EN == DEF_ENABLED)
#if (OS_CFG_DBG_EN == DEF_ENABLED)
OS_EXT OS_MUTEX   *OSMutexDbgListPtr;
OS_EXT OS_OBJ_QTY OSMutexQty;                                   ///< Number of mutexes created
#endif
#endif

//                                                                 PRIORITIES ---------------------------------
OS_EXT OS_PRIO  OSPrioCur;                                      ///< Priority of current task
OS_EXT OS_PRIO  OSPrioHighRdy;                                  ///< Priority of highest priority task
extern CPU_DATA OSPrioTbl[OS_PRIO_TBL_SIZE];

//                                                                 QUEUES -------------------------------------
#if (OS_CFG_Q_EN == DEF_ENABLED)
#if (OS_CFG_DBG_EN == DEF_ENABLED)
OS_EXT OS_Q       *OSQDbgListPtr;
OS_EXT OS_OBJ_QTY OSQQty;                                       ///< Number of message queues created
#endif
#endif

//                                                                 READY LIST ---------------------------------
OS_EXT OS_RDY_LIST OSRdyList[OS_CFG_PRIO_MAX];                  ///< Table of tasks ready to run

#ifdef OS_SAFETY_CRITICAL_IEC61508
OS_EXT CPU_BOOLEAN OSSafetyCriticalStartFlag;                   ///< Flag indicating that all init. done
#endif

//                                                                 SCHEDULER ----------------------------------
#if (OS_CFG_SCHED_LOCK_TIME_MEAS_EN == DEF_ENABLED)
OS_EXT CPU_TS_TMR OSSchedLockTimeBegin;                         ///< Scheduler lock time measurement
OS_EXT CPU_TS_TMR OSSchedLockTimeMax;
OS_EXT CPU_TS_TMR OSSchedLockTimeMaxCur;
#endif

OS_EXT OS_NESTING_CTR OSSchedLockNestingCtr;                    ///< Lock nesting level
#if (OS_CFG_SCHED_ROUND_ROBIN_EN == DEF_ENABLED)
OS_EXT OS_TICK     OSSchedRoundRobinDfltTimeQuanta;
OS_EXT CPU_BOOLEAN OSSchedRoundRobinEn;                         ///< Enable/Disable round-robin scheduling
#endif

//                                                                 SEMAPHORES ---------------------------------
#if (OS_CFG_SEM_EN == DEF_ENABLED)
#if (OS_CFG_DBG_EN == DEF_ENABLED)
OS_EXT OS_SEM     *OSSemDbgListPtr;
OS_EXT OS_OBJ_QTY OSSemQty;                                     ///< Number of semaphores created
#endif
#endif

//                                                                 MONITORS -----------------------------------
#if (OS_CFG_MON_EN == DEF_ENABLED)
#if (OS_CFG_DBG_EN == DEF_ENABLED)
OS_EXT OS_MON     *OSMonDbgListPtr;
OS_EXT OS_OBJ_QTY OSMonQty;                                     ///< Number of monitors created
#endif
#endif

//                                                                 STATISTICS ---------------------------------
#if (OS_CFG_STAT_TASK_EN == DEF_ENABLED)
OS_EXT CPU_BOOLEAN  OSStatResetFlag;                            ///< Force the reset of the computed statistics
OS_EXT OS_CPU_USAGE OSStatTaskCPUUsage;                         ///< CPU Usage in %
OS_EXT OS_CPU_USAGE OSStatTaskCPUUsageMax;                      ///< CPU Usage in % (Peak)
OS_EXT CPU_BOOLEAN  OSStatTaskRdy;
OS_EXT OS_TCB       OSStatTaskTCB;
#if (OS_CFG_TS_EN == DEF_ENABLED)
OS_EXT CPU_TS OSStatTaskTimeMax;
#endif
#endif

//                                                                 TASKS --------------------------------------
#if ((OS_CFG_TASK_PROFILE_EN == DEF_ENABLED) || (OS_CFG_DBG_EN == DEF_ENABLED))
OS_EXT OS_CTX_SW_CTR OSTaskCtxSwCtr;                            // Number of context switches
#if (OS_CFG_DBG_EN == DEF_ENABLED)
OS_EXT OS_TCB *OSTaskDbgListPtr;
#endif
#endif

OS_EXT OS_OBJ_QTY OSTaskQty;                                    // Number of tasks created

#if (OS_CFG_TASK_REG_TBL_SIZE > 0u)
OS_EXT OS_REG_ID OSTaskRegNextAvailID;                          // Next available Task Register ID
#endif

#if (OS_CFG_TMR_EN == DEF_ENABLED)                              // TIMERS -------------------------------------
#if (OS_CFG_DBG_EN == DEF_ENABLED)
OS_EXT OS_TMR     *OSTmrDbgListPtr;
OS_EXT OS_OBJ_QTY OSTmrListEntries;                             // Doubly-linked list of timers
#endif
OS_EXT OS_TMR *OSTmrListPtr;
#if (OS_CFG_MUTEX_EN == DEF_ENABLED)                            // Use a Mutex (if available) to protect tmrs
OS_EXT OS_MUTEX OSTmrMutex;
#endif
#if (OS_CFG_DBG_EN == DEF_ENABLED)
OS_EXT OS_OBJ_QTY OSTmrQty;                                     // Number of timers created
#endif
OS_EXT OS_TCB OSTmrTaskTCB;                                     // TCB of timer task
#if (OS_CFG_TS_EN == DEF_ENABLED)
OS_EXT CPU_TS OSTmrTaskTimeMax;
#endif
OS_EXT OS_TICK OSTmrTickCtr;                                    // Current time for the timers
OS_EXT OS_CTR  OSTmrUpdateCnt;                                  // Counter for updating timers
OS_EXT OS_CTR  OSTmrUpdateCtr;
#endif
//                                                                 TCBs ---------------------------------------
OS_EXT OS_TCB *OSTCBCurPtr;                                     // Pointer to currently running TCB
OS_EXT OS_TCB *OSTCBHighRdyPtr;                                 // Pointer to highest priority  TCB

/********************************************************************************************************
 ********************************************************************************************************
 *                                           DEFAULT CONFIGURATIONS
 ********************************************************************************************************
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
extern const OS_INIT_CFG OS_InitCfgDflt;                        // OS (Kernel) default configuration.
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                   EXTERNAL CONFIGURATION VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

extern CPU_STK * OS_CONST    OSCfg_ISRStkBasePtr;
extern CPU_STK_SIZE OS_CONST OSCfg_ISRStkSize;
extern CPU_INT32U OS_CONST   OSCfg_ISRStkSizeRAM;

extern OS_MSG * OS_CONST    OSCfg_MsgPoolBasePtr;
extern OS_MSG_SIZE OS_CONST OSCfg_MsgPoolSize;
extern CPU_INT32U OS_CONST  OSCfg_MsgPoolSizeRAM;

extern OS_PRIO OS_CONST      OSCfg_StatTaskPrio;
extern OS_RATE_HZ OS_CONST   OSCfg_StatTaskRate_Hz;
extern CPU_STK * OS_CONST    OSCfg_StatTaskStkBasePtr;
extern CPU_STK_SIZE OS_CONST OSCfg_StatTaskStkLimit;
extern CPU_STK_SIZE OS_CONST OSCfg_StatTaskStkSize;
extern CPU_INT32U OS_CONST   OSCfg_StatTaskStkSizeRAM;

extern CPU_STK_SIZE const OSCfg_StkSizeMin;

extern OS_PRIO OS_CONST      OSCfg_TmrTaskPrio;
extern OS_RATE_HZ OS_CONST   OSCfg_TmrTaskRate_Hz;
extern CPU_STK * OS_CONST    OSCfg_TmrTaskStkBasePtr;
extern CPU_STK_SIZE OS_CONST OSCfg_TmrTaskStkLimit;
extern CPU_STK_SIZE OS_CONST OSCfg_TmrTaskStkSize;
extern CPU_INT32U OS_CONST   OSCfg_TmrTaskStkSizeRAM;

extern CPU_INT32U OS_CONST OSCfg_DataSizeRAM;

#ifdef  OS_CFG_COMPAT_INIT
extern CPU_STK OSCfg_ISRStk[];
#else
extern CPU_STK *OSCfg_ISRStk;
#endif

#if (OS_MSG_EN == DEF_ENABLED)
#ifdef  OS_CFG_COMPAT_INIT
extern OS_MSG OSCfg_MsgPool[];
#else
extern OS_MSG *OSCfg_MsgPool;
#endif
#endif

#if (OS_CFG_STAT_TASK_EN == DEF_ENABLED)
#ifdef  OS_CFG_COMPAT_INIT
extern CPU_STK OSCfg_StatTaskStk[];
#else
extern CPU_STK *OSCfg_StatTaskStk;
#endif
#endif

#if (OS_CFG_TMR_EN == DEF_ENABLED)
#ifdef  OS_CFG_COMPAT_INIT
extern CPU_STK OSCfg_TmrTaskStk[];
#else
extern CPU_STK *OSCfg_TmrTaskStk;
#endif
#endif

#if (OS_CFG_TICK_EN == DEF_ENABLED)
extern OS_RATE_HZ OS_CONST   OSCfg_TickRate_Hz;
extern CPU_INT32U OS_SleeptimerFrequency_Hz;
#endif

extern CPU_INT32U OSDelayMaxMilli;
extern OS_TICK OSDelayMaxTick;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************************************//**
 *                                               EVENT FLAGS
 * @addtogroup KERNEL_EVENT
 * @{
 *******************************************************************************************************/

#if (OS_CFG_FLAG_EN == DEF_ENABLED)
void OSFlagCreate(OS_FLAG_GRP *p_grp,
                  CPU_CHAR    *p_name,
                  OS_FLAGS    flags,
                  RTOS_ERR    *p_err);

OS_OBJ_QTY OSFlagDel(OS_FLAG_GRP *p_grp,
                     OS_OPT      opt,
                     RTOS_ERR    *p_err);

OS_FLAGS OSFlagPend(OS_FLAG_GRP *p_grp,
                    OS_FLAGS    flags,
                    OS_TICK     timeout,
                    OS_OPT      opt,
                    CPU_TS      *p_ts,
                    RTOS_ERR    *p_err);

OS_OBJ_QTY OSFlagPendAbort(OS_FLAG_GRP *p_grp,
                           OS_OPT      opt,
                           RTOS_ERR    *p_err);

OS_FLAGS OSFlagPendGetFlagsRdy(RTOS_ERR *p_err);

OS_FLAGS OSFlagPost(OS_FLAG_GRP *p_grp,
                    OS_FLAGS    flags,
                    OS_OPT      opt,
                    RTOS_ERR    *p_err);
#endif

///< @}

/****************************************************************************************************//**
 *                                               MONITORS
 * @addtogroup KERNEL_MON
 * @{
 *******************************************************************************************************/

#if (OS_CFG_MON_EN == DEF_ENABLED)
void OSMonCreate(OS_MON   *p_mon,
                 CPU_CHAR *p_name,
                 void     *p_mon_data,
                 RTOS_ERR *p_err);

void OSMonOp(OS_MON              *p_mon,
             OS_TICK             timeout,
             void                *p_arg,
             OS_MON_ON_ENTER_PTR p_on_enter,
             OS_MON_ON_EVAL_PTR  p_on_eval,
             OS_OPT              opt,
             RTOS_ERR            *p_err);

OS_OBJ_QTY OSMonDel(OS_MON   *p_mon,
                    OS_OPT   opt,
                    RTOS_ERR *p_err);
#endif
///< @}

/****************************************************************************************************//**
 *                                       MUTUAL EXCLUSION SEMAPHORES
 * @addtogroup KERNEL_MUTEX
 * @{
 *******************************************************************************************************/

#if (OS_CFG_MUTEX_EN == DEF_ENABLED)
void OSMutexCreate(OS_MUTEX *p_mutex,
                   CPU_CHAR *p_name,
                   RTOS_ERR *p_err);

OS_OBJ_QTY OSMutexDel(OS_MUTEX *p_mutex,
                      OS_OPT   opt,
                      RTOS_ERR *p_err);

void OSMutexPend(OS_MUTEX *p_mutex,
                 OS_TICK  timeout,
                 OS_OPT   opt,
                 CPU_TS   *p_ts,
                 RTOS_ERR *p_err);

OS_OBJ_QTY OSMutexPendAbort(OS_MUTEX *p_mutex,
                            OS_OPT   opt,
                            RTOS_ERR *p_err);

void OSMutexPost(OS_MUTEX *p_mutex,
                 OS_OPT   opt,
                 RTOS_ERR *p_err);
#endif
///< @}

/****************************************************************************************************//**
 *                                               MESSAGE QUEUES
 * @addtogroup KERNEL_MSGQ
 * @{
 *******************************************************************************************************/

#if (OS_CFG_Q_EN == DEF_ENABLED)
void OSQCreate(OS_Q       *p_q,
               CPU_CHAR   *p_name,
               OS_MSG_QTY max_qty,
               RTOS_ERR   *p_err);

OS_OBJ_QTY OSQDel(OS_Q     *p_q,
                  OS_OPT   opt,
                  RTOS_ERR *p_err);

OS_MSG_QTY OSQFlush(OS_Q     *p_q,
                    RTOS_ERR *p_err);

void *OSQPend(OS_Q        *p_q,
              OS_TICK     timeout,
              OS_OPT      opt,
              OS_MSG_SIZE *p_msg_size,
              CPU_TS      *p_ts,
              RTOS_ERR    *p_err);

OS_OBJ_QTY OSQPendAbort(OS_Q     *p_q,
                        OS_OPT   opt,
                        RTOS_ERR *p_err);

void OSQPost(OS_Q        *p_q,
             void        *p_void,
             OS_MSG_SIZE msg_size,
             OS_OPT      opt,
             RTOS_ERR    *p_err);
#endif
///< @}

/****************************************************************************************************//**
 *                                           RUNTIME CONFIGURATION
 * @addtogroup KERNEL_CORE
 * @{
 *******************************************************************************************************/

#ifndef  OS_CFG_COMPAT_INIT
#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void OS_ConfigureISRStk(CPU_STK      *p_stk_base_ptr,
                        CPU_STK_SIZE stk_size);

void OS_ConfigureMemSeg(MEM_SEG *p_mem_seg);

void OS_ConfigureMsgPoolSize(OS_MSG_SIZE msg_pool_size);

void OS_ConfigureStkLimit(CPU_STK_SIZE task_stk_limit);

#if (OS_CFG_STAT_TASK_EN == DEF_ENABLED)
void OS_ConfigureStatTask(OS_TASK_CFG *p_stat_task_cfg);
#endif

#if (OS_CFG_TMR_EN == DEF_ENABLED)
void OS_ConfigureTmrTask(OS_TASK_CFG *p_tmr_task_cfg);
#endif

#if (OS_CFG_TICK_EN == DEF_ENABLED)
void OS_ConfigureTickRate(OS_RATE_HZ tick_rate);
#endif
#endif
#endif

///< @}

/****************************************************************************************************//**
 *                                               SEMAPHORES
 * @addtogroup KERNEL_SEM
 * @{
 *******************************************************************************************************/

#if (OS_CFG_SEM_EN == DEF_ENABLED)
void OSSemCreate(OS_SEM     *p_sem,
                 CPU_CHAR   *p_name,
                 OS_SEM_CTR cnt,
                 RTOS_ERR   *p_err);

OS_OBJ_QTY OSSemDel(OS_SEM   *p_sem,
                    OS_OPT   opt,
                    RTOS_ERR *p_err);

OS_SEM_CTR OSSemPend(OS_SEM   *p_sem,
                     OS_TICK  timeout,
                     OS_OPT   opt,
                     CPU_TS   *p_ts,
                     RTOS_ERR *p_err);

OS_OBJ_QTY OSSemPendAbort(OS_SEM   *p_sem,
                          OS_OPT   opt,
                          RTOS_ERR *p_err);

OS_SEM_CTR OSSemPost(OS_SEM   *p_sem,
                     OS_OPT   opt,
                     RTOS_ERR *p_err);

void OSSemSet(OS_SEM     *p_sem,
              OS_SEM_CTR cnt,
              RTOS_ERR   *p_err);
#endif
///< @}

/****************************************************************************************************//**
 *                                               TASK MANAGEMENT
 * @addtogroup KERNEL_TASKMGNT
 * @{
 *******************************************************************************************************/

void OSTaskChangePrio(OS_TCB   *p_tcb,
                      OS_PRIO  prio_new,
                      RTOS_ERR *p_err);

void OSTaskCreate(OS_TCB       *p_tcb,
                  CPU_CHAR     *p_name,
                  OS_TASK_PTR  p_task,
                  void         *p_arg,
                  OS_PRIO      prio,
                  CPU_STK      *p_stk_base,
                  CPU_STK_SIZE stk_limit,
                  CPU_STK_SIZE stk_size,
                  OS_MSG_QTY   q_size,
                  OS_TICK      time_quanta,
                  void         *p_ext,
                  OS_OPT       opt,
                  RTOS_ERR     *p_err);

#if (OS_CFG_TASK_DEL_EN == DEF_ENABLED)
void OSTaskDel(OS_TCB   *p_tcb,
               RTOS_ERR *p_err);
#endif

#if (OS_CFG_TASK_Q_EN == DEF_ENABLED)
OS_MSG_QTY OSTaskQFlush(OS_TCB   *p_tcb,
                        RTOS_ERR *p_err);

void *OSTaskQPend(OS_TICK     timeout,
                  OS_OPT      opt,
                  OS_MSG_SIZE *p_msg_size,
                  CPU_TS      *p_ts,
                  RTOS_ERR    *p_err);

CPU_BOOLEAN OSTaskQPendAbort(OS_TCB   *p_tcb,
                             OS_OPT   opt,
                             RTOS_ERR *p_err);

void OSTaskQPost(OS_TCB      *p_tcb,
                 void        *p_void,
                 OS_MSG_SIZE msg_size,
                 OS_OPT      opt,
                 RTOS_ERR    *p_err);
#endif

#if (OS_CFG_TASK_REG_TBL_SIZE > 0u)
OS_REG OSTaskRegGet(OS_TCB    *p_tcb,
                    OS_REG_ID id,
                    RTOS_ERR  *p_err);

OS_REG_ID OSTaskRegGetID(RTOS_ERR *p_err);

void OSTaskRegSet(OS_TCB    *p_tcb,
                  OS_REG_ID id,
                  OS_REG    value,
                  RTOS_ERR  *p_err);
#endif

#if (OS_CFG_TASK_SUSPEND_EN == DEF_ENABLED)
void OSTaskResume(OS_TCB   *p_tcb,
                  RTOS_ERR *p_err);

void OSTaskSuspend(OS_TCB   *p_tcb,
                   RTOS_ERR *p_err);
#endif

OS_SEM_CTR OSTaskSemPend(OS_TICK  timeout,
                         OS_OPT   opt,
                         CPU_TS   *p_ts,
                         RTOS_ERR *p_err);

CPU_BOOLEAN OSTaskSemPendAbort(OS_TCB   *p_tcb,
                               OS_OPT   opt,
                               RTOS_ERR *p_err);

OS_SEM_CTR OSTaskSemPost(OS_TCB   *p_tcb,
                         OS_OPT   opt,
                         RTOS_ERR *p_err);

OS_SEM_CTR OSTaskSemSet(OS_TCB     *p_tcb,
                        OS_SEM_CTR cnt,
                        RTOS_ERR   *p_err);

#if (OS_CFG_STAT_TASK_STK_CHK_EN == DEF_ENABLED)
void OSTaskStkChk(OS_TCB       *p_tcb,
                  CPU_STK_SIZE *p_free,
                  CPU_STK_SIZE *p_used,
                  RTOS_ERR     *p_err);
#endif

#if (OS_CFG_TASK_STK_REDZONE_EN == DEF_ENABLED)
CPU_BOOLEAN OSTaskStkRedzoneChk(OS_TCB *p_tcb);
#endif

#if (OS_CFG_SCHED_ROUND_ROBIN_EN == DEF_ENABLED)
void OSTaskTimeQuantaSet(OS_TCB   *p_tcb,
                         OS_TICK  time_quanta,
                         RTOS_ERR *p_err);
#endif

void OS_TaskReturn(void);

///< @}

#if (OS_CFG_TICK_EN == DEF_ENABLED)
/*****************************************************************************************************//**
 *                                               TIME MANAGEMENT
 * @addtogroup KERNEL_TIMEMGNT
 * @{
 *******************************************************************************************************/

OS_RATE_HZ OSTimeTickRateHzGet(RTOS_ERR *p_err);

void OSTimeDly(OS_TICK  dly,
               OS_OPT   opt,
               RTOS_ERR *p_err);

void OSTimeDlyHMSM(CPU_INT16U hours,
                   CPU_INT16U minutes,
                   CPU_INT16U seconds,
                   CPU_INT32U milli,
                   OS_OPT     opt,
                   RTOS_ERR   *p_err);

void OSTimeDlyResume(OS_TCB   *p_tcb,
                     RTOS_ERR *p_err);

OS_TICK OSTimeGet(RTOS_ERR *p_err);
#endif

///< @}

/****************************************************************************************************//**
 *                                           TIMER MANAGEMENT
 * @addtogroup KERNEL_TIMER
 * @{
 *******************************************************************************************************/

#if (OS_CFG_TMR_EN == DEF_ENABLED)
void OSTmrCreate(OS_TMR              *p_tmr,
                 CPU_CHAR            *p_name,
                 OS_TICK             dly,
                 OS_TICK             period,
                 OS_OPT              opt,
                 OS_TMR_CALLBACK_PTR p_callback,
                 void                *p_callback_arg,
                 RTOS_ERR            *p_err);

CPU_BOOLEAN OSTmrDel(OS_TMR   *p_tmr,
                     RTOS_ERR *p_err);

void OSTmrSet(OS_TMR              *p_tmr,
              OS_TICK             dly,
              OS_TICK             period,
              OS_TMR_CALLBACK_PTR p_callback,
              void                *p_callback_arg,
              RTOS_ERR            *p_err);

OS_TICK OSTmrRemainGet(OS_TMR   *p_tmr,
                       RTOS_ERR *p_err);

CPU_BOOLEAN OSTmrStart(OS_TMR   *p_tmr,
                       RTOS_ERR *p_err);

OS_STATE OSTmrStateGet(OS_TMR   *p_tmr,
                       RTOS_ERR *p_err);

CPU_BOOLEAN OSTmrStop(OS_TMR   *p_tmr,
                      OS_OPT   opt,
                      void     *p_callback_arg,
                      RTOS_ERR *p_err);
#endif

///< @}

/********************************************************************************************************
 *                                   TASK LOCAL STORAGE (TLS) SUPPORT
 *******************************************************************************************************/

#if defined(OS_CFG_TLS_TBL_SIZE) && (OS_CFG_TLS_TBL_SIZE > 0u)
OS_TLS_ID OS_TLS_GetID(RTOS_ERR *p_err);

OS_TLS OS_TLS_GetValue(OS_TCB    *p_tcb,
                       OS_TLS_ID id,
                       RTOS_ERR  *p_err);

void OS_TLS_Init(RTOS_ERR *p_err);

void OS_TLS_SetValue(OS_TCB    *p_tcb,
                     OS_TLS_ID id,
                     OS_TLS    value,
                     RTOS_ERR  *p_err);

void OS_TLS_SetDestruct(OS_TLS_ID           id,
                        OS_TLS_DESTRUCT_PTR p_destruct,
                        RTOS_ERR            *p_err);

void OS_TLS_TaskCreate(OS_TCB *p_tcb);

void OS_TLS_TaskDel(OS_TCB *p_tcb);

void OS_TLS_TaskSw(void);
#endif

/****************************************************************************************************//**
 *                                               MISCELLANEOUS
 * @addtogroup KERNEL_CORE
 * @{
 *******************************************************************************************************/

void OSInit(RTOS_ERR *p_err);

void OSIntEnter(void);

void OSIntExit(void);

#if (OS_CFG_SCHED_ROUND_ROBIN_EN == DEF_ENABLED)
void OSSchedRoundRobinCfg(CPU_BOOLEAN en,
                          OS_TICK     dflt_time_quanta,
                          RTOS_ERR    *p_err);

void OSSchedRoundRobinYield(RTOS_ERR *p_err);
#endif

void OSSched(void);

void OSSchedLock(RTOS_ERR *p_err);

void OSSchedUnlock(RTOS_ERR *p_err);

void OSStart(RTOS_ERR *p_err);

CPU_INT16U OSVersion(RTOS_ERR *p_err);

CPU_BOOLEAN OSIsIdle(void);

///< @}

/****************************************************************************************************//**
 *                                           KERNEL STATISTICS
 * @addtogroup KERNEL_STAT
 * @{
 *******************************************************************************************************/

#if (OS_CFG_STAT_TASK_EN == DEF_ENABLED)
void OSStatReset(RTOS_ERR *p_err);
#endif

///< @}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   PORT SPECIFIC FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void OSInitHook(void);

#if (OS_CFG_TASK_STK_REDZONE_EN == DEF_ENABLED)
void OSRedzoneHitHook(OS_TCB *p_tcb);
#endif

void OSStatTaskHook(void);

void OSTaskCreateHook(OS_TCB *p_tcb);

void OSTaskDelHook(OS_TCB *p_tcb);

void OSTaskReturnHook(OS_TCB *p_tcb);

CPU_STK *OSTaskStkInit(OS_TASK_PTR  p_task,
                       void         *p_arg,
                       CPU_STK      *p_stk_base,
                       CPU_STK      *p_stk_limit,
                       CPU_STK_SIZE stk_size,
                       OS_OPT       opt);

void OSTaskSwHook(void);
#ifdef __cplusplus
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           CONFIGURATION ERRORS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                               MISCELLANEOUS
 *******************************************************************************************************/

#ifndef OS_CFG_APP_HOOKS_EN
#error  "OS_CFG.H, Missing OS_CFG_APP_HOOKS_EN: Enable (1) or Disable (0) application specific hook functions"
#endif

#ifndef OS_CFG_DBG_EN
#error  "OS_CFG.H, Missing OS_CFG_DBG_EN: Allows you to include variables for debugging or not"
#endif

#if     OS_CFG_PRIO_MAX < 8u
#error  "OS_CFG.H,         OS_CFG_PRIO_MAX must be >= 8"
#endif

#ifndef OS_CFG_SCHED_LOCK_TIME_MEAS_EN
#error  "OS_CFG.H, Missing OS_CFG_SCHED_LOCK_TIME_MEAS_EN: Include code to measure scheduler lock time"
#else
    #if    (OS_CFG_SCHED_LOCK_TIME_MEAS_EN == DEF_ENABLED) \
  && (OS_CFG_TS_EN == DEF_DISABLED)
    #error  "OS_CFG.H,         OS_CFG_TS_EN must be Enabled (1) to measure scheduler lock time"
    #endif
#endif

#ifndef OS_CFG_SCHED_ROUND_ROBIN_EN
#error  "OS_CFG.H, Missing OS_CFG_SCHED_ROUND_ROBIN_EN: Include code for Round Robin Scheduling"
#else
#if   ((OS_CFG_SCHED_ROUND_ROBIN_EN == DEF_ENABLED) \
  && (OS_CFG_TICK_EN == DEF_DISABLED))
#error  "OS_CFG.H, OS_CFG_TICK_EN must be enabled to use the Round Robin Scheduler."
#endif
#endif

#ifndef OS_CFG_STK_SIZE_MIN
#error  "OS_CFG.H, Missing OS_CFG_STK_SIZE_MIN: Determines the minimum size for a task stack"
#endif

#ifndef OS_CFG_TS_EN
#error  "OS_CFG.H, Missing OS_CFG_TS_EN: Determines whether time stamping is enabled"
#else
    #if    (OS_CFG_TS_EN == DEF_ENABLED) \
  && (CPU_CFG_TS_EN == DEF_DISABLED)
    #error  "CPU_CFG.H,        CPU_CFG_TS_32_EN or CPU_CFG_TS_64_EN must be Enabled (1) to use time stamp feature"
    #endif
#endif

#ifndef OS_CFG_TICK_EN
#error  "OS_CFG.H, Missing OS_CFG_TICK_EN: Determines whether to use the ticking feature"
#endif

/********************************************************************************************************
 *                                               EVENT FLAGS
 *******************************************************************************************************/

#ifndef OS_CFG_FLAG_EN
#error  "OS_CFG.H, Missing OS_CFG_FLAG_EN: Enable (1) or Disable (0) code generation for Event Flags"
#else
    #ifndef OS_CFG_FLAG_MODE_CLR_EN
    #error  "OS_CFG.H, Missing OS_CFG_FLAG_MODE_CLR_EN: Include code for Wait on Clear EVENT FLAGS"
    #endif
#endif

/********************************************************************************************************
 *                                       MUTUAL EXCLUSION SEMAPHORES
 *******************************************************************************************************/

#ifndef OS_CFG_MUTEX_EN
#error  "OS_CFG.H, Missing OS_CFG_MUTEX_EN: Enable (1) or Disable (0) code generation for MUTEX"
#endif

/********************************************************************************************************
 *                                               MESSAGE QUEUES
 *******************************************************************************************************/

#ifndef OS_CFG_Q_EN
#error  "OS_CFG.H, Missing OS_CFG_Q_EN: Enable (1) or Disable (0) code generation for QUEUES"
#endif

/********************************************************************************************************
 *                                               SEMAPHORES
 *******************************************************************************************************/

#ifndef OS_CFG_SEM_EN
#error  "OS_CFG.H, Missing OS_CFG_SEM_EN: Enable (1) or Disable (0) code generation for SEMAPHORES"
#endif

/********************************************************************************************************
 *                                               TASK MANAGEMENT
 *******************************************************************************************************/

#ifndef OS_CFG_STAT_TASK_EN
#error  "OS_CFG.H, Missing OS_CFG_STAT_TASK_EN: Enable (1) or Disable(0) the statistics task"
#else
#if   ((OS_CFG_STAT_TASK_EN == DEF_ENABLED) \
  && (OS_CFG_TICK_EN == DEF_DISABLED))
#error  "OS_CFG.H, OS_CFG_TICK_EN must be enabled to use the Statistics Task."
#endif
#endif

#ifndef OS_CFG_STAT_TASK_STK_CHK_EN
#error  "OS_CFG.H, Missing OS_CFG_STAT_TASK_STK_CHK_EN: Check task stacks from statistics task"
#endif

#ifndef OS_CFG_TASK_Q_EN
#error  "OS_CFG.H, Missing OS_CFG_TASK_Q_EN: Include code for OSTaskQxxx()"
#endif

#ifndef OS_CFG_TASK_PROFILE_EN
#error  "OS_CFG.H, Missing OS_CFG_TASK_PROFILE_EN: Include code for task profiling"
#else
#if    (OS_CFG_TASK_PROFILE_EN == DEF_ENABLED) \
  && (OS_CFG_STAT_TASK_EN == DEF_DISABLED)
#error  "OS_CFG.H,         OS_CFG_STAT_TASK_EN must be Enabled (1) to use the task profiling feature"
#endif
#endif

#ifndef OS_CFG_TASK_REG_TBL_SIZE
#error  "OS_CFG.H, Missing OS_CFG_TASK_REG_TBL_SIZE: Include support for task specific registers"
#endif

/********************************************************************************************************
 *                                           TIMER MANAGEMENT
 *******************************************************************************************************/

#ifndef OS_CFG_TMR_EN
#error  "OS_CFG.H, Missing OS_CFG_TMR_EN: When (1) enables code generation for Timer Management"
#else
#if   ((OS_CFG_TMR_EN == DEF_ENABLED) \
  && (OS_CFG_TICK_EN == DEF_DISABLED))
#error  "OS_CFG.H, OS_CFG_TICK_EN must be enabled to use Timers."
#endif
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // End of Kernel module include.
