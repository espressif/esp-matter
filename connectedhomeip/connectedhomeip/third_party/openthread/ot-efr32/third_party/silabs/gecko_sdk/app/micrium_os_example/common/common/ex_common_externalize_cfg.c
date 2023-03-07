/***************************************************************************//**
 * @file
 * @brief Common Init Example
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
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>
#include  <cpu/include/cpu.h>
#include  <common/include/common.h>
#include  <common/include/auth.h>
#include  <common/include/rtos_err.h>

#include  <rtos_cfg.h>

#ifdef  RTOS_MODULE_COMMON_SHELL_AVAIL
#include  <common/include/shell.h>
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_ENABLED)

#if (RTOS_CFG_LOG_EN == DEF_ENABLED)
const COMMON_INIT_CFG Common_InitCfg =
{
  .CommonMemSegPtr = DEF_NULL,
  .LoggingCfg =
  {
    .AsyncBufSize = 512u
  },
  .LoggingMemSegPtr = DEF_NULL
};
#else
const COMMON_INIT_CFG Common_InitCfg =
{
  .CommonMemSegPtr = DEF_NULL,
};
#endif

#ifdef  RTOS_MODULE_COMMON_SHELL_AVAIL
const SHELL_INIT_CFG Shell_InitCfg =
{
  .CfgCmdUsage =
  {
    .CmdTblItemNbrInit = 10u,
    .CmdTblItemNbrMax = 10u,
    .CmdArgNbrMax = 10u,
    .CmdNameLenMax = 10u
  },
  .MemSegPtr = DEF_NULL
};
#endif

const AUTH_INIT_CFG Auth_InitCfg =
{
  .RootUserCfg =
  {
    .RootUserName = "admin",
    .RootUserPwd = "admin"
  },
  .ResourceCfg =
  {
    .NbUserMax = 4u,
    .NameLenMax = 10u,
    .PwdLenMax = 10u
  },
  .MemSegPtr = DEF_NULL
};

#endif // RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN
