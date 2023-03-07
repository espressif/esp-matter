/***************************************************************************//**
 * @file
 * @brief File System - Core Posix Operations
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.
 * The software is governed by the sections of the MSLA applicable to Micrium
 * Software.
 *
 ******************************************************************************/

/********************************************************************************************************
 * @note             (1) The Institute of Electrical and Electronics Engineers and The Open Group, have given
 *                       us permission to reprint portions of their documentation.  Portions of this text are
 *                       reprinted and reproduced in electronic form from the IEEE Std 1003.1, 2004 Edition,
 *                       Standard for Information Technology -- Portable Operating System Interface (POSIX),
 *                       The Open Group Base Specifications Issue 6, Copyright (C) 2001-2004 by the Institute
 *                       of Electrical and Electronics Engineers, Inc and The Open Group.  In the event of any
 *                       discrepancy between these versions and the original IEEE and The Open Group Standard,
 *                       the original IEEE and The Open Group Standard is the referee document.  The original
 *                       Standard can be obtained online at http://www.opengroup.org/unix/online.html.
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  FS_CORE_POSIX_PRIV_
#define  FS_CORE_POSIX_PRIV_

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------------------ FS ------------------------
#include  <fs_core_cfg.h>

//                                                                 ----------------------- EXT ------------------------

#include  <common/include/rtos_err.h>
#include  <cpu/include/cpu.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void FSPosix_Init(RTOS_ERR *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                       CONFIGURATION ERRORS
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  FS_CORE_CFG_POSIX_EN
#error  "FS_CORE_CFG_POSIX_EN not #define'd in 'fs_core_cfg.h. MUST be DEF_ENABLED or DEF_DISABLED."
#elif  ((FS_CORE_CFG_POSIX_EN != DEF_DISABLED) \
  && (FS_CORE_CFG_POSIX_EN != DEF_ENABLED))
#error  "FS_CORE_CFG_POSIX_EN illegally #define'd in 'fs_core_cfg.h'. MUST be DEF_ENABLED or DEF_DISABLED."
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
