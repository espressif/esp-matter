/***************************************************************************//**
 * @file
 * @brief USB Host Endpoint Declarations
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

#ifndef  _USBH_CORE_EP_PRIV_H_
#define  _USBH_CORE_EP_PRIV_H_

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
#include  <usb/source/host/core/usbh_core_ep_priv.h>

#include  <usb/include/host/usbh_core.h>
#include  <usb/include/host/usbh_core_handle.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void USBH_EP_Init(CPU_INT16U ep_qty,
                  CPU_INT16U ep_open_qty,
                  CPU_INT16U urb_qty,
                  RTOS_ERR   *p_err);

USBH_EP_HANDLE USBH_EP_Open(USBH_DEV   *p_dev,
                            USBH_FNCT  *p_fnct,
                            USBH_EP    *p_ep,
                            CPU_INT08U if_ix,
                            RTOS_ERR   *p_err);

void USBH_EP_Close(USBH_DEV       *p_dev,
                   USBH_EP_HANDLE ep_handle,
                   RTOS_ERR       *p_err);

void USBH_EP_CtrlReset(USBH_DEV *p_dev,
                       RTOS_ERR *p_err);

USBH_EP *USBH_EP_Create(CPU_INT08U *p_desc_buf,
                        RTOS_ERR   *p_err);

void USBH_EP_Del(USBH_EP *p_ep);

CPU_INT16U USBH_EP_CtrlXferLocked(USBH_DEV   *p_dev,
                                  CPU_INT08U req,
                                  CPU_INT08U req_type,
                                  CPU_INT16U val,
                                  CPU_INT16U ix,
                                  CPU_INT08U *p_buf,
                                  CPU_INT16U len,
                                  CPU_INT16U buf_len,
                                  CPU_INT32U timeout,
                                  RTOS_ERR   *p_err);

void USBH_EP_AbortLocked(USBH_DEV       *p_dev,
                         USBH_EP_HANDLE ep_handle,
                         RTOS_ERR       *p_err);

void USBH_EP_SuspendLocked(USBH_DEV    *p_dev,
                           CPU_BOOLEAN suspended);

void USBH_EP_URB_ListAbortedFree(USBH_DEV_HANDLE dev_handle,
                                 USBH_EP_HANDLE  ep_handle,
                                 USBH_URB_MGMT   *p_urb_mgmt_aborted_head);

void USBH_URB_Complete(USBH_DEV_HANDLE dev_handle,
                       USBH_EP_HANDLE  ep_handle);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
