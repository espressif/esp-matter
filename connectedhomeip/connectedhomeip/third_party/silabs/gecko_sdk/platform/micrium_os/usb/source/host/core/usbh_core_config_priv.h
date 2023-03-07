/***************************************************************************//**
 * @file
 * @brief USB Host Core Configuration Declarations
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
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _USBH_CORE_CONFIG_PRIV_H_
#define  _USBH_CORE_CONFIG_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>

#include  <common/include/rtos_err.h>
#include  <common/include/rtos_path.h>
#include  <usbh_cfg.h>

#include  <usb/source/host/core/usbh_core_types_priv.h>
#include  <usb/source/host/core/usbh_core_dev_priv.h>

#include  <usb/include/host/usbh_core.h>
#include  <usb/include/host/usbh_core_handle.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void USBH_ConfigOpen(USBH_DEV   *p_dev,
                     CPU_INT08U cfg_nbr,
                     RTOS_ERR   *p_err);

void USBH_ConfigClose(USBH_DEV *p_dev);

void USBH_ConfigSet(USBH_DEV   *p_dev,
                    CPU_INT08U cfg_nbr,
                    RTOS_ERR   *p_err);

CPU_BOOLEAN USBH_ConfigAcceptNotify(void);

void USBH_ConfigSetNotify(USBH_DEV_HANDLE dev_handle,
                          CPU_INT08U      cfg_nbr,
                          RTOS_ERR        err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
