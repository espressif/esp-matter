/***************************************************************************//**
 * @file
 * @brief Common Basic Operations
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

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#include  <cpu/include/cpu.h>

#include  <common/include/common.h>
#include  <common/include/rtos_path.h>
#include  <common/include/lib_math.h>
#include  <common/include/lib_mem.h>
#include  <common/include/platform_mgr.h>

#if (RTOS_CFG_LOG_EN == DEF_ENABLED)
#include  <common/include/logging.h>
#endif

#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/platform_mgr/platform_mgr_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                       (COMMON)
#define  RTOS_MODULE_CUR                    RTOS_CFG_MODULE_COMMON

#if (RTOS_CFG_LOG_EN == DEF_ENABLED)
#define  COMMON_INIT_CFG_DFLT               { \
    .CommonMemSegPtr = DEF_NULL,              \
    .LoggingCfg =                             \
    {                                         \
      .AsyncBufSize = 512u                    \
    },                                        \
    .LoggingMemSegPtr = DEF_NULL              \
}
#else
#define  COMMON_INIT_CFG_DFLT               { \
    .CommonMemSegPtr = DEF_NULL               \
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
static CPU_BOOLEAN Common_IsInit = DEF_NO;

const COMMON_INIT_CFG  Common_InitCfgDflt = COMMON_INIT_CFG_DFLT;
static COMMON_INIT_CFG Common_InitCfg = COMMON_INIT_CFG_DFLT;
#else
extern const COMMON_INIT_CFG Common_InitCfg;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           Common_ConfigureLogging()
 *
 * @brief    Configure the properties of the logging module.
 *
 * @param    p_cfg   Pointer to the structure containing the new logging parameters.
 *
 * @note     (1) This function is optional. If it is called, it must be called before Common_Init().
 *               If it is not called, default values will be used to initialize the module.
 *******************************************************************************************************/

#if ((RTOS_CFG_LOG_EN == DEF_ENABLED) \
  && (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED))
void Common_ConfigureLogging(COMMON_CFG_LOGGING *p_cfg)
{
  RTOS_ASSERT_DBG((Common_IsInit == DEF_FALSE), RTOS_ERR_ALREADY_INIT,; );

  RTOS_ASSERT_DBG((p_cfg != DEF_NULL), RTOS_ERR_NULL_PTR,; );

  Common_InitCfg.LoggingCfg = *p_cfg;
}
#endif

/****************************************************************************************************//**
 *                                       Common_ConfigureMemSegLogging()
 *
 * @brief    Configure the memory segment that will be used to allocate internal data needed by the
 *           logging module instead of the default memory segment.
 *
 * @param    p_mem_seg   Pointer to the memory segment from which the internal data will be allocated.
 *                       If DEF_NULL, the internal data will be allocated from the global Heap.
 *
 * @note     (1) This function is optional. If it is called, it must be called before Common_Init().
 *               If it is not called, default values will be used to initialize the module.
 *******************************************************************************************************/

#if ((RTOS_CFG_LOG_EN == DEF_ENABLED) \
  && (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED))
void Common_ConfigureMemSegLogging(MEM_SEG *p_mem_seg)
{
  RTOS_ASSERT_DBG((Common_IsInit == DEF_FALSE), RTOS_ERR_ALREADY_INIT,; );

  Common_InitCfg.LoggingMemSegPtr = p_mem_seg;
}
#endif

/****************************************************************************************************//**
 *                                           Common_ConfigureMemSeg()
 *
 * @brief    Configure the memory segment that will be used to allocate internal data needed by Common
 *           modules instead of the default memory segment.
 *
 * @param    p_mem_seg   Pointer to the memory segment from which the internal data will be allocated.
 *                       If DEF_NULL, the internal data will be allocated from the global Heap.
 *
 * @note     (1) This function is optional. If it is called, it must be called before Common_Init().
 *               If it is not called, default values will be used to initialize the module.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void Common_ConfigureMemSeg(MEM_SEG *p_mem_seg)
{
  RTOS_ASSERT_DBG((Common_IsInit == DEF_FALSE), RTOS_ERR_ALREADY_INIT,; );

  Common_InitCfg.CommonMemSegPtr = p_mem_seg;
}
#endif

/****************************************************************************************************//**
 *                                               Common_Init()
 *
 * @brief    Initializes all the Common modules (Lib Mem, Lib Math, Logging, KAL, platform manager) in
 *           the correct order with the specified configurations.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s) from
 *                   this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                       - RTOS_ERR_SEG_OVF
 *
 * @note     (1) The functions Common_Configure...() can be used to configure more specific properties
 *               of the Common module, when RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN is set to
 *               DEF_DISABLED. If set to DEF_ENABLED, the structure Common_InitCfg needs to be
 *               declared and filled by the application to configure these specific properties for the
 *               module.
 *******************************************************************************************************/
void Common_Init(RTOS_ERR *p_err)
{
#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
  Common_IsInit = DEF_YES;
#endif

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  Mem_Init();
  Math_Init();

#if (RTOS_CFG_LOG_EN == DEF_ENABLED)
  Log_Init((COMMON_CFG_LOGGING *)(&(Common_InitCfg.LoggingCfg)), p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
#endif

  PlatformMgrInit(p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           PRIVATE FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       Common_MemSegLoggingPtrGet()
 *
 * @brief    Obtain the memory segment that must be used for logging.
 *
 * @return   Pointer to memory segment that loggin must use.
 *******************************************************************************************************/

#if (RTOS_CFG_LOG_EN == DEF_ENABLED)
MEM_SEG *Common_MemSegLoggingPtrGet(void)
{
  return (Common_InitCfg.LoggingMemSegPtr);
}
#endif

/****************************************************************************************************//**
 *                                           Common_MemSegPtrGet()
 *
 * @brief    Obtain the memory segment that must be used by Common modules.
 *
 * @return   Pointer to memory segment that Common modules must use.
 *******************************************************************************************************/
MEM_SEG *Common_MemSegPtrGet(void)
{
  return (Common_InitCfg.CommonMemSegPtr);
}
