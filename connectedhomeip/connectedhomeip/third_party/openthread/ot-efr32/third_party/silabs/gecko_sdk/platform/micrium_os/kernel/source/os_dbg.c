/***************************************************************************//**
 * @file
 * @brief Kernel - Debugger Constants
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

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const CPU_CHAR *os_dbg__c = "$Id: $";
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                                   DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                               VOLATILE
 *******************************************************************************************************/

#if (OS_CFG_DBG_EN == DEF_ENABLED)
#define  OS_VOLATILE  volatile
#else
#define  OS_VOLATILE
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                               DEBUG DATA
 *******************************************************************************************************/

CPU_INT08U OS_VOLATILE OSDbg_DbgEn = OS_CFG_DBG_EN;             // Debug constants are defined below

#if (OS_CFG_DBG_EN == DEF_ENABLED)
CPU_INT08U OS_VOLATILE OSDbg_ArgChkEn = OS_ARG_CHK_EN;
CPU_INT08U OS_VOLATILE OSDbg_AppHooksEn = OS_CFG_APP_HOOKS_EN;

CPU_INT32U OS_VOLATILE OSDbg_EndiannessTest = 0x12345678LU;     // Variable to test CPU endianness

CPU_INT08U OS_VOLATILE  OSDbg_FlagEn = OS_CFG_FLAG_EN;
OS_FLAG_GRP OS_VOLATILE OSDbg_FlagGrp = { 0u };
#if (OS_CFG_FLAG_EN == DEF_ENABLED)
CPU_INT08U OS_VOLATILE OSDbg_FlagModeClrEn = OS_CFG_FLAG_MODE_CLR_EN;
CPU_INT16U OS_VOLATILE OSDbg_FlagGrpSize = sizeof(OS_FLAG_GRP); // Size in Bytes of OS_FLAG_GRP
CPU_INT16U OS_VOLATILE OSDbg_FlagWidth = sizeof(OS_FLAGS);      // Width (in bytes) of OS_FLAGS
#else
CPU_INT08U OS_VOLATILE OSDbg_FlagModeClrEn = 0u;
CPU_INT16U OS_VOLATILE OSDbg_FlagGrpSize = 0u;
CPU_INT16U OS_VOLATILE OSDbg_FlagWidth = 0u;
#endif

#if (OS_MSG_EN == DEF_ENABLED)
CPU_INT08U OS_VOLATILE OSDbg_MsgEn = 1u;
CPU_INT16U OS_VOLATILE OSDbg_MsgSize = sizeof(OS_MSG);          // OS_MSG size
CPU_INT16U OS_VOLATILE OSDbg_MsgPoolSize = sizeof(OS_MSG_POOL);
CPU_INT16U OS_VOLATILE OSDbg_MsgQSize = sizeof(OS_MSG_Q);
#else
CPU_INT08U OS_VOLATILE OSDbg_MsgEn = 0u;
CPU_INT16U OS_VOLATILE OSDbg_MsgSize = 0u;
CPU_INT16U OS_VOLATILE OSDbg_MsgPoolSize = 0u;
CPU_INT16U OS_VOLATILE OSDbg_MsgQSize = 0u;
#endif

OS_MUTEX OS_VOLATILE   OSDbg_Mutex = { 0u };
CPU_INT08U OS_VOLATILE OSDbg_MutexEn = OS_CFG_MUTEX_EN;
#if (OS_CFG_MUTEX_EN == DEF_ENABLED)
CPU_INT16U OS_VOLATILE OSDbg_MutexSize = sizeof(OS_MUTEX);      // Size in bytes of OS_MUTEX
#else
CPU_INT16U OS_VOLATILE OSDbg_MutexSize = 0u;
#endif

CPU_INT16U OS_VOLATILE OSDbg_PendListSize = sizeof(OS_PEND_LIST);
CPU_INT16U OS_VOLATILE OSDbg_PendObjSize = sizeof(OS_PEND_OBJ);

CPU_INT16U OS_VOLATILE OSDbg_PrioMax = OS_CFG_PRIO_MAX;         // Maximum number of priorities
CPU_INT16U OS_VOLATILE OSDbg_PrioTblSize = sizeof(OSPrioTbl);

CPU_INT16U OS_VOLATILE OSDbg_PtrSize = sizeof(void *);          // Size in Bytes of a pointer

OS_Q OS_VOLATILE       OSDbg_Q = { 0u };
CPU_INT08U OS_VOLATILE OSDbg_QEn = OS_CFG_Q_EN;
#if (OS_CFG_Q_EN == DEF_ENABLED)
CPU_INT16U OS_VOLATILE OSDbg_QSize = sizeof(OS_Q);              // Size in bytes of OS_Q structure
#else
CPU_INT16U OS_VOLATILE OSDbg_QSize = 0u;
#endif

