/***************************************************************************//**
 * @file
 * @brief Kernel - Os Configuration (Application Specifics)
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
 * @note (1) Do NOT modify this file.
 *******************************************************************************************************/

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

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const CPU_CHAR *os_cfg_app__c = "$Id: $";
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                               KERNEL DATA
 *******************************************************************************************************/

#ifdef  OS_CFG_COMPAT_INIT
#if (OS_CFG_ISR_STK_SIZE > 0u)
CPU_STK OSCfg_ISRStk[OS_CFG_ISR_STK_SIZE];
#endif
#else
CPU_STK *OSCfg_ISRStk;
#endif

#if (OS_MSG_EN == DEF_ENABLED)
#ifdef  OS_CFG_COMPAT_INIT
OS_MSG OSCfg_MsgPool[OS_CFG_MSG_POOL_SIZE];
#else
OS_MSG *OSCfg_MsgPool;
#endif
#endif

#if (OS_CFG_STAT_TASK_EN == DEF_ENABLED)
#ifdef  OS_CFG_COMPAT_INIT
CPU_STK OSCfg_StatTaskStk[OS_CFG_STAT_TASK_STK_SIZE];
#else
CPU_STK *OSCfg_StatTaskStk;
#endif
#endif

#if (OS_CFG_TMR_EN == DEF_ENABLED)
#ifdef  OS_CFG_COMPAT_INIT
CPU_STK OSCfg_TmrTaskStk[OS_CFG_TMR_TASK_STK_SIZE];
#else
CPU_STK *OSCfg_TmrTaskStk;
#endif
#endif

/********************************************************************************************************
 *                                           CONFIGURATION VARIABLES
 *******************************************************************************************************/

#ifdef  OS_CFG_COMPAT_INIT
#if (OS_CFG_ISR_STK_SIZE > 0u)
CPU_STK * const    OSCfg_ISRStkBasePtr = &OSCfg_ISRStk[0];
CPU_STK_SIZE const OSCfg_ISRStkSize = OS_CFG_ISR_STK_SIZE;
CPU_INT32U const   OSCfg_ISRStkSizeRAM = sizeof(OSCfg_ISRStk);
#endif
#else
CPU_STK      *OSCfg_ISRStkBasePtr = DEF_NULL;
CPU_STK_SIZE OSCfg_ISRStkSize = 0u;
CPU_INT32U   OSCfg_ISRStkSizeRAM = 0u;
#endif

#if (OS_MSG_EN == DEF_ENABLED)
#ifdef  OS_CFG_COMPAT_INIT
OS_MSG * const    OSCfg_MsgPoolBasePtr = &OSCfg_MsgPool[0];
OS_MSG_SIZE const OSCfg_MsgPoolSize = OS_CFG_MSG_POOL_SIZE;
CPU_INT32U const  OSCfg_MsgPoolSizeRAM = sizeof(OSCfg_MsgPool);
#else
OS_MSG      *OSCfg_MsgPoolBasePtr = DEF_NULL;
OS_MSG_SIZE OSCfg_MsgPoolSize = 0u;
CPU_INT32U  OSCfg_MsgPoolSizeRAM = 0u;
#endif
#endif

#if (OS_CFG_STAT_TASK_EN == DEF_ENABLED)
#ifdef  OS_CFG_COMPAT_INIT
OS_PRIO const      OSCfg_StatTaskPrio = OS_CFG_STAT_TASK_PRIO;
OS_RATE_HZ const   OSCfg_StatTaskRate_Hz = OS_CFG_STAT_TASK_RATE_HZ;
CPU_STK * const    OSCfg_StatTaskStkBasePtr = &OSCfg_StatTaskStk[0];
CPU_STK_SIZE const OSCfg_StatTaskStkLimit = ((OS_CFG_STAT_TASK_STK_SIZE * OS_CFG_TASK_STK_LIMIT_PCT_EMPTY) / 100u);
CPU_STK_SIZE const OSCfg_StatTaskStkSize = OS_CFG_STAT_TASK_STK_SIZE;
CPU_INT32U const   OSCfg_StatTaskStkSizeRAM = sizeof(OSCfg_StatTaskStk);
#else
OS_PRIO      OSCfg_StatTaskPrio = 0u;
OS_RATE_HZ   OSCfg_StatTaskRate_Hz = 0u;
CPU_STK      *OSCfg_StatTaskStkBasePtr = DEF_NULL;
CPU_STK_SIZE OSCfg_StatTaskStkLimit = 0u;
CPU_STK_SIZE OSCfg_StatTaskStkSize = 0u;
CPU_INT32U   OSCfg_StatTaskStkSizeRAM = 0u;
#endif
#endif

CPU_STK_SIZE const OSCfg_StkSizeMin = OS_CFG_STK_SIZE_MIN;

