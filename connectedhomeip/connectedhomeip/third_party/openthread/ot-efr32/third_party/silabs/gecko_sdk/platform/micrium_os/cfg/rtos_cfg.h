/***************************************************************************//**
 * @file
 * @brief RTOS Configuration - Configuration Template File
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

#ifndef  _RTOS_CFG_H_
#define  _RTOS_CFG_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                             INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <common/include/rtos_opt_def.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/*
 ********************************************************************************************************
 *                                         ASSERTS CONFIGURATION
 ********************************************************************************************************
 */

// <h>Asserts Configuration

// <q RTOS_CFG_ASSERT_DBG_ARG_CHK_EXT_APP_EN> Debug assert on argument checking in application
// <i> When enabled, any invocation of macro APP_RTOS_ASSERT_DBG() from the application will cause a END_CALL on failed assertion.
// <i> It is recommended to disable this feature in 'release' code.
// <i> Default: 1
#define  RTOS_CFG_ASSERT_DBG_ARG_CHK_EXT_APP_EN                  1

// <q RTOS_CFG_ASSERT_DBG_ARG_CHK_EXT_BSP_EN> Debug assert on argument checking in BSP
// <i> When enabled, function arguments will be validated in the BSP and any incorrect argument will cause a END_CALL.
// <i> It is recommended to disable this feature in 'release' code.
// <i> Default: 1
#define  RTOS_CFG_ASSERT_DBG_ARG_CHK_EXT_BSP_EN                  1

// <q RTOS_CFG_ASSERT_DBG_ARG_CHK_EXT_CAN_EN> Debug assert on argument checking in CAN
// <i> When enabled, function arguments will be validated in CAN and any incorrect argument will cause a END_CALL.
// <i> It is recommended to disable this feature in 'release' code.
// <i> Default: 1
#define  RTOS_CFG_ASSERT_DBG_ARG_CHK_EXT_CAN_EN                  1

// <q RTOS_CFG_ASSERT_DBG_ARG_CHK_EXT_COMMON_EN> Debug assert on argument checking in Common
// <i> When enabled, function arguments will be validated in Common and any incorrect argument will cause a END_CALL.
// <i> It is recommended to disable this feature in 'release' code.
// <i> Default: 1
#define  RTOS_CFG_ASSERT_DBG_ARG_CHK_EXT_COMMON_EN               1

// <q RTOS_CFG_ASSERT_DBG_ARG_CHK_EXT_CPU_EN> Debug assert on argument checking in CPU
// <i> When enabled, function arguments will be validated in CPU and any incorrect argument will cause a END_CALL.
// <i> It is recommended to disable this feature in 'release' code.
// <i> Default: 1
#define  RTOS_CFG_ASSERT_DBG_ARG_CHK_EXT_CPU_EN                  1

// <q RTOS_CFG_ASSERT_DBG_ARG_CHK_EXT_FS_EN> Debug assert on argument checking in the File System
// <i> When enabled, function arguments will be validated in the File System and any incorrect argument will cause a END_CALL.
// <i> It is recommended to disable this feature in 'release' code.
// <i> Default: 1
#define  RTOS_CFG_ASSERT_DBG_ARG_CHK_EXT_FS_EN                   1

// <q RTOS_CFG_ASSERT_DBG_ARG_CHK_EXT_KERNEL_EN> Debug assert on argument checking in the Kernel
// <i> When enabled, function arguments will be validated in the Kernel and any incorrect argument will cause a END_CALL.
// <i> It is recommended to disable this feature in 'release' code.
// <i> Default: 1
#define  RTOS_CFG_ASSERT_DBG_ARG_CHK_EXT_KERNEL_EN               1

// <q RTOS_CFG_ASSERT_DBG_ARG_CHK_EXT_NET_EN> Debug assert on argument checking in the Network module
// <i> When enabled, function arguments will be validated in the Network module and any incorrect argument will cause a END_CALL.
// <i> It is recommended to disable this feature in 'release' code.
// <i> Default: 1
#define  RTOS_CFG_ASSERT_DBG_ARG_CHK_EXT_NET_EN                  1

// <q RTOS_CFG_ASSERT_DBG_ARG_CHK_EXT_NET_APP_EN> Debug assert on argument checking in the Network Applications
// <i> When enabled, function arguments will be validated in the Network Applications and any incorrect argument will cause a END_CALL.
// <i> It is recommended to disable this feature in 'release' code.
// <i> Default: 1
#define  RTOS_CFG_ASSERT_DBG_ARG_CHK_EXT_NET_APP_EN              1