CPU_INT08U OS_VOLATILE OSDbg_SchedRoundRobinEn = OS_CFG_SCHED_ROUND_ROBIN_EN;

OS_SEM OS_VOLATILE     OSDbg_Sem = { 0u };
CPU_INT08U OS_VOLATILE OSDbg_SemEn = OS_CFG_SEM_EN;
#if (OS_CFG_SEM_EN == DEF_ENABLED)
CPU_INT16U OS_VOLATILE OSDbg_SemSize = sizeof(OS_SEM);          // Size in bytes of OS_SEM
#else
CPU_INT16U OS_VOLATILE OSDbg_SemSize = 0u;
#endif

CPU_INT16U OS_VOLATILE OSDbg_RdyList = sizeof(OS_RDY_LIST);
CPU_INT32U OS_VOLATILE OSDbg_RdyListSize = sizeof(OSRdyList);   // Number of bytes in the ready table

CPU_INT08U OS_VOLATILE OSDbg_StkWidth = sizeof(CPU_STK);

CPU_INT08U OS_VOLATILE OSDbg_StatTaskEn = OS_CFG_STAT_TASK_EN;
CPU_INT08U OS_VOLATILE OSDbg_StatTaskStkChkEn = OS_CFG_STAT_TASK_STK_CHK_EN;

CPU_INT08U OS_VOLATILE OSDbg_TaskChangePrioEn = 1u;             // DEPRECATED, always enabled.
CPU_INT08U OS_VOLATILE OSDbg_TaskDelEn = OS_CFG_TASK_DEL_EN;
CPU_INT08U OS_VOLATILE OSDbg_TaskQEn = OS_CFG_TASK_Q_EN;
CPU_INT08U OS_VOLATILE OSDbg_TaskProfileEn = OS_CFG_TASK_PROFILE_EN;
CPU_INT16U OS_VOLATILE OSDbg_TaskRegTblSize = OS_CFG_TASK_REG_TBL_SIZE;
CPU_INT08U OS_VOLATILE OSDbg_TaskSuspendEn = OS_CFG_TASK_SUSPEND_EN;

CPU_INT16U OS_VOLATILE OSDbg_TCBSize = sizeof(OS_TCB);          // Size in Bytes of OS_TCB

CPU_INT16U OS_VOLATILE OSDbg_TickListSize = sizeof(OS_TICK_LIST);

#if defined(OS_CFG_TLS_TBL_SIZE) && (OS_CFG_TLS_TBL_SIZE > 0u)
CPU_INT16U OS_VOLATILE OSDbg_TLS_TblSize = OS_CFG_TLS_TBL_SIZE * sizeof(OS_TLS);
#else
CPU_INT16U OS_VOLATILE OSDbg_TLS_TblSize = 0u;
#endif

#if (OS_CFG_TMR_EN == DEF_ENABLED)
OS_TMR OS_VOLATILE OSDbg_Tmr = { 0u };
#endif

CPU_INT08U OS_VOLATILE OSDbg_TmrEn = OS_CFG_TMR_EN;
#if (OS_CFG_TMR_EN == DEF_ENABLED)
CPU_INT16U OS_VOLATILE OSDbg_TmrSize = sizeof(OS_TMR);
#else
CPU_INT16U OS_VOLATILE OSDbg_TmrSize = 0u;
#endif

CPU_INT16U OS_VOLATILE OSDbg_VersionNbr = RTOS_VERSION;

/********************************************************************************************************
 *                           TOTAL RAM USED BY KERNEL, EXCLUDING CONFIGURATION
 *******************************************************************************************************/

CPU_INT32U OS_VOLATILE const OSDbg_DataSize = 0u

#if (OS_CFG_APP_HOOKS_EN == DEF_ENABLED)
#if (OS_CFG_TASK_STK_REDZONE_EN == DEF_ENABLED)
                                              + sizeof(OS_AppRedzoneHitHookPtr)
#endif
                                              + sizeof(OS_AppTaskCreateHookPtr)
                                              + sizeof(OS_AppTaskDelHookPtr)
                                              + sizeof(OS_AppTaskReturnHookPtr)

                                              + sizeof(OS_AppStatTaskHookPtr)
                                              + sizeof(OS_AppTaskSwHookPtr)
#endif

                                              + sizeof(OSRunning)
                                              + sizeof(OSInitialized)

#ifdef OS_SAFETY_CRITICAL_IEC61508
                                              + sizeof(OSSafetyCriticalStartFlag)
#endif

