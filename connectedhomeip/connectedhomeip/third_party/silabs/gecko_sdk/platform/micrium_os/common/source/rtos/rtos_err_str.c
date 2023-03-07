/***************************************************************************//**
 * @file
 * @brief Common - Error Strings
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
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <common/include/rtos_path.h>
#include  <rtos_err_cfg.h>

#include  <common/include/lib_def.h>

#if (RTOS_ERR_CFG_STR_EN == DEF_ENABLED)                        // Do not compile if error strings are turned off.

#include  <cpu/include/cpu.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

#define  RTOS_ERR_STR_MODULE                                    // This define must be present when including rtos_err.h

#undef   RTOS_ERR_DECLARE
#include  <common/source/preprocessor/preprocessor_priv.h>
#define  RTOS_ERR_DECLARE(err, str)  PP_STRINGIFY(err),

const CPU_CHAR *  const RTOS_ErrStrTbl[] = {
#include  <common/include/rtos_err.h>
};

#undef   RTOS_ERR_DECLARE
#include  <common/source/preprocessor/preprocessor_priv.h>
#define  RTOS_ERR_DECLARE(err, str)  str,

const CPU_CHAR *  const RTOS_ErrDescStrTbl[] = {
#include  <common/include/rtos_err.h>
};

#endif