#if (OS_CFG_TMR_EN == DEF_ENABLED)
#ifdef  OS_CFG_COMPAT_INIT
OS_PRIO const      OSCfg_TmrTaskPrio = OS_CFG_TMR_TASK_PRIO;
OS_RATE_HZ const   OSCfg_TmrTaskRate_Hz = OS_CFG_TMR_TASK_RATE_HZ;
CPU_STK * const    OSCfg_TmrTaskStkBasePtr = &OSCfg_TmrTaskStk[0];
CPU_STK_SIZE const OSCfg_TmrTaskStkLimit = ((OS_CFG_TMR_TASK_STK_SIZE * OS_CFG_TASK_STK_LIMIT_PCT_EMPTY) / 100u);
CPU_STK_SIZE const OSCfg_TmrTaskStkSize = OS_CFG_TMR_TASK_STK_SIZE;
CPU_INT32U const   OSCfg_TmrTaskStkSizeRAM = sizeof(OSCfg_TmrTaskStk);
#else
OS_PRIO      OSCfg_TmrTaskPrio = 0u;
OS_RATE_HZ   OSCfg_TmrTaskRate_Hz = 0u;
CPU_STK      *OSCfg_TmrTaskStkBasePtr = DEF_NULL;
CPU_STK_SIZE OSCfg_TmrTaskStkLimit = 0u;
CPU_STK_SIZE OSCfg_TmrTaskStkSize = 0u;
CPU_INT32U   OSCfg_TmrTaskStkSizeRAM = 0u;
#endif
#endif

#if (OS_CFG_TICK_EN == DEF_ENABLED)
#ifdef  OS_CFG_COMPAT_INIT
__USED OS_RATE_HZ const   OSCfg_TickRate_Hz = OS_CFG_TICK_RATE_HZ;
#else
__USED OS_RATE_HZ OSCfg_TickRate_Hz = 0u;
#endif
#endif

/********************************************************************************************************
 *                                       TOTAL RAM USED BY CONFIGURATION
 *******************************************************************************************************/

#ifdef  OS_CFG_COMPAT_INIT
CPU_INT32U const OSCfg_DataSizeRAM = 0u

#if (OS_MSG_EN == DEF_ENABLED)
                                     + sizeof(OSCfg_MsgPool)
#endif

#if (OS_CFG_STAT_TASK_EN == DEF_ENABLED)
                                     + sizeof(OSCfg_StatTaskStk)
#endif

#if (OS_CFG_TMR_EN == DEF_ENABLED)
                                     + sizeof(OSCfg_TmrTaskStk)
#endif

#if (OS_CFG_ISR_STK_SIZE > 0u)
                                     + sizeof(OSCfg_ISRStk)
#endif
;
#else
CPU_INT32U OSCfg_DataSizeRAM = 0u;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               OSCfg_Init()
 *
 * @brief    This function is used to make sure that debug variables that are unused in the application
 *           are not optimized away. This function might not be necessary for all compilers. In this
 *           case, you should simply DELETE the code in this function while still leaving the declaration
 *           of the function itself.
 *
 * @note     (1) This code doesn't do anything, it simply prevents the compiler from optimizing out the
 *               'const' variables which are declared in this file.
 *
 * @note     (2) You may decide to 'compile out' the code (by using #if 0/#endif) INSIDE the function if
 *               your compiler DOES NOT optimize out the 'const' variables above.
 *******************************************************************************************************/
void OSCfg_Init(void)
{
  (void)&OSCfg_DataSizeRAM;

#ifdef  OS_CFG_COMPAT_INIT
#if (OS_CFG_ISR_STK_SIZE > 0u)
  (void)&OSCfg_ISRStkBasePtr;
  (void)&OSCfg_ISRStkSize;
  (void)&OSCfg_ISRStkSizeRAM;
#endif
#else
  (void)&OSCfg_ISRStkBasePtr;
  (void)&OSCfg_ISRStkSize;
  (void)&OSCfg_ISRStkSizeRAM;
#endif

#if (OS_MSG_EN == DEF_ENABLED)
  (void)&OSCfg_MsgPoolSize;
  (void)&OSCfg_MsgPoolSizeRAM;
  (void)&OSCfg_MsgPoolBasePtr;
#endif

#if (OS_CFG_STAT_TASK_EN == DEF_ENABLED)
  (void)&OSCfg_StatTaskPrio;
  (void)&OSCfg_StatTaskRate_Hz;
  (void)&OSCfg_StatTaskStkBasePtr;
  (void)&OSCfg_StatTaskStkLimit;
  (void)&OSCfg_StatTaskStkSize;
  (void)&OSCfg_StatTaskStkSizeRAM;
#endif

  (void)&OSCfg_StkSizeMin;

#if (OS_CFG_TMR_EN == DEF_ENABLED)
  (void)&OSCfg_TmrTaskPrio;
  (void)&OSCfg_TmrTaskRate_Hz;
  (void)&OSCfg_TmrTaskStkBasePtr;
  (void)&OSCfg_TmrTaskStkLimit;
  (void)&OSCfg_TmrTaskStkSize;
  (void)&OSCfg_TmrTaskStkSizeRAM;
#endif
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_KERNEL_AVAIL))