#if (OS_CFG_FLAG_EN == DEF_ENABLED)
#if (OS_CFG_DBG_EN == DEF_ENABLED)
                                              + sizeof(OSFlagDbgListPtr)
                                              + sizeof(OSFlagQty)
#endif
#endif

#if (OS_CFG_MON_EN == DEF_ENABLED)
#if (OS_CFG_DBG_EN == DEF_ENABLED)
                                              + sizeof(OSMonDbgListPtr)
                                              + sizeof(OSMonQty)
#endif
#endif

#if (OS_MSG_EN == DEF_ENABLED)
                                              + sizeof(OSMsgPool)
#endif

#if (OS_CFG_MUTEX_EN == DEF_ENABLED)
#if (OS_CFG_DBG_EN == DEF_ENABLED)
                                              + sizeof(OSMutexDbgListPtr)
                                              + sizeof(OSMutexQty)
#endif
#endif

                                              + sizeof(OSPrioCur)
                                              + sizeof(OSPrioHighRdy)
                                              + sizeof(OSPrioTbl)

#if (OS_CFG_Q_EN == DEF_ENABLED)
#if (OS_CFG_DBG_EN == DEF_ENABLED)
                                              + sizeof(OSQDbgListPtr)
                                              + sizeof(OSQQty)
#endif
#endif

                                              + sizeof(OSRdyList)

                                              + sizeof(OSSchedLockNestingCtr)

#if (OS_CFG_SCHED_LOCK_TIME_MEAS_EN == DEF_ENABLED)
                                              + sizeof(OSSchedLockTimeBegin)
                                              + sizeof(OSSchedLockTimeMax)
                                              + sizeof(OSSchedLockTimeMaxCur)
#endif

#if (OS_CFG_SCHED_ROUND_ROBIN_EN == DEF_ENABLED)
                                              + sizeof(OSSchedRoundRobinDfltTimeQuanta)
                                              + sizeof(OSSchedRoundRobinEn)
#endif

#if (OS_CFG_SEM_EN == DEF_ENABLED)
#if (OS_CFG_DBG_EN == DEF_ENABLED)
                                              + sizeof(OSSemDbgListPtr)
#endif
                                              + sizeof(OSSemQty)
#endif
#if ((OS_CFG_TASK_PROFILE_EN == DEF_ENABLED) || (OS_CFG_DBG_EN == DEF_ENABLED))
                                              + sizeof(OSTaskCtxSwCtr)
#if (OS_CFG_DBG_EN == DEF_ENABLED)
                                              + sizeof(OSTaskDbgListPtr)
#endif
#endif

                                              + sizeof(OSTaskQty)

#if (OS_CFG_STAT_TASK_EN == DEF_ENABLED)
                                              + sizeof(OSStatResetFlag)
                                              + sizeof(OSStatTaskCPUUsage)
                                              + sizeof(OSStatTaskCPUUsageMax)
                                              + sizeof(OSStatTaskRdy)
                                              + sizeof(OSStatTaskTCB)
#if (OS_CFG_TS_EN == DEF_ENABLED)
                                              + sizeof(OSStatTaskTimeMax)
#endif
#endif

#if (OS_CFG_TMR_EN == DEF_ENABLED)
#if (OS_CFG_DBG_EN == DEF_ENABLED)
                                              + sizeof(OSTmrDbgListPtr)
                                              + sizeof(OSTmrListEntries)
#endif
                                              + sizeof(OSTmrListPtr)
#if (OS_CFG_MUTEX_EN == DEF_ENABLED)
                                              + sizeof(OSTmrMutex)
#endif
#if (OS_CFG_DBG_EN == DEF_ENABLED)
                                              + sizeof(OSTmrQty)
#endif
                                              + sizeof(OSTmrTaskTCB)
#if (OS_CFG_TS_EN == DEF_ENABLED)
                                              + sizeof(OSTmrTaskTimeMax)
#endif
                                              + sizeof(OSTmrTickCtr)
                                              + sizeof(OSTmrUpdateCnt)
                                              + sizeof(OSTmrUpdateCtr)
#endif

#if (OS_CFG_TASK_REG_TBL_SIZE > 0u)
                                              + sizeof(OSTaskRegNextAvailID)
