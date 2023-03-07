/***************************************************************************//**
 * @file
 * @brief Common Basic Operations
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
 * @defgroup COMMON Common API
 * @brief      Common API
 *
 * @defgroup LIB LIB API
 * @ingroup  COMMON
 * @brief      LIB API
 *
 * @defgroup COMMON_CORE Common Core API
 * @ingroup  COMMON
 * @brief      Common Core API
 *
 * @addtogroup COMMON_CORE
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _COMMON_H_
#define  _COMMON_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>

#include  <common/include/rtos_err.h>
#include  <common/include/lib_mem.h>

#include  <rtos_cfg.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           LOGGING CFG STRUCTURE
 * @brief   Logging configuration
 *******************************************************************************************************/

#if (RTOS_CFG_LOG_EN == DEF_ENABLED)
typedef struct common_cfg_logging {
  CPU_INT16U AsyncBufSize;                                      ///< Size of buf used for asynchronous logging.
} COMMON_CFG_LOGGING;
#endif

/****************************************************************************************************//**
 *                                           INIT CFG STRUCTURE
 * @brief   Initialization configuration
 *******************************************************************************************************/
typedef struct common_init_cfg {
  MEM_SEG            *CommonMemSegPtr;                          ///< Ptr to common module's mem seg.

#if (RTOS_CFG_LOG_EN == DEF_ENABLED)
  COMMON_CFG_LOGGING LoggingCfg;                                ///< Logging sub-module cfg.
  MEM_SEG            *LoggingMemSegPtr;                         ///< Ptr to logging sub-module's mem seg.
#endif
} COMMON_INIT_CFG;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
extern const COMMON_INIT_CFG Common_InitCfgDflt;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#if ((RTOS_CFG_LOG_EN == DEF_ENABLED) \
  && (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED))
void Common_ConfigureLogging(COMMON_CFG_LOGGING *p_cfg);
#endif

#if ((RTOS_CFG_LOG_EN == DEF_ENABLED) \
  && (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED))
void Common_ConfigureMemSegLogging(MEM_SEG *p_mem_seg);
#endif

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void Common_ConfigureMemSeg(MEM_SEG *p_mem_seg);
#endif

void Common_Init(RTOS_ERR *p_err);

#ifdef __cplusplus
}
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                          MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // End of common module include.
