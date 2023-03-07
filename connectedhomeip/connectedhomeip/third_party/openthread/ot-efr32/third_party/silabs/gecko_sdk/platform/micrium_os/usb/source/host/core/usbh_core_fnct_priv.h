/***************************************************************************//**
 * @file
 * @brief USB Host Function Declarations
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

#ifndef  _USBH_CORE_FNCT_PRIV_H_
#define  _USBH_CORE_FNCT_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>

#include  <common/include/rtos_err.h>

#include  <usb/source/host/core/usbh_core_types_priv.h>

#include  <usb/include/host/usbh_core_handle.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void USBH_FnctInit(CPU_INT16U fnct_qty,
                   RTOS_ERR   *p_err);

USBH_FNCT *USBH_FnctCreate(CPU_INT08U *p_desc_buf,
                           RTOS_ERR   *p_err);

USBH_FNCT *USBH_FnctDevCreate(USBH_DEV *p_dev,
                              RTOS_ERR *p_err);

void USBH_FnctDel(USBH_FNCT *p_fnct);

void USBH_FnctIF_Add(USBH_FNCT *p_fnct,
                     USBH_IF   *p_if,
                     RTOS_ERR  *p_err);

USBH_FNCT *USBH_FnctPtrGet(USBH_CONFIG      *p_config,
                           USBH_FNCT_HANDLE fnct_handle,
                           RTOS_ERR         *p_err);

void USBH_FnctEP_Open(USBH_DEV  *p_dev,
                      USBH_FNCT *p_fnct,
                      RTOS_ERR  *p_err);

void USBH_FnctEP_Close(USBH_DEV  *p_dev,
                       USBH_FNCT *p_fnct,
                       RTOS_ERR  *p_err);

void USBH_FnctConnFailNotify(USBH_FNCT_HANDLE fnct_handle,
                             RTOS_ERR         err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