#endif

                                              + sizeof(OSTCBCurPtr)
                                              + sizeof(OSTCBHighRdyPtr);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               OS_Dbg_Init()
 *
 * @brief    This function is used to make sure that debug variables that are unused in the application
 *           are not optimized away. This function might not be necessary for all compilers. In this
 *           case, you should simply DELETE the code in this function while still leaving the
 *           declaration of the function itself.
 *
 * @note     (1) This code doesn't do anything, it simply prevents the compiler from optimizing out the
 *               'const' variables which are declared in this file.
 *
 * @note     (2) You may decide to 'compile out' the code (by using #if 0/#endif) INSIDE the function if
 *               your compiler DOES NOT optimize out the 'const' variables above.
 *******************************************************************************************************/
void OS_Dbg_Init(void)
{
  CPU_INT08U OS_VOLATILE temp08;
  CPU_INT08U OS_VOLATILE temp16;
  CPU_INT08U OS_VOLATILE temp32;

  temp08 = (CPU_INT08U)OSDbg_DbgEn;

  temp32 = (CPU_INT32U)OSDbg_DataSize;

  temp08 = (CPU_INT08U)OSDbg_ArgChkEn;
  temp08 = (CPU_INT08U)OSDbg_AppHooksEn;

  temp32 = (CPU_INT32U)OSDbg_EndiannessTest;

  temp32 = (CPU_INT32U)OSDbg_FlagGrp.Flags;
  temp08 = (CPU_INT08U)OSDbg_FlagEn;
#if (OS_CFG_FLAG_EN == DEF_ENABLED)
  temp08 = (CPU_INT08U)OSDbg_FlagModeClrEn;
  temp16 = (CPU_INT16U)OSDbg_FlagGrpSize;
  temp16 = (CPU_INT16U)OSDbg_FlagWidth;
#endif

  temp08 = (CPU_INT08U)OSDbg_MsgEn;
#if (OS_MSG_EN == DEF_ENABLED)
  temp16 = (CPU_INT16U)OSDbg_MsgSize;
  temp16 = (CPU_INT16U)OSDbg_MsgPoolSize;
  temp16 = (CPU_INT16U)OSDbg_MsgQSize;
#endif

  temp32 = (CPU_INT32U)OSDbg_Mutex.OwnerNestingCtr;
  temp08 = (CPU_INT08U)OSDbg_MutexEn;
#if (OS_CFG_MUTEX_EN == DEF_ENABLED)
  temp16 = (CPU_INT16U)OSDbg_MutexSize;
#endif

  temp16 = (CPU_INT16U)OSDbg_PendListSize;
  temp16 = (CPU_INT16U)OSDbg_PendObjSize;

  temp16 = (CPU_INT16U)OSDbg_PrioMax;
  temp16 = (CPU_INT16U)OSDbg_PrioTblSize;

  temp16 = (CPU_INT16U)OSDbg_PtrSize;

  temp32 = (CPU_INT32U)OSDbg_Q.MsgQ.NbrEntries;
  temp08 = (CPU_INT08U)OSDbg_QEn;
#if (OS_CFG_Q_EN == DEF_ENABLED)
  temp16 = (CPU_INT16U)OSDbg_QSize;
#endif

  temp16 = (CPU_INT16U)OSDbg_SchedRoundRobinEn;

  temp32 = (CPU_INT32U)OSDbg_Sem.Ctr;
  temp08 = (CPU_INT08U)OSDbg_SemEn;
#if (OS_CFG_SEM_EN == DEF_ENABLED)
  temp16 = (CPU_INT16U)OSDbg_SemSize;
#endif

  temp16 = (CPU_INT16U)OSDbg_RdyList;
  temp32 = (CPU_INT32U)OSDbg_RdyListSize;

  temp16 = (CPU_INT16U)OSDbg_StkWidth;

  temp08 = (CPU_INT08U)OSDbg_StatTaskEn;
  temp08 = (CPU_INT08U)OSDbg_StatTaskStkChkEn;

  temp08 = (CPU_INT08U)OSDbg_TaskChangePrioEn;
  temp08 = (CPU_INT08U)OSDbg_TaskDelEn;
  temp08 = (CPU_INT08U)OSDbg_TaskQEn;
  temp08 = (CPU_INT08U)OSDbg_TaskProfileEn;
  temp16 = (CPU_INT16U)OSDbg_TaskRegTblSize;
  temp08 = (CPU_INT08U)OSDbg_TaskSuspendEn;

  temp16 = (CPU_INT16U)OSDbg_TCBSize;

  temp16 = (CPU_INT16U)OSDbg_TickListSize;

  temp08 = (CPU_INT08U)OSDbg_TmrEn;
  temp16 = (CPU_INT16U)OSDbg_TmrSize;
#if (OS_CFG_TMR_EN == DEF_ENABLED)
  temp32 = (CPU_INT32U)OSDbg_Tmr.Opt;
#endif

  temp16 = (CPU_INT16U)OSDbg_VersionNbr;

  PP_UNUSED_PARAM(temp08);                                      // Prevent compiler warning for not using 'temp'
  PP_UNUSED_PARAM(temp16);
  PP_UNUSED_PARAM(temp32);
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_KERNEL_AVAIL))
