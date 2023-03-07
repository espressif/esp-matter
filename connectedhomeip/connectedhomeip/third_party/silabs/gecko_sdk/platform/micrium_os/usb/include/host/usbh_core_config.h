/***************************************************************************//**
 * @file
 * @brief USB Host Configuration Operations
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

/****************************************************************************************************//**
 * @addtogroup USBH_CORE
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _USBH_CORE_CONFIG_H_
#define  _USBH_CORE_CONFIG_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>

#include  <common/include/rtos_err.h>
#include  <common/include/rtos_path.h>
#include  <usbh_cfg.h>

#include  <usb/include/host/usbh_core_utils.h>
#include  <usb/include/host/usbh_core_opt_def.h>
#include  <usb/include/host/usbh_core_handle.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       CONFIGURATION ATTRIBUTES
 *
 * Note(s) : (1) The bmAttributes field from the configuration descriptor is organized the following way:
 *
 *               +---------+------------------------------------------+
 *               | BITS    | Description                              |
 *               +---------+------------------------------------------+
 *               | 0..4    | Reserved.                                |
 *               +---------+------------------------------------------+
 *               | 5       | Remote wakeup.                           |
 *               +---------+------------------------------------------+
 *               | 6       | Self-powered.                            |
 *               +---------+------------------------------------------+
 *               | 7       | Reserved (USB 1.0 bus powered).          |
 *               +---------+------------------------------------------+
 *
 *               (a) See 'Universal Serial Bus Specification Revision 2.0', Section 9.6.3, Table 9-10
 *******************************************************************************************************/

#define  USBH_CONFIG_ATTR_REMOTE_WAKEUP                 DEF_BIT_05
#define  USBH_CONFIG_ATTR_SELF_POWERED                  DEF_BIT_06

/********************************************************************************************************
 *                                           DESCRIPTOR TYPES
 *
 * Note(s) : (1) See 'Universal Serial Bus Specification Revision 2.0', Section 9.4, Table 9-5, and
 *               Section 9.4.3.
 *
 *           (2) For a 'get descriptor' setup request, the low byte of the 'wValue' field may contain
 *               one of these values.
 *******************************************************************************************************/

#define  USBH_DESC_TYPE_CONFIG                             2u

#define  USBH_DESC_LEN_CONFIG                              9u

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

CPU_INT08U USBH_ConfigFnctQtyGet(USBH_DEV_HANDLE dev_handle,
                                 RTOS_ERR        *p_err);

#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_CONFIG_ATTR) == DEF_YES)
CPU_INT08U USBH_ConfigAttribGet(USBH_DEV_HANDLE dev_handle,
                                RTOS_ERR        *p_err);
#endif

#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_CONFIG_MAX_PWR) == DEF_YES)
CPU_INT08U USBH_ConfigMaxPwrGet(USBH_DEV_HANDLE dev_handle,
                                RTOS_ERR        *p_err);
#endif

#if (USBH_CFG_STR_EN == DEF_ENABLED)
CPU_INT08U USBH_ConfigStrGet(USBH_DEV_HANDLE dev_handle,
                             CPU_CHAR        *p_str_buf,
                             CPU_INT08U      str_buf_len,
                             RTOS_ERR        *p_err);
#endif

#ifdef __cplusplus
}
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                            MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