// <q RTOS_CFG_ASSERT_DBG_ARG_CHK_EXT_USBD_EN> Debug assert on argument checking in the USB Device module
// <i> When enabled, function arguments will be validated in the USB Device module and any incorrect argument will cause a END_CALL.
// <i> It is recommended to disable this feature in 'release' code.
// <i> Default: 1
#define  RTOS_CFG_ASSERT_DBG_ARG_CHK_EXT_USBD_EN                 1

// <q RTOS_CFG_ASSERT_DBG_ARG_CHK_EXT_USBH_EN> Debug assert on argument checking in the USB Host module
// <i> When enabled, function arguments will be validated in the USB Host module and any incorrect argument will cause a END_CALL.
// <i> It is recommended to disable this feature in 'release' code.
// <i> Default: 1
#define  RTOS_CFG_ASSERT_DBG_ARG_CHK_EXT_USBH_EN                 1

// <q RTOS_CFG_ASSERT_DBG_ARG_CHK_EXT_IO_EN> Debug assert on argument checking in the IO module
// <i> When enabled, function arguments will be validated in the IO module and any incorrect argument will cause a END_CALL.
// <i> It is recommended to disable this feature in 'release' code.
// <i> Default: 1
#define  RTOS_CFG_ASSERT_DBG_ARG_CHK_EXT_IO_EN                   1

// <q RTOS_CFG_ASSERT_DBG_ARG_CHK_EXT_PROBE_EN> Debug assert on argument checking in the Probe drivers
// <i> When enabled, function arguments will be validated in the probe drivers and any incorrect argument will cause a END_CALL.
// <i> It is recommended to disable this feature in 'release' code.
// <i> Default: 1
#define  RTOS_CFG_ASSERT_DBG_ARG_CHK_EXT_PROBE_EN                1

// <o RTOS_CFG_RTOS_ASSERT_DBG_FAILED_END_CALL_SEL> END_CALL action on failed debug assertions.
//   <RTOS_ASSERT_END_CALL_SEL_TRAP=> Trap
//   <RTOS_ASSERT_END_CALL_SEL_RETURN=> Return
//   <RTOS_ASSERT_END_CALL_SEL_CUSTOM=> Custom
// <i> When Trap is selected, the system will hang where the error occured in an endless loop.
// <i> When Return is selected, the system will return immediately from the function and report the proper error.
// <i> When Custom is selected, the system will invoke the macro RTOS_CFG_RTOS_ASSERT_DBG_FAILED_END_CALL(ret_val).
// <i> It is your responsibility to define this macro from the compiler defines list when Custom is selected.
// <i> Default: RTOS_ASSERT_END_CALL_SEL_TRAP
#define  RTOS_CFG_RTOS_ASSERT_DBG_FAILED_END_CALL_SEL           RTOS_ASSERT_END_CALL_SEL_TRAP

// <o RTOS_CFG_RTOS_ASSERT_CRITICAL_FAILED_END_CALL_SEL> END_CALL action on failed critical assertions.
//   <RTOS_ASSERT_END_CALL_SEL_TRAP=> Trap
//   <RTOS_ASSERT_END_CALL_SEL_RETURN=> Return
//   <RTOS_ASSERT_END_CALL_SEL_CUSTOM=> Custom
// <i> When Trap is selected, the system will hang where the error occured in an endless loop.
// <i> When Return is selected, the system will return immediately from the function and report the proper error.
// <i> When Custom is selected, the system will invoke the macro RTOS_CFG_RTOS_ASSERT_CRITICAL_FAILED_END_CALL(ret_val).
// <i> It is your responsibility to define this macro from the compiler defines list when Custom is selected.
// <i> Default: RTOS_ASSERT_END_CALL_SEL_TRAP
#define  RTOS_CFG_RTOS_ASSERT_CRITICAL_FAILED_END_CALL_SEL      RTOS_ASSERT_END_CALL_SEL_TRAP

// <q RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN> Externalize module configurations via const value instead of using Configure functions
// <i> Determines whether the configurations can be provided via optional 'Configure' functions calls to
// <i> the modules or if these configurations are assumed extern by the modules and must be provided by
// <i> the application. This option allows to reduce the memory and code space usage.
// <i> Default: 0
#define  RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN               0

// </h>

/*
 ********************************************************************************************************
 *                                         LOGGING CONFIGURATION
 ********************************************************************************************************
 */

// <h>Logging Configuration

// <q RTOS_CFG_LOG_EN> Enable logging module
// <i> When enabled, all logging message will output in a default channel that logs ALL level of messages
// <i> synchronously and uses the function "putchar()" to print the messages.
// <i> It is possible to define other channels, or re-define the default channel by re-defining "RTOS_CFG_LOG_ALL".
// <i> Refer to the logging configuration section of the user manual for more information.
// <i> Default: 0
#define  RTOS_CFG_LOG_EN                                    0

// </h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                             MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // End of rtos_cfg.h module include.

// <<< end of configuration section >>>
