/***************************************************************************//**
 * @file
 * @brief USB Host Function Operations
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

#ifndef  _USBH_CORE_FNCT_H_
#define  _USBH_CORE_FNCT_H_

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
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

CPU_INT08U USBH_FnctIF_QtyGet(USBH_DEV_HANDLE  dev_handle,
                              USBH_FNCT_HANDLE fnct_handle,
                              RTOS_ERR         *p_err);

CPU_INT08U USBH_FnctClassGet(USBH_DEV_HANDLE  dev_handle,
                             USBH_FNCT_HANDLE fnct_handle,
                             RTOS_ERR         *p_err);

#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_FNCT_SUBCLASS) == DEF_YES)
CPU_INT08U USBH_FnctSubclassGet(USBH_DEV_HANDLE  dev_handle,
                                USBH_FNCT_HANDLE fnct_handle,
                                RTOS_ERR         *p_err);
#endif

#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_FNCT_PROTOCOL) == DEF_YES)
CPU_INT08U USBH_FnctProtocolGet(USBH_DEV_HANDLE  dev_handle,
                                USBH_FNCT_HANDLE fnct_handle,
                                RTOS_ERR         *p_err);
#endif

#if (USBH_CFG_STR_EN == DEF_ENABLED)
CPU_INT08U USBH_FnctStrGet(USBH_DEV_HANDLE  dev_handle,
                           USBH_FNCT_HANDLE fnct_handle,
                           CPU_CHAR         *p_str_buf,
                           CPU_INT08U       str_buf_len,
                           RTOS_ERR         *p_err);
#endif

#ifdef __cplusplus
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           CONFIGURATION ERRORS
 ********************************************************************************************************
 *******************************************************************************************************/

#if ((USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_FNCT_SUBCLASS) == DEF_YES) \
  && (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_DEV_SUBCLASS) != DEF_YES))
#error  "USBH_CFG_FIELD_EN_MASK   illegally #define'd in 'usbh_cfg.h', MUST enable USBH_CFG_FIELD_EN_DEV_SUBCLASS when USBH_CFG_FIELD_EN_FNCT_SUBCLASS is enabled"
#endif

#if ((USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_FNCT_PROTOCOL) == DEF_YES) \
  && (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_DEV_PROTOCOL) != DEF_YES))
#error  "USBH_CFG_FIELD_EN_MASK   illegally #define'd in 'usbh_cfg.h', MUST enable USBH_CFG_FIELD_EN_DEV_PROTOCOL when USBH_CFG_FIELD_EN_FNCT_PROTOCOL is enabled"
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                          MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
