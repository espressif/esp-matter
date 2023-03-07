/***************************************************************************//**
 * @file
 * @brief RTOS_ERR Configuration - Configuration Template File
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

// <<< Use Configuration Wizard in Context Menu >>>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _RTOS_ERR_CFG_H_
#define  _RTOS_ERR_CFG_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                          INCLUDE FILES NOTE
 *
 * Note(s) : (1) No files including rtos_err.h must be included by this file. This could lead to circular
 *               inclusion problems.
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

// <h>RTOS Error Configuration

// <q RTOS_ERR_CFG_EXT_EN> Extended error type
// <i> Extend RTOS_ERR type to contain more debug information. Extended information provides a string containing
// <i> the file name and line number where the error has been set. It will also contain the function name, if compiling in C99.
// <i> Enabling this configuration may have an impact on performance and resource usage.
// <i> It is recommended to disable this configuration in release code.
// <i> Default: 1
#define  RTOS_ERR_CFG_EXT_EN                                1

// <q RTOS_ERR_CFG_STR_EN> Add string description to errors
// <i> Add strings associated with each error code, in order to print them.
// <i> If disabled, the error code enum value will be outputted instead.
// <i> For example, if set to 1, it would be possible to print RTOS_ERR_NONE or RTOS_ERR_INVALID_ARG as a string
// <i> instead of printing the associated numerical value.
// <i> Default: 0
#define  RTOS_ERR_CFG_STR_EN                                0

// </h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                             MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // End of rtos_err_cfg.h module include.

// <<< end of configuration section >>>
